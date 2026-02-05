#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Cartesian_coordinate.h"
#include "Polar_coordinate.h"

#include "Euclidean_coordinate.h"
#include "Spherical_coordinate.h"

using namespace std;
using namespace Navtech;


class GivenCoordinates : public ::testing::Test {
protected:
    GivenCoordinates() = default;
};

TEST_F(GivenCoordinates, DefaultCartesianCoordinatesAreZero)
{
    Cartesian::Coordinate c1 { };

    ASSERT_FLOAT_EQ(c1.x, 0.0f);
    ASSERT_FLOAT_EQ(c1.y, 0.0f);
}


TEST_F(GivenCoordinates, DefaultPolarCoordinatesAreZero)
{
    Polar::Coordinate p1 { };

    ASSERT_FLOAT_EQ(p1.range, 0.0f);
    ASSERT_EQ(p1.bearing, 0_deg);
}



TEST_F(GivenCoordinates, CartesianCoordinatesAreEqualIfWithin1mm)
{
    Cartesian::Coordinate c1 { 100.0f, 100.0f };
    Cartesian::Coordinate c2 { 100.0005f, 100.0005f };
    Cartesian::Coordinate c3 { 100.001f, 100.001f };
    Cartesian::Coordinate c4 { 100.0011f, 100.0011f };

    ASSERT_TRUE(c1 == c2);
    ASSERT_TRUE(c1 == c3);
    ASSERT_TRUE(c1 != c4);
}


TEST_F(GivenCoordinates, PolarCoordinatesAreEqualIfWithin1mmAndLessThanEncoderResolution)
{
    Polar::Coordinate p1 { 100.0f, 45_deg };
    Polar::Coordinate p2 { 100.0005f, 45.0005_deg };
    Polar::Coordinate p3 { 100.0011f, 45.0011_deg };

    ASSERT_TRUE(p1 == p2);
    ASSERT_TRUE(p1 != p3);
}


TEST_F(GivenCoordinates, CartesianCoordinatesConvertCorrectlyToPolar)
{
    // A 100-unit line, going round the circle from radar North
    //
    Cartesian::Coordinate c1 { 0.0f, 100.0f };
    Cartesian::Coordinate c2 { 70.710678f, 70.710678f };
    Cartesian::Coordinate c3 { 100.0f, 0.0f };
    Cartesian::Coordinate c4 { 70.710678f, -70.710678f };
    Cartesian::Coordinate c5 { 0.0f, -100.0f };
    Cartesian::Coordinate c6 { -70.710678f, -70.710678f };
    Cartesian::Coordinate c7 { -100.0f, 0.0f };
    Cartesian::Coordinate c8 { -70.710678f, 70.710678f };

    auto p1 = c1.to_polar();
    auto p2 = c2.to_polar();
    auto p3 = c3.to_polar();
    auto p4 = c4.to_polar();
    auto p5 = c5.to_polar();
    auto p6 = c6.to_polar();
    auto p7 = c7.to_polar();
    auto p8 = c8.to_polar();

    ASSERT_FLOAT_EQ(p1.range, 100.0f);
    ASSERT_EQ(p1.bearing, 0_deg);

    ASSERT_FLOAT_EQ(p2.range, 100.0f);
    ASSERT_EQ(p2.bearing, 45_deg);

    ASSERT_FLOAT_EQ(p3.range, 100.0f);
    ASSERT_EQ(p3.bearing, 90_deg);

    ASSERT_FLOAT_EQ(p4.range, 100.0f);
    ASSERT_EQ(p4.bearing, 135_deg);

    ASSERT_FLOAT_EQ(p5.range, 100.0f);
    ASSERT_EQ(p5.bearing, 180_deg);

    ASSERT_FLOAT_EQ(p6.range, 100.0f);
    ASSERT_EQ(p6.bearing, 225_deg);

    ASSERT_FLOAT_EQ(p7.range, 100.0f);
    ASSERT_EQ(p7.bearing, 270_deg);

    ASSERT_FLOAT_EQ(p8.range, 100.0f);
    ASSERT_EQ(p8.bearing, 315_deg);
}


