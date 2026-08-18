#include "shiploader_monitor/pose_display_widget.h"
#include <QFont>

namespace shiploader_monitor {

PoseDisplayWidget::PoseDisplayWidget(QWidget* parent)
  : QWidget(parent)
{
  setupUI();
}

PoseDisplayWidget::~PoseDisplayWidget() = default;

void PoseDisplayWidget::setupUI()
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(6);
  mainLayout->setContentsMargins(4, 4, 4, 4);

  QFont valFont;
  valFont.setPointSize(11);
  valFont.setBold(true);

  // =================================================================
  //  关节状态 — 当前 vs 目标 横向对比
  // =================================================================
  QGroupBox* jointGroup = new QGroupBox("关节状态（当前 / 目标）", this);
  QGridLayout* jointGrid = new QGridLayout(jointGroup);

  // 表头
  QLabel* hdrItem = new QLabel("项目");
  QLabel* hdrCur  = new QLabel("当前值");
  QLabel* hdrTgt  = new QLabel("目标值");
  hdrItem->setStyleSheet("font-weight: bold; color: #555;");
  hdrCur->setStyleSheet("font-weight: bold; color: #1976D2;");
  hdrTgt->setStyleSheet("font-weight: bold; color: #E64A19;");
  jointGrid->addWidget(hdrItem, 0, 0);
  jointGrid->addWidget(hdrCur,  0, 1);
  jointGrid->addWidget(hdrTgt,  0, 2);

  // 行走
  jointGrid->addWidget(new QLabel("大车行走"), 1, 0);
  lbl_cur_track_    = new QLabel("---");  lbl_cur_track_->setFont(valFont);
  lbl_target_track_ = new QLabel("---");  lbl_target_track_->setFont(valFont);
  lbl_cur_track_->setStyleSheet("color: #1976D2;");
  lbl_target_track_->setStyleSheet("color: #E64A19;");
  jointGrid->addWidget(lbl_cur_track_,    1, 1);
  jointGrid->addWidget(lbl_target_track_, 1, 2);

  // 回转
  jointGrid->addWidget(new QLabel("基座回转"), 2, 0);
  lbl_cur_slew_    = new QLabel("---");  lbl_cur_slew_->setFont(valFont);
  lbl_target_slew_ = new QLabel("---");  lbl_target_slew_->setFont(valFont);
  lbl_cur_slew_->setStyleSheet("color: #1976D2;");
  lbl_target_slew_->setStyleSheet("color: #E64A19;");
  jointGrid->addWidget(lbl_cur_slew_,    2, 1);
  jointGrid->addWidget(lbl_target_slew_, 2, 2);

  // 俯仰
  jointGrid->addWidget(new QLabel("大臂俯仰"), 3, 0);
  lbl_cur_luff_    = new QLabel("---");  lbl_cur_luff_->setFont(valFont);
  lbl_target_luff_ = new QLabel("---");  lbl_target_luff_->setFont(valFont);
  lbl_cur_luff_->setStyleSheet("color: #1976D2;");
  lbl_target_luff_->setStyleSheet("color: #E64A19;");
  jointGrid->addWidget(lbl_cur_luff_,    3, 1);
  jointGrid->addWidget(lbl_target_luff_, 3, 2);

  // 伸缩
  jointGrid->addWidget(new QLabel("小臂伸缩"), 4, 0);
  lbl_cur_stretch_    = new QLabel("---");  lbl_cur_stretch_->setFont(valFont);
  lbl_target_stretch_ = new QLabel("---");  lbl_target_stretch_->setFont(valFont);
  lbl_cur_stretch_->setStyleSheet("color: #1976D2;");
  lbl_target_stretch_->setStyleSheet("color: #E64A19;");
  jointGrid->addWidget(lbl_cur_stretch_,    4, 1);
  jointGrid->addWidget(lbl_target_stretch_, 4, 2);

  // 溜筒回转
  jointGrid->addWidget(new QLabel("溜筒回转"), 5, 0);
  lbl_cur_chute_yaw_    = new QLabel("---");  lbl_cur_chute_yaw_->setFont(valFont);
  lbl_target_chute_yaw_ = new QLabel("---");  lbl_target_chute_yaw_->setFont(valFont);
  lbl_cur_chute_yaw_->setStyleSheet("color: #1976D2;");
  lbl_target_chute_yaw_->setStyleSheet("color: #E64A19;");
  jointGrid->addWidget(lbl_cur_chute_yaw_,    5, 1);
  jointGrid->addWidget(lbl_target_chute_yaw_, 5, 2);

  mainLayout->addWidget(jointGroup);

  // =================================================================
  //  溜筒姿态
  // =================================================================
  QGroupBox* chuteGroup = new QGroupBox("溜筒姿态", this);
  QGridLayout* chuteGrid = new QGridLayout(chuteGroup);

  lbl_cur_chute_pitch_x_ = new QLabel("俯仰 X: ---");  lbl_cur_chute_pitch_x_->setFont(valFont);
  lbl_cur_chute_pitch_y_ = new QLabel("俯仰 Y: ---");  lbl_cur_chute_pitch_y_->setFont(valFont);
  chuteGrid->addWidget(lbl_cur_chute_pitch_x_, 0, 0);
  chuteGrid->addWidget(lbl_cur_chute_pitch_y_, 0, 1);

