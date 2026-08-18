#include "shiploader_monitor/param_edit_widget.h"
#include <QScrollArea>

namespace shiploader_monitor {

// ============================================================================
//  kParamDescs — 完全对齐 motion_rqt_plugin 的参数定义
// ============================================================================

static const std::vector<ParamDesc> kParamDescs = {
  // ==================== TAB 0: OBB Collision (motion_node) ====================
  {"d_obb.arm_width", "臂架宽度 (m)", "motion", 0},
  {"d_obb.arm_height", "臂架高度 (m)", "motion", 0},
  {"d_obb.arm_length", "臂架长度 (m)", "motion", 0},
  {"d_obb.chute_radius", "溜筒半径 (m)", "motion", 0},
  {"d_obb.chute_height", "溜筒高度 (m)", "motion", 0},
  {"d_obb.chute_platform_distance", "溜筒平台距离 (m)", "motion", 0},
  {"d_obb.chute_platform_radius", "溜筒平台半径 (m)", "motion", 0},
  {"d_obb.chute_platform_height", "溜筒平台高度 (m)", "motion", 0},
  {"c_大臂碰撞停止阈值", "大臂碰撞停止阈值 (m)", "motion", 0},
  {"c_大臂碰撞减速阈值", "大臂碰撞减速阈值 (m)", "motion", 0},
  {"c_溜筒及平台碰撞停止阈值", "溜筒及平台碰撞停止阈值 (m)", "motion", 0},
  {"c_溜筒及平台碰撞减速阈值", "溜筒及平台碰撞减速阈值 (m)", "motion", 0},
  {"a_启用OBB防撞检测", "启用 OBB 防撞检测", "motion", 0, true},
  {"a_启用两阶段IK求解", "启用两阶段 IK 求解", "motion", 0, true},

  // ==================== TAB 1: First Loading (motion_node) ====================
  {"a_first左侧安全距离", "左侧安全距离 (m)", "motion", 1},
  {"a_first右侧安全距离", "右侧安全距离 (m)", "motion", 1},
  {"a_first海测安全距离", "海侧安全距离 (m)", "motion", 1},
  {"a_first路测安全距离", "陆侧安全距离 (m)", "motion", 1},
  {"a_first中线布料点间距", "中线布料点间距 (m)", "motion", 1},

  // ==================== TAB 2: Second Loading (motion_node) ====================
  {"b_second左侧安全距离", "左侧安全距离 (m)", "motion", 2},
  {"b_second右侧安全距离", "右侧安全距离 (m)", "motion", 2},
  {"b_second海测安全距离", "海侧安全距离 (m)", "motion", 2},
  {"b_second路测安全距离", "陆侧安全距离 (m)", "motion", 2},
  {"b_second蛇形布料点间距", "蛇形布料点间距 (m)", "motion", 2},
  {"b_second启用动态补料", "启用动态补料", "motion", 2, true},
  {"b_second动态补料Y间隔", "动态补料 Y 间隔 (m)", "motion", 2},
  {"b_second动态补料采样半径", "料面采样半径 (m)", "motion", 2},
  {"b_second动态补料最小采样点数", "最小采样点数", "motion", 2, false, true, 1, 100000, 1},
  {"b_second动态补料最大点数", "最大规划点数", "motion", 2, false, true, 1, 1000, 1},
  {"b_second动态补料料面权重", "料面权重", "motion", 2, false, false, 0.0, 100.0, 0.1},
  {"b_second动态补料移动代价权重", "移动代价权重", "motion", 2, false, false, 0.0, 100.0, 0.1},
  {"b_second动态补料方向权重", "方向权重", "motion", 2, false, false, 0.0, 100.0, 0.1},
  {"b_second动态补料达标容差", "达标容差 (m)", "motion", 2, false, false, 0.0, 20.0, 0.05},
  {"b_second动态补料达标连续次数", "达标连续次数", "motion", 2, false, true, 1, 1000, 1},
  {"b_second动态补料选中后蛇形执行", "选中后按蛇形执行", "motion", 2, true},
  {"b_second动态补料目标料面偏移", "目标料面偏移 (m)", "motion", 2, false, false, -20.0, 20.0, 0.05},
  {"b_second动态补料点云最大延迟", "点云最大延迟 (s)", "motion", 2, false, false, 0.01, 60.0, 0.1},
  {"b_second动态补料无点云时回退固定规划", "无点云时回退固定规划", "motion", 2, true},
  // ==================== TAB 3: Loading Thresholds (motion_node) ==============
  {"c_布料溜筒最低高度", "布料溜筒最低高度 (m)", "motion", 3},
  {"c_布料俯仰最大角度", "布料俯仰最大角度 (°)", "motion", 3, false, false, -90.0, 90.0, 0.1},
  {"c_布料舱口高度偏移量", "布料舱口高度偏移量 (m)", "motion", 3, false, false, 0.0, 20.0, 0.05},
  {"move_point_height_offset", "换点高度偏移量 (m)", "motion", 3, false, false, 0.0, 20.0, 0.05},
  {"b_second偏载进入角度阈值", "偏载进入角度阈值 (°)", "motion", 3, false, false, 0.1, 30.0, 0.1},
  {"b_second偏载退出角度阈值", "偏载退出角度阈值 (°)", "motion", 3, false, false, 0.0, 30.0, 0.1},
  {"b_second严重偏载角度阈值", "严重偏载角度阈值 (°)", "motion", 3, false, false, 0.1, 30.0, 0.1},
  {"b_second偏载触发连续次数", "偏载触发连续消息数", "motion", 3, false, true, 1, 1000, 1},
  {"b_second偏载恢复连续次数", "偏载恢复连续消息数", "motion", 3, false, true, 1, 1000, 1},

  // ==================== TAB 4: Debug (read-only, motion_node) ================
  {"debug_chute_位置X", "溜筒位置 X (m)", "motion", 4},
  {"debug_chute_位置Y", "溜筒位置 Y (m)", "motion", 4},
  {"debug_chute_位置Z", "溜筒位置 Z (m)", "motion", 4},
  {"debug_goal_行走距离", "目标行走距离 (m)", "motion", 4},
  {"debug_goal_回转角度", "目标回转角度 (°)", "motion", 4},
  {"debug_goal_俯仰角度", "目标俯仰角度 (°)", "motion", 4},
  {"debug_goal_伸缩距离", "目标伸缩距离 (m)", "motion", 4},
  {"debug_goal_位置X", "目标位置 X (m)", "motion", 4},
  {"debug_goal_位置Y", "目标位置 Y (m)", "motion", 4},
  {"debug_goal_位置Z", "目标位置 Z (m)", "motion", 4},
  {"debug_goal_有效", "目标有效", "motion", 4, true},
  {"debug_臂架_边0陆侧", "臂架-陆侧边 (m)", "motion", 4},
  {"debug_臂架_边1船头", "臂架-船头边 (m)", "motion", 4},
  {"debug_臂架_边2海侧", "臂架-海侧边 (m)", "motion", 4},
  {"debug_臂架_边3船尾", "臂架-船尾边 (m)", "motion", 4},
  {"debug_臂架_最小距离", "臂架最小距离 (m)", "motion", 4},
  {"debug_溜筒_边0陆侧", "溜筒-陆侧边 (m)", "motion", 4},
  {"debug_溜筒_边1船头", "溜筒-船头边 (m)", "motion", 4},
  {"debug_溜筒_边2海侧", "溜筒-海侧边 (m)", "motion", 4},
  {"debug_溜筒_边3船尾", "溜筒-船尾边 (m)", "motion", 4},
  {"debug_溜筒_最小距离", "溜筒最小距离 (m)", "motion", 4},
  {"debug_溜筒平台_边0陆侧", "溜筒平台-陆侧边 (m)", "motion", 4},
  {"debug_溜筒平台_边1船头", "溜筒平台-船头边 (m)", "motion", 4},
  {"debug_溜筒平台_边2海侧", "溜筒平台-海侧边 (m)", "motion", 4},
  {"debug_溜筒平台_边3船尾", "溜筒平台-船尾边 (m)", "motion", 4},
  {"debug_溜筒平台_最小距离", "溜筒平台最小距离 (m)", "motion", 4},
  {"debug_OBB数据有效", "OBB 数据有效", "motion", 4, true},
  {"debug_cover_边1顶部Z", "舱盖边 1 顶部 Z (m)", "motion", 4},
  {"debug_cover_边3顶部Z", "舱盖边 3 顶部 Z (m)", "motion", 4},
  {"debug_chute_to_pile_dis", "溜筒到料面距离 (m)", "motion", 4},
  {"debug_移动检测高度", "移动检测高度 (m)", "motion", 4},
  {"debug_原始目标高度", "原始目标高度 (m)", "motion", 4},
  {"debug_干预目标高度", "干预目标高度 (m)", "motion", 4},
  {"debug_移动重量", "预计移动重量 (t)", "motion", 4},
  {"debug_当前料面高度", "当前料面高度 (m)", "motion", 4},
  {"debug_目标料面高度", "目标料面高度 (m)", "motion", 4},
  {"debug_料面高度缺口", "料面高度缺口 (m)", "motion", 4},
  {"debug_料面数据有效", "料面数据有效", "motion", 4, true},

  // ==================== TAB 5: plc_interface_node =============================
  {"read_plc_port", "PLC 读取端口", "simulator", 5, false, true, 1, 65535, 1},
  {"write_plc_port", "PLC 写入端口", "simulator", 5, false, true, 1, 65535, 1},

  // ==================== TAB 6: Perception / Material ==========================
  {"height_diff", "感知舱口高度差阈值", "perception", 6, false, false, 0.0, 20.0, 0.1},
  {"line_thres", "感知线离散度阈值", "perception", 6, false, false, 0.0, 5.0, 0.01},
  {"hatch_min_length", "舱口最小长度 (m)", "perception", 6, false, false, 0.0, 100.0, 0.1},
  {"hatch_max_length", "舱口最大长度 (m)", "perception", 6, false, false, 0.0, 100.0, 0.1},
  {"material_height_diff", "物料舱口高度差阈值", "material", 6, false, false, 0.0, 20.0, 0.1},
  {"update_threshold", "舱口位置更新阈值", "material", 6, false, false, 0.0, 20.0, 0.1},
};

// ============================================================================
//  Construction
// ============================================================================

ParamEditWidget::ParamEditWidget(QWidget* parent)
  : QWidget(parent)
{
  setupUI();
  populateWidgets();
}

ParamEditWidget::~ParamEditWidget() = default;

void ParamEditWidget::setupUI()
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this);

  // ── 顶部栏 ───────────────────────────────────────────────────
  QHBoxLayout* topBar = new QHBoxLayout();
  QPushButton* btnRefresh = new QPushButton("刷新全部参数");
  btnRefresh->setToolTip("从 km-shiploader 运行节点重新读取所有参数");
  connect(btnRefresh, &QPushButton::clicked, this, &ParamEditWidget::onRefreshAll);
  topBar->addWidget(btnRefresh);
  topBar->addStretch();

  status_label_ = new QLabel("就绪");
  status_label_->setStyleSheet("color: gray;");
  topBar->addWidget(status_label_);

  mainLayout->addLayout(topBar);

  // ── Tab 容器 ─────────────────────────────────────────────────
  tab_widget_ = new QTabWidget(this);
  mainLayout->addWidget(tab_widget_);

  const char* tabNames[] = {
    "OBB 防撞", "一轮布料", "二轮布料", "控制阈值",
    "调试显示(只读)", "PLC 接口", "感知系统"
  };

  for (int t = 0; t < 7; ++t) {
    tab_containers_[t] = new QWidget();
    scroll_areas_[t] = new QScrollArea();
    scroll_areas_[t]->setWidgetResizable(true);
    scroll_areas_[t]->setWidget(tab_containers_[t]);
    tab_widget_->addTab(scroll_areas_[t], tabNames[t]);
  }
}

