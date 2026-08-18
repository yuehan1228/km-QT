#ifndef SHIPLOADER_MONITOR__MAIN_WINDOW_H_
#define SHIPLOADER_MONITOR__MAIN_WINDOW_H_

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QGridLayout>
#include <QGroupBox>
#include <memory>
#include <thread>

#include "ui_main_window.h"
#include "shiploader_monitor/ros_node.h"
#include "shiploader_monitor/data_store.h"
#include "shiploader_monitor/config_manager.h"
#include "shiploader_monitor/param_writer.h"
#include "shiploader_monitor/plc_status_widget.h"
#include "shiploader_monitor/pose_display_widget.h"
#include "shiploader_monitor/heartbeat_widget.h"
#include "shiploader_monitor/status_widget.h"
#include "shiploader_monitor/log_widget.h"
#include "shiploader_monitor/param_edit_widget.h"
#include "shiploader_monitor/point_cloud_widget.h"

namespace shiploader_monitor {

/// @brief 主窗口 —— 加载 .ui 文件布局，注入子面板到占位 GroupBox 中
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

  /// 初始化（ROS2 初始化后调用）
  void init();

protected:
  void closeEvent(QCloseEvent* event) override;

private slots:
  void onDataUpdated(const QString& topic);
  void onHistoryChanged();
  void onConnectionStatusChanged(bool connected);
  void onExportRequested(const QString& filepath);
  void onClearHistory();

private:
  void setupUI();
  void setupConnections();
  void loadConfig();
  void saveConfig();

  // ── 核心组件 ─────────────────────────────────────────────────
  std::shared_ptr<RosNode> ros_node_;
  std::shared_ptr<DataStore> data_store_;
  std::shared_ptr<ConfigManager> config_manager_;
  std::shared_ptr<ParamWriter> param_writer_;

  // ── UI（从 .ui 文件加载） ─────────────────────────────────────
  Ui::MainWindowUI ui_;

  // ── 子面板 ───────────────────────────────────────────────────
  PlcStatusWidget* plc_widget_;
  StatusWidget* status_widget_;
  HeartbeatWidget* heartbeat_widget_;
  LogWidget* log_widget_;
  ParamEditWidget* param_edit_widget_;
  PointCloudWidget* point_cloud_widget_;

  QLabel* status_label_;
  QTimer* refresh_timer_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__MAIN_WINDOW_H_
