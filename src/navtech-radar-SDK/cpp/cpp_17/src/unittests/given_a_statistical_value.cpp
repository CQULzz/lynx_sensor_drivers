#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include "Statistical_value.h"

using namespace std;
using namespace Navtech::Utility;

// Type aliases to clean-up code
//
class GivenAStatisticalValue : public ::testing::Test {
protected:
    array<float, 5> test_floats { 2.0f, 3.0f, 7.0f, 4.0f, 1.0f };
    array<int, 5>   test_ints   { 2, 3, 7, 4, 1 };
};


// -------------------------------------------------------------------
// Single float value tests
//
TEST_F(GivenAStatisticalValue, AFloatValueCanBeDefaultConstructed)
{
    Statistical_value<float> val { };

    ASSERT_FLOAT_EQ(val.latest(), 0.0f);
}


TEST_F(GivenAStatisticalValue, AFloatValueCanBeInitialised)
{
    Statistical_value<float> val { 17.6f };

    ASSERT_FLOAT_EQ(val.latest(), 17.6f);
}


TEST_F(GivenAStatisticalValue, ByDefaultLatestFloatValueIsReturned)
{
    Statistical_value<float> val { 10.3f };

    ASSERT_FLOAT_EQ(val, 10.3f);
}


TEST_F(GivenAStatisticalValue, FloatAssignmentAndUpdateAreTheSame)
{
    Statistical_value<float> val1 { };
    Statistical_value<float> val2 { };

    val1.update(10.3f);
    val2 = 10.3f;

    ASSERT_FLOAT_EQ(val1, 10.3F);
    ASSERT_FLOAT_EQ(val2, 10.3F);
}


TEST_F(GivenAStatisticalValue, MinAndMaxFloatAreUpdatedOnInitialisation)
{
    Statistical_value<float> val { };

    ASSERT_FLOAT_EQ(val.min(), 0.0F);
    ASSERT_FLOAT_EQ(val.max(), 0.0F);

    val = 10.3f;

    ASSERT_FLOAT_EQ(val.min(), 10.3f);
    ASSERT_FLOAT_EQ(val.max(), 10.3f);
}


TEST_F(GivenAStatisticalValue, MinAndMaxFloatAreUpdatedWithUpdates)
{
    Statistical_value<float> val { };

    ASSERT_FLOAT_EQ(val.min(), 0.0f);
    ASSERT_FLOAT_EQ(val.max(), 0.0f);

    val = 10.3f;

    ASSERT_FLOAT_EQ(val.min(), 10.3f);
    ASSERT_FLOAT_EQ(val.max(), 10.3f);

    val = 4.5f;
    val = 6.6f;

    ASSERT_FLOAT_EQ(val.latest(), 6.6f);
    ASSERT_FLOAT_EQ(val.min(), 4.5f);
    ASSERT_FLOAT_EQ(val.max(), 10.3f);
}


TEST_F(GivenAStatisticalValue, FloatDeltaIsUpdatedWithNewValues)
{
    Statistical_value<float> val { 10.0f };

    ASSERT_FLOAT_EQ(val.delta(), 0.0f);

    val = 12.5f;

    ASSERT_FLOAT_EQ(val.delta(), 2.5f);
}


TEST_F(GivenAStatisticalValue, FloatMeanIsUpdatedWithNewValues)
{
    Statistical_value<float> val { };

    ASSERT_FLOAT_EQ(val.mean(), 0.0f);

    for (float f : test_floats) val = f;

    ASSERT_FLOAT_EQ(val.mean(), 2.5f);
}


TEST_F(GivenAStatisticalValue, FloatVarianceIsUpdatedWithNewValues)
{
    Statistical_value<float> val { };

    ASSERT_FLOAT_EQ(val.mean(), 0.0f);

    for (float f : test_floats) val = f;

    ASSERT_FLOAT_EQ(val.variance(), 2.25f);
}


TEST_F(GivenAStatisticalValue, FloatStandardDeviationIsUpdatedWithNewValues)
{
    Statistical_value<float> val { };

    ASSERT_FLOAT_EQ(val.mean(), 0.0f);

    for (float f : test_floats) val = f;

    ASSERT_FLOAT_EQ(val.std_deviation(), 1.5f);
}

// -------------------------------------------------------------------
// Single integer value tests
//