// ============================================================================
//  populateWidgets — 对齐 motion_rqt_plugin
// ============================================================================

void ParamEditWidget::populateWidgets()
{
  QGridLayout* grids[7];
  int rows[7] = {0, 0, 0, 0, 0, 0, 0};
  for (int t = 0; t < 7; ++t) {
    grids[t] = new QGridLayout(tab_containers_[t]);
  }

  // Group boxes
  QMap<QString, QGroupBox*> groups[7];
  QMap<QString, QGridLayout*> groupGrids[7];
  int groupRows[7] = {0, 0, 0, 0, 0, 0, 0};

  auto getGroup = [&](int tab, const QString& gname) -> QGridLayout* {
    if (groups[tab].contains(gname)) return groupGrids[tab][gname];
    auto* gb = new QGroupBox(gname);
    auto* gl = new QGridLayout(gb);
    groups[tab][gname] = gb;
    groupGrids[tab][gname] = gl;
    grids[tab]->addWidget(gb, rows[tab]++, 0, 1, 1);
    return gl;
  };

  for (auto& d : kParamDescs) {
    int t = d.tab;
    bool is_readonly = (d.name.find("debug_") == 0) || t == 4;

    // Determine group
    QString group;
    if (t == 0) {
      if (d.name.find("d_obb") == 0 || d.name.find("d_collision") == 0)
        group = "OBB 尺寸";
      else if (d.name.find("c_") == 0)
        group = "碰撞阈值";
      else
        group = "开关 & 其他";
    } else if (t == 1) {
      group = d.is_bool ? "开关" : "安全距离";
    } else if (t == 2) {
      if (d.name.find("b_second动态补料") == 0)
        group = "动态补料参数";
      else
        group = d.is_bool ? "开关" : "安全距离";
    } else if (t == 3) {
      if (d.name.find("b_second偏载") == 0 ||
          d.name.find("b_second严重偏载") == 0)
        group = "偏载纠偏参数";
      else
        group = "控制阈值";
    } else if (t == 4) {
      if (d.name.find("debug_chute_位置") == 0)     group = "溜筒位置";
      else if (d.name.find("debug_goal_") == 0)     group = "目标关节/位置";
      else if (d.name.find("debug_臂架") == 0)      group = "臂架碰撞距离";
      else if (d.name.find("debug_溜筒") == 0)      group = "溜筒碰撞距离";
      else if (d.name.find("debug_旧版") == 0)      group = "旧版防撞";
      else if (d.name.find("debug_cover") == 0)     group = "Cover Diff";
      else group = "高度检测";
    } else {
      if (d.name.find("hatch_") == 0)              group = "舱口数据";
      else if (d.name.find("pose.") == 0)          group = "设备初始关节";
      else if (d.name.find("sim.") == 0)           group = "运动模拟参数";
      else if (d.name.find("plc.is_") == 0)        group = "PLC 状态开关";
      else if (d.name.find("plc.single_") == 0 ||
               d.name.find("plc.target_") == 0 ||
               d.name.find("plc.current_") == 0 ||
               d.name.find("plc.return_") == 0)     group = "PLC 工艺参数";
      else if (d.name.find("plc.ship_scan_") == 0 ||
               d.name.find("plc.slide_chute_") == 0 ||
               d.name.find("plc.hatch_inst_") == 0 ||
               d.name.find("plc.autoluffangadj") == 0) group = "PLC 传感器/数值";
      else if (d.name.find("pile_sim.") == 0)      group = "物料堆积模拟";
      else group = "其他";
    }

    QGridLayout* gl = getGroup(t, group);
    int r = groupRows[t]++;

    auto* label = new QLabel(QString::fromStdString(d.label));
    label->setMinimumWidth(180);
    gl->addWidget(label, r, 0);

    if (is_readonly) {
      auto* rl = new QLabel("---");
      rl->setFrameStyle(QFrame::Panel | QFrame::Sunken);
      rl->setStyleSheet("background: #f0f0f0; padding: 2px 4px;");
      rl->setMinimumWidth(100);
      gl->addWidget(rl, r, 1);
      readonly_labels_[QString::fromStdString(d.name)] = rl;
    } else if (d.is_bool) {
      auto* cb = new QCheckBox();
      connect(cb, &QCheckBox::toggled, this, &ParamEditWidget::onParamChanged);
      gl->addWidget(cb, r, 1);
      bool_widgets_[QString::fromStdString(d.name)] = cb;
    } else if (d.is_int) {
      auto* sb = new QSpinBox();
      sb->setRange(static_cast<int>(d.min), static_cast<int>(d.max));
      sb->setSingleStep(static_cast<int>(d.step));
      sb->setMinimumWidth(100);
      connect(sb, QOverload<int>::of(&QSpinBox::valueChanged),
              this, &ParamEditWidget::onParamChanged);
      gl->addWidget(sb, r, 1);
      int_widgets_[QString::fromStdString(d.name)] = sb;
    } else {
      auto* dsb = new QDoubleSpinBox();
      dsb->setRange(d.min, d.max);
      dsb->setSingleStep(d.step);
      dsb->setDecimals(3);
      dsb->setMinimumWidth(100);
      connect(dsb, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
              this, &ParamEditWidget::onParamChanged);
      gl->addWidget(dsb, r, 1);
      double_widgets_[QString::fromStdString(d.name)] = dsb;
    }
  }
}

