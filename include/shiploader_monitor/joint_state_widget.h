#ifndef SHIPLOADER_MONITOR__JOINT_STATE_WIDGET_H_
#define SHIPLOADER_MONITOR__JOINT_STATE_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <km_custom_msgs/msg/ship_loader_speed.hpp>
#include <km_custom_msgs/msg/ship_loader_target_pose.hpp>

namespace shiploader_monitor {

/// @brief 关节状态面板 —— 显示速度指令和目标位姿
class JointStateWidget : public QWidget
{
  Q_OBJECT

public:
  explicit JointStateWidget(QWidget* parent = nullptr);
  ~JointStateWidget() override;

public slots:
  void updateSpeed(const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr& msg);
  void updateTargetPose(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg);
  void clear();

private:
  void setupUI();

  QLabel* lbl_walking_dir_;
  QLabel* lbl_walking_speed_;
  QLabel* lbl_slewing_dir_;
  QLabel* lbl_slewing_speed_;
  QLabel* lbl_luffing_dir_;
  QLabel* lbl_luffing_speed_;
  QLabel* lbl_stretch_dir_;
  QLabel* lbl_stretch_speed_;
  QLabel* lbl_chute_slewing_dir_;

  QLabel* lbl_target_walking_;
  QLabel* lbl_target_slewing_;
  QLabel* lbl_target_luffing_;
  QLabel* lbl_target_stretch_;
  QLabel* lbl_target_chute_yaw_;
  QLabel* lbl_target_chute_pitch_x_;
  QLabel* lbl_target_chute_pitch_y_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__JOINT_STATE_WIDGET_H_
