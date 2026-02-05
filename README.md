# lynx_sensor_drivers

Sensor drivers and ROS packages used for the Lynx platform.

**Contents**
- `src/bringup`: launch and bringup configs
- `src/fixposition_driver`: Fixposition GNSS/IMU driver
- `src/lio_sam`: LIO-SAM integration
- `src/navtech-radar-SDK`: Navtech radar SDK and ROS2 driver
- `src/rslidar_msg`: RoboSense message definitions
- `src/rslidar_sdk`: RoboSense driver
- `src/TransducerM_TM341`: transducer driver (TM341)

**Build Notes**
- Each package has its own README with dependencies and usage.
- Generated artifacts (`build/`, `install/`, `log/`) are ignored.

**Navtech Radar ROS2 Quick Start**
- Workspace: `src/navtech-radar-SDK/ros/ros2`
- Install deps: ROS2 for your OS, `rosdep`, and `libbotan-2-dev` (required by `nav_radar`)
- Build:
```bash
source /opt/ros/<distro>/setup.bash
colcon build --symlink-install
```
- If you use conda, prefer system Python for ROS2 builds.
