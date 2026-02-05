import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():

  package_name = "nav_radar"
  executable_name = "colossus_publisher"
  config_file_name = "colossus_publisher.yaml"
  
  share_dir = get_package_share_directory(package_name)
  current_dir = os.getcwd()
  current_folder = os.path.split(current_dir)[-1]

  if (current_folder != "launch"):
    print("Please launch from the launch directory")
    print("Example - /iasdk-public/ros/ros2/src/nav_launch/launch")
    exit()

  base_dir = os.path.normpath(os.path.join(current_dir, "../..",))
  config_path = os.path.join(base_dir, package_name, "config", config_file_name)

  print(f"Executable directory: {share_dir}")
  print(f"Executable name: {executable_name}")
  print(f"Executable config path: {config_path}")

  return LaunchDescription([

    Node(
        package=package_name,
        parameters=[config_path],
        executable=executable_name
    )
  ])