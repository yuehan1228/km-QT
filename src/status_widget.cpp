#include "shiploader_monitor/status_widget.h"
#include <QFont>
#include <QFrame>

namespace shiploader_monitor {

namespace {

constexpr double kRadiansToDegrees = 180.0 / 3.14159265358979323846;

double radiansToDegrees(double radians)
{
  return radians * kRadiansToDegrees;
}

}  // namespace

StatusWidget::StatusWidget(QWidget* parent) : QWidget(parent) { setupUI(); }
StatusWidget::~StatusWidget() = default;

QString StatusWidget::statusToString(int s) {
  switch (s) {
    case 0: return "停止"; case 1: return "运行中";
    case 2: return "完成"; case 3: return "异常"; case 4: return "初始化";
    default: return "未知";
  }
}

void StatusWidget::setupUI()
{
  QVBoxLayout* main = new QVBoxLayout(this);
  main->setSpacing(12);
  main->setContentsMargins(8, 8, 8, 8);

  QFont vf; vf.setPointSize(12); vf.setBold(true);
  auto mk = [&](const QString& t) { auto* l = new QLabel(t); l->setFont(vf); return l; };

  // =================================================================
  //  关节状态 — 当前 vs 目标
  // =================================================================
  {
    QGroupBox* gb = new QGroupBox("关节状态（当前 / 目标）");
    QGridLayout* g = new QGridLayout(gb);
    g->setSpacing(10);
    g->setContentsMargins(12, 16, 12, 12);

    QFont hf; hf.setPointSize(11); hf.setBold(true);
    auto* h1 = new QLabel(" 项目"); h1->setFont(hf); h1->setStyleSheet("color: #888;");
    auto* h2 = new QLabel("当前值"); h2->setFont(hf); h2->setStyleSheet("color: #1976D2;");
    auto* h3 = new QLabel("目标值"); h3->setFont(hf); h3->setStyleSheet("color: #E64A19;");
    g->addWidget(h1, 0, 0); g->addWidget(h2, 0, 1); g->addWidget(h3, 0, 2);

    auto row = [&](int r, const QString& name, QLabel*& cur, QLabel*& tgt) {
      auto* lb = new QLabel(name); lb->setFont(hf);
      cur = mk("---"); cur->setStyleSheet("color: #1976D2;");
      tgt = mk("---"); tgt->setStyleSheet("color: #E64A19;");
      g->addWidget(lb, r, 0); g->addWidget(cur, r, 1); g->addWidget(tgt, r, 2);
    };
    row(1, "大车行走", lbl_cur_track_, lbl_target_track_);
    row(2, "基座回转", lbl_cur_slew_, lbl_target_slew_);
    row(3, "大臂俯仰", lbl_cur_luff_, lbl_target_luff_);
    row(4, "小臂伸缩", lbl_cur_stretch_, lbl_target_stretch_);
    row(5, "溜筒回转", lbl_cur_chute_yaw_, lbl_target_chute_yaw_);
    main->addWidget(gb);
  }

  // =================================================================
  //  溜筒姿态 + 位置
  // =================================================================
  {
    QGroupBox* gb = new QGroupBox("溜筒姿态 / 位置");
    QGridLayout* g = new QGridLayout(gb);
    g->setSpacing(10);
    g->setContentsMargins(12, 16, 12, 12);

    lbl_chute_pitch_x_ = mk("俯仰X: ---"); lbl_chute_pitch_y_ = mk("俯仰Y: ---");
    lbl_chute_pos_x_ = mk("X: ---"); lbl_chute_pos_y_ = mk("Y: ---"); lbl_chute_pos_z_ = mk("Z: ---");
    g->addWidget(lbl_chute_pitch_x_, 0, 0); g->addWidget(lbl_chute_pitch_y_, 0, 1);
    g->addWidget(lbl_chute_pos_x_, 1, 0); g->addWidget(lbl_chute_pos_y_, 1, 1); g->addWidget(lbl_chute_pos_z_, 1, 2);
    main->addWidget(gb);
  }

  // =================================================================
  //  旋转中心
  // =================================================================
  {
    QGroupBox* gb = new QGroupBox("旋转中心位置");
    QGridLayout* g = new QGridLayout(gb);
    g->setSpacing(10);
    g->setContentsMargins(12, 16, 12, 12);

    lbl_rot_center_x_ = mk("X: ---");
    lbl_rot_center_y_ = mk("Y: ---");
    lbl_rot_center_z_ = mk("Z: ---");
    g->addWidget(lbl_rot_center_x_, 0, 0);
    g->addWidget(lbl_rot_center_y_, 0, 1);
    g->addWidget(lbl_rot_center_z_, 0, 2);
    main->addWidget(gb);
  }

  // =================================================================
  //  工作流状态
  // =================================================================
  {
    QGroupBox* gb = new QGroupBox("工作流状态");
    QGridLayout* g = new QGridLayout(gb);
    g->setSpacing(10);
    g->setContentsMargins(12, 16, 12, 12);

    lbl_wf_scan_ = mk("扫描: 停止"); lbl_wf_switch_ = mk("移舱: 停止");
    lbl_wf_load_ = mk("装船: 停止"); lbl_wf_zero_ = mk("归零: 停止");
    lbl_wf_error_ = mk("错误: 0");
    g->addWidget(lbl_wf_scan_, 0, 0); g->addWidget(lbl_wf_switch_, 0, 1);
    g->addWidget(lbl_wf_load_, 1, 0); g->addWidget(lbl_wf_zero_, 1, 1);
    g->addWidget(lbl_wf_error_, 2, 0, 1, 2);
    main->addWidget(gb);
  }

  // =================================================================
  //  机构使能 / 安全状态
  // =================================================================
  {
    QGroupBox* gb = new QGroupBox("机构使能 / 安全状态");
    QHBoxLayout* columns = new QHBoxLayout(gb);
    columns->setSpacing(18);
    columns->setContentsMargins(12, 16, 12, 12);

    auto* enableGroup = new QGroupBox("机构使能");
    auto* enableLayout = new QVBoxLayout(enableGroup);
    enableLayout->setSpacing(8);
    lbl_enable_gantry_ = mk("大车使能: 关");
    lbl_enable_slewing_ = mk("回转使能: 关");
    lbl_enable_luffing_ = mk("俯仰使能: 关");
    lbl_enable_stretch_ = mk("伸缩使能: 关");
    enableLayout->addWidget(lbl_enable_gantry_);
    enableLayout->addWidget(lbl_enable_slewing_);
    enableLayout->addWidget(lbl_enable_luffing_);
    enableLayout->addWidget(lbl_enable_stretch_);

    auto* safetyGroup = new QGroupBox("安全状态");
    auto* safetyLayout = new QGridLayout(safetyGroup);
    safetyLayout->setSpacing(8);
    lbl_safety_walking_slow_ = mk("走行减速: 正常");
    lbl_safety_slewing_slow_ = mk("回转减速: 正常");
    lbl_safety_luffing_slow_ = mk("俯仰减速: 正常");
    lbl_safety_stretch_slow_ = mk("伸缩减速: 正常");
    lbl_safety_walking_stop_ = mk("走行停止: 正常");
    lbl_safety_slewing_stop_ = mk("回转停止: 正常");
    lbl_safety_luffing_stop_ = mk("俯仰停止: 正常");
    lbl_safety_stretch_stop_ = mk("伸缩停止: 正常");
    QLabel* safetyLabels[] = {
      lbl_safety_walking_slow_, lbl_safety_slewing_slow_,
      lbl_safety_luffing_slow_, lbl_safety_stretch_slow_,
      lbl_safety_walking_stop_, lbl_safety_slewing_stop_,
      lbl_safety_luffing_stop_, lbl_safety_stretch_stop_
    };
    for (int i = 0; i < 8; ++i) {
      safetyLayout->addWidget(safetyLabels[i], i / 2, i % 2);
    }

    columns->addWidget(enableGroup);
    columns->addWidget(safetyGroup, 1);
    main->addWidget(gb);
  }

  main->addStretch();
}

void StatusWidget::updatePose(const km_custom_msgs::msg::ShipLoaderPose::SharedPtr& msg) {
  if (!msg) return;
  lbl_cur_track_->setText(QString("%1 m").arg(msg->track_position, 0, 'f', 2));
  lbl_cur_slew_->setText(QString("%1°").arg(radiansToDegrees(msg->base_yaw), 0, 'f', 2));
  lbl_cur_luff_->setText(QString("%1°").arg(radiansToDegrees(msg->large_arm_pitch), 0, 'f', 2));
  lbl_cur_stretch_->setText(QString("%1 m").arg(msg->small_arm_extension, 0, 'f', 2));
  lbl_cur_chute_yaw_->setText(QString("%1°").arg(radiansToDegrees(msg->chute_yaw), 0, 'f', 2));
  lbl_chute_pos_x_->setText(QString("X: %1 m").arg(msg->chute_pose.x, 0, 'f', 2));
  lbl_chute_pos_y_->setText(QString("Y: %1 m").arg(msg->chute_pose.y, 0, 'f', 2));
  lbl_chute_pos_z_->setText(QString("Z: %1 m").arg(msg->chute_pose.z, 0, 'f', 2));
  lbl_rot_center_x_->setText(QString("X: %1 m").arg(msg->rot_center_pose.x, 0, 'f', 2));
  lbl_rot_center_y_->setText(QString("Y: %1 m").arg(msg->rot_center_pose.y, 0, 'f', 2));
  lbl_rot_center_z_->setText(QString("Z: %1 m").arg(msg->rot_center_pose.z, 0, 'f', 2));
}

void StatusWidget::updatePlcStatus(const km_custom_msgs::msg::PlcStatus::SharedPtr& msg) {
  if (!msg) return;
  lbl_chute_pitch_x_->setText(QString("俯仰X: %1°").arg(msg->chute_pitch_x, 0, 'f', 2));
  lbl_chute_pitch_y_->setText(QString("俯仰Y: %1°").arg(msg->chute_pitch_y, 0, 'f', 2));
}

void StatusWidget::updateTargetPose(const km_custom_msgs::msg::ShipLoaderTargetPose::SharedPtr& msg) {
  if (!msg) return;
  lbl_target_track_->setText(QString("%1 m").arg(msg->gantry_target_position, 0, 'f', 2));
  lbl_target_slew_->setText(QString("%1°").arg(radiansToDegrees(msg->slewing_target_position), 0, 'f', 2));
  lbl_target_luff_->setText(QString("%1°").arg(radiansToDegrees(msg->luffing_target_position), 0, 'f', 2));
  lbl_target_stretch_->setText(QString("%1 m").arg(msg->stretch_target_position, 0, 'f', 2));
  lbl_target_chute_yaw_->setText(QString("%1°").arg(radiansToDegrees(msg->chute_target_position), 0, 'f', 2));
}

void StatusWidget::updateSpeed(const km_custom_msgs::msg::ShipLoaderSpeed::SharedPtr& msg) {
  // 保留槽接口以兼容现有调用；运行状态页不显示该数据。
  (void)msg;
}

void StatusWidget::updateWorkflowStatus(const km_custom_msgs::msg::WorkFlowStatus::SharedPtr& msg) {
  if (!msg) return;
  lbl_wf_scan_->setText(QString("扫描: %1").arg(statusToString(msg->ship_scan_status)));
  lbl_wf_switch_->setText(QString("移舱: %1").arg(statusToString(msg->hatch_switch_status)));
  lbl_wf_load_->setText(QString("装船: %1").arg(statusToString(msg->ship_load_status)));
  lbl_wf_zero_->setText(QString("归零: %1").arg(statusToString(msg->device_to_zero_status)));
  lbl_wf_error_->setText(QString("错误: %1").arg(msg->error_status));
}

void StatusWidget::updateDeviceEnable(
  const km_custom_msgs::msg::DeviceEnable::SharedPtr& msg)
{
  if (!msg) return;

  const auto setEnable = [](QLabel* label, const QString& name, bool enabled) {
    label->setText(QString("%1: %2").arg(name).arg(enabled ? "开" : "关"));
    label->setStyleSheet(enabled
      ? "color: #1976D2; font-weight: bold;"
      : "color: gray;");
  };
  setEnable(lbl_enable_gantry_, "大车使能", msg->gantry_enable);
  setEnable(lbl_enable_slewing_, "回转使能", msg->slewing_enable);
  setEnable(lbl_enable_luffing_, "俯仰使能", msg->luffing_enable);
  setEnable(lbl_enable_stretch_, "伸缩使能", msg->stretch_enable);
}

void StatusWidget::updateDeviceSafetyStatus(
  const km_custom_msgs::msg::DeviceSafetyStatus::SharedPtr& msg)
{
  if (!msg) return;

  const auto setSafety = [](QLabel* label, const QString& name, bool triggered) {
    label->setText(QString("%1: %2").arg(name).arg(triggered ? "触发" : "正常"));
    label->setStyleSheet(triggered
      ? "color: red; font-weight: bold;"
      : "color: green;");
  };
  setSafety(lbl_safety_walking_slow_, "走行减速", msg->walking_slow);
  setSafety(lbl_safety_slewing_slow_, "回转减速", msg->slewing_slow);
  setSafety(lbl_safety_luffing_slow_, "俯仰减速", msg->luffing_slow);
  setSafety(lbl_safety_stretch_slow_, "伸缩减速", msg->stretch_slow);
  setSafety(lbl_safety_walking_stop_, "走行停止", msg->walking_stop);
  setSafety(lbl_safety_slewing_stop_, "回转停止", msg->slewing_stop);
  setSafety(lbl_safety_luffing_stop_, "俯仰停止", msg->luffing_stop);
  setSafety(lbl_safety_stretch_stop_, "伸缩停止", msg->stretch_stop);
}

void StatusWidget::clear() {
  lbl_cur_track_->setText("---"); lbl_target_track_->setText("---");
  lbl_cur_slew_->setText("---"); lbl_target_slew_->setText("---");
  lbl_cur_luff_->setText("---"); lbl_target_luff_->setText("---");
  lbl_cur_stretch_->setText("---"); lbl_target_stretch_->setText("---");
  lbl_cur_chute_yaw_->setText("---"); lbl_target_chute_yaw_->setText("---");
  lbl_chute_pitch_x_->setText("俯仰X: ---"); lbl_chute_pitch_y_->setText("俯仰Y: ---");
  lbl_chute_pos_x_->setText("X: ---"); lbl_chute_pos_y_->setText("Y: ---"); lbl_chute_pos_z_->setText("Z: ---");
  lbl_rot_center_x_->setText("X: ---"); lbl_rot_center_y_->setText("Y: ---"); lbl_rot_center_z_->setText("Z: ---");
  lbl_wf_scan_->setText("扫描: 停止"); lbl_wf_switch_->setText("移舱: 停止");
  lbl_wf_load_->setText("装船: 停止"); lbl_wf_zero_->setText("归零: 停止");
  lbl_wf_error_->setText("错误: 0");
  lbl_enable_gantry_->setText("大车使能: 关");
  lbl_enable_slewing_->setText("回转使能: 关");
  lbl_enable_luffing_->setText("俯仰使能: 关");
  lbl_enable_stretch_->setText("伸缩使能: 关");
  lbl_safety_walking_slow_->setText("走行减速: 正常");
  lbl_safety_slewing_slow_->setText("回转减速: 正常");
  lbl_safety_luffing_slow_->setText("俯仰减速: 正常");
  lbl_safety_stretch_slow_->setText("伸缩减速: 正常");
  lbl_safety_walking_stop_->setText("走行停止: 正常");
  lbl_safety_slewing_stop_->setText("回转停止: 正常");
  lbl_safety_luffing_stop_->setText("俯仰停止: 正常");
  lbl_safety_stretch_stop_->setText("伸缩停止: 正常");
}

} // namespace shiploader_monitor
