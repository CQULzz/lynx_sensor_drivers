
/*
Copyright 2016 Navtech Radar Limited
This file is part of iasdk which is released under The MIT License (MIT).
See file LICENSE.txt in project root or go to https://opensource.org / licenses / MIT
for full license details.
*/


/* 
Node Description: 
Example Application: Moving Platform Data Processor + Visualizer
This app will subscribe to the raw data and give a user-configurable way to process this data.
Functionality can be switched on / off using the dynamic reconfigure rqt plugin.
Functionality available:
Thresholded pointCloud of raw data
Image topic using object detection algorithms 
Grid lines
*/

#define AZIMUTH_AVERAGING true

#include "nav_ross/nav_msg.h"

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <cstdint>
#include <functional>
#include <chrono>
#include <thread>
#include <cmath>

#include <ros/ros.h>
#include <std_msgs/String.h>
#include <dynamic_reconfigure/server.h>
#include <dynamic_paramConfig.h>
#include <std_msgs/String.h>
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <std_msgs/Float32.h>

#include <pcl/point_cloud.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_cloud2_iterator.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sensor_msgs/LaserScan.h>

#include "Time_utils.h"
#include "Log.h"
#include "constants.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Iphlpapi.lib")
#endif

using namespace Navtech;
using namespace Navtech::Time;
using namespace Navtech::Time::Monotonic;

using namespace cv;

using Utility::stdout_log;
using Utility::endl;

int downsamplingFactor = 1;
Observation _lastScanComplete = Clock::now();

// DYNAMICALLY CONFIGURABLE PARAMETERS (on the parameter server)
uint16_t threshold_value        = 62;
uint16_t dilation_value         = 2; //Default Image Slider value
uint16_t gaussian_value         = 5; //Default Dilation filter size
uint16_t maxAngle               = 35;
uint16_t pcl_threshold_value    = 60;
uint16_t grid_stepSize          = 10;
uint8_t  colormap               = 10;
uint16_t level_above_average    = 19;
uint16_t adaptive_size          = 2;
uint16_t grid_stepSize_m;
uint16_t grid_width;
uint16_t grid_height;

bool boundingBoxes2 = false;
bool pointCloud = false;
bool LaserScan2 = false;
bool MinAreaRect = false;
bool radarImage = false;
bool radarImageProcessing =false;
bool grid = true;
bool adaptive_threshold = false;
bool longTermAverage = false;
bool ThreeDScanApp;
static bool Configuration_NOT_Set = 1;
double grid_opacity = 0.5;

// End of configurable Parameters
uint16_t azimuths = 400;
bool initialisedAverage;
float range_res;
float sin_values [405] = {};
float cos_values [405] = {};
float Phi = 0;
float last_Phi = 0;
pcl::PointCloud<pcl::PointXYZI>::Ptr threeDPCL(new pcl::PointCloud<pcl::PointXYZI>);

image_transport::Publisher CartesianPublisher;
image_transport::Publisher FilteredPublisher;
image_transport::Publisher BoxesPublisher;

ros::Publisher LaserScanPublisher2;
ros::Publisher PointcloudPublisher;
ros::Publisher PointcloudPublisher3d;

ros::Publisher MarkerPub;
cv::Mat Long_Term_Average;


void InitialiseParameters()
{
    if((ros::param::has("configuration_range_res")) && (ros::param::has("configuration_azimuths"))){
        
        ros::param::getCached("configuration_range_res", range_res);
        int az_tmp;
        ros::param::get(" / configuration_azimuths", az_tmp);
        azimuths = az_tmp;
        stdout_log << "Parameter server range resolution [" << range_res << "] m" << endl;
        stdout_log << "Number of Azimuths Set from Parameter Server [" << az_tmp << "]" << endl;

        for (uint16_t i = 0; i < azimuths; i++) {
            float theta     = static_cast<float>(i * 2) * pi<float> / static_cast<float>(azimuths);
            sin_values[i]   = sin(theta); 
            cos_values[i]   = cos(theta);
        }
        for (uint16_t i = az_tmp; i < azimuths + 5; i++) {
            float theta     = static_cast<float>(i-azimuths) * 2 * pi<float> / static_cast<float>(azimuths);
            sin_values[i]   = sin(theta);
            cos_values[i]   = cos(theta);
        }
    }
    else {
        stdout_log << "Configuration Settings not loaded from ros::param server..\n"
                   << "If doing data playback, set parameters in server manually using ROS param CLI.\n"
                   << "Parameters required are: configuration_range_res and configuration_azimuths\n"
                   << "Exiting..."
                   << endl;
    }
}


