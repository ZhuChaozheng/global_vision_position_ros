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
#include "timer.h"
#include "kalman.h"
#include "car.h"
#include "udp.h"
#include "pid.h"
#include "file_operation.h"
#include "car.h"
#include "navigation.h"

using namespace cv;
using namespace std;

bool app_stopped = false;
udp udp_server;
vector<Car> car_set;
/*
 *
 * Car:
 *     target_point, target_speed, target_slope, stop_flag
 * explain the interface data
 */
void hardware_control_interface(vector<Car> &cars_control_set)
{    
    /*
     * according the protocol, the vector's size is 10, each one express
     * a car, and the first of vector is the car of 0 marker. the last of vector
     * is the car of 9 marker.
     *
     * the next code is to map the relational sequence.
     */
    int i = 0;      
    for (auto iter = cars_control_set.begin(); iter != cars_control_set.end();)
    {           
        Point2f target_point = (*iter).get_target_point();
        float target_speed = (*iter).get_target_speed();
        float target_slope = (*iter).get_target_slope();
        float stop_flag = (*iter).get_stop_flag();
        for (auto car = car_set.begin(); car != car_set.end();)
        {
            int marker = (*car).get_marker();
            if (marker == i)
            {
                (*car).set_target_point(target_point);
                (*car).set_target_speed(target_speed);
                (*car).set_target_slope(target_slope);
                (*car).set_stop_flag(stop_flag);
            }
            car ++;
        }
        i ++;
        iter ++;
    }
}

/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
void comm_call_back(const std_msgs::String::ConstPtr& msg)
{
    ROS_INFO("I heard: [%s]", msg->data.c_str());
    
    // convert from const char * to char * 
    char *str;
    str = const_cast<char *>(msg->data.c_str());
    char *pch;
    pch = strtok (str," "); // match space
    vector<float> float_set;
    float pch_f;
    while (pch != NULL)
    {
     // printf ("%s\n", pch);
      pch_f = atof(pch);
      float_set.push_back(pch_f);
      pch = strtok (NULL, " ");
    }
    // convert float vector to char array, every car is 5*4,
    // the total of ten cars is equal to 5*4*10=200
    char src[200];
    char *temp;
    int i = 0;
    for(auto iter = float_set.begin(); iter != float_set.end();)
    {       
        temp = (char*)(&(*iter));
        src[i] = temp[0];
        src[i + 1] = temp[1];
        src[i + 2] = temp[2];
        src[i + 3] = temp[3];
        i = i + 4;
        iter ++;
    }
    udp udp_comm;
    int sock_fd = udp_comm.udp_init("127.0.0.1", 8888); // for process interaction
    // send data through udp
    udp_comm.send_data(sock_fd, src, sizeof(src));
}

/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
void vel_command_callback(const geometry_msgs::TwistConstPtr& msg, int marker)
{
    float linear_x = static_cast<float>(msg->linear.x);
    float theta = static_cast<float>(msg->angular.z);
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        if ((*iter).get_marker() == marker)
        {
            float linear_x = static_cast<float>(msg->linear.x);
            float theta = static_cast<float>(msg->angular.z);
            (*iter).set_target_speed(linear_x);
            (*iter).set_target_slope(theta);

            cout << "control: %d" << marker << "linear_x %f" << linear_x << "theta: %f" << theta << endl;
        }
        iter ++;
    }
}


/* 
 * call back function
 */
void robotOdomCallback(const nav_msgs::OdometryConstPtr& locator, int marker)
{
    int i = marker;
    nav_msgs::Odometry robotOdometryMsg = *locator;
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        if ((*iter).get_marker() == i)
        {
            float slope;
            float speed;
            Point2f medianPoint;

            slope = convertDegree(tf::getYaw(robotOdometryMsg.pose.pose.orientation));

            cout << "control: " << i << " slope: " << slope << endl;
            medianPoint.x = robotOdometryMsg.pose.pose.position.x;
            medianPoint.y = robotOdometryMsg.pose.pose.position.y;
            speed = robotOdometryMsg.twist.twist.linear.x;
            (*iter).set_slope(slope);
            (*iter).set_median_point(medianPoint);
            (*iter).set_speed(speed);
        }
        iter ++;
    }

}

void* sub_spin(void* args) 
{
	// create ros node handle
	ros::NodeHandle n;
    // compatible the old interface
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
    vector<ros::Subscriber> odom_sub_set_;
    for (int i = 0; i <= 9; i ++)
    {
        string front_str = "/odom_";
        stringstream ss;
        // construct topic '/marker1/cmd_vel'
        ss << front_str << i;
        string topic = ss.str();
        ros::Subscriber odom_sub_ = n.subscribe<nav_msgs::Odometry>(topic, 
                25, boost::bind(&robotOdomCallback, _1, i));
        odom_sub_set_.push_back(odom_sub_);
    }
	ros::spin();
}

/*
 * launch a thread to wait for interface control information
 */
void* udp_server_interface(void* args)
{
    udp_server.udp_server_init();
    return 0;
}

int main(int argc, char** argv)
{
    ros::init(argc,argv,"control_sub");
    ros::NodeHandle n;
    ros::Rate rate(20); 
    // ************ udp control interface thread *********
    pthread_t tids[2];
    int ret_udp = pthread_create(&tids[0], NULL, udp_server_interface, NULL);
    if (ret_udp != 0)
    {
       cout << "pthread_create error at ret_udp: error_code=" << ret_udp << endl;
    }
    // ********************* sub thread **************
    int ret_sub_spin = pthread_create(&tids[1], NULL, 
            sub_spin, NULL);
    if (ret_sub_spin != 0)
    {
       cout << "pthread_create init error at ret_sub_spin: error_code=" << ret_sub_spin << endl;
    }
    // **************** main thread *********************
    ConfigParamtersRead(car_set);
    double consumeTime;
    ros::Time currentTime,lastTime;
    lastTime = ros::Time::now();
    while(n.ok())
    {
        vector<Car> cars_control_set = udp_server.get_cars_control_set();
        // cout << cars_control_set.size() << endl;
        if(cars_control_set.size() == 10)
        {
            hardware_control_interface(cars_control_set);
            udp_server.erase_cars_control_set();
        }
        for (auto iter = car_set.begin(); iter != car_set.end();)
        {
            int marker = (*iter).get_marker();
            float slope = (*iter).get_slope();
            float speed =  (*iter).get_speed();
            cout << marker << " ,receive: " << slope << endl;
            // ***************** Navigation Step ********************            
            if (NavigateTargetPoint(*iter) == 0) // reach target
                // assign next target
                if (!(*iter).navigateQueue.empty()) 
                {
                    Point2f temp = (*iter).navigateQueue.front();
                    (*iter).set_target_point(temp);
                    (*iter).navigateQueue.pop(); // delete front point
                   // (*iter).navigateQueue.push_back(temp); // insert front point into end 
                }
            /************************* end Navigation *****************/
            pid pid_control;
            pid_control.controlSpeedAndAngular(*iter); 
            iter ++;
        }
        currentTime = ros::Time::now();
        consumeTime = currentTime.toSec() - lastTime.toSec();
        cout << "control_sub fps: " << 1/consumeTime << "Hz" << endl;
        lastTime = currentTime;
        rate.sleep();
    }
    
    return 0;
}
