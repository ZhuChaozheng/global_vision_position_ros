/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 *
 *
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *            佛祖保佑       永不宕机     永无BUG
 */

#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <stdio.h>
#include <tf/transform_listener.h>

#include <iostream>

#include "boid_model.h"
#include "car.h"
#include "ros/ros.h"

using namespace std;

/*
 * define data space
 */
int boid_num = 2;
int waypoint_num = 4;
extern float *pos_x_array = new float[boid_num];
extern float *pos_y_array = new float[boid_num];
extern float *vel_x_array = new float[boid_num];
extern float *vel_y_array = new float[boid_num];
extern float *vel_x_matrix = new float[boid_num];
extern float *vel_y_matrix = new float[boid_num];
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
void robotOdomCallback(const nav_msgs::OdometryConstPtr &locator, int marker) {
  int k = marker;
  nav_msgs::Odometry robotOdometryMsg = *locator;
  if (k == 0) {
    ROS_INFO("marker: %d, robot Position: %f, %f", k,
             robotOdometryMsg.pose.pose.position.x,
             robotOdometryMsg.pose.pose.position.y);
    ROS_INFO("marker: %d, robot Heading: linear:%f, angular:%f", k,
             robotOdometryMsg.twist.twist.linear.x,
             robotOdometryMsg.twist.twist.angular.z);
  }
  pos_x_array[k] = robotOdometryMsg.pose.pose.position.x;
  pos_y_array[k] = robotOdometryMsg.pose.pose.position.y;
  vel_x_array[k] = robotOdometryMsg.twist.twist.linear.x;
  //   cout << "car_ID = " << k << " vel: " << vel_x_array[k] << endl;
  //   cout << "before angle:" <<
  //   tf::getYaw(robotOdometryMsg.pose.pose.orientation)
  //        << endl;
  vel_y_array[k] = convertDegree(tf::getYaw(
      robotOdometryMsg.pose.pose.orientation));  // vel_y -> yaw(0-360)
  //   cout << "after angleL:" << vel_y_array[k] << endl;
}

void *sub_spin(void *args) {
  // create ros node handle
  ros::NodeHandle nh;
  vector<ros::Subscriber> odom_sub_set_;
  for (int i = 0; i <= 2; i++) {
    string front_str = "robot_";
    string end_str = "/pose";
    stringstream ss;
    // construct topic '/marker1/cmd_vel'
    ss << front_str << i << end_str;
    string topic = ss.str();
    ros::Subscriber odom_sub_ = nh.subscribe<nav_msgs::Odometry>(
        topic, 1, boost::bind(&robotOdomCallback, _1, i));

    odom_sub_set_.push_back(odom_sub_);
  }
  ros::spin();
  return 0;
}

