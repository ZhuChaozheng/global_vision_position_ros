#include <iostream>
#include "boid_model.h"
#include <stdio.h>
#include "ros/ros.h"
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_listener.h>
#include "car.h"

using namespace std;

/*
 * define data space
 */
int boid_num = 1;
extern float *pos_x_array = new float[boid_num];
extern float *pos_y_array = new float[boid_num];
extern float *vel_x_array = new float[boid_num];
extern float *vel_y_array = new float[boid_num];
extern bool modi_flag = true;

/*
 * define output space
 *
 */
float *out_cmd_vel = new float[boid_num];
float *out_theta_cmd = new float[boid_num];

/* 
 * call back function
 */
void robotOdomCallback(const nav_msgs::OdometryConstPtr& locator, int marker)
{
        int k = marker;
        nav_msgs::Odometry robotOdometryMsg = *locator;
        if (k == 0)
        {
            ROS_INFO("marker: %d, robot Position: %f, %f", k, robotOdometryMsg.pose.pose.position.x,
                 robotOdometryMsg.pose.pose.position.y);
            ROS_INFO("marker: %d, robot Heading: linear:%f, angular:%f", k, robotOdometryMsg.twist.twist.linear.x,
                 robotOdometryMsg.twist.twist.angular.z);    
        }   
        pos_x_array[k] = robotOdometryMsg.pose.pose.position.x * 100;
        pos_y_array[k] = robotOdometryMsg.pose.pose.position.y * 100;
        vel_x_array[k] = robotOdometryMsg.twist.twist.linear.x * 100;
        cout << "%d" << k << " vel: " << vel_x_array[k] << endl;
        cout << "before angle:" << tf::getYaw(robotOdometryMsg.pose.pose.orientation) << endl;
        vel_y_array[k] = convertDegree(tf::getYaw(robotOdometryMsg.pose.pose.orientation));  // vel_y -> yaw(0-360)  
        cout << "after angleL:" << vel_y_array[k] << endl;  
        
}

void* sub_spin(void* args) 
{
    // create ros node handle
    ros::NodeHandle nh;   
    vector<ros::Subscriber> odom_sub_set_;
    for (int i = 0; i <= 2; i ++)
    {
        string front_str = "/odom_";
        stringstream ss;
        // construct topic '/marker1/cmd_vel'
        ss << front_str << i;
        string topic = ss.str();
        ros::Subscriber odom_sub_ = nh.subscribe<nav_msgs::Odometry>(topic, 
                1, boost::bind(&robotOdomCallback, _1, i));

        odom_sub_set_.push_back(odom_sub_);
    }
    ros::spin();
    return 0;
}

