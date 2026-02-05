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
#include <atomic>
#include "sdk.h"
#include "Option_parser.h"
#include "Endpoint.h"
#include "Time_utils.h"
#include "Log.h"
#include "File_writer.h"
#include "pointer_types.h"

#include "Signal_handler.h"

#include "Colossus_TCP_client.h"
#include "Colossus_UDP_client.h"

#include "Euclidean_target.h"
#include "Polar_target.h"
#include "Spherical_target.h"

#include "Spherical_coordinate.h"

#include "CFAR_algorithms.h"

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Monotonic;
using namespace Navtech::Networking;

using Navtech::Utility::Option_parser;
using Navtech::Utility::Option;

using Navtech::Networking::IP_address;
using Navtech::Networking::Port;
using Navtech::Networking::Endpoint;

using Navtech::Utility::syslog;

using Navtech::Utility::File_writer;
using Navtech::Utility::Signal_handler;

// ---------------------------------------------------------------------------------------------------------------------
//
Option_parser options {
    {
        Option { "--ipaddress",     "-i",   "IP address to connect to", optional, has_argument, "127.0.0.1" },
        Option { "--port",          "-p",   "Port to connect to",       optional, has_argument, "6317" },
        Option { "--udpaddress",    "-u",   "UDP address to listen on", optional, has_argument, "127.0.0.1" },
        Option { "-udpport",        "-d",   "UDP port to listen on",    optional, has_argument, "6317" },
        Option { "-filetag",        "-f",   "Output filename",          optional, has_argument, "points" },
        Option { "--mode",          "-m",   "Data mode",                optional, has_argument, "0" }
    }
};


enum class Network_mode { TCP, UDP };


class Pointcloud_3d_writer : public File_writer<Navigation::Euclidean::Target> {
public:
    using File_writer<Navigation::Euclidean::Target>::File_writer;

protected:
    void header() override;
    void on_write(const Navigation::Euclidean::Target& target) override;
};


void Pointcloud_3d_writer::header() 
{
    output_file << "x [m]" << ", ";
    output_file << "y [m]" << ", ";
    output_file << "z [m]" << ", ";
    output_file << "power [dB]" << std::endl;
}


void Pointcloud_3d_writer::on_write(const Navigation::Euclidean::Target& target)
{
    output_file << target.coordinate.x << ", ";
    output_file << target.coordinate.y << ", ";
    output_file << target.coordinate.z << ", ";
    output_file << target.power << std::endl;
}


class Pointcloud_3d_client : public Utility::Active {
public:
    Pointcloud_3d_client(
        const Endpoint&     tcp_endpoint,
        const Endpoint&     udp_endpoint,
        std::string_view    filename,
        Network_mode        mode
    );
    
    ~Pointcloud_3d_client() noexcept = default;

    void update_current_rotation(const Unit::Degrees& new_rotation);
    void start_new_capture();
    bool has_finished_capture();

private:
    void on_start() override;
    void on_stop() override;

    Colossus_protocol::TCP::Client  tcp_client;
    Colossus_protocol::UDP::Client  udp_client;

    Network_mode                          net_mode  { Network_mode::TCP };
    Colossus_protocol::TCP::Configuration radar_cfg { };

    // Colossus TCP Message Handlers
    //
    void process_config(const Colossus_protocol::TCP::Message& msg);
    void process_fft(const Colossus_protocol::TCP::Message& msg);
    void process_nav_points(const Colossus_protocol::TCP::Message& msg);
    void on_process_nav_points(const Colossus_protocol::TCP::Message& msg);

    // Colossus UDP Message Handlers
    //
    void process_pointcloud(const Colossus_protocol::UDP::Message& msg);
    void on_process_pointcloud(const Colossus_protocol::UDP::Message& msg);

    // Capture
    //
    std::atomic_bool        capturing           { };
    bool                    captured_first      { false };
    bool                    rotated_once        { false };

    std::vector<Navigation::Polar::Target>      latest_capture      { };
    Unit::Degrees                               current_rotation    { 0.0f };
    Monotonic::Observation                      last{ };

    bool has_rotated_once(Unit::Azimuth latest_azimuth);
    bool has_completed_rotation(Unit::Azimuth latest_azimuth);
    void finish_rotation();

    // File Writing an processing
    //
    Pointcloud_3d_writer point_writer { };
   
    void process_latest_scan();
    
    // User input processing
    //
    void on_update_current_rotation(const Unit::Degrees& new_rotation);
};


