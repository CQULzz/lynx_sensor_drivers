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
#include <algorithm>

#include "sdk.h"
#include "Option_parser.h"
#include "Endpoint.h"
#include "Time_utils.h"
#include "Log.h"
#include "Signal_handler.h"
#include "string_helpers.h"

#include "Colossus_TCP_client.h"
#include "Colossus_TCP_server.h"

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Monotonic;
using namespace Navtech::Networking;

using Navtech::Utility::syslog;

using Navtech::Utility::Option_parser;
using Navtech::Utility::Option;
using Navtech::Utility::Option_group;
using Navtech::Utility::Signal_handler;

// ---------------------------------------------------------------------------------------------------------------------
//
void process_config(Colossus_protocol::TCP::Client& client, Colossus_protocol::TCP::Message& msg)
{
    client.send(Colossus_protocol::TCP::Type::start_fft_data);
}


void process_fft(Colossus_protocol::TCP::Client& client, Colossus_protocol::TCP::Message& msg)
{
    auto fft_header = msg.view_as<Colossus_protocol::TCP::FFT_data>();
    auto fft_data   = fft_header->to_vector();

    syslog.debug("Data offset: " + std::to_string(fft_header->fft_data_offset()));
    syslog.debug("Data size:   " + std::to_string(fft_data.size()));

    client.send(Colossus_protocol::TCP::Type::stop_fft_data);
}


// ---------------------------------------------------------------------------------------------------------------------
//
Option_parser options {
    Option_group { "radar",
        {
            Option { "--ipaddress", "-i", "IP address to connect to", optional, has_argument, "192.168.0.1" },
            Option { "--port",      "-p", "Port to connect to",       optional, has_argument, "6317" }
        }
    },
    Option_group { "server",
        {
            Option { "--ipaddress", "-i", "IP address to bind to", optional, has_argument, "0.0.0.0" },
            Option { "--port",      "-p", "Port to bind to",       optional, has_argument, "56317" }
        }
    }
};


// ---------------------------------------------------------------------------------------------------------------------
// The Relay class is a simple piece of code to demonstrate the use
// of the Colossus client and server.
// The Relay connects to a radar (server) and receives FFT data, which
// it forwards on via a client.
// Any commands coming in via the Relay's server are simply forwarded
// on the radar.
// This Relay performs no processing on the incoming 8-bit FFT data. 
// For incoming 16-bit (high-precision) FFT data the Relay allows the gain 
// (contrast) and offset (brightness) to be adjusted. The resultant modified
// FFT is output as 8-bit data, for visualisation.
//
class Relay : public Active {
public:
    Relay(const Endpoint& radar, const Endpoint& server);

    void brightness(std::int16_t val)   { async_call([this](std::int16_t val) { offset = val; }, val); }
    std::int16_t brightness()           { return sync_call<std::int16_t>([this] { return offset; }); }

    void contrast(float val)            { async_call([this](float val) { gain = val; }, val); }
    float contrast()                    { return sync_call<float>([this] { return gain; }); }

protected:
    void on_start() override;
    void on_stop()  override;

private:
    Endpoint radar_endpt;
    Endpoint bind_addr;

    Colossus_protocol::TCP::Client client;
    Colossus_protocol::TCP::Server server;

    float        gain   { 1.0 };
    std::int16_t offset { 0 };

    // Event handlers
    //
    Utility::Event_handler<Colossus_protocol::TCP::Message::ID> connect_handler { };
    Utility::Event_handler<Colossus_protocol::TCP::Message::ID> disconnect_handler { };

    void client_connected(const Colossus_protocol::TCP::Message::ID& id);
    void client_disconnected(const Colossus_protocol::TCP::Message::ID& id);

    // Client message handlers (messages from radar)
    //
    void process_config(const Colossus_protocol::TCP::Message& msg);
    void process_FFT(const Colossus_protocol::TCP::Message& msg);
    void process_high_precision_FFT(const Colossus_protocol::TCP::Message& msg);
    void process_health(const Colossus_protocol::TCP::Message& msg);

    // Server message handlers (messages from clients)
    //
    void request_FFT(const Colossus_protocol::TCP::Message& msg);
    void request_health(const Colossus_protocol::TCP::Message& msg);

    Colossus_protocol::TCP::Message config_msg  { };

    // Very simplistic client management
    //
    std::vector<Colossus_protocol::TCP::Message::ID> connected_clients { };
};


Relay::Relay(const Endpoint& radar_endpt, const Endpoint& server_endpt) :
    client  { radar_endpt },
    server  { server_endpt }
{
}