void PublishPointcloud(cv_bridge::CvImagePtr &cv_polar_image, uint16_t &pcl_threshold_value, uint16_t &maxAngle, float &range_res)
{
    float theta;
    cv::Mat navigation_image = (cv_polar_image->image - (double)pcl_threshold_value) * 255.0 / (255.0 - pcl_threshold_value);
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);

    for (int bearing = 0; bearing < cv_polar_image->image.rows; bearing++)
    {
        theta = (static_cast<float>(bearing) / cv_polar_image->image.rows) * 2 * pi<float>;
        for (size_t ind = 1; ind < cv_polar_image->image.cols; ind++)
        {
            pcl::PointXYZI p;
            if ((theta < maxAngle * pi<float> / 180) || (theta > 2 * pi<float> - maxAngle * pi<float> / 180))
            {
                //The coordinate of the point is taken from the depth map
                //Y and Z  taken negative to immediately visualize the cloud in the right way
                //
                p.x = range_res * ind * cos(theta);
                p.y = range_res * ind * sin(theta);

                //Coloring the point with the corrispondent point in the rectified image
                //
                p.intensity = navigation_image.at<uchar>(bearing, ind);
                p.z = 0;//(static_cast<float>(p.intensity) / 64);//0; // Uncomment to make height of pixel proportional to Radar cross sectional area (static_cast<float>(p.intensity / 32);
            }
            //Insert point in the cloud, cutting the points that are too distant
            if (cv_polar_image->image.at<uchar>(bearing, ind) > pcl_threshold_value)
                cloud->points.push_back(p);
        }
    }
    cloud->width = static_cast<int>(cloud->points.size());
    cloud->height = 1;
    cloud->header.frame_id = "navtech";
    pcl_conversions::toPCL(cv_polar_image->header.stamp,cloud->header.stamp);//pcl_conversions::toPCL(cv_polar_image->header.stamp,cloud->header.stamp);
    PointcloudPublisher.publish(cloud);
}


void PublishLaserScan2(cv::Mat &new_im2,float range_resolution, uint16_t range_bins, uint16_t ScanAzimuths, int radarbins,cv_bridge::CvImagePtr &cv_polar_image)
{
    sensor_msgs::LaserScan scan2;
    scan2.angle_increment   = 2 * pi<float> / static_cast<float>(ScanAzimuths);
    scan2.time_increment    = 1 / (static_cast<float>(4) * static_cast<float>(ScanAzimuths));
    scan2.scan_time         = 1 / static_cast<float>(4);
    scan2.range_min         = 0.0;
    scan2.range_max         = 500.0;    //Set MaxRange of Scan
    scan2.ranges.resize(ScanAzimuths);

    stdout_log << "range_bins [" << range_bins << "], radar_bins [" << radarbins << "]" << endl;
    
    for (int i = 0; i < ScanAzimuths; i++) { 
        // azimuths
        for (int j = 30; j < range_bins; j++) { 
            // ranges
            if (j == range_bins - 2) {
                scan2.ranges[i] = static_cast<float>(j) * range_resolution * static_cast<float>(range_bins) / static_cast<float>(radarbins);    
                break;
            }

            // If there's an edge, assign an edge value. Otherwise, continue
            //
            if (new_im2.at<uchar>(i, j) != 0) 
            {
                scan2.ranges[i] = static_cast<float>(j) * range_resolution * static_cast<float>(range_bins) / static_cast<float>(radarbins);
                break;
            }
            else //else, an edge is there - assign edge value
            {

            }
        }
    }
    scan2.header.stamp = cv_polar_image->header.stamp;//cv_polar_image->header.stamp;
    scan2.header.frame_id = "navtech";
    LaserScanPublisher2.publish(scan2);
}