// ============================================================================
//  applyParameter — 值改变时自动写回对应节点
// ============================================================================

void ParamEditWidget::applyParameter(const std::string& name, QWidget* w)
{
  if (updating_widgets_) return;

  std::string node;
  for (auto& d : kParamDescs) {
    if (d.name == name) { node = d.node; break; }
  }
  if (node.empty()) return;

  if (auto* dsb = qobject_cast<QDoubleSpinBox*>(w)) {
    if (node == "perception")
      emit setPerceptionDouble(name, dsb->value());
    else if (node == "material")
      emit setMaterialDouble(name, dsb->value());
    else if (node == "simulator")
      emit setPlcSimDouble(name, dsb->value());
    else
      emit setMotionDouble(name, dsb->value());
  } else if (auto* sb = qobject_cast<QSpinBox*>(w)) {
    if (node == "simulator")
      emit setPlcSimInt(name, sb->value());
    else
      emit setMotionInt(name, sb->value());
  } else if (auto* cb = qobject_cast<QCheckBox*>(w)) {
    if (node == "simulator")
      emit setPlcSimBool(name, cb->isChecked());
    else
      emit setMotionBool(name, cb->isChecked());
  }
}

void ParamEditWidget::onParamChanged()
{
  if (updating_widgets_) return;
  QWidget* sender = qobject_cast<QWidget*>(QObject::sender());
  if (!sender) return;

  for (auto it = double_widgets_.begin(); it != double_widgets_.end(); ++it) {
    if (it.value() == sender) { applyParameter(it.key().toStdString(), sender); return; }
  }
  for (auto it = int_widgets_.begin(); it != int_widgets_.end(); ++it) {
    if (it.value() == sender) { applyParameter(it.key().toStdString(), sender); return; }
  }
  for (auto it = bool_widgets_.begin(); it != bool_widgets_.end(); ++it) {
    if (it.value() == sender) { applyParameter(it.key().toStdString(), sender); return; }
  }
}

