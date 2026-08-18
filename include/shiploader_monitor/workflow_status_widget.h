#ifndef SHIPLOADER_MONITOR__WORKFLOW_STATUS_WIDGET_H_
#define SHIPLOADER_MONITOR__WORKFLOW_STATUS_WIDGET_H_

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QGridLayout>
#include <km_custom_msgs/msg/work_flow_status.hpp>

namespace shiploader_monitor {

/// @brief 工作流状态面板
class WorkflowStatusWidget : public QWidget
{
  Q_OBJECT

public:
  explicit WorkflowStatusWidget(QWidget* parent = nullptr);
  ~WorkflowStatusWidget() override;

public slots:
  void updateWorkflowStatus(const km_custom_msgs::msg::WorkFlowStatus::SharedPtr& msg);
  void clear();

private:
  void setupUI();
  QString statusToString(int status);

  QLabel* lbl_scanning_status_;
  QLabel* lbl_switching_status_;
  QLabel* lbl_loading_status_;
  QLabel* lbl_error_status_;
  QLabel* lbl_eccentric_status_;
  QLabel* lbl_partial_status_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__WORKFLOW_STATUS_WIDGET_H_
