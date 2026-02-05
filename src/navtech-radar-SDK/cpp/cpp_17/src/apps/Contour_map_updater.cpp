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
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>

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
#include "Message_buffer.h"
#include "csv_parser.h"


using Navtech::Utility::syslog;

using Navtech::Utility::Option_parser;
using Navtech::Utility::Option;
using Navtech::Utility::Option_group;
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
using Navtech::Networking::Message_buffer;


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
//
Option_parser options {
    Option_group { "radar",
        {
            Option { "--ipaddress", "-i", "Colossus server IP address", optional, has_argument, "192.168.0.1" },
            Option { "--port", "-p",      "Colossus server port",       optional, has_argument, "6317" }
        }
    },
    Option_group { "contour_map",
        {
            Option { "--file",  "-f", "csv contour map file", required, has_argument, "contour_map.csv" },
            Option { "--clear", "-c", "Clear contour map",    optional, no_argument }
        }
    }
};


// ---------------------------------------------------------------------------------------------------------------------
//
using Contour_map = std::vector<std::uint16_t>;

std::optional<Message_buffer> load_contour_map()
{
    using std::filesystem::exists;
    using std::filesystem::path;
    using Utility::parse_csv;
    using std::nullopt;
    using std::getline;
    using std::ifstream;
    using std::string;

    Contour_map contour_map  { };
    path        csv_filepath { options["contour_map"]["-f"].value() };

    // Parse the csv file...
    //
    if (!exists(csv_filepath)) {
        syslog.error("Cannot find " + csv_filepath.string());
        return nullopt;
    }

    ifstream file_stream { csv_filepath };
    string   line        { };

    while (getline(file_stream, line)) {
        // Should only be a single line, if the csv
        // file is properly formatted
        //
        auto vals = parse_csv<uint16_t>(line);
        contour_map.insert(contour_map.end(), vals.begin(), vals.end());
    }
    file_stream.close();

    if (contour_map.size() != 360) {
        syslog.error(
            "Not enough values in " + csv_filepath.string() + ". "
            "Expected 360, received " + std::to_string(contour_map.size())
        );
        return nullopt;
    }
    
    // Ensure the values are in the correct endianness...
    //
    std::transform(
        contour_map.begin(),
        contour_map.end(),
        contour_map.begin(),
        [](std::uint16_t b) { return to_uint16_network(b); }
    );

    // Convert into a 'raw' buffer for sending...
    //
    std::vector<std::uint8_t> payload { };
    payload.resize(720);
    std::memcpy(payload.data(), contour_map.data(), 720);

    return payload;
}


void process_config(Client& radar_client, Message& msg)
{
    syslog.debug("Configuration received.");

    Message contour_msg { };
    contour_msg.type(Type::contour_update);

    if (options["contour_map"]["-c"]) {
        syslog.write("Clearing contour map...");
        radar_client.send(contour_msg);
    }
    else {
        syslog.write("Sending contour map [" + options["contour_map"]["-f"].value() + "]");

        auto contour_map = load_contour_map();

        if (contour_map.has_value()) {
            contour_msg.append(std::move(contour_map.value()));
        }
        else {
            return;
        }
    }

    radar_client.send(contour_msg);

    running = false;
}



// ---------------------------------------------------------------------------------------------------------------------
//
int main(int argc, char* argv[])
try
{
    // This function *must* be called before using
    // any networking client/server
    //
    Navtech::SDK::initialise();
    
    // Set up signal handling for ctrl-c (SIGINT)
    // and terminate (SIGTERM)
    // 
    Signal_handler signal_handler { };
    signal_handler.register_handler(SIGINT,  stop_running);
    signal_handler.register_handler(SIGTERM, stop_running);

    syslog.write("Starting...");

    // Command line option parsing
    //
    options.parse(argc, argv);
    auto server_addr = options["radar"]["-i"].translate_to<IP_address>();
    auto server_port = options["radar"]["-p"].to_int<std::uint16_t>();
  
    // Construct a radar client.
    // Note, the radar will always send a configuration message
    // upon connection, so you should provide a handler for this
    // message. In this example, we are telling the radar client
    // to ignore any keep-alive messages it may receive. this stops
    // the client reporting that it has no handler for these messages
    //
    Client radar_client { Endpoint { server_addr, server_port } };
    radar_client.set_handler(Type::configuration, process_config);
    radar_client.ignore(Type::keep_alive);

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