void PublishProcessedImage(cv_bridge::CvImagePtr &Input_cv_image){
        cv::Mat radar_image_polar_copy ;
        Input_cv_image->image.copyTo(radar_image_polar_copy);
         cv::Mat maskmat(Input_cv_image->image.size(),CV_8UC1);
        uint16_t cartesian_rows = Input_cv_image->image.cols;
        uint16_t cartesian_cols = Input_cv_image->image.cols;
        cv::Mat radar_image_cart = cv::Mat::zeros(cartesian_rows, cartesian_cols, CV_8UC1);
        cv::Point2f center(static_cast<float>(cartesian_cols) / 2, static_cast<float>(cartesian_rows) / 2);
        double maxRadius = min(center.y, center.x);
        int flags = cv::INTER_LINEAR + cv::WARP_FILL_OUTLIERS;
        if(radarImageProcessing){
            cv::GaussianBlur(radar_image_polar_copy, radar_image_polar_copy, cvSize(gaussian_value * 2 + 1, gaussian_value * 2 + 1), 0);
            cv::Mat processed_im;
            cv::threshold(radar_image_polar_copy, radar_image_polar_copy, threshold_value, 255, CV_THRESH_TOZERO);
            cv::Canny(radar_image_polar_copy, radar_image_polar_copy, 50, 100);
            cv::dilate(radar_image_polar_copy, radar_image_polar_copy, cv::getStructuringElement(cv::MORPH_RECT, cvSize(dilation_value + 1, dilation_value + 1)));
            std::vector<std::vector<cv::Point>> contours;
            cv::findContours(radar_image_polar_copy, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); //Save only outer contours
            cv::drawContours(maskmat, contours, -1, 255, -1);
            cv::warpPolar(maskmat, radar_image_cart, radar_image_cart.size(), center, maxRadius, flags + cv::WARP_INVERSE_MAP);
        }
        else{
            
            cv::warpPolar(radar_image_polar_copy, radar_image_cart, radar_image_cart.size(), center, maxRadius, flags + cv::WARP_INVERSE_MAP);            
        }
        cv::rotate(radar_image_cart, radar_image_cart, cv::ROTATE_90_COUNTERCLOCKWISE);
        cv::bitwise_not(radar_image_cart,radar_image_cart);
        cv::Mat radar_image_color;
        cv::applyColorMap(radar_image_cart, radar_image_color, colormap);
        if (grid){
            cv::Mat radar_grid;
            grid_width = radar_image_color.size().width;
            grid_height = radar_image_color.size().height;
            radar_image_color.copyTo(radar_grid);
            int start_point_y = (grid_height / 2) % grid_stepSize_m;
            int start_point_x = (grid_width / 2) % grid_stepSize_m;
            for (int i = start_point_y; i < grid_height; i += grid_stepSize_m)
                cv::line(radar_grid, Point(0, i), Point(grid_width, i), cv::Scalar(255, 255, 255));

            for (int i = start_point_x; i < grid_width; i += grid_stepSize_m)
                cv::line(radar_grid, Point(i, 0), Point(i, grid_height), cv::Scalar(255, 255, 255));
            addWeighted(radar_image_color, grid_opacity, radar_grid, 1 - grid_opacity, 0.0, radar_image_color);
        }
        sensor_msgs::ImagePtr CartesianMsg = cv_bridge::CvImage(Input_cv_image->header, "rgb8", radar_image_color).toImageMsg();
        CartesianPublisher.publish(CartesianMsg);
}


