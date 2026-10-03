#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "std_msgs/msg/string.hpp"

class MonitorNode : public rclcpp::Node {
public:
    MonitorNode() : Node("monitor_node") {
        // 订阅安全处理后的指令
        sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel_safe", 10, std::bind(&MonitorNode::cmd_callback, this, std::placeholders::_1));

        // 发布状态
        pub_ = this->create_publisher<std_msgs::msg::String>("/robot_status", 10);

        RCLCPP_INFO(this->get_logger(), "监控节点已启动！");
    }

private:
    void cmd_callback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        std_msgs::msg::String status_msg;
        status_msg.data = "安全速度 - 线速度: " + std::to_string(msg->linear.x) + 
                          ", 角速度: " + std::to_string(msg->angular.z);
        pub_->publish(status_msg);
    }

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MonitorNode>());
    rclcpp::shutdown();
    return 0;
}