TEST_F(GivenAStatisticalValue, AnIntegerValueCanBeDefaultConstructed)
{
    Statistical_value<int> val { };

    ASSERT_EQ(val.latest(), 0);
}


TEST_F(GivenAStatisticalValue, AnIntegerValueCanBeInitialised)
{
    Statistical_value<int> val { 17 };

    ASSERT_EQ(val.latest(), 17);
}


TEST_F(GivenAStatisticalValue, ByDefaultLatestIntegerValueIsReturned)
{
    Statistical_value<int> val { 10 };

    ASSERT_EQ(val, 10);
}


TEST_F(GivenAStatisticalValue, IntegerAssignmentAndUpdateAreTheSame)
{
    Statistical_value<int> val1 { };
    Statistical_value<int> val2 { };

    val1.update(10);
    val2 = 10;

    ASSERT_EQ(val1, 10);
    ASSERT_EQ(val2, 10);
}


TEST_F(GivenAStatisticalValue, MinAndMaxIntegerAreUpdatedOnInitialisation)
{
    Statistical_value<int> val { };

    ASSERT_EQ(val.min(), 0);
    ASSERT_EQ(val.max(), 0);

    val = 10;

    ASSERT_EQ(val.min(), 10);
    ASSERT_EQ(val.max(), 10);
}


TEST_F(GivenAStatisticalValue, MinAndMaxIntegerAreUpdatedWithUpdates)
{
    Statistical_value<int> val { };

    ASSERT_EQ(val.min(), 0);
    ASSERT_EQ(val.max(), 0);

    val = 10;

    ASSERT_EQ(val.min(), 10);
    ASSERT_EQ(val.max(), 10);

    val = 4;
    val = 6;

    ASSERT_EQ(val.latest(), 6);
    ASSERT_EQ(val.min(), 4);
    ASSERT_EQ(val.max(), 10);
}


TEST_F(GivenAStatisticalValue, IntegerDeltaIsUpdatedWithNewValues)
{
    Statistical_value<int> val { 10 };

    ASSERT_EQ(val.delta(), 0);

    val = 12;

    ASSERT_EQ(val.delta(), 2);
}


TEST_F(GivenAStatisticalValue, IntegerMeanIsUpdatedWithNewValues)
{
    Statistical_value<int> val { };

    ASSERT_FLOAT_EQ(val.mean(), 0.0f);

    for (auto new_val : test_ints) val = new_val;

    ASSERT_FLOAT_EQ(val.mean(), 2.5f);
}


TEST_F(GivenAStatisticalValue, IntegerVarianceIsUpdatedWithNewValues)
{
    Statistical_value<int> val { };

    ASSERT_EQ(val.mean(), 0);

    for (auto new_val : test_ints) val = new_val;

    ASSERT_FLOAT_EQ(val.variance(), 2.25f);
}


TEST_F(GivenAStatisticalValue, IntegerStandardDeviationIsUpdatedWithNewValues)
{
    Statistical_value<int> val { };

    ASSERT_EQ(val.mean(), 0);

    for (auto new_val : test_ints) val = new_val;

    ASSERT_FLOAT_EQ(val.std_deviation(), 1.5f);
}


// -------------------------------------------------------------------
// Multiple float value tests
//
TEST_F(GivenAStatisticalValue, AMultiFloatValueCanBeDefaultConstructed)
{
    Statistical_value<float, 5> val { };

    ASSERT_FLOAT_EQ(val.latest(), 0.0f);
}


TEST_F(GivenAStatisticalValue, AMultiFloatValueCanBeInitialised)
{
    Statistical_value<float, 5> val { 17.6f };

    ASSERT_FLOAT_EQ(val.latest(), 17.6f);
}


TEST_F(GivenAStatisticalValue, ByDefaultLatestMultiFloatValueIsReturned)
{
    Statistical_value<float, 5> val { 10.3f };

    ASSERT_FLOAT_EQ(val, 10.3f);
}


TEST_F(GivenAStatisticalValue, MultiFloatAssignmentAndUpdateAreTheSame)
{
    Statistical_value<float, 5> val1 { };
    Statistical_value<float, 5> val2 { };

    val1.update(10.3f);
    val2 = 10.3f;

    ASSERT_FLOAT_EQ(val1, 10.3f);
    ASSERT_FLOAT_EQ(val2, 10.3f);
}


