#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "net_conversion.h"

using namespace std;
using namespace Navtech::Networking;


class GivenNetConversion : public ::testing::Test {
protected:
    GivenNetConversion() = default;
};


TEST_F(GivenNetConversion, HostEndianUint16ToNetworkEndianUint16)
{
    uint16_t value  { 0xCAFE };
    uint16_t result { };

    result = to_uint16_network(value);

    ASSERT_EQ(result, 0xFECA);
}


TEST_F(GivenNetConversion, NetworkEndianUint16ToHostEndianUint16)
{
    uint16_t value  { 0xFECA };
    uint16_t result { };

    result = to_uint16_host(value);

    ASSERT_EQ(result, 0xCAFE);
}


TEST_F(GivenNetConversion, FloatToNetworkEndianUint32)
{
    float    value  { 17.6f };
    uint32_t result { };

    result = to_uint32_network(value);
    ASSERT_EQ(result, 0xCDCC8C41);
}


TEST_F(GivenNetConversion, FloatToHostEndianUint32)
{
    float    value  { 17.6f };
    uint32_t result { };

    result = to_uint32_host(value);
    ASSERT_EQ(result, 0x418CCCCD);
}


TEST_F(GivenNetConversion, HostEndianUint32ToHostEndianFloat)
{
    uint32_t value  { 0x418CCCCD };
    float    result { };

    result = from_uint32_host(value);
    EXPECT_FLOAT_EQ(result, 17.6f);
}


TEST_F(GivenNetConversion, NetworkEndianUint32ToHostEndianFloat)
{
    uint32_t value  { 0xCDCC8C41 };
    float    result { };

    result = from_uint32_network(value);
    EXPECT_FLOAT_EQ(result, 17.6f);
}


TEST_F(GivenNetConversion, NetworkEndianUint32ToHostEndianUint32)
{
    uint32_t value  { 0xBEBAFECA };
    uint32_t result { };

    result = to_uint32_host(value);

    ASSERT_EQ(result, 0xCAFEBABE);
}
    

TEST_F(GivenNetConversion, HostEndianUint32ToNetworkEndianUInt32)
{
    uint32_t value  { 0xCAFEBABE};
    uint32_t result { };

    result = to_uint32_network(value);

    ASSERT_EQ(result, 0xBEBAFECA);
}


