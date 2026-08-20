#include "shiploader_monitor/plc_status_widget.h"

namespace shiploader_monitor {

PlcStatusWidget::PlcStatusWidget(QWidget* parent)
  : QWidget(parent)
{
  setAutoFillBackground(true);
  setupUI();
}

PlcStatusWidget::~PlcStatusWidget() = default;

QLabel* PlcStatusWidget::makeStatusIndicator(const QString& label)
{
  return new QLabel(label + ": ");
}

void PlcStatusWidget::setIndicator(QLabel* indicator, bool active)
{
  QString text = indicator->text();
  const int idx = text.lastIndexOf(": ");
  if (idx >= 0) {
    text = text.left(idx + 2) + (active ? "开" : "关");
  }
  indicator->setText(text);
  indicator->setStyleSheet(active
    ? "color: green; font-weight: bold;"
    : "color: gray;");
}

void PlcStatusWidget::setAlarmIndicator(QLabel* indicator, bool active)
{
  QString text = indicator->text();
  const int idx = text.lastIndexOf(": ");
  if (idx >= 0) {
    text = text.left(idx + 2) + (active ? "告警" : "正常");
  }
  indicator->setText(text);
  indicator->setStyleSheet(active
    ? "color: red; font-weight: bold;"
    : "color: green;");
}

void PlcStatusWidget::setupUI()
{
  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(2);

  auto* statusGroup = new QGroupBox("系统与流程状态", this);
  auto* statusGrid = new QGridLayout(statusGroup);

  lbl_machine_ready_       = makeStatusIndicator("大机就绪");
  lbl_machine_broken_      = makeStatusIndicator("大机故障");
  lbl_emergency_stop_      = makeStatusIndicator("急停");
  lbl_lidar_power_         = makeStatusIndicator("3D 激光雷达");
  lbl_gantry_in_position_  = makeStatusIndicator("大车到位");
  lbl_slewing_in_position_ = makeStatusIndicator("回转到位");
  lbl_luffing_in_position_ = makeStatusIndicator("俯仰到位");
  lbl_stretch_in_position_ = makeStatusIndicator("伸缩到位");
  lbl_ship_scanning_       = makeStatusIndicator("整船扫描");
  lbl_hatch_switch_        = makeStatusIndicator("移舱流程");
  lbl_ship_loading_        = makeStatusIndicator("舱内作业");
  lbl_ship_loading_stop_   = makeStatusIndicator("作业暂停");
  lbl_device_to_zero_      = makeStatusIndicator("机构归零");

  QLabel* statusLabels[] = {
    lbl_machine_ready_, lbl_machine_broken_, lbl_emergency_stop_, lbl_lidar_power_,
    lbl_gantry_in_position_, lbl_slewing_in_position_,
    lbl_luffing_in_position_, lbl_stretch_in_position_,
    lbl_ship_scanning_, lbl_hatch_switch_, lbl_ship_loading_, lbl_ship_loading_stop_,
    lbl_device_to_zero_
  };
  for (int i = 0; i < 13; ++i) {
    statusGrid->addWidget(statusLabels[i], i / 4, i % 4);
  }
  mainLayout->addWidget(statusGroup);

  auto* controlGroup = new QGroupBox("作业控制", this);
  auto* controlGrid = new QGridLayout(controlGroup);

  lbl_return_first_    = makeStatusIndicator("交还第一轮");
  lbl_return_second_   = makeStatusIndicator("交还第二轮");
  lbl_move_next_point_ = makeStatusIndicator("移动下一点");
  lbl_move_over_hatch_ = makeStatusIndicator("跨舱盖移舱");
  lbl_lidar_join_      = makeStatusIndicator("激光参与建模");

  controlGrid->addWidget(lbl_return_first_, 0, 0);
  controlGrid->addWidget(lbl_return_second_, 0, 1);
  controlGrid->addWidget(lbl_move_next_point_, 0, 2);
  controlGrid->addWidget(lbl_move_over_hatch_, 1, 0);
  controlGrid->addWidget(lbl_lidar_join_, 1, 1);
  mainLayout->addWidget(controlGroup);

  auto* paramGroup = new QGroupBox("作业参数", this);
  auto* paramGrid = new QGridLayout(paramGroup);

  lbl_auto_system_mode_  = new QLabel("自动化系统模式: 暂无");
  lbl_auto_working_mode_ = new QLabel("自动化作业模式: 暂无");
  lbl_belt_accumulation_ = new QLabel("皮带累计量: 暂无");
  lbl_hatch_inst_flow_   = new QLabel("瞬时流量: 暂无");
  lbl_return_point_      = new QLabel("交还点 ID: 暂无");
  lbl_ship_th_thresholds_= new QLabel("首尾减速/停止距离: 暂无");
  lbl_ship_sr_thresholds_= new QLabel("海陆侧减速/停止距离: 暂无");
  lbl_load_spacing_      = new QLabel("装载长/宽间距: 暂无");
  lbl_chute_pitch_       = new QLabel("溜筒倾角 X/Y: 暂无");

  QLabel* paramLabels[] = {
    lbl_auto_system_mode_, lbl_auto_working_mode_, lbl_belt_accumulation_,
    lbl_hatch_inst_flow_, lbl_return_point_, lbl_ship_th_thresholds_,
    lbl_ship_sr_thresholds_, lbl_load_spacing_, lbl_chute_pitch_
  };
  for (int i = 0; i < 9; ++i) {
    paramGrid->addWidget(paramLabels[i], i / 3, i % 3);
  }
  mainLayout->addWidget(paramGroup);

  auto* zoneGroup = new QGroupBox("防区状态", this);
  auto* zoneGrid = new QGridLayout(zoneGroup);
  lbl_zone_front_  = new QLabel("前侧: 暂无");
  lbl_zone_left_   = new QLabel("左侧: 暂无");
  lbl_zone_right_  = new QLabel("右侧: 暂无");
  lbl_zone_bottom_ = new QLabel("下侧: 暂无");
  zoneGrid->addWidget(lbl_zone_front_, 0, 0);
  zoneGrid->addWidget(lbl_zone_left_, 0, 1);
  zoneGrid->addWidget(lbl_zone_right_, 1, 0);
  zoneGrid->addWidget(lbl_zone_bottom_, 1, 1);
  mainLayout->addWidget(zoneGroup);

  auto* taskGroup = new QGroupBox("任务与船舶信息", this);
  auto* taskGrid = new QGridLayout(taskGroup);
  lbl_task_             = new QLabel("任务 ID/信息: 暂无");
  lbl_task_instruction_ = new QLabel("任务指令: 暂无");
  lbl_ship_size_        = new QLabel("船舶长/宽: 暂无");
  lbl_hatch_info_       = new QLabel("舱口数量/舱盖类型: 暂无");
  lbl_hatch_work_plan_  = new QLabel("舱口作业计划: 暂无");
  lbl_hatch_work_tech_  = new QLabel("舱口作业工艺: 暂无");
  lbl_hatch_work_order_ = new QLabel("舱口作业顺序: 暂无");
  lbl_load_direction_   = new QLabel("装舱方向: 暂无");
  taskGrid->addWidget(lbl_task_, 0, 0);
  taskGrid->addWidget(lbl_task_instruction_, 0, 1);
  taskGrid->addWidget(lbl_ship_size_, 1, 0);
  taskGrid->addWidget(lbl_hatch_info_, 1, 1);
  taskGrid->addWidget(lbl_hatch_work_plan_, 2, 0);
  taskGrid->addWidget(lbl_hatch_work_tech_, 2, 1);
  taskGrid->addWidget(lbl_hatch_work_order_, 3, 0);
  taskGrid->addWidget(lbl_load_direction_, 3, 1);
  mainLayout->addWidget(taskGroup);

  mainLayout->addStretch();
  clear();
}

void PlcStatusWidget::updatePlcStatus(
  const km_custom_msgs::msg::PlcStatus::SharedPtr& msg)
{
  if (!msg) return;

  setIndicator(lbl_machine_ready_, msg->is_machine_ready);
  setAlarmIndicator(lbl_machine_broken_, msg->is_machine_broken);
  setAlarmIndicator(lbl_emergency_stop_, msg->is_emergency_stop);
  setIndicator(lbl_lidar_power_, msg->is_3d_lidar_power_on);
  setIndicator(lbl_gantry_in_position_, msg->is_gantry_in_position);
  setIndicator(lbl_slewing_in_position_, msg->is_slewing_in_position);
  setIndicator(lbl_luffing_in_position_, msg->is_luffing_in_position);
  setIndicator(lbl_stretch_in_position_, msg->is_stretch_in_position);
  setIndicator(lbl_ship_scanning_, msg->is_ship_scanning_start);
  setIndicator(lbl_hatch_switch_, msg->is_hatch_switch_start);
  setIndicator(lbl_ship_loading_, msg->is_ship_loading_start);
  setIndicator(lbl_ship_loading_stop_, msg->is_ship_loading_stop);
  setIndicator(lbl_device_to_zero_, msg->is_device_to_zero);

  setIndicator(lbl_return_first_, msg->is_return_first_round);
  setIndicator(lbl_return_second_, msg->is_return_second_round);
  setIndicator(lbl_move_next_point_, msg->is_move_next_point);
  setIndicator(lbl_move_over_hatch_, msg->is_move_over_hatch);
  setIndicator(lbl_lidar_join_, msg->is_lidar_join_modeling);

  lbl_auto_system_mode_->setText(
    QString("自动化系统模式: %1").arg(msg->auto_system_mode));
  lbl_auto_working_mode_->setText(
    QString("自动化作业模式: %1").arg(msg->auto_working_mode));
  lbl_belt_accumulation_->setText(
    QString("皮带累计量: %1 T").arg(msg->belt_accumulation));
  lbl_hatch_inst_flow_->setText(
    QString("瞬时流量: %1 T/H").arg(msg->hatch_inst_flow));
  lbl_return_point_->setText(
    QString("交还点 ID: %1").arg(msg->return_point_id));
  lbl_ship_th_thresholds_->setText(
    QString("首尾减速/停止: %1 / %2 m")
      .arg(msg->ship_th_slow_dist, 0, 'f', 2)
      .arg(msg->ship_th_stop_dist, 0, 'f', 2));
  lbl_ship_sr_thresholds_->setText(
    QString("海陆侧减速/停止: %1 / %2 m")
      .arg(msg->ship_sr_slow_dist, 0, 'f', 2)
      .arg(msg->ship_sr_stop_dist, 0, 'f', 2));
  lbl_load_spacing_->setText(
    QString("装载长/宽间距: %1 / %2 m")
      .arg(msg->load_length_spacing, 0, 'f', 2)
      .arg(msg->load_width_spacing, 0, 'f', 2));
  lbl_chute_pitch_->setText(
    QString("溜筒倾角 X/Y: %1 / %2°")
      .arg(msg->chute_pitch_x, 0, 'f', 2)
      .arg(msg->chute_pitch_y, 0, 'f', 2));

  lbl_zone_front_->setText(
    QString("前侧: 状态 %1，距离 %2 m")
      .arg(msg->zone_front_status).arg(msg->zone_front_distance, 0, 'f', 2));
  lbl_zone_left_->setText(
    QString("左侧: 状态 %1，距离 %2 m")
      .arg(msg->zone_left_status).arg(msg->zone_left_distance, 0, 'f', 2));
  lbl_zone_right_->setText(
    QString("右侧: 状态 %1，距离 %2 m")
      .arg(msg->zone_right_status).arg(msg->zone_right_distance, 0, 'f', 2));
  lbl_zone_bottom_->setText(
    QString("下侧: 状态 %1，距离 %2 m")
      .arg(msg->zone_bottom_status).arg(msg->zone_bottom_distance, 0, 'f', 2));

  lbl_task_->setText(
    QString("任务 ID/信息: %1 / %2").arg(msg->task_id).arg(msg->task_info));
  lbl_task_instruction_->setText(
    QString("任务指令: %1").arg(msg->task_instruction));
  lbl_ship_size_->setText(
    QString("船舶长/宽: %1 / %2 m")
      .arg(msg->ship_length, 0, 'f', 1).arg(msg->ship_width, 0, 'f', 1));
  lbl_hatch_info_->setText(
    QString("舱口数量/舱盖类型: %1 / %2")
      .arg(msg->hatch_count).arg(msg->hatch_cover_type));
  const auto formatHatchValues = [](const auto& values) {
    QStringList formatted;
    formatted.reserve(static_cast<int>(values.size()));
    for (const auto value : values) {
      formatted << QString::number(value);
    }
    return formatted.join(", ");
  };
  lbl_hatch_work_plan_->setText(
    QString("舱口作业计划: %1").arg(formatHatchValues(msg->hatch_work_plan)));
  lbl_hatch_work_tech_->setText(
    QString("舱口作业工艺: %1").arg(formatHatchValues(msg->hatch_work_tech)));
  lbl_hatch_work_order_->setText(
    QString("舱口作业顺序: %1").arg(formatHatchValues(msg->hatch_work_order)));
  lbl_load_direction_->setText(
    QString("装舱方向: %1").arg(msg->load_direction));
}

void PlcStatusWidget::clear()
{
  setIndicator(lbl_machine_ready_, false);
  setAlarmIndicator(lbl_machine_broken_, false);
  setAlarmIndicator(lbl_emergency_stop_, false);
  setIndicator(lbl_lidar_power_, false);
  setIndicator(lbl_gantry_in_position_, false);
  setIndicator(lbl_slewing_in_position_, false);
  setIndicator(lbl_luffing_in_position_, false);
  setIndicator(lbl_stretch_in_position_, false);
  setIndicator(lbl_ship_scanning_, false);
  setIndicator(lbl_hatch_switch_, false);
  setIndicator(lbl_ship_loading_, false);
  setIndicator(lbl_ship_loading_stop_, false);
  setIndicator(lbl_device_to_zero_, false);
  setIndicator(lbl_return_first_, false);
  setIndicator(lbl_return_second_, false);
  setIndicator(lbl_move_next_point_, false);
  setIndicator(lbl_move_over_hatch_, false);
  setIndicator(lbl_lidar_join_, false);

  lbl_auto_system_mode_->setText("自动化系统模式: 暂无");
  lbl_auto_working_mode_->setText("自动化作业模式: 暂无");
  lbl_belt_accumulation_->setText("皮带累计量: 暂无");
  lbl_hatch_inst_flow_->setText("瞬时流量: 暂无");
  lbl_return_point_->setText("交还点 ID: 暂无");
  lbl_ship_th_thresholds_->setText("首尾减速/停止距离: 暂无");
  lbl_ship_sr_thresholds_->setText("海陆侧减速/停止距离: 暂无");
  lbl_load_spacing_->setText("装载长/宽间距: 暂无");
  lbl_chute_pitch_->setText("溜筒倾角 X/Y: 暂无");
  lbl_zone_front_->setText("前侧: 暂无");
  lbl_zone_left_->setText("左侧: 暂无");
  lbl_zone_right_->setText("右侧: 暂无");
  lbl_zone_bottom_->setText("下侧: 暂无");
  lbl_task_->setText("任务 ID/信息: 暂无");
  lbl_task_instruction_->setText("任务指令: 暂无");
  lbl_ship_size_->setText("船舶长/宽: 暂无");
  lbl_hatch_info_->setText("舱口数量/舱盖类型: 暂无");
  lbl_hatch_work_plan_->setText("舱口作业计划: 暂无");
  lbl_hatch_work_tech_->setText("舱口作业工艺: 暂无");
  lbl_hatch_work_order_->setText("舱口作业顺序: 暂无");
  lbl_load_direction_->setText("装舱方向: 暂无");
}

} // namespace shiploader_monitor
