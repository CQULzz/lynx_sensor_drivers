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
#ifndef OFFLINE_CLIENT_H
#define OFFLINE_CLIENT_H

#include <functional>
#include <string_view>
#include <fstream>

#include "Colossus_TCP_messages.h"
#include "Message_dispatchers.h"
#include "File_client.h"

namespace Navtech::Networking::Offline {

    class Client {
    public:
        using Handler = std::function<void(Client&, Colossus_protocol::TCP::Message&)>;
        using Dispatcher = Message_dispatcher<Protocol::colossus, Transport::tcp, TLS::Type::none, Client>;
        using Event_dispatcher = Dispatcher::Dispatcher_Ty;
        using Fl_client = File_client<Protocol::colossus, Transport::file, TLS::Type::none>;
        using Event_traits = Navtech::Networking::Event_traits<Protocol::colossus, Transport::file, TLS::Type::none>;

        using ID_Ty = typename Navtech::Networking::Offline::File_client_traits<Protocol::colossus, Transport::file, TLS::Type::none>::ID;
        using Event_handler = Utility::Event_handler<ID_Ty>;


        Client(const std::string& filepath);

        void start();
        void stop();
        void join();

        void set_file_closed_handler(Event_handler& handler);
        void set_handler(Colossus_protocol::TCP::Type type, const Handler& handler);
        void remove_handler(Colossus_protocol::TCP::Type type);
        void ignore(Colossus_protocol::TCP::Type type);

        Colossus_protocol::TCP::Configuration read_config_msg();

    private:
        Fl_client client;
        Dispatcher msg_dispatcher;
        association_to<Event_dispatcher> event_dispatcher;
        Utility::Event_handler<ID_Ty> event_handler { };
    };

}

#endif