Pointcloud_3d_client::Pointcloud_3d_client(
    const Endpoint&     tcp_endpoint,
    const Endpoint&     udp_endpoint,
    std::string_view    filename,
    Network_mode        mode
) :
    Active          { "Pointcloud client"},
    tcp_client      { tcp_endpoint },
    udp_client      { udp_endpoint },
    net_mode        { mode },
    point_writer    { filename }
{
}


void Pointcloud_3d_client::on_start() 
{
    using namespace Colossus_protocol;

    tcp_client.set_handler(
        TCP::Type::keep_alive,
        [this](TCP::Client&, TCP::Message&) { return; }
    );

    tcp_client.set_handler(
        TCP::Type::configuration,
        [this](TCP::Client&, TCP::Message& msg) { process_config(msg); }
    );

    tcp_client.set_handler(
        TCP::Type::fft_data,
        [this](TCP::Client&, TCP::Message& msg) { process_fft(msg); }
    );

    tcp_client.start();
    point_writer.start();
}


void Pointcloud_3d_client::on_stop() 
{
    syslog.debug("Pointcloud 3D client stopping...");
    
    point_writer.stop();
    tcp_client.stop();

    if (net_mode == Network_mode::UDP) {
        udp_client.stop();
    }
}


void Pointcloud_3d_client::process_config(const Colossus_protocol::TCP::Message& msg)
{
    using namespace Colossus_protocol;

    auto config = msg.view_as<TCP::Configuration>();
    radar_cfg.azimuth_samples(config->azimuth_samples());
    radar_cfg.encoder_size(config->azimuth_samples());
    radar_cfg.bin_size(config->bin_size());

    if (net_mode == Network_mode::UDP) {
        syslog.write("Starting UDP client.");

        udp_client.set_handler(
            UDP::Type::point_cloud,
            [this](UDP::Client&, UDP::Message& msg) { process_pointcloud(msg); }
        );

        udp_client.start();
    }
    else {
        syslog.write("Starting Nav data.");

        tcp_client.set_handler(
            TCP::Type::navigation_data,
            [this] (TCP::Client&, TCP::Message& msg) { process_nav_points(msg); }
        );

        tcp_client.send(TCP::Type::start_nav_data);
    }

}


void Pointcloud_3d_client::process_fft(const Colossus_protocol::TCP::Message& msg [[maybe_unused]])
{
    // This is included for compatibility with playback data
    // While there should be no need for any FFT processing in this application,
    // logic may be added here if that changes
    //
    return;
}


void Pointcloud_3d_client::process_nav_points(const Colossus_protocol::TCP::Message& msg)
{
    async_call(&Pointcloud_3d_client::on_process_nav_points, this, msg);
}


void Pointcloud_3d_client::on_process_nav_points(const Colossus_protocol::TCP::Message& msg)
{
    using namespace Colossus_protocol;

    if (!capturing) return;

    auto nav_point_msg = msg.view_as<TCP::Navigation_data>();
    auto bearing       = nav_point_msg->azimuth() * 360.0f / radar_cfg.encoder_size();

    if (!has_rotated_once(nav_point_msg->azimuth())) return;

    auto [sz, points] = nav_point_msg->points();

    for (std::size_t i { 0 }; i < sz; ++i) {
        latest_capture.emplace_back(points[i].range(), bearing, points[i].power());
    }

    if (!has_completed_rotation(nav_point_msg->azimuth())) return;

    finish_rotation();
}


void Pointcloud_3d_client::process_pointcloud(const Colossus_protocol::UDP::Message& msg)
{
    async_call(&Pointcloud_3d_client::on_process_pointcloud, this, msg);
}


void Pointcloud_3d_client::on_process_pointcloud(const Colossus_protocol::UDP::Message& msg)
{
    using namespace Colossus_protocol;

    if (!capturing) return;
    auto pointcloud_spoke = msg.view_as<UDP::Pointcloud_spoke>();

    if (!has_rotated_once(pointcloud_spoke->azimuth())) return;

    auto bearing = pointcloud_spoke->bearing();

    auto [sz, points] = pointcloud_spoke->points();

    for (std::size_t i { 0 }; i < sz; ++i) {
        const auto& point = points[i];
        latest_capture.emplace_back(point.range(), bearing.to_float(), point.power() );
    }

    if (!has_completed_rotation(pointcloud_spoke->azimuth())) return;

    finish_rotation();
}


bool Pointcloud_3d_client::has_rotated_once(Unit::Azimuth latest_azimuth) 
{
    static Unit::Azimuth last_azimuth { 0 };

    if (rotated_once) return rotated_once;

    rotated_once = latest_azimuth < last_azimuth;   
    last_azimuth = latest_azimuth;

    return rotated_once;
}


