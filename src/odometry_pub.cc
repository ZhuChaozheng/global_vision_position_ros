/*
 * functions:
 * navigation Point, publish pose and map in ROS
 * interface has completed half of content.
 *
 */
#include <Eigen/Core>
#include <Eigen/Dense>

#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TransformStamped.h>
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

// classification of car
vector<Car> car_set;
/* 
 * wait for the signal keyboard of ctrl+c
 */
void sigint_handler(int sig) 
{
    if (sig == SIGINT) 
    {
        std::cout << "C pressed!" << endl;
        app_stopped = true;
    }
}

float convertDegree(double yaw) {
    if (yaw < 0)
        yaw = yaw + 2 * 3.1415926;
    return 180 * yaw / 3.1415926;
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "odometry_pub");
    ros::NodeHandle n;
    ros::Publisher odom_publisher_;
    // batch initilization the topic vector
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        int marker = (*iter).get_marker();    
        string front_str = "/odom_";
        stringstream ss;
        ss << front_str << marker;
        string topic = ss.str();
        odom_publisher_ = n.advertise<nav_msgs::Odometry>(topic, 50);
        odom_publisher_set_.push_back(odom_publisher_);
        iter ++;
    }
    ros::Rate rate(30);  
    tf::TransformListener car_location;
    
    //用来保存寻找到的坐标变换数据
    tf::StampedTransform tag_0;
    tf::StampedTransform tag_1;
    tf::StampedTransform tag_2;
    tf::StampedTransform tag_3;
    tf::StampedTransform tag_4;
    tf::StampedTransform tag_5;
    tf::StampedTransform tag_6;
    tf::StampedTransform tag_7;
    tf::StampedTransform tag_8;
    tf::StampedTransform tag_9;
    // init parameter of pid through rosparams
    ConfigParamtersRead(car_set);
    // *********** main thread ******************
    // register signal ctrl+c
    signal(SIGINT, sigint_handler);
    // clock_t lastTime = clock(); 
    vector<Car> carStateSet; 
    
    double consumeTime;
    ros::Time currentTime,lastTime;
    lastTime = ros::Time::now();
    // build a car alive set based on slope, once it is not zero
    vector<Car> car_alive_set;
    while(n.ok())
    {

        //tag_0.child_frame_id_ = "tag_0";
        try
        {
            //寻找坐标变换
          
            car_location.lookupTransform("hik_camera","tag_0",ros::Time(0),tag_0);
            car_location.lookupTransform("hik_camera","tag_1",ros::Time(0),tag_1);
            car_location.lookupTransform("hik_camera","tag_2",ros::Time(0),tag_2);
            car_location.lookupTransform("hik_camera","tag_3",ros::Time(0),tag_3);/*
            car_location.lookupTransform("hik_camera","tag_4",ros::Time(0),tag_4);
            car_location.lookupTransform("hik_camera","tag_5",ros::Time(0),tag_5);
            car_location.lookupTransform("hik_camera","tag_6",ros::Time(0),tag_6);
/*
            car_location.lookupTransform("hik_camera","tag_7",ros::Time(0),tag_7);
            car_location.lookupTransform("hik_camera","tag_8",ros::Time(0),tag_8);
            car_location.lookupTransform("hik_camera","tag_9",ros::Time(0),tag_9);
  */          
     
            for (auto iter = car_set.begin(); iter != car_set.end();)
            {
                float target_slope = (*iter).get_target_slope();
                // Point2f medianPoint = (*iter).get_median_point();
                //Point2f medianPoint = Point2f(0, 0);
                int marker = (*iter).get_marker();
                //    float slope = (*iter).get_slope();
                // Point3f targetPoint = (*iter).get_target();
                Point2f target_point = (*iter).get_target_point();
        	    float target_speed = (*iter).get_target_speed();
    	        // float speed = (*iter).get_speed();
    	        double quatx, quaty, quatz, quatw;
                double roll, pitch, yaw; // save roll pitch yaw
                float slope;
                Point2f medianPoint;

                if (marker == 1) {
            	    quatx = tag_1.getRotation().getX(); 
                    quaty = tag_1.getRotation().getY();
                    quatz = tag_1.getRotation().getZ();
                    quatw = tag_1.getRotation().getW();
            	    tf::Quaternion quat(quatx, quaty, quatz, quatw);
                    slope = tf::getYaw(quat);
                    // tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);//convert
                    // slope = convertDegree(yaw); 
                    medianPoint = Point2f(tag_1.getOrigin().x() * 1000, 
                            tag_1.getOrigin().y() * 1000);
                }
                (*iter).set_slope(slope);
                (*iter).set_median_point(medianPoint);
        	    //cout << "marker: " << marker << endl;
        	    //cout << "slope: " << slope << endl; 
    	        cout << marker << " " << slope << "medianPoint: " 
                        << medianPoint << endl;
                Car lastCar;
                if (Exist((*iter), carStateSet, lastCar))
                {
                    Point2f lastMedianPoint = lastCar.get_median_point();
                    // cout << "lastMedianPoint: " << lastMedianPoint << endl;
                    float lastSlope = lastCar.get_slope();
                    float lastspeed = (*iter).get_speed();
    		        float speed = sqrt(pow(medianPoint.x - 
                            lastMedianPoint.x, 2) + pow(medianPoint.y - 
                            lastMedianPoint.y, 2)) / consumeTime; 
                    float angular = (slope - lastSlope) / consumeTime;
                    (*iter).set_speed(speed);
                    DeleteCar(*iter, carStateSet);
                }
                carStateSet.push_back((*iter));
                // if the slope is 0, then the car may stay offline
        	    if (slope != 0)
                    car_alive_set.push_back(*iter);

                nav_msgs::Odometry odom;
                odom.header.stamp = current_time;
                odom.header.frame_id = "odom";

                //set the position
                odom.pose.pose.position.x = medianPoint.x;
                odom.pose.pose.position.y = medianPoint.y;
                odom.pose.pose.position.z = 0.0;
                odom.pose.pose.orientation = quat;

                //set the velocity
                odom.child_frame_id = "base_link";
                odom.twist.twist.linear.x = speed;
                odom.twist.twist.linear.y = 0;
                odom.twist.twist.angular.z = angular;

                //publish the message
                for(auto odom_publisher = odom_publisher_set_.begin(); 
                    odom_publisher != odom_publisher_set_.end();)
                {    
                    string front_str = "/odom_";
                    stringstream ss;
                    ss << front_str << marker;
                    string topic = ss.str();
                    if ((*odom_publisher).getTopic() == topic)
                    {
                        (*odom_publisher).publish(odom);
                        break;
                    }
                    odom_publisher ++;
                }
                iter ++;
            }
        	currentTime = ros::Time::now();
        	consumeTime = currentTime.toSec() - lastTime.toSec();
        	//cout << "main fps: " << 1/consumeTime << "Hz" << endl;
        	lastTime = currentTime;
            }
            catch(tf2::TransformException& ex) {
                ROS_ERROR("%s", ex.what());
                ros::Duration(1.0).sleep();
                continue;
            }
            rate.sleep();
        }
    return 0;
}
