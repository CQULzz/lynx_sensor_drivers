#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Colossus_protocol.h"
#include "IP_address.h"
#include "MAC_address.h"

#include "Protobuf_helpers.h"


using namespace Navtech;
using namespace Navtech::Networking;
using namespace Navtech::Utility;
using namespace std;

class Test_Network_settings : public Networking::Colossus_protocol::UDP::Network_settings {
public:
    using Network_settings::ip_addr;
};


class GivenADiscoveryMessage : public ::testing::Test {
protected:
    GivenADiscoveryMessage() = default;

    array<std::uint8_t, 8> header_only {
        0x02, 
        0x0A,
        0xCE, 0x00,
        0x00, 0x00, 0x00, 0x00 
    };

    array<std::uint8_t, 36> network_settings_message {
        0x02, 
        0x14,
        0xCE, 0x00,
        0x00, 0x00, 0x00, 0x1C,
        0x01, 0x00, 0xA8, 0xC0,
        0x00, 0xFF, 0xFF, 0xFF,
        0x02, 0x00, 0xA8, 0xC0,
        0x03, 0x00, 0xA8, 0xC0,
        0x04, 0x00, 0xA8, 0xC0,
        0x05, 0x00, 0xA8, 0xC0,
        0x06, 0x00, 0xA8, 0xC0
    };

    array<std::uint8_t, 28> network_settings_payload {
        0x01, 0x00, 0xA8, 0xC0,
        0x00, 0xFF, 0xFF, 0xFF,
        0x02, 0x00, 0xA8, 0xC0,
        0x03, 0x00, 0xA8, 0xC0,
        0x04, 0x00, 0xA8, 0xC0,
        0x05, 0x00, 0xA8, 0xC0,
        0x06, 0x00, 0xA8, 0xC0
    };

    vector<uint8_t> only_header_vector          { begin(header_only), end(header_only) };
    vector<uint8_t> network_settings_vector     { begin(network_settings_message), end(network_settings_message) };
    vector<uint8_t> payload_vector              { begin(network_settings_payload), end(network_settings_payload) };
};


TEST_F(GivenADiscoveryMessage, MACAddressObjectsWorkCorrectly)
{
    MAC_address m1 { "DE:AD:CA:FE:BA:BE" };
    ASSERT_EQ(m1.to_string(), "de:ad:ca:fe:ba:be");

    auto octets = m1.to_byte_array();
    ASSERT_EQ(octets[0], 0xDE);
    ASSERT_EQ(octets[1], 0xAD);
    ASSERT_EQ(octets[2], 0xCA);
    ASSERT_EQ(octets[3], 0xFE);
    ASSERT_EQ(octets[4], 0xBA);
    ASSERT_EQ(octets[5], 0xBE);

    auto m2 = "BE:EE:EF:C0:FF:EE"_mac;
    ASSERT_EQ(m2.to_string(), "be:ee:ef:c0:ff:ee");

    MAC_address m3 { "DE:AD:BE:EF:CA:FE:BA:BE" };
    ASSERT_EQ(m3.to_string(), "00:00:00:00:00:00");

    auto m4 = "B:E:F:C:F:E"_mac;
    ASSERT_EQ(m4.to_string(), "0b:0e:0f:0c:0f:0e");
}


TEST_F(GivenADiscoveryMessage, ADiscoveryHeaderCanBeDefaultConstructed)
{
    Networking::Colossus_protocol::UDP::Discovery header { };

    ASSERT_EQ(header.azimuth_samples(), 0);
    ASSERT_EQ(header.ip_address().to_string(), "0.0.0.0");
    ASSERT_EQ(header.mac_address().to_string(), "00:00:00:00:00:00");
}