TEST_F(GivenNetConversion, DoubleToHostEndianUint64)
{
    double   value  { 17.6 };
    uint64_t result { };

    result = to_uint64_host(value);

    ASSERT_EQ(result, 0x4031'9999'9999'999A);
}


TEST_F(GivenNetConversion, DoubleToNetworkEndianUint64)
{
    double   value  { 17.6 };
    uint64_t result { };

    result = to_uint64_network(value);

    ASSERT_EQ(result, 0x9999'3140'9A99'9999);
}


TEST_F(GivenNetConversion, Uint64ToDouble)
{
    uint64_t value  { 0x4031'9999'9999'999A };
    double   result { };

    result = from_uint64_host(value);

    ASSERT_DOUBLE_EQ(result, 17.6);
}


TEST_F(GivenNetConversion, NetworkEndianUint64ToHostEndianDouble)
{
    uint64_t value  { 0x9999'3140'9A99'9999 };
    double   result { };

    result = from_uint64_network(value);

    ASSERT_DOUBLE_EQ(result, 17.600000);
}


TEST_F(GivenNetConversion, Uint32ToArray)
{
    uint32_t     value  { 0xDEADBEEF };
    Byte_array_4 result { };

    result = to_byte_array(value);

    ASSERT_EQ(result[0], 0xEF);
    ASSERT_EQ(result[1], 0xBE);
    ASSERT_EQ(result[2], 0xAD);
    ASSERT_EQ(result[3], 0xDE);
}


TEST_F(GivenNetConversion, Uint16ToArray)
{
    uint16_t     value  { 0xDEAD };
    Byte_array_2 result { };

    result = to_byte_array(value);

    ASSERT_EQ(result[0], 0xAD);
    ASSERT_EQ(result[1], 0xDE);
}


TEST_F(GivenNetConversion, ArrayToUint32)
{
    Byte_array_4 value  { 0xEF, 0xBE, 0xAD, 0xDE };
    uint32_t     result { };

    result = from_byte_array(value);

    ASSERT_EQ(result, 0xDEADBEEF);
}


TEST_F(GivenNetConversion, ArrayToUint16)
{
    Byte_array_2 value  { 0xAD, 0xDE };
    uint16_t     result { };

    result = from_byte_array(value);

    ASSERT_EQ(result, 0xDEAD);
}


TEST_F(GivenNetConversion, ScalarTypeToVector)
{
    uint16_t u16 { 0xCAFE };
    uint32_t u32 { 0xC0FFEE };
    float    f   { 17.6f };
    double   d   { 17.6 };

    vector<uint8_t> result { };

    result = to_vector(u16);
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0], 0xFE);
    ASSERT_EQ(result[1], 0xCA);
    result.clear();

    result = to_vector(u32);
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0], 0xEE);
    ASSERT_EQ(result[1], 0xFF);
    ASSERT_EQ(result[2], 0xC0);
    ASSERT_EQ(result[3], 0x00);
    result.clear();

    result = to_vector(f);
    ASSERT_EQ(result.size(), 4);
    ASSERT_EQ(result[0], 0xCD);
    ASSERT_EQ(result[1], 0xCC);
    ASSERT_EQ(result[2], 0x8C);
    ASSERT_EQ(result[3], 0x41);
    result.clear();

    result = to_vector(d);
    ASSERT_EQ(result.size(), 8);
    ASSERT_EQ(result[0], 0x9A);
    ASSERT_EQ(result[1], 0x99);
    ASSERT_EQ(result[2], 0x99);
    ASSERT_EQ(result[3], 0x99);
    ASSERT_EQ(result[4], 0x99);
    ASSERT_EQ(result[5], 0x99);
    ASSERT_EQ(result[6], 0x31);
    ASSERT_EQ(result[7], 0x40);
    result.clear();
}


TEST_F(GivenNetConversion, VectorToScalarType)
{
    uint16_t u16 { };
    uint32_t u32 { };
    float    flt { };
    double   dbl { };

    vector<uint8_t> vec_u16 { 0xFE, 0xCA };
    vector<uint8_t> vec_u32 { 0xEE, 0xFF, 0xC0, 0x00 };
    vector<uint8_t> vec_flt { 0xCD, 0xCC, 0x8C, 0x41 };
    vector<uint8_t> vec_dbl { 0x9A, 0x99, 0x99, 0x99, 0x99, 0x99, 0x31, 0x40 };

    u16 = from_vector_to<uint16_t>(vec_u16).value();
    u32 = from_vector_to<uint32_t>(vec_u32).value();
    flt = from_vector_to<float>(vec_flt).value();
    dbl = from_vector_to<double>(vec_dbl).value();

    ASSERT_EQ(u16, 0xCAFE);
    ASSERT_EQ(u32, 0xC0FFEE);
    ASSERT_FLOAT_EQ(flt, 17.6f);
    ASSERT_DOUBLE_EQ(dbl, 17.6);
}


TEST_F(GivenNetConversion, NetworkEndianUint64ToHostEndianUint64)
{
    uint64_t value  { 0xDEADBEEF'BA5EBA11 };
    uint64_t result { };

    result = to_uint64_host(value);

    ASSERT_EQ(result, 0x11BA5EBA'EFBEADDE);
}


TEST_F(GivenNetConversion, HostEndianUint64ToNetworkEndianUint64)
{
    uint64_t value  { 0x11BA5EBA'EFBEADDE };
    uint64_t result { };

    result = to_uint64_network(value);

    ASSERT_EQ(result, 0xDEADBEEF'BA5EBA11);  
}
