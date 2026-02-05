#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "tls.pb.h"
#include "Protobuf_helpers.h"


using namespace TLS;
using namespace Navtech;
using namespace std;


class GivenProtobufHelpers : public ::testing::Test {
protected:
    GivenProtobufHelpers() = default;

    TLS_settings tls_settings { };

    void SetUp()
    {
        tls_settings.set_server_certificate("certificate");
        tls_settings.set_server_key("key");
        tls_settings.set_server_key_passphrase("passphrase");
    }
};


TEST_F(GivenProtobufHelpers, ConvertAVectorToAProtobuf)
{
    auto vec = Protobuf::vector_from(tls_settings);

    ASSERT_TRUE(vec.has_value());

    auto result = Protobuf::from_vector_into<TLS_settings>(vec.value());

    ASSERT_TRUE(result.has_value());
    
    auto result_protobuf { result.value() };
    ASSERT_EQ(result_protobuf.server_certificate(), "certificate"s);
    ASSERT_EQ(result_protobuf.server_key(), "key"s);
    ASSERT_EQ(result_protobuf.server_key_passphrase(), "passphrase"s);
}


TEST_F(GivenProtobufHelpers, ConvertABinaryStringToAProtobuf)
{
    auto str = Protobuf::string_from(tls_settings);

    ASSERT_TRUE(str.has_value());

    auto result = Protobuf::from_string_into<TLS_settings>(str.value());

    ASSERT_TRUE(result.has_value());
    
    auto result_protobuf { result.value() };
    ASSERT_EQ(result_protobuf.server_certificate(), "certificate"s);
    ASSERT_EQ(result_protobuf.server_key(), "key"s);
    ASSERT_EQ(result_protobuf.server_key_passphrase(), "passphrase"s);
}


TEST_F(GivenProtobufHelpers, AVectorCanBeConvertedIntoASharedOwnerProtobuf)
{
    auto vec = Protobuf::vector_from(tls_settings);

    ASSERT_TRUE(vec.has_value());

    auto result = Protobuf::from_vector_into_shared<TLS_settings>(vec.value());

    ASSERT_TRUE(result != nullptr);

    ASSERT_EQ(result->server_certificate(), "certificate"s);
    ASSERT_EQ(result->server_key(), "key"s);
    ASSERT_EQ(result->server_key_passphrase(), "passphrase"s);
}
