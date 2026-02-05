/*
Copyright 2016 Navtech Radar Limited
This file is part of iasdk which is released under The MIT License (MIT).
See file LICENSE.txt in project root or go to https://opensource.org/licenses/MIT 
for full license details.
*/


#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <math.h>

#include "ros/ros.h"
#include "Colossus_TCP_client.h"
#include "Colossus_TCP_messages.h"
#include "Protobuf_helpers.h"
#include "configurationdata.pb.h"
#include "Signal_handler.h"

#include "std_msgs/String.h"
#include "nav_ross/nav_msg.h"

#include "Log.h"

//#include "nav_ross/config_msg.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>
#include <pcl_ros/point_cloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud2_iterator.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Iphlpapi.lib")
#endif

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Networking::Colossus_protocol;

using Navtech::Networking::Endpoint;
using Navtech::Networking::IP_address;
using Navtech::Networking::Port;
using Utility::Signal_handler;
using Utility::stdout_log;
using Utility::endl;

uint16_t _packetCount = 0;

image_transport::Publisher  PolarPublisher;            
uint16_t _lastAzimuth_navigation = 0;
ros::Publisher Radar_Config_Publisher;
uint16_t _lastAzimuth = 0;
std::shared_ptr<TCP::Client> _radarClient = std::make_shared<TCP::Client>(
    Endpoint {
        IP_address { "127.0.0.1" } ,
        6317
    }
);

int azimuth_counter = 0;
int azimuth_counter_navigation = 0;
cv::Mat radar_image_polar;
long frame_number = 0;
std::vector<std::vector<std::tuple<float, uint16_t>>> all_peaks_raw;
std::vector<uint16_t> all_azimuths_raw;
std::vector<std::vector<float>> all_peaks_cart;
std_msgs::Header header;  // empty header
std::string check3;
int publish_image = 0;
float range_res;
int range_in_bins;
int azimuths;
int encoder_size;
int bin_size;
int expected_rotation_rate;


void FFTDataHandler(const TCP::Message& msg)
{
    ros::spinOnce();

    auto fft_msg    = msg.view_as<TCP::FFT_data>();
    auto data       = fft_msg->to_vector();

    _packetCount++;
    if (fft_msg->azimuth() < _lastAzimuth) {

        nav_ross::nav_msg nav_msg;
        nav_msg.range_resolution        = range_res;
        nav_msg.AzimuthSamples          = azimuths;
        nav_msg.EncoderSize             = encoder_size;
        nav_msg.BinSize                 = bin_size;
        nav_msg.RangeInBins             = range_in_bins;
        nav_msg.ExpectedRotationRate    = expected_rotation_rate;
        //Radar_Config_Publisher.publish(msg);
        if (frame_number > 2) {
            //IF image topic is on
            if (publish_image) {
                header.seq          = frame_number;        // user defined counter
                header.stamp.sec    = fft_msg->ntp_seconds();  // time
                header.stamp.nsec   = fft_msg->ntp_split_seconds();

                sensor_msgs::ImagePtr PolarMsg = cv_bridge::CvImage(header, "mono8", radar_image_polar).toImageMsg();
                PolarPublisher.publish(PolarMsg);
            }
        }
        
        // update/reset variables
        frame_number++;
        azimuth_counter = 0;
        
        _packetCount = 0;
    }
    // populate image
    if (frame_number > 2) {

        int bearing = (static_cast<double>(fft_msg->azimuth()) / static_cast<double>(encoder_size)) * static_cast<double>(azimuths);
        for (size_t i = 0; i < data.size(); i++) {
            radar_image_polar.at<uchar>(i, bearing) = static_cast<int>(data[i]);
        }
    }

    _lastAzimuth = fft_msg->azimuth();
}



