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
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "Stream_server_traits.h"
#include "Endpoint.h"
#include "Active.h"
#include "pointer_types.h"
#include "Log.h"

using Navtech::Utility::syslog;

namespace Navtech::Networking {

    template <Protocol, Transport, TLS::Type> class Connection_manager;


    template <Protocol protocol, Transport transport, TLS::Type tls>
    class Acceptor : public Utility::Active {
    public:
        // Type aliases.
        // The '_Ty' postfix denotes a template type. Rather than being
        // supplied as template parameters on the class (which would be unwieldy) 
        // these parameters are looked up from the Stream_server_traits class, 
        // using the appropriate combination of protocol, transport and TLS type
        //
        using Server_traits     = Stream_server_traits<protocol, transport, tls>;
        using Protocol_traits   = Navtech::Networking::Protocol_traits<protocol, transport, tls>;
        using Socket_Ty         = typename Server_traits::Socket;

        Acceptor(Connection_manager<protocol, transport, tls>& conx_mgr);
        Acceptor(Connection_manager<protocol, transport, tls>& conx_mgr, const Endpoint& port);

        void bind_to(const Endpoint& listen_endpt);

    private:
        association_to<Connection_manager<protocol, transport, tls>> connection_mgr;
    
        Endpoint  endpoint;
        Socket_Ty listening_socket { };

        // Active object overrides
        //
        void on_start() override;
        void on_stop()  override;
        Active::Task_state run() override;
    };



    template <Protocol protocol, Transport transport, TLS::Type tls>
    Acceptor<protocol, transport, tls>::Acceptor(Connection_manager<protocol, transport, tls>& conx_mgr) :
        Active          { "TCP Acceptor" },
        connection_mgr  { associate_with(conx_mgr) }
    {
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    Acceptor<protocol, transport, tls>::Acceptor(
        Connection_manager<protocol, transport, tls>&   conx_mgr, 
        const Endpoint&                                 listen_endpt
    ) :
        Active          { "TCP Acceptor" },
        connection_mgr  { associate_with(conx_mgr) },
        endpoint        { listen_endpt }
    {
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Acceptor<protocol, transport, tls>::bind_to(const Endpoint& bind_endpt)
    {
        endpoint = bind_endpt;
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Acceptor<protocol, transport, tls>::on_start()
    {
        syslog.debug("Acceptor - listening on port [" + endpoint.port.to_string() + "]");

        try {
            listening_socket.bind_to(endpoint);
            listening_socket.listen(Server_traits::listener_backlog);
        }
        catch (std::system_error& e) {
            syslog.debug("Acceptor - on_start() caught exception: " + std::string { e.what() });
        }
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Acceptor<protocol, transport, tls>::on_stop()
    {
        try {
            listening_socket.close();
        }
        catch (std::system_error& e) {
            syslog.debug("Acceptor - on_stop() caught exception: " + std::string { e.what() });
        }
        syslog.debug("Acceptor - stopping.");
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    Utility::Active::Task_state Acceptor<protocol, transport, tls>::run()
    {
        try {
            try_dispatch_async();
            auto incoming = Server_traits::allocate_socket(listening_socket.accept());

            // TODO - configure socket options, if required.
            //
            
        #ifdef _WIN32
            // Windows sockets need their rx timeouts explicitly set to 0
            // (don't timeout) to prevent them from immediately erroring
            // when receive() is called.
            //
            incoming->rx_timeout(0_sec);
        #endif

            auto client = incoming->peer();
            
            syslog.debug("Acceptor - connected to [" + client.to_string() + "]");
            
            connection_mgr->create_connection(incoming);
        }
        catch (std::system_error& e) {
            syslog.debug("Acceptor - caught exception: " + std::string { e.what() });
            return Task_state::finished;
        }

        return Task_state::not_finished;
    }

} // namespace Navtech::Networking


#endif