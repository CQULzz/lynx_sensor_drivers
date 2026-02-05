#include <iostream>
#include <vector>
#include <iostream>
#include "math.h"
#include <cmath>
#include "nmea_functions.h"


Nmea_Functions::Nmea_Functions()
{

}


Nmea_Functions::~Nmea_Functions()
{

}


Nmea_Functions::NmeaLatLong Nmea_Functions::googlemaps_latlong_to_nmea_latlong(Nmea_Functions::GoogleMapsLatLong latlong){
    auto degrees_lat = ((int)(latlong.latitude) * 100);
    auto decimal_lat = latlong.latitude - ((int)(latlong.latitude));
    auto output_lat = (double)degrees_lat + (decimal_lat * 60.0);
    auto degrees_long = ((int)(latlong.longitude) * 100);
    auto decimal_long = latlong.longitude - ((int)(latlong.longitude));
    auto output_long = (double)degrees_long + (decimal_long * 60.0);
    Nmea_Functions::NmeaLatLong output_latlong {};
    output_latlong.latitude_hemisphere = output_lat > 0.0 ? 'N' : 'S';
    output_latlong.latitude = abs(output_lat);
    output_latlong.longitude_hemisphere = output_long > 0.0 ? 'E' : 'W';
    output_latlong.longitude = abs(output_long);
    return output_latlong;
}


Nmea_Functions::GoogleMapsLatLong Nmea_Functions::nmea_latlong_to_googlemaps_latlong(Nmea_Functions::NmeaLatLong latlong){
    auto degrees_lat = (int)(latlong.latitude / 100);
    auto output_lat = (double)degrees_lat + ((latlong.latitude - (degrees_lat * 100.0)) / 60.0);
    auto degrees_long = (int)(latlong.longitude / 100);
    auto output_long = (double)degrees_long + ((latlong.longitude - (degrees_long * 100.0)) / 60.0);
    Nmea_Functions::GoogleMapsLatLong output_latlong {};
    output_latlong.latitude = latlong.latitude_hemisphere == 'N' ? abs(output_lat) : abs(output_lat) * -1;
    output_latlong.longitude = latlong.longitude_hemisphere == 'E' ? abs(output_long) : abs(output_long) * -1;
    return output_latlong;
}


Nmea_Functions::XYDistance Nmea_Functions::latlongbearing_to_xy_translation(Nmea_Functions::LatLongPair lat_long_pair){
    auto R = 6378137;
    auto dLat = (M_PI / 180) * (lat_long_pair.current_lat - lat_long_pair.start_lat);
    auto dLong = (M_PI / 180) * (lat_long_pair.current_long - lat_long_pair.start_long);
    auto a = sin(dLat / 2) * sin(dLat / 2) +
    cos((M_PI / 180) * (lat_long_pair.start_lat)) * cos((M_PI / 180) * (lat_long_pair.current_lat)) *
    sin(dLong / 2) * sin(dLong / 2);
    auto c = 2 * atan2(sqrt(a), sqrt(1 - a));
    Nmea_Functions::XYDistance xydistance {};
    xydistance.euclidian_dist = R * c;
    xydistance.x_dist = sin(lat_long_pair.current_heading * (M_PI / 180)) * xydistance.euclidian_dist;
    xydistance.y_dist = cos(lat_long_pair.current_heading * (M_PI / 180)) * xydistance.euclidian_dist;
    return xydistance;
}


void Nmea_Functions::calculateTransform(){

    if (gprmc_waiting)
    {
        return;
    }

    if ((current_transform_pair.start_latlong.latitude == current_transform_pair.current_latlong.latitude) &&
    (current_transform_pair.start_latlong.longitude == current_transform_pair.current_latlong.longitude))
    {
        return;
    }

    LatLongPair latlong_pair{};
    latlong_pair.start_lat = current_transform_pair.start_latlong.latitude;
    latlong_pair.start_long = current_transform_pair.start_latlong.longitude;
    latlong_pair.start_heading = current_transform_pair.start_heading;
    latlong_pair.current_lat = current_transform_pair.current_latlong.latitude;
    latlong_pair.current_long = current_transform_pair.current_latlong.longitude;
    latlong_pair.current_heading = current_transform_pair.current_heading;
    auto translation = latlongbearing_to_xy_translation(latlong_pair);

    transform_data.x = translation.x_dist;
    transform_data.y = translation.y_dist;
    transform_data.z = current_transform_pair.current_height - current_transform_pair.start_height;
    transform_data.roll = current_transform_pair.current_rotation.roll - current_transform_pair.start_rotation.roll;
    transform_data.pitch = current_transform_pair.current_rotation.pitch - current_transform_pair.start_rotation.pitch;
    transform_data.yaw = current_transform_pair.current_rotation.yaw - current_transform_pair.start_rotation.yaw;
}


