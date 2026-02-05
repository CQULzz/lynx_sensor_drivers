cmake_minimum_required(VERSION 3.16)

project(sdk_protobuf C CXX)

set(PROTOBUF_DIR ../../../../protobuf)

set(protobuf_files 
    ${PROTOBUF_DIR}/configurationdata.proto 
    ${PROTOBUF_DIR}/discovery.proto 
    ${PROTOBUF_DIR}/health.proto 
    ${PROTOBUF_DIR}/healthinfo.proto 
    ${PROTOBUF_DIR}/networkinfo.proto 
    ${PROTOBUF_DIR}/nvramcontents.proto 
    ${PROTOBUF_DIR}/radarmodel.proto 
    ${PROTOBUF_DIR}/softwareversions.proto
    ${PROTOBUF_DIR}/tls.proto
    ${PROTOBUF_DIR}/timestatus.proto
)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${protobuf_files})

message(STATUS "Protobuf current output dir: ${CMAKE_CURRENT_BINARY_DIR}")

add_library(protobuf STATIC 
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
)

target_include_directories(protobuf INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(protobuf PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(protobuf ${PROTOBUF_LIBRARY})
