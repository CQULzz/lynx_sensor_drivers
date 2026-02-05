from launch import LaunchDescription
from launch.actions import ExecuteProcess, RegisterEventHandler, IncludeLaunchDescription
from launch.event_handlers import OnProcessExit
from launch.launch_description_sources import AnyLaunchDescriptionSource
from launch_ros.substitutions import FindPackageShare
from launch_ros.actions import Node

from launch.substitutions import PathJoinSubstitution


def include_launch(pkg_name: str, launch_file: str):
    # 兼容 .launch.py / .py / .xml 等各种 launch 后缀
    return IncludeLaunchDescription(
        AnyLaunchDescriptionSource(
            PathJoinSubstitution([FindPackageShare(pkg_name), "launch", launch_file])
        )
    )


def generate_launch_description():
    # 1) fixposition
    fixposition = include_launch("fixposition_driver_ros2", "node.launch")

    # 2) rslidar
    rslidar = include_launch("rslidar_sdk", "start.py")

    # 3) tm_imu：先 chmod，再启动
    chmod_tty = ExecuteProcess(
        cmd=["bash", "-lc", "chmod 777 /dev/ttyACM0"],
        output="screen",
    )

    # 这里启动 tm_imu 的 imu.launch.py 文件
    tm_imu = include_launch("tm_imu", "imu.launch.py")

    start_tm_imu_after_chmod = RegisterEventHandler(
        OnProcessExit(
            target_action=chmod_tty,
            on_exit=[tm_imu],
        )
    )

    # 4) nav_radar b_scan_publisher with params file
    b_scan_publisher = Node(
        package="nav_radar",
        executable="b_scan_publisher",
        output="screen",
        parameters=["/home/lynx/navtech-radar-SDK/ros/ros2/src/nav_radar/config/b_scan_publisher.yaml"],
    )

    return LaunchDescription([
        fixposition,
        rslidar,
        chmod_tty,
        start_tm_imu_after_chmod,
        b_scan_publisher,
    ])
