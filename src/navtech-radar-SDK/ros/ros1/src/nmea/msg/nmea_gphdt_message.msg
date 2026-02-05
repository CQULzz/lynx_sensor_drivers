# A ROS message to define an MNEA GPHDT message

# add a header message to hold message timestamp
std_msgs/Header header

# Message ID
string message_id

# Heading in degrees
float32 heading

# T: Indicates heading relative to True North
char heading_relative

# The checksum data, always begins with *
string checksum_data