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
#include "Colossus_TCP_client.h"
#include "Log.h"
#include "Time_utils.h"
#include "Colossus_protocol.h"
#include "Protobuf_helpers.h"
#include "Option_parser.h"
#include "health.pb.h"
#include "configurationdata.pb.h"
#include "Statistical_value.h"
#include "Signal_handler.h"
#include "Units.h"
#include "Protobuf_helpers.h"

using Navtech::Utility::Log;
using Navtech::Utility::syslog;

using Navtech::Utility::Option_parser;
using Navtech::Utility::Option;
using Navtech::Utility::Signal_handler;

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Monotonic;

using namespace Navtech::Unit;

using Navtech::Networking::Colossus_protocol::TCP::Client;
using Navtech::Networking::Colossus_protocol::TCP::Message;
using Navtech::Networking::Colossus_protocol::TCP::Type;
using Navtech::Networking::Endpoint;
using Navtech::Networking::IP_address;
using Navtech::Networking::Port;


// ---------------------------------------------------------------------------------------------------------------------
// Signal handling: If SIGINT or SIGTERM are sent to the 
// program, stop processing.
//
volatile bool running { true };

void stop_running(std::int32_t signal [[maybe_unused]], std::int32_t info [[maybe_unused]])
{
    syslog.write("Ctrl-C received. Terminating...");
    running = false;
}

// ---------------------------------------------------------------------------------------------------------------------
// A simple example of a message handler accessing the 
// message header component of a Colossus message.
// (In this example, a Configuration message has both a
// header and protocol buffer payload, but we are ignoring
// the protocol buffer.  See below for an example of 
// processing a protocol buffer message)
//
void process_config(Client& radar_client, Message& msg)
{
    using Navtech::Protobuf::from_vector_into;
    using Core::Configuration::Protobuf::ConfigurationData;
    using namespace Networking::Colossus_protocol::TCP;

    syslog.debug("Handler for configuration messages");

    auto config   = msg.view_as<Configuration>();
    auto protobuf = from_vector_into<ConfigurationData>(config->to_vector());
  
    syslog.write("Azimuth samples [" + std::to_string(config->azimuth_samples()) + "]");
    syslog.write("Bin size        [" + std::to_string(config->bin_size()) + "]");
    syslog.write("Range in bins   [" + std::to_string(config->range_in_bins()) + "]");
    syslog.write("Encoder size    [" + std::to_string(config->encoder_size()) + "]");
    syslog.write("Rotation rate   [" + std::to_string(config->rotation_speed()) + "]");
    syslog.write("Range gain      [" + std::to_string(config->range_gain()) + "]");
    syslog.write("Range offset    [" + std::to_string(config->range_offset()) + "]");

    syslog.write("Requesting FFT data...");
    radar_client.send(Type::start_fft_data);
}



void process_health(Client& radar_client [[maybe_unused]], Message& msg)
{
    using Navtech::Protobuf::from_vector_into;
    using namespace Navtech::Networking;

    auto health = msg.view_as<Colossus_protocol::TCP::Health>();
    auto data   = from_vector_into<Core::Configuration::Protobuf::Health>(health->to_vector()).value();

    auto to_string = [](Core::Configuration::Protobuf::HealthStatus h) -> std::string
    {
        std::string strings[] { "UNHEALTHY", "WARNING", "HEALTHY", "UNKNOWN" };
        return strings[static_cast<int>(h)];
    };

    syslog.write("Die temp: " + std::to_string(data.dietemperature().value()));
    syslog.write("Status:   " + to_string(data.dietemperature().status()));
}


// ---------------------------------------------------------------------------------------------------------------------
//
bool rotated_once(Azimuth_num azimuth)
{
    static bool has_rotated_once { };
    static Azimuth_num prev { 0 };
    
    if (has_rotated_once) return true;
    if (azimuth < prev) has_rotated_once = true;
    prev = azimuth;

    return has_rotated_once;
}


bool completed_full_rotation(Azimuth_num azimuth)
{
    if (!rotated_once(azimuth)) return false;

    bool has_completed_rotation { false };
    static Azimuth_num prev { };

    if (azimuth < prev) has_completed_rotation = true;
    prev = azimuth;

    return has_completed_rotation;
}


