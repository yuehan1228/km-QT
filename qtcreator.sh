#!/bin/bash
# 在 ROS2 环境中启动 QT Creator 并打开 ShipLoaderMonitor 项目
source /opt/ros/humble/setup.bash
source /home/kk/code/ros2/project/ShipLoader/install/setup.bash
qtcreator /home/kk/code/QT/ShipLoaderMonitor/CMakeLists.txt &