Nmea_Functions::TranslationRotation Nmea_Functions::getLatestTransformData(){
    return transform_data;
}


void Nmea_Functions::parseRosGprmc(const nmea_ros::nmea_gprmc_message::Ptr msg){
    
    try
    {
        NmeaLatLong lat_long {};
        lat_long.latitude = msg->latitude;
        lat_long.latitude_hemisphere = msg->latitude_dir;
        lat_long.longitude = msg->longitude;
        lat_long.longitude_hemisphere = msg->longitude_dir;
        auto google_lat_long = nmea_latlong_to_googlemaps_latlong(lat_long);

        if (first_gprmc_received)
        {
            current_transform_pair.current_latlong.latitude = google_lat_long.latitude;
            current_transform_pair.current_latlong.longitude = google_lat_long.longitude;
            gprmc_waiting = true;
        }
        else
        {
            current_transform_pair.start_latlong.latitude = google_lat_long.latitude;
            current_transform_pair.start_latlong.longitude = google_lat_long.longitude;
            current_transform_pair.current_latlong.latitude = google_lat_long.latitude;
            current_transform_pair.current_latlong.longitude = google_lat_long.longitude;
            first_gprmc_received = true;
        }

        // If we have all the required data, calculate the transform
        if (first_gprmc_received && first_gphdt_received && first_pashr_received)
        {
            calculateTransform();
        }
    }
    catch (...) {
        ROS_INFO("Error calculating translation from ROS GPRMC message");
    }
}


void Nmea_Functions::parseRosGphdt(const nmea_ros::nmea_gphdt_message::Ptr msg){

    try
    {
        if (first_gphdt_received)
        {
            current_transform_pair.current_heading = msg->heading;
            gprmc_waiting = false;
        }
        else
        {
            current_transform_pair.start_heading = msg->heading;
            current_transform_pair.current_heading = msg->heading;
            first_gphdt_received = true;
        }

        // If we have all the required data, calculate the transform
        if (first_gprmc_received && first_gphdt_received && first_pashr_received)
        {
            calculateTransform();
        }
    }
    catch (...) {
        ROS_INFO("Error calculating heading from ROS GPHDT message");
    }
}


void Nmea_Functions::parseRosPashr(const nmea_ros::nmea_pashr_message::Ptr msg){

    try
    {
        if (first_pashr_received)
        {
            current_transform_pair.current_rotation.roll = msg->roll;
            current_transform_pair.current_rotation.pitch = msg->pitch;
            current_transform_pair.current_rotation.yaw = msg->heading;
            current_transform_pair.current_height = msg->heave;
        }
        else
        {
            current_transform_pair.start_rotation.roll = msg->roll;
            current_transform_pair.start_rotation.pitch = msg->pitch;
            current_transform_pair.start_rotation.yaw = msg->heading;
            current_transform_pair.start_height = msg->heave;
            current_transform_pair.current_rotation.roll = msg->roll;
            current_transform_pair.current_rotation.pitch = msg->pitch;
            current_transform_pair.current_rotation.yaw = msg->heading;
            current_transform_pair.current_height = msg->heave;
            first_pashr_received = true;
        }

        // If we have all the required data, calculate the transform
        if (first_gprmc_received && first_gphdt_received && first_pashr_received)
        {
            calculateTransform();
        }
    }
    catch (...) {
        ROS_INFO("Error calculating rotation from ROS PASHR message");
    }
}


