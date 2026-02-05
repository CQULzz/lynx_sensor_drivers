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
#ifndef OFFLINE_CONNECTION_H
#define OFFLINE_CONNECTION_H

#include <atomic>

#include "Connection_traits.h"
#include "Event_traits.h"
#include "Connection_error_events.h"
#include "socket_exceptions.h"

#include "Recording_metadata.h"

#include "string_helpers.h"

#include "Active.h"
#include "Compression_utils.h"
#include "Log.h"


using Navtech::Utility::syslog;


namespace Navtech::Networking::Offline {
    
    enum class File_format { colraw, radar, invalid };

    template <Protocol protocol, Transport transport, TLS::Type tls>
    class Receiver : public Utility::Active {
    public:
        using Event_traits      = Networking::Event_traits<protocol, transport, tls>;
        using Connection_traits = Networking::Connection_traits<protocol, transport, tls>;
        using Protocol_traits   = typename Connection_traits::Protocol_traits;
        using Stream_Ty         = typename Connection_traits::Stream;
        using Message_buffer_Ty = typename Connection_traits::Message_buffer;
        using Message           = typename Connection_traits::Message;
        using ID_Ty             = typename Connection_traits::ID;
        using Dispatcher_Ty     = typename Event_traits::Dispatcher;

        Receiver(
            Stream_Ty&          stream,
            ID_Ty               identity,
            Dispatcher_Ty&      protocol_event_dispatcher,
            Error::Dispatcher&  error_event_dispatcher,
            File_format         format
        );

        void enable();
        void disable();

        std::uint64_t   bytes_read() { return total_read; }

    private:
        // External Service Associations
        //
        association_to<Stream_Ty>           stream;
        association_to<Dispatcher_Ty>       protocol_events;
        association_to<Error::Dispatcher>   error_events;

        // Operating state
        //
        std::atomic<bool>   enabled { false };
        ID_Ty               id;
        File_format         format { File_format::invalid };

        // Active class overrides
        //
        void on_start() override;
        void on_stop() override;

        // File reading helpers
        //
        Colossus_protocol::TCP::Type    get_next_message_type();
        std::uint8_t                    read_byte();
        std::uint32_t                   read_uint32();
        void                            increment_file_position(std::size_t bytes);
        std::vector<std::uint8_t>       read_bytes(std::size_t bytes);

        // Buffers for incoming data
        //
        Message         incoming_msg { };
        std::uint32_t   record_payload_sz { 0 };
        std::uint64_t   total_read { 0 };

        // Finite State Machine implementation 
        // (Moore machine - behaviour in-state)
        //
        enum State { initial, reading_meta, reading_record, decompressing, read_uncmprssd, dispatching, closing, num_states };
        enum Event { error, go, read_meta, compressed, uncompressed, valid_message, invalid_message, dispatched, eof, num_events };

        using Activity = void (Receiver::*)(void);

        struct State_cell {
            State    next_state;
            Activity do_action;
        };

        State current_state { initial };

        void post_event(Event e) { async_call(&Receiver::process_event, this, e); }

        void read_metadata();
        void process_radar_meta();
        void read_record();
        void decompress();
        void read_message();
        void dispatch();
        void end_of_file();
        void shutdown();
        void process_event(Event e);

        static constexpr State_cell state_machine[num_events][num_states] {
        //                  Initial                                         Reading Metadata                            Read Record                                 Decompressing                               Read Uncompressed                           Dispatching     Closing
        /* error        */ { { },                                           { closing, &Receiver::shutdown},            { closing, &Receiver::shutdown },           { closing, &Receiver::shutdown },           { closing, &Receiver::shutdown },           { },                                        { } },
        /* go           */ { { reading_meta, &Receiver::read_metadata },    { },                                        { },                                        { },                                        { },                                        { },                                        { } },
        /* Read meta    */ { {  },                                          { reading_record, &Receiver::read_record }, { },                                        { },                                        { },                                        { },                                        { } },
        /* compressed   */ { { },                                           { },                                        { decompressing, &Receiver::decompress },   { },                                        { },                                        { },                                        { } },
        /* uncompressed */ { { },                                           { },                                        { read_uncmprssd, &Receiver::read_message}, { },                                        { },                                        { },                                        { } },
        /* valid msg    */ { { },                                           { },                                        { },                                        { dispatching, &Receiver::dispatch },       { dispatching, &Receiver::dispatch },       { },                                        { } },
        /* invalid msg  */ { { },                                           { },                                        { },                                        { reading_record, &Receiver::read_record }, { reading_record, &Receiver::read_record }, { },                                        { } },
        /* dispatched   */ { { },                                           { },                                        { },                                        { },                                        { },                                        { reading_record, &Receiver::read_record }, { } },
        /* EOF          */ { { },                                           { },                                        { closing, &Receiver::end_of_file },        { },                                        { },                                        { closing, &Receiver::end_of_file },        { } }
        };