bool Pointcloud_3d_client::has_completed_rotation(Unit::Azimuth latest_azimuth) 
{
    if (!has_rotated_once(latest_azimuth)) return false;

    static Unit::Azimuth    last_azimuth       { 0 };
    bool                    completed_rotation { false };

    if (latest_azimuth < last_azimuth) completed_rotation = true;

    last_azimuth = latest_azimuth;

    return completed_rotation;
}


void Pointcloud_3d_client::finish_rotation()
{
    if (capturing) {
        process_latest_scan();
        capturing = false;
    }

    latest_capture.clear();
}


void Pointcloud_3d_client::process_latest_scan()
{
    for (auto& p : latest_capture) {
        // The radar returns values in 2D Polar coordinates
        // So rotation about the y axis requires only changing the value of Theta
        // Applying this logic, we can directly compute the euclidean
        //
        Navigation::Spherical::Target target {
            p.coordinate.range, 
            p.coordinate.bearing, 
            current_rotation, 
            p.power
        };

        point_writer.write(target.to_euclidean());
    }
}


void Pointcloud_3d_client::update_current_rotation(const Unit::Degrees& new_rotation)
{
    async_call(&Pointcloud_3d_client::on_update_current_rotation, this, new_rotation);
}


void Pointcloud_3d_client::on_update_current_rotation(const Unit::Degrees& new_rotation) 
{
    // Rotation measured as angle to the ground
    // Needs to be converted to polar angle
    //
    current_rotation = Unit::Degrees { 90.0 } - new_rotation;
}


void Pointcloud_3d_client::start_new_capture()
{
    capturing    = true;
    rotated_once = false;
}


bool Pointcloud_3d_client::has_finished_capture()
{
    return !capturing;
}


class User_interface : public Utility::Active {
public:
    User_interface(Pointcloud_3d_client& pointcloud_client);

    void cancel();

private:
    Navtech::association_to<Pointcloud_3d_client> pc3d_client { };

    Utility::Active::Task_state run() override;
    bool cancel_run { false };
};


User_interface::User_interface(Pointcloud_3d_client& pointcloud_client) :
    Active      { "User interface" },
    pc3d_client { Navtech::associate_with(pointcloud_client) }
{
} 


void User_interface::cancel()
{
    cancel_run = true;
}


Utility::Active::Task_state User_interface::run()
{
    if (cancel_run) return Task_state::finished;

    try_dispatch_async();

    if (!pc3d_client->has_finished_capture()) return Task_state::not_finished;

    syslog.write("Enter the next rotation angle");

    std::string user_input;
    std::cin >> user_input;

    try {
        pc3d_client->update_current_rotation(std::stof(user_input));
    }
    catch (std::invalid_argument&) {
        syslog.write("A non-number was passed as an angle. Shutting down...");
        return Task_state::finished;
    }

    pc3d_client->start_new_capture();

    return Task_state::not_finished;
}


// ---------------------------------------------------------------------------------------------------------------------
//
int main(int argc, char* argv[])
{
    SDK::initialise();

    // Command line option parsing
    //
    options.parse(argc, argv);
    auto tcp_addr = options.global("-i").translate_to<IP_address>();
    auto tcp_port = options.global("-p").to_int<std::uint16_t>();
    auto udp_addr = options.global("-u").translate_to<IP_address>();   
    auto udp_port = options.global("-d").to_int<std::uint16_t>();
    auto filetag  = options.global("-f").value();

    Network_mode mode { options.global("-m").to_int<std::uint16_t>() };

    auto date_prefix = Real_time::Clock::now().format_as("%Y%m%d_%H%M%S").to_string();
    std::string filename { date_prefix + "_" + filetag + ".csv"};
    
    Pointcloud_3d_client point_client {
        { tcp_addr, tcp_port},
        { udp_addr, udp_port },
        filename,
        mode
    };

    User_interface  user_interface { point_client };

    // Set up signal handling for ctrl-c (SIGINT)
    // and kill (SIGTERM)
    // 
    Signal_handler signal_handler{ };
    auto stop_running = [&user_interface](std::int32_t, std::int32_t) { user_interface.cancel(); };
    signal_handler.register_handler(SIGINT, stop_running);
    signal_handler.register_handler(SIGTERM, stop_running);
    
    syslog.write("Starting...");
    
    point_client.start();
    user_interface.start();
    user_interface.join();
    point_client.stop();

    SDK::shutdown();
    syslog.write("Done.");
}