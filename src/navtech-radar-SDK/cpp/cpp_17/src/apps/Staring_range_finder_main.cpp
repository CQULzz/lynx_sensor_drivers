// ---------------------------------------------------------------------------------------------------------------------
// Copyright 2025 Navtech Radar Limited
// This file is part of IASDK which is released under The MIT License (MIT).
// See file LICENSE.txt in project root or go to https://opensource.org/licenses/MIT
// for full license details.
//
// Disclaimer:
// Navtech Radar is furnishing this item "as is". Navtech Radar does not provide 
// any warranty of the item whatsoever, whether express, implied, or statutory,
// including, but not limited to, any warranty of merchantability or fitness
// for a particular purpose or any warranty that the contents of the item will
// be error-free.
// In no respect shall Navtech Radar incur any liability for any damages, including,
// but limited to, direct, indirect, special, or consequential damages arising
// out of, resulting from, or any way connected to the use of the item, whether
// or not based upon warranty, contract, tort, or otherwise; whether or not
// injury was sustained by persons or property or otherwise; and whether or not
// loss was sustained from, or arose out of, the results of, the item, or any
// services that may be provided by Navtech Radar.
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// This file is a basic app framework which can be used to construct your own
// applications.  It contains code for:
// - Parsing command line options
// - Signal handling
// - Configuring logging
// ---------------------------------------------------------------------------------------------------------------------
#include "sdk.h"
#include "Option_parser.h"
#include "Endpoint.h"
#include "Time_utils.h"
#include "Log.h"
#include "Signal_handler.h"

#include "Colossus_TCP_client.h"
#include "CFAR_Peak_finder.h"
#include "CFAR_algorithms.h"

#include "FFT_Buffer.h"

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Monotonic;

using namespace Navtech::Networking::Colossus_protocol;
using namespace Navtech::Navigation;

using Navtech::Utility::Option_group;
using Navtech::Utility::Option_parser;
using Navtech::Utility::Option;

using Navtech::Networking::IP_address;
using Navtech::Networking::Port;

using Navtech::Utility::Log;
using Navtech::Utility::syslog;

using Navtech::Utility::Signal_handler;


class Staring_range_finder : public Utility::Active {
public:
    Staring_range_finder(
        Navtech::Networking::Endpoint   radar_endpoint,
        Unit::Bin                       min_bin,
        Unit::dB                        threshold,
        Unit::Bin                       window_sz,
        Unit::Bin                       max_peaks,
        Unit::Bin                       guard_cells,
        Unit::Azimuth                   fft_samples,
        bool                            continuous,
        Peak_mode                     return_first
    );

    void cancel();

private:
    Networking::Colossus_protocol::TCP::Client radar_client;
    Unit::Bin       min_bin             { };
    Unit::dB        threshold           { };
    Unit::Bin       window_sz           { };
    Unit::Bin       max_peaks           { };
    Unit::Bin       guard_cells         { };
    Unit::dB        range_gain          { };
    Unit::Metre     range_offset        { };
    Unit::Metre     bin_size            { };
    float           steps_per_azimuth   { 5600.0f / 400.0f };

    CA_CFAR::Window                 cfar_window     { };

    volatile std::atomic_bool       is_done         { false };
    bool                            continuous      { false };
    bool                            rcvd_fst_tgt    { false };
    Peak_mode                       peak_mode       { Peak_mode::max };

    Navigation::CFAR_Peak_finder    peak_finder     { };
    Navigation::FFT_Buffer          fft_buffer;

    void on_start()             override;
    void on_stop()              override;
    Active::Task_state run()    override;

    void process_config(const TCP::Message& msg);
    void process_fft(const TCP::Message& msg);
    void on_process_fft(const TCP::Message& msg);
    void process_target(Navigation::CFAR_Target target);

    void process_azimuths();
    void on_process_azimuths();
};


Staring_range_finder::Staring_range_finder(
    Navtech::Networking::Endpoint   radar_endpoint,
    Unit::Bin                       min_bin,
    Unit::dB                        threshold,
    Unit::Bin                       window_sz,
    Unit::Bin                       max_peaks,
    Unit::Bin                       guard_cells,
    Unit::Azimuth                   fft_samples,
    bool                            continuous,
    Peak_mode                       peak_mode
) : 
    Active              { "Staring Range Finder" },
    radar_client        { radar_endpoint },
    min_bin             { min_bin },
    threshold           { threshold },
    window_sz           { window_sz },
    max_peaks           { max_peaks },
    guard_cells         { guard_cells },
    continuous          { continuous },
    peak_mode           { peak_mode },
    fft_buffer          { Buffer_mode::average, fft_samples }
{
}


void Staring_range_finder::cancel()
{
    is_done = true;
}


void Staring_range_finder::on_start()
{
    radar_client.set_handler(
        TCP::Type::configuration,
        [this] (TCP::Client&, TCP::Message& msg) { process_config(msg); }
    );

    radar_client.ignore(TCP::Type::fft_data);

    radar_client.start();
}


void Staring_range_finder::on_stop()
{
    peak_finder.stop();
    radar_client.stop();
}


Utility::Active::Task_state Staring_range_finder::run()
{
    try_dispatch_async();

    if (!is_done) return Task_state::not_finished;

    radar_client.stop();
    peak_finder.stop();
    peak_finder.join();

    return Task_state::finished;
}


