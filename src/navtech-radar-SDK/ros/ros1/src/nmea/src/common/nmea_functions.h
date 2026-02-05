#ifndef NMEA_FUNCTIONS_H
#define NMEA_FUNCTIONS_H

#include "ros/ros.h"
#include "nmea_ros/nmea_gprmc_message.h"
#include "nmea_ros/nmea_gphdt_message.h"
#include "nmea_ros/nmea_pashr_message.h"


class Nmea_Functions
{
public:
    Nmea_Functions();
    ~Nmea_Functions();

    // Base NMEA message structure
    struct NmeaBaseMessage {
        std::string message_id;
        std::string checksum_data;
    };

    // Structure to store an NMEA $GPRMC message
    struct NmeaGprmcMessage : NmeaBaseMessage {
        float position_utc;
        char status;
        float latitude;
        char latitude_dir;
        float longitude;
        char longitude_dir;
        float speed;
        float angle;
        std::string date;
        float magnetic_variation;
        char magnetic_variation_dir;
        char mode_indicator;
    };

    // Structure to store an NMEA $GPHDT message
    struct nmea_gphdt_message : NmeaBaseMessage {
        float heading;
        char heading_relative;
    };

    // Structure to store an NMEA $PASHR message
    struct nmea_pashr_message : NmeaBaseMessage {
        float utc_time;
        float heading;
        char true_heading_flag;
        float roll;
        float pitch;
        float heave;
        float roll_accuracy;
        float pitch_accuracy;
        float heading_accuracy;
        uint aiding_status;
        uint imu_status;
    };

    // Structure to store an NMEA style lat long
    struct NmeaLatLong {
        double latitude;
        char latitude_hemisphere;
        double longitude;
        char longitude_hemisphere;
    };

    // Structure to store a Google maps style lat long
    struct GoogleMapsLatLong {
        double latitude;
        double longitude;
    };

    // Structure to store rotation data
    struct RotationPair {
        double roll;
        double pitch;
        double yaw;
    };

    // Structure to store all info required to create a translation and rotation
    struct TransformPair {
        GoogleMapsLatLong start_latlong;
        GoogleMapsLatLong current_latlong;
        double start_heading;
        double current_heading;
        RotationPair start_rotation;
        RotationPair current_rotation;
        double start_height;
        double current_height;
    };

    // Structure to store a Googlemaps style lat long pair
    struct LatLongPair {
        double start_lat;
        double start_long;
        double start_heading;
        double current_lat;
        double current_long;
        double current_heading;
    };

    // Structure to store X, Y and Euclidian distance translation
    struct XYDistance {
        double x_dist;
        double y_dist;
        double euclidian_dist;
    };

    // Structure to store X, Y, Z, roll, pitch, yaw
    struct TranslationRotation {
        double x;
        double y;
        double z;
        double roll;
        double pitch;
        double yaw;
    };

    // Convert Googlemaps style coordinates back to NMEA style coordinates
    NmeaLatLong googlemaps_latlong_to_nmea_latlong(GoogleMapsLatLong);

    // NMEA lat long needs to be converted to Googlemaps style coordinates
    GoogleMapsLatLong nmea_latlong_to_googlemaps_latlong(NmeaLatLong);

    // Convert a lat, long and bearing to an X, Y translation in metres
    XYDistance latlongbearing_to_xy_translation (LatLongPair lat_long_pair);

    // Parse a ROS format GPRMC message
    void parseRosGprmc(const nmea_ros::nmea_gprmc_message::Ptr msg);

    // Parse a ROS format GPHDT message
    void parseRosGphdt(const nmea_ros::nmea_gphdt_message::Ptr msg);

    // Parse a ROS format PASHR message
    void parseRosPashr(const nmea_ros::nmea_pashr_message::Ptr msg);

    // Parse an NMEA message string
    void parseNmeaMessage(std::string message);

    // Calculate the transformation data
    void calculateTransform();

    // Get the latest transform data
    TranslationRotation getLatestTransformData();

private:

    // Check to see if we have received our first mesages of each type
    bool first_gprmc_received {};
    bool first_gphdt_received {};
    bool first_pashr_received {};

    // Check to see if we have a new gprmc message that is waiting for it's counterpart gphdt message
    bool gprmc_waiting {};

    // The coordinate pairs from which to calculate translation and rotation
    TransformPair current_transform_pair;
    TranslationRotation transform_data;
};

#endif