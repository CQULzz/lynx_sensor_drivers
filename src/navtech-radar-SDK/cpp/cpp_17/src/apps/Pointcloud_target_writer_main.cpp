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
#include "sdk.h"
#include "Option_parser.h"
#include "Time_utils.h"
#include "Endpoint.h"
#include "Log.h"
#include "Signal_handler.h"

#include "Colossus_TCP_client.h"
#include "File_writer.h"
#include "Protobuf_helpers.h"
#include "configurationdata.pb.h"

#include "CFAR_Peak_finder.h"

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Monotonic;
using namespace Navtech::Networking;
using namespace Navtech::Networking::Colossus_protocol;

using Navtech::Utility::Log;
using Navtech::Utility::syslog;

using Navtech::Utility::Option_parser;
using Navtech::Utility::Option;

using Navtech::Networking::IP_address;
using Navtech::Networking::Port;
using Navtech::Networking::Endpoint;

using Navtech::Utility::File_writer;
using Navtech::Utility::Signal_handler;


// ---------------------------------------------------------------------------------------------------------------------
//
Option_parser options {
    {
        Option { "--help",          "-h", "Show the help message",          optional, no_argument, },
        Option { "--ipaddress",     "-i", "TCP address of radar",           optional, has_argument, "127.0.0.1" },
        Option { "--port",          "-p", "TCP port of radar",              optional, has_argument, "6317" },
        Option { "--rotations",     "-r", "Rotations to record",            optional, has_argument, "5"},
        Option { "--filetag",       "-f", "Tag for output file",            optional, has_argument, "targets" },
        Option { "--windowsize",    "-w", "Samples taken about a bin",      optional, has_argument },
        Option { "--threshold",     "-t", "Threshold above local average",  optional, has_argument },
        Option { "--minbin",        "-b", "CFAR Minimum bin",               optional, has_argument },
        Option { "--maxpeaks",      "-m", "CFAR maximum peaks",             optional, has_argument },
        Option { 
            "--mode", "-s",
            "Subresolution mode: [0: curve fitting, 1: 1D centre-of-mass, 2: 2D centre of mass]",
            optional, has_argument, "0"
        }
    }
};


// ---------------------------------------------------------------------------------------------------------------------
// Signal handling: If SIGINT or SIGTERM are sent to the 
// program, stop processing.
//
volatile bool running { true };

void stop_running(std::int32_t signal [[maybe_unused]], std::int32_t info [[maybe_unused]])
{
    running = false;
}


// ---------------------------------------------------------------------------------------------------------------------
//
class Target_writer : public File_writer<Navtech::Navigation::CFAR_Target> {
public:
    using File_writer<Navtech::Navigation::CFAR_Target>::File_writer;

protected:
    void header() override;
    void on_write(const Navtech::Navigation::CFAR_Target& target) override;
};


void Target_writer::header()
{
    output_file << "Bearing [deg], Range [m]" << std::endl;
}


void Target_writer::on_write(const Navtech::Navigation::CFAR_Target& target) 
{
    output_file << target.bearing << ",";
    output_file << target.range << std::endl;
}


// ---------------------------------------------------------------------------------------------------------------------
//
class Pointcloud_target_writer : public Active {
public:
    Pointcloud_target_writer(
        const Endpoint& server_endpoint,
        std::string_view filename,
        std::uint16_t rotations,
        Navtech::Navigation::Subresolution_mode mode
    );

    void set_config_from_args(Unit::Bin window_sz, Unit::Bin min_bin, Unit::dB nav_threshold, Unit::Bin max_peaks);
    
    std::uint16_t completed_rotations() const { return rotations_completed; }

protected:
    void on_start() override;
    void on_stop()  override;

private:
    TCP::Client client;
    TCP::Configuration      radar_cfg         { };
    bool                    has_user_cfg      { false };
    float                   steps_per_azimuth { };

    // Colossus message handlers
    //
    void process_config(const TCP::Message& msg);
    void process_nav_config(const TCP::Message& msg);
    void process_Fft(const TCP::Message& msg);
    void on_process_Fft(const TCP::Message& msg);

    // File writing
    //
    Target_writer  target_writer { };

