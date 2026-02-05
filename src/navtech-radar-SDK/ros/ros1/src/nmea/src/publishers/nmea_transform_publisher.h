#include <ros/ros.h>
#include "tf2_ros/transform_broadcaster.h"
#include "../common/nmea_functions.h"
#include "NMEA_client.h"
#include "Message_buffer.h"
#include "Endpoint.h"
#include "../common/nmea_functions.h"

using namespace Navtech::Networking;
using namespace Navtech::Networking::NMEA_protocol;

class Nmea_transform_publisher 
{
public:
    Nmea_transform_publisher(ros::NodeHandle& nh);
    ~Nmea_transform_publisher();

    void start();
    void stop();

    void send_transform();

private:
    // Owned components
    //
    Navtech::owner_of<Navtech::Networking::NMEA_protocol::Client> nmea_client { };

    // Nmea client callbacks
    //
    void handle_NMEA_message(Client& nmea_client [[maybe_unused]], Message& msg);

    std::string nmea_ip{ "" };
    uint16_t nmea_port{ 0 };
    std::string parent_topic{ "" };
    std::string child_topic{ "" };

    Nmea_Functions nmea_functions;

    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_static_broadcaster_;
};