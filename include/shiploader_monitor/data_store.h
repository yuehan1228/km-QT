#ifndef SHIPLOADER_MONITOR__DATA_STORE_H_
#define SHIPLOADER_MONITOR__DATA_STORE_H_

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QVector>
#include <QMutex>
#include <deque>
#include <memory>
#include <km_custom_msgs/msg/plc_status.hpp>
#include <km_custom_msgs/msg/ship_loader_pose.hpp>
#include <km_custom_msgs/msg/heart_beat.hpp>
#include <km_custom_msgs/msg/work_flow_status.hpp>
#include <km_custom_msgs/msg/ship_loader_speed.hpp>
#include <km_custom_msgs/msg/ship_loader_target_pose.hpp>
#include <km_custom_msgs/msg/hatch_position.hpp>
#include <km_custom_msgs/msg/device_enable.hpp>
#include <km_custom_msgs/msg/device_safety_status.hpp>

namespace shiploader_monitor {

/// @brief 带时间戳的通用数据条目
struct DataEntry {
  QDateTime timestamp;
  QString topic;
  QString summary;
};

/// @brief 数据存储中心 —— 缓存最新数据 + 维护历史记录
class DataStore : public QObject
{
  Q_OBJECT

public:
  explicit DataStore(QObject* parent = nullptr);
  ~DataStore() override;

  /// 设置历史最大条数
  void setMaxHistorySize(int size);
  int maxHistorySize() const { return max_history_size_; }

  // ── 数据更新 ──────────────────────────────────────────────────

  void updatePlcStatus(const km_custom_msgs::msg::PlcStatus::SharedPtr& msg);
  void updateShipLoaderPose(const km_custom_msgs::msg::ShipLoaderPose::SharedPtr& msg);
  void updateHeartbeat(const km_custom_msgs::msg::HeartBeat::SharedPtr& msg);
  void updateWorkflowStatus(const km_custom_msgs::msg::WorkFlowStatus::SharedPtr& msg);
  void updateShipLoaderSpeed(const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr& msg);
  void updateTargetPose(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg);
  void updateHatchPosition(const km_custom_msgs::msg::HatchPosition::SharedPtr& msg);
  void updateDeviceEnable(const km_custom_msgs::msg::DeviceEnable::SharedPtr& msg);
  void updateDeviceSafetyStatus(const km_custom_msgs::msg::DeviceSafetyStatus::SharedPtr& msg);

  // ── 最新数据访问 ──────────────────────────────────────────────

  km_custom_msgs::msg::PlcStatus::SharedPtr latestPlcStatus() const;
  km_custom_msgs::msg::ShipLoaderPose::SharedPtr latestShipLoaderPose() const;
  km_custom_msgs::msg::HeartBeat::SharedPtr latestHeartbeat() const;
  km_custom_msgs::msg::WorkFlowStatus::SharedPtr latestWorkflowStatus() const;
  km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr latestShipLoaderSpeed() const;
  km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr latestTargetPose() const;
  km_custom_msgs::msg::HatchPosition::SharedPtr latestHatchPosition() const;
  km_custom_msgs::msg::DeviceEnable::SharedPtr latestDeviceEnable() const;
  km_custom_msgs::msg::DeviceSafetyStatus::SharedPtr latestDeviceSafetyStatus() const;

  /// 最近一次各 topic 数据的时间戳
  QDateTime lastUpdateTime(const QString& topic) const;

  // ── 历史记录 ──────────────────────────────────────────────────

  const std::deque<DataEntry>& history() const { return history_; }

  /// 导出历史数据到 CSV 文件
  bool exportToCSV(const QString& filepath);

  /// 清空历史
  void clearHistory();

signals:
  void dataUpdated(const QString& topic);
  void historyChanged();

private:
  void addHistoryEntry(const QString& topic, const QString& summary);

  // ── 最新数据缓存 ─────────────────────────────────────────────
  mutable QMutex plc_status_mutex_;
  km_custom_msgs::msg::PlcStatus::SharedPtr latest_plc_status_;

  mutable QMutex pose_mutex_;
  km_custom_msgs::msg::ShipLoaderPose::SharedPtr latest_pose_;

  mutable QMutex heartbeat_mutex_;
  km_custom_msgs::msg::HeartBeat::SharedPtr latest_heartbeat_;

  mutable QMutex workflow_mutex_;
  km_custom_msgs::msg::WorkFlowStatus::SharedPtr latest_workflow_status_;

  mutable QMutex speed_mutex_;
  km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr latest_speed_;

  mutable QMutex target_pose_mutex_;
  km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr latest_target_pose_;

  mutable QMutex hatch_mutex_;
  km_custom_msgs::msg::HatchPosition::SharedPtr latest_hatch_position_;

  mutable QMutex device_enable_mutex_;
  km_custom_msgs::msg::DeviceEnable::SharedPtr latest_device_enable_;

  mutable QMutex device_safety_status_mutex_;
  km_custom_msgs::msg::DeviceSafetyStatus::SharedPtr latest_device_safety_status_;

  // ── 更新时间 ─────────────────────────────────────────────────
  mutable QMutex update_time_mutex_;
  QHash<QString, QDateTime> last_update_times_;

  // ── 历史 ─────────────────────────────────────────────────────
  mutable QMutex history_mutex_;
  std::deque<DataEntry> history_;
  int max_history_size_{1000};
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__DATA_STORE_H_