void Nmea_Functions::parseNmeaMessage(std::string message){
    try{
        std::vector<std::string> result;
        std::stringstream s_stream(message);
        while(s_stream.good()) {
            std::string substr;
            getline(s_stream, substr, ',');
            result.push_back(substr);
        }

        // Check message type and parse accordingly
        auto message_type = result[0];

        // This message type can be used to get lat and long
        // requires a minimum of two of thse messages to calculate a translation
        if (message_type == "$GPRMC")
        {
            Nmea_Functions::NmeaGprmcMessage message {};
            message.message_id = message_type;
            message.position_utc = std::stof(result[1]);
            message.status = result[2].at(0);
            message.latitude = std::stof(result[3]);
            message.latitude_dir = result[4].at(0);
            message.longitude = std::stof(result[5]);
            message.longitude_dir = result[6].at(0);
            message.speed = std::stof(result[7]);
            message.angle = std::stof(result[8]);
            message.date = result[9];
            message.magnetic_variation = std::stof(result[10]);
            message.magnetic_variation_dir = result[11].at(0);
            message.mode_indicator = result[12].at(0);
            message.checksum_data = result[12].substr(1,3);

            NmeaLatLong lat_long {};
            lat_long.latitude = message.latitude;
            lat_long.latitude_hemisphere = message.latitude_dir;
            lat_long.longitude = message.longitude;
            lat_long.longitude_hemisphere = message.longitude_dir;
            auto google_lat_long = nmea_latlong_to_googlemaps_latlong(lat_long);

            if (first_gprmc_received)
            {
                current_transform_pair.current_latlong.latitude = google_lat_long.latitude;
                current_transform_pair.current_latlong.longitude = google_lat_long.longitude;
                gprmc_waiting = true;
            }
            else
            {
                current_transform_pair.start_latlong.latitude = google_lat_long.latitude;
                current_transform_pair.start_latlong.longitude = google_lat_long.longitude;
                current_transform_pair.current_latlong.latitude = google_lat_long.latitude;
                current_transform_pair.current_latlong.longitude = google_lat_long.longitude;
                first_gprmc_received = true;
            }
        }

        // This message type can be used to get heading relative to north
        // requires a minimum of two of thse messages to calculate a translation
        else if (message_type == "$GPHDT")
        {
            Nmea_Functions::nmea_gphdt_message message {};
            message.message_id = message_type;
            message.heading = std::stof(result[1]);
            message.heading_relative = result[2].at(0);
            message.checksum_data = result[2].substr(1,3);

            if (first_gphdt_received)
            {
                current_transform_pair.current_heading = message.heading;
                gprmc_waiting = false;
            }
            else
            {
                current_transform_pair.start_heading = message.heading;
                current_transform_pair.current_heading = message.heading;
                first_gphdt_received = true;
            }
        }

        // This message can be used to get roll, pitch, yaw (heading) and height (heave)
        // only requires a single message of this type to get roll, pitch and yaw
        else if (message_type == "$PASHR")
        {
            Nmea_Functions::nmea_pashr_message message {};
            message.message_id = message_type;
            message.utc_time = std::stof(result[1]);
            message.heading = std::stof(result[2]);
            message.true_heading_flag = result[3].at(0);
            message.roll = std::stof(result[4]);
            message.pitch = std::stof(result[5]);
            message.heave = std::stof(result[6]);
            message.roll_accuracy = std::stof(result[7]);
            message.pitch_accuracy = std::stof(result[8]);
            message.heading_accuracy = std::stof(result[9]);
            message.aiding_status = std::stof(result[10]);
            message.imu_status = std::stof(result[11]);
            message.checksum_data = result[11].substr(1,3);

            if (first_pashr_received)
            {
                current_transform_pair.current_rotation.roll = message.roll;
                current_transform_pair.current_rotation.pitch = message.pitch;
                current_transform_pair.current_rotation.yaw = message.heading;
                current_transform_pair.current_height = message.heave;
            }
            else
            {
                current_transform_pair.start_rotation.roll = message.roll;
                current_transform_pair.start_rotation.pitch = message.pitch;
                current_transform_pair.start_rotation.yaw = message.heading;
                current_transform_pair.start_height = message.heave;
                current_transform_pair.current_rotation.roll = message.roll;
                current_transform_pair.current_rotation.pitch = message.pitch;
                current_transform_pair.current_rotation.yaw = message.heading;
                current_transform_pair.current_height = message.heave;
                first_pashr_received = true;
            }
        }
        else
        {
            ROS_INFO("%s", message_type.c_str());
            Nmea_Functions::NmeaBaseMessage message {};
        }

        // If we have all the required data, calculate the transform
        if (first_gprmc_received && first_gphdt_received && first_pashr_received)
        {
            calculateTransform();
        }
    }
    catch (...) {
            ROS_INFO("Message parsing error");
    }
}