TEST_F(GivenCoordinates, NullPolarCoordinatesConvertCorrectlyToCartesian)
{
    Polar::Coordinate p1 { 0.0f, 0_deg };

    auto c1 = p1.to_cartesian();

    ASSERT_FLOAT_EQ(c1.x, 0.0f);
    ASSERT_FLOAT_EQ(c1.y, 0.0f);
}


TEST_F(GivenCoordinates, PolarCoordinatesConvertCorrectlyToCartesian)
{
    // A 100-unit line, going round the circle from radar North
    //
    Polar::Coordinate p1 { 100.0f, 0_deg };
    Polar::Coordinate p2 { 100.0f, 45_deg };
    Polar::Coordinate p3 { 100.0f, 90_deg };
    Polar::Coordinate p4 { 100.0f, 135_deg };
    Polar::Coordinate p5 { 100.0f, 180_deg };
    Polar::Coordinate p6 { 100.0f, 225_deg };
    Polar::Coordinate p7 { 100.0f, 270_deg };
    Polar::Coordinate p8 { 100.0f, 315_deg };

    auto c1 = p1.to_cartesian();
    auto c2 = p2.to_cartesian();
    auto c3 = p3.to_cartesian();
    auto c4 = p4.to_cartesian();
    auto c5 = p5.to_cartesian();
    auto c6 = p6.to_cartesian();
    auto c7 = p7.to_cartesian();
    auto c8 = p8.to_cartesian();

    ASSERT_NEAR(c1.x, 0.0f, 0.001f);
    ASSERT_NEAR(c1.y, 100.0f, 0.001f);

    ASSERT_NEAR(c2.x, 70.710678f, 0.001f);
    ASSERT_NEAR(c2.y, 70.710678f, 0.001f);

    ASSERT_NEAR(c3.x, 100.0f, 0.001f);
    ASSERT_NEAR(c3.y, 0.0f, 0.001f);

    ASSERT_NEAR(c4.x, 70.710678f, 0.001f);
    ASSERT_NEAR(c4.y, -70.710678f, 0.001f);

    ASSERT_NEAR(c5.x, 0.0f, 0.001f);
    ASSERT_NEAR(c5.y, -100.0f, 0.001f);

    ASSERT_NEAR(c6.x, -70.710678f, 0.001f);
    ASSERT_NEAR(c6.y, -70.710678f, 0.001f);

    ASSERT_NEAR(c7.x, -100.0f, 0.001f);
    ASSERT_NEAR(c7.y, 0.0f, 0.001f);

    ASSERT_NEAR(c8.x, -70.710678f, 0.001f);
    ASSERT_NEAR(c8.y, 70.710678f, 0.001f);
}


TEST_F(GivenCoordinates, CartesianCoordinatesCanBeAdded)
{
    Cartesian::Coordinate c1 { 100.0f, 200.0f };
    Cartesian::Coordinate c2 { 50.0f, 50.0f };

    auto c3 = c1 + c2;

    ASSERT_FLOAT_EQ(c3.x, 150.0f);
    ASSERT_FLOAT_EQ(c3.y, 250.0f);
}


TEST_F(GivenCoordinates, CartesianCoordinatesCanBeModifiedByAddition)
{
    Cartesian::Coordinate c1 { 100.0f, 200.0f };
    Cartesian::Coordinate c2 { 50.0f, 50.0f };

    c1 += c2;

    ASSERT_FLOAT_EQ(c1.x, 150.0f);
    ASSERT_FLOAT_EQ(c1.y, 250.0f);
}


TEST_F(GivenCoordinates, SubtractingCartessianCoordinatesYieldsLinearDistanceBetweenThem)
{
    Cartesian::Coordinate c1 { 0.0f, 0.0f };
    Cartesian::Coordinate c2 { 3.0f, 4.0f };

    Cartesian::Coordinate c3 { 0.0f, 0.0f };
    Cartesian::Coordinate c4 { -3.0f, -4.0f };

    auto d1 = c2 - c1;
    auto d2 = c4 - c3;
    auto d3 = c1 - c2;
    auto d4 = c3 - c4;

    ASSERT_FLOAT_EQ(d1, 5.0f);
    ASSERT_FLOAT_EQ(d2, 5.0f);
    ASSERT_FLOAT_EQ(d3, 5.0f);
    ASSERT_FLOAT_EQ(d4, 5.0f);
}



