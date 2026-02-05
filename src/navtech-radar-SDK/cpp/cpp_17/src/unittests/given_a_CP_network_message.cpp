#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "CP_protocol.h"
#include "IP_address.h"
#include "sdk.h"

using namespace Navtech;
using namespace Navtech::Networking;
using namespace std;

class GivenACPNetworkMessage : public ::testing::Test {
protected:
    GivenACPNetworkMessage()
    {
        SDK::initialise();
    };

    array<std::uint8_t, 32> only_protobuf {
        0x00, 0x01, 0x03, 0x03, 0x09, 0x09, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x6F, 0x6F, 0x8F, 0x8F, 0xFE, 0xFE,
        10,
        00, 00, 00, 11,
        '+','+','M','e','s','s','a','g','e','+','+'
    };


    array<std::uint8_t, 23> only_header {
        0x00, 0x01, 0x03, 0x03, 0x09, 0x09, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x6F, 0x6F, 0x8F, 0x8F, 0xFE, 0xFE,
        10,
        00, 00, 00, 2,
        0xCA, 0xFE
    };


    array<std::uint8_t, 52> header_and_protobuf {
        0x00, 0x01, 0x03, 0x03, 0x09, 0x09, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x6F, 0x6F, 0x8F, 0x8F, 0xFE, 0xFE,
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

    vector<uint8_t> only_protobuf_vector       { begin(only_protobuf), end(only_protobuf) };
    vector<uint8_t> only_header_vector         { begin(only_header), end(only_header) };
    vector<uint8_t> header_and_protobuf_vector { begin(header_and_protobuf), end(header_and_protobuf) };

    vector<uint8_t> payload_vector { '+','+','M','e','s','s','a','g','e','+','+' };
    string          payload_string { "++Message++" };
};


TEST_F(GivenACPNetworkMessage, PreTestValidation)
{
    ASSERT_EQ(payload_string.size(), 11);
    ASSERT_EQ(payload_vector.size(), 11);
    ASSERT_EQ(only_header_vector.size(), 23);
    ASSERT_EQ(only_protobuf_vector.size(), 32);
    ASSERT_EQ(header_and_protobuf_vector.size(), 52);
}



TEST_F(GivenACPNetworkMessage, DefaultConstructionIsCorrect)
{
    CP_protocol::Message msg { };

    ASSERT_EQ(msg.size(), 21);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.type(), CP_protocol::Type::invalid);
    ASSERT_EQ(msg.ip_address().to_string(), "0.0.0.0");
    ASSERT_EQ(msg.id(), 0);
}


TEST_F(GivenACPNetworkMessage, EmptyMessageIsCorrect)
{
    CP_protocol::Message msg { 
        "192.168.2.1"_ipv4, 
        1
    };

    ASSERT_EQ(msg.size(), 21);
    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.type(), CP_protocol::Type::invalid);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01 );
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenACPNetworkMessage, ConstructionWithCopiedVectorIsCorrect)
{
    CP_protocol::Message msg { 
        "192.168.2.1"_ipv4, 
        1,
        only_header_vector
    };

    ASSERT_EQ(msg.size(), 23);
    ASSERT_EQ(msg.header_size(), 21);
    ASSERT_EQ(msg.payload_size(), 2);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenACPNetworkMessage, ConstructionWithMovedVectorIsCorrect)
{
    vector<uint8_t> input_vector { header_and_protobuf_vector };

    CP_protocol::Message msg { 
        IP_address { "192.168.2.1" }, 
        1,
        move(input_vector)
    };

    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(msg.header_size(), 21);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);

    ASSERT_EQ(input_vector.size(), 0);
}


TEST_F(GivenACPNetworkMessage, ConstructionWithIteratorsIsCorrect)
{
    CP_protocol::Message msg { 
        IP_address { "192.168.2.1" },
        1,
        header_and_protobuf.data(), header_and_protobuf.size()
    };

    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(msg.header_size(), 21);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);
}


TEST_F(GivenACPNetworkMessage, ConstructionWithoutIPandIDIsCorrect)
{
    CP_protocol::Message msg { 
        header_and_protobuf_vector
    };

    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(msg.header_size(), 21);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address().to_string(), "0.0.0.0");
    ASSERT_EQ(msg.id(), 0);
}


TEST_F(GivenACPNetworkMessage, DefaultConstructionIsValid)
{
    CP_protocol::Message msg { };

    ASSERT_TRUE(msg.is_valid());
}


TEST_F(GivenACPNetworkMessage, NonDefaultConstructionIsValid)
{
    CP_protocol::Message msg { 
        IP_address { "192.168.2.1" }, 
        1,
        only_header_vector
    };

    ASSERT_TRUE(msg.is_valid());
}


