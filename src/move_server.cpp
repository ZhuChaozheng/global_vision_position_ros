#include <iostream>
#include <stdio.h>
#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_listener.h>
#include <actionlib/server/simple_action_server.h>
#include "global_vision_position/MoveAction.h"
#include "car.h"
using namespace std;

/*
 * define data space
 */
int boid_num = 9;
float *pos_x_array = new float[boid_num];
float *pos_y_array = new float[boid_num];
float *pos_theta_array = new float[boid_num];
bool  *flag_array = new bool[boid_num];
bool modi_flag = true;

typedef actionlib::SimpleActionServer<global_vision_position::MoveAction> 
    Server;

struct MyCar
{
    float x;
    float y;
    float theta;
}car_target_pose;

/*
 * define output space
 *
 */
float *out_cmd_vel = new float[boid_num];
float *out_theta_cmd = new float[boid_num];

vector<ros::Publisher> vel_pub_set_;

float convertYaw(float yaw)
{
    if (yaw < 0)
        yaw = 2 * 3.14 + yaw;
    return yaw;
}

/* 
 * call back function
 */
void robotOdomCallback(const nav_msgs::OdometryConstPtr& locator, int marker)
{
    int k = marker;
    nav_msgs::Odometry robotOdometryMsg = *locator;
    if (k == 0)
    {
        ROS_INFO("marker: %d, robot Position: %f, %f", k, robotOdometryMsg.pose.pose.position.x,
             robotOdometryMsg.pose.pose.position.y);
        ROS_INFO("marker: %d, robot Heading: theta:%f", k, 
        (tf::getYaw(robotOdometryMsg.pose.pose.orientation)));
    }
    // current pose, x, y, theta
    pos_x_array[k] = robotOdometryMsg.pose.pose.position.x;
    pos_y_array[k] = robotOdometryMsg.pose.pose.position.y;
    // thata -> yaw(0 ~ 2 * 3.14) 
    pos_theta_array[k] = convertYaw(tf::getYaw(robotOdometryMsg.pose.pose.orientation));  
}



void execute(const global_vision_position::MoveGoalConstPtr& goal, 
        Server* as)
{
    global_vision_position::MoveFeedback feedback;

    ROS_INFO("Car Move is working.");
    // each car will keep in the distance of 10cm 
    car_target_pose.x = goal->car_target_x;
    car_target_pose.y = goal->car_target_y; 
    car_target_pose.theta = goal->car_target_theta;
    
    geometry_msgs::Twist vel_msgs;
    float break_flag;
    
    ros::Rate r(10);
    while(ros::ok())
    {  
        int i = 0;
        for(auto vel_pub = vel_pub_set_.begin(); 
                vel_pub != vel_pub_set_.end();)  
        {
            // 1 is the parameter, follow the specific car preference
            vel_msgs.angular.z = 1 * (atan2((car_target_pose.y + 0.1 * i)- 
                    pos_y_array[i], car_target_pose.x - 
                    pos_x_array[i]) - pos_theta_array[i]);
            
            double angle_to_goal = atan2((car_target_pose.y + 0.1 * i) - 
                    pos_y_array[i], car_target_pose.x - 
                    pos_x_array[i]);
            ROS_INFO("marker = %d, angle_to_goal = %f, current_angle = %f", 
                    i, angle_to_goal, pos_theta_array[i]);
            // 0.125 is the parameter, follow the specific car preference
            vel_msgs.linear.x = 0.125 * sqrt(pow(car_target_pose.x - 
                    pos_x_array[i], 2) + pow((car_target_pose.y + 0.1 * i) - 
                    pos_y_array[i], 2));
            ROS_INFO("linear.x  = %f, angular.z = %f", vel_msgs.linear.x,
                    vel_msgs.angular.z);
            feedback.present_car_x = pos_x_array[i];
            feedback.present_car_y = pos_y_array[i];
            feedback.present_car_theta = pos_theta_array[i];
            as->publishFeedback(feedback);
            break_flag = sqrt(pow(car_target_pose.x - 
                    pos_x_array[i], 2) + pow((car_target_pose.y + 0.1 * i) - 
                    pos_y_array[i], 2));
            ROS_INFO("break_flag = %f", break_flag);
            
            if((break_flag < 0.1) | (abs(pos_x_array[i]) < 0.001))
            {
                flag_array[i] = false;// true 1, flase 0
            }
            else
                flag_array[i] = true;
            (*vel_pub).publish(vel_msgs);
            vel_pub ++;
            i ++; 
        }
        // all break flag is 0
        bool flag = false;
        for (int j = 0; j <= boid_num; j++)
            flag = flag | flag_array[j];
        if (!flag)
            break;
        r.sleep();
    }
    // 当action完成后，向客户端返回结果
    ROS_INFO("carMove is finished.");
    as->setSucceeded();
}

int main(int argc, char** argv) 
{
    ros::init(argc, argv, "move_server");
    // create ros node handle
    ros::NodeHandle n;
    
    // create ros node handle
    ros::NodeHandle nh;   
    vector<ros::Subscriber> odom_sub_set_;
    for (int i = 0; i < boid_num; i ++)
    {
        string front_str = "/odom_";
        stringstream ss;
        // construct topic '/marker1/cmd_vel'
        ss << front_str << i;
        string topic = ss.str();
        ros::Subscriber odom_sub_ = nh.subscribe<nav_msgs::Odometry>(topic, 
                1, boost::bind(&robotOdomCallback, _1, i));

        odom_sub_set_.push_back(odom_sub_);
    }
    ros::Publisher vel_pub_;
    
    for (int i = 0; i < boid_num; i ++)
    {
        string front_str = "/marker";
        string end_str = "/cmd_vel";
        stringstream ss;
        // construct topic '/marker1/cmd_vel'
        ss << front_str << i << end_str;
        string topic = ss.str();
        vel_pub_ = n.advertise<geometry_msgs::Twist>(topic, 30);
        vel_pub_set_.push_back(vel_pub_);
    }
    // define a server
    Server server(n, "carMove", boost::bind(&execute, _1, &server),
            false);
    // server start
    server.start();
    ROS_INFO("server has started.");
    ros::spin();
    
    return 0;
}