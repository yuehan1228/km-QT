#ifndef SHIPLOADER_MONITOR__STATUS_WIDGET_H_
#define SHIPLOADER_MONITOR__STATUS_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <km_custom_msgs/msg/plc_status.hpp>
#include <km_custom_msgs/msg/ship_loader_pose.hpp>
#include <km_custom_msgs/msg/ship_loader_target_pose.hpp>
#include <km_custom_msgs/msg/ship_loader_speed.hpp>
#include <km_custom_msgs/msg/work_flow_status.hpp>
#include <km_custom_msgs/msg/device_enable.hpp>
#include <km_custom_msgs/msg/device_safety_status.hpp>

namespace shiploader_monitor {

/// @brief 合并显示：位姿数据（当前/目标）+ 溜筒姿态 + 工作流状态
class StatusWidget : public QWidget
{
  Q_OBJECT

public:
  explicit StatusWidget(QWidget* parent = nullptr);
  ~StatusWidget() override;

public slots:
  void updatePose(const km_custom_msgs::msg::ShipLoaderPose::SharedPtr& msg);
  void updatePlcStatus(const km_custom_msgs::msg::PlcStatus::SharedPtr& msg);
  void updateTargetPose(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg);
  void updateSpeed(const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr& msg);
  void updateWorkflowStatus(const km_custom_msgs::msg::WorkFlowStatus::SharedPtr& msg);
  void updateDeviceEnable(const km_custom_msgs::msg::DeviceEnable::SharedPtr& msg);
  void updateDeviceSafetyStatus(const km_custom_msgs::msg::DeviceSafetyStatus::SharedPtr& msg);
  void clear();

private:
  void setupUI();
  QString statusToString(int status);

  // ── 关节状态（当前 / 目标） ─────────────────────────────────
  QLabel* lbl_cur_track_;    QLabel* lbl_target_track_;
  QLabel* lbl_cur_slew_;     QLabel* lbl_target_slew_;
  QLabel* lbl_cur_luff_;     QLabel* lbl_target_luff_;
  QLabel* lbl_cur_stretch_;  QLabel* lbl_target_stretch_;
  QLabel* lbl_cur_chute_yaw_; QLabel* lbl_target_chute_yaw_;

  // ── 溜筒姿态 ────────────────────────────────────────────────
  QLabel* lbl_chute_pitch_x_;
  QLabel* lbl_chute_pitch_y_;

  // ── 溜筒位置 ────────────────────────────────────────────────
  QLabel* lbl_chute_pos_x_;
  QLabel* lbl_chute_pos_y_;
  QLabel* lbl_chute_pos_z_;

  // ── 旋转中心位置 ────────────────────────────────────────────
  QLabel* lbl_rot_center_x_;
  QLabel* lbl_rot_center_y_;
  QLabel* lbl_rot_center_z_;

  // ── 工作流状态 ──────────────────────────────────────────────
  QLabel* lbl_wf_scan_;
  QLabel* lbl_wf_switch_;
  QLabel* lbl_wf_load_;
  QLabel* lbl_wf_zero_;
  QLabel* lbl_wf_error_;

  // ── 机构使能 / 安全状态 ──────────────────────────────────────
  QLabel* lbl_enable_gantry_;
  QLabel* lbl_enable_slewing_;
  QLabel* lbl_enable_luffing_;
  QLabel* lbl_enable_stretch_;
  QLabel* lbl_safety_walking_slow_;
  QLabel* lbl_safety_slewing_slow_;
  QLabel* lbl_safety_luffing_slow_;
  QLabel* lbl_safety_stretch_slow_;
  QLabel* lbl_safety_walking_stop_;
  QLabel* lbl_safety_slewing_stop_;
  QLabel* lbl_safety_luffing_stop_;
  QLabel* lbl_safety_stretch_stop_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__STATUS_WIDGET_H_
