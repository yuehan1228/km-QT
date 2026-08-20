#include "shiploader_monitor/main_window.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMessageBox>
#include <QScrollArea>
#include <QSplitter>
#include <QFile>
#include <QVBoxLayout>
#include <QDir>
#include <QStandardPaths>
#include <ament_index_cpp/get_package_share_directory.hpp>

namespace shiploader_monitor {

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
  config_manager_ = std::make_shared<ConfigManager>(this);
  data_store_     = std::make_shared<DataStore>(this);

  setupUI();
  setupConnections();
  loadConfig();
}

MainWindow::~MainWindow()
{
  saveConfig();
  if (ros_node_) {
    ros_node_->stop();
  }
}

void MainWindow::init()
{
  // 创建 ROS 节点并启动 spin 线程
  ros_node_ = std::make_shared<RosNode>(this);

  // 连接 ROS 信号到 DataStore（QueuedConnection 跨线程）
  QObject::connect(ros_node_.get(), &RosNode::plcStatusReceived,
                   data_store_.get(), &DataStore::updatePlcStatus,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::shipLoaderPoseReceived,
                   data_store_.get(), &DataStore::updateShipLoaderPose,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::heartbeatReceived,
                   data_store_.get(), &DataStore::updateHeartbeat,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::workflowStatusReceived,
                   data_store_.get(), &DataStore::updateWorkflowStatus,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::shipLoaderSpeedReceived,
                   data_store_.get(), &DataStore::updateShipLoaderSpeed,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::targetPoseReceived,
                   data_store_.get(), &DataStore::updateTargetPose,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::hatchPositionReceived,
                   data_store_.get(), &DataStore::updateHatchPosition,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::connectionStatusChanged,
                   this, &MainWindow::onConnectionStatusChanged,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::statusCodeReceived,
                   heartbeat_widget_, &HeartbeatWidget::updateStatusCode,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::deviceEnableReceived,
                   data_store_.get(), &DataStore::updateDeviceEnable,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::deviceSafetyStatusReceived,
                   data_store_.get(), &DataStore::updateDeviceSafetyStatus,
                   Qt::QueuedConnection);

  // 注册点云话题并连接信号
  point_cloud_widget_->registerTopic("/ship_model_point_cloud", QVector3D(1.0f, 1.0f, 1.0f));
  point_cloud_widget_->registerTopic("/material_point_cloud", QVector3D(1.0f, 0.8f, 0.2f));
  point_cloud_widget_->registerTopic("/filtered_cloud", QVector3D(0.2f, 0.9f, 0.3f));
  point_cloud_widget_->registerTopic("/merged_points", QVector3D(0.8f, 0.4f, 1.0f));
  point_cloud_widget_->registerTopic("/rviz_arm_bbox", QVector3D(1.0f, 0.3f, 0.3f));
  point_cloud_widget_->registerTopic("/rviz_chute_platform_bbox", QVector3D(0.3f, 1.0f, 1.0f));
  point_cloud_widget_->registerTopic("/rviz_chute_bbox", QVector3D(1.0f, 0.5f, 0.0f));
  point_cloud_widget_->registerTopic("/rviz_loadpos", QVector3D(0.5f, 1.0f, 0.5f));
  point_cloud_widget_->registerTopic("/rviz_target", QVector3D(1.0f, 0.0f, 0.0f));
  point_cloud_widget_->registerTopic(
    "/update_ship_attitude_markers", QVector3D(0.2f, 0.8f, 1.0f), "update_ship_markers");
  point_cloud_widget_->registerTopic(
    "/ship_attitude_markers", QVector3D(1.0f, 0.0f, 1.0f), "ship_markers");

  QObject::connect(ros_node_.get(), &RosNode::pointCloudReceived,
                   point_cloud_widget_, &PointCloudWidget::updatePointCloud,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::markerArrayReceived,
                   point_cloud_widget_, &PointCloudWidget::updateMarkerArray,
                   Qt::QueuedConnection);
  QObject::connect(ros_node_.get(), &RosNode::markerReceived,
                   point_cloud_widget_, &PointCloudWidget::updateMarker,
                   Qt::QueuedConnection);
  // ── 初始化参数写入客户端 ─────────────────────────────────────
  param_writer_ = std::make_shared<ParamWriter>(this);

  // ── 连接 ParamEditWidget 信号到 ParamWriter ──────────────────
  // motion_node 参数写入
  connect(param_edit_widget_, &ParamEditWidget::setMotionDouble,
          param_writer_.get(), &ParamWriter::setMotionDouble);
  connect(param_edit_widget_, &ParamEditWidget::setMotionInt,
          param_writer_.get(), &ParamWriter::setMotionInt);
  connect(param_edit_widget_, &ParamEditWidget::setMotionBool,
          param_writer_.get(), &ParamWriter::setMotionBool);
  // plc_interface_node 参数写入
  connect(param_edit_widget_, &ParamEditWidget::setPlcSimDouble,
          param_writer_.get(), &ParamWriter::setPlcSimDouble);
  connect(param_edit_widget_, &ParamEditWidget::setPlcSimInt,
          param_writer_.get(), &ParamWriter::setPlcSimInt);
  connect(param_edit_widget_, &ParamEditWidget::setPlcSimBool,
          param_writer_.get(), &ParamWriter::setPlcSimBool);
  // 参数刷新
  connect(param_edit_widget_, &ParamEditWidget::fetchMotionParams,
          param_writer_.get(), &ParamWriter::fetchMotionParams);
  connect(param_edit_widget_, &ParamEditWidget::fetchPlcSimParams,
          param_writer_.get(), &ParamWriter::fetchPlcSimParams);
  // 刷新结果回调
  connect(param_writer_.get(), &ParamWriter::motionParamsFetched,
          param_edit_widget_, &ParamEditWidget::onMotionParamsFetched);
  connect(param_writer_.get(), &ParamWriter::plcSimParamsFetched,
          param_edit_widget_, &ParamEditWidget::onPlcSimParamsFetched);
  // 写入结果通知
  connect(param_writer_.get(), &ParamWriter::paramSetResult,
          param_edit_widget_, &ParamEditWidget::onParamSetResult);

  // perception_node 参数写入
  connect(param_edit_widget_, &ParamEditWidget::setPerceptionDouble,
          param_writer_.get(), &ParamWriter::setPerceptionDouble);
  // material_distribution_node 参数写入
  connect(param_edit_widget_, &ParamEditWidget::setMaterialDouble,
          param_writer_.get(), &ParamWriter::setMaterialDouble);
  // 参数刷新
  connect(param_edit_widget_, &ParamEditWidget::fetchPerceptionParams,
          param_writer_.get(), &ParamWriter::fetchPerceptionParams);
  connect(param_edit_widget_, &ParamEditWidget::fetchMaterialParams,
          param_writer_.get(), &ParamWriter::fetchMaterialParams);
  // 刷新结果回调
  connect(param_writer_.get(), &ParamWriter::perceptionParamsFetched,
          param_edit_widget_, &ParamEditWidget::onPerceptionParamsFetched);
  connect(param_writer_.get(), &ParamWriter::materialParamsFetched,
          param_edit_widget_, &ParamEditWidget::onMaterialParamsFetched);

  // 启动 spin 线程，在 spin 线程里初始化 param_writer
  ParamWriter* pw = param_writer_.get();
  ros_node_->start([pw](rclcpp::Node::SharedPtr node) {
    pw->init(node);
  });
}

