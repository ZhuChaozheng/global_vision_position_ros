/*
 * functions:
 * navigation Point, publish pose and map in ROS
 * interface has completed half of content.
 *
 */
#include <Eigen/Core>
#include <Eigen/Dense>

#include <unistd.h>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "nav_msgs/OccupancyGrid.h"
#include "nav_msgs/MapMetaData.h"
#include <tf/transform_listener.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TransformStamped.h>
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
#include "image_process.h"
#include "init_camera.h"
#include "image_transformation.h"
#include "file_operation.h"
#include "map_pub.h"
#include "car.h"
#include "navigation.h"

using namespace cv;
using namespace std;

// classification of car
vector<Car> car_set;
vector<Car> car_set_right;
vector<Car> car_set_left;
Mat src_left;
Mat src_right;
Mat src_thresh_left;
Mat src_thresh_right;
Mat barriers_image;
Mat barriers_image_right;
Mat barriers_image_left;
bool app_stopped = false;
udp udp_server;

int right_getImg_flag;
int left_getImg_flag;

int getCarLeft=0; // left camera data flag
int getCarRight=0; // right camera data flag

float K[3][3] = {676.643, 0, 639.651, 0, 677.714, 550.392, 0, 0, 1};
Mat camera_matrix_left=Mat(3, 3, CV_32FC1, K);
Mat camera_matrix_right=Mat(3, 3, CV_32FC1, K);
float dist[4][1] = {-0.049090, 0.057062, -0.004362, -0.000891};
//float dist[4][1] = {0, 0, 0, 0};
Mat distCoeffsRight=Mat(4, 1, CV_32FC1, dist);
Mat distCoeffsLeft=Mat(4, 1, CV_32FC1, dist);

// the parameters for coordination algorithm 
float k_dx = 0.3;
float k_dy = 0.3;
// Those two variables belong to extern, 
// and defined in image_process.h
int frame_type;
RotatedRect boundary_rect = RotatedRect(Point2f(100,100),
        Size2f(500,500), 0);
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

