#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Bit_ops.h"

using namespace std;
using namespace Navtech::Utility;


class GivenBitOps : public ::testing::Test {
protected:
    GivenBitOps() = default;
};

// bit
//
TEST_F(GivenBitOps, BitZeroIsFirstBit)
{
    ASSERT_EQ(bit(0), 1);
}


TEST_F(GivenBitOps, NEqualsNumberOfBitsMinusOne)
{
    ASSERT_EQ(bit(31), 2147483648);
}


TEST_F(GivenBitOps, NGreaterThanNumberOfBitsOverflows)
{
    ASSERT_EQ(bit(32), 0);
}


// bit range
//

TEST_F(GivenBitOps, BitRangeIsInclusiveSet)
{
    ASSERT_EQ(bit_range(0, 1), 3);
}


TEST_F(GivenBitOps, StartAndEndTheSameSetsOneBit)
{
    ASSERT_EQ(bit_range(0, 0), 1);
}


TEST_F(GivenBitOps, EndGreaterThanNumberOfBitsSetsUpToHighestBit)
{
    ASSERT_EQ(bit_range(0, 64), 0xFFFFFFFF);
}


TEST_F(GivenBitOps, StartGreaterThanEndSetsRangeCorrectly)
{
    ASSERT_EQ(bit_range(3, 0), 15);
}


// is set
//
TEST_F(GivenBitOps, ASetBitYieldsTrue)
{
    uint32_t value { bit(12) };
    ASSERT_TRUE(is_set(value, 12));
}


TEST_F(GivenBitOps, AnUnsetBitYieldsFalse)
{
    uint32_t value { bit(12) };
    ASSERT_FALSE(is_set(value, 13));
}



TEST_F(GivenBitOps, AnOutOfRangeBitYieldsFalse)
{
    uint32_t value { bit(12) };
    ASSERT_FALSE(is_set(value, 42));
}