/// Helper: 将子 widget 替换到 GroupBox 的 layout 中
static void injectWidget(QGroupBox* group, QWidget* widget)
{
  if (!group || !widget) return;
  // 清空原有 layout 内容
  QLayout* groupLayout = group->layout();
  if (groupLayout) {
    QLayoutItem* item;
    while ((item = groupLayout->takeAt(0)) != nullptr) {
      delete item;
    }
    groupLayout->addWidget(widget);
  }
}

void MainWindow::setupUI()
{
  // ── 加载 .ui 文件布局 ────────────────────────────────────────
  ui_.setupUi(this);

  // ── 创建各子面板 ─────────────────────────────────────────────
  heartbeat_widget_ = new HeartbeatWidget(this);
  plc_widget_       = new PlcStatusWidget(this);
  status_widget_    = new StatusWidget(this);
  log_widget_       = new LogWidget(this);
  param_edit_widget_= new ParamEditWidget(this);
  point_cloud_widget_ = new PointCloudWidget(this);

  // ── 注入面板到 .ui 中预定义的 GroupBox 占位符中 ──────────────

  // Tab 0: Connection — 直接注入到 connGroup 的 layout
  {
    QLayout* l = ui_.connGroup->layout();
    if (l) {
      // 找到并替换现有的占位 label
      ui_.lblConnection->hide();
      ui_.lblNodeName->hide();
      ui_.lblHbSeq->hide();
      ui_.lblStatusCode->hide();
      ui_.lblLastHbTime->hide();
    }
    // 用 ScrollArea 包裹
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(heartbeat_widget_);
    // 替换整个 tabConnection 的 layout
    QLayout* tabLayout = ui_.tabConnection->layout();
    if (tabLayout) {
      QLayoutItem* item;
      while ((item = tabLayout->takeAt(0)) != nullptr) {
        delete item;
      }
      tabLayout->addWidget(scroll);
    }
  }

  // Tab 1: PLC Status — 注入到 plcLayout 中的各组
  {
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->viewport()->setAutoFillBackground(true);
    scroll->viewport()->setPalette(Qt::white);
    scroll->setWidget(plc_widget_);
    QLayout* tabLayout = ui_.tabPlc->layout();
    if (tabLayout) {
      QLayoutItem* item;
      while ((item = tabLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
          widget->hide();
        }
        delete item;
      }
      tabLayout->addWidget(scroll);
    }
  }

  // Tab 2: 运行状态 + 点云显示（合并，左右分割）
  {
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(status_widget_);
    scroll->setMinimumWidth(350);
    splitter->addWidget(scroll);

    splitter->addWidget(point_cloud_widget_);

    // 左边 35%，右边 65%
    splitter->setStretchFactor(0, 35);
    splitter->setStretchFactor(1, 65);

    ui_.tabWidget->addTab(splitter, "运行状态 & 点云");
  }

  // Tab 3: 日志
  {
    QLayout* l = ui_.tabLog->layout();
    if (l) {
      ui_.logTable->hide();
      l->addWidget(log_widget_);
    }
  }

  // Tab 4: 参数设置
  {
    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setWidget(param_edit_widget_);
    ui_.tabWidget->addTab(scroll, "参数设置");
  }

  // ── 状态栏 ───────────────────────────────────────────────────
  status_label_ = new QLabel("正在初始化...");
  status_label_->setStyleSheet("color: orange; font-weight: bold;");
  statusBar()->addPermanentWidget(status_label_);

  // ── 日志面板的按钮信号重连到 MainWindow ──────────────────────
  // 从 .ui 定义的按钮
  connect(ui_.btnExport, &QPushButton::clicked, this, [this]() {
    onExportRequested(QString());
  });
  connect(ui_.btnClear, &QPushButton::clicked, this, &MainWindow::onClearHistory);

  // ── 定时器 ───────────────────────────────────────────────────
  refresh_timer_ = new QTimer(this);
  refresh_timer_->setInterval(200);
  connect(refresh_timer_, &QTimer::timeout, this, [this]() {
    onDataUpdated("/plc_status");
    onDataUpdated("/shiploader_pos");
    onDataUpdated("/heartbeat");
    onDataUpdated("/workflow_status");
    onDataUpdated("/ship_loader_speed");
    onDataUpdated("/ship_loader_target_pos");
    onDataUpdated("/device_enable");
    onDataUpdated("/device_safety_status");
    onHistoryChanged();
  });
  refresh_timer_->start();

  // ── 只读调试参数定时刷新（1 秒一次） ──────────────────────────
  QTimer* debugTimer = new QTimer(this);
  debugTimer->setInterval(1000);
  connect(debugTimer, &QTimer::timeout, param_edit_widget_, &ParamEditWidget::onRefreshDebug);
  debugTimer->start();
}