void Staring_range_finder::process_config(const TCP::Message& msg)
{
    auto cfg = msg.view_as<TCP::Configuration>();

    // cell_average = CFAR::Cell_average<float> { min_bin, window_sz, guard_cells, threshold };

    cfar_window.size            = window_sz;
    cfar_window.guard_cells     = guard_cells;
    cfar_window.threshold_delta = threshold;

    steps_per_azimuth = static_cast<float>(cfg->encoder_size() / cfg->azimuth_samples());

    peak_finder.set_target_callback([this] (const Navigation::CFAR_Target& tgt) { process_target(tgt); });

    peak_finder.configure(
        *cfg,
        min_bin,
        max_peaks,
        Navigation::Subresolution_mode::curve_fit,
        peak_mode
    );

    peak_finder.start();

    radar_client.set_handler(
        TCP::Type::fft_data,
        [this](TCP::Client&, TCP::Message& msg) { process_fft(msg); }
    );

    radar_client.send(TCP::Type::start_fft_data);
}


void Staring_range_finder::process_fft(const TCP::Message& msg)
{
    async_call(&Staring_range_finder::on_process_fft, this, msg);
}


void Staring_range_finder::on_process_fft(const TCP::Message& msg)
{
    auto fft_msg = msg.view_as<TCP::FFT_data>();

    auto az_idx        = static_cast<Unit::Azimuth>(fft_msg->azimuth() / steps_per_azimuth);
    auto buffered_data = fft_buffer.process_fft(fft_msg->to_vector());

    if (!buffered_data.has_value()) return;

    // peak_finder.find_peaks(az_idx, cell_average.process(buffered_data.value()));

    peak_finder.find_peaks(
        az_idx, 
        CA_CFAR::process(
            buffered_data->begin() + min_bin, 
            buffered_data->end(), 
            cfar_window
        )
    );
}


void Staring_range_finder::process_target(Navigation::CFAR_Target target)
{
    if (!continuous & rcvd_fst_tgt) return;

    if (target.range < 0.0f || std::isnan(target.range) & !continuous) {
        syslog.error("Failed to find target with current settings.");
    }
    else {
        Log::Stream log_stream { };

        log_stream  << "Target found at ["
                    << std::fixed << std::setprecision(3)
                    << target.range << "] m";

        syslog.write(log_stream);
    }

    rcvd_fst_tgt = true;
    
    if (!continuous) {
        is_done = true;
    }
}


// ---------------------------------------------------------------------------------------------------------------------
//
Option_parser options {
    {   
        Option { "--ipaddress",     "-i", "IP address to connect to",           optional, has_argument, "127.0.0.1" },
        Option { "--port",          "-p", "Port to connect to",                 optional, has_argument, "6317" },
        Option { "--minbin",        "-b", "Minimum bin for CFAR to act upon",   optional, has_argument, "50" },
        Option { "--threshold",     "-t", "CFAR threshold for incoming data",   optional, has_argument, "25" },
        Option { "--window_size",   "-w", "CFAR sliding window size",           optional, has_argument, "15" },
        Option { "--max_peaks",     "-m", "CFAR maximum peaks",                 optional, has_argument, "3"},
        Option { "--guard_cells",   "-g", "The number CFAR guard cells",        optional, has_argument, "2" },
        Option { "--samples",       "-s", "Samples used in the average",        optional, has_argument, "200" },
        Option { 
            "--return_mode",   "-r",
            "Return mode for peaks: [0: max peak, 1: first peak]", 
            optional, has_argument, "0" 
        },
        Option { "--continuous",    "-c", "Make the app run continuously",      optional, no_argument }
    }
};


// ---------------------------------------------------------------------------------------------------------------------
//
int main(int argc, char* argv[])
{
    SDK::initialise();

    // Command line option parsing
    //
    options.parse(argc, argv);
    auto server_addr = options.global("-i").translate_to<IP_address>();
    auto server_port = options.global("-p").to_int<std::uint16_t>();
    auto min_bin     = options.global("-b").to_int<Unit::Bin>();
    auto threshold   = options.global("-t").to_float();
    auto window_sz   = options.global("-w").to_int<Unit::Bin>();
    auto max_peaks   = options.global("-m").to_int<Unit::Bin>();
    auto guard_cells = options.global("-g").to_int<Unit::Bin>();
    auto samples     = options.global("-s").to_int<Unit::Azimuth>();
    auto continuous  = options.global("-c").to_bool();
    auto return_mode = options.global("-r").to_int<std::uint16_t>();

    Staring_range_finder range_finder { 
        { server_addr, server_port },
        min_bin,
        threshold,
        window_sz,
        max_peaks,
        guard_cells, 
        samples,
        continuous,
        static_cast<Peak_mode>(return_mode)
    };


    // Set up signal handling for ctrl-c (SIGINT)
    // and kill (SIGTERM)
    // 
    Signal_handler signal_handler { };
    auto stop_running = [&range_finder] (std::int32_t, std::int32_t) { range_finder.cancel(); };

    signal_handler.register_handler(SIGINT, stop_running);
    signal_handler.register_handler(SIGTERM, stop_running);
    
    range_finder.start();

    syslog.write("Starting...");

    range_finder.join();

    SDK::shutdown();
    syslog.write("Done.");
}