#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "IP_address.h"

using namespace Navtech;
using namespace Navtech::Networking;
using namespace std;



class GivenAnIPAddress : public ::testing::Test {
protected:
    GivenAnIPAddress() = default;
};


TEST_F(GivenAnIPAddress, IPAddressIs32Bits)
{
    ASSERT_EQ(sizeof(IP_address), 4);
}


TEST_F(GivenAnIPAddress, DefaultConstructionGivesInvalidAddress)
{
    IP_address addr { };
    ASSERT_EQ(addr, 0x00u);
    ASSERT_EQ(addr.to_string(), "0.0.0.0"s);
}


TEST_F(GivenAnIPAddress, LocalHostIsCorrect)
{
    ASSERT_EQ(IP_address::localhost().to_string(), "127.0.0.1");
}


TEST_F(GivenAnIPAddress, CanBeConstructedFromAStringLiteral)
{
    IP_address addr { "192.168.2.1" };
    ASSERT_EQ(addr, 0xC0A80201);
}


TEST_F(GivenAnIPAddress, CanBeConstructedFromAStdString)
{
    std::string addr_str { "192.168.2.1" };
    IP_address addr { addr_str  };
    ASSERT_EQ(addr, 0xC0A80201);
}


TEST_F(GivenAnIPAddress, EndiannessCanBeSpecifiedOnConstruction)
{
    IP_address addr1 { 0x0102A8C0, Endian::network };
    IP_address addr2 { 0xC0A80201, Endian::host };

    ASSERT_EQ(addr1.to_string(), "192.168.2.1");
    ASSERT_EQ(addr2.to_string(), "192.168.2.1");
}


TEST_F(GivenAnIPAddress, StringOutputIsCorrect)
{
    IP_address addr { };
    ASSERT_STREQ(addr.to_string().c_str(), "0.0.0.0");
}


TEST_F(GivenAnIPAddress, CanAssignAWord)
{
    IP_address addr { };
    addr = 0xC0A80201;
    ASSERT_EQ(addr, 0xC0A80201);
}


TEST_F(GivenAnIPAddress, CanAssignAString)
{
    IP_address addr { };
    addr = "192.168.2.1";
    ASSERT_EQ(addr, 0xC0A80201);
}


TEST_F(GivenAnIPAddress, InvalidIPAddressThrows)
{
    IP_address addr { };
    
    ASSERT_THROW((addr = "30.168.1.255.1"), std::out_of_range);
    ASSERT_THROW((addr = "127.1"),          std::out_of_range);
    ASSERT_THROW((addr = "192.168.1.256"),  std::out_of_range);
    ASSERT_THROW((addr = "-1.2.3.4"),       std::out_of_range);
    ASSERT_THROW((addr = "3...3"),          std::out_of_range);
}


TEST_F(GivenAnIPAddress, ValidIPAddressDoesntThrow)
{
    IP_address addr { };

    ASSERT_NO_THROW(addr = "127.0.0.1");
    ASSERT_NO_THROW(addr = "192.168.1.1");
    ASSERT_NO_THROW(addr = "192.168.1.255");
    ASSERT_NO_THROW(addr = "0.0.0.0");
    ASSERT_NO_THROW(addr = "1.1.1.01");
}


TEST_F(GivenAnIPAddress, OctalValuesAreAccepted)
{
    IP_address addr { };

    ASSERT_NO_THROW(addr = "192.168.1.010");
    ASSERT_EQ(addr.to_string(), "192.168.1.8");
}


TEST_F(GivenAnIPAddress, AnIPCanBeConstructedFromAnInt)
{
    IP_address addr { 0xC0A80201 };

    ASSERT_EQ(addr.to_string(), "192.168.2.1"s);
}


TEST_F(GivenAnIPAddress, CanBeConstructedFromLiterals)
{
    auto addr1 = 0xC0A80201_ipv4;
    auto addr2 = "192.168.2.1"_ipv4;

    ASSERT_EQ(addr1.to_string(), "192.168.2.1"s);
    ASSERT_EQ(addr2.to_string(), "192.168.2.1"s);
}


TEST_F(GivenAnIPAddress, EndiannessIsCorrectForHostAndNetwork)
{
    IP_address addr { 0xC0A80201 };

    ASSERT_EQ(addr.to_host_endian(), 0xC0A80201);
    ASSERT_EQ(addr.to_network_endian(), 0x0102A8C0);

    ASSERT_EQ(addr.value_as(Endian::host), 0xC0A80201);
    ASSERT_EQ(addr.value_as(Endian::network), 0x0102A8C0);
}


TEST_F(GivenAnIPAddress, OperatorNotIsCorrect)
{
    IP_address addr { "255.255.255.0" };

    ASSERT_EQ(~addr, 0x000000FFu);
}


TEST_F(GivenAnIPAddress, AndOperatorsAreCorrect)
{
    IP_address addr   { "192.168.2.1" };
    IP_address subnet { "255.255.255.0" }; 

    auto result1 = addr & subnet;

    ASSERT_EQ(addr.to_string(), "192.168.2.1"s);
    ASSERT_EQ(result1.to_string(), "192.168.2.0"s);

    auto result2 = addr & 0xFFFFFF00;

    ASSERT_EQ(addr.to_string(), "192.168.2.1"s);
    ASSERT_EQ(result2.to_string(), "192.168.2.0"s);

    auto network_addr = 0xFFFFFF00 & addr;

    ASSERT_EQ(addr.to_string(), "192.168.2.1"s);
    ASSERT_EQ(network_addr.to_string(), "192.168.2.0"s);

    auto host_addr = addr & ~subnet;

    ASSERT_EQ(addr.to_string(), "192.168.2.1"s);
    ASSERT_EQ(host_addr.to_string(), "0.0.0.1"s);

    addr &= ~subnet;
    ASSERT_EQ(addr.to_string(), "0.0.0.1"s);
}


TEST_F(GivenAnIPAddress, OrOperatorsAreCorrect)
{
    IP_address network_addr { "192.168.2.0" };
    IP_address host_addr    { "0.0.0.1" }; 

    auto full_addr = network_addr | host_addr;

    ASSERT_EQ(host_addr.to_string(), "0.0.0.1"s);
    ASSERT_EQ(network_addr.to_string(), "192.168.2.0"s);
    ASSERT_EQ(full_addr.to_string(), "192.168.2.1"s);

    network_addr |= host_addr;
    ASSERT_EQ(network_addr.to_string(), "192.168.2.1"s);
}
