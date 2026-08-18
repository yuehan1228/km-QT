#ifndef SHIPLOADER_MONITOR__PARAM_EDIT_WIDGET_H_
#define SHIPLOADER_MONITOR__PARAM_EDIT_WIDGET_H_

#include <QWidget>
#include <QTabWidget>
#include <QScrollArea>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QString>
#include <QFrame>
#include <string>
#include <vector>

namespace shiploader_monitor {

struct ParamDesc {
  std::string name;
  std::string label;
  std::string node;    // "motion" or "simulator"
  int         tab;     // motion: 0=OBB,1=First,2=Second,3=Fall,4=Debug; simulator: 5
  bool        is_bool{false};
  bool        is_int{false};
  double      min{0.0};
  double      max{100.0};
  double      step{0.01};
};

/// @brief 参数编辑面板 — 完全对齐 motion_rqt_plugin 的参数定义
class ParamEditWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ParamEditWidget(QWidget* parent = nullptr);
  ~ParamEditWidget() override;

signals:
  void setMotionDouble(const std::string& name, double value);
  void setMotionInt(const std::string& name, int value);
  void setMotionBool(const std::string& name, bool value);
  void setPlcSimDouble(const std::string& name, double value);
  void setPlcSimInt(const std::string& name, int value);
  void setPlcSimBool(const std::string& name, bool value);
  void setPerceptionDouble(const std::string& name, double value);
  void setMaterialDouble(const std::string& name, double value);
  void fetchMotionParams(const std::vector<std::string>& names);
  void fetchPlcSimParams(const std::vector<std::string>& names);
  void fetchPerceptionParams(const std::vector<std::string>& names);
  void fetchMaterialParams(const std::vector<std::string>& names);

public slots:
  void onMotionParamsFetched(const QMap<QString, double>& doubles,
                             const QMap<QString, int>& ints,
                             const QMap<QString, bool>& bools);
  void onPlcSimParamsFetched(const QMap<QString, double>& doubles,
                             const QMap<QString, int>& ints,
                             const QMap<QString, bool>& bools);
  void onPerceptionParamsFetched(const QMap<QString, double>& doubles,
                                 const QMap<QString, int>& ints,
                                 const QMap<QString, bool>& bools);
  void onMaterialParamsFetched(const QMap<QString, double>& doubles,
                               const QMap<QString, int>& ints,
                               const QMap<QString, bool>& bools);
  void onParamSetResult(const QString& target, const QString& name, bool success);

public slots:
  /// 刷新全部参数（motion + plc_simulator）
  void onRefreshAll();
  /// 只刷新调试只读参数（motion_node 的 debug_ 前缀参数）
  void onRefreshDebug();

private slots:
  void onParamChanged();

private:
  void setupUI();
  void populateWidgets();
  void applyParameter(const std::string& name, QWidget* w);
  void setWidgetValue(QWidget* w, double val);
  void setWidgetValue(QWidget* w, int val);
  void setWidgetValue(QWidget* w, bool val);

  QTabWidget* tab_widget_;
  QLabel* status_label_;
  bool updating_widgets_{false};

  // 6 个 Tab 的 scroll area + 内部容器
  QScrollArea* scroll_areas_[7];
  QWidget* tab_containers_[7];

  // 控件映射
  QMap<QString, QDoubleSpinBox*> double_widgets_;
  QMap<QString, QSpinBox*>       int_widgets_;
  QMap<QString, QCheckBox*>      bool_widgets_;
  QMap<QString, QLabel*>         readonly_labels_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__PARAM_EDIT_WIDGET_H_