    // CFAR processing
    //
    Unit::Bin                               min_bin             { };
    Unit::Bin                               max_peaks           { };
    Unit::Bin                               window_sz           { };
    Unit::dB                                threshold           { };
    std::uint16_t                           max_rotations       { };
    Navtech::Navigation::Subresolution_mode mode                { };
    Navtech::Navigation::CFAR_Peak_finder   peak_finder         { };
    std::uint16_t                           rotations_completed { 0 };
    std::uint16_t                           last_azimuth        { 0 };
    std::uint32_t                           seen_azimuths       { 0 };
    bool                                    pk_fndr_is_cfgd     { false };

    CA_CFAR::Window                         cfar_window        {  };

    void configure_peak_finder(
        const TCP::Configuration &cfg,
        Unit::Bin minimum_bin,
        Unit::Bin max_peaks,
        std::function<void(const Navtech::Navigation::CFAR_Target&)> callback,
        Navtech::Navigation::Subresolution_mode& mode
    );
};


Pointcloud_target_writer::Pointcloud_target_writer(
    const Endpoint& server_endpoint,
    std::string_view filename,
    std::uint16_t rotations,
    Navtech::Navigation::Subresolution_mode mode
) :
    client          { server_endpoint },
    target_writer   { filename, std::ios_base::out },
    max_rotations   { rotations },
    mode            { mode }
{
}


void Pointcloud_target_writer::set_config_from_args(
    Unit::Bin   win_sz,
    Unit::Bin   minimum_bin,
    Unit::dB    nav_threshold,
    Unit::Bin   max_pks
)
{
    window_sz   = win_sz;
    min_bin     = minimum_bin;
    threshold   = nav_threshold;
    max_peaks   = max_peaks;

    has_user_cfg = true;
}


void Pointcloud_target_writer::on_start()
{
    using namespace TCP;
    // -----------------------------------------------------------------
    // Message handlers
    // 
    client.set_handler(
        Type::configuration,
        [this](Client&, Message& msg) { process_config(msg); }
    );

    client.set_handler(
        Type::navigation_configuration,
        [this](Client&, Message& msg) { process_nav_config(msg); }
    );

    client.set_handler(
        Type::fft_data,
        [this](Client&, Message& msg) { process_Fft(msg); }
    );
   
    client.start();
}


void Pointcloud_target_writer::on_stop()
{
    syslog.debug("Pointcloud writer stopping...");

    target_writer.stop();
    peak_finder.stop();
    client.stop();
}


void Pointcloud_target_writer::process_config(const TCP::Message& msg)
{
    using namespace TCP;

    syslog.debug("Received radar configuration");

    auto cfg = msg.view_as<Configuration>();
    radar_cfg.azimuth_samples(cfg->azimuth_samples());
    radar_cfg.encoder_size(cfg->encoder_size());
    radar_cfg.bin_size(cfg->bin_size());
    radar_cfg.range_gain(cfg->range_gain());
    radar_cfg.range_in_bins(cfg->range_in_bins());
    radar_cfg.range_offset(cfg->range_offset());

    steps_per_azimuth = static_cast<float>(cfg->encoder_size() / cfg->azimuth_samples());

    if (has_user_cfg) {
        configure_peak_finder(
            radar_cfg,
            min_bin,
            max_peaks,
            [this] (const Navigation::CFAR_Target& t) { target_writer.write(t); },
            mode
        );
    }
    
}


void Pointcloud_target_writer::process_nav_config(const TCP::Message& msg)
{
    syslog.debug("Navigation configuration received");
    
    if (has_user_cfg) {
        // prefer the user config
        //
        syslog.write("A user configuration already exists; ignoring incoming configuration");
        return;
    }

    auto nav_cfg_msg = msg.view_as<TCP::Navigation_config>();

    cfar_window.size            = nav_cfg_msg->bins_to_operate_on();
    cfar_window.guard_cells     = 2;
    cfar_window.threshold_delta = nav_cfg_msg->navigation_threshold();
    min_bin                     = nav_cfg_msg->min_bin_to_operate_on();

    configure_peak_finder(
        radar_cfg,
        nav_cfg_msg->min_bin_to_operate_on(),
        nav_cfg_msg->max_peaks_per_azimuth(),
        [this] (const Navigation::CFAR_Target& t) { target_writer.write(t); },
        mode
    );
}