TEST_F(GivenACPNetworkMessage, CorruptedSignatureIsFound)
{
    auto corrupt_msg { header_and_protobuf_vector };
    corrupt_msg[0] = 0xFF;

    CP_protocol::Message msg { };
    msg.replace(corrupt_msg);

    ASSERT_FALSE(msg.is_valid());
}


TEST_F(GivenACPNetworkMessage, SizesAreCorrect)
{
    CP_protocol::Message msg { 
        header_and_protobuf_vector
    };

    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(msg.header_size(), 21);
    ASSERT_EQ(msg.payload_size(), 31);
}



TEST_F(GivenACPNetworkMessage, IPAddressCanBeUpdated)
{
    CP_protocol::Message msg { };

    msg.ip_address("192.168.2.1"_ipv4);

    ASSERT_EQ(msg.ip_address().to_string(), "192.168.2.1"s);
}


TEST_F(GivenACPNetworkMessage, PayloadIteratorsAreCorrect)
{
    CP_protocol::Message msg { 
        header_and_protobuf_vector
    };

    auto config =  msg.view_as<CP_protocol::Test_message>();

    ASSERT_EQ(config->end(), config->begin() + config->header_size());

    string result { config->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, AddingAMessageByCopyIsCorrect)
{
    CP_protocol::Message msg { "192.168.2.1"_ipv4, 1 };

    msg.replace(header_and_protobuf_vector);

    ASSERT_TRUE(msg.is_valid());
    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);

    auto config = msg.view_as<CP_protocol::Test_message>();
    string result { config->to_string() };
    ASSERT_EQ(payload_string, result);
}


