#include <functional>
#include <memory>
#include <stdexcept>
#include <exception>
#include <cassert>
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"

#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"
#include "nmea_subscriber_transform_publisher.h"


Nmea_subscriber_transform_publisher::Nmea_subscriber_transform_publisher(ros::NodeHandle& nh)
{
    using std::placeholders::_1;

    nh.param<std::string>("parent_topic", parent_topic, "");
    nh.param<std::string>("child_topic", child_topic, "");

    nmea_gprmc_subscriber = nh.subscribe(
        "nmea_data/gphdt_data",
        1,
        &Nmea_subscriber_transform_publisher::gprmc_data_callback,
        this
    );

    nmea_gphdt_subscriber = 
    nh.subscribe(
        "nmea_data/gphdt_data",
        1,
        &Nmea_subscriber_transform_publisher::gphdt_data_callback,
        this
    );

    nmea_pashr_subscriber = 
    nh.subscribe(
        "nmea_data/pashr_data",
        1,
        &Nmea_subscriber_transform_publisher::pashr_data_callback,
        this
    );

    // Create a broadcaster
    ROS_INFO("%s", parent_topic.c_str());
    ROS_INFO("%s", child_topic.c_str());
    tf_static_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>();
}


void Nmea_subscriber_transform_publisher::gprmc_data_callback(const nmea_ros::nmea_gprmc_message::Ptr msg)
{
    nmea_functions.parseRosGprmc(msg);
    send_transform();
}


void Nmea_subscriber_transform_publisher::gphdt_data_callback(const nmea_ros::nmea_gphdt_message::Ptr msg)
{
    nmea_functions.parseRosGphdt(msg);
    send_transform();
}


void Nmea_subscriber_transform_publisher::pashr_data_callback(const nmea_ros::nmea_pashr_message::Ptr msg)
{
    nmea_functions.parseRosPashr(msg);
    send_transform();
}


void Nmea_subscriber_transform_publisher::send_transform()
{
    // Get the latest transform
    auto latest_transform = nmea_functions.getLatestTransformData();

    // Send a transform
    geometry_msgs::TransformStamped t;

    t.header.stamp = ros::Time::now();;
    t.header.frame_id = parent_topic;
    t.child_frame_id = child_topic;

    t.transform.translation.x = latest_transform.x;
    t.transform.translation.y = latest_transform.y;
    t.transform.translation.z = latest_transform.z;
    tf2::Quaternion q;
    q.setRPY(latest_transform.roll * M_PI / 180, latest_transform.pitch * M_PI / 180, latest_transform.yaw * M_PI / 180);
    t.transform.rotation.x = q.x();
    t.transform.rotation.y = q.y();
    t.transform.rotation.z = q.z();
    t.transform.rotation.w = q.w();

    tf_static_broadcaster_->sendTransform(t);
}