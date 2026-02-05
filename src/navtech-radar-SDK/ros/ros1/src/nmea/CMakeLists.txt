## Define project name
project(nmea_ros)

## Define the SDK path
set(SDK_PATH ${PROJECT_SOURCE_DIR}/../../../../cpp/cpp_17/src)

## Set the Cmake module path (used for Botan)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SDK_PATH}/cmake/modules/")

## Add compile options
add_compile_options(
    -std=c++17
    -Wall
    -Wextra
    -pthread
    $<$<CONFIG:DEBUG>:-ggdb>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:RELEASE>:-O3>
)

## Find required packages for build
# find_package(roscpp REQUIRED)
# find_package(tf2 REQUIRED)
# find_package(tf2_ros REQUIRED)
# find_package(messages REQUIRED) 
# find_package(std_msgs REQUIRED)
# find_package(sensor_msgs REQUIRED)
# find_package(OpenCV 4.5.3 REQUIRED)

# This is required for the networking components
find_package(catkin REQUIRED COMPONENTS
          roscpp
          rospy
          message_generation
          std_msgs
          geometry_msgs
          sensor_msgs
          tf
          )

################################################
## Declare ROS messages, services and actions ##
################################################

# ## Generate messages in the 'msg' folder
add_message_files(
  DIRECTORY
  msg
  FILES
  nmea_gpgga_message.msg
  nmea_gpgst_message.msg
  nmea_gphdt_message.msg
  nmea_gprmc_message.msg
  nmea_pashr_message.msg
)

# ## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
  tf
)

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a exec_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
# ##     and list every .cfg file to be processed
# generate_dynamic_reconfigure_options(
#    cfg/dynamic_param.cfg
# )


###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
# catkin_package(
#   CATKIN_DEPENDS roscpp)


## Build executable for NMEA Transform publisher
add_executable(nmea_transform_publisher
src/publishers/nmea_transform_publisher_main.cpp
src/publishers/nmea_transform_publisher.cpp
src/common/nmea_functions.cpp)

target_link_libraries(nmea_transform_publisher
${catkin_LIBRARIES}
networking
utility)

add_dependencies(nmea_transform_publisher nmea_ros_generate_messages_cpp )

## Build executable for NMEA Publisher
add_executable(nmea_publisher
src/publishers/nmea_publisher_main.cpp
src/publishers/nmea_publisher.cpp
src/common/nmea_functions.cpp)

target_link_libraries(nmea_publisher
${catkin_LIBRARIES}
networking
utility)

add_dependencies(nmea_publisher nmea_ros_generate_messages_cpp )

# ament_target_dependencies(nmea_publisher
# rclcpp
# std_msgs
# messages)

## Build executable for NMEA Subscriber
add_executable(nmea_subscriber
src/subscribers/nmea_subscriber_main.cpp
src/subscribers/nmea_subscriber.cpp
src/common/nmea_functions.cpp)

target_link_libraries(nmea_subscriber ${catkin_LIBRARIES})

add_dependencies(nmea_subscriber nmea_ros_generate_messages_cpp )
# ament_target_dependencies(nmea_subscriber
# rclcpp
# messages)

## Build executable for NMEA Subscriber transform publisher
add_executable(nmea_subscriber_transform_publisher
src/publishers/nmea_subscriber_transform_publisher_main.cpp
src/publishers/nmea_subscriber_transform_publisher.cpp
src/common/nmea_functions.cpp)

target_link_libraries(nmea_subscriber_transform_publisher
${catkin_LIBRARIES}
networking
utility)

add_dependencies(nmea_subscriber_transform_publisher nmea_ros_generate_messages_cpp )
## Install the built executables
install(TARGETS
  nmea_transform_publisher
  nmea_publisher
  nmea_subscriber
  nmea_subscriber_transform_publisher
  DESTINATION lib/${PROJECT_NAME})

## Install the package
# ament_package()