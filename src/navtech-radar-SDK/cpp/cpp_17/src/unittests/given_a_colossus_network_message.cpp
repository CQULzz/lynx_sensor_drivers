#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Colossus_TCP_network_message.h"
#include "Colossus_TCP_messages.h"
#include "IP_address.h"
#include "sdk.h"

using namespace Navtech;
using namespace Navtech::Networking;
using namespace Navtech::Utility;
using namespace std;

class GivenAColossusMessage : public ::testing::Test {
protected:
    GivenAColossusMessage() {
        SDK::initialise();
    }

    array<std::uint8_t, 32> message {
        0x00, 0x01, 0x03, 0x03, 0x09, 0x09, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x6F, 0x6F, 0x8F, 0x8F, 0xFE, 0xFE,
        10,
        00, 00, 00, 11,
        '+','+','M','e','s','s','a','g','e','+','+'
    };

    array<std::uint8_t, 33> protobuf_only {
        0x00, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0xFE, 0xFE,
        01,
        10,
        00, 00, 00, 11,
        '+','+','M','e','s','s','a','g','e','+','+'
    };


    array<std::uint8_t, 24> header_only {
        0x00, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0xFE, 0xFE,
        01,
        10,
        00, 00, 00, 2,
        0xCA, 0xFE
    };


    array<std::uint8_t, 53> header_and_protobuf {
        0x00, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0xFE, 0xFE,
        01,
        10,
        00, 00, 00, 31,
        0xCA, 0xFE, 
        0xBA, 0xBE, 
        0xDE, 0xAD, 
        0xBE, 0xEF, 
        0x00, 0xC0, 
        0xFF, 0xEE,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        '+','+','M','e','s','s','a','g','e','+','+'
    };


    std::vector<std::uint8_t> real_data {
        0x00, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0xFE, 0xFE, 
        0x01, 0x0A, 0x00, 0x00, 0x01, 0x87, 0x01, 0x90, 0x06, 0xD8, 0x0B, 0x28, 0x15, 0xE0, 0x0F, 0xA0, 
        0x06, 0x40, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x65, 0x08, 0x00, 0x12, 0x06, 
        0x43, 0x54, 0x53, 0x33, 0x35, 0x30, 0x1A, 0x24, 0x38, 0x42, 0x37, 0x46, 0x42, 0x37, 0x37, 0x39, 
        0x2D, 0x36, 0x35, 0x45, 0x44, 0x2D, 0x34, 0x30, 0x32, 0x38, 0x2D, 0x39, 0x31, 0x36, 0x31, 0x2D, 
        0x37, 0x43, 0x39, 0x43, 0x36, 0x35, 0x32, 0x30, 0x35, 0x34, 0x32, 0x37, 0x25, 0x00, 0x00, 0xFA, 
        0x43, 0x2A, 0x24, 0x31, 0x37, 0x37, 0x45, 0x46, 0x30, 0x30, 0x39, 0x2D, 0x38, 0x31, 0x32, 0x46, 
        0x2D, 0x34, 0x41, 0x42, 0x46, 0x2D, 0x42, 0x45, 0x35, 0x39, 0x2D, 0x46, 0x33, 0x38, 0x43, 0x34, 
        0x30, 0x37, 0x32, 0x35, 0x44, 0x30, 0x43, 0x35, 0x00, 0x00, 0x00, 0x3F, 0x3D, 0xCD, 0xCC, 0x4C, 
        0x3F, 0x12, 0x11, 0x35, 0x34, 0x3A, 0x31, 0x30, 0x3A, 0x65, 0x63, 0x3A, 0x36, 0x65, 0x3A, 0x61, 
        0x35, 0x3A, 0x30, 0x65, 0x1A, 0x4E, 0x0A, 0x08, 0x33, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x36, 0x34, 
        0x12, 0x08, 0x33, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x39, 0x35, 0x1A, 0x08, 0x33, 0x2E, 0x30, 0x2E, 
        0x30, 0x2E, 0x39, 0x35, 0x22, 0x07, 0x34, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x39, 0x2A, 0x0A, 0x34, 
        0x2E, 0x31, 0x30, 0x2E, 0x30, 0x2E, 0x32, 0x31, 0x32, 0x32, 0x07, 0x37, 0x2E, 0x30, 0x2E, 0x30, 
        0x2E, 0x30, 0x3A, 0x07, 0x30, 0x2E, 0x30, 0x2E, 0x30, 0x2E, 0x30, 0x42, 0x07, 0x30, 0x2E, 0x30, 
        0x2E, 0x30, 0x2E, 0x30, 0x22, 0x53, 0x08, 0xF5, 0x0A, 0x10, 0xFE, 0x02, 0x18, 0x6F, 0x20, 0x00,
        0x28, 0x03, 0x30, 0x00, 0x38, 0x00, 0x40, 0x00, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x58, 0x80, 0x95, 
        0xFD, 0xDB, 0x05, 0x60, 0x80, 0xBB, 0x9A, 0xA7, 0x06, 0x6A, 0x24, 0x38, 0x42, 0x37, 0x46, 0x42, 
        0x37, 0x37, 0x39, 0x2D, 0x36, 0x35, 0x45, 0x44, 0x2D, 0x34, 0x30, 0x32, 0x38, 0x2D, 0x39, 0x31, 
        0x36, 0x31, 0x2D, 0x37, 0x43, 0x39, 0x43, 0x36, 0x35, 0x32, 0x30, 0x35, 0x34, 0x32, 0x37, 0x70, 
        0x00, 0x78, 0x00, 0x80, 0x01, 0x01, 0x88, 0x01, 0x01, 0x2D, 0xA0, 0x40, 0xDD, 0x44, 0x32, 0x06, 
        0x36, 0x65, 0x61, 0x35, 0x30, 0x65, 0x38, 0x80, 0x04, 0x42, 0x24, 0x35, 0x34, 0x31, 0x30, 0x45, 
        0x43, 0x36, 0x45, 0x2D, 0x35, 0x30, 0x45, 0x35, 0x2D, 0x31, 0x30, 0x45, 0x43, 0x2D, 0x45, 0x41, 
        0x35, 0x30, 0x2D, 0x35, 0x34, 0x31, 0x30, 0x45, 0x43, 0x36, 0x45, 0x41, 0x35, 0x30, 0x45, 0x48,
        0x01, 0x55, 0x9B, 0x71, 0x33, 0x3E, 0x58, 0x06, 0x60, 0x00, 0x6A, 0x11, 0x30, 0x30, 0x3A, 0x30, 
        0x63, 0x3A, 0x63, 0x36, 0x3A, 0x38, 0x37, 0x3A, 0x39, 0x37, 0x3A, 0x33, 0x63
    };

