cmake_minimum_required(VERSION 3.8)
project(task_10_3)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. 寻找依赖包（你的代码用到了这些库，必须在这里声明）
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)

# 2. 编译 pub_node（安全处理节点）
add_executable(pub_node src/pub_node.cpp)
ament_target_dependencies(pub_node rclcpp geometry_msgs std_msgs)

# 3. 编译 sub_node（状态监控节点）
add_executable(sub_node src/sub_node.cpp)
ament_target_dependencies(sub_node rclcpp geometry_msgs std_msgs)

# 4. 安装可执行文件（让 ros2 run 能找到节点）
install(TARGETS
  pub_node
  sub_node
  DESTINATION lib/${PROJECT_NAME}
)

# 5. 安装 launch 文件夹（让 ros2 launch 能找到你的启动文件）
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)

# 6. 固定结尾，不能漏
ament_package()