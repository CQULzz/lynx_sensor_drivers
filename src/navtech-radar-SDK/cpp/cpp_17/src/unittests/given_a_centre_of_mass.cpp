#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Centre_of_mass.h"

// Linear Centre of Mass

using namespace Navtech::Utility;

class GivenACentreOfMass : public ::testing::Test {
public:
    GivenACentreOfMass() = default;
};


TEST_F(GivenACentreOfMass, CentreOfMassInArrayIsCorrect)
{
    std::vector<float> quadratic_data_float {
        49.0f, 36.0f, 25.0f, 16.0f, 9.0f, 4.0f, 1.0f, 0.0f, 1.0f, 4.0f, 9.0f, 16.0f, 25.0f, 36.0f, 49.0f 
    };

    auto quadratic_centre = centre_of_mass(quadratic_data_float);

    ASSERT_FLOAT_EQ(quadratic_centre, 7.0f);

    // Something not so easily centred
    //
    std::vector<float> log10_10_to_100 {
        1.0f, 1.301030f, 1.477121f, 1.602060f, 1.698970f, 1.778151f, 1.845098f, 1.903090f, 1.954243f, 2.0f
    };

    auto log_centre = centre_of_mass(log10_10_to_100);

    ASSERT_FLOAT_EQ(log_centre, 4.998516f);
}


TEST_F(GivenACentreOfMass, CentreOfMassIn2DStructureIsCorrect)
{
    // Taken from internal centre of mass demo
    //
    std::vector<std::vector<float>> shape {
        { 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 15.0f, 15 },
        { 10.0f, 10.0f, 10.0f, 12.0f, 13.0f, 18.0f, 20.0f, 15 },
        { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 19.0f, 18.0f, 10 },
        { 10.0f, 12.0f, 13.0f, 15.0f, 16.0f, 17.0f, 16.0f, 10 },
        { 10.0f, 10.0f, 10.0f, 13.0f, 14.0f, 15.0f, 16.0f, 10 },
        { 10.0f, 10.0f, 10.0f, 10.0f, 13.0f, 14.0f, 15.0f, 10 },
        { 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10 },
        { 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10 }
    };

    auto centre = centre_of_mass(shape);

    ASSERT_FLOAT_EQ(centre.first, 3.72643979f);
    ASSERT_FLOAT_EQ(centre.second, 3.325916f);
}