    vector<uint8_t> only_header_vector         { begin(header_only), end(header_only) };
    vector<uint8_t> only_protobuf_vector       { begin(protobuf_only), end(protobuf_only) };
    vector<uint8_t> header_and_protobuf_vector { begin(header_and_protobuf), end(header_and_protobuf) };
    
    vector<uint8_t> payload_vector { '+','+','M','e','s','s','a','g','e','+','+' };
    string          payload_string { "++Message++" };
};



TEST_F(GivenAColossusMessage, DefaultConstructionIsCorrect)
{
    Colossus_protocol::TCP::Message msg { };

    ASSERT_EQ(msg.size(), 22);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.type(), Colossus_protocol::TCP::Type::invalid);
    ASSERT_EQ(msg.ip_address().to_string(), "0.0.0.0" );
}


TEST_F(GivenAColossusMessage, EmptyMessageIsCorrect)
{
    Colossus_protocol::TCP::Message msg { 
        IP_address { "192.168.2.1" }, 
        1
    };

    ASSERT_EQ(msg.size(), 22);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.type(), Colossus_protocol::TCP::Type::invalid);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01 );
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenAColossusMessage, IPAddressCanBeUpdated)
{
    Colossus_protocol::TCP::Message msg { };

    msg.ip_address("192.168.2.1"_ipv4);

    ASSERT_EQ(msg.ip_address().to_string(), "192.168.2.1"s);
}


TEST_F(GivenAColossusMessage, ConstructionWithCopiedVectorIsCorrect)
{
    Colossus_protocol::TCP::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        only_header_vector
    };

    ASSERT_EQ(msg.size(), 24);
    ASSERT_EQ(msg.header_size(), 22);
    ASSERT_EQ(msg.payload_size(), 2);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenAColossusMessage, ConstructionWithMovedVectorIsCorrect)
{
    vector<uint8_t> input_vector { header_and_protobuf_vector };

    Colossus_protocol::TCP::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        move(input_vector)
    };

    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(msg.header_size(), 22);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);

    ASSERT_EQ(input_vector.size(), 0);
}


TEST_F(GivenAColossusMessage, ConstructionWithIteratorsIsCorrect)
{
    Colossus_protocol::TCP::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        header_and_protobuf.data(), header_and_protobuf.size()
    };

    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(msg.header_size(), 22);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenAColossusMessage, ConstructionWithoutIPandIDIsCorrect)
{
    Colossus_protocol::TCP::Message msg { 
        header_and_protobuf_vector
    };

    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(msg.header_size(), 22);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address().to_string(), "0.0.0.0");
    ASSERT_EQ(msg.id(), 0);
}