void MainWindow::setupConnections()
{
  // 不去连接 dataUpdated / historyChanged 信号直接触发 UI 刷新
  // UI 统一由 refresh_timer_ 定时驱动，避免高频刷新导致卡顿
  connect(log_widget_, &LogWidget::exportRequested,
          this, &MainWindow::onExportRequested);
  connect(log_widget_, &LogWidget::clearRequested,
          this, &MainWindow::onClearHistory);
}

void MainWindow::onDataUpdated(const QString& topic)
{
  if (topic == "/plc_status") {
    plc_widget_->updatePlcStatus(data_store_->latestPlcStatus());
    status_widget_->updatePlcStatus(data_store_->latestPlcStatus());
  } else if (topic == "/shiploader_pos") {
    status_widget_->updatePose(data_store_->latestShipLoaderPose());
  } else if (topic == "/heartbeat") {
    heartbeat_widget_->updateHeartbeat(data_store_->latestHeartbeat());
  } else if (topic == "/workflow_status") {
    status_widget_->updateWorkflowStatus(data_store_->latestWorkflowStatus());
  } else if (topic == "/ship_loader_speed") {
    status_widget_->updateSpeed(data_store_->latestShipLoaderSpeed());
  } else if (topic == "/ship_loader_target_pos") {
    status_widget_->updateTargetPose(data_store_->latestTargetPose());
  } else if (topic == "/device_enable") {
    status_widget_->updateDeviceEnable(data_store_->latestDeviceEnable());
  } else if (topic == "/device_safety_status") {
    status_widget_->updateDeviceSafetyStatus(data_store_->latestDeviceSafetyStatus());
  }
}

