#ifndef SHIPLOADER_MONITOR__LOG_WIDGET_H_
#define SHIPLOADER_MONITOR__LOG_WIDGET_H_

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <deque>
#include "shiploader_monitor/data_store.h"

namespace shiploader_monitor {

/// @brief 日志面板 —— 历史数据表格显示与导出
class LogWidget : public QWidget
{
  Q_OBJECT

public:
  explicit LogWidget(QWidget* parent = nullptr);
  ~LogWidget() override;

public slots:
  void refreshHistory(const std::deque<DataEntry>& history);
  void clear();

private slots:
  void onExportClicked();
  void onClearClicked();

signals:
  void exportRequested(const QString& filepath);
  void clearRequested();

private:
  void setupUI();

  QTableWidget* table_;
  QPushButton* btn_export_;
  QPushButton* btn_clear_;
  QLabel* lbl_count_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__LOG_WIDGET_H_