TEST_F(GivenAColossusMessage, DefaultConstructionIsValid)
{
    Colossus_protocol::TCP::Message msg { };

    ASSERT_TRUE(msg.is_valid());
}


TEST_F(GivenAColossusMessage, NonDefaultConstructionIsValid)
{
    Colossus_protocol::TCP::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        only_header_vector
    };

    ASSERT_TRUE(msg.is_valid());
}


TEST_F(GivenAColossusMessage, SizesAreCorrect)
{
    Colossus_protocol::TCP::Message msg { 
        header_and_protobuf_vector
    };

    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(msg.header_size(), 22);
    ASSERT_EQ(msg.payload_size(), 31);
}


TEST_F(GivenAColossusMessage, PayloadIteratorsAreCorrect)
{
    Colossus_protocol::TCP::Message msg { 
        header_and_protobuf_vector
    };

    auto config =  msg.view_as<Colossus_protocol::TCP::Configuration>();

    ASSERT_EQ(config->end(), config->begin() + config->header_size());

    string result { config->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenAColossusMessage, AddingAMessageByCopyIsCorrect)
{
    Colossus_protocol::TCP::Message msg { "192.168.2.1"_ipv4, 1 };

    msg.replace(header_and_protobuf_vector);

    ASSERT_TRUE(msg.is_valid());
    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);

    auto config = msg.view_as<Colossus_protocol::TCP::Configuration>();
    string result { config->to_string() };
    ASSERT_EQ(payload_string, result);
}


TEST_F(GivenAColossusMessage, AddingAMessageByMoveIsCorrect)
{
    vector<uint8_t> input_vector { header_and_protobuf_vector };

    Colossus_protocol::TCP::Message msg { "192.168.2.1"_ipv4, 1 };

    msg.replace(move(input_vector));

    ASSERT_TRUE(msg.is_valid());
    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);

    ASSERT_EQ(input_vector.size(), 0);

    auto config = msg.view_as<Colossus_protocol::TCP::Configuration>();
    string result { config->to_string() };
    ASSERT_EQ(payload_string, result);
}


TEST_F(GivenAColossusMessage, AddedMessageTypeIsCorrect)
{
    Colossus_protocol::TCP::Message msg { "192.168.2.1"_ipv4, 1 };

    msg.replace(header_and_protobuf_vector);

    ASSERT_EQ(msg.type(), Colossus_protocol::TCP::Type::configuration);
}