void check_for_lost_packet(std::uint16_t counter, unsigned long packet_count)
{
    static bool first_update { true };
    static std::uint16_t prev { };

    if (first_update) {
        prev = counter;
        first_update = false;
        return;
    }

    if (counter != static_cast<std::uint16_t>(prev + 1)) {
        Log::Stream log_stream { };

        log_stream  << "Packets lost! "
                    << "packet [" << packet_count << "] "
                    << "current sweep counter [" << counter << "] "
                    << "previous [" << prev << "] ";
        
        syslog.error(log_stream);
    }

    prev = counter;
}


void process_FFT(Client& radar_client [[maybe_unused]], Message& msg)
{
    using namespace Navtech::Networking;
    using namespace Navtech::Time::Monotonic;
    using Navtech::Protobuf::from_vector_into;
    using Utility::Statistical_value;

    static unsigned long packet_count { };
    static unsigned rotations { };
    static Observation t0 { now() };
    static Statistical_value<double, 10> packet_rate { };

    ++packet_count;

    auto fft  = msg.view_as<Colossus_protocol::TCP::FFT_data>();
    auto data = fft->to_vector();

    // check_for_lost_packet(fft->sweep_counter(), packet_count);

    if (!completed_full_rotation(fft->azimuth())) return;

    ++rotations;
    auto t1 = now();
    auto rotation_period = t1 - t0;
    
    packet_rate = packet_count / rotation_period.in_sec();

    if (rotations % 10 == 0) {
        Log::Stream log_stream { };
        
        log_stream  << "Rotation [" << rotations << "] "
                    << "Timestamp [" << fft->timestamp().to_string() << "] "
                    << "average packet rate [" << packet_rate.mean() << "] ";

        syslog.write(log_stream);           
    }

    packet_count = 0;
    t0 = t1;
}

// ---------------------------------------------------------------------------------------------------------------------
//
Option_parser options {
    Option { "--ipaddress", "-i", "Colossus server IP address", optional, has_argument, "127.0.0.1" },
    Option { "--port",      "-p", "Colossus server port",       optional, has_argument, "6317" },
    Option { "--loglevel",  "-l", "Minimum log level",          optional, has_argument, "info" }
};


// ---------------------------------------------------------------------------------------------------------------------
//
int main(int argc, char** argv)
try
{ 
    // Set up signal handling for ctrl-c (SIGINT)
    // and kill (SIGTERM)
    // 
    Signal_handler signal_handler { };
    signal_handler.register_handler(SIGINT, stop_running);
    signal_handler.register_handler(SIGTERM, stop_running);

    // Command line option parsing
    //
    options.parse(argc, argv);
    auto server_addr = options.global("-i").translate_to<IP_address>();
    auto server_port = options.global("-p").to_int<std::uint16_t>();
    auto log_level   = options.global("-l").value();

    syslog.min_level(log_level);
    syslog.write("Starting...");

    // This function *must* be called before using
    // any networking client/server
    //
    Navtech::SDK::initialise();
  
    // Construct a radar client and set up handlers for
    // a couple of messages.
    // Note, the radar will always send a configuration message
    // upon connection, so you should provide a handler for this
    // message.
    //
    Client radar_client { Endpoint { server_addr, server_port } };
    radar_client.set_handler(Type::configuration, process_config);
    radar_client.set_handler(Type::fft_data, process_FFT);
    radar_client.set_handler(Type::health, process_health);

    radar_client.start();
    
    while (running) {
        sleep_for(500_msec);
    }

    // Failing to call these functions may lead to instability
    // on shutdown.
    //
    radar_client.stop();
    Navtech::SDK::shutdown();

    syslog.write("Done.");
}
catch (std::exception& ex) {
    syslog.critical("TERMINATING MAIN DUE TO EXCEPTION: " + std::string { ex.what() });
    Navtech::SDK::shutdown();
}
catch (...) {
    syslog.critical("TERMINATING MAIN DUE TO UNHANDLED EXCEPTION");
    Navtech::SDK::shutdown();
}