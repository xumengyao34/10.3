一、系统结构
如图片。<img width="3454" height="776" alt="0741aee9799f3ba3f2ae80f9f4c4e853" src="https://github.com/user-attachments/assets/2ff6bc2a-512e-45f9-ab43-295a9ab8d40d" />

二、节点与话题，关键参数
节点1（safety_node)订阅速度指令，拦截nan和inf，并将最大线速度限制为1.0，最大角速度限制为1.5，通信超时时间限制为1秒（三个关键参数)。通过话题输出给下一个节点。
节点2（monitor_node ）订阅话题（/cmd_vel_safe）并输出。
三、编译与运行
终端一.
colcon build --packages-select task_10_3
source install/setup.bash
终端二
cd ~/Downloads/cmd_vel
ros2 bag play cmd_vel_0.db3 --loop
终端三
source install/setup.bash
ros2 topic echo /cmd_vel_safe 以验证速度限制
四.代码均为AI编写，大致框架由我构建。
五.视频如下（当时测试时把线速度最大设置成0.1了，所以和前面说的不太一样）
https://applink.feishu.cn/client/message/link/open?token=AmrBDPrIwFE4asEaru3AET8%3D
