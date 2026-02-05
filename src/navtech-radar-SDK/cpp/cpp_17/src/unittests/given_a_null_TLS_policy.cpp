#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks.h"

#include "TLS_traits.h"
#include "pointer_types.h"

using namespace std;
using namespace Navtech;

using Null_TLS_services = TLS::TLS_traits<TLS::Policy::none>::TLS_services;
using Null_TLS_engine   = TLS::TLS_traits<TLS::Policy::none>::TLS_engine;

class GivenANullTLSPolicy : public ::testing::Test {
protected:
    GivenANullTLSPolicy() = default;

    MockCoreServices core_services { };
};


TEST_F(GivenANullTLSPolicy, TLSComponentsCanBeConstructed)
{
    Null_TLS_services tls_services { core_services };
    Null_TLS_engine   tls_engine   { tls_services };
}


TEST_F(GivenANullTLSPolicy, TLSEngineReportsStatus)
{
    Null_TLS_services tls_services { core_services };
    Null_TLS_engine   tls_engine   { tls_services };

    string result { };
    tls_engine.status.on_receive([&result](Network::Status& status) { result = status.what(); });
    tls_engine.open();

    ASSERT_EQ(result, "TLS is open.");

    tls_engine.close();

    ASSERT_EQ(result, "TLS is closed.");
}


TEST_F(GivenANullTLSPolicy, TLSEnginePortsForwardEncryptionData)
{
    Null_TLS_services tls_services { core_services };
    Null_TLS_engine   tls_engine   { tls_services };

    Network::Message_buffer in { 
        0x00, 0x01, 0x03, 0x03, 0x09, 0x09, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x6F, 0x6F, 0x8F, 0x8F, 0xFE, 0xFE
    };

    string in_str { begin(in), end(in) };

    Network::Message_buffer out { };

    tls_engine.encrypt_out.on_receive([&out](Null_TLS_engine::Outgoing_type& msg) { out = move(msg); });

    tls_engine.open();
    tls_engine.encrypt_in.post(move(in));
    string out_str { begin(out), end(out) };

    ASSERT_FALSE(out.empty());
    ASSERT_TRUE(in.empty());
    ASSERT_EQ(in_str, out_str);
}


TEST_F(GivenANullTLSPolicy, TLSEnginePortsForwardDecryptionData)
{
    Null_TLS_services tls_services { core_services };
    Null_TLS_engine   tls_engine   { tls_services };

    Network::Message_buffer in { 
        0x00, 0x01, 0x03, 0x03, 0x09, 0x09, 0x0F, 0x0F, 
        0x1F, 0x1F, 0x6F, 0x6F, 0x8F, 0x8F, 0xFE, 0xFE
    };

    string in_str { begin(in), end(in) };

    Network::Message_buffer out { };

    tls_engine.decrypt_out.on_receive([&out](Network::Message_buffer& msg) { out = move(msg); });

    tls_engine.open();
    tls_engine.decrypt_in.post(move(in));
    string out_str { begin(out), end(out) };

    ASSERT_FALSE(out.empty());
    ASSERT_TRUE(in.empty());
    ASSERT_EQ(in_str, out_str);
}