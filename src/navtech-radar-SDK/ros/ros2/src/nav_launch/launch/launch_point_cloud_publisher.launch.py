import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

  package_name = "nav_radar"
  executable_name = "point_cloud_publisher"
  config_file_name = "point_cloud_publisher.yaml"
  rviz_file_name = "point_cloud_view.rviz"
  
  share_dir = get_package_share_directory(package_name)
  current_dir = os.getcwd()
  current_folder = os.path.split(current_dir)[-1]

  if (current_folder != "launch"):
    print("Please launch from the launch directory")
    print("Example - /iasdk-public/ros/ros2/src/nav_launch/launch")
    exit()

  base_dir = os.path.normpath(os.path.join(current_dir, "../..",))
  config_path = os.path.join(base_dir, package_name, "config", config_file_name)
  rviz_path = os.path.join(base_dir, "nav_rviz", rviz_file_name)

  print(f"Executable directory: {share_dir}")
  print(f"Executable name: {executable_name}")
  print(f"Executable config path: {config_path}")
  print(f"RVIZ Config path: {rviz_path}")

  return LaunchDescription([

    Node(
        package=package_name,
        parameters=[config_path],
        executable=executable_name
    ),

    Node(
        package="tf2_ros",
        arguments = ["0", "0", "0", "0", "0", "0", "map", "point_cloud"],
        executable="static_transform_publisher"
    ),

    Node(
        package="rviz2",
        arguments=["-d" + rviz_path],
        executable="rviz2"
    )
  ])