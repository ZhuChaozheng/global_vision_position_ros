#include <iostream>
#include "boid_model.h"
#include <stdio.h>
#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
using namespace std;

/*
 * define data space
 */
int boid_num = 10;
extern float *pos_x_array = new float[boid_num];
extern float *pos_y_array = new float[boid_num];
extern float *vel_x_array = new float[boid_num];
extern float *vel_y_array = new float[boid_num];

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
	  int i = marker;
    nav_msgs::Odometry robotOdometryMsg = *locator;
  	// ROS_INFO("marker: %d, robot Position: %f, %f", i, robotOdometryMsg.pose.pose.position.x,
   //          robotOdometryMsg.pose.pose.position.y);
  	// ROS_INFO("marker: %d, robot Heading: linear:%f, angular:%f", i, robotOdometryMsg.twist.twist.linear.x,
   //          robotOdometryMsg.twist.twist.angular.z);
  	pos_x_array[i] = robotOdometryMsg.pose.pose.position.x;
  	pos_y_array[i] = robotOdometryMsg.pose.pose.position.y;
  	vel_x_array[i] = robotOdometryMsg.twist.twist.linear.x;
  	vel_y_array[i] = robotOdometryMsg.twist.twist.linear.y;
}

void* sub_spin(void* args) 
{
    // create ros node handle
    ros::NodeHandle nh;   
    vector<ros::Subscriber> odom_sub_set_;
    for (int i = 0; i <= 9; i ++)
    {
        string front_str = "/odom_";
        stringstream ss;
        // construct topic '/marker1/cmd_vel'
        ss << front_str << i;
        string topic = ss.str();
        ros::Subscriber odom_sub_ = nh.subscribe<nav_msgs::Odometry>(topic, 
                25, boost::bind(&robotOdomCallback, _1, i));
        odom_sub_set_.push_back(odom_sub_);
    }
    ros::spin();
    return 0;
}

int main(int argc, char** argv) 
{
    float *tar_pos_x_array = new float[boid_num];
    float *tar_pos_y_array = new float[boid_num];
    float *tar_vel_x_array = new float[boid_num];
    float *tar_vel_y_array = new float[boid_num];
    /*
     * define target information
     *
     */
    for (int i = 0; i <= 9; i ++)
    {
      tar_pos_x_array[i] = 1.23;
      tar_pos_y_array[i] = 2.31;
      tar_vel_x_array[i] = 0;
      tar_vel_y_array[i] = 0;
    }
    /*
     * define obtacles
     */
    int ob_num = 1;
    float ob_pos_x, ob_pos_y;
    float *ob_pos_x_array = new float[ob_num];
    float *ob_pos_y_array = new float[ob_num];

    for(int j = 0; j < ob_num; j ++)
    {
        ob_pos_x = 110.0;
        ob_pos_y = 75.0;        
        ob_pos_x_array[j] = ob_pos_x;
        ob_pos_y_array[j] = ob_pos_y;
    }
    ros::init(argc, argv, "planning");
  	// create ros node handle
  	ros::NodeHandle n;
  	ros::Rate rate(30);  
    
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
  	// loop
  	while(n.ok())
    {
    		getFlockVelCmd(boid_num, pos_x_array, pos_y_array, 
    	                tar_pos_x_array, tar_pos_y_array, tar_vel_x_array, tar_vel_y_array,
    	                ob_num, ob_pos_x_array, ob_pos_y_array, vel_x_array, vel_y_array,
    	                out_cmd_vel, out_theta_cmd);
    		// convert out_cmd_vel, out_theta_cmd to /cmd_vel
    		// for(int i = 0, auto vel_pub = vel_pub_set_.begin(); 
      //           vel_pub != vel_pub_set_.end();)
        int i = 0;
        for(auto vel_pub = vel_pub_set_.begin(); 
                vel_pub != vel_pub_set_.end();)  
        {
            geometry_msgs::Twist twist;
    		  	twist.linear.x = out_cmd_vel[i];
    		  	twist.linear.y = 0;
    		  	twist.angular.z = out_theta_cmd[i];
            if (i == 2)
              cout << "out_cmd_vel: " << out_cmd_vel[i] << "out_theta_cmd: " << out_theta_cmd[i] << endl;
    		  	(*vel_pub).publish(twist); 
            vel_pub ++;
            i ++;
        }
    		rate.sleep();
  	}
  	return 0;
}
	