TEST_F(GivenAStatisticalValue, MinAndMaxMultiFloatAreUpdatedOnInitialisation)
{
    Statistical_value<float, 5> val { };

    ASSERT_FLOAT_EQ(val.min(), 0.0f);
    ASSERT_FLOAT_EQ(val.max(), 0.0f);

    val = 10.3f;

    ASSERT_FLOAT_EQ(val.min(), 10.3f);
    ASSERT_FLOAT_EQ(val.max(), 10.3f);
}


TEST_F(GivenAStatisticalValue, MinAndMaxMultiFloatAreUpdatedWithUpdates)
{
    Statistical_value<float, 5> val { };

    ASSERT_FLOAT_EQ(val.min(), 0.0f);
    ASSERT_FLOAT_EQ(val.max(), 0.0f);

    val = 10.3f;

    ASSERT_FLOAT_EQ(val.min(), 10.3f);
    ASSERT_FLOAT_EQ(val.max(), 10.3f);

    val = 4.5f;
    val = 6.6f;

    ASSERT_FLOAT_EQ(val.latest(), 6.6f);
    ASSERT_FLOAT_EQ(val.min(), 4.5f);
    ASSERT_FLOAT_EQ(val.max(), 10.3f);
}


TEST_F(GivenAStatisticalValue, MultiFloatDeltaIsUpdatedWithNewValues)
{
    Statistical_value<float, 5> val { 10.0 };

    ASSERT_FLOAT_EQ(val.delta(), 10.0f);

    val = 12.5f;

    ASSERT_FLOAT_EQ(val.delta(), 2.5f);
}


TEST_F(GivenAStatisticalValue, MultiFloatMeanIsUpdatedWithNewValues)
{
    Statistical_value<int, 5> val { };

    ASSERT_FLOAT_EQ(val.mean(), 0.0f);

    for (auto new_val : test_ints) val = new_val;

    ASSERT_FLOAT_EQ(val.mean(), 3.4f);
}


TEST_F(GivenAStatisticalValue, MultiFloatVarianceIsUpdatedWithNewValues)
{
    Statistical_value<int, 5> val { };

    ASSERT_FLOAT_EQ(val.mean(), 0.0f);

    for (auto new_val : test_ints) val = new_val;

    ASSERT_FLOAT_EQ(val.variance(), 4.24f);
}


TEST_F(GivenAStatisticalValue, MultiFloatStandardDeviationIsUpdatedWithNewValues)
{
    Statistical_value<int, 5> val { };

    ASSERT_FLOAT_EQ(val.mean(), 0.0f);

    for (auto new_val : test_ints) val = new_val;

    ASSERT_FLOAT_EQ(val.std_deviation(), 2.059126f);
}


// -------------------------------------------------------------------
// Multiple int value tests
//
TEST_F(GivenAStatisticalValue, MultipleIntsWillGenerateCorrectMean)
{
    Statistical_value<int, 5> val { };

    for (auto new_val : test_ints) val = new_val;

    ASSERT_FLOAT_EQ(val.mean(), 3.4f);
}


// -------------------------------------------------------------------
// Overflow tests
//
TEST_F(GivenAStatisticalValue, LargeIntegerValuesWillNotOverflow)
{
    Statistical_value<uint8_t, 5> val_8 { };
    Statistical_value<int16_t, 5> val_16 { };

    val_8.update(101);
    val_8.update(102);
    val_8.update(103);
    val_8.update(104);
    val_8.update(105);

    val_16.update(32001);
    val_16.update(32002);
    val_16.update(32003);
    val_16.update(32004);
    val_16.update(32005);

    ASSERT_FLOAT_EQ(val_8.mean(), 103.0f);
    ASSERT_FLOAT_EQ(val_16.mean(), 32003.0f);
}


TEST_F(GivenAStatisticalValue, LargeSingleIntegerValueWillNotOverflow)
{
    Statistical_value<uint8_t, 1> val { };

    val.update(253);
    val.update(254);

    ASSERT_FLOAT_EQ(val.mean(), 253.5f);
}


TEST_F(GivenAStatisticalValue, LargeSetOfIntegerValuesWillNotOverflow)
{
    Statistical_value<uint8_t, 100> val { };

    std::uint64_t total { };

    for (int i { 0 }; i < 100; ++i) {
        total += 255;
        val.update(255);
    }

    ASSERT_FLOAT_EQ(val.mean(), total / 100.0f);
}