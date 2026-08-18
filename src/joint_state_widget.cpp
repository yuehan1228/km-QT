#include "shiploader_monitor/joint_state_widget.h"
#include <QFont>

namespace shiploader_monitor {

JointStateWidget::JointStateWidget(QWidget* parent)
  : QWidget(parent)
{
  setupUI();
}

JointStateWidget::~JointStateWidget() = default;

void JointStateWidget::setupUI()
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this);

  QFont valFont;
  valFont.setPointSize(10);
  valFont.setBold(true);

  // ── 速度指令组 ───────────────────────────────────────────────
  QGroupBox* speedGroup = new QGroupBox("速度指令", this);
  QGridLayout* speedGrid = new QGridLayout(speedGroup);

  QLabel* lblWalkTitle   = new QLabel("大车行走:");
  QLabel* lblSlewTitle   = new QLabel("基座回转:");
  QLabel* lblLuffTitle   = new QLabel("大臂俯仰:");
  QLabel* lblStrTitle    = new QLabel("小臂伸缩:");
  QLabel* lblChuteSTitle = new QLabel("溜筒回转:");

  lbl_walking_dir_       = new QLabel("停止");
  lbl_walking_speed_     = new QLabel("速度: 0");
  lbl_slewing_dir_       = new QLabel("停止");
  lbl_slewing_speed_     = new QLabel("速度: 0");
  lbl_luffing_dir_       = new QLabel("停止");
  lbl_luffing_speed_     = new QLabel("速度: 0");
  lbl_stretch_dir_       = new QLabel("停止");
  lbl_stretch_speed_     = new QLabel("速度: 0");
  lbl_chute_slewing_dir_ = new QLabel("停止");

  auto setWidgetFonts = [&](QLabel* l) { l->setFont(valFont); };
  setWidgetFonts(lbl_walking_dir_);
  setWidgetFonts(lbl_slewing_dir_);
  setWidgetFonts(lbl_luffing_dir_);
  setWidgetFonts(lbl_stretch_dir_);
  setWidgetFonts(lbl_chute_slewing_dir_);

  speedGrid->addWidget(lblWalkTitle,          0, 0);
  speedGrid->addWidget(lbl_walking_dir_,       0, 1);
  speedGrid->addWidget(lbl_walking_speed_,     0, 2);
  speedGrid->addWidget(lblSlewTitle,          1, 0);
  speedGrid->addWidget(lbl_slewing_dir_,       1, 1);
  speedGrid->addWidget(lbl_slewing_speed_,     1, 2);
  speedGrid->addWidget(lblLuffTitle,          2, 0);
  speedGrid->addWidget(lbl_luffing_dir_,       2, 1);
  speedGrid->addWidget(lbl_luffing_speed_,     2, 2);
  speedGrid->addWidget(lblStrTitle,           3, 0);
  speedGrid->addWidget(lbl_stretch_dir_,       3, 1);
  speedGrid->addWidget(lbl_stretch_speed_,     3, 2);
  speedGrid->addWidget(lblChuteSTitle,        4, 0);
  speedGrid->addWidget(lbl_chute_slewing_dir_, 4, 1);

  mainLayout->addWidget(speedGroup);

  // ── 目标位姿组 ───────────────────────────────────────────────
  QGroupBox* targetGroup = new QGroupBox("目标位姿", this);
  QGridLayout* targetGrid = new QGridLayout(targetGroup);

  lbl_target_walking_       = new QLabel("大车目标: 暂无");
  lbl_target_slewing_       = new QLabel("回转目标: 暂无");
  lbl_target_luffing_       = new QLabel("俯仰目标: 暂无");
  lbl_target_stretch_       = new QLabel("伸缩目标: 暂无");
  lbl_target_chute_yaw_     = new QLabel("溜筒回转目标: 暂无");
  lbl_target_chute_pitch_x_ = new QLabel("溜筒俯仰X: 暂无");
  lbl_target_chute_pitch_y_ = new QLabel("溜筒俯仰Y: 暂无");

  targetGrid->addWidget(lbl_target_walking_,       0, 0);
  targetGrid->addWidget(lbl_target_slewing_,       0, 1);
  targetGrid->addWidget(lbl_target_luffing_,       1, 0);
  targetGrid->addWidget(lbl_target_stretch_,       1, 1);
  targetGrid->addWidget(lbl_target_chute_yaw_,     2, 0);
  targetGrid->addWidget(lbl_target_chute_pitch_x_, 2, 1);
  targetGrid->addWidget(lbl_target_chute_pitch_y_, 3, 0);

  mainLayout->addWidget(targetGroup);
  mainLayout->addStretch();
}