TEST_F(GivenADiscoveryMessage, ADiscoveryHeaderCanBeModified)
{
    Networking::Colossus_protocol::UDP::Discovery header { };

    header.azimuth_samples(400);
    header.ip_address("192.168.0.1"_ipv4);
    header.mac_address("DE:AD:CA:FE:BA:BE"_mac);

    ASSERT_EQ(header.azimuth_samples(), 400);
    ASSERT_EQ(header.ip_address().to_string(), "192.168.0.1");
    ASSERT_EQ(header.mac_address().to_string(), "de:ad:ca:fe:ba:be");
}


TEST_F(GivenADiscoveryMessage, MessageDefaultConstructionIsCorrect)
{
    Networking::Colossus_protocol::UDP::Message msg { };

    ASSERT_EQ(msg.size(), 8);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.type(), Networking::Colossus_protocol::UDP::Type::invalid);
    ASSERT_EQ(msg.ip_address().to_string(), "0.0.0.0" );
}


TEST_F(GivenADiscoveryMessage, NetworkSettingsDefaultConstructionIsCorrect)
{
    Networking::Colossus_protocol::UDP::Network_settings header { };

    ASSERT_EQ(header.ip_address().to_string(), "0.0.0.0");
    ASSERT_EQ(header.subnet_mask().to_string(), "0.0.0.0");
    ASSERT_EQ(header.gateway().to_string(), "0.0.0.0");
    ASSERT_EQ(header.primary_DNS().to_string(), "0.0.0.0");
    ASSERT_EQ(header.secondary_DNS().to_string(), "0.0.0.0");
    ASSERT_EQ(header.NTP_server().to_string(), "0.0.0.0");
    ASSERT_EQ(header.syslog_server().to_string(), "0.0.0.0");
}


TEST_F(GivenADiscoveryMessage, NetworkSettingsAddressesAreStoredInNetworkOrder)
{
    Test_Network_settings test_hdr { };

    auto test_addr = "192.168.0.1"_ipv4;

    test_hdr.ip_address(test_addr);
    ASSERT_EQ(test_hdr.ip_addr, 0x0100A8C0);

    auto result = test_hdr.ip_address();
    ASSERT_EQ(result.to_string(), "192.168.0.1");
}   


TEST_F(GivenADiscoveryMessage, EmptyMessageIsCorrect)
{
    Networking::Colossus_protocol::UDP::Message msg { 
        IP_address { "192.168.2.1" }, 
        1
    };

    ASSERT_EQ(msg.size(), 8);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.type(), Networking::Colossus_protocol::UDP::Type::invalid);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01 );
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenADiscoveryMessage, IPAddressCanBeUpdated)
{
    Networking::Colossus_protocol::UDP::Message msg { };

    msg.ip_address("192.168.2.1"_ipv4);

    ASSERT_EQ(msg.ip_address().to_string(), "192.168.2.1");
}


TEST_F(GivenADiscoveryMessage, ConstructionWithCopiedVectorIsCorrect)
{
    Networking::Colossus_protocol::UDP::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        only_header_vector
    };

    ASSERT_EQ(msg.size(), 8);
    ASSERT_EQ(msg.header_size(), 8);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenADiscoveryMessage, ConstructionWithMovedVectorIsCorrect)
{
    vector<uint8_t> input_vector { only_header_vector };

    Networking::Colossus_protocol::UDP::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        move(input_vector)
    };

    ASSERT_EQ(msg.size(), 8);
    ASSERT_EQ(msg.header_size(), 8);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);

    ASSERT_EQ(input_vector.size(), 0);
}


TEST_F(GivenADiscoveryMessage, ConstructionWithIteratorsIsCorrect)
{
    Networking::Colossus_protocol::UDP::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        &(*only_header_vector.begin()), only_header_vector.size()
    };

    ASSERT_EQ(msg.size(), 8);
    ASSERT_EQ(msg.header_size(), 8);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenADiscoveryMessage, ConstructionWithoutIPandIDIsCorrect)
{
    Networking::Colossus_protocol::UDP::Message msg { 
        network_settings_vector
    };

    ASSERT_EQ(msg.size(), 36);
    ASSERT_EQ(msg.header_size(), 8);
    ASSERT_EQ(msg.payload_size(), 28);
    ASSERT_EQ(msg.ip_address().to_string(), "0.0.0.0");
    ASSERT_EQ(msg.id(), 0);
}


