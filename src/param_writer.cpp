#include "shiploader_monitor/param_writer.h"
#include <QMap>

namespace shiploader_monitor {

ParamWriter::ParamWriter(QObject* parent)
  : QObject(parent)
{
}

ParamWriter::~ParamWriter() = default;

void ParamWriter::init(rclcpp::Node::SharedPtr node)
{
  node_ = node;

  // 创建 km-shiploader 运行节点的参数客户端
  motion_client_ = std::make_shared<rclcpp::AsyncParametersClient>(
    node_, "/motion_node");
  plc_interface_client_ = std::make_shared<rclcpp::AsyncParametersClient>(
    node_, "/plc_interface_node");
  perception_client_ = std::make_shared<rclcpp::AsyncParametersClient>(
    node_, "/perception_node");
  material_client_ = std::make_shared<rclcpp::AsyncParametersClient>(
    node_, "/material_distribution_node");

  // 用 ros timer 延迟检查服务可用性（等 executor 跑起来）
  // 不能用 wait_for_service 阻塞，因为 init 在 spin_once 之前执行
  readiness_timer_ = node_->create_wall_timer(
    std::chrono::seconds(1),
    [this]() {
      bool motion_ready = motion_client_->service_is_ready();
      bool plc_ready = plc_interface_client_->service_is_ready();
      bool perception_ready = perception_client_->service_is_ready();
      bool material_ready = material_client_->service_is_ready();
      clients_ready_ = motion_ready && plc_ready && perception_ready && material_ready;
      if (clients_ready_) {
        RCLCPP_INFO(node_->get_logger(), "All km-shiploader parameter services are ready");
        readiness_timer_->cancel();
      }
    });
}

// ── motion_node 写入 ─────────────────────────────────────────────────

void ParamWriter::setMotionDouble(const std::string& name, double value)
{
  if (!motion_client_) return;
  motion_client_->set_parameters(
    {rclcpp::Parameter(name, value)},
    [this, name](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> f) {
      auto results = f.get();
      bool ok = !results.empty() && results[0].successful;
      emit paramSetResult("motion_node", QString::fromStdString(name), ok);
    });
}

void ParamWriter::setMotionInt(const std::string& name, int value)
{
  if (!motion_client_) return;
  motion_client_->set_parameters(
    {rclcpp::Parameter(name, value)},
    [this, name](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> f) {
      auto results = f.get();
      bool ok = !results.empty() && results[0].successful;
      emit paramSetResult("motion_node", QString::fromStdString(name), ok);
    });
}

void ParamWriter::setMotionBool(const std::string& name, bool value)
{
  if (!motion_client_) return;
  motion_client_->set_parameters(
    {rclcpp::Parameter(name, value)},
    [this, name](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> f) {
      auto results = f.get();
      bool ok = !results.empty() && results[0].successful;
      emit paramSetResult("motion_node", QString::fromStdString(name), ok);
    });
}

// ── plc_interface_node 写入 ──────────────────────────────────────────

void ParamWriter::setPlcSimDouble(const std::string& name, double value)
{
  if (!plc_interface_client_) return;
  plc_interface_client_->set_parameters(
    {rclcpp::Parameter(name, value)},
    [this, name](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> f) {
      auto results = f.get();
      bool ok = !results.empty() && results[0].successful;
      emit paramSetResult("plc_interface_node", QString::fromStdString(name), ok);
    });
}

void ParamWriter::setPlcSimInt(const std::string& name, int value)
{
  if (!plc_interface_client_) return;
  plc_interface_client_->set_parameters(
    {rclcpp::Parameter(name, value)},
    [this, name](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> f) {
      auto results = f.get();
      bool ok = !results.empty() && results[0].successful;
      emit paramSetResult("plc_interface_node", QString::fromStdString(name), ok);
    });
}

void ParamWriter::setPlcSimBool(const std::string& name, bool value)
{
  if (!plc_interface_client_) return;
  plc_interface_client_->set_parameters(
    {rclcpp::Parameter(name, value)},
    [this, name](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> f) {
      auto results = f.get();
      bool ok = !results.empty() && results[0].successful;
      emit paramSetResult("plc_interface_node", QString::fromStdString(name), ok);
    });
}

// ── perception_node 写入 ───────────────────────────────────────────

void ParamWriter::setPerceptionDouble(const std::string& name, double value)
{
  if (!perception_client_) return;
  perception_client_->set_parameters(
    {rclcpp::Parameter(name, value)},
    [this, name](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> f) {
      auto results = f.get();
      bool ok = !results.empty() && results[0].successful;
      emit paramSetResult("perception_node", QString::fromStdString(name), ok);
    });
}

// ── material_distribution_node 写入 ─────────────────────────────────

void ParamWriter::setMaterialDouble(const std::string& name, double value)
{
  if (!material_client_) return;
  // 名称映射：UI 用 "material_height_diff" 避免和 perception_node 的 "height_diff" 冲突
  std::string real_name = (name == "material_height_diff") ? "height_diff" : name;
  material_client_->set_parameters(
    {rclcpp::Parameter(real_name, value)},
    [this, name](std::shared_future<std::vector<rcl_interfaces::msg::SetParametersResult>> f) {
      auto results = f.get();
      bool ok = !results.empty() && results[0].successful;
      emit paramSetResult("material_distribution_node", QString::fromStdString(name), ok);
    });
}

void ParamWriter::fetchMotionParams(const std::vector<std::string>& names)
{
  if (!motion_client_ || names.empty()) return;
  RCLCPP_INFO(node_->get_logger(), "fetchMotionParams: requesting %zu parameters", names.size());
  motion_client_->get_parameters(
    names,
    [this](std::shared_future<std::vector<rclcpp::Parameter>> f) {
      QMap<QString, double> doubles;
      QMap<QString, int> ints;
      QMap<QString, bool> bools;

      try {
        auto params = f.get();
        RCLCPP_INFO(node_->get_logger(), "fetchMotionParams: got %zu parameters back", params.size());
        for (const auto& p : params) {
          QString name = QString::fromStdString(p.get_name());
          switch (p.get_type()) {
            case rclcpp::ParameterType::PARAMETER_DOUBLE:
              doubles[name] = p.as_double();
              break;
            case rclcpp::ParameterType::PARAMETER_INTEGER:
              ints[name] = p.as_int();
              break;
            case rclcpp::ParameterType::PARAMETER_BOOL:
              bools[name] = p.as_bool();
              break;
            default:
              break;
          }
        }
      } catch (const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "fetchMotionParams failed: %s", e.what());
      }
      emit motionParamsFetched(doubles, ints, bools);
    });
}