TEST_F(GivenCoordinates, DefaultEuclideanCoordinatesAreZero)
{
    Euclidean::Coordinate e1 { };

    ASSERT_FLOAT_EQ(e1.x, 0.0f);
    ASSERT_FLOAT_EQ(e1.y, 0.0f);
}


TEST_F(GivenCoordinates, DefaultSphericalCoordinatesAreZero)
{
    Spherical::Coordinate s1 { };

    ASSERT_FLOAT_EQ(s1.range, 0.0f);
    ASSERT_EQ(s1.bearing, 0.0_deg);
    ASSERT_EQ(s1.inclination, 0.0_deg);
}


TEST_F(GivenCoordinates, EuclideanCoordinatesAreEqualIfWithin1mm)
{
    Euclidean::Coordinate c1 { 100.0f, 100.0f, 100.0f };
    Euclidean::Coordinate c2 { 100.0005f, 100.0005f, 100.0005f };
    Euclidean::Coordinate c3 { 100.001f, 100.001f, 100.001f };
    Euclidean::Coordinate c4 { 100.0011f, 100.0011f, 100.0011f };

    ASSERT_TRUE(c1 == c2);
    ASSERT_TRUE(c1 == c3);
    ASSERT_TRUE(c1 != c4);
}


TEST_F(GivenCoordinates, SphericalCoordinatesAreEqualIfWithin1mmAndLessThanEncoderResolution)
{
    Spherical::Coordinate p1 { 100.0f, 45_deg, 45_deg };
    Spherical::Coordinate p2 { 100.0005f, 45.0005_deg, 45.0005_deg };
    Spherical::Coordinate p3 { 100.0011f, 45.0011_deg, 45.0011_deg };

    ASSERT_TRUE(p1 == p2);
    ASSERT_TRUE(p1 != p3);
}


TEST_F(GivenCoordinates, EuclideanCoordinatesConvertCorrectlyIntoSpherical)
{
    Euclidean::Coordinate e1 { 0.00f, 0.00f, 100.00f };
    Euclidean::Coordinate e2 { 50.000f, 50.000f, 70.710678f };
    Euclidean::Coordinate e3 { 70.710678f, 70.710678f,  0.0f };
    Euclidean::Coordinate e4 { 50.000f, 50.000f, -70.710678f };
    Euclidean::Coordinate e5 { 0.00f, 0.00f, -100.00f };
    Euclidean::Coordinate e6 { -50.000f, -50.000f, -70.710678f };
    Euclidean::Coordinate e7 { -70.710678f, -70.710678f, 0.0f };
    Euclidean::Coordinate e8 { -50.000f, -50.000f, 70.710678f };

    auto s1 = e1.to_spherical();

    ASSERT_FLOAT_EQ(s1.range, 100.0f);
    ASSERT_TRUE(std::isnan(s1.bearing.to_float()));
    ASSERT_EQ(s1.inclination, 0_deg);

    auto s2 = e2.to_spherical();

    ASSERT_FLOAT_EQ(s2.range, 100.0f);
    ASSERT_EQ(s2.bearing, 45_deg);
    ASSERT_EQ(s2.inclination, 45_deg);

    auto s3 = e3.to_spherical();

    ASSERT_FLOAT_EQ(s3.range, 100.0f);
    ASSERT_EQ(s3.bearing, 45_deg);
    ASSERT_EQ(s3.inclination, 90_deg);

    auto s4 = e4.to_spherical();

    ASSERT_FLOAT_EQ(s4.range, 100.0f);
    ASSERT_EQ(s4.bearing, 45_deg);
    ASSERT_EQ(s4.inclination, 135_deg);

    auto s5 = e5.to_spherical();

    ASSERT_FLOAT_EQ(s5.range, 100.0f);
    ASSERT_TRUE(std::isnan(s5.bearing.to_float()));
    ASSERT_EQ(s5.inclination, 180_deg);

    auto s6 = e6.to_spherical();

    ASSERT_FLOAT_EQ(s6.range, 100.0f);
    ASSERT_EQ(s6.bearing, 225_deg);
    ASSERT_EQ(s6.inclination, 135_deg);

    auto s7 = e7.to_spherical();

    ASSERT_FLOAT_EQ(s7.range, 100.0f);
    ASSERT_EQ(s7.bearing, 225_deg);
    ASSERT_EQ(s7.inclination, 90_deg);   


    auto s8 = e8.to_spherical();

    ASSERT_FLOAT_EQ(s8.range, 100.0f);
    ASSERT_EQ(s8.bearing, 225_deg);
    ASSERT_EQ(s8.inclination, 45_deg);
}