  mainLayout->addWidget(chuteGroup);

  // =================================================================
  //  溜筒位置
  // =================================================================
  QGroupBox* posGroup = new QGroupBox("溜筒位置 (XYZ)", this);
  QGridLayout* posGrid = new QGridLayout(posGroup);

  lbl_cur_pos_x_ = new QLabel("X: ---");  lbl_cur_pos_x_->setFont(valFont);
  lbl_cur_pos_y_ = new QLabel("Y: ---");  lbl_cur_pos_y_->setFont(valFont);
  lbl_cur_pos_z_ = new QLabel("Z: ---");  lbl_cur_pos_z_->setFont(valFont);
  posGrid->addWidget(lbl_cur_pos_x_, 0, 0);
  posGrid->addWidget(lbl_cur_pos_y_, 0, 1);
  posGrid->addWidget(lbl_cur_pos_z_, 0, 2);

  mainLayout->addWidget(posGroup);

  // =================================================================
  //  旋转中心
  // =================================================================
  QGroupBox* rotGroup = new QGroupBox("旋转中心位置", this);
  QGridLayout* rotGrid = new QGridLayout(rotGroup);

  lbl_rot_x_ = new QLabel("X: ---");
  lbl_rot_y_ = new QLabel("Y: ---");
  lbl_rot_z_ = new QLabel("Z: ---");
  rotGrid->addWidget(lbl_rot_x_, 0, 0);
  rotGrid->addWidget(lbl_rot_y_, 0, 1);
  rotGrid->addWidget(lbl_rot_z_, 0, 2);

  mainLayout->addWidget(rotGroup);
  mainLayout->addStretch();
}

void PoseDisplayWidget::updatePose(const km_custom_msgs::msg::ShipLoaderPose::SharedPtr& msg)
{
  if (!msg) return;

  lbl_cur_track_->setText(QString("%1 m").arg(msg->track_position, 0, 'f', 2));
  lbl_cur_slew_->setText(QString("%1°").arg(msg->base_yaw, 0, 'f', 2));
  lbl_cur_luff_->setText(QString("%1°").arg(msg->large_arm_pitch, 0, 'f', 2));
  lbl_cur_stretch_->setText(QString("%1 m").arg(msg->small_arm_extension, 0, 'f', 2));

  lbl_cur_chute_yaw_->setText(QString("%1°").arg(msg->chute_yaw, 0, 'f', 2));
  lbl_cur_chute_pitch_x_->setText(QString("俯仰 X: %1°").arg(msg->chute_pitch_x, 0, 'f', 2));
  lbl_cur_chute_pitch_y_->setText(QString("俯仰 Y: %1°").arg(msg->chute_pitch_y, 0, 'f', 2));

  lbl_cur_pos_x_->setText(QString("X: %1 m").arg(msg->chute_pose.x, 0, 'f', 2));
  lbl_cur_pos_y_->setText(QString("Y: %1 m").arg(msg->chute_pose.y, 0, 'f', 2));
  lbl_cur_pos_z_->setText(QString("Z: %1 m").arg(msg->chute_pose.z, 0, 'f', 2));

  lbl_rot_x_->setText(QString("X: %1 m").arg(msg->rot_center_pose.x, 0, 'f', 2));
  lbl_rot_y_->setText(QString("Y: %1 m").arg(msg->rot_center_pose.y, 0, 'f', 2));
  lbl_rot_z_->setText(QString("Z: %1 m").arg(msg->rot_center_pose.z, 0, 'f', 2));
}

void PoseDisplayWidget::updateTargetPose(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg)
{
  if (!msg) return;

  lbl_target_track_->setText(QString("%1 m").arg(msg->gantry_target_position, 0, 'f', 2));
  lbl_target_slew_->setText(QString("%1°").arg(msg->slewing_target_position, 0, 'f', 2));
  lbl_target_luff_->setText(QString("%1°").arg(msg->luffing_target_position, 0, 'f', 2));
  lbl_target_stretch_->setText(QString("%1 m").arg(msg->stretch_target_position, 0, 'f', 2));
  lbl_target_chute_yaw_->setText(QString("%1°").arg(msg->chute_target_position, 0, 'f', 2));
}

void PoseDisplayWidget::clear()
{
  lbl_cur_track_->setText("---");  lbl_target_track_->setText("---");
  lbl_cur_slew_->setText("---");   lbl_target_slew_->setText("---");
  lbl_cur_luff_->setText("---");   lbl_target_luff_->setText("---");
  lbl_cur_stretch_->setText("---"); lbl_target_stretch_->setText("---");
  lbl_cur_chute_yaw_->setText("---"); lbl_target_chute_yaw_->setText("---");

  lbl_cur_chute_pitch_x_->setText("俯仰 X: ---");
  lbl_cur_chute_pitch_y_->setText("俯仰 Y: ---");

  lbl_cur_pos_x_->setText("X: ---");
  lbl_cur_pos_y_->setText("Y: ---");
  lbl_cur_pos_z_->setText("Z: ---");

  lbl_rot_x_->setText("X: ---");
  lbl_rot_y_->setText("Y: ---");
  lbl_rot_z_->setText("Z: ---");
}

} // namespace shiploader_monitor
