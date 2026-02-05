# A ROS message to define an MNEA PASHR message

# add a header message to hold message timestamp
std_msgs/Header header

# Message ID
string message_id

# UTC time. mm is minutes, ss.sss is seconds
float32 utc_time

# hhh.hh - Heading in degrees
float32 heading

# T - flag to indicate that the Heading is True Heading (i.e. to True North)
char true_heading_flag

# rrr.rr - Roll Angle in degrees
float32 roll

# ppp.pp - Pitch Angle in degrees
float32 pitch

# xxx.xx - Heave
float32 heave

# a.aaa - Roll Angle Accuracy Estimate (Stdev) in degrees
float32 roll_accuracy

# b.bbb - Pitch Angle Accuracy Estimate (Stdev) in degrees
float32 pitch_accuracy

# c.ccc - Heading Angle Accuracy Estimate (Stdev) in degrees
float32 heading_accuracy

# d - Aiding Status
uint8 aiding_status

# e - IMU Status
uint8 imu_status

# The checksum data, always begins with *
string checksum_data