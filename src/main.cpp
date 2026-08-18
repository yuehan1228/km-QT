#include <QApplication>
#include <rclcpp/rclcpp.hpp>
#include <km_custom_msgs/msg/plc_status.hpp>
#include <km_custom_msgs/msg/ship_loader_pose.hpp>
#include <km_custom_msgs/msg/heart_beat.hpp>
#include <km_custom_msgs/msg/work_flow_status.hpp>
#include <km_custom_msgs/msg/ship_loader_speed.hpp>
#include <km_custom_msgs/msg/ship_loader_target_pose.hpp>
#include <km_custom_msgs/msg/hatch_position.hpp>
#include <km_custom_msgs/msg/status_code.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include "shiploader_monitor/main_window.h"

int main(int argc, char* argv[])
{
  // ── 1. 初始化 ROS2（必须在 QApplication 之前） ───────────────
  rclcpp::init(argc, argv);

  // ── 2. 创建 QT Application ───────────────────────────────────
  QApplication app(argc, argv);
  app.setApplicationName("ShipLoader Monitor");
  app.setApplicationVersion("0.1.0");

  // ── 2.5 注册 ROS2 消息类型到 Qt 类型系统（跨线程信号/槽必需） ──
  qRegisterMetaType<km_custom_msgs::msg::PlcStatus::SharedPtr>("km_custom_msgs::msg::PlcStatus::SharedPtr");
  qRegisterMetaType<km_custom_msgs::msg::ShipLoaderPose::SharedPtr>("km_custom_msgs::msg::ShipLoaderPose::SharedPtr");
  qRegisterMetaType<km_custom_msgs::msg::HeartBeat::SharedPtr>("km_custom_msgs::msg::HeartBeat::SharedPtr");
  qRegisterMetaType<km_custom_msgs::msg::WorkFlowStatus::SharedPtr>("km_custom_msgs::msg::WorkFlowStatus::SharedPtr");
  qRegisterMetaType<km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr>("km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr");
  qRegisterMetaType<km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr>("km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr");
  qRegisterMetaType<km_custom_msgs::msg::HatchPosition::SharedPtr>("km_custom_msgs::msg::HatchPosition::SharedPtr");
  qRegisterMetaType<km_custom_msgs::msg::StatusCode::SharedPtr>("km_custom_msgs::msg::StatusCode::SharedPtr");
  qRegisterMetaType<sensor_msgs::msg::PointCloud2::SharedPtr>("sensor_msgs::msg::PointCloud2::SharedPtr");
  qRegisterMetaType<visualization_msgs::msg::MarkerArray::SharedPtr>("visualization_msgs::msg::MarkerArray::SharedPtr");
  qRegisterMetaType<visualization_msgs::msg::Marker::SharedPtr>("visualization_msgs::msg::Marker::SharedPtr");
  // 注册参数读写需要的 Qt 容器类型
  qRegisterMetaType<QMap<QString,double>>("QMap<QString,double>");
  qRegisterMetaType<QMap<QString,int>>("QMap<QString,int>");
  qRegisterMetaType<QMap<QString,bool>>("QMap<QString,bool>");

  // ── 3. 创建并显示主窗口 ──────────────────────────────────────
  shiploader_monitor::MainWindow mainWindow;
  mainWindow.show();

  // ── 4. 初始化 ROS 连接（创建节点 + 启动 spin 线程） ──────────
  mainWindow.init();

  // ── 5. 运行 QT 事件循环 ──────────────────────────────────────
  int ret = app.exec();

  // ── 6. 清理 ROS2 ─────────────────────────────────────────────
  rclcpp::shutdown();

  return ret;
}
