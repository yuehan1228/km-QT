#ifndef SHIPLOADER_MONITOR__CONFIG_MANAGER_H_
#define SHIPLOADER_MONITOR__CONFIG_MANAGER_H_

#include <QObject>
#include <QString>
#include <QSize>
#include <QPoint>
#include <yaml-cpp/yaml.h>

namespace shiploader_monitor {

/// @brief 配置管理器 —— 使用 YAML 读写应用配置
class ConfigManager : public QObject
{
  Q_OBJECT

public:
  explicit ConfigManager(QObject* parent = nullptr);
  ~ConfigManager() override;

  /// 加载配置文件
  bool load(const QString& filepath);
  /// 保存配置文件
  bool save(const QString& filepath);
  /// 加载默认值
  void loadDefaults();

  // ── 窗口设置 ──────────────────────────────────────────────────
  QSize windowSize() const;
  void setWindowSize(const QSize& size);

  QPoint windowPosition() const;
  void setWindowPosition(const QPoint& pos);

  // ── 数据设置 ──────────────────────────────────────────────────
  int historyMaxSize() const;
  void setHistoryMaxSize(int size);

  int refreshIntervalMs() const;
  void setRefreshIntervalMs(int ms);

  QString logExportPath() const;
  void setLogExportPath(const QString& path);

  // ── Topic 过滤设置 ────────────────────────────────────────────
  bool isTopicEnabled(const QString& topic) const;
  void setTopicEnabled(const QString& topic, bool enabled);

  QStringList enabledTopics() const;

signals:
  void configChanged();

private:
  YAML::Node config_;

  // 默认值
  QSize default_window_size_{1280, 800};
  QPoint default_window_pos_{100, 100};
  int default_history_size_{1000};
  int default_refresh_interval_{200};
  QString default_export_path_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__CONFIG_MANAGER_H_
