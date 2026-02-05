# A ROS message to define an MNEA GPGST message

# add a header message to hold message timestamp
std_msgs/Header header

# Message ID
string message_id

# UTC of position fix
float32 position_utc

# RMS value of the pseudorange residuals; includes carrier phase residuals during periods of RTK (float) and RTK (fixed) processing
float32 pseudorange_residual_rms

# Error ellipse semi-major axis 1-sigma error, in meters
float32 ellipse_major_sigma_error

# Error ellipse semi-minor axis 1-sigma error, in meters
float32 ellipse_minor_sigma_error

# Error ellipse orientation, degrees from true north
float32 ellipse_orientation_error

# Latitude 1-sigma error, in meters
float32 latitude_sigma_error

# Longitude 1-sigma error, in meters
float32 longitude_sigma_error

# Height 1-sigma error, in meters
float32 height_sigma_error

# The checksum data, always begins with *
string checksum_data