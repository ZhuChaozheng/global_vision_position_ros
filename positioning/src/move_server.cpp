#include <actionlib/server/simple_action_server.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <stdio.h>
#include <tf/transform_listener.h>

#include <iostream>

#include "car.h"
#include "global_vision_position/MoveAction.h"
#include "ros/ros.h"
using namespace std;

/*
 * define data space
 */
int boid_num = 2;
float *pos_x_array = new float[boid_num];
float *pos_y_array = new float[boid_num];
float *pos_theta_array = new float[boid_num];
bool *flag_array = new bool[boid_num];
/*
 * controller params
 */
bool modi_flag = true;
double _linear_velocity = 0.15;
double _ratio = 0.8;  // 误差比例

double ld = _linear_velocity * _ratio;
double L = 0.14;  //轮子前后距离

typedef actionlib::SimpleActionServer<global_vision_position::MoveAction>
    Server;

struct MyCar {
  float x;
  float y;
  float theta;
} car_target_pose;

/*
 * define output space
 *
 */
float *out_cmd_vel = new float[boid_num];
float *out_theta_cmd = new float[boid_num];

vector<ros::Publisher> vel_pub_set_;

/*
 * call back function
 */
void robotOdomCallback(const nav_msgs::OdometryConstPtr &locator, int marker) {
  int k = marker;
  nav_msgs::Odometry robotOdometryMsg = *locator;
  // current pose, x, y, theta
  pos_x_array[k] = robotOdometryMsg.pose.pose.position.x;
  pos_y_array[k] = robotOdometryMsg.pose.pose.position.y;
  // thata -> yaw(0 ~ 2 * 3.14)
  pos_theta_array[k] =
      convert_pi(tf::getYaw(robotOdometryMsg.pose.pose.orientation));
}

void execute(const global_vision_position::MoveGoalConstPtr &goal, Server *as) {
  global_vision_position::MoveFeedback feedback;

  ROS_INFO("Car Move is working.");
  // each car will keep in the distance of 10cm
  car_target_pose.x = goal->car_target_x;
  car_target_pose.y = goal->car_target_y;
  car_target_pose.theta = goal->car_target_theta;

  geometry_msgs::Twist vel_msgs;
  float break_flag;

  ros::Rate r(10);

  while (ros::ok()) {
    int i = 1;
    for (auto vel_pub = vel_pub_set_.begin(); vel_pub != vel_pub_set_.end();) {
      double angle_to_goal =
          convert_pi(atan2((car_target_pose.y + 0.5 * (i - 1)) - pos_y_array[i],
                           car_target_pose.x - pos_x_array[i]));

      double move_orientation = (angle_to_goal - pos_theta_array[i]);

      double dist =
          sqrt(pow(car_target_pose.x - pos_x_array[i], 2) +
               pow((car_target_pose.y + 0.5 * (i - 1)) - pos_y_array[i], 2));
      // 0.125 is the parameter, follow the specific car preference
      // vel_msgs.linear.x = 0.15 * look_ahead_distance;
      vel_msgs.linear.x = 0;
      vel_msgs.angular.z = 0;

      feedback.present_car_x = pos_x_array[i];
      feedback.present_car_y = pos_y_array[i];
      feedback.present_car_theta = pos_theta_array[i];
      as->publishFeedback(feedback);
      if (dist > ld) {
        double radius = 0.5 * (ld / sin(move_orientation));
        double linear_velocity = _linear_velocity;
        double angular_velocity = (-1) * _linear_velocity / radius;
        vel_msgs.linear.x = linear_velocity;
        vel_msgs.angular.z = angular_velocity;

        flag_array[i] = true;  // true 1, flase 0
      } else
        flag_array[i] = false;

      (*vel_pub).publish(vel_msgs);
      vel_pub++;
      i++;
    }
    // all break flag is 0
    bool flag = false;
    for (int j = 1; j < boid_num; j++) flag = flag | flag_array[j];
    if (!flag) break;
    r.sleep();
  }
  // 当action完成后，向客户端返回结果
  ROS_INFO("carMove is finished.");
  as->setSucceeded();

  for (auto vel_pub = vel_pub_set_.begin(); vel_pub != vel_pub_set_.end();) {
    // 1 is the parameter, follow the specific car preference
    // 安全距离
    vel_msgs.angular.z = 0.0;
    // 0.125 is the parameter, follow the specific car preference
    vel_msgs.linear.x = 0.0;
    (*vel_pub).publish(vel_msgs);
    vel_pub++;
  }
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "move_server");
  // create ros node handle
  ros::NodeHandle n;

  // create ros node handle
  ros::NodeHandle nh;
  vector<ros::Subscriber> odom_sub_set_;
  for (int i = 1; i < boid_num; i++) {
    string front_str = "/robot_";
    string end_str = "/pose";
    stringstream ss;
    // construct topic '/marker1/cmd_vel'
    ss << front_str << i << end_str;
    string topic = ss.str();
    ros::Subscriber odom_sub_ = nh.subscribe<nav_msgs::Odometry>(
        topic, 1, boost::bind(&robotOdomCallback, _1, i));

    odom_sub_set_.push_back(odom_sub_);
  }
  ros::Publisher vel_pub_;

  for (int i = 1; i < boid_num; i++) {
    string front_str = "/robot_";
    string end_str = "/cmd_vel";
    stringstream ss;
    // construct topic '/marker1/cmd_vel'
    ss << front_str << i << end_str;
    string topic = ss.str();
    vel_pub_ = n.advertise<geometry_msgs::Twist>(topic, 30);
    vel_pub_set_.push_back(vel_pub_);
  }
  // define a server
  Server server(n, "carMove", boost::bind(&execute, _1, &server), false);
  // server start
  server.start();
  ROS_INFO("server has started.");
  ros::spin();

  return 0;
}