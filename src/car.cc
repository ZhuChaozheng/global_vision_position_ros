#include "car.h"

Car::Car(int marker, float slope_P, float slope_I, 
		float slope_D, float speed_P, float speed_I, 
		float speed_D, string ip, int port, float target_slope,
		float target_speed, float target_point_x, 
		float target_point_y)
{ 
	marker_ = marker; slope_p_ = slope_P; 
	slope_i_ = slope_I; slope_d_ = slope_D; 
	speed_p_ = speed_P; speed_i_ = speed_I; 
	speed_d_ = speed_D; ip_ = ip; port_ = port;
	target_slope_ = target_slope;
	target_speed_ = target_speed;
	target_point_ = Point2f(target_point_x, target_point_y);
}

Car::Car() {}

void Car::update_parameters(float slope_P, float slope_I, 
	float slope_D, float speed_P, float speed_I, 
	float speed_D, string ip, float target_slope,
	float target_speed, float target_point_x, 
	float target_point_y)
{
	set_slope_p(slope_P);
	set_slope_i(slope_I);
	set_slope_d(slope_D);
	set_speed_p(speed_P);
	set_speed_i(speed_I);
	set_speed_d(speed_D);
	set_ip(ip);
	set_target_slope(target_slope);
	set_target_speed(target_speed);
	Point2f target_point = Point2f(target_point_x, target_point_y);
	set_target_point(target_point);
}

