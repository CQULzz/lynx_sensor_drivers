# A ROS message to define an MNEA GPGGA message

# add a header message to hold message timestamp
std_msgs/Header header

# Message ID
string message_id

# UTC of position fix
float32 position_utc

# Latitude
float32 latitude

# Direction of Latitude
char latitude_dir

# Longitude
float32 longitude

# Direction of Longitude
char longitude_dir

# GPS Quality indicator
uint8 gps_quality

# Number of SV's in use, from 00 to 24+
uint8 num_sv

# HDOP
float32 hdop

# Orthometric height (MSL reference)
float32 orthometric_height

# M: Unit of measure for orthometric height is metres
char orthometric_height_unit

# Geoid seperation
float32 geoid_seperation

# M: Geoid seperation measured in metres
char geoid_seperation_unit

# Age of differential GPS data record, Type 1 or Type 9. Null field when DGPS is not used
float32 differential_gps_data_record_age

# Reference station ID, range 0000 to 4095. A null field when any reference station ID is selected and no corrections
# are received
uint16 reference_station_id

# The checksum data, always begins with *
string checksum_data