void FindEdges2(
    cv_bridge::CvImagePtr &inputImage,
    uint16_t &gaussian_value,
    cv::Mat &OutputMatImage,
    uint16_t &threshold_value,
    uint16_t &dilation_value,
    uint16_t maxAngle
)
{
    cv::Mat radar_image_rcs;
    inputImage->image.copyTo(radar_image_rcs);
    if(adaptive_threshold) {
        cv::adaptiveThreshold(OutputMatImage,OutputMatImage,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,adaptive_size * 2+1,-(double)level_above_average);
    }
    cv::GaussianBlur(inputImage->image, inputImage->image, cvSize(gaussian_value * 2 + 1, gaussian_value * 2 + 1), 0);
    if(AZIMUTH_AVERAGING) {
        cv::Mat mean;
        cv::reduce(inputImage->image,mean,0,CV_REDUCE_AVG,CV_8UC1);
        inputImage->image.copyTo(OutputMatImage);
        for(int i = 0;i<inputImage->image.rows;i++) {
            cv::Mat temp_im;
            cv::threshold(inputImage->image.row(i), inputImage->image.row(i), static_cast<int>(mean.at<uchar>(0,i)) + threshold_value, 255, CV_THRESH_TOZERO);
            inputImage->image.row(i).copyTo(OutputMatImage.row(i));
        }
    }
    else {
        cv::threshold(inputImage->image, OutputMatImage, threshold_value, 255, CV_THRESH_TOZERO);
    }
    cv::vconcat(OutputMatImage,OutputMatImage.rowRange(0,5),OutputMatImage);
    
    uint16_t deltaRow = inputImage->image.rows /2 - std::round((static_cast<float>(maxAngle) / 360.0f) * (inputImage->image.rows));
    OutputMatImage.rowRange(inputImage->image.rows / 2 - deltaRow,inputImage->image.rows / 2 +deltaRow).setTo(0);
    cv::Canny(OutputMatImage, OutputMatImage, 50, 100);
}

