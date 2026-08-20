#include "shiploader_monitor/data_store.h"

#include <QFile>
#include <QTextStream>
#include <QMutexLocker>
#include <sstream>

namespace shiploader_monitor {

DataStore::DataStore(QObject* parent)
  : QObject(parent)
{
}

DataStore::~DataStore() = default;

void DataStore::setMaxHistorySize(int size)
{
  QMutexLocker lock(&history_mutex_);
  max_history_size_ = size;
  while (static_cast<int>(history_.size()) > max_history_size_) {
    history_.pop_front();
  }
}

// ── 数据更新 ─────────────────────────────────────────────────────────

void DataStore::updatePlcStatus(const km_custom_msgs::msg::PlcStatus::SharedPtr& msg)
{
  {
    QMutexLocker lock(&plc_status_mutex_);
    latest_plc_status_ = std::make_shared<km_custom_msgs::msg::PlcStatus>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/plc_status"] = QDateTime::currentDateTime();
  }
  std::ostringstream ss;
  ss << "PLC: ready=" << (msg->is_machine_ready ? "ON" : "OFF")
     << " scan=" << (msg->is_ship_scanning_start ? "ON" : "OFF")
     << " load=" << (msg->is_ship_loading_start ? "ON" : "OFF")
     << " task=" << msg->task_id;
  addHistoryEntry("/plc_status", QString::fromStdString(ss.str()));
  emit dataUpdated("/plc_status");
}

void DataStore::updateShipLoaderPose(const km_custom_msgs::msg::ShipLoaderPose::SharedPtr& msg)
{
  {
    QMutexLocker lock(&pose_mutex_);
    latest_pose_ = std::make_shared<km_custom_msgs::msg::ShipLoaderPose>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/shiploader_pos"] = QDateTime::currentDateTime();
  }
  std::ostringstream ss;
  ss << "Pose: track=" << msg->track_position
     << " yaw=" << msg->base_yaw
     << " arm_pitch=" << msg->large_arm_pitch
     << " ext=" << msg->small_arm_extension;
  addHistoryEntry("/shiploader_pos", QString::fromStdString(ss.str()));
  emit dataUpdated("/shiploader_pos");
}

void DataStore::updateHeartbeat(const km_custom_msgs::msg::HeartBeat::SharedPtr& msg)
{
  {
    QMutexLocker lock(&heartbeat_mutex_);
    latest_heartbeat_ = std::make_shared<km_custom_msgs::msg::HeartBeat>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/heartbeat"] = QDateTime::currentDateTime();
  }
  std::ostringstream ss;
  ss << "HB: seq=" << msg->heartbeat_seq
     << " node=" << msg->node_name
     << " code=" << static_cast<int>(msg->status_code);
  addHistoryEntry("/heartbeat", QString::fromStdString(ss.str()));
  emit dataUpdated("/heartbeat");
}

void DataStore::updateWorkflowStatus(const km_custom_msgs::msg::WorkFlowStatus::SharedPtr& msg)
{
  {
    QMutexLocker lock(&workflow_mutex_);
    latest_workflow_status_ = std::make_shared<km_custom_msgs::msg::WorkFlowStatus>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/workflow_status"] = QDateTime::currentDateTime();
  }
  addHistoryEntry("/workflow_status", "Workflow status updated");
  emit dataUpdated("/workflow_status");
}

void DataStore::updateShipLoaderSpeed(const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr& msg)
{
  {
    QMutexLocker lock(&speed_mutex_);
    latest_speed_ = std::make_shared<km_custom_msgs::msg::ShipLoaderSpeed>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/ship_loader_speed"] = QDateTime::currentDateTime();
  }
  addHistoryEntry("/ship_loader_speed", "Speed command updated");
  emit dataUpdated("/ship_loader_speed");
}

void DataStore::updateTargetPose(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg)
{
  {
    QMutexLocker lock(&target_pose_mutex_);
    latest_target_pose_ = std::make_shared<km_custom_msgs::msg::ShipLoaderTargetPose>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/ship_loader_target_pos"] = QDateTime::currentDateTime();
  }
  addHistoryEntry("/ship_loader_target_pos", "Target pose updated");
  emit dataUpdated("/ship_loader_target_pos");
}

void DataStore::updateHatchPosition(const km_custom_msgs::msg::HatchPosition::SharedPtr& msg)
{
  {
    QMutexLocker lock(&hatch_mutex_);
    latest_hatch_position_ = std::make_shared<km_custom_msgs::msg::HatchPosition>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/hatch_position"] = QDateTime::currentDateTime();
  }
  addHistoryEntry("/hatch_position", "Hatch position updated");
  emit dataUpdated("/hatch_position");
}

void DataStore::updateDeviceEnable(const km_custom_msgs::msg::DeviceEnable::SharedPtr& msg)
{
  {
    QMutexLocker lock(&device_enable_mutex_);
    latest_device_enable_ = std::make_shared<km_custom_msgs::msg::DeviceEnable>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/device_enable"] = QDateTime::currentDateTime();
  }
  addHistoryEntry("/device_enable", "Device enable status updated");
  emit dataUpdated("/device_enable");
}

void DataStore::updateDeviceSafetyStatus(
  const km_custom_msgs::msg::DeviceSafetyStatus::SharedPtr& msg)
{
  {
    QMutexLocker lock(&device_safety_status_mutex_);
    latest_device_safety_status_ = std::make_shared<km_custom_msgs::msg::DeviceSafetyStatus>(*msg);
  }
  {
    QMutexLocker lock(&update_time_mutex_);
    last_update_times_["/device_safety_status"] = QDateTime::currentDateTime();
  }
  addHistoryEntry("/device_safety_status", "Device safety status updated");
  emit dataUpdated("/device_safety_status");
}

// ── 最新数据访问 ─────────────────────────────────────────────────────

km_custom_msgs::msg::PlcStatus::SharedPtr DataStore::latestPlcStatus() const
{
  QMutexLocker lock(&plc_status_mutex_);
  return latest_plc_status_;
}

km_custom_msgs::msg::ShipLoaderPose::SharedPtr DataStore::latestShipLoaderPose() const
{
  QMutexLocker lock(&pose_mutex_);
  return latest_pose_;
}

km_custom_msgs::msg::HeartBeat::SharedPtr DataStore::latestHeartbeat() const
{
  QMutexLocker lock(&heartbeat_mutex_);
  return latest_heartbeat_;
}

km_custom_msgs::msg::WorkFlowStatus::SharedPtr DataStore::latestWorkflowStatus() const
{
  QMutexLocker lock(&workflow_mutex_);
  return latest_workflow_status_;
}

km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr DataStore::latestShipLoaderSpeed() const
{
  QMutexLocker lock(&speed_mutex_);
  return latest_speed_;
}

km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr DataStore::latestTargetPose() const
{
  QMutexLocker lock(&target_pose_mutex_);
  return latest_target_pose_;
}

km_custom_msgs::msg::HatchPosition::SharedPtr DataStore::latestHatchPosition() const
{
  QMutexLocker lock(&hatch_mutex_);
  return latest_hatch_position_;
}

km_custom_msgs::msg::DeviceEnable::SharedPtr DataStore::latestDeviceEnable() const
{
  QMutexLocker lock(&device_enable_mutex_);
  return latest_device_enable_;
}

km_custom_msgs::msg::DeviceSafetyStatus::SharedPtr DataStore::latestDeviceSafetyStatus() const
{
  QMutexLocker lock(&device_safety_status_mutex_);
  return latest_device_safety_status_;
}

QDateTime DataStore::lastUpdateTime(const QString& topic) const
{
  QMutexLocker lock(&update_time_mutex_);
  return last_update_times_.value(topic);
}

// ── 历史记录 ─────────────────────────────────────────────────────────

void DataStore::addHistoryEntry(const QString& topic, const QString& summary)
{
  QMutexLocker lock(&history_mutex_);
  DataEntry entry;
  entry.timestamp = QDateTime::currentDateTime();
  entry.topic = topic;
  entry.summary = summary;
  history_.push_back(entry);

  while (static_cast<int>(history_.size()) > max_history_size_) {
    history_.pop_front();
  }
  emit historyChanged();
}

bool DataStore::exportToCSV(const QString& filepath)
{
  QMutexLocker lock(&history_mutex_);

  QFile file(filepath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return false;
  }

  QTextStream stream(&file);
  stream << "Timestamp,Topic,Summary\n";
  for (const auto& entry : history_) {
    stream << entry.timestamp.toString(Qt::ISODateWithMs) << ","
           << entry.topic << ","
           << "\"" << entry.summary << "\"\n";
  }

  file.close();
  return true;
}

void DataStore::clearHistory()
{
  QMutexLocker lock(&history_mutex_);
  history_.clear();
  emit historyChanged();
}

} // namespace shiploader_monitor
