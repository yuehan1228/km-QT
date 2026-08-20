#ifndef SHIPLOADER_MONITOR__PLC_STATUS_WIDGET_H_
#define SHIPLOADER_MONITOR__PLC_STATUS_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QStringList>
#include <km_custom_msgs/msg/plc_status.hpp>

namespace shiploader_monitor {

/// @brief PLC 状态面板 —— 显示所有 PLC 信号的详细状态
class PlcStatusWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PlcStatusWidget(QWidget* parent = nullptr);
  ~PlcStatusWidget() override;

public slots:
  void updatePlcStatus(const km_custom_msgs::msg::PlcStatus::SharedPtr& msg);
  void clear();

private:
  void setupUI();

  // ── 状态指示灯辅助 ───────────────────────────────────────────
  QLabel* makeStatusIndicator(const QString& label);
  void setIndicator(QLabel* indicator, bool active);
  void setAlarmIndicator(QLabel* indicator, bool active);

  // ── 系统与流程状态 ───────────────────────────────────────────
  QLabel* lbl_machine_ready_;
  QLabel* lbl_machine_broken_;
  QLabel* lbl_emergency_stop_;
  QLabel* lbl_lidar_power_;
  QLabel* lbl_gantry_in_position_;
  QLabel* lbl_slewing_in_position_;
  QLabel* lbl_luffing_in_position_;
  QLabel* lbl_stretch_in_position_;
  QLabel* lbl_ship_scanning_;
  QLabel* lbl_hatch_switch_;
  QLabel* lbl_ship_loading_;
  QLabel* lbl_ship_loading_stop_;
  QLabel* lbl_device_to_zero_;

  // ── 作业控制 ─────────────────────────────────────────────────
  QLabel* lbl_return_first_;
  QLabel* lbl_return_second_;
  QLabel* lbl_move_next_point_;
  QLabel* lbl_move_over_hatch_;
  QLabel* lbl_lidar_join_;

  // ── 作业参数 ─────────────────────────────────────────────────
  QLabel* lbl_auto_system_mode_;
  QLabel* lbl_auto_working_mode_;
  QLabel* lbl_belt_accumulation_;
  QLabel* lbl_hatch_inst_flow_;
  QLabel* lbl_return_point_;
  QLabel* lbl_ship_th_thresholds_;
  QLabel* lbl_ship_sr_thresholds_;
  QLabel* lbl_load_spacing_;
  QLabel* lbl_chute_pitch_;

  // ── 防区状态 ─────────────────────────────────────────────────
  QLabel* lbl_zone_front_;
  QLabel* lbl_zone_left_;
  QLabel* lbl_zone_right_;
  QLabel* lbl_zone_bottom_;

  // ── 任务与船舶信息 ───────────────────────────────────────────
  QLabel* lbl_task_;
  QLabel* lbl_task_instruction_;
  QLabel* lbl_ship_size_;
  QLabel* lbl_hatch_info_;
  QLabel* lbl_hatch_work_plan_;
  QLabel* lbl_hatch_work_tech_;
  QLabel* lbl_hatch_work_order_;
  QLabel* lbl_load_direction_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__PLC_STATUS_WIDGET_H_
