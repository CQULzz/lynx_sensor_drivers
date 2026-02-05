#include <ros/ros.h>
#include "tf2_ros/transform_broadcaster.h"
#include "../common/nmea_functions.h"
#include "../common/nmea_functions.h"

#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"

class Nmea_subscriber_transform_publisher
{
public:
    Nmea_subscriber_transform_publisher(ros::NodeHandle& nh);

    void send_transform();

private:
    constexpr static int nmea_gprmc_queue_size{ 4 };
    constexpr static int nmea_gphdt_queue_size{ 4 };
    constexpr static int nmea_pashr_queue_size{ 4 };

    void gprmc_data_callback(const nmea_ros::nmea_gprmc_message::Ptr msg);
    void gphdt_data_callback(const nmea_ros::nmea_gphdt_message::Ptr msg);
    void pashr_data_callback(const nmea_ros::nmea_pashr_message::Ptr msg);

    ros::Subscriber nmea_gprmc_subscriber;
    ros::Subscriber nmea_gphdt_subscriber;
    ros::Subscriber nmea_pashr_subscriber;

    std::string parent_topic{ "" };
    std::string child_topic{ "" };

    Nmea_Functions nmea_functions;

    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_static_broadcaster_;
};