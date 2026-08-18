cmake_minimum_required(VERSION 3.16)
project(shiploader_monitor)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

# ── QT 自动处理 ─────────────────────────────────────────────────────
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# ── 依赖查找 ────────────────────────────────────────────────────────
find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(km_custom_msgs REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(OpenGL REQUIRED)

# ── UI 文件 ─────────────────────────────────────────────────────────
set(UI_FILES ui/main_window.ui)
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

# ── 源文件 ──────────────────────────────────────────────────────────
set(SOURCES
  src/main.cpp
  src/main_window.cpp
  src/ros_node.cpp
  src/data_store.cpp
  src/config_manager.cpp
  src/param_writer.cpp
  src/plc_status_widget.cpp
  src/status_widget.cpp
  src/heartbeat_widget.cpp
  src/log_widget.cpp
  src/param_edit_widget.cpp
  src/point_cloud_widget.cpp
)

# ── 头文件（MOC 需要扫描包含 Q_OBJECT 的头文件） ──────────────────
set(HEADERS
  include/shiploader_monitor/main_window.h
  include/shiploader_monitor/ros_node.h
  include/shiploader_monitor/data_store.h
  include/shiploader_monitor/config_manager.h
  include/shiploader_monitor/param_writer.h
  include/shiploader_monitor/plc_status_widget.h
  include/shiploader_monitor/status_widget.h
  include/shiploader_monitor/heartbeat_widget.h
  include/shiploader_monitor/log_widget.h
  include/shiploader_monitor/param_edit_widget.h
  include/shiploader_monitor/point_cloud_widget.h
)

# ── 可执行文件 ──────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}
)

ament_target_dependencies(${PROJECT_NAME}
  ament_index_cpp
  rclcpp
  sensor_msgs
  visualization_msgs
  km_custom_msgs
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Widgets
  OpenGL::GL
  ${YAML_CPP_LIBRARIES}
)

# ── 安装 ────────────────────────────────────────────────────────────
install(TARGETS ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

ament_package()
