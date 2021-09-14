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

/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
void vel_command_callback(const geometry_msgs::TwistConstPtr& msg, int marker)
{
    cout << "vel_command_callback:  " << marker << endl;
    float linear_x = static_cast<float>(msg->linear.x);
    float theta = static_cast<float>(msg->angular.z);
    vector<float> float_set;
    // fill in the full float_set, 10*5
    for ( int i = 0; i < marker * 5; i++)
        float_set.push_back(0);
    float_set.push_back(0);
    float_set.push_back(0);
    float_set.push_back(linear_x);
    float_set.push_back(theta);
    float_set.push_back(0);
    for (int j = 0; j < (9 - marker) * 5; j++)
        float_set.push_back(0);
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

void sub_spin() 
{
	// create ros node handle
	ros::NodeHandle n;
    vector<ros::Subscriber> cmd_vel_set_;
    for (int i = 0; i <= 9; i ++)
    {
        string front_str = "/marker";
        string end_str = "/cmd_vel";
        stringstream ss;
        ss << front_str << i << end_str;
        string topic = ss.str();
        ros::Subscriber sub = n.subscribe<geometry_msgs::Twist>(topic, 15,
                boost::bind(&vel_command_callback, _1, i));
        cmd_vel_set_.push_back(sub);
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
    // ************ udp control interface thread *********
    pthread_t tids[2];
    int ret_udp = pthread_create(&tids[0], NULL, udp_server_interface, NULL);
    if (ret_udp != 0)
    {
       cout << "pthread_create error at ret_udp: error_code=" << ret_udp << endl;
    }
    sub_spin();
    return 0;
}
