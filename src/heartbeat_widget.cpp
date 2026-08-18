#include "shiploader_monitor/heartbeat_widget.h"
#include <QFont>
#include <QDateTime>

namespace shiploader_monitor {

HeartbeatWidget::HeartbeatWidget(QWidget* parent)
  : QWidget(parent)
{
  setupUI();

  timeout_timer_ = new QTimer(this);
  timeout_timer_->setInterval(3000); // 3 秒无心跳则认为超时
  connect(timeout_timer_, &QTimer::timeout, this, &HeartbeatWidget::checkTimeout);
  timeout_timer_->start();
}

HeartbeatWidget::~HeartbeatWidget() = default;

void HeartbeatWidget::setupUI()
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this);

  QGroupBox* group = new QGroupBox("连接与心跳", this);
  QVBoxLayout* groupLayout = new QVBoxLayout(group);

  QFont statusFont;
  statusFont.setPointSize(14);
  statusFont.setBold(true);

  lbl_connection_ = new QLabel("● 未连接");
  lbl_connection_->setFont(statusFont);
  lbl_connection_->setStyleSheet("color: red;");
  lbl_connection_->setAlignment(Qt::AlignCenter);

  lbl_timeout_warning_ = new QLabel("");
  lbl_timeout_warning_->setStyleSheet("color: orange; font-weight: bold;");
  lbl_timeout_warning_->setAlignment(Qt::AlignCenter);

  QFont infoFont;
  infoFont.setPointSize(10);

  lbl_node_name_     = new QLabel("节点: 暂无");
  lbl_heartbeat_seq_ = new QLabel("心跳序列号: 暂无");
  lbl_status_code_   = new QLabel("状态码: 暂无");
  lbl_last_hb_time_  = new QLabel("上次心跳时间: 暂无");

  lbl_node_name_->setFont(infoFont);
  lbl_heartbeat_seq_->setFont(infoFont);
  lbl_status_code_->setFont(infoFont);
  lbl_last_hb_time_->setFont(infoFont);

  groupLayout->addWidget(lbl_connection_);
  groupLayout->addWidget(lbl_timeout_warning_);
  groupLayout->addWidget(lbl_node_name_);
  groupLayout->addWidget(lbl_heartbeat_seq_);
  groupLayout->addWidget(lbl_status_code_);
  groupLayout->addWidget(lbl_last_hb_time_);

  mainLayout->addWidget(group);

  auto* healthGroup = new QGroupBox("km-shiploader 系统健康状态", this);
  auto* healthGrid = new QGridLayout(healthGroup);

  lbl_fixed_gnss_  = new QLabel();
  lbl_active_gnss_ = new QLabel();
  lbl_fixed_lidar_ = new QLabel();
  lbl_active_lidar_= new QLabel();
  lbl_lf_radar_    = new QLabel();
  lbl_lb_radar_    = new QLabel();
  lbl_rf_radar_    = new QLabel();
  lbl_rb_radar_    = new QLabel();
  lbl_perception_  = new QLabel();
  lbl_fusion_      = new QLabel();
  lbl_collision_   = new QLabel();
  lbl_motion_      = new QLabel();
  lbl_material_    = new QLabel();

  QLabel* healthLabels[] = {
    lbl_fixed_gnss_, lbl_active_gnss_, lbl_fixed_lidar_, lbl_active_lidar_,
    lbl_lf_radar_, lbl_lb_radar_, lbl_rf_radar_, lbl_rb_radar_,
    lbl_perception_, lbl_fusion_, lbl_collision_, lbl_motion_, lbl_material_
  };
  for (int i = 0; i < 13; ++i) {
    healthGrid->addWidget(healthLabels[i], i / 3, i % 3);
  }

  mainLayout->addWidget(healthGroup);
  mainLayout->addStretch();
  clear();
}

void HeartbeatWidget::setStatusLabel(
  QLabel* label, const QString& name, uint16_t status)
{
  QString statusText;
  QString color;
  switch (status) {
    case 0:
      statusText = "正常";
      color = "green";
      break;
    case 1:
      statusText = "超时";
      color = "red";
      break;
    case 2:
      statusText = "数据未更新";
      color = "orange";
      break;
    default:
      statusText = "未知";
      color = "gray";
      break;
  }
  label->setText(QString("%1: %2").arg(name, statusText));
  label->setStyleSheet(QString("color: %1; font-weight: bold;").arg(color));
}

