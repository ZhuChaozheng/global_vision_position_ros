#include <iostream>
#include "boid_model.h"
#include <stdio.h>
#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include "navigation.h"
#include <tf/transform_listener.h>
using namespace std;

/*
 * define data space
 */
int boid_num = 3;
extern float *pos_x_array = new float[boid_num];
extern float *pos_y_array = new float[boid_num];
extern float *vel_x_array = new float[boid_num];
extern float *vel_y_array = new float[boid_num];
extern bool modi_flag = true;

/*
 * define output space
 *
 */
float *out_cmd_vel = new float[boid_num];
float *out_theta_cmd = new float[boid_num];

/* 
 * call back function
 */
void robotOdomCallback(const nav_msgs::OdometryConstPtr& locator, int marker)
{
        int k = marker;
        nav_msgs::Odometry robotOdometryMsg = *locator;
        if (k == 0)
        {
            ROS_INFO("marker: %d, robot Position: %f, %f", i, robotOdometryMsg.pose.pose.position.x,
                 robotOdometryMsg.pose.pose.position.y);
            ROS_INFO("marker: %d, robot Heading: linear:%f, angular:%f", i, robotOdometryMsg.twist.twist.linear.x,
                 robotOdometryMsg.twist.twist.angular.z);    
        }   
        pos_x_array[k] = robotOdometryMsg.pose.pose.position.x * 100;
        pos_y_array[k] = robotOdometryMsg.pose.pose.position.y * 100;
        vel_x_array[k] = robotOdometryMsg.twist.twist.linear.x * 100;
        cout << "%d" << k << " vel: " << vel_x_array[i] << endl;
        vel_y_array[k] = convertDegree(tf::getYaw(robotOdometryMsg.pose.pose.orientation));  // vel_y -> yaw(0-360)    
        
}

void* sub_spin(void* args) 
{
    // create ros node handle
    ros::NodeHandle nh;   
    vector<ros::Subscriber> odom_sub_set_;
    for (int i = 0; i <= 2; i ++)
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
    ros::spin();
    return 0;
}

int main(int argc, char** argv) 
{
    ros::init(argc, argv, "planning");
    // create ros node handle
    ros::NodeHandle n;
    ros::Rate rate(15);  
    
    // ************ udp control interface thread *********
    pthread_t tids[1];
    int ret_sub_spin = pthread_create(&tids[0], NULL, sub_spin, NULL);
    if (ret_sub_spin != 0)
    {
        cout << "pthread_create error at ret_udp: error_code=" << ret_sub_spin << endl;
    }
    // ******* main thread ********
    ros::Publisher vel_pub_;
    vector<ros::Publisher> vel_pub_set_;
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
    while(n.ok())
    {            
        int i = 0;
        for(auto vel_pub = vel_pub_set_.begin(); 
                vel_pub != vel_pub_set_.end();)  
        {
            
            if (out_cmd_vel[i] == 0)
            {
                vel_pub ++;
                i ++;
                continue;
            }
            else
            {
                geometry_msgs::Twist twist;
                twist.linear.x = out_cmd_vel[i] / 100.0;
                twist.linear.y = 0;
                // convert orientation
                if (out_theta_cmd[i] <= 0)
                    out_theta_cmd[i] = -out_theta_cmd[i];
                else
                    out_theta_cmd[i] = 360.0 - out_theta_cmd[i];
                twist.angular.z = out_theta_cmd[i];
                if (i == 2)
                    cout << "out_cmd_vel: " << out_cmd_vel[i] << "out_theta_cmd: " << out_theta_cmd[i] << endl;
                (*vel_pub).publish(twist);
                vel_pub ++;
                i ++;
            }            
        }
        rate.sleep();
    }
    return 0;
}