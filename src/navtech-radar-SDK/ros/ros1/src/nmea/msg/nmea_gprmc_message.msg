# A ROS message to define an MNEA GPRMC message

# add a header message to hold message timestamp
std_msgs/Header header

# Message ID
string message_id

# UTC of position fix
float32 position_utc

# Status A=active or V=void
char status

# Latitude
float32 latitude

# Direction of Latitude
char latitude_dir

# Longitude
float32 longitude

# Direction of Longitude
char longitude_dir

# Speed over the ground in knots
float32 speed

# Track angle in degrees (True)
float32 angle

# Date
string date

# Magnetic variation, in degrees
float32 magnetic_variation

# Magnetic variation direction
char magnetic_variation_dir

# 	Mode Indicator: A=Autonomous, D=Differential, E=Estimated, F=Float RTK, M=Manual input, N=No fix, P=Precise,
# R=Real time kinematic, S=Simulator
char mode_indicator

# The checksum data, always begins with *
string checksum_data