TEST_F(GivenCoordinates, SphericalCoordinatesConvertCorrectlyToEuclidean)
{
    // A 100-unit line, at 45.0f degrees to radar north
    // Rotating in a sphere about Z axis
    //
    Spherical::Coordinate s1 { 100.0f, 45_deg, 0_deg };
    Spherical::Coordinate s2 { 100.0f, 45_deg, 45_deg };
    Spherical::Coordinate s3 { 100.0f, 45_deg, 90_deg };
    Spherical::Coordinate s4 { 100.0f, 45_deg, 135_deg };
    Spherical::Coordinate s5 { 100.0f, 45_deg, 180_deg };
    Spherical::Coordinate s6 { 100.0f, 45_deg, 225_deg };
    Spherical::Coordinate s7 { 100.0f, 45_deg, 270_deg };
    Spherical::Coordinate s8 { 100.0f, 45_deg, 315_deg };

    auto e1 = s1.to_euclidean();

    ASSERT_FLOAT_EQ(e1.x, 0.000f);
    ASSERT_FLOAT_EQ(e1.y, 0.000f);
    ASSERT_FLOAT_EQ(e1.z, 100.0f);

    auto e2 = s2.to_euclidean();

    ASSERT_FLOAT_EQ(e2.x, 50.000f);
    ASSERT_FLOAT_EQ(e2.y, 50.000f);
    ASSERT_FLOAT_EQ(e2.z, 70.711f);

    auto e3 = s3.to_euclidean();

    ASSERT_FLOAT_EQ(e3.x, 70.711f);
    ASSERT_FLOAT_EQ(e3.y, 70.711f);
    ASSERT_FLOAT_EQ(e3.z, 0.00f);

    auto e4 = s4.to_euclidean();

    ASSERT_FLOAT_EQ(e4.x, 50.0f);
    ASSERT_FLOAT_EQ(e4.y, 50.000f);
    ASSERT_FLOAT_EQ(e4.z, -70.711f);

    auto e5 = s5.to_euclidean();

    ASSERT_FLOAT_EQ(e5.x, 0.0f);
    ASSERT_FLOAT_EQ(e5.y, 0.0f);
    ASSERT_FLOAT_EQ(e5.z, -100.0f);

    auto e6 = s6.to_euclidean();

    ASSERT_FLOAT_EQ(e6.x, -50.000f);
    ASSERT_FLOAT_EQ(e6.y, -50.000f);
    ASSERT_FLOAT_EQ(e6.z, -70.711f);

    auto e7 = s7.to_euclidean();

    ASSERT_FLOAT_EQ(e7.x, -70.711f);
    ASSERT_FLOAT_EQ(e7.y, -70.711f);
    ASSERT_FLOAT_EQ(e7.z, 0.0f);

    auto e8 = s8.to_euclidean();

    ASSERT_FLOAT_EQ(e8.x, -50.000f);
    ASSERT_FLOAT_EQ(e8.y, -50.000f);
    ASSERT_FLOAT_EQ(e8.z, 70.711f);
}


TEST_F(GivenCoordinates, NullSphericalCoordinatesConvertCorrectlyToEuclidean)
{
    Spherical::Coordinate s1 { 0.0f, 0_deg, 0_deg };

    auto e1 = s1.to_euclidean();

    ASSERT_NEAR(e1.x, 0.0f, 0.001f);
    ASSERT_NEAR(e1.y, 0.0f, 0.001f);
    ASSERT_NEAR(e1.z, 0.0f, 0.001f);
}