// ============================================================================
//  setWidgetValue
// ============================================================================

void ParamEditWidget::setWidgetValue(QWidget* w, double val)
{
  if (auto* dsb = qobject_cast<QDoubleSpinBox*>(w)) dsb->setValue(val);
  else if (auto* lb = qobject_cast<QLabel*>(w)) lb->setText(QString::number(val, 'f', 3));
}
void ParamEditWidget::setWidgetValue(QWidget* w, int val)
{
  if (auto* sb = qobject_cast<QSpinBox*>(w)) sb->setValue(val);
  else if (auto* lb = qobject_cast<QLabel*>(w)) lb->setText(QString::number(val));
}
void ParamEditWidget::setWidgetValue(QWidget* w, bool val)
{
  if (auto* cb = qobject_cast<QCheckBox*>(w)) cb->setChecked(val);
  else if (auto* lb = qobject_cast<QLabel*>(w)) lb->setText(val ? "true" : "false");
}

// ============================================================================
//  参数刷新回调
// ============================================================================

void ParamEditWidget::onMotionParamsFetched(const QMap<QString, double>& doubles,
                                             const QMap<QString, int>& ints,
                                             const QMap<QString, bool>& bools)
{
  updating_widgets_ = true;

  for (auto it = doubles.begin(); it != doubles.end(); ++it) {
    if (double_widgets_.contains(it.key()))
      setWidgetValue(double_widgets_[it.key()], it.value());
    else if (readonly_labels_.contains(it.key()))
      setWidgetValue(readonly_labels_[it.key()], it.value());
  }
  for (auto it = ints.begin(); it != ints.end(); ++it) {
    if (int_widgets_.contains(it.key()))
      setWidgetValue(int_widgets_[it.key()], it.value());
    else if (readonly_labels_.contains(it.key()))
      setWidgetValue(readonly_labels_[it.key()], it.value());
  }
  for (auto it = bools.begin(); it != bools.end(); ++it) {
    if (bool_widgets_.contains(it.key()))
      setWidgetValue(bool_widgets_[it.key()], it.value());
    else if (readonly_labels_.contains(it.key()))
      setWidgetValue(readonly_labels_[it.key()], it.value());
  }

  updating_widgets_ = false;
  status_label_->setText("motion_node 参数刷新完成");
  status_label_->setStyleSheet("color: green;");
}

