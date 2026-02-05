cmake_minimum_required(VERSION 3.16)

project(sdk_utilities C CXX)

if (PLATFORM STREQUAL "win64")

    message(STATUS "Fetching ZLIB from source...")

    include(FetchContent)
    
    include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    
    FetchContent_Declare(
        zlib
        GIT_REPOSITORY "https://github.com/madler/zlib.git"
        FIND_PACKAGE_ARGS NAMES ZLIB
        )
        
    FetchContent_MakeAvailable(zlib)
    set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR})
    set(ZLIB_LIBRARY zlibstatic CACHE STRING "")
    set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
    set(ZLIB_FOUND 1)
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
    include(FindZLIB)
else ()

endif ()

find_package(ZLIB REQUIRED)

add_library(utility STATIC
    ${PROJECT_SOURCE_DIR}/geometry/Cartesian_coordinate.cpp
    ${PROJECT_SOURCE_DIR}/geometry/Euclidean_coordinate.cpp
    ${PROJECT_SOURCE_DIR}/geometry/Polar_coordinate.cpp
    ${PROJECT_SOURCE_DIR}/geometry/Spherical_coordinate.cpp

    ${PROJECT_SOURCE_DIR}/system/Log.cpp
    ${PROJECT_SOURCE_DIR}/system/Log_format.cpp
    ${PROJECT_SOURCE_DIR}/system/Signal_handler.cpp
    ${PROJECT_SOURCE_DIR}/system/Option_parser/Option_parser.cpp
    ${PROJECT_SOURCE_DIR}/system/Option_parser/Option_group.cpp
    ${PROJECT_SOURCE_DIR}/system/Option_parser/Option.cpp

    ${PROJECT_SOURCE_DIR}/threading/Active.cpp
    ${PROJECT_SOURCE_DIR}/threading/Mutex.cpp

    ${PROJECT_SOURCE_DIR}/time/Duration.cpp
    ${PROJECT_SOURCE_DIR}/time/Monotonic_observation.cpp
    ${PROJECT_SOURCE_DIR}/time/Realtime_observation.cpp
    ${PROJECT_SOURCE_DIR}/time/Threadsafe_duration.cpp
    ${PROJECT_SOURCE_DIR}/time/Threadsafe_monotonic.cpp
    ${PROJECT_SOURCE_DIR}/time/Threadsafe_realtime.cpp
    ${PROJECT_SOURCE_DIR}/time/Timer.cpp

    ${PROJECT_SOURCE_DIR}/math/Compression_utils.cpp
)

target_include_directories(utility INTERFACE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/architecture
    ${PROJECT_SOURCE_DIR}/containers
    ${PROJECT_SOURCE_DIR}/geometry
    ${PROJECT_SOURCE_DIR}/math
    ${PROJECT_SOURCE_DIR}/protobuf
    ${PROJECT_SOURCE_DIR}/radar_client
    ${PROJECT_SOURCE_DIR}/security
    ${PROJECT_SOURCE_DIR}/string
    ${PROJECT_SOURCE_DIR}/system
    ${PROJECT_SOURCE_DIR}/threading
    ${PROJECT_SOURCE_DIR}/time
    ${PROJECT_SOURCE_DIR}/units
    ${PROJECT_SOURCE_DIR}/system/Option_parser
)

target_include_directories(utility PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/architecture
    ${PROJECT_SOURCE_DIR}/containers
    ${PROJECT_SOURCE_DIR}/geometry
    ${PROJECT_SOURCE_DIR}/math
    ${PROJECT_SOURCE_DIR}/protobuf
    ${PROJECT_SOURCE_DIR}/radar_client
    ${PROJECT_SOURCE_DIR}/security
    ${PROJECT_SOURCE_DIR}/string
    ${PROJECT_SOURCE_DIR}/system
    ${PROJECT_SOURCE_DIR}/threading
    ${PROJECT_SOURCE_DIR}/time
    ${PROJECT_SOURCE_DIR}/units
    ${PROJECT_SOURCE_DIR}/system/Option_parser
)

target_link_libraries(utility PRIVATE ${CMAKE_THREAD_LIBS_INIT} ZLIB::ZLIB)