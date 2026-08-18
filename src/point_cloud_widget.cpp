#include "shiploader_monitor/point_cloud_widget.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <rclcpp/rclcpp.hpp>
#include <QVBoxLayout>
#include <cstring>
#include <cmath>
#include <algorithm>

namespace shiploader_monitor {

PointCloudWidget::PointCloudWidget(QWidget* parent)
  : QOpenGLWidget(parent)
{
  setMinimumSize(400, 300);
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);

  QSurfaceFormat fmt;
  fmt.setDepthBufferSize(24);
  setFormat(fmt);

  // 话题选择栏
  QVBoxLayout* wrapper = new QVBoxLayout(this);
  wrapper->setContentsMargins(0, 0, 0, 0);
  checkbox_bar_ = new QHBoxLayout();
  checkbox_bar_->setContentsMargins(4, 2, 4, 2);
  wrapper->addLayout(checkbox_bar_);
  wrapper->addStretch();  // OpenGL 区域填满剩余空间

  // OpenGL widget 需要占满
  setLayout(wrapper);
}

PointCloudWidget::~PointCloudWidget()
{
  makeCurrent();
  if (vbo_) glDeleteBuffers(1, &vbo_);
  if (line_vbo_) glDeleteBuffers(1, &line_vbo_);
  doneCurrent();
}

void PointCloudWidget::registerTopic(const std::string& topic, const QVector3D& color,
                                     const QString& display_name)
{
  CloudLayer layer;
  layer.topic = topic;
  layer.color = color;
  layers_[topic] = layer;

  // 创建 checkbox
  QString label = display_name;
  if (label.isEmpty()) {
    label = QString::fromStdString(topic);
    // 取 topic 最后一段作为显示名
    int lastSlash = label.lastIndexOf('/');
    if (lastSlash >= 0) label = label.mid(lastSlash + 1);
    if (label.startsWith("rviz_")) label.remove(0, 5);
  }

  QCheckBox* cb = new QCheckBox(label, this);
  cb->setChecked(true);
  QString style = QString("color: rgb(%1,%2,%3); font-weight: bold;")
    .arg(int(color.x()*255)).arg(int(color.y()*255)).arg(int(color.z()*255));
  cb->setStyleSheet(style);
  checkbox_bar_->addWidget(cb);
  checkboxes_[topic] = cb;

  connect(cb, &QCheckBox::toggled, this, [this, topic](bool checked) {
    QMutexLocker lock(&cloud_mutex_);
    auto it = layers_.find(topic);
    if (it != layers_.end()) {
      it->second.visible = checked;
    }
    update();
  });
}

void PointCloudWidget::initializeGL()
{
  initializeOpenGLFunctions();
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glPointSize(3.0f);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &line_vbo_);
}

void PointCloudWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);
  proj_matrix_.setToIdentity();
  proj_matrix_.perspective(45.0f, float(w) / std::max(h, 1), 0.1f, 500.0f);
}

void PointCloudWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 计算全局包围盒
  QVector3D gmin(1e9, 1e9, 1e9), gmax(-1e9, -1e9, -1e9);
  bool hasData = false;
  {
    QMutexLocker lock(&cloud_mutex_);
    for (auto& kv : layers_) {
      auto& layer = kv.second;
      if (!layer.visible || (layer.point_count == 0 && layer.line_count == 0)) continue;
      hasData = true;
      for (size_t i = 0; i < layer.point_count; ++i) {
        float x = layer.points[i * 3 + 0];
        float y = layer.points[i * 3 + 1];
        float z = layer.points[i * 3 + 2];
        gmin.setX(std::min(gmin.x(), x)); gmin.setY(std::min(gmin.y(), y)); gmin.setZ(std::min(gmin.z(), z));
        gmax.setX(std::max(gmax.x(), x)); gmax.setY(std::max(gmax.y(), y)); gmax.setZ(std::max(gmax.z(), z));
      }
      for (size_t i = 0; i < layer.line_count; ++i) {
        float x = layer.lines[i * 3 + 0];
        float y = layer.lines[i * 3 + 1];
        float z = layer.lines[i * 3 + 2];
        gmin.setX(std::min(gmin.x(), x)); gmin.setY(std::min(gmin.y(), y)); gmin.setZ(std::min(gmin.z(), z));
        gmax.setX(std::max(gmax.x(), x)); gmax.setY(std::max(gmax.y(), y)); gmax.setZ(std::max(gmax.z(), z));
      }
    }
  }

  if (!hasData) {
    // 纯 OpenGL 坐标轴
    float s = 2.0f / zoom_;
    float rx = rotate_x_ * M_PI / 180.0f;
    float rz = rotate_z_ * M_PI / 180.0f;

    QVector3D right(cosf(rz), sinf(rz), 0);
    QVector3D up(-sinf(rx) * sinf(rz), sinf(rx) * cosf(rz), cosf(rx));
    QVector3D pan3D = right * pan_offset_.x() + up * pan_offset_.y();

    QMatrix4x4 view;
    QVector3D lookDirEmpty(cosf(rz) * cosf(rx), sinf(rz) * cosf(rx), sinf(rx));
    QVector3D camRightE = QVector3D::crossProduct(lookDirEmpty, camera_up_).normalized();
    QVector3D camUpE = QVector3D::crossProduct(camRightE, lookDirEmpty).normalized();
    QVector3D pan3D_empty = camRightE * pan_offset_.x() + camUpE * pan_offset_.y();
    QVector3D eye(s * cosf(rz) * cosf(rx) + pan3D_empty.x(),
                  s * sinf(rz) * cosf(rx) + pan3D_empty.y(),
                  s * sinf(rx) + pan3D_empty.z());
    view.lookAt(eye, pan3D_empty, camera_up_);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = float(width()) / std::max(height(), 1);
    glOrtho(-aspect * s, aspect * s, -s, s, -100, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(view.constData());

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(1,0,0);
    glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,1,0);
    glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,1);
    glColor3f(0.3f, 0.3f, 0.3f);
    for (int i = -5; i <= 5; ++i) {
      glVertex3f(i, -5, 0); glVertex3f(i, 5, 0);
      glVertex3f(-5, i, 0); glVertex3f(5, i, 0);
    }
    glEnd();
    glLineWidth(1.0f);
    return;
  }

  // 相机计算
  QVector3D center = (gmin + gmax) * 0.5f;
  float worldSize = (gmax - gmin).length();
  float dist = worldSize / zoom_;
  float rx = rotate_x_ * M_PI / 180.0f;
  float rz = rotate_z_ * M_PI / 180.0f;

  // 相机右向量和上向量（屏幕空间方向，用于平移）
  // right = camera_up_ × lookDir, up = lookDir × right
  QVector3D lookDir(cosf(rz) * cosf(rx), sinf(rz) * cosf(rx), sinf(rx));
  QVector3D camRight = QVector3D::crossProduct(lookDir, camera_up_).normalized();
  QVector3D camUp = QVector3D::crossProduct(camRight, lookDir).normalized();

  // 平移：沿屏幕右/上方向偏移（不受旋转影响）
  QVector3D pan3D = camRight * pan_offset_.x() + camUp * pan_offset_.y();
  QVector3D shiftedCenter = center + pan3D;

  QVector3D eye(
    shiftedCenter.x() + dist * cosf(rz) * cosf(rx),
    shiftedCenter.y() + dist * sinf(rz) * cosf(rx),
    shiftedCenter.z() + dist * sinf(rx));
  view_matrix_.setToIdentity();
  view_matrix_.lookAt(eye, shiftedCenter, camera_up_);

  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(proj_matrix_.constData());
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(view_matrix_.constData());

  // 分离渲染：点云用 vbo_，线段用 line_vbo_
  QMutexLocker lock(&cloud_mutex_);

  // ── 点云（所有可见图层合并到 vbo_） ──────────────────────────
  size_t total_pts = 0;
  bool pt_dirty = false;
  for (auto& kv : layers_) {
    if (kv.second.visible) {
      total_pts += kv.second.point_count;
      if (kv.second.dirty) pt_dirty = true;
    }
  }

  if (total_pts > 0) {
    if (pt_dirty) {
      std::vector<float> ptVerts, ptColors;
      ptVerts.reserve(total_pts * 3);
      ptColors.reserve(total_pts * 3);
      for (auto& kv : layers_) {
        auto& layer = kv.second;
        if (!layer.visible || layer.point_count == 0) continue;
        float r = layer.color.x(), g = layer.color.y(), b = layer.color.z();
        for (size_t i = 0; i < layer.point_count; ++i) {
          ptVerts.push_back(layer.points[i*3+0]);
          ptVerts.push_back(layer.points[i*3+1]);
          ptVerts.push_back(layer.points[i*3+2]);
          ptColors.push_back(r); ptColors.push_back(g); ptColors.push_back(b);
        }
      }
      std::vector<float> ptInterleaved;
      ptInterleaved.reserve(total_pts * 6);
      for (size_t i = 0; i < total_pts; ++i) {
        ptInterleaved.push_back(ptVerts[i*3+0]); ptInterleaved.push_back(ptVerts[i*3+1]); ptInterleaved.push_back(ptVerts[i*3+2]);
        ptInterleaved.push_back(ptColors[i*3+0]); ptInterleaved.push_back(ptColors[i*3+1]); ptInterleaved.push_back(ptColors[i*3+2]);
      }
      glBindBuffer(GL_ARRAY_BUFFER, vbo_);
      glBufferData(GL_ARRAY_BUFFER, ptInterleaved.size() * sizeof(float), ptInterleaved.data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    glColorPointer(3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glPointSize(1.0f);
    glDrawArrays(GL_POINTS, 0, total_pts);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  }

  // ── 线段（优先渲染常规图层，target/arm_bbox 最后渲染） ──────
  size_t total_lines = 0;
  size_t overlay_lines = 0;  // target/arm_bbox 的顶点数（最后渲染）
  for (auto& kv : layers_) {
    if (!kv.second.visible) continue;
    if (kv.first == "/rviz_target" || kv.first == "/rviz_arm_bbox")
      overlay_lines += kv.second.line_count;
    else
      total_lines += kv.second.line_count;
  }

  auto drawLines = [&](size_t count, size_t startOffset, bool disableDepth) {
    if (count == 0) return;
    if (disableDepth) glDisable(GL_DEPTH_TEST);
    glLineWidth(disableDepth ? 4.0f : 2.0f);
    glDrawArrays(GL_LINES, startOffset, count);
    glLineWidth(1.0f);
    if (disableDepth) glEnable(GL_DEPTH_TEST);
  };

  if (total_lines + overlay_lines > 0) {
    std::vector<float> lnVerts, lnColors;
    lnVerts.reserve((total_lines + overlay_lines) * 3);
    lnColors.reserve((total_lines + overlay_lines) * 3);

    // 先放常规图层，再放 target/arm_bbox
    auto pushLayer = [&](bool overlay) {
      for (auto& kv : layers_) {
        auto& layer = kv.second;
        if (!layer.visible || layer.line_count == 0) continue;
        bool isOverlay = (kv.first == "/rviz_target" || kv.first == "/rviz_arm_bbox");
        if (isOverlay != overlay) continue;
        float r = layer.color.x(), g = layer.color.y(), b = layer.color.z();
        for (size_t i = 0; i < layer.line_count; ++i) {
          lnVerts.push_back(layer.lines[i*3+0]);
          lnVerts.push_back(layer.lines[i*3+1]);
          lnVerts.push_back(layer.lines[i*3+2]);
          lnColors.push_back(r); lnColors.push_back(g); lnColors.push_back(b);
        }
      }
    };
    pushLayer(false); // 常规图层
    pushLayer(true);  // overlay 图层

    std::vector<float> lnInterleaved;
    size_t totalVerts = total_lines + overlay_lines;
    lnInterleaved.reserve(totalVerts * 6);
    for (size_t i = 0; i < totalVerts; ++i) {
      lnInterleaved.push_back(lnVerts[i*3+0]); lnInterleaved.push_back(lnVerts[i*3+1]); lnInterleaved.push_back(lnVerts[i*3+2]);
      lnInterleaved.push_back(lnColors[i*3+0]); lnInterleaved.push_back(lnColors[i*3+1]); lnInterleaved.push_back(lnColors[i*3+2]);
    }
    glBindBuffer(GL_ARRAY_BUFFER, line_vbo_);
    glBufferData(GL_ARRAY_BUFFER, lnInterleaved.size() * sizeof(float), lnInterleaved.data(), GL_DYNAMIC_DRAW);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    glColorPointer(3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    drawLines(total_lines, 0, false);
    drawLines(overlay_lines, total_lines, true);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // 清除各图层 dirty 标志
  for (auto& kv : layers_) kv.second.dirty = false;
}

void PointCloudWidget::updatePointCloud(const QString& topic_q,
                                         const sensor_msgs::msg::PointCloud2::SharedPtr& msg)
{
  if (!msg || msg->data.empty()) return;
  std::string topic = topic_q.toStdString();

  QMutexLocker lock(&cloud_mutex_);
  auto it = layers_.find(topic);
  if (it == layers_.end()) return;
  auto& layer = it->second;

  int x_off = -1, y_off = -1, z_off = -1;
  int point_step = msg->point_step;
  for (size_t i = 0; i < msg->fields.size(); ++i) {
    if (msg->fields[i].name == "x") x_off = msg->fields[i].offset;
    else if (msg->fields[i].name == "y") y_off = msg->fields[i].offset;
    else if (msg->fields[i].name == "z") z_off = msg->fields[i].offset;
  }
  if (x_off < 0 || y_off < 0 || z_off < 0) return;

  size_t new_count = msg->data.size() / point_step;

  // 如果数据没变，跳过（减少闪烁）
  if (new_count == layer.point_count && !msg->data.empty() && layer.points.size() == new_count * 3) {
    // 采样几个点对比
    bool same = true;
    const uint8_t* base = msg->data.data();
    size_t check_count = std::min(new_count, size_t(10));
    for (size_t i = 0; i < check_count && same; ++i) {
      const uint8_t* ptr = base + (i * point_step);
      float x, y, z;
      memcpy(&x, ptr + x_off, sizeof(float));
      memcpy(&y, ptr + y_off, sizeof(float));
      memcpy(&z, ptr + z_off, sizeof(float));
      if (fabsf(x - layer.points[i * 3 + 0]) > 0.001f ||
          fabsf(y - layer.points[i * 3 + 1]) > 0.001f ||
          fabsf(z - layer.points[i * 3 + 2]) > 0.001f) {
        same = false;
      }
    }
    if (same) return;
  }

  layer.point_count = new_count;
  layer.line_count = 0;
  layer.points.resize(new_count * 3);
  layer.lines.clear();
  layer.dirty = true;

  const uint8_t* base = msg->data.data();
  for (size_t i = 0; i < new_count; ++i) {
    const uint8_t* ptr = base + i * point_step;
    memcpy(&layer.points[i * 3 + 0], ptr + x_off, sizeof(float));
    memcpy(&layer.points[i * 3 + 1], ptr + y_off, sizeof(float));
    memcpy(&layer.points[i * 3 + 2], ptr + z_off, sizeof(float));
  }

  update();
}

void PointCloudWidget::updateMarkerArray(const QString& topic_q,
                                          const visualization_msgs::msg::MarkerArray::SharedPtr& msg)
{
  if (!msg) return;
  std::string topic = topic_q.toStdString();

  QMutexLocker lock(&cloud_mutex_);
  auto it = layers_.find(topic);
  if (it == layers_.end()) return;
  auto& layer = it->second;

  // rviz_arm_bbox 交替发布两组 marker，用缓存累积避免互相覆盖
  if (topic == "/rviz_arm_bbox") {
    for (const auto& m : msg->markers) {
      std::string key = m.ns + "_" + std::to_string(m.id);
      layer.marker_cache[key] = m;
    }
    layer.dirty = true;
    layer.points.clear();
    layer.lines.clear();
    layer.point_count = 0;
    layer.line_count = 0;

    for (auto& [id, m] : layer.marker_cache) {
      if (m.type == visualization_msgs::msg::Marker::LINE_LIST && m.points.size() >= 2) {
        layer.lines.resize((layer.line_count + m.points.size()) * 3);
        for (size_t i = 0; i + 1 < m.points.size(); i += 2) {
          layer.lines[layer.line_count*3+0]=m.points[i].x; layer.lines[layer.line_count*3+1]=m.points[i].y; layer.lines[layer.line_count*3+2]=m.points[i].z; layer.line_count++;
          layer.lines[layer.line_count*3+0]=m.points[i+1].x; layer.lines[layer.line_count*3+1]=m.points[i+1].y; layer.lines[layer.line_count*3+2]=m.points[i+1].z; layer.line_count++;
        }
      }
      else if (m.type == visualization_msgs::msg::Marker::LINE_STRIP && m.points.size() >= 2) {
        layer.lines.resize((layer.line_count + (m.points.size()-1)*2) * 3);
        for (size_t i = 1; i < m.points.size(); ++i) {
          layer.lines[layer.line_count*3+0]=m.points[i-1].x; layer.lines[layer.line_count*3+1]=m.points[i-1].y; layer.lines[layer.line_count*3+2]=m.points[i-1].z; layer.line_count++;
          layer.lines[layer.line_count*3+0]=m.points[i].x;   layer.lines[layer.line_count*3+1]=m.points[i].y;   layer.lines[layer.line_count*3+2]=m.points[i].z;   layer.line_count++;
        }
      }
      else if (m.type == visualization_msgs::msg::Marker::SPHERE_LIST) {
        layer.points.resize((layer.point_count + m.points.size()) * 3);
        for (const auto& p : m.points) {
          layer.points[layer.point_count*3+0]=p.x; layer.points[layer.point_count*3+1]=p.y; layer.points[layer.point_count*3+2]=p.z;
          layer.point_count++;
        }
      }
      else if (m.type == visualization_msgs::msg::Marker::CUBE) {
        float hx=m.scale.x*0.5f, hy=m.scale.y*0.5f, hz=m.scale.z*0.5f;
        float c[8][3]={{-hx,-hy,-hz},{hx,-hy,-hz},{hx,hy,-hz},{-hx,hy,-hz},{-hx,-hy,hz},{hx,-hy,hz},{hx,hy,hz},{-hx,hy,hz}};
        float qx=m.pose.orientation.x, qy=m.pose.orientation.y, qz=m.pose.orientation.z, qw=m.pose.orientation.w;
        float xx=qw*qw+qx*qx-qy*qy-qz*qz, yy=qw*qw-qx*qx+qy*qy-qz*qz, zz=qw*qw-qx*qx-qy*qy+qz*qz;
        float xy2=2*(qx*qy+qw*qz), xz2=2*(qx*qz-qw*qy), yz2=2*(qy*qz+qw*qx);
        float yx2=2*(qx*qy-qw*qz), zx2=2*(qx*qz+qw*qy), zy2=2*(qy*qz-qw*qx);
        float v[8][3];
        for(int i=0;i<8;++i){v[i][0]=m.pose.position.x+xx*c[i][0]+xy2*c[i][1]+xz2*c[i][2]; v[i][1]=m.pose.position.y+yx2*c[i][0]+yy*c[i][1]+yz2*c[i][2]; v[i][2]=m.pose.position.z+zx2*c[i][0]+zy2*c[i][1]+zz*c[i][2];}
        int e[12][2]={{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
        layer.lines.resize((layer.line_count+24)*3);
        for(auto& ed:e){layer.lines[layer.line_count*3+0]=v[ed[0]][0];layer.lines[layer.line_count*3+1]=v[ed[0]][1];layer.lines[layer.line_count*3+2]=v[ed[0]][2];layer.line_count++;layer.lines[layer.line_count*3+0]=v[ed[1]][0];layer.lines[layer.line_count*3+1]=v[ed[1]][1];layer.lines[layer.line_count*3+2]=v[ed[1]][2];layer.line_count++;}
      }
    }
    if (layer.line_count > 0 || layer.point_count > 0) update();
    return;
  }

  // 其他 topic：直接替换模式
  layer.lines.clear();
  layer.point_count = 0;
  layer.line_count = 0;
  layer.dirty = true;

  for (const auto& m : msg->markers) {
    // LINE_LIST
    if (m.type == visualization_msgs::msg::Marker::LINE_LIST && m.points.size() >= 2) {
      size_t start = layer.line_count;
      layer.lines.resize((layer.line_count + m.points.size()) * 3);
      for (size_t i = 0; i + 1 < m.points.size(); i += 2) {
        layer.lines[layer.line_count*3+0]=m.points[i].x; layer.lines[layer.line_count*3+1]=m.points[i].y; layer.lines[layer.line_count*3+2]=m.points[i].z; layer.line_count++;
        layer.lines[layer.line_count*3+0]=m.points[i+1].x; layer.lines[layer.line_count*3+1]=m.points[i+1].y; layer.lines[layer.line_count*3+2]=m.points[i+1].z; layer.line_count++;
      }
    }
    // LINE_STRIP
    else if (m.type == visualization_msgs::msg::Marker::LINE_STRIP && m.points.size() >= 2) {
      layer.lines.resize((layer.line_count + (m.points.size()-1)*2) * 3);
      for (size_t i = 1; i < m.points.size(); ++i) {
        layer.lines[layer.line_count*3+0]=m.points[i-1].x; layer.lines[layer.line_count*3+1]=m.points[i-1].y; layer.lines[layer.line_count*3+2]=m.points[i-1].z; layer.line_count++;
        layer.lines[layer.line_count*3+0]=m.points[i].x;   layer.lines[layer.line_count*3+1]=m.points[i].y;   layer.lines[layer.line_count*3+2]=m.points[i].z;   layer.line_count++;
      }
    }
    // CUBE
    else if (m.type == visualization_msgs::msg::Marker::CUBE) {
      float hx=m.scale.x*0.5f, hy=m.scale.y*0.5f, hz=m.scale.z*0.5f;
      float c[8][3]={{-hx,-hy,-hz},{hx,-hy,-hz},{hx,hy,-hz},{-hx,hy,-hz},{-hx,-hy,hz},{hx,-hy,hz},{hx,hy,hz},{-hx,hy,hz}};
      float qx=m.pose.orientation.x, qy=m.pose.orientation.y, qz=m.pose.orientation.z, qw=m.pose.orientation.w;
      float xx=qw*qw+qx*qx-qy*qy-qz*qz, yy=qw*qw-qx*qx+qy*qy-qz*qz, zz=qw*qw-qx*qx-qy*qy+qz*qz;
      float xy2=2*(qx*qy+qw*qz), xz2=2*(qx*qz-qw*qy), yz2=2*(qy*qz+qw*qx);
      float yx2=2*(qx*qy-qw*qz), zx2=2*(qx*qz+qw*qy), zy2=2*(qy*qz-qw*qx);
      float v[8][3];
      for(int i=0;i<8;++i){v[i][0]=m.pose.position.x+xx*c[i][0]+xy2*c[i][1]+xz2*c[i][2]; v[i][1]=m.pose.position.y+yx2*c[i][0]+yy*c[i][1]+yz2*c[i][2]; v[i][2]=m.pose.position.z+zx2*c[i][0]+zy2*c[i][1]+zz*c[i][2];}
      int e[12][2]={{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
      layer.lines.resize((layer.line_count+24)*3);
      for(auto& ed:e){layer.lines[layer.line_count*3+0]=v[ed[0]][0];layer.lines[layer.line_count*3+1]=v[ed[0]][1];layer.lines[layer.line_count*3+2]=v[ed[0]][2];layer.line_count++;layer.lines[layer.line_count*3+0]=v[ed[1]][0];layer.lines[layer.line_count*3+1]=v[ed[1]][1];layer.lines[layer.line_count*3+2]=v[ed[1]][2];layer.line_count++;}
    }
  // CYLINDER: 16边形线框
    else if (m.type == visualization_msgs::msg::Marker::CYLINDER) {
      float radius = fmaxf(m.scale.x, m.scale.y) * 0.5f;
      float hh = m.scale.z * 0.5f;
      float cx = m.pose.position.x, cy = m.pose.position.y, cz = m.pose.position.z;
      if (radius <= 0 || hh <= 0) continue;
      int n = 16;
      int total = n * 2 * 2 + n * 2;  // 顶圆 + 底圆 + 竖边
      layer.lines.resize((layer.line_count + total) * 3);
      auto addLine = [&](float x1,float y1,float z1, float x2,float y2,float z2) {
        layer.lines[layer.line_count*3+0]=x1; layer.lines[layer.line_count*3+1]=y1; layer.lines[layer.line_count*3+2]=z1; layer.line_count++;
        layer.lines[layer.line_count*3+0]=x2; layer.lines[layer.line_count*3+1]=y2; layer.lines[layer.line_count*3+2]=z2; layer.line_count++;
      };
      for (int i = 0; i < n; ++i) {
        int j = (i+1)%n;
        float a1 = 2.0f*M_PI*i/n, a2 = 2.0f*M_PI*j/n;
        float x1=cx+radius*cosf(a1), y1=cy+radius*sinf(a1);
        float x2=cx+radius*cosf(a2), y2=cy+radius*sinf(a2);
        addLine(x1,y1,cz-hh, x2,y2,cz-hh);  // 底面圆
        addLine(x1,y1,cz+hh, x2,y2,cz+hh);  // 顶面圆
        addLine(x1,y1,cz-hh, x1,y1,cz+hh);  // 竖边
      }
    }
    // SPHERE/TEXT的中心点
    else if (m.type == visualization_msgs::msg::Marker::SPHERE || m.type == visualization_msgs::msg::Marker::TEXT_VIEW_FACING) {
      layer.points.resize((layer.point_count+1)*3);
      layer.points[layer.point_count*3+0]=m.pose.position.x; layer.points[layer.point_count*3+1]=m.pose.position.y; layer.points[layer.point_count*3+2]=m.pose.position.z;
      layer.point_count++;
    }
  }

  if (layer.line_count > 0 || layer.point_count > 0) {
    update();
  }
}

void PointCloudWidget::updateMarker(const QString& topic_q,
                                     const visualization_msgs::msg::Marker::SharedPtr& msg)
{
  if (!msg) return;
  std::string topic = topic_q.toStdString();

  QMutexLocker lock(&cloud_mutex_);
  auto it = layers_.find(topic);
  if (it == layers_.end()) return;
  auto& layer = it->second;
  layer.single_marker = true;
  layer.dirty = true;

  auto& cache = layer.marker_cache;
  auto& times = layer.marker_times;
  int64_t stamp_ns = (int64_t)msg->header.stamp.sec * 1000000000LL + msg->header.stamp.nanosec;
  std::string key = msg->ns + "_" + std::to_string(msg->id);
  cache[key] = *msg;
  times[key] = stamp_ns;

  // 清理 2 秒未更新的旧 marker（快速淘汰，因为同一帧内 marker 时间戳几乎相同）
  for (auto it = cache.begin(); it != cache.end(); ) {
    auto ti = times.find(it->first);
    if (ti != times.end() && (stamp_ns - ti->second) > 2000000000LL) {
      times.erase(it->first);
      it = cache.erase(it);
    } else {
      ++it;
    }
  }
  layer.points.clear();
  layer.lines.clear();
  layer.point_count = 0;
  layer.line_count = 0;

  for (auto& [id, m] : cache) {
    // LINE_STRIP (type=4)
    if (m.type == visualization_msgs::msg::Marker::LINE_STRIP && m.points.size() >= 2) {
      size_t start = layer.line_count;
      layer.lines.resize((layer.line_count + (m.points.size() - 1) * 2) * 3);
      for (size_t i = 1; i < m.points.size(); ++i) {
        layer.lines[layer.line_count * 3 + 0] = m.points[i-1].x;
        layer.lines[layer.line_count * 3 + 1] = m.points[i-1].y;
        layer.lines[layer.line_count * 3 + 2] = m.points[i-1].z;
        layer.line_count++;
        layer.lines[layer.line_count * 3 + 0] = m.points[i].x;
        layer.lines[layer.line_count * 3 + 1] = m.points[i].y;
        layer.lines[layer.line_count * 3 + 2] = m.points[i].z;
        layer.line_count++;
      }
    }
  }

  if (layer.line_count > 0 || layer.point_count > 0) {
    update();
  }
}

void PointCloudWidget::clearAll()
{
  QMutexLocker lock(&cloud_mutex_);
  for (auto& kv : layers_) {
    kv.second.points.clear();
    kv.second.point_count = 0;
    kv.second.dirty = true;
  }
  update();
}

// ── 鼠标交互 ─────────────────────────────────────────────────────────

void PointCloudWidget::mousePressEvent(QMouseEvent* event)
{
  last_mouse_pos_ = event->pos();
  dragging_ = true;
  event->accept();
}

void PointCloudWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (!dragging_) return;
  QPoint delta = event->pos() - last_mouse_pos_;
  last_mouse_pos_ = event->pos();

  auto btns = event->buttons();

  if (btns & Qt::LeftButton) {
    rotate_z_ += delta.x() * 0.5f;
    rotate_x_ += delta.y() * 0.5f;
    rotate_x_ = std::max(-89.0f, std::min(89.0f, rotate_x_));
  } else if (btns & Qt::MiddleButton) {
    float pixelToWorld = zoom_ * 0.02f;
    pan_offset_.setX(pan_offset_.x() + delta.x() * pixelToWorld);
    pan_offset_.setY(pan_offset_.y() + delta.y() * pixelToWorld);
  }

  update();
}

void PointCloudWidget::wheelEvent(QWheelEvent* event)
{
  float factor = (event->angleDelta().y() > 0) ? 1.1f : 0.9f;
  zoom_ = std::max(0.1f, std::min(10.0f, zoom_ * factor));
  update();
  event->accept();
}

void PointCloudWidget::mouseReleaseEvent(QMouseEvent* event)
{
  Q_UNUSED(event);
  dragging_ = false;
}

void PointCloudWidget::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Space) {
    pan_offset_ = QVector3D(0, 0, 0);
    rotate_x_ = 30.0f;
    rotate_z_ = -45.0f;
    zoom_ = 1.0f;
    update();
  }
  QOpenGLWidget::keyPressEvent(event);
}

} // namespace shiploader_monitor