int main(int argc, char **argv) {
  // unit: cm
  // Boids Swarm Paras Setting
  float border_x = 1.8, border_y = 1.5, coeff_vel = 2.0;
  float border_x_low = 0.0, border_x_high = 2.0, border_y_low = 0.0,
        border_y_high = 2.0;
  float sep_dist = 0.0, coh_dist = 2.5, avoid_border_dist = 0.0,
        avoid_obs_dist = 0.075;
  int *wp_ids, *next_wp_ids;
  float *weight_coeff;
  float *eva_scores;
  wp_ids = (int*)malloc(boid_num*sizeof(int));
  next_wp_ids = (int*)malloc(boid_num*sizeof(int)); 
  weight_coeff=(float*)malloc(7*sizeof(float)); 
  eva_scores=(float*)malloc(2*sizeof(float)); 

  //weight setting 
  weight_coeff[0] = 2.0;
  weight_coeff[1] = 1.5;
  weight_coeff[2] = 1.0;
  weight_coeff[3] = 1.0;
  weight_coeff[4] = 0.75;
  weight_coeff[5] = 1.5;
  weight_coeff[6] = 1.5;

  // waypoints setting
  float *wp_x = new float[waypoint_num];
  float *wp_y = new float[waypoint_num];
  int wp_id = 1;
  wp_x[0] = 0.2;
  wp_y[0] = 0.2;
  wp_x[1] = 1.6;
  wp_y[1] = 0.2;
  wp_x[2] = 1.6;
  wp_y[2] = 1.6;
  wp_x[3] = 0.2;
  wp_y[3] = 1.6;

  // obstacles setting
  int ob_num = 0;
  float ob_pos_x, ob_pos_y;
  float *ob_pos_x_array = new float[ob_num];
  float *ob_pos_y_array = new float[ob_num];

  for (int j = 0; j < ob_num; j++) {
    ob_pos_x = 1.1;
    ob_pos_y = 0.75;
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
  if (ret_sub_spin != 0) {
    cout << "pthread_create error at ret_udp: error_code=" << ret_sub_spin
         << endl;
  }
  sleep(2);
  // ******* main thread ********
  ros::Publisher vel_pub_;
  vector<ros::Publisher> vel_pub_set_;
  for (int i = 0; i < boid_num; i++) {
    string front_str = "robot_";
    string end_str = "/cmd_vel";
    stringstream ss;
    // construct topic '/marker1/cmd_vel'
    ss << front_str << i << end_str;
    string topic = ss.str();
    vel_pub_ = n.advertise<geometry_msgs::Twist>(topic, 30);
    vel_pub_set_.push_back(vel_pub_);
  }
  // loop
  int index = 0;
  bool abandon_flag = false;

  // modified by lq
  // malloc memory for current angles of boid
  float *boid_current_angle;
  boid_current_angle = (float *)malloc(boid_num * sizeof(float));

  while (n.ok()) {
    printf("boid_num:%d\n", boid_num);
    srand(time(NULL));
    float current_angle, vel_x, vel_y;
    printf("========================step:%d==============\n", index);
    for (int k = 0; k < boid_num; k++) {
      if (vel_x_array[k] < 0) {
        abandon_flag = true;
        break;
      }

      // unit: deg
      current_angle = vel_y_array[k];

      // convert -pi to pi
      if (current_angle > 180.0) current_angle -= 360;

      boid_current_angle[k] = current_angle;

      // convert unit: deg to rad
      current_angle *= 3.1415926 / 180.0;

      // calc vel x and y vector ???
      // note: ros vel not constant!!!
      // convert vel unit: m/s to cm/s
      printf("ros vel:%f\n", vel_x_array[k]);
      vel_x_matrix[k] = vel_x_array[k] * sin(current_angle);
      vel_y_matrix[k] = vel_x_array[k] * cos(current_angle);

      if (index > 0) {
        printf("id:%d\n", k);

        printf("pos:[%f,%f]\n", pos_x_array[k], pos_y_array[k]);

        // printf("tar_pos:[%f,%f]\n", tar_pos_x_array[k], tar_pos_y_array[k]);

        // printf("ros angel:%f, control angel:%f\n", vel_y_array[k],
        //        current_angle);

        // printf("angel:%f\n", current_angle);
        ROS_INFO("Current angle: %f", current_angle * 180 / 3.1415926);
        cout << "sin: " << sin(current_angle) << "cos: " << cos(current_angle)
             << endl;

        printf("vel:[%f,%f]\n", vel_x_matrix[k], vel_y_matrix[k]);
      }

      if (pos_x_array[k] < border_x_low || pos_x_array[k] > border_x_high ||
          pos_y_array[k] < border_y_low || pos_y_array[k] > border_y_high) {
        std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
                     "<<<<<<< \n"
                  << std::endl;
      }
    }
    if (abandon_flag) {
      abandon_flag = false;
      rate.sleep();
      continue;
    }

    std::cout << "before wp_id = " << wp_id << std::endl;

    // !!!!!!!!!!!!!!! record pos_x_array pos_y_array vel_x_matrix vel_y_matrix
    // current_angle out_theta_cmd twist.linear.x twist.angular.z
    getFlockVelCmd4PathPlanning(boid_num, border_x_low, border_x_high, 
        border_y_low, border_y_high,
        weight_coeff, sep_dist, coh_dist,  avoid_obs_dist, avoid_border_dist,
        waypoint_num, wp_x, wp_y,
        pos_x_array, pos_y_array, 
        ob_num, ob_pos_x_array, ob_pos_y_array, vel_x_array, vel_y_array,
        out_cmd_vel, out_theta_cmd, eva_scores, wp_ids, next_wp_ids);

    // copy next_wp_ids to wp_ids
    int i=0;
    for (i = 0; i < boid_num; i++) 
    {
        wp_ids[i] = next_wp_ids[i];
    }

    std::cout << "after wp_id = " << wp_id << std::endl;

    index++;
    i = 0;
    float sign;
    for (auto vel_pub = vel_pub_set_.begin(); vel_pub != vel_pub_set_.end();) {
      float angular_speed;
      // // theta range 0 to 360.0
      out_theta_cmd[i] = out_theta_cmd[i] * 180.0 / 3.1415926;
      if (out_theta_cmd[i] < 0.0) {
        out_theta_cmd[i] = 360.0 + out_theta_cmd[i];
      }

      float delta_theta = out_theta_cmd[i] - boid_current_angle[i];

      std::cout << "current angle = " << boid_current_angle[i] << std::endl;
      std::cout << "expect angle = " << out_theta_cmd[i] << std::endl;

      //   ROS_INFO("before delta_theta = %f", delta_theta);
      if (delta_theta > 180.0) delta_theta -= 360.0;
      if (delta_theta < -180.0) delta_theta += 360.0;
      //   ROS_INFO("after delta_theta = %f", delta_theta);
      float kk = -1.0;  // 负号表示改变方向。 x正方向-->y正方向为负方向。
      angular_speed = kk * delta_theta * 3.1415926 / 180.0;
      geometry_msgs::Twist twist;
      // c

      if (angular_speed > 1.0) {
        angular_speed = 1.0;
      }
      if (angular_speed < -1.0) {
        angular_speed = -1.0;
      }
      printf("ctrl_vel and angle:[%f,%f]\n", out_cmd_vel[i],
             angular_speed);

      // let car move first
      if (index <= 1) {
          twist.linear.x = 0.2;
          twist.linear.y = 0.0;
          twist.angular.z = 0.0;
      } else  // swarm control
      {
   
          twist.linear.x = out_cmd_vel[i];
          twist.linear.y = 0.0;
          twist.angular.z = angular_speed;
      }

      (*vel_pub).publish(twist);
      vel_pub++;
      i++;
      // }
    }
    rate.sleep();
  }

  // free memory
  free(pos_x_array);
  free(pos_y_array);
  free(ob_pos_x_array);
  free(ob_pos_y_array);
  free(vel_x_array);
  free(vel_y_array);
  free(wp_x);
  free(wp_y);
  free(eva_scores);
  free(wp_ids);
  free(next_wp_ids);
  
  delete out_cmd_vel;
  delete out_theta_cmd;


  return 0;
}
