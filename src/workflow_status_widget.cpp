#include "shiploader_monitor/workflow_status_widget.h"
#include <QFont>

namespace shiploader_monitor {

WorkflowStatusWidget::WorkflowStatusWidget(QWidget* parent)
  : QWidget(parent)
{
  setupUI();
}

WorkflowStatusWidget::~WorkflowStatusWidget() = default;

QString WorkflowStatusWidget::statusToString(int status)
{
  switch (status) {
    case 0: return "停止";
    case 1: return "运行中";
    case 2: return "完成";
    case 3: return "异常";
    case 4: return "初始化中";
    default: return "未知";
  }
}

void WorkflowStatusWidget::setupUI()
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this);

  QGroupBox* group = new QGroupBox("工作流状态", this);
  QGridLayout* grid = new QGridLayout(group);

  QFont statusFont;
  statusFont.setPointSize(11);
  statusFont.setBold(true);

  QLabel* lblScanTitle    = new QLabel("整船扫描:");
  QLabel* lblSwitchTitle  = new QLabel("移舱切换:");
  QLabel* lblLoadTitle    = new QLabel("装船作业:");
  QLabel* lblErrorTitle   = new QLabel("错误状态:");
  QLabel* lblEccTitle     = new QLabel("偏载状态:");
  QLabel* lblPartialTitle = new QLabel("局部装载:");

  lbl_scanning_status_  = new QLabel("停止");
  lbl_switching_status_ = new QLabel("停止");
  lbl_loading_status_   = new QLabel("停止");
  lbl_error_status_     = new QLabel("0");
  lbl_eccentric_status_ = new QLabel("暂无");
  lbl_partial_status_   = new QLabel("暂无");

  lbl_scanning_status_->setFont(statusFont);
  lbl_switching_status_->setFont(statusFont);
  lbl_loading_status_->setFont(statusFont);
  lbl_error_status_->setFont(statusFont);
  lbl_eccentric_status_->setFont(statusFont);
  lbl_partial_status_->setFont(statusFont);

  grid->addWidget(lblScanTitle,          0, 0);
  grid->addWidget(lbl_scanning_status_,  0, 1);
  grid->addWidget(lblSwitchTitle,        1, 0);
  grid->addWidget(lbl_switching_status_, 1, 1);
  grid->addWidget(lblLoadTitle,          2, 0);
  grid->addWidget(lbl_loading_status_,   2, 1);
  grid->addWidget(lblErrorTitle,         3, 0);
  grid->addWidget(lbl_error_status_,     3, 1);
  grid->addWidget(lblEccTitle,           4, 0);
  grid->addWidget(lbl_eccentric_status_, 4, 1);
  grid->addWidget(lblPartialTitle,       5, 0);
  grid->addWidget(lbl_partial_status_,   5, 1);

  mainLayout->addWidget(group);
  mainLayout->addStretch();
}

void WorkflowStatusWidget::updateWorkflowStatus(const km_custom_msgs::msg::WorkFlowStatus::SharedPtr& msg)
{
  if (!msg) return;

  auto setStatusStyle = [](QLabel* lbl, int status) {
    QString text;
    QString color;
    switch (status) {
      case 0: text = "停止"; color = "gray"; break;
      case 1: text = "运行中"; color = "green"; break;
      case 2: text = "完成"; color = "blue"; break;
      case 3: text = "异常"; color = "red"; break;
      case 4: text = "初始化中"; color = "orange"; break;
      default: text = "未知"; color = "gray"; break;
    }
    lbl->setText(text);
    lbl->setStyleSheet(QString("color: %1; font-weight: bold;").arg(color));
  };

  setStatusStyle(lbl_scanning_status_, msg->ship_scan_status);
  setStatusStyle(lbl_switching_status_, msg->hatch_switch_status);
  setStatusStyle(lbl_loading_status_, msg->ship_load_status);

  lbl_error_status_->setText(QString::number(msg->error_status));
  if (msg->error_status != 0) {
    lbl_error_status_->setStyleSheet("color: red; font-weight: bold;");
  } else {
    lbl_error_status_->setStyleSheet("color: green; font-weight: bold;");
  }

  lbl_eccentric_status_->setText("暂无");
  lbl_eccentric_status_->setStyleSheet("");
  lbl_partial_status_->setText("暂无");
  lbl_partial_status_->setStyleSheet("");
}

void WorkflowStatusWidget::clear()
{
  lbl_scanning_status_->setText("停止");
  lbl_scanning_status_->setStyleSheet("color: gray;");
  lbl_switching_status_->setText("停止");
  lbl_switching_status_->setStyleSheet("color: gray;");
  lbl_loading_status_->setText("停止");
  lbl_loading_status_->setStyleSheet("color: gray;");
  lbl_error_status_->setText("0");
  lbl_error_status_->setStyleSheet("color: green;");
  lbl_eccentric_status_->setText("暂无");
  lbl_eccentric_status_->setStyleSheet("");
  lbl_partial_status_->setText("暂无");
  lbl_partial_status_->setStyleSheet("");
}

} // namespace shiploader_monitor
