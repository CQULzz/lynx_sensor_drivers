#include <memory>
#include "nmea_transform_publisher.h"
#include "geometry_msgs/TransformStamped.h"
#include "ros/ros.h"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_ros/transform_broadcaster.h"
#include <math.h>
#include "NMEA_client.h"
#include "Message_buffer.h"
#include "Endpoint.h"

using namespace Navtech::Networking;
using namespace Navtech::Networking::NMEA_protocol;

Nmea_transform_publisher::Nmea_transform_publisher(ros::NodeHandle& nh)
{
    // Parse parameters
    //
    nh.param<std::string>("nmea_ip", nmea_ip, "");
    nh.param<std::string>("parent_topic", parent_topic, "");
    nh.param<std::string>("child_topic", child_topic, "");
    nmea_port = nh.param("nmea_port", 0);

    // Set up the nmea client
    //
    Endpoint server_addr { Navtech::Networking::IP_address(nmea_ip), nmea_port };
    nmea_client = Navtech::allocate_owned<Navtech::Networking::NMEA_protocol::Client>(
        server_addr
    );

    nmea_client->set_handler(
        Type::gprmc, 
        std::bind(&Nmea_transform_publisher::handle_NMEA_message, this, std::placeholders::_1, std::placeholders::_2)
    );
    
    nmea_client->set_handler(
        Type::gphdt, 
        std::bind(&Nmea_transform_publisher::handle_NMEA_message, this, std::placeholders::_1, std::placeholders::_2)
    );

    nmea_client->set_handler(
        Type::pashr, 
        std::bind(&Nmea_transform_publisher::handle_NMEA_message, this, std::placeholders::_1, std::placeholders::_2)
    );

    // Create a broadcaster
    //
    ROS_INFO("%s", parent_topic.c_str());
    ROS_INFO("%s", child_topic.c_str());
    tf_static_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>();
}


Nmea_transform_publisher::~Nmea_transform_publisher()
{
    stop();
}


void Nmea_transform_publisher::start()
{
    nmea_client->start();
}


void Nmea_transform_publisher::stop()
{
    nmea_client->remove_handler(Type::gprmc);
    nmea_client->remove_handler(Type::gphdt);
    nmea_client->remove_handler(Type::pashr);

    nmea_client->stop();
}


void Nmea_transform_publisher::handle_NMEA_message(Client& nmea_client [[maybe_unused]], Message& msg)
{
  nmea_functions.parseNmeaMessage(std::string {msg.begin(), msg.end()});
}


void Nmea_transform_publisher::send_transform()
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