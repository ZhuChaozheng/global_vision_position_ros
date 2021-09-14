#ifndef BOID_MODEL_H 
#define BOID_MODEL_H                              

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <cstdio>
#include <iostream>
#include<stdio.h>
typedef struct 
{
    float x;
    float y;
}PVector;

PVector vec_add(PVector vec1, PVector vec2);

PVector vec_sub(PVector vec1, PVector vec2);

PVector vec_mult(PVector vec, float coeff);

PVector vec_limit(PVector vec, float scale);

PVector vec_normalize(PVector vec);

float vec_mag(PVector vec);

float vec_dist(PVector vec1, PVector vec2);



float Uniform_Rand(float a, float b);

class Obstacle
{
    public:
        PVector location;
        float r;
        float steps;
        
        Obstacle(float, float, float);

        void run(void);
};

class Boid {
    public:
        PVector location;    
        PVector velocity;
        PVector acceleration;
        PVector tar_loc;
        PVector tar_vel;
        int id;
        float r;
        float maxforce;    // Maximum steering force
        float maxspeed;    // Maximum speed
        bool IsLeader;    // Leader yes or no

        Boid(float, float, int, float, float);

        void run(std::vector<Boid> boids, std::vector<Obstacle> ob_list);
        void run2(std::vector<Boid> boids, std::vector<Obstacle> ob_list);
        void step(std::vector<Boid> boids, std::vector<Obstacle> ob_list, float* line_vel, float* angle);

        void applyForce(PVector force);

        void flock(std::vector<Boid> boids, std::vector<Obstacle> ob_list);

        void update(void);

        PVector seek(PVector target);

        PVector virtual_leader(std::vector<Boid> boids);

        PVector border_horizontal_fear(void);

        PVector border_vertical_fear(void);

        PVector obstacle_fear(std::vector<Obstacle> ob_list);

        PVector separate(std::vector<Boid> boids);

        PVector align(std::vector<Boid> boids);

        PVector cohesion(std::vector<Boid> boids);

        PVector get_pos(void);

        void set_leader(int);

};

class Flock
{
    public:
        std::vector<Boid> boids;
        std::vector<Obstacle> ob_list;

        void run(void);
        void addBoid(Boid b);
        void addObstacle(Obstacle ob);
        std::vector<PVector> getBoidState(void);
};



#ifdef __cplusplus  
extern "C"{ 
    void getFlockCmd(int boids_num, float* pos_x, float* pos_y, int obstacle_num, float* ob_pos_x, float* ob_pos_y, float* line_vel_array, float* angle_array);
    void getFlockVel(int boids_num, float* pos_x, float* pos_y, 
    float* tar_pos_x, float* tar_pos_y, float* tar_vel_x, float* tar_vel_y, int obstacle_num, float* ob_pos_x, float* ob_pos_y, float* out_vel_x, float* out_vel_y);

    //接口函数
    //输入:   
    //        boids_num-机器人数量
    //        pos_x-所有机器人的位置(x坐标)(单位cm)
    //        pos_y-所有机器人的位置(y坐标)
    //        tar_pos_x-所有机器人的目的地位置(x坐标)
    //        tar_pos_y-所有机器人的目的地位置(y坐标)
    //        tar_vel_x-所有机器人的接近目的地的速度(x坐标)(单位 cm/s)
    //        tar_vel_y-所有机器人的接近目的地的速度(y坐标)(单位 cm/s)
    //        obstacle_num-虚拟障碍物的数量
    //        ob_pos_x-所有障碍物的位置(x坐标)
    //        ob_pos_y-所有障碍物的位置(y坐标)
    //        vel_x-所有机器人的速度(x坐标)
    //        vel_x-所有机器人的速度(y坐标)
    
    //输出:
    //        vel_cmd-所有机器人下一步长所要控制的线速度(单位 cm/s)
    //        theta_cmd-所有机器人下一步长所要控制的角度(单位 °) 以正北为方向顺时针旋转，正北为0度，范围[-pi,pi]，需要转换角度
        
    void getFlockVelCmd(int boids_num, float* pos_x, float* pos_y, 
                float* tar_pos_x, float* tar_pos_y, float* tar_vel_x, float* tar_vel_y,
                int obstacle_num, float* ob_pos_x, 
                float* ob_pos_y, float* vel_x, float* vel_y,
                float* vel_cmd, float* theta_cmd);
                

}
#endif

#endif