void JointStateWidget::updateSpeed(const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr& msg)
{
  if (!msg) return;

  auto dirStr = [](int dir, const QString& pos, const QString& neg) -> QString {
    if (dir > 0) return pos;
    if (dir < 0) return neg;
    return "停止";
  };

  lbl_walking_dir_->setText(dirStr(msg->walking_speed, "向前", "向后"));
  lbl_walking_speed_->setText(QString("速度: %1").arg(std::abs(msg->walking_speed)));
  lbl_slewing_dir_->setText(dirStr(msg->slewing_speed, "向右", "向左"));
  lbl_slewing_speed_->setText(QString("速度: %1").arg(std::abs(msg->slewing_speed)));
  lbl_luffing_dir_->setText(dirStr(msg->luffing_speed, "向上", "向下"));
  lbl_luffing_speed_->setText(QString("速度: %1").arg(std::abs(msg->luffing_speed)));
  lbl_stretch_dir_->setText(dirStr(msg->stretch_speed, "伸出", "缩回"));
  lbl_stretch_speed_->setText(QString("速度: %1").arg(std::abs(msg->stretch_speed)));
  lbl_chute_slewing_dir_->setText(dirStr(msg->chute_slewing_speed, "向右", "向左"));
}

void JointStateWidget::updateTargetPose(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg)
{
  if (!msg) return;

  lbl_target_walking_->setText(QString("大车目标: %1").arg(msg->gantry_target_position, 0, 'f', 2));
  lbl_target_slewing_->setText(QString("回转目标: %1°").arg(msg->slewing_target_position, 0, 'f', 2));
  lbl_target_luffing_->setText(QString("俯仰目标: %1°").arg(msg->luffing_target_position, 0, 'f', 2));
  lbl_target_stretch_->setText(QString("伸缩目标: %1").arg(msg->stretch_target_position, 0, 'f', 2));
  lbl_target_chute_yaw_->setText(QString("溜筒回转目标: %1°").arg(msg->chute_target_position, 0, 'f', 2));
  lbl_target_chute_pitch_x_->setText("溜筒俯仰X: 暂无");
  lbl_target_chute_pitch_y_->setText("溜筒俯仰Y: 暂无");
}

void JointStateWidget::clear()
{
  lbl_walking_dir_->setText("停止");
  lbl_walking_speed_->setText("速度: 0");
  lbl_slewing_dir_->setText("停止");
  lbl_slewing_speed_->setText("速度: 0");
  lbl_luffing_dir_->setText("停止");
  lbl_luffing_speed_->setText("速度: 0");
  lbl_stretch_dir_->setText("停止");
  lbl_stretch_speed_->setText("速度: 0");
  lbl_chute_slewing_dir_->setText("停止");

  lbl_target_walking_->setText("大车目标: 暂无");
  lbl_target_slewing_->setText("回转目标: 暂无");
  lbl_target_luffing_->setText("俯仰目标: 暂无");
  lbl_target_stretch_->setText("伸缩目标: 暂无");
  lbl_target_chute_yaw_->setText("溜筒回转目标: 暂无");
  lbl_target_chute_pitch_x_->setText("溜筒俯仰X: 暂无");
  lbl_target_chute_pitch_y_->setText("溜筒俯仰Y: 暂无");
}

} // namespace shiploader_monitor