void Relay::on_start()
{
    using namespace Colossus_protocol::TCP;

    syslog.debug ("Relay starting...");

    // --------------------------------------------------------------------------------
    // Event handlers
    //
    connect_handler.when_notified_invoke(
        [this](const Message::ID& id) 
        { 
            async_call(&Relay::client_connected, this, id);
        }
    );
    Server_event::dispatcher.attach_to<Event::client_connected>(connect_handler);

    disconnect_handler.when_notified_invoke(
        [this](const Message::ID& id) 
        { 
            async_call(&Relay::client_disconnected, this, id);
        }
    );
    Server_event::dispatcher.attach_to<Event::client_disconnected>(disconnect_handler);

    // --------------------------------------------------------------------------------
    // Message handlers
    //
    client.set_handler(
        Type::configuration,
        [this](Client&, Message& msg) { process_config(msg); }
    );

    client.set_handler(
        Type::fft_data,
        [this](Client&, Message& msg) { process_FFT(msg); }
    );

    client.set_handler(
        Type::high_precision_fft_data,
        [this](Client&, Message& msg) { process_high_precision_FFT(msg); }
    );

    client.set_handler(
        Type::health,
        [this](Client&, Message& msg) { process_health(msg); }
    );

    client.set_handler(
        Type::keep_alive,
        [](Client&, Message& msg) { /* Do nothing*/ }
    );

    server.set_handler(
        Type::start_fft_data,
        [this](Server&, Message& msg) { request_FFT(msg); }
    );

    server.set_handler(
        Type::start_health_msgs,
        [this](Server&, Message& msg) { request_health(msg); }
    );

    // Start the client, but *not* the server. Don't allow server connections 
    // until we've received a configuration from the radar
    //
    client.start();
}


void Relay::on_stop()
{
    using namespace Colossus_protocol::TCP;

    syslog.debug("Relay stopping...");

    server.stop();
    client.stop();

    Server_event::dispatcher.detach_from<Event::client_connected>(connect_handler);
    Server_event::dispatcher.detach_from<Event::client_disconnected>(disconnect_handler);
}


void Relay::client_connected(const Colossus_protocol::TCP::Message::ID& id)
{
    using namespace Colossus_protocol::TCP;

    syslog.write("Relay - new client connection [" + std::to_string(id) + "]");

    connected_clients.emplace_back(id);

    // When a client connects, the first action of the server must be
    // to send a configuration message. 
    // Don't forget to update the id for the newly-connected client.
    //
    config_msg.id(id);
    server.send(config_msg);
}


void Relay::client_disconnected(const Colossus_protocol::TCP::Message::ID& id)
{
    syslog.write("Relay - disconnecting client [" + std::to_string(id) + "]");

    connected_clients.erase(
        std::remove(connected_clients.begin(), connected_clients.end(), id),
        connected_clients.end()
    );
}

// Server message handlers (messages from client).
// Requests from clients are simply forwarded on to the radar
//
void Relay::request_FFT(const Colossus_protocol::TCP::Message& msg)
{
    using namespace Colossus_protocol::TCP;

    syslog.write("Relay - client [" + std::to_string(msg.id()) + "] requesting FFT data");

    client.send(Type::start_fft_data);
}


void Relay::request_health(const Colossus_protocol::TCP::Message& msg)
{
    using namespace Colossus_protocol::TCP;

    syslog.write("Relay - client [" + std::to_string(msg.id()) + "] requesting health data");

    client.send(Type::start_health_msgs);
}


// Client message handlers (messages from radar)
// In this example messages from the radar are simply forwarded on to 
// all clients.  An additional level of client feature management would
// be required to handle which client requested which feature.
//
void Relay::process_config(const Colossus_protocol::TCP::Message& msg)
{
    using namespace Colossus_protocol::TCP;

    syslog.write("Relay - configuration received");

    auto config   = msg.view_as<Configuration>();
    syslog.write("Azimuth samples [" + std::to_string(config->azimuth_samples()) + "]");
    syslog.write("Bin size        [" + std::to_string(config->bin_size()) + "]");
    syslog.write("Range in bins   [" + std::to_string(config->range_in_bins()) + "]");
    syslog.write("Encoder size    [" + std::to_string(config->encoder_size()) + "]");
    syslog.write("Rotation rate   [" + std::to_string(config->rotation_speed()) + "]");
    syslog.write("Range gain      [" + std::to_string(config->range_gain()) + "]");
    syslog.write("Range offset    [" + std::to_string(config->range_offset()) + "]");

    syslog.write("Requesting FFT data...");

    // We don't do any processing on any of the messages so the config message 
    // is stored as-is so it can be sent out to any connecting clients when 
    // they connect.
    //
    config_msg = msg;

    // Now we have a configuration we can start up the server
    //
    server.start();
}


