#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Radar_features.h"

using namespace Navtech;
using namespace std;

class GivenRadarFeatures : public ::testing::Test {
};

TEST_F(GivenRadarFeatures, DefaultConstruction)
{
    Radar_features features { };

    ASSERT_EQ(features.to_uint32(), 0);
}


TEST_F(GivenRadarFeatures, InitialiseFromUint32)
{
    Radar_features features { 0x03 };

    ASSERT_EQ(features.auto_tune, 1);
    ASSERT_EQ(features.secondary_processing_module, 1);
    ASSERT_EQ(features.non_contour_data, 0);
}


TEST_F(GivenRadarFeatures, SetBooleanFlag)
{
    Radar_features features { };

    features.safeguard_enabled = true;
    ASSERT_EQ(features.safeguard_enabled, 1);

    features.safeguard_enabled = false;
    ASSERT_EQ(features.safeguard_enabled, 0);

    features.auto_tune           = 1;
    features.contour_map_defined = 1;
    features.sector_blanking     = 1;

    ASSERT_EQ(features.to_uint32(), 0x19);
}


TEST_F(GivenRadarFeatures, EnumFields)
{
    Radar_features features { };

    features.fft_protocol = Radar_features::Protocol::cat_240;
    features.modbus_mode  = static_cast<int>(Radar_features::Modbus_mode::master);

    ASSERT_EQ(features.to_uint32(), 0x840);
}

TEST_F(GivenRadarFeatures, FlagsFromUint32)
{
    Radar_features features { 0b110010010101110 };

    ASSERT_TRUE(features.safeguard_enabled);
    ASSERT_TRUE(features.motor_enabled);
    ASSERT_EQ(features.modbus_mode, Radar_features::Modbus_mode::disabled);
    ASSERT_EQ(features.point_data_output, Radar_features::Point_data::nav_mode);
    ASSERT_FALSE(features.high_precision_output);
    ASSERT_TRUE(features.low_precision_output);
    ASSERT_EQ(features.fft_protocol, Radar_features::Protocol::colossus);
    ASSERT_FALSE(features.sector_blanking);
    ASSERT_TRUE(features.contour_map_defined);
    ASSERT_TRUE(features.non_contour_data);
    ASSERT_TRUE(features.secondary_processing_module);
    ASSERT_FALSE(features.auto_tune);
}


TEST_F(GivenRadarFeatures, RadarFeaturesCanBeConvertedToUint32)
{
    
}