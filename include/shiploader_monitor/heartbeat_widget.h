#ifndef SHIPLOADER_MONITOR__HEARTBEAT_WIDGET_H_
#define SHIPLOADER_MONITOR__HEARTBEAT_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include <QDateTime>
#include <km_custom_msgs/msg/heart_beat.hpp>
#include <km_custom_msgs/msg/status_code.hpp>

namespace shiploader_monitor {

/// @brief 心跳/连接状态面板
class HeartbeatWidget : public QWidget
{
  Q_OBJECT

public:
  explicit HeartbeatWidget(QWidget* parent = nullptr);
  ~HeartbeatWidget() override;

public slots:
  void updateHeartbeat(const km_custom_msgs::msg::HeartBeat::SharedPtr& msg);
  void updateStatusCode(const km_custom_msgs::msg::StatusCode::SharedPtr& msg);
  void setConnectionStatus(bool connected);
  void clear();

private slots:
  void checkTimeout();

private:
  void setupUI();
  void setStatusLabel(QLabel* label, const QString& name, uint16_t status);

  QLabel* lbl_connection_;
  QLabel* lbl_node_name_;
  QLabel* lbl_heartbeat_seq_;
  QLabel* lbl_status_code_;
  QLabel* lbl_last_hb_time_;
  QLabel* lbl_timeout_warning_;
  QLabel* lbl_fixed_gnss_;
  QLabel* lbl_active_gnss_;
  QLabel* lbl_fixed_lidar_;
  QLabel* lbl_active_lidar_;
  QLabel* lbl_lf_radar_;
  QLabel* lbl_lb_radar_;
  QLabel* lbl_rf_radar_;
  QLabel* lbl_rb_radar_;
  QLabel* lbl_perception_;
  QLabel* lbl_fusion_;
  QLabel* lbl_collision_;
  QLabel* lbl_motion_;
  QLabel* lbl_material_;

  QTimer* timeout_timer_;
  QDateTime last_hb_timestamp_;
  bool connected_{false};
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__HEARTBEAT_WIDGET_H_