void Relay::process_FFT(const Colossus_protocol::TCP::Message& msg)
{
    // If the radar is outputting 8-bit data, just forward it on.
    // Don't apply gain and offset
    //
    using namespace Colossus_protocol::TCP;

    Message forwarded_msg { msg };

    for (auto id : connected_clients) {
        forwarded_msg.id(id);
        server.send(forwarded_msg);
    }
}


void Relay::process_high_precision_FFT(const Colossus_protocol::TCP::Message& msg)
{
    // Apply gain first to scale the data range (increasing the video contrast)
    // Then apply the offset (adjusting the overall video brightness)
    // Output the data as a 8-bit FFT
    //
    using namespace Colossus_protocol::TCP;

    auto incoming_fft = msg.view_as<FFT_data>();
    auto fft_16bit    = incoming_fft->to_vector();
    auto fft_sz       = fft_16bit.size() / 2;

    FFT_data fft_header { };
    fft_header.sweep_counter(incoming_fft->sweep_counter());
    fft_header.azimuth(incoming_fft->azimuth());
    fft_header.timestamp(incoming_fft->timestamp());

    std::vector<std::uint8_t> fft_8bit { };
    fft_8bit.reserve(fft_sz);

    // fft_16bit is a buffer of std::uint8_t, which we are
    // going to treat as 16-bit values.
    //
    std::uint16_t* bin_itr { reinterpret_cast<std::uint16_t*>(fft_16bit.data()) };

    for (unsigned i { 0 }; i < fft_sz; ++i ) {
        auto adjusted_bin = static_cast<std::uint8_t>(
            std::clamp(
                static_cast<int>(((*bin_itr * gain) / 256) + offset),
                0, 
                255
            )
        );
        fft_8bit.emplace_back(adjusted_bin);
        ++bin_itr;
    }

    Message output_msg { };

    output_msg.type(Type::fft_data);
    output_msg.append(fft_header);
    output_msg.append(fft_8bit);

    server.send(output_msg);
}


void Relay::process_health(const Colossus_protocol::TCP::Message& msg)
{
    using namespace Colossus_protocol::TCP;

    Message forwarded_msg { msg };

    for (auto id : connected_clients) {
        forwarded_msg.id(id);
        server.send(forwarded_msg);
    }
}



// ---------------------------------------------------------------------------------------------------------------------
// UI, to allow gain and offset to be changed
//
class UI : public Utility::Active {
public:
    UI(Relay& the_relay) : relay { associate_with(the_relay) }
    {
    }

protected:
    void on_start()             override;
    void on_stop()              override;
    Active::Task_state run()    override;

private:
    association_to<Relay> relay { };
};


void UI::on_start()
{
    syslog.debug("UI starting...");
}


void UI::on_stop()
{
    syslog.debug("UI stopping...");
}


Active::Task_state UI::run()
{
    try_dispatch_async();

    std::string command { };
    std::cin >> command;

    if (command == "gain" || command == "g") {
        auto gain = relay->contrast();
        std::cout << "Gain [" << gain << "]" << " - new value: ";
        float value { };
        std::cin >> value;

        relay->contrast(value);
    }
    
    if (command == "offset" || command == "o") {
        auto offset = relay->brightness();
        std::cout << "Offset [" << offset << "]" << " - new value: ";
        std::int16_t value { };
        std::cin >> value;

        relay->brightness(value);
    }

    if (command == "quit" || command == "q") {
        std::cout << "Quitting UI..." << std::endl;
        return Task_state::finished;
    }
   
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
    auto radar_addr  = options["radar"]["-i"].translate_to<IP_address>();
    auto radar_port  = options["radar"]["-p"].to_int<Port>();
    auto server_addr = options["server"]["-i"].translate_to<IP_address>();
    auto server_port = options["server"]["-p"].to_int<Port>();

    syslog.write("Starting...");

    Relay relay { 
        Endpoint { radar_addr, radar_port } ,
        Endpoint { server_addr, server_port }
    };

    UI ui { relay };

    relay.start();
    ui.start();

    ui.join();
    relay.stop();

    SDK::shutdown();
    syslog.write("Done.");
}