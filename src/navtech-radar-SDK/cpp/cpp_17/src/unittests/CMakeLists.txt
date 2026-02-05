include(GoogleTest)

add_executable(
    unittests
    given_an_IP_address.cpp
    given_a_port.cpp
    given_a_circular_buffer.cpp
    given_protobuf_helpers.cpp
    given_a_monotonic_clock.cpp
    given_a_realtime_clock.cpp
    given_an_event_dispatcher.cpp
    given_net_conversion_functions.cpp
    given_a_timer.cpp
    given_a_colossus_network_message.cpp
    given_a_discovery_network_message.cpp
    given_a_CP_network_message.cpp
    given_an_angle.cpp
    given_a_coordinate.cpp
    given_a_pointcloud_spoke.cpp
    given_a_cfar_algorithm.cpp
    given_a_statistical_value.cpp
    given_a_FIFO.cpp
    given_net_conversion_functions.cpp
    given_a_centre_of_mass.cpp
    given_a_shape_finder.cpp
    given_a_string_helper.cpp
    given_a_threadsafe_queue.cpp
    given_an_option_parser.cpp
    given_FFT_types.cpp
    given_fixed_point_types.cpp
    given_a_csv_parser.cpp
    given_radar_features.cpp
)

target_link_libraries(unittests
    gtest_main
    gmock
    utility
    networking
    protobuf
)

gtest_discover_tests(unittests)

