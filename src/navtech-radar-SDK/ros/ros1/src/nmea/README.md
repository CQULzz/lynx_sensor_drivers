# nmea_ros

This package contains examples based on a connection to an NMEA sensor. Examples include conencting to and reading
NMEA sensor data, publishing NMEA sensor data, subscribing to NMEA sensor data, and publishing a ROS tf transform using the
NMEA sensor data. Sample config file(s) are also included in the 'config' directory.

## nmea_ros/config

The config folder contains an example *configuration YAML file*, which define the basic *settings* needed to to run the
package executables.

## nmea_ros/src

The src folder contains the actual *cpp and header source files* for the nmea_ros package examples. This folder contains the subfolders 'publishers', 'subscribers' and 'common' (where applicable), to further organise the source files. The files define the following executables:

### nmea_publisher

Contains a basic example of *connecting to an NMEA sesnor*, and *publishing* NMEA messages.

#### configuration options

The following *configuration options* are included in the .yaml settings file in the config folder. These settings can
be changed by *modifying* the values in the .yaml file, prior to launching the executable.

The following options are shown for each of the settings:

* setting name

* data type

* data example

* setting description

* how to change during execution (if applicable)

#####The following options cannot be changed during execution:

| Name           | Type      | Example                  |Description            | How to change |
| :------------- | :-------: | :---------: | :--------------------------------: | :-----------: |
| nmea_ip        | string    | 192.168.0.1 | *The IP address of the NMEA sensor*| N/A           |
| nmea_port      | string    | 9095        | *The NMEA sensor port*             | N/A           |

### nmea_subscriber

Contains a basic example of *receiving NMEA ROS messages*, from the published NMEA topic.

#### configuration options

This executable has no settings which can be modified

### nmea_transform_publisher

Contains a basic example of *connecting to an NMEA sensor*, generating a tf transform from the NMEA
sensor data, and *publishing* the transform on a *ros topic*.

NOTE - This transform is not currently working as expected, as points are not being overlayed as they should.
Todo - Awaiting fix

#### configuration options

The following *configuration options* are included in the .yaml settings file in the config folder. These settings can
be changed by *modifying* the values in the .yaml file, prior to launching the executable.

The following options are shown for each of the settings:

* setting name

* data type

* data example

* setting description

* how to change during execution (if applicable)

#####The following options cannot be changed during execution:

| Name           | Type      | Example       |Description                          | How to change |
| :------------- | :-------: | :-----------: | :---------------------------------: | :-----------: |
| nmea_ip        | string    | 192.168.0.1   | *The IP address of the NMEA sensor* | N/A           |
| nmea_port      | string    | 9095          | *The NMEA sensor port*              | N/A           |
| parent_topic   | string    | 'map'         | *The parent topic of the transform* | N/A           |
| child_topic    | string    | 'point_cloud' | *The child topic of the transform*  | N/A           |

### nmea_subscriber_transform_publisher

Contains a basic example of *subscribing to NMEA ROS messages*, generating a tf transform from the NMEA
sensor data, and *publishing* the transform on a *ros topic*.

#### configuration options

The following *configuration options* are included in the .yaml settings file in the config folder. These settings can
be changed by *modifying* the values in the .yaml file, prior to launching the executable.

The following options are shown for each of the settings:

* setting name

* data type

* data example

* setting description

* how to change during execution (if applicable)

#####The following options cannot be changed during execution:

| Name           | Type      | Example       |Description                          | How to change |
| :------------- | :-------: | :-----------: | :---------------------------------: | :-----------: |
| parent_topic   | string    | 'map'         | *The parent topic of the transform* | N/A           |
| child_topic    | string    | 'point_cloud' | *The child topic of the transform*  | N/A           |

### nmea_functions

Contains some *helper code* with *NMEA related functions*.

#### configuration options

This executable has no settings which can be modified

## CMakeLists.txt

This is the *cmake build file* which defines how the nmea_ros ROS package is built and installed.

## package.xml

This file contains *properties* of the nmea_ros package, such as package name, versions, authors etc.

Note - these properties are not intended to be edited by the user, these are *build related properties*

# Project Build instructions

Both the IASDK and the ROS2 SDK, with all prerequisites, must be installed before building this package.

See higher level README.md files for IASDK and ROS2 SDK install instructions

To build and install all ROS2 pacakges, run the following commands from the ROS2 folder:

```
colcon build

. install/setup.bash
```

To build and install this package only, run the following commands from the ROS2 folder:

```
colcon build --packages-select camera_ros

. install/setup.bash
```