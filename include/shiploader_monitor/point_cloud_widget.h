#ifndef SHIPLOADER_MONITOR__POINT_CLOUD_WIDGET_H_
#define SHIPLOADER_MONITOR__POINT_CLOUD_WIDGET_H_

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QPoint>
#include <QMutex>
#include <QCheckBox>
#include <QHBoxLayout>
#include <vector>
#include <string>
#include <map>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

namespace shiploader_monitor {

struct CloudLayer {
  std::string topic;
  bool visible{true};
  QVector3D color{1,1,1};
  std::vector<float> points;     // x,y,z 交错（点云）
  std::vector<float> lines;      // x,y,z 交错（线段）
  size_t point_count{0};
  size_t line_count{0};
  bool dirty{false};
  bool single_marker{false};     // 是否为单 Marker 累积模式
  std::map<std::string, visualization_msgs::msg::Marker> marker_cache;  // key = "ns_id"
  std::map<std::string, int64_t> marker_times;     // 最后更新时间戳(ns)
};

/// @brief OpenGL 点云渲染 Widget，支持多话题合并显示
class PointCloudWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit PointCloudWidget(QWidget* parent = nullptr);
  ~PointCloudWidget() override;

  /// 注册一个话题（自动创建 checkbox，可指定界面显示名）
  void registerTopic(const std::string& topic, const QVector3D& color,
                     const QString& display_name = QString());

public slots:
  /// 接收点云数据
  void updatePointCloud(const QString& topic, const sensor_msgs::msg::PointCloud2::SharedPtr& msg);
  void updateMarkerArray(const QString& topic, const visualization_msgs::msg::MarkerArray::SharedPtr& msg);
  void updateMarker(const QString& topic, const visualization_msgs::msg::Marker::SharedPtr& msg);
  /// 清空所有点云
  void clearAll();

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;

private:
  void updateVBOs();

  // ── 图层管理 ─────────────────────────────────────────────────
  QMutex cloud_mutex_;
  std::map<std::string, CloudLayer> layers_;

  // ── 相机控制 ─────────────────────────────────────────────────
  QMatrix4x4 view_matrix_;
  QMatrix4x4 proj_matrix_;
  QVector3D camera_up_{0, 0, 1};
  float zoom_{1.0f};
  float rotate_x_{30.0f};
  float rotate_z_{-45.0f};
  QVector3D pan_offset_{0, 0, 0};

  // ── 鼠标交互 ─────────────────────────────────────────────────
  QPoint last_mouse_pos_;
  bool dragging_{false};

  // ── OpenGL 资源 ──────────────────────────────────────────────
  GLuint vbo_{0};
  GLuint line_vbo_{0};

  // ── 话题选择 UI ──────────────────────────────────────────────
  QHBoxLayout* checkbox_bar_;
  std::map<std::string, QCheckBox*> checkboxes_;
};

} // namespace shiploader_monitor

#endif // SHIPLOADER_MONITOR__POINT_CLOUD_WIDGET_H_
