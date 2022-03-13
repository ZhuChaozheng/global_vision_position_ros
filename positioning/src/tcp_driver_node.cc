/*
 * @Author: your name
 * @Date: 2022-02-17 11:12:19
 * @LastEditTime: 2022-02-25 15:37:32
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置:
 * https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: /amov_ws/src/global_vision_position_ros/src/tcp_driver_node.cc
 */
#include <ctype.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <tf/transform_listener.h>

#include <cmath>
#include <opencv2/plot.hpp>
#include <sstream>

#include "car.h"
#include "file_operation.h"
#include "opencv2/imgproc.hpp"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "tcp.h"

extern car car_[11];

void *server_thread(void *arg) {
  create_server_and_update_data();  // update struct car[10]
}

void combine_buff(unsigned char buff[], int size, float linear_velocity,
                  float angular_velocity) {
  int buff_linear = int(linear_velocity * 1000);
  int buff_angular = int(angular_velocity * 1000);
  buff[3] = buff_linear >> 8;
  buff[4] = buff_linear;
  buff[7] = buff_angular >> 8;
  buff[8] = buff_angular;
  buff[9] = check_num(buff, 9);  // generate check num
}

/**
 * receive linear_velocity and angular_velocity and packet them to buf,
 * then write the corresponding sockfd.
 */
void vel_command_callback(const geometry_msgs::TwistConstPtr &msg, int marker) {
  float linear_velocity = static_cast<float>(msg->linear.x);
  float angular_velocity = (static_cast<float>(msg->angular.z));
  unsigned char buff[11];
  buff[0] = 123;  // 7B
  buff[1] = 122;  // 7A
  buff[2] = marker;
  buff[5] = 0;
  buff[6] = 0;
  buff[10] = 125;  // 7D
  combine_buff(buff, 11, linear_velocity, angular_velocity);
  write_func(marker, buff, sizeof(buff));
}

/**
 * convert the car status to /comm topic
 */
int main(int argc, char **argv) {
  // sub-thread for creating server
  pthread_t pid;
  pthread_create(&pid, NULL, server_thread, NULL);
  // ros main thread
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<std_msgs::String>("comm", 1000);
  ros::Rate loop_rate(20);

  // extend the new interface
  vector<ros::Subscriber> cmd_vel_set_;
  for (int i = 1; i <= 10; i++) {
    string front_str = "/robot_";
    string end_str = "/cmd_vel";
    stringstream ss;
    ss << front_str << i << end_str;
    string topic = ss.str();
    ros::Subscriber sub = n.subscribe<geometry_msgs::Twist>(
        topic, 30, boost::bind(&vel_command_callback, _1, i));
    cmd_vel_set_.push_back(sub);
  }
  while (ros::ok()) {
    std_msgs::String msg;
    std::stringstream ss;
    for (int i = 1; i <= 10; i++)
      ss << car_[i].connfd // 0
         << " " << car_[i].velocity 
         << " " << car_[i].angular_velocity // 2
         << " " << car_[i].left_wheel_velocity
         << " " << car_[i].right_wheel_velocity  // 4
         << " " << car_[i].acc_x
         << " " << car_[i].acc_y
         << " " << car_[i].gyro_z;
    msg.data = ss.str();
    // ROS_INFO("%s", msg.data.c_str());
    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