void ParamEditWidget::onPlcSimParamsFetched(const QMap<QString, double>& doubles,
                                             const QMap<QString, int>& ints,
                                             const QMap<QString, bool>& bools)
{
  updating_widgets_ = true;

  for (auto it = doubles.begin(); it != doubles.end(); ++it) {
    if (double_widgets_.contains(it.key()))
      setWidgetValue(double_widgets_[it.key()], it.value());
    else if (readonly_labels_.contains(it.key()))
      setWidgetValue(readonly_labels_[it.key()], it.value());
  }
  for (auto it = ints.begin(); it != ints.end(); ++it) {
    if (int_widgets_.contains(it.key()))
      setWidgetValue(int_widgets_[it.key()], it.value());
    else if (readonly_labels_.contains(it.key()))
      setWidgetValue(readonly_labels_[it.key()], it.value());
  }
  for (auto it = bools.begin(); it != bools.end(); ++it) {
    if (bool_widgets_.contains(it.key()))
      setWidgetValue(bool_widgets_[it.key()], it.value());
    else if (readonly_labels_.contains(it.key()))
      setWidgetValue(readonly_labels_[it.key()], it.value());
  }

  updating_widgets_ = false;
  status_label_->setText("plc_interface_node 参数刷新完成");
  status_label_->setStyleSheet("color: green;");
}

