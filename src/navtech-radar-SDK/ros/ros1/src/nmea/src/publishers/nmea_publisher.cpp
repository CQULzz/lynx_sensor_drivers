#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include "NMEA_client.h"
#include "Message_buffer.h"
#include "Endpoint.h"

#include "nmea_publisher.h"
#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"

using namespace Navtech::Networking;
using namespace Navtech::Networking::NMEA_protocol;


Nmea_publisher::Nmea_publisher(ros::NodeHandle& nh)
{
    nh.param<std::string>("nmea_ip", nmea_ip, "");
    nmea_port = nh.param("nmea_port", 0);

    // Set up the nmea client
    //
    Endpoint server_addr { Navtech::Networking::IP_address(nmea_ip), nmea_port };
    nmea_client = Navtech::allocate_owned<Navtech::Networking::NMEA_protocol::Client>(
        server_addr
    );

    nmea_client->set_handler(
        Type::gprmc, 
        std::bind(&Nmea_publisher::handle_GPRMC, this, std::placeholders::_1, std::placeholders::_2)
    );
    nmea_client->set_handler(
        Type::gphdt, 
        std::bind(&Nmea_publisher::handle_GPHDT, this, std::placeholders::_1, std::placeholders::_2)
    );
        nmea_client->set_handler(
        Type::pashr, 
        std::bind(&Nmea_publisher::handle_PASHR, this, std::placeholders::_1, std::placeholders::_2)
    );

    nmea_gprmc_publisher =
    nh.advertise<nmea_ros::nmea_gprmc_message>(
        "nmea_data/gprmc_data",
         1000
    );

    nmea_gphdt_publisher =
    nh.advertise<nmea_ros::nmea_gphdt_message>(
        "nmea_data/gphdt_data",
         1000
    );

    nmea_pashr_publisher =
    nh.advertise<nmea_ros::nmea_pashr_message>(
        "nmea_data/pashr_data",
         1000
    );
}


Nmea_publisher::~Nmea_publisher()
{
    stop();
}


void Nmea_publisher::start()
{
    nmea_client->start();
}


void Nmea_publisher::stop()
{
    nmea_client->remove_handler(Type::gprmc);
    nmea_client->remove_handler(Type::gphdt);
    nmea_client->remove_handler(Type::pashr);

    nmea_client->stop();
}


void Nmea_publisher::handle_GPRMC(Client& nmea_client [[maybe_unused]], Message& msg)
{
    std::vector<std::string> result;
    std::stringstream s_stream(std::string {msg.begin(), msg.end()});
    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, ',');
        result.push_back(substr);
        }

    nmea_ros::nmea_gprmc_message gprmc_message {};
    gprmc_message.header = std_msgs::Header();
    gprmc_message.header.stamp = ros::Time::now();;
    gprmc_message.header.frame_id = "nmea";
    gprmc_message.message_id = result[0];
    gprmc_message.position_utc = std::stof(result[1]);
    gprmc_message.status = result[2].at(0);
    gprmc_message.latitude = std::stof(result[3]);
    gprmc_message.latitude_dir = result[4].at(0);
    gprmc_message.longitude = std::stof(result[5]);
    gprmc_message.longitude_dir = result[6].at(0);
    gprmc_message.speed = std::stof(result[7]);
    gprmc_message.angle = std::stof(result[8]);
    gprmc_message.date = result[9];
    gprmc_message.magnetic_variation = std::stof(result[10]);
    gprmc_message.magnetic_variation_dir = result[11].at(0);
    gprmc_message.mode_indicator = result[12].at(0);
    gprmc_message.checksum_data = result[12].substr(1,3);
    nmea_gprmc_publisher.publish(gprmc_message);
}


void Nmea_publisher::handle_GPHDT(Client& nmea_client [[maybe_unused]], Message& msg)
{
    std::vector<std::string> result;
    std::stringstream s_stream(std::string {msg.begin(), msg.end()});
    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, ',');
        result.push_back(substr);
        }

    nmea_ros::nmea_gphdt_message gphdt_message {};
    gphdt_message.header = std_msgs::Header();
    gphdt_message.header.stamp = ros::Time::now();;
    gphdt_message.header.frame_id = "nmea";
    gphdt_message.message_id = result[0];
    gphdt_message.heading = std::stof(result[1]);
    gphdt_message.heading_relative = result[2].at(0);
    gphdt_message.checksum_data = result[2].substr(1,3);
    nmea_gphdt_publisher.publish(gphdt_message);
}


void Nmea_publisher::handle_PASHR(Client& nmea_client [[maybe_unused]], Message& msg)
{
    std::vector<std::string> result;
    std::stringstream s_stream(std::string {msg.begin(), msg.end()});
    while(s_stream.good()) {
        std::string substr;
        getline(s_stream, substr, ',');
        result.push_back(substr);
        }

    nmea_ros::nmea_pashr_message pashr_message {};
    pashr_message.header = std_msgs::Header();
    pashr_message.header.stamp = ros::Time::now();;
    pashr_message.header.frame_id = "nmea";
    pashr_message.message_id = result[0];
    pashr_message.utc_time = std::stof(result[1]);
    pashr_message.heading = std::stof(result[2]);
    pashr_message.true_heading_flag = result[3].at(0);
    pashr_message.roll = std::stof(result[4]);
    pashr_message.pitch = std::stof(result[5]);
    pashr_message.heave = std::stof(result[6]);
    pashr_message.roll_accuracy = std::stof(result[7]);
    pashr_message.pitch_accuracy = std::stof(result[8]);
    pashr_message.heading_accuracy = std::stof(result[9]);
    pashr_message.aiding_status = std::stof(result[10]);
    pashr_message.imu_status = std::stof(result[11]);
    pashr_message.checksum_data = result[11].substr(1,3);
    nmea_pashr_publisher.publish(pashr_message);
}