#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Port.h"


using namespace Navtech::Utility;
using namespace std;



class GivenAPort : public ::testing::Test {
protected:
    GivenAPort() = default;
};


TEST_F(GivenAPort, ForwardIfConnectedToAnotherPort)
{
    Port<int> port1  { };
    Port<int> port2  { };
    int       result { };

    port2.on_receive([&result](int& i) { result = i; });
    port1.forward_to(port2);
    port1.post(100);

    ASSERT_EQ(result, 100);
}


TEST_F(GivenAPort, CallbackIfNotConnected)
{
    Port<int> port1  { };
    int       result { };

    port1.on_receive([&result](int& i) { result = i; });
    port1.post(100);

    ASSERT_EQ(result, 100);
}


TEST_F(GivenAPort, DoNothingIfNotConnectedOrCallback)
{
    Port<int> port1  { };
    int       result { };

    port1.post(100);

    ASSERT_EQ(result, 0);
}


TEST_F(GivenAPort, ForwardInPreferenceToCallback)
{
    Port<int> port1  { };
    Port<int> port2  { };
    int       result { };

    port1.on_receive([&result](int& i) { result = i + 1; });
    port2.on_receive([&result](int& i) { result = i; });
    port1.forward_to(port2);
    port1.post(100);

    ASSERT_EQ(result, 100);
}


TEST_F(GivenAPort, PortsWillForwardThroughAChain)
{
    Port<int> port1  { };
    Port<int> port2  { };
    Port<int> port3  { };
    int       result { };

    port1.forward_to(port2);
    port2.forward_to(port3);
    port3.on_receive([&result](int& i) { result = i; });
    
    port1.post(100);

    ASSERT_EQ(result, 100);
}
