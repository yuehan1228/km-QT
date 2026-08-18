#include "shiploader_monitor/ros_node.h"

namespace shiploader_monitor {

RosNode::RosNode(QObject* parent)
  : QObject(parent)
{
}

RosNode::~RosNode()
{
  stop();
}

void RosNode::start(std::function<void(rclcpp::Node::SharedPtr)> init_cb)
{
  if (running_.load()) return;
  running_.store(true);

  spin_thread_ = std::thread(&RosNode::spinLoop, this, init_cb);
}

void RosNode::stop()
{
  if (!running_.load()) return;
  running_.store(false);

  if (spin_thread_.joinable()) {
    spin_thread_.join();
  }
}

void RosNode::spinLoop(std::function<void(rclcpp::Node::SharedPtr)> init_cb)
{
  // ── 在 spin 线程内创建 rclcpp::Node（context 绑定到此线程） ──
  auto node = std::make_shared<rclcpp::Node>("shiploader_monitor_node");

  RCLCPP_INFO(node->get_logger(), "Node created in spin thread, creating subscriptions...");

  // ── 先创建 executor 并 add_node ────────────────────────────
  auto executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
  executor->add_node(node);

  // ── 创建所有订阅器 ──────────────────────────────────────────
  auto plc_sub = node->create_subscription<km_custom_msgs::msg::PlcStatus>(
    "/plc_status", 10,
    [this](const km_custom_msgs::msg::PlcStatus::SharedPtr msg) { emit plcStatusReceived(msg); });

  auto pose_sub = node->create_subscription<km_custom_msgs::msg::ShipLoaderPose>(
    "/shiploader_pos", 10,
    [this](const km_custom_msgs::msg::ShipLoaderPose::SharedPtr msg) { emit shipLoaderPoseReceived(msg); });

  auto hb_sub = node->create_subscription<km_custom_msgs::msg::HeartBeat>(
    "/heartbeat", 10,
    [this](const km_custom_msgs::msg::HeartBeat::SharedPtr msg) { emit heartbeatReceived(msg); });

  auto wf_sub = node->create_subscription<km_custom_msgs::msg::WorkFlowStatus>(
    "/workflow_status", 10,
    [this](const km_custom_msgs::msg::WorkFlowStatus::SharedPtr msg) { emit workflowStatusReceived(msg); });

  auto speed_sub = node->create_subscription<km_custom_msgs::msg::ShipLoaderSpeed>(
    "/ship_loader_speed", 10,
    [this](const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr msg) { emit shipLoaderSpeedReceived(msg); });

  auto tp_sub = node->create_subscription<km_custom_msgs::msg::ShipLoaderTargetPose>(
    "/ship_loader_target_pos", 10,
    [this](const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr msg) { emit targetPoseReceived(msg); });

  auto hatch_sub = node->create_subscription<km_custom_msgs::msg::HatchPosition>(
    "/hatch_position", 10,
    [this](const km_custom_msgs::msg::HatchPosition::SharedPtr msg) { emit hatchPositionReceived(msg); });

  auto status_sub = node->create_subscription<km_custom_msgs::msg::StatusCode>(
    "/status_code", 10,
    [this](const km_custom_msgs::msg::StatusCode::SharedPtr msg) { emit statusCodeReceived(msg); });

  auto ship_model_sub = node->create_subscription<sensor_msgs::msg::PointCloud2>(
    "/ship_model_point_cloud", 1,
    [this](const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
      emit pointCloudReceived("/ship_model_point_cloud", msg);
    });

  auto material_point_sub = node->create_subscription<sensor_msgs::msg::PointCloud2>(
    "/material_point_cloud", 1,
    [this](const sensor_msgs::msg::PointCloud2::SharedPtr msg) { emit pointCloudReceived("/material_point_cloud", msg); });

  auto merged_points_sub = node->create_subscription<sensor_msgs::msg::PointCloud2>(
    "/merged_points", 1,
    [this](const sensor_msgs::msg::PointCloud2::SharedPtr msg) { emit pointCloudReceived("/merged_points", msg); });

  auto filtered_cloud_sub = node->create_subscription<sensor_msgs::msg::PointCloud2>(
    "/filtered_cloud", 1,
    [this](const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
      emit pointCloudReceived("/filtered_cloud", msg);
    });

  auto rviz_arm_sub = node->create_subscription<visualization_msgs::msg::MarkerArray>(
    "/rviz_arm_bbox", 1,
    [this](const visualization_msgs::msg::MarkerArray::SharedPtr msg) {
      emit markerArrayReceived("/rviz_arm_bbox", msg);
    });

  auto rviz_chute_sub = node->create_subscription<visualization_msgs::msg::MarkerArray>(
    "/rviz_chute_bbox", 1,
    [this](const visualization_msgs::msg::MarkerArray::SharedPtr msg) { emit markerArrayReceived("/rviz_chute_bbox", msg); });

  auto rviz_chute_platform_sub = node->create_subscription<visualization_msgs::msg::MarkerArray>(
    "/rviz_chute_platform_bbox", 1,
    [this](const visualization_msgs::msg::MarkerArray::SharedPtr msg) {
      emit markerArrayReceived("/rviz_chute_platform_bbox", msg);
    });

  auto rviz_loadpos_sub = node->create_subscription<visualization_msgs::msg::MarkerArray>(
    "/rviz_loadpos", 1,
    [this](const visualization_msgs::msg::MarkerArray::SharedPtr msg) { emit markerArrayReceived("/rviz_loadpos", msg); });

  auto rviz_target_sub = node->create_subscription<visualization_msgs::msg::MarkerArray>(
    "/rviz_target", 1,
    [this](const visualization_msgs::msg::MarkerArray::SharedPtr msg) { emit markerArrayReceived("/rviz_target", msg); });

  auto update_ship_attitude_marker_sub = node->create_subscription<visualization_msgs::msg::Marker>(
    "/update_ship_attitude_markers", rclcpp::QoS(10),
    [this](const visualization_msgs::msg::Marker::SharedPtr msg) {
      auto copy = std::make_shared<visualization_msgs::msg::Marker>(*msg);
      emit markerReceived("/update_ship_attitude_markers", copy);
    });

  auto ship_attitude_marker_sub = node->create_subscription<visualization_msgs::msg::Marker>(
    "/ship_attitude_markers", rclcpp::QoS(10),
    [this](const visualization_msgs::msg::Marker::SharedPtr msg) {
      // 深拷贝，避免 SharedPtr 被 ROS2 复用
      auto copy = std::make_shared<visualization_msgs::msg::Marker>(*msg);
      emit markerReceived("/ship_attitude_markers", copy);
    });

  RCLCPP_INFO(node->get_logger(), "All subscriptions created");

  // ── 执行外部初始化回调 ─────────────────────────────────────
  if (init_cb) {
    init_cb(node);
  }

  emit connectionStatusChanged(true);

  // ── Spin 循环 ──────────────────────────────────────────────
  while (running_.load() && rclcpp::ok()) {
    executor->spin_once(std::chrono::milliseconds(100));
  }

  emit connectionStatusChanged(false);
  RCLCPP_INFO(node->get_logger(), "Spin loop exited");
}

} // namespace shiploader_monitor