void MainWindow::onHistoryChanged()
{
  // 只显示最近 200 条，避免重建整个表格
  const auto& history = data_store_->history();
  std::deque<DataEntry> recent;
  int start = std::max(0, static_cast<int>(history.size()) - 200);
  for (size_t i = start; i < history.size(); ++i) {
    recent.push_back(history[i]);
  }
  log_widget_->refreshHistory(recent);
}

void MainWindow::onConnectionStatusChanged(bool connected)
{
  heartbeat_widget_->setConnectionStatus(connected);

  if (connected) {
    status_label_->setText("● 已连接");
    status_label_->setStyleSheet("color: green; font-weight: bold;");
  } else {
    status_label_->setText("● 未连接");
    status_label_->setStyleSheet("color: red; font-weight: bold;");
  }
}

void MainWindow::onExportRequested(const QString& filepath)
{
  bool ok = data_store_->exportToCSV(filepath);
  if (ok) {
    status_label_->setText(QString("已导出至 %1").arg(filepath));
    status_label_->setStyleSheet("color: green;");
  } else {
    QMessageBox::warning(this, "导出错误",
      QString("导出失败: %1").arg(filepath));
  }
}

void MainWindow::onClearHistory()
{
  data_store_->clearHistory();
  status_label_->setText("历史已清空");
  status_label_->setStyleSheet("color: gray;");
}

void MainWindow::loadConfig()
{
  const QString userConfigPath = QDir(
    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
      .filePath("default.yaml");

  QString configPath;
  if (QFile::exists(userConfigPath)) {
    configPath = userConfigPath;
  } else {
    try {
      const std::string shareDir =
        ament_index_cpp::get_package_share_directory("shiploader_monitor");
      configPath = QDir(QString::fromStdString(shareDir))
        .filePath("config/default.yaml");
    } catch (const std::exception&) {
      // 直接运行 build 目录中的二进制时使用源码配置作为回退。
      configPath = QApplication::applicationDirPath() + "/../../config/default.yaml";
    }
  }

  if (!QFile::exists(configPath) || !config_manager_->load(configPath)) {
    config_manager_->loadDefaults();
  }

  resize(config_manager_->windowSize());
  move(config_manager_->windowPosition());
  data_store_->setMaxHistorySize(config_manager_->historyMaxSize());
  refresh_timer_->setInterval(config_manager_->refreshIntervalMs());
}

void MainWindow::saveConfig()
{
  if (!config_manager_) return;

  config_manager_->setWindowSize(size());
  config_manager_->setWindowPosition(pos());

  const QString configPath = QDir(
    QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
      .filePath("default.yaml");
  config_manager_->save(configPath);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  saveConfig();
  QMainWindow::closeEvent(event);
}

} // namespace shiploader_monitor