// the control of coordination
void coordination_move(vector<Car> &car_alive_set, float dx, 
        float dy, float v_ex, float v_ey)
{
    //cout << "corrdination size: " << car_alive_set.size() << endl;
    if (car_alive_set.size() == 0)
        return;
    for (auto iter = car_alive_set.begin(); 
            iter != car_alive_set.end();)
    {
/*
        float x_ij = (*iter).get_median_point().x - 
                (*(iter+1)).get_median_point().x;
	cout << "x_ij: " << x_ij << endl;
        float y_ij = (*iter).get_median_point().y - 
                (*(iter+1)).get_median_point().y;
	cout << "y_ij: " << y_ij << endl;
        // calculate the real speed on x, y direction
        float v_rx = k_dx * (x_ij - dx) + v_ex;
        float v_ry = k_dy * (y_ij - dy) + v_ey;
        // send command value
        float speed = sqrt(pow(v_rx, 2)+pow(v_ry, 2));
        float slope = asin (v_ry / speed);
	cout << "real speed: " << speed << endl;
	cout << "real slope: " << slope << endl;
*/
	float v_ij = (*iter).get_speed() - k_dx * ((*(iter+1)).get_speed() - (*iter).get_speed());
//	cout << "v_ij: " << v_ij << endl;
	for (auto iter_car = car_set.begin(); iter_car != car_set.end();)
    {
	    if ((*iter_car).get_marker() == (*(iter+1)).get_marker())
        {  
    /*
    		(*iter_car).set_target_speed(speed);
    		(*iter_car).set_target_slope(slope);
    */
    		(*iter_car).set_target_speed(v_ij);
	    }
	     iter_car ++;
	}
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


void* sub_spin(void* args) 
{
	// create ros node handle
	ros::NodeHandle n;
    	ros::Subscriber sub = n.subscribe("comm", 
            1000, comm_call_back);
	ros::spin();
	return 0;
}

/*
 * launch a thread to wait for interface control information
 */
void* udp_server_interface(void* args)
{
    udp_server.udp_server_init();
    return 0;
}
/* 
 * capture the camera and analyse the detail, then return 
 * point set of cars and barrier images. 
 * note: this is a thread
 */
void* getRightCameraImg(void* args)
{
    capture_handle_image(src_right, barriers_image_right, 
            car_set_right, getCarRight, camera_matrix_right,
            distCoeffsRight, 1);
}
/*
 * similiarly with the above function.
 */
void* getLeftCameraImg(void* args)
{
    capture_handle_image(src_left, barriers_image_left, 
            car_set_left, getCarLeft, camera_matrix_left,
            distCoeffsLeft, 0);
}

float convertDegree(double yaw) {
    if (yaw < 0)
        yaw = yaw + 2 * 3.1415926;
    return 180 * yaw / 3.1415926;
}

int main(int argc, char** argv)
{
    ros::init(argc,argv,"car_location");
    ros::NodeHandle n;
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
    
    // ************ udp control interface thread *********
    pthread_t tids[5];
    int ret_udp = pthread_create(&tids[0], NULL, udp_server_interface, NULL);
    if (ret_udp != 0)
    {
       cout << "pthread_create error at ret_udp: error_code=" << ret_udp << endl;
    }
    
    int ret_sub_spin = pthread_create(&tids[4], NULL, 
            sub_spin, NULL);
    if (ret_sub_spin != 0)
    {
       cout << "pthread_create init error at ret_sub_spin: error_code=" << ret_sub_spin << endl;
    }
    // *********** main thread ******************
    // register signal ctrl+c
    signal(SIGINT, sigint_handler);
    // clock_t lastTime = clock(); 
    vector<Car> carStateSet; 
    
    float main_period_sec = 1/30.0;
    float delay_sec = 0;
    double consumeTime;
    ros::Time currentTime,lastTime;
    lastTime = ros::Time::now();
    // build a car alive set based on slope, once it is not zero
    vector<Car> car_alive_set;

    Point2f p1 = Point2f(100, 200);
    Point2f p2 = Point2f(200, 200);
    Point2f p3 = Point2f(300, 200);
    Point2f p4 = Point2f(400, 200);
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        (*iter).navigateQueue.push(p1);
        (*iter).navigateQueue.push(p2);
        (*iter).navigateQueue.push(p3);
        (*iter).navigateQueue.push(p4);
    }

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
            vector<Car> cars_control_set = udp_server.get_cars_control_set();
            // cout << cars_control_set.size() << endl;
            if(cars_control_set.size() == 10)
        	{
                hardware_control_interface(cars_control_set);
        	    udp_server.erase_cars_control_set();
        	}
            if (app_stopped) break;
     
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
                            
                            tf::Matrix3x3(quat).getRPY(roll, pitch, yaw);//convert
                            slope = convertDegree(yaw); 
                medianPoint = Point2f(tag_1.getOrigin().x() * 1000, tag_1.getOrigin().y() * 1000);
                /*
                	    ROS_INFO("tag_1:Tx=%.2f,Ty=%.2f,Tz=%.2f, yaw=%.2f ",
                            tag_1.getOrigin().x(),
                            tag_1.getOrigin().y(),
                            tag_1.getOrigin().z(), slope);*/
                }

                if (marker == 2) {
                	    quatx = tag_2.getRotation().getX();
                            quaty = tag_2.getRotation().getY();
                            quatz = tag_2.getRotation().getZ();
                            quatw = tag_2.getRotation().getW();
                	    tf::Quaternion quat2(quatx, quaty, quatz, quatw);
                            //double roll, pitch, yaw; // save roll pitch yaw
                            tf::Matrix3x3(quat2).getRPY(roll, pitch, yaw);//convert
                            slope = convertDegree(yaw); 
                medianPoint = Point2f(tag_2.getOrigin().x() * 1000, tag_2.getOrigin().y() * 1000);
                	    /*ROS_INFO("tag_2:Tx=%.2f,Ty=%.2f,Tz=%.2f, yaw=%.2f ",
                            tag_2.getOrigin().x(),
                            tag_2.getOrigin().y(),
                            tag_2.getOrigin().z(), slope);
                	 */
                }
                if (marker == 3) {
                	    quatx = tag_3.getRotation().getX();
                            quaty = tag_3.getRotation().getY();
                            quatz = tag_3.getRotation().getZ();
                            quatw = tag_3.getRotation().getW();
                	    tf::Quaternion quat3(quatx, quaty, quatz, quatw);
                            //double roll, pitch, yaw; // save roll pitch yaw
                            tf::Matrix3x3(quat3).getRPY(roll, pitch, yaw);//convert
                            slope = convertDegree(yaw); 
                medianPoint = Point2f(tag_3.getOrigin().x() * 1000, tag_3.getOrigin().y() * 1000);
                	    /*ROS_INFO("tag_3:Tx=%.2f,Ty=%.2f,Tz=%.2f, yaw=%.2f ",
                            tag_3.getOrigin().x(),
                            tag_3.getOrigin().y(),
                            tag_3.getOrigin().z(), slope);*/
                }
                /*
                if (marker == 4) {
                	    quatx = tag_4.getRotation().getX();
                            quaty = tag_4.getRotation().getY();
                            quatz = tag_4.getRotation().getZ();
                            quatw = tag_4.getRotation().getW();
                	    tf::Quaternion quat4(quatx, quaty, quatz, quatw);
                            //double roll, pitch, yaw; // save roll pitch yaw
                            tf::Matrix3x3(quat4).getRPY(roll, pitch, yaw);//convert
                            slope = convertDegree(yaw); 
                	    /*ROS_INFO("tag_4:Tx=%.2f,Ty=%.2f,Tz=%.2f, yaw=%.2f ",
                            tag_4.getOrigin().x(),
                            tag_4.getOrigin().y(),
                            tag_4.getOrigin().z(), slope);
                            medianPoint = Point2f(tag_4.getOrigin().x() * 1000, tag_4.getOrigin().y() * 1000);
                }
                if (marker == 6) {
                	    quatx = tag_6.getRotation().getX();
                            quaty = tag_6.getRotation().getY();
                            quatz = tag_6.getRotation().getZ();
                            quatw = tag_6.getRotation().getW();
                	    tf::Quaternion quat6(quatx, quaty, quatz, quatw);
                            //double roll, pitch, yaw; // save roll pitch yaw
                            tf::Matrix3x3(quat6).getRPY(roll, pitch, yaw);//convert
                            slope = convertDegree(yaw); 
                medianPoint = Point2f(tag_6.getOrigin().x() * 1000, tag_6.getOrigin().y() * 1000);

                	    /*ROS_INFO("tag_0:Tx=%.2f,Ty=%.2f,Tz=%.2f, yaw=%.2f ",
                            tag_0.getOrigin().x(),
                            tag_0.getOrigin().y(),
                            tag_0.getOrigin().z(), slope);
                }*/
                (*iter).set_slope(slope);
                (*iter).set_median_point(medianPoint);
        	    //cout << "marker: " << marker << endl;
        	    //cout << "slope: " << slope << endl; 
    	        cout << marker << " " << slope << "medianPoint: " 
                        << medianPoint << endl;
        	    // cout << "target_slope: " << target_slope << endl;
        	    // cout << "medianPoint: " << medianPoint << endl;
        	    //cout << "target_point: " << target_point << endl;
        	    //cout << "target_speed: " << target_speed << endl;
                // if (marker == 0) 
                // {
                //     if (!(*iter).navigateQueue.empty()) 
                //     {
                //         Point2f temp = (*iter).navigateQueue.front();
                //         cout << temp << endl;
                //         //(*iter).set_target_point(temp);
                //         (*iter).navigateQueue.pop(); // delete front point
                //        // (*iter).navigateQueue.push_back(temp); // insert front point into end 
                //     }
                // }
            IsPointInRotatedRect(boundary_rect, )
            // ***************** Navigation Step ********************            
            if (NavigateTargetPoint(*iter) == 0) // reach target
                // assign next target
                if (!(*iter).navigateQueue.empty()) 
                {
                    Point2f temp = (*iter).navigateQueue.front();
                    cout << temp << endl;
                    // (*iter).set_target_point(temp);
                    (*iter).navigateQueue.pop(); // delete front point
                   // (*iter).navigateQueue.push_back(temp); // insert front point into end 
                }
                /************************* end Navigation *****************/
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
                    (*iter).set_speed(speed);
                    DeleteCar(*iter, carStateSet);
                    // TODO
                    /*
                     * define the target_speed and target_angel
                     * global variable of target_angel
                     * note: target_angel derived from two 2d points,
                     * you can also try to use 3d, but may it has 
                     * subtle error.
                     */
                    pid pid_control;
                    pid_control.controlSpeedAndAngular(*iter);
                }
                carStateSet.push_back((*iter));
                // if the slope is 0, then the car may stay offline
        	    if (slope != 0)
                         car_alive_set.push_back(*iter);
                    iter ++;
            }   
        	currentTime = ros::Time::now();
        	consumeTime = currentTime.toSec() - lastTime.toSec();
        	delay_sec = main_period_sec - consumeTime;
        	if(delay_sec>0){
        	    ros::Duration(delay_sec).sleep();
    	    }
        	currentTime = ros::Time::now();
        	consumeTime = currentTime.toSec() - lastTime.toSec();
        	//cout << "main fps: " << 1/consumeTime << "Hz" << endl;
        	lastTime = currentTime;
            }
            catch(tf2::TransformException& ex)
            {
                    ROS_ERROR("%s", ex.what());
                    ros::Duration(1.0).sleep();
                    continue;
            }
            rate.sleep();
        }
    // send stop command to cars
    /*
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        (*iter).set_target_speed(0);
    }*/
    return 0;
}