void PublishBoundingBoxes(cv_bridge::CvImagePtr &cv_polar_image,cv::Mat &EdgesMat,float &range_resolution){
    cv::dilate(EdgesMat, EdgesMat, cv::getStructuringElement(cv::MORPH_RECT, cvSize(dilation_value + 1, dilation_value + 1)));
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(EdgesMat, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); //Save only outer contours
    visualization_msgs::MarkerArray Marker_Array;
    Marker_Array.markers.resize(contours.size());
    cv::Mat radar_image_rcs;
    cv_polar_image->image.copyTo(radar_image_rcs);
    //Draw Box Contours Round Features
    if (MinAreaRect) {
        geometry_msgs::Point p;
        p.z = 0;

        for (unsigned int i = 0; i < contours.size(); i++) {
            Marker_Array.markers[i].header.frame_id = "navtech";
            Marker_Array.markers[i].header.stamp = cv_polar_image->header.stamp;//cv_polar_image->header.stamp;
            Marker_Array.markers[i].action = visualization_msgs::Marker::ADD;
            Marker_Array.markers[i].type = visualization_msgs::Marker::LINE_STRIP;
            Marker_Array.markers[i].pose.orientation.w = 1.0;
            Marker_Array.markers[i].id = i;
            Marker_Array.markers[i].scale.x = .3;
            Marker_Array.markers[i].color.b = 0;//1.0;
            Marker_Array.markers[i].color.a = 1.0;
            Marker_Array.markers[i].lifetime = ros::Duration(.25);
                
                
            Mat maskmat(radar_image_rcs.size(),CV_8UC1);
            double minVal;
            double maxVal;
            Scalar mean;
            cv::Point p1,p2;

            drawContours(maskmat, contours, i, 255, -1);
            cv::minMaxLoc(radar_image_rcs, &minVal, &maxVal, &p1,&p2, maskmat);
            mean = cv::mean(radar_image_rcs, maskmat);

            if (mean[0]>127) mean[0] = 127;

            Marker_Array.markers[i].color.r = (127+mean[0]) / 255;
            Marker_Array.markers[i].color.g = (127-mean[0]) / 255;

            cv::RotatedRect rect = cv::minAreaRect(contours[i]);
            cv::Point2f points[4];
            rect.points(points);
            p.z = 1;

            for (unsigned int j = 0; j<4; j++) {
                p.y = points[j].x * sin(points[j].y * 2 * pi<float> / static_cast<float>(azimuths)) *range_resolution;
                p.x = points[j].x * cos(points[j].y * 2 * pi<float> / static_cast<float>(azimuths)) *range_resolution;
                Marker_Array.markers[i].points.push_back(p);
            }
            
            p.y = points[0].x * sin(points[0].y *2 * pi<float> / static_cast<float>(azimuths)) *range_resolution;
            p.x = points[0].x * cos(points[0].y *2 * pi<float> / static_cast<float>(azimuths)) *range_resolution;
            Marker_Array.markers[i].points.push_back(p);
            Marker_Array.markers.push_back(Marker_Array.markers[i]);    
        }    
    }    
        
    else {
        for (unsigned int i = 0; i < contours.size(); i++) {
            Marker_Array.markers[i].header.frame_id = "navtech";
            Marker_Array.markers[i].header.stamp = cv_polar_image->header.stamp;
            Marker_Array.markers[i].action = visualization_msgs::Marker::ADD;
            Marker_Array.markers[i].pose.orientation.w = 1.0;
            Marker_Array.markers[i].id = i;
            Marker_Array.markers[i].type = visualization_msgs::Marker::LINE_LIST;
            Marker_Array.markers[i].scale.x = .1;
            Marker_Array.markers[i].scale.z = .1;
            Marker_Array.markers[i].scale.y = .1;
            Marker_Array.markers[i].color.b = 0;//1.0;
            Marker_Array.markers[i].color.a = 1.0;
            Marker_Array.markers[i].lifetime = ros::Duration(.25);    
            std::vector<cv::Point> boundingContour;
            geometry_msgs::Point p;
            p.z = 0;

            cv::Mat maskmat(radar_image_rcs.size(),CV_8UC1);
            double minVal;
            double maxVal;
            Scalar mean;
            cv::Point p1,p2;

            drawContours(maskmat, contours, i, 255, -1);
            cv::minMaxLoc(radar_image_rcs, &minVal, &maxVal, &p1,&p2, maskmat);
            mean = cv::mean(radar_image_rcs, maskmat);
            /* if(mean[0]>100){mean[0] = 100;}
            if(mean[0] <10 ) {mean[0] = 10;} */
            Marker_Array.markers[i].color.r = maxVal / 255;
            Marker_Array.markers[i].color.g = 1.0-maxVal / 255;

            for (int k = 0;k<10;k++) {
                for (unsigned int j = 0; j<contours[i].size(); ++j) {
                    p.y = (contours[i][j].x * sin_values[contours[i][j].y]) * range_resolution;
                    p.x = (contours[i][j].x * cos_values[contours[i][j].y]) * range_resolution;
                    Marker_Array.markers[i].points.push_back(p);
                    if(j+1<contours[i].size()) {
                        p.y = (contours[i][j+1].x * sin_values[contours[i][j+1].y]) * range_resolution;
                        p.x = (contours[i][j+1].x * cos_values[contours[i][j+1].y]) * range_resolution;
                    }
                    else {
                        p.y = (contours[i][0].x * sin_values[contours[i][0].y]) * range_resolution;
                        p.x = (contours[i][0].x * cos_values[contours[i][0].y]) * range_resolution;
                    }
                    Marker_Array.markers[i].points.push_back(p);
                }
                p.z = p.z+.2;
            } // for (unsigned int j = 0...)
            Marker_Array.markers.push_back(Marker_Array.markers[i]);
        } // for (int k = 10...)
    } // else
    MarkerPub.publish(Marker_Array);
}