void ConfigurationDataHandler(const TCP::Message& msg)
{
    using Navtech::Protobuf::from_vector_into;
    using Colossus::Protobuf::ConfigurationData;

    auto cfg = msg.view_as<TCP::Configuration>();
    auto protobuf = from_vector_into<ConfigurationData>(cfg->to_vector());

    range_res = (static_cast<float>(cfg->bin_size()) / 10000.0f);    
    range_in_bins = cfg->range_in_bins();
    azimuths = cfg->azimuth_samples();
    encoder_size = cfg->encoder_size();
    bin_size= cfg->bin_size();
    expected_rotation_rate= cfg->packet_rate();

    stdout_log << "ConfigurationDataHandler - Expected Rotation Rate [" << expected_rotation_rate << "Hz]" << endl;
    stdout_log << "ConfigurationDataHandler - Range In Bins [" << range_in_bins << "]" << endl;
    stdout_log << "ConfigurationDataHandler - Bin Size [" << bin_size << "m]" << endl;
    stdout_log << "ConfigurationDataHandler - Range In Metres ["<<+ (bin_size * static_cast<float>(range_in_bins)) << "m]" << endl;
    stdout_log << "ConfigurationDataHandler - Azimuth Samples [" << azimuths << "]" << endl;
    

    float configuration_range_res;
    int configuration_range_in_bins;
    int configuration_azimuths;
    // PUBLISH TO PARAMETER SERVER //
    ros::param::set("/configuration_range_res",range_res);
    ros::param::set("/configuration_azimuths",azimuths);
    ros::param::set("/configuration_range_in_bins",range_in_bins);
    if (
        ros::param::has("/configuration_range_res") 
        && ros::param::has("/configuration_range_res")
        && ros::param::has("/configuration_azimuths")
    ) {
        std::cout<<"\nRadar Configuration published to parameter server";
    }

    _packetCount = 0;
    _lastAzimuth = 0;
    
    
    ros::NodeHandle nh3("~");

    nh3.getParam("/talker1/param3", check3);
    std::cout << check3 << std::endl;
    ROS_INFO("Got parameter : %s", check3.c_str());
    
    if (check3.compare("image_on") == 0) {
        publish_image = 1;
        ROS_INFO("image topic publishing...");
    }
    else {
        ROS_INFO("image publisher off");
    }


    radar_image_polar = cv::Mat::zeros(range_in_bins, azimuths, CV_8UC1);
    
    _radarClient->send(TCP::Type::start_fft_data);
    // _radarClient->send(Type::start_nav_data);
}

// Not used in this example, but can be activated in a manner similar
// to FFTDataHandler
//
void NavigationDataHandler(const TCP::Message& msg)
{    
    auto data = msg.view_as<TCP::Navigation_data>();

    auto [sz, points] = data->points();

    if (sz == 0) return;

    auto firstRange = points[0].range();
    auto firstPower = points[0].power();
    auto angle = static_cast<float>(data->azimuth()) * 360.0f / static_cast<float>(encoder_size);

    stdout_log << "NavigationDataHandler - First Target Range [" << firstRange << "]"
               << "Power [" << (firstPower / 10.0f) << "] Angle " << angle << "]"
               << endl;
    
}


// ---------------------------------------------------------------------------------------------------------------------
// Signal handling: If SIGINT or SIGTERM are sent to the 
// program, stop processing.
//
volatile bool running { true };

void stop_running(std::int32_t signal [[maybe_unused]], std::int32_t info [[maybe_unused]])
{
    stdout_log << "Ctrl-C received.  Terminating..." << endl;
    running = false;
}

int32_t main(int32_t argc, char** argv)
{
    using namespace Navtech::Time::Monotonic;
#ifdef _WIN32
    WSADATA wsaData;
    auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) {
        stdout_log << "Tracker - WSAStartup failed with error [" << std::to_string(err) << "]" << endl;
        return EXIT_FAILURE;
    }
#endif
    ros::init(argc, argv, "talker1");
    ros::NodeHandle n;
    //Radar_Config_Publisher = n.advertise<nav_ross::nav_msg>("Navtech/Configuration_Topic", 1,true);
    ros::NodeHandle node;
    image_transport::ImageTransport it(node);
    PolarPublisher = it.advertise("/Navtech/Polar", 1000);
    
    Signal_handler signal_handler { };
    signal_handler.register_handler(SIGINT, stop_running);
    signal_handler.register_handler(SIGTERM, stop_running);

    stdout_log << "Test Client Starting" << endl;
    
    _radarClient->set_handler(
        TCP::Type::configuration,
        [] (TCP::Client&, TCP::Message& msg) { ConfigurationDataHandler(msg); }
    );

    _radarClient->set_handler(
        TCP::Type::fft_data,
        [] (TCP::Client&, TCP::Message& msg) { FFTDataHandler(msg); }
    );

    _radarClient->start();
    
    while(running) {
        Monotonic::sleep_for(250_msec);
    }

    stdout_log << "Test Client Stopping" << endl;

    _radarClient->stop();

    stdout_log << "Test client stopped" << endl;
    
    return 0;
}
