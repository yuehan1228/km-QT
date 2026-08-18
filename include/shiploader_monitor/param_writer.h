#ifndef SHIPLOADER_MONITOR__PARAM_WRITER_H_
#define SHIPLOADER_MONITOR__PARAM_WRITER_H_

#include <QObject>
#include <rclcpp/rclcpp.hpp>
#include <memory>
#include <string>
#include <map>
#include <vector>

namespace shiploader_monitor {

/// @brief ROS2 参数客户端，适配 km-shiploader 的运行节点
class ParamWriter : public QObject
{
  Q_OBJECT

public:
  explicit ParamWriter(QObject* parent = nullptr);
  ~ParamWriter() override;

  /// 初始化，创建参数客户端
  void init(rclcpp::Node::SharedPtr node);

  // ── 写入 ──────────────────────────────────────────────────────

  /// 向 motion_node 写入一个 double 参数
  void setMotionDouble(const std::string& name, double value);
  /// 向 motion_node 写入一个 int 参数
  void setMotionInt(const std::string& name, int value);
  /// 向 motion_node 写入一个 bool 参数
  void setMotionBool(const std::string& name, bool value);

  /// 向 plc_interface_node 写入一个 double 参数
  void setPlcSimDouble(const std::string& name, double value);
  /// 向 plc_interface_node 写入一个 int 参数
  void setPlcSimInt(const std::string& name, int value);
  /// 向 plc_interface_node 写入一个 bool 参数
  void setPlcSimBool(const std::string& name, bool value);

  /// 向 perception_node 写入一个 double 参数
  void setPerceptionDouble(const std::string& name, double value);
  /// 向 material_distribution_node 写入一个 double 参数
  void setMaterialDouble(const std::string& name, double value);

  // ── 读取（异步，结果通过信号返回） ────────────────────────────

  /// 获取 motion_node 的指定参数
  void fetchMotionParams(const std::vector<std::string>& names);
  /// 获取 plc_interface_node 的指定参数
  void fetchPlcSimParams(const std::vector<std::string>& names);
  /// 获取 perception_node 的指定参数
  void fetchPerceptionParams(const std::vector<std::string>& names);
  /// 获取 material_distribution_node 的指定参数
  void fetchMaterialParams(const std::vector<std::string>& names);

signals:
  /// 参数写入结果
  void paramSetResult(const QString& target, const QString& name, bool success);

  /// motion_node 参数获取完成，返回 name→value(double) 的映射
  void motionParamsFetched(const QMap<QString, double>& doubles,
                           const QMap<QString, int>& ints,
                           const QMap<QString, bool>& bools);

  /// plc_interface_node 参数获取完成
  void plcSimParamsFetched(const QMap<QString, double>& doubles,
                           const QMap<QString, int>& ints,
                           const QMap<QString, bool>& bools);

  /// perception_node 参数获取完成
  void perceptionParamsFetched(const QMap<QString, double>& doubles,
                               const QMap<QString, int>& ints,
                               const QMap<QString, bool>& bools);

  /// material_distribution_node 参数获取完成
  void materialParamsFetched(const QMap<QString, double>& doubles,
                             const QMap<QString, int>& ints,
                             const QMap<QString, bool>& bools);

private:
  rclcpp::Node::SharedPtr node_;

  std::shared_ptr<rclcpp::AsyncParametersClient> motion_client_;
  std::shared_ptr<rclcpp::AsyncParametersClient> plc_interface_client_;
  std::shared_ptr<rclcpp::AsyncParametersClient> perception_client_;
  std::shared_ptr<rclcpp::AsyncParametersClient> material_client_;
  rclcpp::TimerBase::SharedPtr readiness_timer_;
  bool clients_ready_{false};
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__PARAM_WRITER_H_
