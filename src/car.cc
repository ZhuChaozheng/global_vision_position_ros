#include "car.h"

Car::Car(int marker, string ip, int port, float target_angle,
			float target_velocity, float target_point_x, 
			float target_point_y)
{ 
	marker_ = marker; ip_ = ip; port_ = port;
	target_angle_ = target_angle;
	target_velocity_ = target_velocity;
	target_point_ = Point2f(target_point_x, target_point_y);
}

Car::Car() {}

float convertDegree(double yaw)
{
    if (yaw < 0)
        yaw = yaw + 2 * 3.1415926;
    return 180 * yaw / 3.1415926;
}


void Car::update_parameters(string ip, float target_angle,
			float target_velocity, float target_point_x, 
			float target_point_y)
{
	set_ip(ip);
	set_target_angle(target_angle);
	set_target_velocity(target_velocity);
	Point2f target_point = Point2f(target_point_x, target_point_y);
	set_target_point(target_point);
}


