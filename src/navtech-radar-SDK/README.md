Navtech IA SDK
================

The Navtech IA SDK provides a basic interface to communicate with the IA sensor. The SDK provides the source code for C++ and a .NET DLL that can easily be integrated into applications running on Windows and Linux.


## License
See file `LICENSE.txt` or go to <https://opensource.org/licenses/MIT> for full license details.

## Operation
The IA sensor operates in two modes:

* native data - Used to receive the raw fft data from the sensor

* plot extraction - Used to receive only points where an object is present

The SDK provides support for both modes, allowing developers to receive either type of data from the sensor and also provides an interface to send control and configuration messages to the sensor.

Communication between the sensor and the software SDK is over Ethernet and utilises a proprietary binary communication protocol called _Colossus Network Protocol_. The SDK abstracts this protocol to avoid having to develop the low-level socket and messaging processing code. The [Colossus Protocol documentation can be found here](https://navtechradar.atlassian.net/wiki/display/PROD/Colossus+Network+Data+Protocol).

## Directory Overview
The following are the top-level directories of the SDK. The SDK supports multiple languages and each sub-directory contains a language-idiomatic implementation - that is, an implementation of the SDK for a particular language is written in the programming style of that language.

Further documentation, on how to build an use the SDK in your projects, is contained in the appropriate sub-directory.

### cpp

Contains the old (depreciated) version of the IASDK, built using the c++11 standard. No further changes will be made to this version of the SDK

### cpp_17

Contains the new version of the IASDK, built using the c++17 standard. This folder is configured to use VS Code Build Task and CMake Extensions to enable fast development setup times.

### csharp

Contains a C# version of the IASDK

### iasdk

Contains the IASDK Visual Studio solution files

### protobuf

Contains the protobuf and python protobuf files used in the IASDK

### ros/ros1

Contains the (now depreciated) ROS1 implementation of the IASDK

### ros/ros2

Contains the new ROS2 implementation of the IASDK

### python_nmea_client

Contains a basic python client and example(s) for working with a NMEA sensor

### python_radar_client

Contains a basic python client and example(s) for connecting to and reading/sending data to a Navtech radar

Warning - these Python Radar SDK examples are not fully supported on Windows or Mac, nor are they disgned for use in a production envioronment. These examples are solely
for demonstration of what can be achieved with the Radar SDK, and hence behaviour of these examples is undefined in these scenarios. For details of why these examples
behave differently when not running on a Linux platform, please look at the differences in thread spawning behaviour for the multiprocessing module, for different
operating systems (Linux uses fork by default, Windows/Mac do not currently support fork)

### python_recorded radar_client

Contains a basic python radar recording file reader and example(s) for reading and using the data in the recorded data file