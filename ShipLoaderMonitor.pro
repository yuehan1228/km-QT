# ============================================================================
# ShipLoader Monitor - QT Creator .pro 工程文件
#
# 用途：在 QT Creator 中用 qmake 打开此文件来编辑 UI 和代码。
#       实际编译请使用 CMakeLists.txt（ROS2 集成）。
#
# 用法：
#   source /opt/ros/humble/setup.bash
#   source /home/yh/workspace/km-shiploader/install/setup.bash
#   qtcreator ShipLoaderMonitor.pro
# ============================================================================

QT       += core gui widgets opengl
TEMPLATE = app
TARGET   = shiploader_monitor
CONFIG   += c++17

# ── ROS2 相关配置 ──────────────────────────────────────────────────
INCLUDEPATH += include
INCLUDEPATH += /opt/ros/humble/include
INCLUDEPATH += /home/yh/workspace/km-shiploader/install/km_custom_msgs/include
INCLUDEPATH += /home/yh/workspace/km-shiploader/install/km_custom_msgs/include/km_custom_msgs

# ── 源文件 ─────────────────────────────────────────────────────────
SOURCES += \
    src/main.cpp \
    src/main_window.cpp \
    src/ros_node.cpp \
    src/data_store.cpp \
    src/config_manager.cpp \
    src/param_writer.cpp \
    src/plc_status_widget.cpp \
    src/pose_display_widget.cpp \
    src/joint_state_widget.cpp \
    src/workflow_status_widget.cpp \
    src/heartbeat_widget.cpp \
    src/log_widget.cpp \
    src/param_edit_widget.cpp

# ── 头文件（MOC 需要） ─────────────────────────────────────────────
HEADERS += \
    include/shiploader_monitor/main_window.h \
    include/shiploader_monitor/ros_node.h \
    include/shiploader_monitor/data_store.h \
    include/shiploader_monitor/config_manager.h \
    include/shiploader_monitor/param_writer.h \
    include/shiploader_monitor/plc_status_widget.h \
    include/shiploader_monitor/pose_display_widget.h \
    include/shiploader_monitor/joint_state_widget.h \
    include/shiploader_monitor/workflow_status_widget.h \
    include/shiploader_monitor/heartbeat_widget.h \
    include/shiploader_monitor/log_widget.h \
    include/shiploader_monitor/param_edit_widget.h

# ── UI 文件 ────────────────────────────────────────────────────────
FORMS += \
    ui/main_window.ui