void ParamWriter::fetchPlcSimParams(const std::vector<std::string>& names)
{
  if (!plc_interface_client_ || names.empty()) return;
  plc_interface_client_->get_parameters(
    names,
    [this](std::shared_future<std::vector<rclcpp::Parameter>> f) {
      QMap<QString, double> doubles;
      QMap<QString, int> ints;
      QMap<QString, bool> bools;

      try {
        auto params = f.get();
        for (const auto& p : params) {
          QString name = QString::fromStdString(p.get_name());
          switch (p.get_type()) {
            case rclcpp::ParameterType::PARAMETER_DOUBLE:
              doubles[name] = p.as_double();
              break;
            case rclcpp::ParameterType::PARAMETER_INTEGER:
              ints[name] = p.as_int();
              break;
            case rclcpp::ParameterType::PARAMETER_BOOL:
              bools[name] = p.as_bool();
              break;
            default:
              break;
          }
        }
      } catch (const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "fetchPlcSimParams failed: %s", e.what());
      }
      emit plcSimParamsFetched(doubles, ints, bools);
    });
}

// ── perception_node 读取 ─────────────────────────────────────────────

void ParamWriter::fetchPerceptionParams(const std::vector<std::string>& names)
{
  if (!perception_client_ || names.empty()) return;
  perception_client_->get_parameters(
    names,
    [this](std::shared_future<std::vector<rclcpp::Parameter>> f) {
      QMap<QString, double> doubles;
      QMap<QString, int> ints;
      QMap<QString, bool> bools;
      try {
        auto params = f.get();
        for (const auto& p : params) {
          QString name = QString::fromStdString(p.get_name());
          switch (p.get_type()) {
            case rclcpp::ParameterType::PARAMETER_DOUBLE: doubles[name] = p.as_double(); break;
            case rclcpp::ParameterType::PARAMETER_INTEGER: ints[name] = p.as_int(); break;
            case rclcpp::ParameterType::PARAMETER_BOOL: bools[name] = p.as_bool(); break;
            default: break;
          }
        }
      } catch (const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "fetchPerceptionParams failed: %s", e.what());
      }
      emit perceptionParamsFetched(doubles, ints, bools);
    });
}

// ── material_distribution_node 读取 ──────────────────────────────────

void ParamWriter::fetchMaterialParams(const std::vector<std::string>& names)
{
  if (!material_client_ || names.empty()) return;
  std::vector<std::string> real_names;
  real_names.reserve(names.size());
  for (const auto& name : names) {
    real_names.push_back(name == "material_height_diff" ? "height_diff" : name);
  }
  material_client_->get_parameters(
    real_names,
    [this](std::shared_future<std::vector<rclcpp::Parameter>> f) {
      QMap<QString, double> doubles;
      QMap<QString, int> ints;
      QMap<QString, bool> bools;
      try {
        auto params = f.get();
        for (const auto& p : params) {
          QString name = QString::fromStdString(p.get_name());
          if (name == "height_diff") {
            name = "material_height_diff";
          }
          switch (p.get_type()) {
            case rclcpp::ParameterType::PARAMETER_DOUBLE: doubles[name] = p.as_double(); break;
            case rclcpp::ParameterType::PARAMETER_INTEGER: ints[name] = p.as_int(); break;
            case rclcpp::ParameterType::PARAMETER_BOOL: bools[name] = p.as_bool(); break;
            default: break;
          }
        }
      } catch (const std::exception& e) {
        RCLCPP_ERROR(node_->get_logger(), "fetchMaterialParams failed: %s", e.what());
      }
      emit materialParamsFetched(doubles, ints, bools);
    });
}

} // namespace shiploader_monitor
