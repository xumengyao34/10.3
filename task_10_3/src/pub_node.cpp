#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>
#include <cmath> // std::isnan 和 std::isinf 判断

using namespace std::chrono_literals;

class SafetyNode : public rclcpp::Node {
public:
    SafetyNode() : Node("safety_node") {
        // 1. 声明参数
        this->declare_parameter("max_linear_speed", 1.0);
        this->declare_parameter("max_angular_speed", 1.5);
        this->declare_parameter("timeout_sec", 1.0);
        
        max_linear_ = this->get_parameter("max_linear_speed").as_double();
        max_angular_ = this->get_parameter("max_angular_speed").as_double();
        timeout_sec_ = this->get_parameter("timeout_sec").as_double();

        last_cmd_time_ = this->now();

        // 2. 订阅原始速度指令
        sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel", 10, std::bind(&SafetyNode::cmd_callback, this, std::placeholders::_1));

        // 3. 发布安全速度指令
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel_safe", 10);

        // 4. 定时器：检查是否超时
        timer_ = this->create_wall_timer(
            100ms, std::bind(&SafetyNode::check_timeout, this));

        RCLCPP_INFO(this->get_logger(), "安全节点已启动！");
    }

private:
    void cmd_callback(const geometry_msgs::msg::Twist::SharedPtr msg) {
        last_cmd_time_ = this->now();
        geometry_msgs::msg::Twist safe_msg = *msg;

        // 【新增】拦截 NaN 异常（非数值）
        if (std::isnan(safe_msg.linear.x)) { RCLCPP_WARN(this->get_logger(), "NaN出现! linear.x 强制归零"); safe_msg.linear.x = 0.0; }
        if (std::isnan(safe_msg.linear.y)) { safe_msg.linear.y = 0.0; }
        if (std::isnan(safe_msg.linear.z)) { safe_msg.linear.z = 0.0; }
        if (std::isnan(safe_msg.angular.x)) { safe_msg.angular.x = 0.0; }
        if (std::isnan(safe_msg.angular.y)) { safe_msg.angular.y = 0.0; }
        if (std::isnan(safe_msg.angular.z)) { RCLCPP_WARN(this->get_logger(), "NaN出现! angular.z 强制归零"); safe_msg.angular.z = 0.0; }

        // 【新增】拦截 inf 异常（无穷大）
        if (std::isinf(safe_msg.linear.x)) { RCLCPP_WARN(this->get_logger(), "inf出现! linear.x 强制归零"); safe_msg.linear.x = 0.0; }
        if (std::isinf(safe_msg.linear.y)) { safe_msg.linear.y = 0.0; }
        if (std::isinf(safe_msg.linear.z)) { safe_msg.linear.z = 0.0; }
        if (std::isinf(safe_msg.angular.x)) { safe_msg.angular.x = 0.0; }
        if (std::isinf(safe_msg.angular.y)) { safe_msg.angular.y = 0.0; }
        if (std::isinf(safe_msg.angular.z)) { RCLCPP_WARN(this->get_logger(), "inf出现! angular.z 强制归零"); safe_msg.angular.z = 0.0; }

        // 限速逻辑（注意判断负方向）
        if (safe_msg.linear.x > max_linear_) safe_msg.linear.x = max_linear_;
        if (safe_msg.linear.x < -max_linear_) safe_msg.linear.x = -max_linear_;
        
        if (safe_msg.angular.z > max_angular_) safe_msg.angular.z = max_angular_;
        if (safe_msg.angular.z < -max_angular_) safe_msg.angular.z = -max_angular_;

       safe_msg.linear.x = std::round(safe_msg.linear.x * 100.0) / 100.0;
        pub_->publish(safe_msg);
    }

    void check_timeout() {
        auto now = this->now();
        if ((now - last_cmd_time_).seconds() > timeout_sec_) {
            geometry_msgs::msg::Twist stop_msg;
            stop_msg.linear.x = 0.0;
            stop_msg.angular.z = 0.0;
            pub_->publish(stop_msg);
            
            last_cmd_time_ = now; 
        }
    }

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    double max_linear_, max_angular_, timeout_sec_;
    rclcpp::Time last_cmd_time_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SafetyNode>());
    rclcpp::shutdown();
    return 0;
}