void ParamEditWidget::onPerceptionParamsFetched(const QMap<QString, double>& doubles,
                                                 const QMap<QString, int>& ints,
                                                 const QMap<QString, bool>& bools)
{
  updating_widgets_ = true;
  for (auto it = doubles.begin(); it != doubles.end(); ++it)
    if (double_widgets_.contains(it.key())) setWidgetValue(double_widgets_[it.key()], it.value());
  for (auto it = ints.begin(); it != ints.end(); ++it)
    if (int_widgets_.contains(it.key())) setWidgetValue(int_widgets_[it.key()], it.value());
  for (auto it = bools.begin(); it != bools.end(); ++it)
    if (bool_widgets_.contains(it.key())) setWidgetValue(bool_widgets_[it.key()], it.value());
  updating_widgets_ = false;
  status_label_->setText("perception_node 参数刷新完成");
  status_label_->setStyleSheet("color: green;");
}

void ParamEditWidget::onMaterialParamsFetched(const QMap<QString, double>& doubles,
                                              const QMap<QString, int>& ints,
                                              const QMap<QString, bool>& bools)
{
  updating_widgets_ = true;
  for (auto it = doubles.begin(); it != doubles.end(); ++it)
    if (double_widgets_.contains(it.key())) setWidgetValue(double_widgets_[it.key()], it.value());
  for (auto it = ints.begin(); it != ints.end(); ++it)
    if (int_widgets_.contains(it.key())) setWidgetValue(int_widgets_[it.key()], it.value());
  for (auto it = bools.begin(); it != bools.end(); ++it)
    if (bool_widgets_.contains(it.key())) setWidgetValue(bool_widgets_[it.key()], it.value());
  updating_widgets_ = false;
  status_label_->setText("material_distribution_node 参数刷新完成");
  status_label_->setStyleSheet("color: green;");
}

void ParamEditWidget::onParamSetResult(const QString& target, const QString& name, bool success)
{
  if (success) {
    status_label_->setText(QString("[%1] %2 写入成功").arg(target, name));
    status_label_->setStyleSheet("color: green; font-weight: bold;");
  } else {
    status_label_->setText(QString("[%1] %2 写入失败").arg(target, name));
    status_label_->setStyleSheet("color: red; font-weight: bold;");
  }
}

void ParamEditWidget::onRefreshAll()
{
  status_label_->setText("正在获取参数...");
  status_label_->setStyleSheet("color: orange;");

  std::vector<std::string> motionNames, simNames, perceptionNames, materialNames;
  for (auto& d : kParamDescs) {
    if (d.node == "simulator")
      simNames.push_back(d.name);
    else if (d.node == "perception")
      perceptionNames.push_back(d.name);
    else if (d.node == "material")
      materialNames.push_back(d.name);
    else
      motionNames.push_back(d.name);
  }
  if (!motionNames.empty()) emit fetchMotionParams(motionNames);
  if (!simNames.empty()) emit fetchPlcSimParams(simNames);
  if (!perceptionNames.empty()) emit fetchPerceptionParams(perceptionNames);
  if (!materialNames.empty()) emit fetchMaterialParams(materialNames);
}

void ParamEditWidget::onRefreshDebug()
{
  // 只刷新 motion_node 的 debug_ 前缀参数（只读调试 Tab）
  std::vector<std::string> debugNames;
  for (auto& d : kParamDescs) {
    if (d.name.find("debug_") == 0)
      debugNames.push_back(d.name);
  }
  if (!debugNames.empty())
    emit fetchMotionParams(debugNames);
}

} // namespace shiploader_monitor
 