void Pointcloud_target_writer::configure_peak_finder(
    const TCP::Configuration&                                       cfg,
    Unit::Azimuth                                                   start_bin,
    Unit::Azimuth                                                   max_peaks,
    std::function<void(const Navtech::Navigation::CFAR_Target&)>    callback,
    Navtech::Navigation::Subresolution_mode&                        mode
)
{
    min_bin = start_bin;

    peak_finder.configure(
        cfg,
        min_bin,
        max_peaks,
        mode
    );
    
    peak_finder.set_target_callback(callback);
    pk_fndr_is_cfgd = true;
    target_writer.start();
    peak_finder.start();
    client.send(TCP::Type::start_fft_data);
}


void Pointcloud_target_writer::process_Fft(const TCP::Message& msg)
{
    async_call(&Pointcloud_target_writer::on_process_Fft, this, msg);
}


void Pointcloud_target_writer::on_process_Fft(const TCP::Message& msg)
{

    if (!pk_fndr_is_cfgd) return;

    auto fft     = msg.view_as<TCP::FFT_data>();
    auto azimuth = static_cast<Unit::Azimuth>(fft->azimuth() / steps_per_azimuth);

    if (rotations_completed >= max_rotations) return;
    
    auto fft_data = fft->to_vector();

    peak_finder.find_peaks(
        azimuth,
        CA_CFAR::process(
            fft_data.begin() + min_bin, 
            fft_data.end(),
            cfar_window
        )
    );

    seen_azimuths++;
    
    if (azimuth < last_azimuth) {
        ++rotations_completed;
        syslog.write("Saw [" + std::to_string(seen_azimuths) + "] azimuths this rotation.");
        seen_azimuths = 0;
    }

    last_azimuth = azimuth;
}
// ---------------------------------------------------------------------------------------------------------------------
//
int main(int argc, char* argv[])
{
    SDK::initialise();

    // Set up signal handling for ctrl-c (SIGINT)
    // and kill (SIGTERM)
    // 
    Signal_handler signal_handler { };
    signal_handler.register_handler(SIGINT, stop_running);
    signal_handler.register_handler(SIGTERM, stop_running);

    options.parse(argc, argv);

    if (options.global("-h")) {
        syslog.write(options.usage());
        exit(0);
    }

    auto tcp_addr   = options.global("-i").translate_to<IP_address>();
    auto tcp_port   = options.global("-p").to_int<std::uint16_t>();
    auto filetag    = options.global("-f").value();
    auto rotations  = options.global("-r").to_int<std::uint16_t>();

    auto nav_threshold  = options.global("-t").to_float();
    auto window_size    = options.global("-w").to_int<std::uint16_t>();
    auto min_bin        = options.global("-b").to_int<std::uint16_t>();
    auto max_peaks      = options.global("-m").to_int<std::uint16_t>();

    Navtech::Navigation::Subresolution_mode sub_mode { options.global("-s").to_int<std::uint16_t>() };

    auto date_prefix = Real_time::Clock::now().format_as("%Y%m%d_%H%M%S").to_string();
    std::string filename { filetag + "_" + date_prefix + ".csv" };
    
    syslog.write("Output will be written to " + filename);

    Pointcloud_target_writer pointcloud_target_writer {
        { tcp_addr, tcp_port },
        filename,
        rotations,
        sub_mode
    };

    if ((nav_threshold > 0) && window_size && min_bin && max_peaks) {
        syslog.write("Using user-provided config");

        pointcloud_target_writer.set_config_from_args(window_size, min_bin, nav_threshold, max_peaks);
    }
    else if ((nav_threshold > 0) || window_size || min_bin || max_peaks) {
        syslog.error("If setting the navigation configuration using arguments you must set *all* of them");
        SDK::shutdown();
        return 0;
    }

    syslog.write("Starting recording...");

    pointcloud_target_writer.start();

    while (running && pointcloud_target_writer.completed_rotations() < rotations) {
        sleep_for(250_msec);
    }

    syslog.write("Stopping recording...");
    pointcloud_target_writer.stop();

    pointcloud_target_writer.join();
    
    SDK::shutdown();
    syslog.write("Done. Output has been written to " + filename);
}