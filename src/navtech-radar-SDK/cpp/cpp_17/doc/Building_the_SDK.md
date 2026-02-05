Building the SDK
================

This document gives basic instructions for building the SDK.  It covers both Linux and Windows.

## Prerequisites
### CMake
The SDK is designed to be built using the CMake tools.  These must be installed onto your build platform.

CMake download and installation instructions can be found [here](https://cmake.org/download/).


### Compiler
The SDK is built using C++17.  Your compiler must support this version of the language as a minimum.


### Linux
The SDK requires the linux build essentials and the Google protocol buffer libraries to compile.
For more information on installing protocol buffers, see [Google Protocol buffers](https://github.com/protocolbuffers/protobuf/)

```shell
sudo apt install    \
  build-essential   \
  clang             \
  g++               \
  protobuf-compiler \
  libprotobuf-dev   \
  libbotan-2-dev    \
  cmake
```

### Windows
The SDK has been tested with Visual Studio 2017; and _should_ work with any later version.
The SDK has not been tested with MinGW.

Building the SDK using CMake allows it to be built from the command-line (which is what these instructions do). CMake on Windows will generate a `.sln` file that may be opened with Visual Studio if required.

For more information on getting started with Visual Studio, go [here](https://code.visualstudio.com/learn/get-started/basics)


## Building
### Linux
This assumes that the SDK has been cloned into the folder `~/iasdk-public`.

To simplify the (arcane) CMake syntax, a simple batch script has been written to automate the build configuration process.  The script creates a folder, `build`, which holds the build artefacts and output.

```shell
cd ~/iasdk-public/cpp/cpp_17
scripts/linux/bootstrap.sh [ debug | release ]
```
Unless otherwise specified, the default build type is `Debug`.

To build the SDK CMake syntax may be used, for example:
```shell
cd ~/iasdk-public/cpp/cpp_17
cmake --build build/linux/Debug --config Debug -j
```

A simple script, `build.sh` has also been provided, to avoid the need for memorizing CMake syntax:
```shell
cd ~/iasdk-public/cpp/cpp_17
scripts/linux/build.sh [ debug | release ]
```

The output executables are placed in the directory `iasdk-public/cpp/cpp_17/build/<platform>/<build-type>/bin`.
For example:
```shell
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/colossus_client
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/navigation_client
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/cat240_client
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/nmea_server
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/nmea_client
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/pointcloud_3d_writer
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/pointcloud_client
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/pointcloud_target_writer
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/pointcloud_writer
iasdk-public/cpp/cpp_17/build/linux/Debug/bin/tcp_relay
```

Where `<build-type>` - build configuration - either `Debug` or `Release` 

### Windows
This assumes that the SDK has been cloned into the folder `iasdk-public`.

To simplify the (arcane) CMake syntax, a simple batch script has been written to automate the build configuration process.  The script creates a folder, `build`, which holds the build artifacts and output.

```shell
cd ~/iasdk-public/cpp/cpp_17
scripts\win64\bootstrap.bat [ debug | release ]
```
Unless otherwise specified, the default build type is `Debug`.

To build the SDK CMake syntax may be used, for example:
```shell
cd ~/iasdk-public/cpp/cpp_17
cmake --build build\win64\ --config Release -j
```

Alternatively, CMake will generate an ia_sdk.sln file in `.\build\win64`.
This file can be open in Visual Studio, which can then be used to build the SDK using the Build/Rebuild Solution menu items.

A simple script, `build.bat` has also been provided, to avoid the need for memorizing CMake syntax:
```shell
cd ~/iasdk-public/cpp/cpp_17
scripts\win64\build.bat [ debug | release ]
```

The output executables are placed in thr directory `iasdk-public\cpp\cpp_17\build\<platform>\bin\<build-type>`, below the build-type folder.  For example:
```shell
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\colossus_client
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\navigation_client
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\cat240_client
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\nmea_server
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\nmea_client
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\pointcloud_3d_writer
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\pointcloud_client
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\pointcloud_target_writer
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\pointcloud_writer
iasdk-public\cpp\cpp_17\build\win64\bin\Debug\tcp_relay
```

## Using the VS Code build tasks
The `.vscode` folder contains a build task configuration for invoking CMake.
To build:
* hit ***ctrl-shift-b***
* select ***Build***

The following options are available:
```
Clean       Clean the project (next build forces a re-compile of all files)
Configure   Re-run CMake on the project; no build
Build       Compile the debug configuration
```


## Using Visual Studio
Running the CMake configuration (via `scripts\win64\bootstrap.bat`) will generate a Visual Studio `.sln` file, in
`\build\win64`.

The `.sln` can be opened and built in Visual Studio.

The SDK can be setup for debug in Visual Studio as follows:
 
Select and right-click Solution `ia_sdk` in the Solution Explorer.
Select `Properties` from the menu.

On the `Property` page, select `Common Properties`, then `Startup Project`.
Select the `Multiple startup projects` radio button
Select the project(s) you want to start and select the `Start` action.


------------------------------------------------------------------------------------------------------------------------
## Using the iasdk for your own project/out of source build instructions
In order to incorporate the iasdk into your own project, you must edit your CMake file to perform an out of source build of the iasdk. This can be achieved via the following method:


#### Clone the SDK repository
Clone the IA SDK repository.  We recommend you clone it alongside the rest of your project code.


### Update the CMakeLists.txt for your project
The following will set the root src folder of the SDK. This path is relative to your current project's root directory, and must be changed accordingly.
Then, include the iasdk subdirectories to your project.  Note you have to specify where the SDK build objects will be located. Usually you will locate the SDK build objects in the same location as your other build artifacts.
Adding the SDK's CMake modele path to your project's path ensures the SDK can locate any third-party libraries it requires.

Add the iasdk subdirectory to your project.  Note you have to specify where the SDK build objects will be located. Usually you will locate the SDK build objects in the same location as your other build artifacts.

```cmake
<<<<<<< HEAD:cpp/cpp_17/doc/Building_the_SDK.md
# Define the SDK path.
#
set(SDK_PATH ${PROJECT_SOURCE_DIR}/../../iasdk-public/cpp/cpp_17/src)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SDK_PATH}/cmake/modules/")
=======
set(SDK_PATH ${PROJECT_SOURCE_DIR}/../../iasdk/cpp/cpp_17/src)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SDK_PATH}/cmake/modules/")

>>>>>>> b368e0b59fe180ce359836eee0b8c0c439858ef9:cpp/cpp_17/doc/Buliding_the_SDK.md
add_subdirectory(${SDK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/sdk)

```

If using protocol buffers ("protobuf") in your project (for example parsing the protobuf messages received from a radar) then include the following.

```cmake
# Find and include protobuf directories
#
set (Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

<<<<<<< HEAD:cpp/cpp_17/doc/Building_the_SDK.md
=======
```

>>>>>>> b368e0b59fe180ce359836eee0b8c0c439858ef9:cpp/cpp_17/doc/Buliding_the_SDK.md
Link your project to the iasdk libraries.

```cmake
target_link_libraries(<your_project_name>
    # Any other project library dependencies...

    utility  
    protobuf 
    networking 
    navigation
)

```

## Example
Below is a complete CMakeLists.txt file for a project which uses the iasdk and performs an out of source build.

```cmake
# Minimum CMake to build a 'hello world!' executable
#
cmake_minimum_required(VERSION 3.15)

project(<Your project name> C CXX)

add_compile_options(
    -Wall
    -Wextra
    -std=c++17
    $<$<CONFIG:DEBUG>:-ggdb>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:RELEASE>:-O3>
)

add_compile_definitions()

set(DEMO_SRC_FILES ${PROJECT_SOURCE_DIR}/main.cpp)

add_executable(<your executable> ${DEMO_SRC_FILES})

set_target_properties(<your executable>
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Find and include protobuf directories
#
set (Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIR})

# Add the SDK libraries
#
set(SDK_PATH ${PROJECT_SOURCE_DIR}/../../iasdk-public/cpp/cpp_17/src)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SDK_PATH}/cmake/modules/")
add_subdirectory(${SDK_PATH} ${CMAKE_CURRENT_BINARY_DIR}/sdk)

target_link_libraries(<your executable>  
    utility  
    protobuf 
    networking 
    navigation

    # Other library dependencies...
) 
```