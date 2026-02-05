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
#include "Endpoint.h"
#include "Time_utils.h"
#include "Log.h"
#include "Signal_handler.h"

#include "Colossus_UDP_client.h"

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Monotonic;
using namespace Navtech::Networking;

using Navtech::Utility::Option_parser;
using Navtech::Utility::Option;

using Navtech::Utility::Log;
using Navtech::Utility::syslog;

using Navtech::Utility::Signal_handler;

// ---------------------------------------------------------------------------------------------------------------------
//
Option_parser options {
    {
        Option { "--ipaddress", "-i", "IP address where UDP data arrives",  optional, has_argument, "127.0.0.1" },
        Option { "--port", "-p",      "Port to connect to",                 optional, has_argument, "6317" },
        Option { "--raw", "-r",       "Raw output values",                  optional, no_argument, "0" }
    }
};

std::string float_to_string(float value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

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
// Message handling
//
bool raw_print {false};

void process_imu_message(Colossus_protocol::UDP::Client&, Colossus_protocol::UDP::Message& msg)
{
    const std::string deg {"°"};
    const std::string deg_per_sec {"°/s"};
    const std::string g {"G"};
    using namespace std::chrono;
    using namespace Colossus_protocol::UDP;

    static auto last_call_time = steady_clock::now();
    auto current_time = steady_clock::now();
    auto duration = duration_cast<seconds>(current_time - last_call_time);

    if (duration.count() >= 1) {
        auto imu = msg.view_as<IMU>();
        IMU::IMU_values imu_values = imu->data();
        if (raw_print != false) {
            syslog.write("IMU [" + std::to_string(imu_values.x_acc)
                + " "   + std::to_string(imu_values.y_acc)
                + " "   + std::to_string(imu_values.z_acc)
                + "] [" + std::to_string(imu_values.roll_vel)
                + " "   + std::to_string(imu_values.pitch_vel)
                + " "   + std::to_string(imu_values.yaw_vel)
                + "] [" + std::to_string(imu_values.phi_angl)
                + " "   + std::to_string(imu_values.theta_angl)
                + " "   + std::to_string(imu_values.psi_angl) + "]");
        }
        else {
            syslog.write("IMU - x_acc:" + float_to_string(static_cast<float>(imu_values.x_acc)/1000, 3) + g
                + " y_acc:" + float_to_string(static_cast<float>(imu_values.y_acc)/1000, 3) + g
                + " z_acc:" + float_to_string(static_cast<float>(imu_values.z_acc)/1000, 3) + g
                + " roll:"  + float_to_string(static_cast<float>(imu_values.roll_vel)/10, 1) + deg_per_sec
                + " pitch:" + float_to_string(static_cast<float>(imu_values.pitch_vel)/10, 1) + deg_per_sec
                + " yaw:"   + float_to_string(static_cast<float>(imu_values.yaw_vel)/10, 1) + deg_per_sec
                + " phi:"   + float_to_string(static_cast<float>(imu_values.phi_angl)/10, 1) + deg
                + " theta:" + float_to_string(static_cast<float>(imu_values.theta_angl)/10, 1) + deg
                + " psi:"   + float_to_string(static_cast<float>(imu_values.psi_angl)/10, 1) + deg);
        }

        last_call_time = current_time;

    }
}


// ---------------------------------------------------------------------------------------------------------------------
//
int main(int argc, char* argv[])
try
{
    SDK::initialise();

    // Set up signal handling for ctrl-c (SIGINT)
    // and kill (SIGTERM)
    // 
    Signal_handler signal_handler { };
    signal_handler.register_handler(SIGINT, stop_running);
    signal_handler.register_handler(SIGTERM, stop_running);
    
    // Command line option parsing
    //
    options.parse(argc, argv);
    auto recv_addr = options.global("-i").translate_to<Networking::IP_address>();
    auto recv_port = options.global("-p").translate_to<Networking::Port>();
    raw_print = options.global("-r").to_bool();

    if (raw_print != false) {
        syslog.write("Printing raw values. Units are:");
        syslog.write("Acceleration: 0.0001G.");
        syslog.write("Rotation: 0.1 degree per second");
        syslog.write("Angle: 0.1 degree");
        syslog.write("IMU [x_acc y_acc z_acc] [roll_vel pitch_vel yaw_vel] [phi_angl theta_angl psi_angl]");
    } 
    else {
        syslog.write("Printing human readable values.");
    }

    Colossus_protocol::UDP::Client client { Endpoint { recv_addr, recv_port } };
    client.set_handler(Colossus_protocol::UDP::Type::imu, process_imu_message);
    client.start();

    while (running) {
        sleep_for(500_msec);
    }

    client.stop();
    SDK::shutdown();
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