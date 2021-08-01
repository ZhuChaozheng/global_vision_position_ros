#ifndef PID_H_
#define PID_H_

#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "car.h"
#include "udp.h"

using namespace std;

#define pi acos(-1)

class pid
{
public:
    pid(float slope_p = 0, float slope_i = 0, 
            float slope_d = 0, float speed_p = 0, 
            float speed_i = 0, float speed_d = 0 )
    {
        slope_p_ = slope_p;
        slope_i_ = slope_i;
        slope_d_ = slope_d;
        speed_p_ = speed_p;
        speed_i_ = speed_i;
        speed_d_ = speed_d;
        // i_max_ = abs(i_max);
        rc_ = 1 / (2 * pi * 20);
    }

    void set_slope_pid(float slope_p, float slope_i, 
            float slope_d) { slope_p_ = slope_p; 
            slope_i_ = slope_i; slope_d_ = slope_d; }

    float get_slope_pid(float error, Car &car, float scalar);

    void set_speed_pid(float speed_p, float speed_i, 
            float speed_d) { speed_p_ = speed_p; 
            speed_i_ = speed_i; speed_d_ = speed_d; }

    float get_speed_pid(float error, Car &car, float scalar);

    float get_speed_pid_incre(float error, Car &car, float scalar);

    void controlSpeedAndAngular(Car &car);
    
private:
    float slope_p_;
    float slope_i_;
    float slope_d_;
    float speed_p_;
    float speed_i_;
    float speed_d_;
    float rc_;
    float wp_angel_;
    float wp_move_;
};

#endif // PID_H