void HeartbeatWidget::updateHeartbeat(const km_custom_msgs::msg::HeartBeat::SharedPtr& msg)
{
  if (!msg) return;

  last_hb_timestamp_ = QDateTime::currentDateTime();

  lbl_connection_->setText("● 已连接");
  lbl_connection_->setStyleSheet("color: green; font-weight: bold;");
  lbl_timeout_warning_->setText("");
  connected_ = true;

  lbl_node_name_->setText(QString("节点: %1").arg(QString::fromStdString(msg->node_name)));
  lbl_heartbeat_seq_->setText(QString("心跳序列号: %1").arg(msg->heartbeat_seq));
  lbl_status_code_->setText(QString("状态码: %1").arg(static_cast<int>(msg->status_code)));
  lbl_last_hb_time_->setText(QString("上次心跳时间: %1")
    .arg(last_hb_timestamp_.toString("hh:mm:ss.zzz")));
}

void HeartbeatWidget::updateStatusCode(
  const km_custom_msgs::msg::StatusCode::SharedPtr& msg)
{
  if (!msg) return;

  setStatusLabel(lbl_fixed_gnss_, "固定端 GNSS", msg->fixed_gnss_status);
  setStatusLabel(lbl_active_gnss_, "活动端 GNSS", msg->active_gnss_status);
  setStatusLabel(lbl_fixed_lidar_, "固定端激光雷达", msg->fixed_lidar_status);
  setStatusLabel(lbl_active_lidar_, "活动端激光雷达", msg->active_lidar_status);
  setStatusLabel(lbl_lf_radar_, "左前毫米波雷达", msg->lf_radar_status);
  setStatusLabel(lbl_lb_radar_, "左后毫米波雷达", msg->lb_radar_status);
  setStatusLabel(lbl_rf_radar_, "右前毫米波雷达", msg->rf_radar_status);
  setStatusLabel(lbl_rb_radar_, "右后毫米波雷达", msg->rb_radar_status);
  setStatusLabel(lbl_perception_, "感知节点", msg->perception_node_status);
  setStatusLabel(lbl_fusion_, "融合节点", msg->fusion_node_status);
  setStatusLabel(lbl_collision_, "防撞节点", msg->collision_node_status);
  setStatusLabel(lbl_motion_, "运动控制节点", msg->motion_node_status);
  setStatusLabel(lbl_material_, "物料节点", msg->material_node_status);
}

void HeartbeatWidget::setConnectionStatus(bool connected)
{
  connected_ = connected;
  if (!connected) {
    lbl_connection_->setText("● 未连接");
    lbl_connection_->setStyleSheet("color: red; font-weight: bold;");
  }
}

void HeartbeatWidget::checkTimeout()
{
  if (!connected_) return;

  qint64 elapsed = last_hb_timestamp_.msecsTo(QDateTime::currentDateTime());
  if (elapsed > 3000) {
    lbl_timeout_warning_->setText(QString("⚠ 心跳超时: %1 ms").arg(elapsed));
    lbl_timeout_warning_->setStyleSheet("color: red; font-weight: bold;");
  } else if (elapsed > 1500) {
    lbl_timeout_warning_->setText(QString("⚠ 心跳延迟: %1 ms").arg(elapsed));
    lbl_timeout_warning_->setStyleSheet("color: orange; font-weight: bold;");
  }
}

void HeartbeatWidget::clear()
{
  lbl_connection_->setText("● 未连接");
  lbl_connection_->setStyleSheet("color: red; font-weight: bold;");
  lbl_timeout_warning_->setText("");
  lbl_node_name_->setText("节点: 暂无");
  lbl_heartbeat_seq_->setText("心跳序列号: 暂无");
  lbl_status_code_->setText("状态码: 暂无");
  lbl_last_hb_time_->setText("上次心跳时间: 暂无");
  setStatusLabel(lbl_fixed_gnss_, "固定端 GNSS", 255);
  setStatusLabel(lbl_active_gnss_, "活动端 GNSS", 255);
  setStatusLabel(lbl_fixed_lidar_, "固定端激光雷达", 255);
  setStatusLabel(lbl_active_lidar_, "活动端激光雷达", 255);
  setStatusLabel(lbl_lf_radar_, "左前毫米波雷达", 255);
  setStatusLabel(lbl_lb_radar_, "左后毫米波雷达", 255);
  setStatusLabel(lbl_rf_radar_, "右前毫米波雷达", 255);
  setStatusLabel(lbl_rb_radar_, "右后毫米波雷达", 255);
  setStatusLabel(lbl_perception_, "感知节点", 255);
  setStatusLabel(lbl_fusion_, "融合节点", 255);
  setStatusLabel(lbl_collision_, "防撞节点", 255);
  setStatusLabel(lbl_motion_, "运动控制节点", 255);
  setStatusLabel(lbl_material_, "物料节点", 255);
  connected_ = false;
}

} // namespace shiploader_monitor
