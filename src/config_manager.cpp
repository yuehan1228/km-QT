#include "shiploader_monitor/config_manager.h"

#include <QDir>
#include <QStandardPaths>
#include <fstream>

namespace shiploader_monitor {

ConfigManager::ConfigManager(QObject* parent)
  : QObject(parent)
{
  default_export_path_ = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  loadDefaults();
}

ConfigManager::~ConfigManager() = default;

void ConfigManager::loadDefaults()
{
  config_ = YAML::Node();
  config_["window"]["width"] = default_window_size_.width();
  config_["window"]["height"] = default_window_size_.height();
  config_["window"]["pos_x"] = default_window_pos_.x();
  config_["window"]["pos_y"] = default_window_pos_.y();
  config_["data"]["history_max_size"] = default_history_size_;
  config_["data"]["refresh_interval_ms"] = default_refresh_interval_;
  config_["data"]["export_path"] = default_export_path_.toStdString();

  // 默认启用所有 topic
  YAML::Node topics;
  topics["/plc_status"] = true;
  topics["/shiploader_pos"] = true;
  topics["/heartbeat"] = true;
  topics["/workflow_status"] = true;
  topics["/ship_loader_speed"] = true;
  topics["/ship_loader_target_pos"] = true;
  topics["/hatch_position"] = true;
  topics["/ship_model_point_cloud"] = true;
  topics["/material_point_cloud"] = true;
  topics["/filtered_cloud"] = true;
  topics["/merged_points"] = true;
  config_["topics"] = topics;
}

bool ConfigManager::load(const QString& filepath)
{
  try {
    config_ = YAML::LoadFile(filepath.toStdString());
    emit configChanged();
    return true;
  } catch (const YAML::Exception& e) {
    // 文件不存在或解析失败时使用默认值
    loadDefaults();
    return false;
  }
}

bool ConfigManager::save(const QString& filepath)
{
  try {
    // 确保目录存在
    QDir dir = QFileInfo(filepath).absoluteDir();
    if (!dir.exists() && !dir.mkpath(".")) {
      return false;
    }

    std::ofstream fout(filepath.toStdString());
    if (!fout.is_open()) {
      return false;
    }
    fout << config_;
    if (!fout.good()) {
      return false;
    }
    fout.close();
    emit configChanged();
    return true;
  } catch (const std::exception&) {
    return false;
  }
}

// ── 窗口设置 ─────────────────────────────────────────────────────────

QSize ConfigManager::windowSize() const
{
  return QSize(
    config_["window"]["width"].as<int>(default_window_size_.width()),
    config_["window"]["height"].as<int>(default_window_size_.height()));
}

void ConfigManager::setWindowSize(const QSize& size)
{
  config_["window"]["width"] = size.width();
  config_["window"]["height"] = size.height();
}

QPoint ConfigManager::windowPosition() const
{
  return QPoint(
    config_["window"]["pos_x"].as<int>(default_window_pos_.x()),
    config_["window"]["pos_y"].as<int>(default_window_pos_.y()));
}

void ConfigManager::setWindowPosition(const QPoint& pos)
{
  config_["window"]["pos_x"] = pos.x();
  config_["window"]["pos_y"] = pos.y();
}

// ── 数据设置 ─────────────────────────────────────────────────────────

int ConfigManager::historyMaxSize() const
{
  return config_["data"]["history_max_size"].as<int>(default_history_size_);
}

void ConfigManager::setHistoryMaxSize(int size)
{
  config_["data"]["history_max_size"] = size;
}

int ConfigManager::refreshIntervalMs() const
{
  return config_["data"]["refresh_interval_ms"].as<int>(default_refresh_interval_);
}

void ConfigManager::setRefreshIntervalMs(int ms)
{
  config_["data"]["refresh_interval_ms"] = ms;
}

QString ConfigManager::logExportPath() const
{
  return QString::fromStdString(
    config_["data"]["export_path"].as<std::string>(default_export_path_.toStdString()));
}

void ConfigManager::setLogExportPath(const QString& path)
{
  config_["data"]["export_path"] = path.toStdString();
}

// ── Topic 过滤 ───────────────────────────────────────────────────────

bool ConfigManager::isTopicEnabled(const QString& topic) const
{
  if (config_["topics"][topic.toStdString()]) {
    return config_["topics"][topic.toStdString()].as<bool>();
  }
  return true; // 默认启用
}

void ConfigManager::setTopicEnabled(const QString& topic, bool enabled)
{
  config_["topics"][topic.toStdString()] = enabled;
}

QStringList ConfigManager::enabledTopics() const
{
  QStringList list;
  if (config_["topics"].IsMap()) {
    for (const auto& kv : config_["topics"]) {
      if (kv.second.as<bool>()) {
        list.append(QString::fromStdString(kv.first.as<std::string>()));
      }
    }
  }
  return list;
}

} // namespace shiploader_monitor
