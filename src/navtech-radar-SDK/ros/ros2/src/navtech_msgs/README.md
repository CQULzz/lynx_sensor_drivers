# navtech_msgs

The navtech_msgs ROS package contains the *custom message definitions*, as used in the accompanying ROS packages. For
further details of message fields see brief field descriptions in .msg files.

## navtech_msgs/msg

The msg folder contains the files which represent the *custom ROS message definitions*.

### navtech_msgs/msg/CameraConfigurationMessage.msg

This file contains the custom message which defines the *camera configuration*, the *field names and types*
are as detailed below.

| Data Type      | Name         |
| :------------- | :----------: |
| uint32         | height       |
| uint32         | width        |
| uint32         | channels     |
| uint32         | fps          |
	
### navtech_msgs/msg/RadarConfigurationMsg.msg

This file contains the custom message which defines the *radar configuration*, the *field names and types*
are as detailed below.

| Data Type      | Name                   |
| :------------- | :--------------------: |
| uint16         | azimuth_samples        |
| uint16         | encoder_size           |
| uint16         | bin_size               |
| float32        | range_in_bins          |
| uint16         | expected_rotation_rate |
| float32        | range_gain             |
| float32        | range_offset           |

### navtech_msgs/msg/RadarFftDataMsg.msg

This file contains the custom message which defines the *radar fft data*, the *field names and types* are as
detailed below.

| Data Type      | Name              |
| :------------- | :---------------: |
| float64        | angle             |
| uint16         | azimuth           |
| uint16         | sweep_counter     |
| uint32         | ntp_seconds       |
| uint32         | ntp_split_seconds |
| uint8[]        | data              |
| uint16         | data_length       |

## CMakeLists

This is the *build file* which defines how the camera_ros ROS package is built and installed.

## package

This file contains *properties* of the camera_ros package, such as package name, versions, authors etc.

Note - these properties are not intended to be edited by the user, these are *build related properties*