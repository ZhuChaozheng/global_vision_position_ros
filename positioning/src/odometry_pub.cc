/*
 * functions:
 * navigation Point, publish pose and map in ROS
 * interface has completed half of content.
 *
 */
#include <ctype.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TransformStamped.h>
#include <nav_msgs/Odometry.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <cmath>
#include <opencv2/plot.hpp>

#include "car.h"
#include "file_operation.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "tcp.h"

using namespace cv;
using namespace std;

// classification of car
vector<Car> car_set;

/**
 * receive /status topic and save sockfd
 */
void comm_call_back(const std_msgs::String::ConstPtr& msg) {
  ROS_INFO("I heard: [%s]", msg->data.c_str());

  // convert from const char * to char *
  char* str;
  str = const_cast<char*>(msg->data.c_str());
  char* pch;
  pch = strtok(str, " ");  // match space
  float float_array[80];
  float pch_f;
  int i = 0;
  // remove space and put the valid data in data array
  while (pch != NULL) {
    pch_f = atof(pch);
    float_array[i] = pch_f;
    pch = strtok(NULL, " ");
    i++;
  }
  // put the corresponding data into the specific car
  for (auto iter = car_set.begin(); iter != car_set.end();) {
    int marker = (*iter).get_marker();
    // see the protocol
    int velocity_id = marker * 8 + 1;
    int angular_velocity_id = marker * 8 + 7;  // gyro_z
    (*iter).set_velocity(float_array[velocity_id]);
    (*iter).set_angular_velocity(float_array[angular_velocity_id]);
    iter++;
  }
}

void* comm_sub_spin(void* args) {
  // create ros node handle
  ros::NodeHandle nh;
  // receive /status topic
  ros::Subscriber sub = nh.subscribe("comm", 1000, comm_call_back);
  ros::spin();
  return 0;
}

