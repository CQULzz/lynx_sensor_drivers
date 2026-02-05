#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private   public
#define protected public

#include "Fixed_point.h"

using namespace Navtech;
using namespace Navtech::Utility;
using namespace std;

class GivenFixedPointTypes : public ::testing::Test {
};


TEST_F(GivenFixedPointTypes, ConstructionFromFloat)
{
    Fixed<8, 1>  fixed_8  { 10.5f };
    Fixed<16, 8> fixed_16 { 10.5f };

    ASSERT_EQ(fixed_8.value, 21);
    ASSERT_EQ(fixed_16.value, 2688);
}


TEST_F(GivenFixedPointTypes, Rounding)
{
    Fixed<8, 1>  fixed_8  { 10.51f };
    Fixed<16, 8> fixed_16 { 10.51f };

    ASSERT_EQ(fixed_8.value, 21);
    ASSERT_EQ(fixed_16.value, 2690);

    ASSERT_FLOAT_EQ(fixed_8.to_float(), 10.5f);
    ASSERT_FLOAT_EQ(fixed_16.to_float(), 10.507812f);
}


TEST_F(GivenFixedPointTypes, Addition)
{
    Fixed<16, 8> fixed_a { 10.51f };
    Fixed<16, 8> fixed_b { 52.3f };

    ASSERT_FLOAT_EQ(fixed_a.to_float(), 10.507812f);
    ASSERT_FLOAT_EQ(fixed_b.to_float(), 52.296875f);

    auto result = fixed_a + fixed_b;

    ASSERT_FLOAT_EQ(result.to_float(), 62.804687f);

    fixed_a += fixed_b;
    ASSERT_FLOAT_EQ(fixed_a.to_float(), 62.804687f);
}


TEST_F(GivenFixedPointTypes, Subtraction)
{
    Fixed<16, 8> fixed_a { 10.51f };
    Fixed<16, 8> fixed_b { 52.3f };

    ASSERT_FLOAT_EQ(fixed_a.to_float(), 10.507812f);
    ASSERT_FLOAT_EQ(fixed_b.to_float(), 52.296875f);

    auto result = fixed_b - fixed_a;

    ASSERT_FLOAT_EQ(result.to_float(), 41.78906f);

    fixed_b -= fixed_a;
    ASSERT_FLOAT_EQ(fixed_b.to_float(), 41.78906f);
}


TEST_F(GivenFixedPointTypes, Multiply)
{
    Fixed<16, 8> fixed_a { 10.51f };
    Fixed<16, 8> fixed_b { 8.7f };

    ASSERT_FLOAT_EQ(fixed_a.to_float(), 10.507812f);
    ASSERT_FLOAT_EQ(fixed_b.to_float(), 8.699218f);

    auto result = fixed_b * fixed_a;

    ASSERT_FLOAT_EQ(result.to_float(), 91.40625f);

    fixed_a *= fixed_b;
    ASSERT_FLOAT_EQ(fixed_a.to_float(), 91.40625f);
}


TEST_F(GivenFixedPointTypes, Divide)
{
    Fixed<16, 8> fixed_a { 10.51f };
    Fixed<16, 8> fixed_b { 52.3f };

    ASSERT_FLOAT_EQ(fixed_a.to_float(), 10.507812f);
    ASSERT_FLOAT_EQ(fixed_b.to_float(), 52.296875f);

    auto result = fixed_b / fixed_a;

    ASSERT_FLOAT_EQ(result.to_float(), 4.9765625f);

    fixed_b /= fixed_a;
    ASSERT_FLOAT_EQ(fixed_b.to_float(), 4.9765625f);
}


TEST_F(GivenFixedPointTypes, Assignment)
{
    Fixed<16, 8> fixed_a { };

    ASSERT_FLOAT_EQ(fixed_a.to_float(), 0.0f);

    fixed_a = 10.51f;

    ASSERT_FLOAT_EQ(fixed_a.to_float(), 10.507812f);

    Fixed<16, 8> fixed_b { 52.3f };

    fixed_a = fixed_b;

    ASSERT_FLOAT_EQ(fixed_a.to_float(), 52.296875f);
}


TEST_F(GivenFixedPointTypes, ValueComparison)
{
    Fixed<16, 8> fixed_a { 10.51f };
    Fixed<16, 8> fixed_b { 52.3f };
    Fixed<16, 8> fixed_c { 10.51f };

    ASSERT_TRUE(fixed_a == fixed_c);
    ASSERT_TRUE(fixed_b > fixed_a);
    ASSERT_TRUE(fixed_a < fixed_b);
    ASSERT_TRUE(fixed_a <= fixed_c);

    ASSERT_FALSE(fixed_a == fixed_b);
    ASSERT_FALSE(fixed_a != fixed_c);
    ASSERT_FALSE(fixed_a > fixed_b);
    ASSERT_FALSE(fixed_a < fixed_c);
}


TEST_F(GivenFixedPointTypes, FixedPointConversion)
{
    Fixed<16, 8> fixed_a { 10.51f };
    Fixed<16, 4> fixed_b { 312.25f };

    auto result_a = fixed_a.to_fixed<Fixed<8, 1>>();

    ASSERT_FLOAT_EQ(result_a.to_float(), 10.5f);

    auto result_b = fixed_b.to_fixed<Fixed<8, 1>>();
    
    ASSERT_FLOAT_EQ(result_b.to_float(), 56.0f);
}