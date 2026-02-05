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
#ifndef FILE_CLIENT_H
#define FILE_CLIENT_H

#include <filesystem>
#include <string_view>

#include "Active.h"
#include "Log.h"
#include "File_client_traits.h"
#include "Event_traits.h"

#include "Offline_connection.h"
#include "Message_dispatchers.h"


using Navtech::Utility::syslog;


namespace Navtech::Networking::Offline {
    template <Protocol protocol, Transport transport, TLS::Type tls = TLS::Type::none>
    class File_client : public Utility::Active {
    public:
        // Type aliases.
        // The '_Ty' postfix denotes a template type. Rather than being
        // supplied as template parameters on the class (which would be
        // unwieldy) these parameters are looked up from the Networking::Traits
        // class, using the appropriate combination of protocol and transport
        //
        using Traits            = Navtech::Networking::Offline::File_client_traits<protocol, transport, tls>;
        using Connection_Ty     = Navtech::Networking::Offline::Connection<protocol, transport, tls>;
        using Event_traits      = Navtech::Networking::Event_traits<protocol, transport, tls>;
        using Stream_Ty         = typename Traits::Stream;
        using Stream_ptr_Ty     = typename Traits::Stream_ptr;
        using Message_Ty        = typename Traits::Message;
        using ID_Ty             = typename Traits::ID;
        using Dispatcher_Ty     = typename Event_traits::Dispatcher;
        using Event_handler     = Utility::Event_handler<typename File_client_traits<protocol, transport, tls>::ID>;

        File_client(Dispatcher_Ty& event_dispatcher);

        File_client(std::string_view filepath, Dispatcher_Ty& event_dispatcher);

        void set_file_closed_handler(Event_handler& handler);

    private:
        // External associations
        //
        association_to<Dispatcher_Ty> dispatcher;
        
        owner_of<Connection_Ty>     connection { };
        std::string                 filepath { };
        Stream_ptr_Ty               stream_ptr { };

        // Active overrides
        //
        void on_start() override;
        void on_stop() override;
        Active::Task_state run() override;

        bool finished { false } ;

        // Helper(s)
        //
        File_format format_from_filepath(const std::string&  filepath);

        // Event Handling
        //
        Error::Dispatcher               error_events    { };
        Utility::Event_handler<ID_Ty>   event_handler   { };
        void on_file_disconnect(ID_Ty connection_id);

        // FSM
        //
        enum State    { closed, opening, opened, num_states };
        enum Event    { open, open_fail, open_ok, file_closed, error, num_events };

        using Activity = void (File_client::*)(void);

        struct State_cell {
            State    next_state;
            Activity do_action;
        };

        State current_state { closed };

        void post_event(Event e) { async_call(&File_client::process_event, this, e); }

        void connect();
        void close();
        void open_file();
        void process_event(Event event);

        static constexpr State_cell state_machine[num_events][num_states] {
            /*              Closed                                      Opening                                 Opened                              Closing */
            /* open */      { { opening, &File_client::connect },       { },                                    { },                                },
            /* failed */    { { },                                      { closed, &File_client::close },        { closed, &File_client::close},     },
            /* open OK */   { { },                                      { opened, &File_client::open_file },    { },                                },
            /* file close */{ { },                                      { },                                    { closed, &File_client::close }     },    
            /* error */     { { },                                      { },                                    { closed, &File_client::close },    }
        };
    };


    // -----------------------------------------------------------------------------------------------------------------
    //
    template <Protocol protocol, Transport transport, TLS::Type tls>
    File_client<protocol, transport, tls>::File_client(File_client<protocol, transport, tls>::Dispatcher_Ty& event_dispatcher) :
        Active          { "File client" },
        dispatcher      { associate_with(event_dispatcher) },
        stream_ptr      { allocate_shared<Stream_Ty>() }
    {
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    File_client<protocol, transport, tls>::File_client(
        std::string_view filepath, 
        File_client<protocol,transport, tls>::Dispatcher_Ty& event_dispatcher
    ) :
        Active          { "File client "},
        dispatcher      { associate_with(event_dispatcher) },
        filepath        { filepath },
        stream_ptr      { allocate_shared<Stream_Ty>() }
    {
    }

    
    template <Protocol protocol, Transport transport, TLS::Type tls>
    void File_client<protocol,transport, tls>::set_file_closed_handler(Event_handler& handler)
    {
        dispatcher->template attach_to<Event_traits::Client_disconnected>(handler);
    }


    template <Protocol protocol, Transport transport, TLS::Type  tls>
    void File_client<protocol, transport, tls>::on_start()
    {
        event_handler.when_notified_invoke(
            [this](ID_Ty connection_id [[maybe_unused]])
            {
                async_call(&File_client::on_file_disconnect, this, connection_id);
            }
        );

        dispatcher->template attach_to<Event_traits::Client_disconnected>(event_handler);

        post_event(open);
    }

    
    template <Protocol protocol, Transport transport, TLS::Type  tls>
    void File_client<protocol, transport, tls>::on_stop()
    {
        dispatcher->template detach_from<Event_traits::Client_disconnected>(event_handler);
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    Active::Task_state File_client<protocol, transport, tls>::run()
    {
        try_dispatch_async();
        if (finished) return Task_state::finished;
        else return Task_state::not_finished;
    }


    template <Protocol protocol, Transport transport, TLS::Type  tls>
    void File_client<protocol, transport, tls>::on_file_disconnect(
        File_client<protocol, transport, tls>::ID_Ty connection_id [[maybe_unused]]
    )
    {
        post_event(file_closed);
    }


    template <Protocol protocol, Transport transport, TLS::Type  tls>
    File_format File_client<protocol, transport, tls>::format_from_filepath(const std::string& filepath)
    {
        if (!std::filesystem::exists(filepath)) return File_format::invalid;

        auto loc_and_format = Utility::split(filepath, '.');

        if (loc_and_format.size() <= 1) return File_format::invalid;

        // If the number of file formats expands at any point, consider replacing this with a 
        // map
        if (loc_and_format.back() == "radar")       return File_format::radar;
        else if (loc_and_format.back() == "colraw") return File_format::colraw;
        else                                        return File_format::invalid;
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void File_client<protocol, transport, tls>::connect()
    {
        try {
            stream_ptr->open(filepath, std::ios_base::in | std::ios_base::binary);
            post_event(open_ok);
        }
        catch (std::system_error&) {
            syslog.error("File client - unable to open file");
            post_event(open_fail);
        }
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void File_client<protocol, transport, tls>::close()
    {
        syslog.debug("File client - closing...");

        connection->close();
        finished = true;
    }


    template <Protocol protocol, Transport transport, TLS::Type  tls>
    void File_client<protocol, transport, tls>::open_file()
    {
        try {
            syslog.write("File client - Opening file [" + filepath + "]");

            connection = allocate_owned<Connection_Ty>(
                ID_Ty { 1 },
                std::move(*stream_ptr),
                *dispatcher,
                error_events,
                format_from_filepath(filepath)
            );

            connection->open();

            post_event(open_ok);
        }
        catch (std::system_error&) {
            syslog.critical("File client - Unable to open file [" + filepath + "]");
            post_event(open_fail);
        }
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void File_client<protocol, transport, tls>::process_event(Event event)
    {
        if (!state_machine[event][current_state].do_action) return;

        auto activity = state_machine[event][current_state].do_action;
        current_state = state_machine[event][current_state].next_state;

        (this->*activity)();
    }
} // namespace Navtech::Networking::Offline
#endif