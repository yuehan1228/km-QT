#ifndef SHIPLOADER_MONITOR__ROS_NODE_H_
#define SHIPLOADER_MONITOR__ROS_NODE_H_

#include <QObject>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <km_custom_msgs/msg/plc_status.hpp>
#include <km_custom_msgs/msg/ship_loader_pose.hpp>
#include <km_custom_msgs/msg/heart_beat.hpp>
#include <km_custom_msgs/msg/work_flow_status.hpp>
#include <km_custom_msgs/msg/ship_loader_speed.hpp>
#include <km_custom_msgs/msg/ship_loader_target_pose.hpp>
#include <km_custom_msgs/msg/hatch_position.hpp>
#include <km_custom_msgs/msg/status_code.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <functional>
#include <mutex>

namespace shiploader_monitor {

class ParamWriter;

/// @brief ROS2 订阅节点，在独立线程中运行
/// 所有 rclcpp 操作都在 spin 线程内完成
class RosNode : public QObject
{
  Q_OBJECT

public:
  explicit RosNode(QObject* parent = nullptr);
  ~RosNode() override;

  /// 启动 ROS spin 线程。init_cb 在 spin 线程内执行（在 node 创建后、spin 前）
  void start(std::function<void(rclcpp::Node::SharedPtr)> init_cb = {});
  /// 停止 ROS spin 线程
  void stop();
  bool isRunning() const { return running_.load(); }

signals:
  void plcStatusReceived(const km_custom_msgs::msg::PlcStatus::SharedPtr& msg);
  void shipLoaderPoseReceived(const km_custom_msgs::msg::ShipLoaderPose::SharedPtr& msg);
  void heartbeatReceived(const km_custom_msgs::msg::HeartBeat::SharedPtr& msg);
  void workflowStatusReceived(const km_custom_msgs::msg::WorkFlowStatus::SharedPtr& msg);
  void shipLoaderSpeedReceived(const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr& msg);
  void targetPoseReceived(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg);
  void hatchPositionReceived(const km_custom_msgs::msg::HatchPosition::SharedPtr& msg);
  void statusCodeReceived(const km_custom_msgs::msg::StatusCode::SharedPtr& msg);
  void pointCloudReceived(const QString& topic, const sensor_msgs::msg::PointCloud2::SharedPtr& msg);
  void markerArrayReceived(const QString& topic, const visualization_msgs::msg::MarkerArray::SharedPtr& msg);
  void markerReceived(const QString& topic, const visualization_msgs::msg::Marker::SharedPtr& msg);
  void connectionStatusChanged(bool connected);

private:
  void spinLoop(std::function<void(rclcpp::Node::SharedPtr)> init_cb);

  // 在 spin 线程内创建
  void plcStatusCallback(const km_custom_msgs::msg::PlcStatus::SharedPtr msg);
  void shipLoaderPoseCallback(const km_custom_msgs::msg::ShipLoaderPose::SharedPtr msg);
  void heartbeatCallback(const km_custom_msgs::msg::HeartBeat::SharedPtr msg);
  void workflowStatusCallback(const km_custom_msgs::msg::WorkFlowStatus::SharedPtr msg);
  void shipLoaderSpeedCallback(const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr msg);
  void targetPoseCallback(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr msg);
  void hatchPositionCallback(const km_custom_msgs::msg::HatchPosition::SharedPtr msg);
  void statusCodeCallback(const km_custom_msgs::msg::StatusCode::SharedPtr msg);

  std::thread spin_thread_;
  std::atomic<bool> running_{false};
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__ROS_NODE_H_