TEST_F(GivenACPNetworkMessage, AddingAMessageByMoveIsCorrect)
{
    vector<uint8_t> input_vector { header_and_protobuf_vector };

    CP_protocol::Message msg { "192.168.2.1"_ipv4, 1 };

    msg.replace(move(input_vector));

    ASSERT_TRUE(msg.is_valid());
    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(msg.payload_size(), 31);
    ASSERT_EQ(msg.ip_address(), 0xC0'A8'02'01);
    ASSERT_EQ(msg.id(), 1);

    ASSERT_EQ(input_vector.size(), 0);

    auto config = msg.view_as<CP_protocol::Test_message>();
    string result { config->to_string() };
    ASSERT_EQ(payload_string, result);
}


TEST_F(GivenACPNetworkMessage, AddedMessageTypeIsCorrect)
{
    CP_protocol::Message msg { "192.168.2.1"_ipv4, 1 };

    msg.replace(header_and_protobuf_vector);

    ASSERT_EQ(msg.type(), CP_protocol::Type::restart_tracker);
}


TEST_F(GivenACPNetworkMessage, AddingPayloadByCopiedVector)
{
    CP_protocol::Message msg { "192.168.2.1"_ipv4, 1 };

    EXPECT_EQ(msg.payload_size(), 0);
    EXPECT_EQ(msg.size(), 21);

    msg.append(payload_vector);
    ASSERT_EQ(msg.size(), 32);
    ASSERT_EQ(msg.payload_size(), 11);

    auto health = msg.view_as<CP_protocol::Health>();

    string result { health->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, AddingPayloadByMovedVector)
{
    vector<uint8_t> input_vector { payload_vector };
    CP_protocol::Message msg { "192.168.2.1"_ipv4, 1 };

    ASSERT_EQ(msg.payload_size(), 0);
    ASSERT_EQ(msg.size(), 21);
    ASSERT_EQ(input_vector.size(), 11);

    msg.append(move(input_vector));
    ASSERT_EQ(msg.size(), 32);
    ASSERT_EQ(msg.payload_size(), 11);

    ASSERT_EQ(input_vector.size(), 0);

    auto health = msg.view_as<CP_protocol::Health>();

    string result { health->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, AddingPayloadByCopiedString)
{
    CP_protocol::Message msg { };

    EXPECT_EQ(msg.payload_size(), 0);
    EXPECT_EQ(msg.size(), 21);

    msg.append(payload_string);
    ASSERT_EQ(msg.payload_size(), 11);

    auto health = msg.view_as<CP_protocol::Health>();

    string result { health->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, AddingPayloadByMovedString)
{
    CP_protocol::Message msg { };
    string input { payload_string };

    EXPECT_EQ(msg.payload_size(), 0);
    EXPECT_EQ(msg.size(), 21);

    msg.append(move(input));
    ASSERT_EQ(msg.payload_size(), 11);
    ASSERT_EQ(input.size(), 0);

    auto health = msg.view_as<CP_protocol::Health>();

    string result { health->to_string() };
    ASSERT_EQ(result, payload_string);
}



TEST_F(GivenACPNetworkMessage, AddingHeaderByCopy)
{
    CP_protocol::Message msg { };

    CP_protocol::Set_navigation_threshold set_threshold { };
    set_threshold.threshold(0xCAFE);

    msg.append(set_threshold);

    ASSERT_EQ(msg.size(), 23);
    ASSERT_EQ(msg.payload_size(), 2);
    
    auto set_threshold_view = msg.view_as<CP_protocol::Set_navigation_threshold>();
    ASSERT_EQ(set_threshold_view->threshold(), 0xCAFE);
}



TEST_F(GivenACPNetworkMessage, AddingHeaderThenProtobuf)
{
    CP_protocol::Message msg { };

    CP_protocol::Test_message config { };
    config.rotation_speed(4000);
    config.range_in_bins(2856);
    config.bin_size(1752);
    config.azimuth_samples(400);
    config.range_offset(1.25f);

    msg.append(config);
    
    ASSERT_EQ(msg.size(), 41);
    ASSERT_EQ(msg.payload_size(), 20);
    
    msg.append(payload_vector);

    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(msg.payload_size(), 31);

    auto config_view = msg.view_as<CP_protocol::Test_message>();

    ASSERT_EQ(config_view->rotation_speed(), 4000);
    ASSERT_EQ(config_view->range_in_bins(), 2856);
    ASSERT_EQ(config_view->bin_size(), 1752);
    ASSERT_EQ(config_view->azimuth_samples(), 400);
    ASSERT_FLOAT_EQ(config_view->range_offset(), 1.25f);

    string result { config_view->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, AddingProtobufThenHeader)
{
    CP_protocol::Message msg { };

    msg.append(payload_vector);

    ASSERT_EQ(msg.size(), 32);
    ASSERT_EQ(msg.payload_size(), 11);

    CP_protocol::Test_message config { };
    config.rotation_speed(4000);
    config.range_in_bins(2856);
    config.bin_size(1752);
    config.azimuth_samples(400);
    config.range_offset(1.25f);

    msg.append(config);
    
    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(msg.payload_size(), 31);
    
    auto config_view = msg.view_as<CP_protocol::Test_message>();

    ASSERT_EQ(config_view->rotation_speed(), 4000);
    ASSERT_EQ(config_view->range_in_bins(), 2856);
    ASSERT_EQ(config_view->bin_size(), 1752);
    ASSERT_EQ(config_view->azimuth_samples(), 400);
    ASSERT_FLOAT_EQ(config_view->range_offset(), 1.25f);

    string result { config_view->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, RelinquishEmptiesMessage)
{
    CP_protocol::Message msg { header_and_protobuf_vector };

    ASSERT_EQ(msg.type(), CP_protocol::Type::restart_tracker);
    ASSERT_EQ(msg.size(), 52);

    vector<uint8_t> result { msg.relinquish() };
    
    ASSERT_EQ(result.size(), 52);
    ASSERT_EQ(result, header_and_protobuf_vector);
    ASSERT_EQ(msg.size(), 0);
}


TEST_F(GivenACPNetworkMessage, CopyIsSupported)
{
    CP_protocol::Message msg { header_and_protobuf_vector };
    CP_protocol::Message result_msg { msg };

    ASSERT_EQ(msg.size(), 52);
    ASSERT_EQ(result_msg.size(), 52);

    auto config = result_msg.view_as<CP_protocol::Test_message>();

    string result { config->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, MoveIsSupported)
{
    CP_protocol::Message msg { header_and_protobuf_vector };
    CP_protocol::Message result_msg { move(msg) };

    ASSERT_EQ(msg.size(), 0);
    ASSERT_EQ(result_msg.size(), 52);

    auto config = result_msg.view_as<CP_protocol::Test_message>();

    string result { config->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, ConstMessageCanBeReadAsString)
{
    const CP_protocol::Message msg { header_and_protobuf_vector };

    auto config = msg.view_as<const CP_protocol::Test_message>();

    ASSERT_TRUE(msg.is_valid());

    string result { config->to_string() };
    ASSERT_EQ(result, payload_string);
}


TEST_F(GivenACPNetworkMessage, ConstMessageCanBeReadAsVector)
{
    const CP_protocol::Message msg { header_and_protobuf_vector };

    auto config = msg.view_as<const CP_protocol::Test_message>();

    ASSERT_TRUE(msg.is_valid());

    vector<uint8_t> result { config->to_vector() };
    ASSERT_EQ(result, payload_vector);
}


TEST_F(GivenACPNetworkMessage, AppendConcatenatesPayload)
{
    CP_protocol::Message msg { };

    msg.append("Hello "s);
    msg.append("world"s);

    ASSERT_EQ(msg.payload_size(), 11);

    auto config_view = msg.view_as<CP_protocol::Configuration>();

    string result { config_view->to_string() };
    ASSERT_EQ(result, "Hello world"s);
}


TEST_F(GivenACPNetworkMessage, IterationOverSignatureBehavesCorrectly)
{
    CP_protocol::Signature signature { };

    std::uint16_t count { 0 };

    for (auto i [[maybe_unused]]: signature) ++count;

    ASSERT_EQ(count, 16);
}