int main(int argc, char** argv) 
{

    float *tar_pos_x_array = new float[boid_num];
    float *tar_pos_y_array = new float[boid_num];
    float *tar_vel_x_array = new float[boid_num];
    float *tar_vel_y_array = new float[boid_num];
    float border_x=180.0, border_y=150.0, coeff_vel=2.0;
    float border_x_low=-12.0, border_x_high=222.0, border_y_low=0.0, border_y_high=187.0;
    float sep_dist = 60.0, coh_dist = 250.0, avoid_border_dist = 30.0, avoid_obs_dist=7.5;


    /*
     * define target information
     *
     */
    for (int i = 0; i <= 1; i ++)
    {
        tar_pos_x_array[i] = border_x_high;
        tar_pos_y_array[i] = 0.5*(border_y_low+border_y_high);
        tar_vel_x_array[i] = 7.5*coeff_vel;
        tar_vel_y_array[i] = 0;
    }
    /*
     * define obtacles
     */
    int ob_num = 0;
    float ob_pos_x, ob_pos_y;
    float *ob_pos_x_array = new float[ob_num];
    float *ob_pos_y_array = new float[ob_num];

    for(int j = 0; j < ob_num; j ++)
    {
        ob_pos_x = 110.0;
        ob_pos_y = 75.0;        
        ob_pos_x_array[j] = ob_pos_x;
        ob_pos_y_array[j] = ob_pos_y;
    }

    ros::init(argc, argv, "planning");
    // create ros node handle
    ros::NodeHandle n;
    int rate_hz = 10;
    ros::Rate rate(rate_hz);  
    
    // ************ udp control interface thread *********
    pthread_t tids[1];
    int ret_sub_spin = pthread_create(&tids[0], NULL, sub_spin, NULL);
    if (ret_sub_spin != 0)
    {
        cout << "pthread_create error at ret_udp: error_code=" << ret_sub_spin << endl;
    }
    sleep(2);
    // ******* main thread ********
    ros::Publisher vel_pub_;
    vector<ros::Publisher> vel_pub_set_;
    for (int i = 0; i < boid_num; i ++)
    {
        string front_str = "/marker";
        string end_str = "/cmd_vel";
        stringstream ss;
        // construct topic '/marker1/cmd_vel'
        ss << front_str << i << end_str;
        string topic = ss.str();
        vel_pub_ = n.advertise<geometry_msgs::Twist>(topic, 30);
        vel_pub_set_.push_back(vel_pub_);
    }
    // loop
    int index=0;
    bool abandon_flag = false;

    // modified by lq
    float *boid_current_angle;
    boid_current_angle=(float*)malloc(boid_num*sizeof(float)); 

    while(n.ok())
    {
        printf("boid_num:%d\n",boid_num);
        srand(time(NULL));
        float current_angle, vel_x, vel_y;
        printf("========================step:%d==============\n", index);
        for(int k=0; k<boid_num; k++)
        {            
            if(vel_x_array[k] < 0)
            {
                abandon_flag = true;
                break;
            }
            if(pos_x_array[k]>(border_x_high-avoid_border_dist))
            {
                tar_pos_x_array[k] = border_x_low;
                tar_pos_y_array[k] = 0.5*(border_y_low+border_y_high);
                tar_vel_x_array[k] = -7.5*coeff_vel;
                tar_vel_y_array[k] = 0.0;
            }

            if(pos_x_array[k]<(border_x_low+avoid_border_dist))
            {
                tar_pos_x_array[k] = border_x_high;
                tar_pos_y_array[k] = 0.5*(border_y_low+border_y_high);;
                tar_vel_x_array[k] = 7.5*coeff_vel;
                tar_vel_y_array[k] = 0.0;
            }

          

            if ((vel_y_array[k]>=0.0)&&(vel_y_array[k]<180.0))
                current_angle = -vel_y_array[k];
            else
                current_angle = 360.0 - vel_y_array[k];
            
            boid_current_angle[k] = current_angle;
            
            
            current_angle *= 3.1415926/180.0;
            
            vel_x = vel_x_array[k]*sin(current_angle);
            vel_y = vel_x_array[k]*cos(current_angle);
                            // random speed
            vel_x_array[k] = vel_x;
            vel_y_array[k] = vel_y;


            if(index>0)
            {
                printf("id:%d\n",k);

                printf("pos:[%f,%f]\n", pos_x_array[k], pos_y_array[k]);

                printf("tar_pos:[%f,%f]\n", tar_pos_x_array[k], tar_pos_y_array[k]);

                printf("ros angel:%f, control angel:%f\n", vel_y_array[k], current_angle);

                printf("ros vel:%f\n", vel_x_array[k]);

                printf("angel:%f\n", current_angle);

                cout << "sin: " << sin(current_angle) << "cos: " << cos(current_angle) << endl;

                printf("vel:[%f,%f]\n", vel_x_array[k], vel_y_array[k]);

                printf("tar_vel:[%f,%f]\n", tar_vel_x_array[k], tar_vel_y_array[k]);

            }

            if(pos_x_array[k]<border_x_low || pos_x_array[k] >border_x_high || pos_y_array[k] < border_y_low || pos_y_array[k] > border_y_high)
            {
                std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n" << std::endl;
            }
            
        }
        if (abandon_flag)
        {
            abandon_flag = false;
            rate.sleep();
            continue;
        }
            // getFlockVelCmd(boid_num, pos_x_array, pos_y_array, 
         //                tar_pos_x_array, tar_pos_y_array, tar_vel_x_array, tar_vel_y_array,
         //                ob_num, ob_pos_x_array, ob_pos_y_array, vel_x_array, vel_y_array,
         //                out_cmd_vel, out_theta_cmd);
        
        // getFlockVelCmdBorder(boid_num, border_x, border_y, coeff_vel, 
        //     sep_dist, coh_dist, avoid_obs_dist, avoid_border_dist,
        //     pos_x_array, pos_y_array, 
        //     tar_pos_x_array, tar_pos_y_array, tar_vel_x_array, tar_vel_y_array,
        //     ob_num, ob_pos_x_array, ob_pos_y_array, vel_x_array, vel_y_array,
        //     out_cmd_vel, out_theta_cmd);
        
        getFlockVelCmdBorderDef(boid_num, border_x_low, border_x_high, 
            border_y_low, border_y_high,coeff_vel, 
            sep_dist, coh_dist, avoid_obs_dist, avoid_border_dist,
            pos_x_array, pos_y_array, 
            tar_pos_x_array, tar_pos_y_array, tar_vel_x_array, tar_vel_y_array,
            ob_num, ob_pos_x_array, ob_pos_y_array, vel_x_array, vel_y_array,
            out_cmd_vel, out_theta_cmd);
            // convert out_cmd_vel, out_theta_cmd to /cmd_vel
            // for(int i = 0, auto vel_pub = vel_pub_set_.begin(); 
        //           vel_pub != vel_pub_set_.end();)
        //return 0;

        // for(int l=0; l<boid_num; l++)
        // {
        //     out_cmd_vel[l]=100.0;
        //     out_theta_cmd[l]=270.0;
        // }          
        index++;
        int i = 0;
        float sign;
        for(auto vel_pub = vel_pub_set_.begin(); 
                vel_pub != vel_pub_set_.end();)  
        {
            // if (out_cmd_vel[i] == 0)
            // {
            //     vel_pub ++;
            //     i ++;
            //     continue;
            // }
            // else
            // {
                // if (out_theta_cmd[i] <= 0)
                //     out_theta_cmd[i] = -out_theta_cmd[i];
                // else
                //     out_theta_cmd[i] = 360.0 - out_theta_cmd[i];
                //**************************************************************************//
                // float angular_speed;
                // // theta range 0 to 360.0
                // if(out_theta_cmd[i] < 0.0)
                // {
                //     out_theta_cmd[i] = 360.0 + out_theta_cmd[i];
                // }
                // if(boid_current_angle[i] < 0.0)
                // {
                //     boid_current_angle[i] = 360.0 + boid_current_angle[i];
                // }
                printf("---------------------------------------------------------------------------------\n");
                // float delta_theta = out_theta_cmd[i] -  boid_current_angle[i];

                // ROS_INFO("before delta_theta = %f",delta_theta);
                // if(delta_theta>180.0)
                //     delta_theta -= 360.0;
                // if(delta_theta<-180.0)
                //     delta_theta += 360.0;
                // ROS_INFO("after delta_theta = %f",delta_theta);

                // angular_speed = 1.0 * delta_theta * rate_hz * 3.1415926 /180.0;
                geometry_msgs::Twist twist;
                //c
                
                // if(angular_speed > 1.0)
                // {
                //     angular_speed = 1.0;
                // }
                // if(angular_speed < -1.0)
                // {
                //     angular_speed = -1.0;
                // }
                // printf("ctrl_vel and angle:[%f,%f]\n",out_cmd_vel[i]/100.0, angular_speed);

                //***********************************************************//

                // twist.linear.x = out_cmd_vel[i]/100.0;
                // if(pos_x_array[i]<150)
                // {
                //     twist.linear.x = 0.2;
                //     twist.linear.y = 0.0;
                //     twist.angular.z = 0.0;
                // }
                // else
                // {
                //     twist.linear.x = 0.2;
                //     twist.linear.y = 0.0;
                //     twist.angular.z = 1.0;  
                // }

                // if(pos_x_array[i]>160)
                // {
                //     twist.linear.x = 0.0;
                //     twist.linear.y = 0.0;
                //     twist.angular.z = 0.05*3.1415926;
                // }
                if(index<=70)
                {
                    twist.linear.x = 0.2;
                    twist.linear.y = 0.0;
                    twist.angular.z = 0.0;
                }
                else
                {
                    // if(index>=71)
                    // {
                    //     twist.linear.x = 0.2;
                    //     twist.linear.y = 0.0;
                    //     twist.angular.z = 0.05*3.1415926;
                    // }
                    // if(index>76)
                    // {
                    //     twist.linear.x = 0.2;
                    //     twist.linear.y = 0.0;
                    //     twist.angular.z = 0.0;
                    // }
                    if(int(index/10)%2==0)
                        sign =1.0;
                    else
                        sign = -1.0;
                    ROS_INFO("sign = %f",sign);
                    printf("curr_ang: %f, \n",boid_current_angle[i]);
                    twist.linear.x = 0.2;
                    twist.linear.y = 0.0;
                    twist.angular.z = sign;

                    // twist.linear.x =out_cmd_vel[i]/100.0;
                    // twist.linear.y = 0.0;
                    // twist.angular.z = angular_speed;

                }
            
                // convert orientation

                
                // twist.angular.z = out_theta_cmd[i];
                // if (i == 2)
                //     cout << "out_cmd_vel: " << out_cmd_vel[i] << "out_theta_cmd: " << out_theta_cmd[i] << endl;
                (*vel_pub).publish(twist);
                vel_pub ++;
                i ++;
            // }            
        }
        rate.sleep();
    }
    return 0;
}