void createThreeDPointCloud(cv::Mat &edgesMatInput,float range_resolution, uint16_t range_bins,uint16_t scanAzimuths,cv_bridge::CvImagePtr &cv_polar_image,uint16_t &maxAngle){
    last_Phi = Phi;
    for (int i = 0; i < scanAzimuths; i++) {
        //all azimuths
        float theta = 2 * pi<float> * i / scanAzimuths; 
        pcl::PointXYZI p;
        for (int j = 30; j < range_bins; j++) {
            // bin ranges
            if (edgesMatInput.at<uchar>(i, j) != 0) {
                //if an edge is there - assign edge value. Otherwise, skip
                if ((theta < maxAngle * pi<float> / 180) || (theta > 2 * pi<float> - maxAngle * pi<float> / 180)){
                    p.x =static_cast<float>(j) * range_resolution* cos(theta);
                    p.y = static_cast<float>(j) * range_resolution* sin(theta) * cos(Phi * pi<float> / 180);
                    p.z = static_cast<float>(j) * range_resolution* sin(theta) * sin(Phi * pi<float> / 180);
                    
                    //Coloring the point with the correspondent point in the rectified image
                    //
                    if (p.z > 18) p.intensity = 255;
                    if (p.z < -2) p.intensity = 0;
                    p.intensity = static_cast<int>((p.z + 2) * 255) / 20;
                    threeDPCL->points.push_back(p);
                }
                break;
            }
        }
        
    }
}


void ParamCallback(nav_ross::dynamic_paramConfig &config, uint32_t level){
    ThreeDScanApp           = config.ThreeDScanApp;
    threshold_value         = config.threshold_value;
    dilation_value          = config.dilation_value;
    gaussian_value          = config.gaussian_value;
    pcl_threshold_value     = config.pcl_threshold_value;
    maxAngle                = config.maxangle;
    colormap                = config.colormap;
    boundingBoxes2          = config.boundingboxes2;
    MinAreaRect             = config.MinAreaRect;
    longTermAverage         = config.longtermaverage;
    LaserScan2              = config.LaserScan2_Map;
    pointCloud              = config.pointcloud;
    radarImage              = config.radarimage;
    radarImageProcessing    = config.radarimageprocessing;
    grid                    = config.grid;
    adaptive_threshold      = config.adaptive_threshold;
    level_above_average     = config.level_above_average;
    adaptive_size           = config.adaptive_size;

    if (grid == true) {
        grid_stepSize   = config.grid_stepSize;
        grid_stepSize_m = grid_stepSize / (range_res * 2);
        grid_opacity    = config.grid_opacity;
    }
}


