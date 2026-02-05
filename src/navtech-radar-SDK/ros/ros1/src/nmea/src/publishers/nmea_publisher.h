#include <ros/ros.h>
#include "NMEA_client.h"
#include "Message_buffer.h"
#include "Endpoint.h"
#include "../common/nmea_functions.h"

#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"

using namespace Navtech::Networking;
using namespace Navtech::Networking::NMEA_protocol;


class Nmea_publisher
{
public:
    Nmea_publisher(ros::NodeHandle& nh);
    ~Nmea_publisher();

    void start();
    void stop();

private:
    constexpr static int nmea_gprmc_queue_size{ 4 };
    constexpr static int nmea_gphdt_queue_size{ 4 };
    constexpr static int nmea_pashr_queue_size{ 4 };

    // Owned components
    //
    Navtech::owner_of<Navtech::Networking::NMEA_protocol::Client> nmea_client { };

    // Nmea client callbacks
    //
    void handle_GPRMC(Client& nmea_client [[maybe_unused]], Message& msg);
    void handle_GPHDT(Client& nmea_client [[maybe_unused]], Message& msg);
    void handle_PASHR(Client& nmea_client [[maybe_unused]], Message& msg);

    std::string nmea_ip { "" };
    uint16_t nmea_port { 0 };

    ros::Publisher nmea_gphdt_publisher{};
    ros::Publisher nmea_gprmc_publisher{};
    ros::Publisher nmea_pashr_publisher{};
};