int main(int argc, char** argv) {
  ros::init(argc, argv, "odometry_pub");
  ros::NodeHandle n;
  ros::Rate rate(10);
  ros::Publisher odom_publisher_;
  vector<ros::Publisher> odom_publisher_set_;
  // init parameter of pid through rosparams
  ConfigParamtersRead(car_set);
  cout << car_set.size() << endl;
  // batch initilization the topic vector
  for (auto iter = car_set.begin(); iter != car_set.end();) {
    int marker = (*iter).get_marker();
    cout << marker << endl;
    string front_str = "/robot_";
    string end_str = "/pose";
    stringstream ss;
    // construct topic '/marker1/cmd_vel'
    ss << front_str << marker << end_str;
    string topic = ss.str();
    odom_publisher_ = n.advertise<nav_msgs::Odometry>(topic, 10);
    odom_publisher_set_.push_back(odom_publisher_);
    iter++;
  }
  // ************ receive  thread *********
  pthread_t pid[1];
  int ret_comm_sub_spin = pthread_create(&pid[0], NULL, comm_sub_spin, NULL);
  if (ret_comm_sub_spin != 0) {
    cout << "pthread_create error at ret_comm_sub_spin:error_code= "
         << ret_comm_sub_spin << endl;
  }
  // *********** end thread ************
  tf::TransformListener car_listener;
  //用来保存寻找到的坐标变换数据
  tf::StampedTransform transform;
  // tf::StampedTransform tag_2;
  // tf::StampedTransform tag_3;
  // tf::StampedTransform tag_4;
  // tf::StampedTransform tag_5;
  // tf::StampedTransform tag_6;
  // tf::StampedTransform tag_7;
  // tf::StampedTransform tag_8;
  // tf::StampedTransform tag_9;

  // *********** main thread ******************
  // clock_t lastTime = clock();
  vector<Car> carStateSet;
  // build a car alive set based on angle, once it is not zero
  vector<Car> car_alive_set;
  double consumeTime = 1;
  ros::Time currentTime, lastTime;
  lastTime = ros::Time::now();
  bool initial_flag = false;  // initial flag
  int j = 0;                  // mean the speed and angular
  ros::Duration transform_tolerance_;
  transform_tolerance_.fromSec(0.1);
  tf::TransformBroadcaster odom_broadcaster;
  tf::Transform lastTransfrom_map_in_odom;
  lastTransfrom_map_in_odom =
      tf::Transform(tf::createQuaternionFromRPY(0, 0, 0), tf::Vector3(0, 0, 0));
  tf::TransformBroadcaster base_broadcaster;
  tf::Transform lastTransfrom_tag_in_base;
  // (1, 0, 0, 0, -1, 0, 0, 0, -1)
  lastTransfrom_tag_in_base =
      tf::Transform(tf::Matrix3x3(1, 0, 0, 0, -1, 0, 0, 0, -1), 
      tf::Vector3(0, 0, 0));

  
  // publish the relationship between /map and /hik_camera, static
  tf::TransformBroadcaster map_hik_broadcaster;
  tf::Transform lastTransfrom_map_in_hik;
  lastTransfrom_map_in_hik = tf::Transform(
      tf::Matrix3x3(1, 0, 0, 0, -1, 0, 0, 0, -1), tf::Vector3(0.5, 2, 4));
  map_hik_broadcaster.sendTransform(tf::StampedTransform(
      lastTransfrom_map_in_hik, ros::Time::now(), "map", "/hik_camera"));

  while (n.ok()) {
    for (auto iter = car_set.begin(); iter != car_set.end();) {
      int marker = (*iter).get_marker();
      double quatx, quaty, quatz, quatw;
      tf::Quaternion quat;
      double roll, pitch, yaw;  // save roll pitch yaw
      float angle;
      Point2f medianPoint;
      string front_str_tag = "/tag_";;
      stringstream ss_tag;
      // construct tf '/robot_1/odom'
      ss_tag << front_str_tag << marker;
      string tf_tag = ss_tag.str();
      

      string front_str = "/robot_";
      string end_str_odom = "/odom";
      stringstream ss_odom;
      // construct tf '/robot_1/odom'
      ss_odom << front_str << marker << end_str_odom;
      string tf_odom = ss_odom.str();
      odom_broadcaster.sendTransform(tf::StampedTransform(
          lastTransfrom_map_in_odom, ros::Time::now(), "map", tf_odom));
      // /robot_1/base_link
      stringstream ss_base;
      string end_str_base = "/base_link";
      ss_base << front_str << marker << end_str_base;
      string tf_base = ss_base.str();
      // publish the relationship between /tag_1 and /robot_1/base, static
      base_broadcaster.sendTransform(tf::StampedTransform(
          lastTransfrom_tag_in_base, ros::Time::now(), tf_tag, tf_base));

      try{
        car_listener.lookupTransform("map", tf_base,  
                                 ros::Time(0), transform);
      }
      catch (tf::TransformException ex){
        ROS_ERROR("%s",ex.what());
        ros::Duration(1.0).sleep();
      }
      quatx = transform.getRotation().getX();
      quaty = transform.getRotation().getY();
      quatz = transform.getRotation().getZ();
      quatw = transform.getRotation().getW();
      medianPoint = Point2f(transform.getOrigin().x(), transform.getOrigin().y());
     
      quat = tf::Quaternion(quatx, quaty, quatz, quatw);
      yaw = tf::getYaw(quat);

      // float angle = tag_3.getRotation().getAngle();
      // tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);//convert
      angle = convertDegree(yaw);
      // update the states
      (*iter).set_angle(angle);
      (*iter).set_median_point(medianPoint);
      float velocity = (*iter).get_velocity();
      float angular_velocity = (*iter).get_angular_velocity();

      geometry_msgs::Quaternion odom_quat =
          tf::createQuaternionMsgFromYaw(yaw);

      // next, we'll publish the odometry message over ROS
      nav_msgs::Odometry odom;
      odom.header.stamp = currentTime;
      odom.header.frame_id = "map";
      odom.child_frame_id = tf_base;

      // set the position
      odom.pose.pose.position.x = medianPoint.x;
      odom.pose.pose.position.y = medianPoint.y;
      odom.pose.pose.position.z = 0.0;
      odom.pose.pose.orientation = odom_quat;

      // set the velocity
      odom.twist.twist.linear.x = velocity;
      odom.twist.twist.linear.y = 0;
      odom.twist.twist.angular.z = angular_velocity;

      // publish the message
      for (auto odom_publisher = odom_publisher_set_.begin();
           odom_publisher != odom_publisher_set_.end();) {
        string front_str = "/robot_";
        string end_str = "/pose";
        stringstream ssss;
        // construct topic '/marker1/cmd_vel'
        ssss << front_str << marker << end_str;
        string topic = ssss.str();
        if ((*odom_publisher).getTopic() == topic) {
          (*odom_publisher).publish(odom);
          break;
        }
        odom_publisher++;
      }
      iter++;
    }
    currentTime = ros::Time::now();
    consumeTime = currentTime.toSec() - lastTime.toSec();
    // cout << "odometry fps: " << 1/consumeTime << "Hz" << endl;
    lastTime = currentTime;
  
    rate.sleep();
  }
  return 0;
}
