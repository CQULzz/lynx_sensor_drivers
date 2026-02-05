#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>
#include <cstdint>
#include "Colossus_UDP_messages.h"

using namespace std;
using namespace Navtech::Utility;
using namespace Navtech::Unit;
using namespace Navtech::Networking::Colossus_protocol::UDP;


class GivenAPointcloudSpoke : public ::testing::Test {
protected:
};


// -------------------------------------------------------------------
// Single float value tests
//
TEST_F(GivenAPointcloudSpoke, APointDefaultConstructs)
{
    Pointcloud_spoke::Point pt { };

    ASSERT_FLOAT_EQ(pt.range(), 0.0f);
    ASSERT_FLOAT_EQ(pt.power(), 0.0f);
}


TEST_F(GivenAPointcloudSpoke, PointsCanHaveValues)
{
    Pointcloud_spoke::Point pt { };

    pt.range(100);
    pt.power(45);

    ASSERT_FLOAT_EQ(pt.range(), 100.0f);
    ASSERT_FLOAT_EQ(pt.power(), 45.0f);
}


TEST_F(GivenAPointcloudSpoke, AddSpokeToMessage)
{
    Message msg { Type::point_cloud };

    Pointcloud_spoke spoke { };
    spoke.azimuth(100);
    spoke.bearing(45.2_deg);

    msg.append(spoke);

    auto view = msg.view_as<Pointcloud_spoke>();

    ASSERT_EQ(view->azimuth(), 100);
    ASSERT_FLOAT_EQ(view->bearing().to_float(), 45.2f);
}


TEST_F(GivenAPointcloudSpoke, AddSpokeWithPointToMessage)
{
    Message msg { Type::point_cloud };

    Pointcloud_spoke spoke { };
    spoke.azimuth(100);
    spoke.bearing(45.2_deg);
    spoke.points(1);

    msg.append(spoke);

    vector<uint8_t> points { };
    points << Pointcloud_spoke::Point { 23.5_m, 45.0_dB };
    msg.append(points);

    auto view = msg.view_as<Pointcloud_spoke>();

    ASSERT_EQ(view->azimuth(), 100);
    ASSERT_FLOAT_EQ(view->bearing().to_float(), 45.2f);

    auto [sz, pts] = view->points();

    ASSERT_EQ(sz, 1);
    ASSERT_FLOAT_EQ(pts[0].range(), 23.5f);
    ASSERT_FLOAT_EQ(pts[0].power(), 45.0f);
}


TEST_F(GivenAPointcloudSpoke, ASpokeWithNoPointsReturnsNull)
{
    Message msg { Type::point_cloud };

    Pointcloud_spoke spoke { };
    spoke.azimuth(100);
    spoke.bearing(45.2_deg);

    msg.append(spoke);

    auto view = msg.view_as<Pointcloud_spoke>();
    auto [sz, pts] = view->points();

    ASSERT_EQ(sz, 0);
    ASSERT_EQ(pts, nullptr);
}