TEST_F(GivenAColossusMessage, AddingPayloadByCopiedVector)
{
    Colossus_protocol::TCP::Message msg { "192.168.2.1"_ipv4, 1 };

    EXPECT_EQ(msg.payload_size(), 0);
    EXPECT_EQ(msg.size(), 22);

    msg.append(payload_vector);
    ASSERT_EQ(msg.size(), 33);
    ASSERT_EQ(msg.payload_size(), 11);

    auto health = msg.view_as<Colossus_protocol::TCP::Health>();

    string result { health->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenAColossusMessage, AddingPayloadByMovedVector)
{
    vector<uint8_t> input_vector { payload_vector };
    Colossus_protocol::TCP::Message msg { "192.168.2.1"_ipv4, 1 };

    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.size(), 22);
    ASSERT_EQ(input_vector.size(), 11);

    msg.append(move(input_vector));
    ASSERT_EQ(msg.size(), 33);
    ASSERT_EQ(msg.payload_size(), 11);

    ASSERT_EQ(input_vector.size(), 0);

    auto health = msg.view_as<Colossus_protocol::TCP::Health>();

    string result { health->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenAColossusMessage, AddingPayloadByCopiedString)
{
    Colossus_protocol::TCP::Message msg { };

    EXPECT_EQ(msg.payload_size(), 0);
    EXPECT_EQ(msg.size(), 22);

    msg.append(payload_string);
    ASSERT_EQ(msg.payload_size(), 11);

    auto health = msg.view_as<Colossus_protocol::TCP::Health>();

    string result { health->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenAColossusMessage, AddingPayloadByMovedString)
{
    Colossus_protocol::TCP::Message msg { };
    string input { payload_string };

    EXPECT_EQ(msg.payload_size(), 0);
    EXPECT_EQ(msg.size(), 22);

    msg.append(move(input));
    ASSERT_EQ(msg.payload_size(), 11);
    ASSERT_EQ(input.size(), 0);

    auto health = msg.view_as<Colossus_protocol::TCP::Health>();

    string result { health->to_string() };
    ASSERT_EQ(result, payload_string);
}



TEST_F(GivenAColossusMessage, AddingHeaderByCopy)
{
    Colossus_protocol::TCP::Message msg { };

    Colossus_protocol::TCP::Set_navigation_threshold set_threshold { };
    set_threshold.threshold(0xCAFE);

    msg.append(set_threshold);

    ASSERT_EQ(msg.size(), 24);
    ASSERT_EQ(msg.payload_size(), 2);
    
    auto set_threshold_view = msg.view_as<Colossus_protocol::TCP::Set_navigation_threshold>();
    ASSERT_EQ(set_threshold_view->threshold(), 0xCAFE);
}



TEST_F(GivenAColossusMessage, AddingHeaderThenProtobuf)
{
    Colossus_protocol::TCP::Message msg { };

    Colossus_protocol::TCP::Configuration config { };
    config.rotation_speed(4000);
    config.range_in_bins(2856);
    config.bin_size(1752);
    config.azimuth_samples(400);
    config.range_offset(1.25f);

    msg.append(config);
    
    ASSERT_EQ(msg.size(), 42);
    ASSERT_EQ(msg.payload_size(), 20);
    
    msg.append(payload_vector);

    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(msg.payload_size(), 31);

    auto config_view = msg.view_as<Colossus_protocol::TCP::Configuration>();

    ASSERT_EQ(config_view->rotation_speed(), 4000);
    ASSERT_EQ(config_view->range_in_bins(), 2856);
    ASSERT_EQ(config_view->bin_size(), 1752);
    ASSERT_EQ(config_view->azimuth_samples(), 400);
    ASSERT_FLOAT_EQ(config_view->range_offset(), 1.25f);

    string result { config_view->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenAColossusMessage, AddingProtobufThenHeader)
{
    Colossus_protocol::TCP::Message msg { };

    msg.append(payload_vector);

    ASSERT_EQ(msg.size(), 33);
    ASSERT_EQ(msg.payload_size(), 11);

    Colossus_protocol::TCP::Configuration config { };
    config.rotation_speed(4000);
    config.range_in_bins(2856);
    config.bin_size(1752);
    config.azimuth_samples(400);
    config.range_offset(1.25f);

    msg.append(config);
    
    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(msg.payload_size(), 31);
    
    auto config_view = msg.view_as<Colossus_protocol::TCP::Configuration>();

    ASSERT_EQ(config_view->rotation_speed(), 4000);
    ASSERT_EQ(config_view->range_in_bins(), 2856);
    ASSERT_EQ(config_view->bin_size(), 1752);
    ASSERT_EQ(config_view->azimuth_samples(), 400);
    ASSERT_FLOAT_EQ(config_view->range_offset(), 1.25f);

    string result { config_view->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenAColossusMessage, RelinquishEmptiesMessage)
{
    Colossus_protocol::TCP::Message msg { header_and_protobuf_vector };

    ASSERT_EQ(msg.type(), Colossus_protocol::TCP::Type::configuration);
    ASSERT_EQ(msg.size(), 53);

    vector<uint8_t> result { msg.relinquish() };
    
    ASSERT_EQ(result.size(), 53);
    ASSERT_EQ(result, header_and_protobuf_vector);
    ASSERT_EQ(msg.size(), 0);
}


TEST_F(GivenAColossusMessage, CopyIsSupported)
{
    Colossus_protocol::TCP::Message msg { header_and_protobuf_vector };
    Colossus_protocol::TCP::Message result_msg { msg };

    ASSERT_EQ(msg.size(), 53);
    ASSERT_EQ(result_msg.size(), 53);

    auto config = result_msg.view_as<Colossus_protocol::TCP::Configuration>();

    string result { config->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenAColossusMessage, MoveIsSupported)
{
    Colossus_protocol::TCP::Message msg { header_and_protobuf_vector };
    Colossus_protocol::TCP::Message result_msg { move(msg) };

    ASSERT_EQ(msg.size(), 0);
    ASSERT_EQ(result_msg.size(), 53);

    auto config = result_msg.view_as<Colossus_protocol::TCP::Configuration>();

    string result { config->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenAColossusMessage, RealDataIsInterpretedCorrectly)
{
    Colossus_protocol::TCP::Message msg { real_data };

    ASSERT_EQ(msg.size(), real_data.size());

    auto config_view = msg.view_as<Colossus_protocol::TCP::Configuration>();

    ASSERT_EQ(config_view->rotation_speed(), 4000);
    ASSERT_EQ(config_view->range_in_bins(), 2856);
    ASSERT_EQ(config_view->azimuth_samples(), 400);
}


TEST_F(GivenAColossusMessage, IterationOverSignatureBehavesCorrectly)
{
    Colossus_protocol::TCP::Signature signature { };

    std::uint16_t count { 0 };

    for (auto i [[maybe_unused]]: signature) ++count;
    
    ASSERT_EQ(count, 16);
}