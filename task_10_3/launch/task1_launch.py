from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='task_10_3',
            executable='pub_node',
            name='safety_node',
            parameters=[{
                'max_linear_speed': 1.0,
                'max_angular_speed': 1.5,
                'timeout_sec': 1.0
            }]
        ),
        Node(
            package='task_10_3',
            executable='sub_node',
            name='monitor_node'
        )
    ])
