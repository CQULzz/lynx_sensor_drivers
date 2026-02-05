#ifndef FILE_PROTOCOL_TRAITS_H
#define FILE_PROTOCOL_TRAITS_H

#include "Colossus_TCP_network_message.h"
#include "Protocol_traits.h"

namespace Navtech::Networking {
    
    // -----------------------------------------------------------------------------------------------------------------
    //
    template <>
    class Protocol_traits<Protocol::colossus, Transport::file, TLS::Type::none> {
    public:
        using Message           = Navtech::Networking::Colossus_protocol::TCP::Message;
        using Pointer           = shared_owner<Navtech::Networking::Colossus_protocol::TCP::Message>;
		using ID                = Message::ID;
		using Type              = Navtech::Networking::Colossus_protocol::TCP::Type;
        using Buffer            = std::vector<std::uint8_t>;
        using Iterator          = std::uint8_t*;
        using Const_iterator    = const std::uint8_t*;

        // ---------------------------------------------------
		
		static constexpr const char* name { "Colossus TCP" };

        template <typename... Arg_Ty>
        static Message make(Arg_Ty&&... args)
        {
            return Message { std::forward<Arg_Ty>(args)... };
        }

        static void clear(Message& msg)
        {
            msg.relinquish();
        }

		static void replace_data(Message& inout_msg, const Buffer& in_buffer)
        {
            inout_msg.replace(in_buffer);
        }

		static void replace_data(Message& inout_msg, Buffer&& in_buffer)
        {
            inout_msg.replace(std::move(in_buffer));
        }

        static void add_header(Message& inout_msg, const Buffer& in_buffer)
		{
			inout_msg.replace(in_buffer);
        }

        static void add_header(Message& inout_msg, Buffer&& in_buffer)
		{
			inout_msg.replace(std::move(in_buffer));
        }

        static void add_payload(Message& inout_msg, const Buffer& in_buffer)
		{
		    inout_msg.append(in_buffer);
		}

        static void add_payload(Message& inout_msg, Buffer&& in_buffer)
		{
		    inout_msg.append(std::move(in_buffer));
		}

		static void add_ip_address(Message& inout_msg, const IP_address& in_addr)
		{
			inout_msg.ip_address(in_addr);
		}

		static ID client_id(const Message& in_msg)
		{
			return in_msg.id();
		}

		static ID client_id(const Pointer& in_msg)
		{
			return in_msg->id();
		}

        static void add_client_id(Message& inout_msg, int in_id)
		{
			inout_msg.id(in_id);
		}

		static void add_client_id(Pointer& inout_msg, int in_id)
		{
			inout_msg->id(in_id);
		}

		static Type type(const Message& in_msg)
		{
			return in_msg.type();
		}

		static Type type(const Pointer& in_msg)
		{
			return in_msg->type();
		}
        
        static std::size_t header_size (const Message& in_msg [[maybe_unused]])
		{
			return in_msg.header_size();
		}

        static std::size_t payload_size(const Message& in_msg)
		{
			return in_msg.payload_size();
		}
        
        static bool is_valid(const Message& in_msg)
		{
			return in_msg.is_valid();
		}

        static Pointer dyn_alloc()
		{
			return allocate_shared<Message>();
		}

        static Buffer to_buffer(const Message& in_msg)
        {
            return in_msg.data_view();
        }

		static Buffer to_buffer(Message&& in_msg)
        {
            return in_msg.relinquish();
        }
    };

} // namespace Navtech::Networking


#endif // FILE_PROTOCOL_TRAITS_H