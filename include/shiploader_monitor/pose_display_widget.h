#ifndef SHIPLOADER_MONITOR__POSE_DISPLAY_WIDGET_H_
#define SHIPLOADER_MONITOR__POSE_DISPLAY_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QFrame>
#include <km_custom_msgs/msg/ship_loader_pose.hpp>
#include <km_custom_msgs/msg/ship_loader_target_pose.hpp>

namespace shiploader_monitor {

/// @brief 位姿数据显示面板 —— 当前位姿 + 目标位姿对比显示
class PoseDisplayWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PoseDisplayWidget(QWidget* parent = nullptr);
  ~PoseDisplayWidget() override;

public slots:
  void updatePose(const km_custom_msgs::msg::ShipLoaderPose::SharedPtr& msg);
  void updateTargetPose(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg);
  void clear();

private:
  void setupUI();

  // ── 当前关节状态 ────────────────────────────────────────────
  QLabel* lbl_cur_track_;
  QLabel* lbl_cur_slew_;
  QLabel* lbl_cur_luff_;
  QLabel* lbl_cur_stretch_;

  // ── 目标关节状态 ────────────────────────────────────────────
  QLabel* lbl_target_track_;
  QLabel* lbl_target_slew_;
  QLabel* lbl_target_luff_;
  QLabel* lbl_target_stretch_;

  // ── 当前溜筒姿态 ────────────────────────────────────────────
  QLabel* lbl_cur_chute_yaw_;
  QLabel* lbl_cur_chute_pitch_x_;
  QLabel* lbl_cur_chute_pitch_y_;

  // ── 目标溜筒姿态 ────────────────────────────────────────────
  QLabel* lbl_target_chute_yaw_;

  // ── 当前位置 ────────────────────────────────────────────────
  QLabel* lbl_cur_pos_x_;
  QLabel* lbl_cur_pos_y_;
  QLabel* lbl_cur_pos_z_;

  // ── 旋转中心 ────────────────────────────────────────────────
  QLabel* lbl_rot_x_;
  QLabel* lbl_rot_y_;
  QLabel* lbl_rot_z_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__POSE_DISPLAY_WIDGET_H_