void ChatterCallback(const sensor_msgs::ImageConstPtr &radar_image_polar)
{
    _lastScanComplete = now();

    cv_bridge::CvImagePtr cv_polar_image;
    cv_polar_image = cv_bridge::toCvCopy(radar_image_polar, sensor_msgs::image_encodings::MONO8);
    cv_polar_image->header.stamp = radar_image_polar->header.stamp;
    cv::resize(cv_polar_image->image,cv_polar_image->image,cv::Size(),1,1 / static_cast<float>(downsamplingFactor),INTER_NEAREST);
    //range_res = range_res * 2;
    uint16_t bins = cv_polar_image->image.rows;
    //uint16_t azimuths = cv_polar_image->image.cols;
    rotate(cv_polar_image->image, cv_polar_image->image, cv::ROTATE_90_COUNTERCLOCKWISE);
    cv::normalize(cv_polar_image->image, cv_polar_image->image, 0, 255, NORM_MINMAX, CV_8UC1);
    cv::Mat edges_Mat;
    float adj_range = range_res * downsamplingFactor;
    if (pointCloud == true) {
        PublishPointcloud(cv_polar_image, pcl_threshold_value, maxAngle, adj_range);    
    }

    if(radarImage) {
        //if(initialisedAverage){
        //    cv_polar_image->image = Long_Term_Average;}
        PublishProcessedImage(cv_polar_image);    
    }

    if(longTermAverage) {
        if(initialisedAverage) {
            Long_Term_Average = .9 * Long_Term_Average + .1 * cv_polar_image->image;
            cv_polar_image->image = cv_polar_image->image-Long_Term_Average;
        }
        else {
            Long_Term_Average = cv_polar_image->image;
            initialisedAverage = true;
        }
    }
    else initialisedAverage = false;
    
    if(boundingBoxes2 || LaserScan2 || ThreeDScanApp) {
        FindEdges2(cv_polar_image,gaussian_value,edges_Mat,threshold_value,dilation_value,maxAngle);
    }
    
    if (boundingBoxes2 == true) {
        PublishBoundingBoxes(cv_polar_image,edges_Mat,adj_range);
    }
    
    if (LaserScan2 == true) {
        PublishLaserScan2(edges_Mat, adj_range, edges_Mat.cols, edges_Mat.rows, bins,cv_polar_image);
    }

     if(boundingBoxes2 || LaserScan2) {
        sensor_msgs::ImagePtr FilteredMsg = cv_bridge::CvImage(cv_polar_image->header, "mono8", edges_Mat).toImageMsg();
        FilteredPublisher.publish(FilteredMsg);
    }
    if(ThreeDScanApp) {
        if(Phi != last_Phi){
        createThreeDPointCloud(edges_Mat,adj_range,edges_Mat.cols,edges_Mat.rows,cv_polar_image,maxAngle);
        }
        threeDPCL->width = static_cast<int>(threeDPCL->points.size());
        threeDPCL->height = 1;
        threeDPCL->header.frame_id = "navtech";
        pcl_conversions::toPCL(cv_polar_image->header.stamp,threeDPCL->header.stamp);//pcl_conversions::toPCL(cv_polar_image->header.stamp,cloud->header.stamp);
        PointcloudPublisher3d.publish(threeDPCL);        
    }    
    stdout_log << "Processing time [" << now() - _lastScanComplete << "]" << endl;
    if (now() - _lastScanComplete > 150_msec) {
        if(downsamplingFactor < 8) {
            stdout_log << "Too much to process... Decreasing resolution of image" << endl;
            downsamplingFactor++;
            stdout_log << "Downsampling factor [" << downsamplingFactor << "]" << endl;
        }
        else {
            stdout_log << "Max downsampling reached!";
        }
    }
    if(now() - _lastScanComplete < 50_msec) {
        if(downsamplingFactor > 1) {
            stdout_log << "Available processing power... Using higher resuloution image";
            downsamplingFactor--;
            stdout_log << "Downsampling factor is: "<<downsamplingFactor;
        }
    }
    //if(_lastScanComplete>now())
}


void updatePhi(const std_msgs::Float32::ConstPtr &newPhi){
    Phi = 2 * newPhi->data * 360 / 4096;
    stdout_log << Phi << endl;
}


int main(int argc, char * * argv)
{
    ros::init(argc, argv, "Mapper_V2");
    ROS_INFO("initialized = true");
    ros::NodeHandle n;
    ros::NodeHandle n1;
    InitialiseParameters();
    ros::Subscriber sub2;
    sub2 = n.subscribe(" / Navtech / Polar", 10, ChatterCallback);
    ros::Subscriber arduinoSub;
    arduinoSub = n.subscribe("arduino_phi", 1, updatePhi);

    image_transport::ImageTransport it(n1);
    CartesianPublisher = it.advertise("Navtech / Cartesian1", 10);
    FilteredPublisher = it.advertise("Navtech / Filtered", 10);
    LaserScanPublisher2 = n1.advertise<sensor_msgs::LaserScan>("Navtech / scan3", 1);
    PointcloudPublisher = n1.advertise<sensor_msgs::PointCloud2>("Navtech / FilteredPointcloud", 1);
    PointcloudPublisher3d = n1.advertise<sensor_msgs::PointCloud2>("Navtech / 3dPCL", 1);

    MarkerPub = n1.advertise<visualization_msgs::MarkerArray>("visualization_markers",1000);

    dynamic_reconfigure::Server<nav_ross::dynamic_paramConfig> srv;
    dynamic_reconfigure::Server<nav_ross::dynamic_paramConfig>::CallbackType f;
    f = boost::bind(&ParamCallback, _1, _2);

    srv.setCallback(f);

    ros::spin(); 
    ROS_INFO("spinning = true");

    return 0;
}