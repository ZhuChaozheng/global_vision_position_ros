/*
 * functions:
 * navigation Point, publish pose and map in ROS
 * interface has completed half of content.
 *
 */
#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include "std_msgs/String.h"
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <signal.h>
#include <opencv2/plot.hpp>
#include <cmath>
#include <nav_msgs/Odometry.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include "car.h"
#include "file_operation.h"
#include "tcp.h"

using namespace cv;
using namespace std;

char sockfd_array[10];

void combine_buff(char buff[], int size, float linear_velocity, 
        float angular_velocity);
/**
 * receive /status topic and save sockfd
 */
void comm_call_back(const std_msgs::String::ConstPtr& msg)
{
    ROS_INFO("I heard: [%s]", msg->data.c_str());
    
    // convert from const char * to char * 
    char *str;
    str = const_cast<char *>(msg->data.c_str());
    char *pch;
    pch = strtok (str," "); // match space
    float float_array[30];
    float pch_f;
    int i = 0;
    while (pch != NULL)
    {
     // printf ("%s\n", pch);
      pch_f = atof(pch);
      float_array[i] = pch_f;
      pch = strtok (NULL, " ");
    }
    int j = 0;
    for (int k = 0; k < 10; k++)
    {
        sockfd_array[k] = float_array[j]; // only save sockfd
        j = j + 3;
    }
}

/**
 * receive linear_velocity and angular_velocity and packet them to buf,
 * then write the corresponding sockfd.
 */
void vel_command_callback(const geometry_msgs::TwistConstPtr& msg, int marker)
{
    float linear_velocity = static_cast<float>(msg->linear.x);
    float angular_velocity = static_cast<float>(msg->angular.z);
    int sockfd = sockfd_array[marker];
    char buff[11];
    buff[0] = 0x7D;
    buff[1] = 0x7A;
    buff[2] = marker;
    buff[5] = 0;
    buff[6] = 0;
    buff[10] = 0x7B;
    combine_buff(buff, 11, linear_velocity, angular_velocity);
    write_func(sockfd, buff);
}

void combine_buff(char buff[], int size, float linear_velocity, 
        float angular_velocity)
{
    int buff_linear = int(linear_velocity * 1000);
    int buff_angular = int(angular_velocity * 1000);
    buff[3] = buff_linear >> 8;
    buff[4] = buff_linear;
    buff[7] = buff_angular >> 8;
    buff[8] = buff_angular;
    //buff[9] = check_num(buff, size);
    buff[9] = 0;
}

// implement the check num algorithm
void check_num(char buff[], int size)
{

}

int main(int argc, char** argv)
{
    ros::init(argc,argv,"control_sub");
    // create ros node handle
    ros::NodeHandle n;
    ros::Rate rate(20); 
    // receive /status topic
    ros::Subscriber sub = n.subscribe("comm", 1000, comm_call_back);
    // extend the new interface
    vector<ros::Subscriber> cmd_vel_set_;
    for (int i = 0; i <= 9; i ++)
    {
        string front_str = "/marker";
        string end_str = "/cmd_vel";
        stringstream ss;
        ss << front_str << i << end_str;
        string topic = ss.str();
        ros::Subscriber sub = n.subscribe<geometry_msgs::Twist>(topic, 30,
                boost::bind(&vel_command_callback, _1, i));
        cmd_vel_set_.push_back(sub);
    }
    ros::spin();    
    return 0;
}