TEST_F(GivenADiscoveryMessage, DefaultConstructionIsValid)
{
    Networking::Colossus_protocol::UDP::Message msg { };

    ASSERT_TRUE(msg.is_valid());
}


TEST_F(GivenADiscoveryMessage, NonDefaultConstructionIsValid)
{
    Networking::Colossus_protocol::UDP::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        only_header_vector
    };

    ASSERT_TRUE(msg.is_valid());
}


TEST_F(GivenADiscoveryMessage, SizesAreCorrect)
{
    Networking::Colossus_protocol::UDP::Message msg { 
        network_settings_vector
    };

    ASSERT_EQ(msg.size(), 36);
    ASSERT_EQ(msg.header_size(), 8);
    ASSERT_EQ(msg.payload_size(), 28);
}


// TEST_F(GivenADiscoveryMessage, PayloadIteratorsAreCorrect)
// {
//     Networking::Colossus_protocol::UDP::Message msg { 
//         network_settings_vector
//     };

//     auto settings =  msg.view_as<Networking::Colossus_protocol::UDP::Network_settings>();

//     ASSERT_EQ(settings->end(), settings->begin() + settings->header_size());

//     auto result { settings->to_vector() };
//     ASSERT_EQ(result, payload_vector);
// }


// TEST_F(GivenADiscoveryMessage, AddingAMessageByCopyIsCorrect)
// {
//     Networking::Colossus_protocol::UDP::Message msg { "192.168.0.1"_ipv4, 1 };

//     Networking::Colossus_protocol::UDP::Discovery header { };
//     header.azimuth_samples(400);
//     header.ip_address("192.168.0.1"_ipv4);
//     header.mac_address("DE:AD:CA:FE:BA:BE"_mac);

//     msg.append(header);
//     ASSERT_EQ(msg.size(), 30);
//     ASSERT_EQ(msg.payload_size(), 22);

//     Discovery discovery_payload { };
//     discovery_payload.Model.Id                  = 0;
//     discovery_payload.Model.unique_id			= UUID { };
//     discovery_payload.Model.name 				= "Test Radar";
//     discovery_payload.Model.range_in_metres 	= 500;
//     discovery_payload.MaxClientsAllowed 		= 3;
//     discovery_payload.RadarFeatures 			= Core::Configuration::Radar::Radar_feature::none;
//     discovery_payload.StaringMode 				= false;
//     discovery_payload.TransmitterEnabled 		= false;
//     discovery_payload.RangeResolutionMetres		= 0.175;
//     discovery_payload.OnBoardMacAddress 		= "00:00:00:00:00:00";

//     Core::Configuration::Protobuf::Discovery protobuf { };
//     discovery_payload.to_protocol_buffer(&protobuf);
    
//     auto message_buffer = Protobuf::vector_from(protobuf);
//     ASSERT_TRUE(message_buffer.has_value());

//     msg.append(message_buffer.value());
//     ASSERT_EQ(msg.payload_size(), 220);

//     auto view = msg.view_as<Networking::Colossus_protocol::UDP::Discovery>();
//     ASSERT_EQ(view->azimuth_samples(), 400);
//     ASSERT_EQ(view->ip_address().to_string(), "192.168.0.1");
//     ASSERT_EQ(view->mac_address().to_string(), "de:ad:ca:fe:ba:be");

//     auto payload = view->to_vector();
//     auto result_protobuf = Protobuf::from_vector_into<Core::Configuration::Protobuf::Discovery>(payload).value();
//     ASSERT_EQ(result_protobuf.model().id(), 0);
//     ASSERT_EQ(result_protobuf.model().name(), "Test Radar");
//     ASSERT_EQ(result_protobuf.model().rangeinmetres(), 500);
// }
