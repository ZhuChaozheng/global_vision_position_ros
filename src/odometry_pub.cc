/*
 * functions:
 * navigation Point, publish pose and map in ROS
 * interface has completed half of content.
 *
 */
#include <Eigen/Core>
#include <Eigen/Dense>

#include "ros/ros.h"
#include "std_msgs/String.h"
#include <nav_msgs/Odometry.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TransformStamped.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>                                                                                                
#include <sys/time.h>
#include <signal.h>
#include <opencv2/plot.hpp>
#include <cmath>
#include "tcp.h"
#include "car.h"
#include "file_operation.h"

using namespace cv;
using namespace std;

// classification of car
vector<Car> car_set;

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
      i ++;
    }
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        int marker = (*iter).get_marker();
        // see the protocol
        int velocity_id = marker * 3 + 1;
        int angular_velocity_id = marker * 3 + 2;
        (*iter).set_velocity(float_array[velocity_id]);
        (*iter).set_angular_velocity(float_array[angular_velocity_id]);
        iter ++;
    } 
}

void* comm_sub_spin(void* args) 
{
    // create ros node handle
    ros::NodeHandle nh;   
    // receive /status topic
    ros::Subscriber sub = nh.subscribe("comm", 1000, comm_call_back);
    ros::spin();
    return 0;
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "odometry_pub");
    ros::NodeHandle n;    
    ros::Rate rate(10); 
    ros::Publisher odom_publisher_;
    vector<ros::Publisher> odom_publisher_set_;
    // init parameter of pid through rosparams
    ConfigParamtersRead(car_set);
    cout << car_set.size() << endl;
    // batch initilization the topic vector
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        
        int marker = (*iter).get_marker();
        cout << marker << endl;
        string front_str = "/odom_";
        stringstream ss;
        ss << front_str << marker;
        string topic = ss.str();
        odom_publisher_ = n.advertise<nav_msgs::Odometry>(topic, 10);
        odom_publisher_set_.push_back(odom_publisher_);
        iter ++;
    } 
    // ************ receive  thread *********
    pthread_t pid[1];
    int ret_comm_sub_spin = pthread_create(&pid[0], NULL, comm_sub_spin, NULL);
    if (ret_comm_sub_spin != 0)
    {
        cout << "pthread_create error at ret_comm_sub_spin:error_code= "
                << ret_comm_sub_spin << endl;
    }
    // *********** end thread ************
    tf::TransformListener car_location;
    //用来保存寻找到的坐标变换数据
    tf::StampedTransform tag_0;
    // tf::StampedTransform tag_1;
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
    ros::Time currentTime,lastTime;
    lastTime = ros::Time::now();
    bool initial_flag = false; // initial flag
    int j = 0; // mean the speed and angular
    tf::TransformBroadcaster odom_broadcaster;
    tf::Transform lastTransfrom_map_in_odom;
    lastTransfrom_map_in_odom = tf::Transform(tf::createQuaternionFromRPY
            (0, 0, 0), tf::Vector3(0, 0, 0));
    ros::Duration transform_tolerance_;
    transform_tolerance_.fromSec(0.1);
    while(n.ok())
    {

        //tag_0.child_frame_id_ = "tag_0";
        try
        {
            //寻找坐标变换
            car_location.lookupTransform("hik_camera","tag_0",ros::Time(0),tag_0);
            // car_location.lookupTransform("hik_camera","tag_1",ros::Time(0),tag_1);
            // car_location.lookupTransform("hik_camera","tag_2",ros::Time(0),tag_2);
           // car_location.lookupTransform("hik_camera","tag_3",ros::Time(0),tag_3);
         /*   /car_location.lookupTransform("hik_camera","tag_4",ros::Time(0),tag_4);
            car_location.lookupTransform("hik_camera","tag_5",ros::Time(0),tag_5);
            car_location.lookupTransform("hik_camera","tag_6",ros::Time(0),tag_6);
            car_location.lookupTransform("hik_camera","tag_7",ros::Time(0),tag_7);
            car_location.lookupTransform("hik_camera","tag_8",ros::Time(0),tag_8);*/
         //   car_location.lookupTransform("hik_camera","tag_9",ros::Time(0),tag_9);         
            
            for (auto iter = car_set.begin(); iter != car_set.end();)
            {
                int marker = (*iter).get_marker();
    	        double quatx, quaty, quatz, quatw;
                tf::Quaternion quat;
                double roll, pitch, yaw; // save roll pitch yaw
                float angle;
                Point2f medianPoint;
                if (marker == 0) {
                    quatx = tag_0.getRotation().getX(); 
                    quaty = tag_0.getRotation().getY();
                    quatz = tag_0.getRotation().getZ();
                    quatw = tag_0.getRotation().getW();
                    medianPoint = Point2f(tag_0.getOrigin().x(), 
                            tag_0.getOrigin().y() );
                }
                // if (marker == 1) {
            	//     quatx = tag_1.getRotation().getX(); 
                //     quaty = tag_1.getRotation().getY();
                //     quatz = tag_1.getRotation().getZ();
                //     quatw = tag_1.getRotation().getW();
                //     medianPoint = Point2f(tag_1.getOrigin().x(), 
                //             tag_1.getOrigin().y());
                // }
                // if (marker == 2) {
                //     quatx = tag_2.getRotation().getX(); 
                //     quaty = tag_2.getRotation().getY();
                //     quatz = tag_2.getRotation().getZ();
                //     quatw = tag_2.getRotation().getW();
                     
                //     medianPoint = Point2f(tag_2.getOrigin().x(), 
                //             tag_2.getOrigin().y());
                // }
                // if (marker == 3) {
                //     quatx = tag_3.getRotation().getX(); 
                //     quaty = tag_3.getRotation().getY();
                //     quatz = tag_3.getRotation().getZ();
                //     quatw = tag_3.getRotation().getW();
                //     medianPoint = Point2f(tag_3.getOrigin().x(), 
                //             tag_3.getOrigin().y());
                // }
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
                     
                //first, we'll publish the transform over tf
                geometry_msgs::TransformStamped odom_trans;
                odom_trans.header.stamp = currentTime;
                odom_trans.header.frame_id = "odom";
                odom_trans.child_frame_id = "base_link";
                
                odom_trans.transform.translation.x = medianPoint.x;
                odom_trans.transform.translation.y = medianPoint.y;
                odom_trans.transform.translation.z = 0.0;
                odom_trans.transform.rotation = odom_quat;
                //send the transform
                odom_broadcaster.sendTransform(odom_trans);

                //next, we'll publish the odometry message over ROS
                nav_msgs::Odometry odom;
                odom.header.stamp = currentTime;
                odom.header.frame_id = "odom";
                
                //set the position
                odom.pose.pose.position.x = medianPoint.x;
                odom.pose.pose.position.y = medianPoint.y;
                odom.pose.pose.position.z = 0.0;
                odom.pose.pose.orientation = odom_quat;

                //set the velocity
                odom.child_frame_id = "base_link";
                odom.twist.twist.linear.x = velocity;
                odom.twist.twist.linear.y = 0;
                odom.twist.twist.angular.z = angular_velocity;

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
        	// cout << "odometry fps: " << 1/consumeTime << "Hz" << endl;
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
