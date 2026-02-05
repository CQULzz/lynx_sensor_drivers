#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "constants.h"
#include "Degrees.h"
#include "Radians.h"


using namespace Navtech::Unit;
using Navtech::pi;

class GivenAnAngle : public ::testing::Test {
protected:
};


TEST_F(GivenAnAngle, AnAngleCanBeDefaultConstructed)
{
    Degrees angle { };

    ASSERT_FLOAT_EQ(angle.to_float(), 0.0f);
}


TEST_F(GivenAnAngle, AnAngleCanBeNonDefaultConstructed)
{
    Degrees angle { 17.5f };

    ASSERT_FLOAT_EQ(angle.to_float(), 17.5f);
}


TEST_F(GivenAnAngle, AnglesGreaterThan360AreNormalised)
{
    Degrees one_rotation                { 360 };
    Degrees two_rotations               { 720 };
    Degrees one_and_a_half_rotations    { 540 };

    ASSERT_FLOAT_EQ(one_rotation.to_float(), 0.0f);
    ASSERT_FLOAT_EQ(two_rotations.to_float(), 0.0f);
    ASSERT_FLOAT_EQ(one_and_a_half_rotations.to_float(), 180.0f);
}


TEST_F(GivenAnAngle, UserDefinedLiteralsConstructCorrectAngles)
{
    auto angle1 = 45.5_deg;
    auto angle2 = 90_deg;

    ASSERT_FLOAT_EQ(angle1.to_float(), 45.5f);
    ASSERT_FLOAT_EQ(angle2.to_float(), 90.0f);

    auto rad1 = 0.785398_rad;
    auto rad2 = 1.5708_rad;

    ASSERT_FLOAT_EQ(rad1.to_float(), 0.785398f);
    ASSERT_FLOAT_EQ(rad2.to_float(), 1.5708f);
}


TEST_F(GivenAnAngle, AnglesWithSumLessThan360Add)
{
    auto angle1 = 45.5_deg;
    auto angle2 = 90_deg;

    auto result = angle1 + angle2;

    ASSERT_FLOAT_EQ(result.to_float(), 135.5f);
}


TEST_F(GivenAnAngle, AnglesWithSumGreateThan360Normalise)
{
    auto angle1 = 350_deg;
    auto angle2 = 90_deg;

    auto result = angle1 + angle2;

    ASSERT_FLOAT_EQ(result.to_float(), 80.0f);
}


TEST_F(GivenAnAngle, DifferenceBetweenTwoAnglesIsAlwaysPositive)
{
    auto angle1 = 350_deg;
    auto angle2 = 90_deg;

    auto result = angle1 - angle2;

    ASSERT_FLOAT_EQ(result.to_float(), 260.0f);

    result = angle2 - angle1;

    ASSERT_FLOAT_EQ(result.to_float(), 100.0f);
}


TEST_F(GivenAnAngle, AnglesCanBeUpdated)
{
    auto angle = 90_deg;
    
    angle += 45_deg;

    ASSERT_FLOAT_EQ(angle.to_float(), 135.0f);

    angle += 270_deg;

    ASSERT_FLOAT_EQ(angle.to_float(), 45.0f);

    angle -= 90_deg;

    ASSERT_FLOAT_EQ(angle.to_float(), 315.0f);
}


TEST_F(GivenAnAngle, AnglesWithinEncoderResolutionAreEqual)
{
    auto angle1 = 90.001_deg;
    auto angle2 = 90_deg;

    ASSERT_TRUE(angle1 == angle2);
}


TEST_F(GivenAnAngle, AnglesGreaterThanEncoderResolutionAreNotEqual)
{
    auto angle1 = 90.01_deg;
    auto angle2 = 90_deg;

    ASSERT_FALSE(angle1 == angle2);
}


TEST_F(GivenAnAngle, AnglesCanBeDisplayedAsStrings)
{
    auto angle = 95.5_deg;

    ASSERT_EQ(angle.to_string(), "95.50°");
}


TEST_F(GivenAnAngle, ARadianCanBeDefaultConstructed)
{
    Radians rad { };
    
    ASSERT_FLOAT_EQ(rad.to_float(), 0.0f);
}


TEST_F(GivenAnAngle, ARadianCanBeNonDefaultConstructed)
{
    Radians rad { pi<float> / 4 };

    ASSERT_FLOAT_EQ(rad.to_float(), 0.785398f);
}


TEST_F(GivenAnAngle, RadsGreaterThan2PiAreNormalised)
{
    Radians one_rotation { 2.0f * pi<float> };
    Radians two_rotations { 4.0f *  pi<float> };
    Radians one_and_a_half_rotations { 3 * pi<float> };

    ASSERT_FLOAT_EQ(one_rotation.to_float(), 0.0f);
    ASSERT_FLOAT_EQ(two_rotations.to_float(), 0.0f);
    ASSERT_FLOAT_EQ(one_and_a_half_rotations.to_float(), pi<float>);
}


