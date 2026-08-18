#include "shiploader_monitor/log_widget.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QLabel>

namespace shiploader_monitor {

LogWidget::LogWidget(QWidget* parent)
  : QWidget(parent)
{
  setupUI();
}

LogWidget::~LogWidget() = default;

void LogWidget::setupUI()
{
  QVBoxLayout* mainLayout = new QVBoxLayout(this);

  // ── 顶部栏 ───────────────────────────────────────────────────
  QHBoxLayout* topLayout = new QHBoxLayout();

  lbl_count_ = new QLabel("记录数: 0");
  topLayout->addWidget(lbl_count_);
  topLayout->addStretch();

  btn_export_ = new QPushButton("导出 CSV");
  btn_export_->setToolTip("导出历史数据到 CSV 文件");
  btn_clear_  = new QPushButton("清空");
  btn_clear_->setToolTip("清空所有历史记录");

  topLayout->addWidget(btn_export_);
  topLayout->addWidget(btn_clear_);

  mainLayout->addLayout(topLayout);

  // ── 表格 ─────────────────────────────────────────────────────
  table_ = new QTableWidget(this);
  table_->setColumnCount(3);
  table_->setHorizontalHeaderLabels({"时间戳", "主题", "摘要"});
  table_->horizontalHeader()->setStretchLastSection(true);
  table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->setAlternatingRowColors(true);

  mainLayout->addWidget(table_);

  // ── 信号连接 ─────────────────────────────────────────────────
  connect(btn_export_, &QPushButton::clicked, this, &LogWidget::onExportClicked);
  connect(btn_clear_,  &QPushButton::clicked, this, &LogWidget::onClearClicked);
}

void LogWidget::refreshHistory(const std::deque<DataEntry>& history)
{
  table_->setRowCount(0);

  int row = 0;
  for (const auto& entry : history) {
    table_->insertRow(row);
    table_->setItem(row, 0, new QTableWidgetItem(entry.timestamp.toString("hh:mm:ss.zzz")));
    table_->setItem(row, 1, new QTableWidgetItem(entry.topic));
    table_->setItem(row, 2, new QTableWidgetItem(entry.summary));
    ++row;
  }

  lbl_count_->setText(QString("记录数: %1").arg(history.size()));

  if (row > 0) {
    table_->scrollToBottom();
  }
}

void LogWidget::clear()
{
  table_->setRowCount(0);
  lbl_count_->setText("记录数: 0");
}

void LogWidget::onExportClicked()
{
  QString filepath = QFileDialog::getSaveFileName(
    this, "导出历史数据", "装船机监控日志.csv", "CSV 文件 (*.csv)");
  if (!filepath.isEmpty()) {
    emit exportRequested(filepath);
  }
}

void LogWidget::onClearClicked()
{
  table_->setRowCount(0);
  lbl_count_->setText("记录数: 0");
  emit clearRequested();
}

} // namespace shiploader_monitor
