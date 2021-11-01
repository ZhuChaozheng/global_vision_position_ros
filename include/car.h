/**
 * created by Jason 
 * 2020.12.15
 * class car
 */

#ifndef CAR_H
#define CAR_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

class Car
{
public:
	int marker_; // marker
	// medianPoint is the center point of circumcircle
	Point2f medianPoint_ = Point2f(0,0); 
	float angle_ = 0; // angle
	float velocity_ = 0; // accumulated x y
	Point2f target_point_ = Point2f(0, 0); // target 2d point
	float target_angle_; // target angle
	float target_velocity_; // target velocity
	string ip_;
	int port_;
    queue<Point2f> navigateQueue;
    bool init_angle_flag_ = true;;
    float stop_flag_ = 0;
    float cache_angle_array[10];
    float angular_velocity_; // angular velocity, yaw

public:
	Car(int marker, string ip, int port, float target_angle,
			float target_velocity, float target_point_x, 
			float target_point_y);
	Car();
	
	void update_parameters(string ip, float target_angle,
			float target_velocity, float target_point_x, 
			float target_point_y);
	void update_parameters_angle_velocity(float target_angle,
			float target_velocity);
	//void stop_car(Car &car);
//	void stop_cars(vector<Car> &car_set);

	void set_median_point(Point2f center) { medianPoint_ = center; }
	void set_angle(float angle) { angle_ = angle; }
	void set_velocity(float velocity) { velocity_ = velocity; }
	void set_target_point(Point2f point) { target_point_ = point; }
	void set_target_angle(float target_angle) 
			{ target_angle_ = target_angle; }
	void set_target_velocity(float target_velocity) 
			{ target_velocity_ = target_velocity; }
	void set_ip(string ip) { ip_ = ip; }
	void set_angular_velocity(float angular_velocity) 
			{ angular_velocity_ = angular_velocity; }

	int get_marker() { return marker_; }
	string get_ip() { return ip_; }
	Point2f get_median_point() { return medianPoint_; }
	float get_angle() { return angle_; }
	float get_velocity() { return velocity_; }
	float get_angular_velocity() { return angular_velocity_; }
	Point2f get_target_point() { return target_point_; }
	float get_target_angle() { return target_angle_; }
	float get_target_velocity() { return target_velocity_; }
};
float convertDegree(double yaw);

#endif //CAR_H