TEST_F(GivenAnAngle, RadsWithSumLessThan2PiAdd)
{
    auto rad1 = 0.785398_rad;
    auto rad2 = 1.570796_rad;

    auto result = rad1 + rad2;

    ASSERT_FLOAT_EQ(result.to_float(), 2.356194f);
}


TEST_F(GivenAnAngle, RadsWithSumGreaterThan2PiNormalise)
{
    auto rad1 = 4.712389_rad; // 1.5f * pi
    auto rad2 = 3.141593_rad;

    auto result = rad1 + rad2;

    ASSERT_FLOAT_EQ(result.to_float(), 1.570796f);
}


TEST_F(GivenAnAngle, DifferenceBetweenTwoRadsIsAlwaysPositive)
{
    auto rad1 = 6.1_rad;
    auto rad2 = 1.2_rad;

    auto result = rad1 - rad2;

    ASSERT_FLOAT_EQ(result.to_float(), 4.9f);

    result = rad2 - rad1;

    ASSERT_FLOAT_EQ(result.to_float(), 1.3831859f);
}


TEST_F(GivenAnAngle, RadiansCanBeUpdated)
{
    auto rad = 1.570796_rad;

    rad += 0.785398_rad;

    ASSERT_FLOAT_EQ(rad.to_float(), 2.356194f);

    rad += 4.712389_rad;

    ASSERT_FLOAT_EQ(rad.to_float(), 0.78539753f);

    rad -= 1.570796_rad;

    ASSERT_FLOAT_EQ(rad.to_float(), 5.497787f);
}


TEST_F(GivenAnAngle, RadiansWithinEncoderResolutionAreEqual)
{
    auto rad1 = 3.141_rad;
    auto rad2 = 3.142_rad;

    ASSERT_TRUE(rad1 == rad2);
}


TEST_F(GivenAnAngle, RadiansGreaterThanEncoderResolutionNotEqual)
{
    auto rad1 = 3.14_rad;
    auto rad2 = 3_rad; // Engineer's Pi

    ASSERT_FALSE(rad1 == rad2);
}


TEST_F(GivenAnAngle, RadiansCanBeDisplayedAsStrings)
{
    auto rad = 3.1416_rad;

    ASSERT_EQ(rad.to_string(), "3.14rad");
}


TEST_F(GivenAnAngle, DegreesShouldConvertCorrectlyToRadians)
{
    auto ninety_degrees = 90.0_deg;
    auto fortyfive_degrees = 45.0_deg;
    auto onehundredthirtyfive_degrees = 135.0_deg;
    auto onehundredeighty_degrees = 180.0_deg;
    auto twohundredtwentyfive_degrees = 225_deg;
    auto twohundredseventy_degrees = 270_deg;
    auto threehundredfiften_degrees = 315_deg;

    ASSERT_NEAR(fortyfive_degrees.to_radians(), 0.785398f, 0.000001f);
    ASSERT_NEAR(ninety_degrees.to_radians(), 1.570796f, 0.000001f);
    ASSERT_NEAR(onehundredthirtyfive_degrees.to_radians(), 2.356194f, 0.000001f);
    ASSERT_NEAR(onehundredeighty_degrees.to_radians(), 3.141593f, 0.000001f);
    ASSERT_NEAR(twohundredtwentyfive_degrees.to_radians(), 3.926990f, 0.000001f);
    ASSERT_NEAR(twohundredseventy_degrees.to_radians(), 4.712389f, 0.000001f);
    ASSERT_NEAR(threehundredfiften_degrees.to_radians(), 5.497787f, 0.000001f);
}


TEST_F(GivenAnAngle, RadiansShouldConvertCorrectlyToDegrees)
{
    auto quarterpi_rad = 0.785398_rad;
    auto halfpi_rad = 1.570796_rad;
    auto threequarter_rad = 2.356194_rad;
    auto pi_rad = 3.141593_rad;
    auto oneandaquarter_rad = 3.926990_rad;
    auto oneandahalf_rad = 4.712389_rad;
    auto oneandthreequarter_rad = 5.497787_rad;

    ASSERT_FLOAT_EQ(quarterpi_rad.to_degrees(), 45.0f);
    ASSERT_FLOAT_EQ(halfpi_rad.to_degrees(), 90.0f);
    ASSERT_FLOAT_EQ(threequarter_rad.to_degrees(), 135.0f);
    ASSERT_FLOAT_EQ(pi_rad.to_degrees(), 180.0f);
    ASSERT_FLOAT_EQ(oneandaquarter_rad.to_degrees(), 225.0f);
    ASSERT_FLOAT_EQ(oneandahalf_rad.to_degrees(), 270.0f);
    ASSERT_FLOAT_EQ(oneandthreequarter_rad.to_degrees(), 315.0f);
}