        Utility::Null::Stopwatch<100> stopwatch { };
    };

    template <Protocol protocol, Transport transport, TLS::Type tls>
    Receiver<protocol, transport, tls>::Receiver(
        Stream_Ty&          stream,
        ID_Ty               identity,
        Dispatcher_Ty&      protocol_event_dispatcher,
        Error::Dispatcher&  error_event_dispatcher,
        File_format         format
    ) :
        Active              { "Receiver [" + std::to_string(identity) + "]" },
        stream              { associate_with(stream) },
        protocol_events     { associate_with(protocol_event_dispatcher) },
        error_events        { associate_with(error_event_dispatcher) },
        id                  { identity },
        format              { format }
    {
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::on_start()
    {
        syslog.debug("Offline receiver [" + std::to_string(id) + "] starting...");

        enable();

        // Post an event to kick-start the state machine
        //
        post_event(go);
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::on_stop()
    {
        syslog.debug("Offline receiver [" + std::to_string(id) + "] stopping...");

        disable();
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    std::uint8_t Receiver<protocol, transport, tls>::read_byte()
    {
        std::uint8_t out { 0 };
        if (!enabled) return out;

        stream->read(reinterpret_cast<char*>(&out), sizeof(std::uint8_t));
        return out;
    }

    
    template <Protocol protocol, Transport transport, TLS::Type tls>
    std::uint32_t Receiver<protocol, transport, tls>::read_uint32()
    {
        std::uint32_t read { };
        stream->read(reinterpret_cast<char*>(&read), sizeof(std::uint32_t));
        total_read += read;
        return read;
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::increment_file_position(std::size_t bytes)
    {
        stream->seekg(stream->tellg() + static_cast<std::streampos>(bytes));
        total_read += bytes;
    }   


    template <Protocol protocol, Transport transport, TLS::Type tls>
    std::vector<std::uint8_t> Receiver<protocol, transport, tls>::read_bytes(std::size_t bytes)
    {
        std::vector<std::uint8_t> message_buffer(bytes);
        stream->read(reinterpret_cast<char*>(message_buffer.data()), bytes);
        total_read += bytes;
        return message_buffer;
    }

    
    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::read_metadata()
    {
        try {
            switch (format) {
                case File_format::colraw:
                    // Nothing to be done, no metadata stored
                    //
                    post_event(read_meta);
                    break;
                case File_format::radar:
                    // Metadata should be ignored here
                    //
                    process_radar_meta();
                    break;
                case File_format::invalid:
                    post_event(error);
                    break;
            }
        }
        catch (std::system_error& ex) {
            syslog.debug("Offline receiver [" + std::to_string(id) + "] caught exception: " + ex.what());
        }
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::process_radar_meta()
    {
        // Metadata format
        // start_date   [8 bytes] | end data  [8 bytes] 
        // start_ticks  [8 bytes] | end_ticks [8 bytes]
        // ip_address   [4 bytes]
        // The metadata size is the first 4 bytes of the file, 
        // But it includes the size of the size data in itself
        //
        auto meta_size = Networking::to_uint32_host(read_uint32()) - sizeof(std::uint32_t);
        
        if (meta_size != Metadata::size()) {
            syslog.error(
                "Offline receiver [" + std::to_string(id) + "] - "
                "The size of the file's metadata ["   + std::to_string(meta_size) + "] "
                "does not match the expected value [" + std::to_string(Metadata::size()) + "]"
            );
            
            post_event(error);
            return;
        }
        
        Metadata metadata { read_bytes(meta_size) };
       
        syslog.write(
            "Offline receiver [" + std::to_string(id) + "] "
            "Reading data from radar originally at [" + metadata.ip_address().to_string() + "]"
        );

        post_event(read_meta);
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::read_record()
    {
        if (!enabled) return;
        if (!stream->is_open()) post_event(error);
        
        try {
            std::uint8_t type_byte { 0 };

            switch (format) {
                case File_format::colraw:
                    // Colraw record format:
                    // Length (4 bytes) | Type (1 byte) | Top (4 bytes) | Bottom (4 bytes) | Message Data (<Length> bytes)
                    //
                    record_payload_sz = read_uint32();
                    type_byte = read_byte();
                    increment_file_position(2 * sizeof(std::uint32_t)); // Don't care about Top/Bottom
                    break;

                case File_format::radar:
                    // Radar record format:
                    // Type (1 byte) | Timestamp (8 bytes) | Length (4 bytes) | Message Data (<Length> bytes)
                    //
                    type_byte = read_byte();
                    increment_file_position(2 * sizeof(std::uint32_t));
                    record_payload_sz = Networking::to_uint32_host(read_uint32());
                    break;

                case File_format::invalid:
                    post_event(error);
                    return;
            }

            // Streams can't tell if they're at the end of the file 
            // until they attempt to read past the end of it
            //
            if (stream->eof() || static_cast<int>(stream->tellg()) == -1) {
                post_event(eof);
                return;
            }
            
            switch (type_byte) {
                case 0:
                    post_event(error);
                    break;

                case 255:
                    post_event(compressed);
                    break;

                default:
                    post_event(uncompressed);
                    break;
            }
        }
        catch (std::exception& ex) {
            syslog.debug("Offline receiver [" + std::to_string(id) + "] caught exception: " + ex.what());

            disable();
            post_event(error);
        }
    }


    template<Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::decompress()
    {
        using namespace Utility::Compression;

        auto compressed_data = read_bytes(record_payload_sz);
        std::vector<std::uint8_t> decompressed_data { };

        switch (format) {
            case File_format::colraw:
                decompressed_data = GZIP::decompress(compressed_data);
                break;

            case File_format::radar:
                decompressed_data = ZLIB::decompress(compressed_data);
                break;

            case File_format::invalid:
                // Should NEVER get here, but just in case
                //
                post_event(error);
                break;
        }

        total_read += static_cast<std::uint64_t>(decompressed_data.size());

        // The first 13 bytes of the decompressed data is header information
        // and can be discarded
        //
        decompressed_data.erase(decompressed_data.begin(), decompressed_data.begin() + 13);

        Protocol_traits::replace_data(incoming_msg, decompressed_data);

        if (Protocol_traits::is_valid(incoming_msg)) {
            post_event(valid_message);
        }
        else {
            syslog.debug("Offline receiver [" + std::to_string(id) + "] invalid header");

            incoming_msg.relinquish();
            post_event(invalid_message);
        }
        
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::read_message()
    {
        if (!enabled) return;
        if (!stream->is_open()) post_event(error);

        try {
            stopwatch.start();
            // At this point, the file stream pointer should be at the 
            // Beginning of the message itself
            //
            auto s [[maybe_unused]]  = static_cast<std::size_t>(stream->tellg());
            auto message_as_bytes    = read_bytes(record_payload_sz);
            auto s2 [[maybe_unused]] = static_cast<std::size_t>(stream->tellg());
            stopwatch.stop();

            // Since the receive call (above) is blocking, we may have
            // been disabled during the block (for example, by a failure
            // of the Sender). Don't continue if we've been disabled.
            //
            if (!enabled) {
                syslog.debug("Offline receiver [" + std::to_string(id) + "] disabled after read");
                return;
            }

            Protocol_traits::replace_data(incoming_msg, message_as_bytes);

            if (Protocol_traits::is_valid(incoming_msg)) {
                post_event(valid_message);
            }
            else {
                syslog.debug("Offline receiver [" + std::to_string(id) + "] invalid header");

                incoming_msg.relinquish();
                post_event(invalid_message);
            } 
        }
        catch (client_shutdown&) {
            syslog.debug("Offline receiver [" + std::to_string(id) + "] remote endpoint disconnected.");

            disable();
            post_event(error);
        }
        catch (std::system_error& e) {
            syslog.debug("Offline receiver [" + std::to_string(id) + "] caught exception: " + e.what());

            disable();
            post_event(error);
        }
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::enable()
    {
        syslog.debug("Offline receiver [" + std::to_string(id) + "] enabled.");
        enabled = true;
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::disable()
    {
        if (!enabled) return;
        enabled = false;
        
        syslog.debug("Offline receiver [" + std::to_string(id) + "] disabled.");
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::dispatch()
    {
        if (!enabled) return;

        try {
            Protocol_traits::add_client_id(incoming_msg, id);

            protocol_events->template notify<Event_traits::Received_message>(std::move(incoming_msg));

            if (stream->eof()) {
                post_event(eof);
            }
            else post_event(dispatched);
        }
        catch (std::system_error& e) {
            syslog.debug("Offline receiver [" + std::to_string(id) + "] caught exception: " + e.what());

            disable();
            post_event(error);
        }
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::end_of_file()
    {
        syslog.debug("Offline receiver [" + std::to_string(id) + "] has reached the end of the file.");

        protocol_events->template notify<Event_traits::Client_disconnected>(id);
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::shutdown()
    {
        syslog.debug("Offline receiver [" + std::to_string(id) + "] notifying rx error.");

        error_events->notify<Error::Event::rx_error>(id);
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Receiver<protocol, transport, tls>::process_event(Receiver<protocol, transport, tls>::Event event)
    {
        if (!state_machine[event][current_state].do_action) return;

        auto activity = state_machine[event][current_state].do_action;
        current_state = state_machine[event][current_state].next_state;

        (this->*activity)();
    }

    // -----------------------------------------------------------------------------------------------------------------
    //
    template <Protocol protocol, Transport transport, TLS::Type tls>
    class Connection {
    public:
        // Type aliases.
        // The '_Ty' postfix denotes a template type. Rather than being
        // supplied as template parameters on the class (which would be
        // unwieldy) these parameters are looked up from the Connection_traits
        // class, using the appropriate combination of protocol and transport
        //
        using Event_traits      = Navtech::Networking::Event_traits<protocol, transport, tls>;
        using Connection_traits = Navtech::Networking::Connection_traits<protocol, transport, tls>;
        using Protocol_traits   = typename Connection_traits::Protocol_traits;
        using Stream_Ty         = typename Connection_traits::Stream;
        using Message_Ty        = typename Connection_traits::Message;
        using ID_Ty             = typename Connection_traits::ID;
        using Dispatcher_Ty     = typename Event_traits::Dispatcher;

        Connection(
            ID_Ty               identifier, 
            Stream_Ty&&         strm, 
            Dispatcher_Ty&      protocol_event_dispatcher,
            Error::Dispatcher&  error_event_dispatcher,
            File_format         file_format
        );

        
        ~Connection();

        Connection(const Connection&)                   = delete;
        Connection& operator=(const Connection&)        = delete;
        Connection(Connection&&) noexcept               = default;
        Connection& operator=(Connection&&) noexcept    = default;

        void open();
        void close();

        void open_file();
        bool is_enabled() const;

        ID_Ty id() const;

    private:
        // External Associations
        //
        association_to<Dispatcher_Ty>       protocol_events;
        association_to<Error::Dispatcher>   error_events;

        // Internal Components
        //
        Stream_Ty stream;
        Receiver<protocol, transport, tls> receiver;

        // Event Handling (from Receiver)
        //
        Utility::Event_handler<ID_Ty> rx_error_handler { };
        void on_receive_error(const ID_Ty& identity);

        // Internal State
        //
        ID_Ty             ident         { };
        std::atomic<bool> enabled       { false };  
    };


    template <Protocol protocol, Transport transport, TLS::Type tls>
    Connection<protocol, transport, tls>::Connection(
        Connection<protocol, transport, tls>::ID_Ty            identity, 
        Connection<protocol, transport, tls>::Stream_Ty&&      strm,
        Connection<protocol, transport, tls>::Dispatcher_Ty&   protocol_event_dispatcher,
        Error::Dispatcher&                                     error_event_dispatcher,
        File_format                                            format
    ) :
        protocol_events { associate_with(protocol_event_dispatcher) },
        error_events    { associate_with(error_event_dispatcher) },
        stream          { std::move(strm) },
        receiver        { stream, identity, protocol_event_dispatcher, error_event_dispatcher, format },
        ident           { identity }
    {
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    Connection<protocol, transport, tls>::~Connection()
    {
        close();
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Connection<protocol, transport, tls>::open()
    {
        rx_error_handler.when_notified_invoke(&Connection::on_receive_error, this);
        error_events->attach_to<Error::Event::rx_error>(rx_error_handler);

        receiver.start();

        enabled = true;

        syslog.debug("Offline connection [" + std::to_string(ident) + "] opened.");
        
        protocol_events->template notify<Event_traits::Client_connected>(ident);
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Connection<protocol, transport, tls>::close()
    {
        if (!enabled) return;
        enabled = false;

        error_events->detach_from<Error::Event::rx_error>(rx_error_handler);

        try {
            stream.close();
        }
        catch (...) {
            // Don't care about errors, as closing file
            //
        }

        receiver.stop();
        receiver.join();

        // Diagnostics
        //
        syslog.debug(
            "Offline connection [" + std::to_string(ident) + "] closed. "
            "Received [" + Utility::to_memory_string(receiver.bytes_read()) + "]"
        );

        
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    void Connection<protocol, transport, tls>::on_receive_error(const ID_Ty& session_in_error)
    {
        if (!enabled)                           return;
        if (session_in_error != this-> ident)   return;

        // The receiver of the connection_error event 
        // should terminate this connection.
        //
        syslog.debug("Offline connection [" + std::to_string(ident) + "] receive error. Requesting termination.");
        
        error_events->notify<Error::Event::connection_error>(ident);
        protocol_events->template notify<Event_traits::Client_disconnected>(ident);
    }


    template <Protocol protocol, Transport transport, TLS::Type tls>
    bool Connection<protocol, transport, tls>::is_enabled() const
    {
        return enabled;
    }


    
    template <Protocol protocol, Transport transport, TLS::Type tls>
    typename Connection<protocol, transport, tls>::ID_Ty 
    Connection<protocol, transport, tls>::id() const
    {
        return ident;
    }
} // namespace Navtech::Networking::Offline
#endif