#include <actionlib/client/simple_action_client.h>
#include "global_vision_position/MoveAction.h"
#include <geometry_msgs/Twist.h>
#include <iostream>

using namespace std;
 
typedef actionlib::SimpleActionClient<global_vision_position::MoveAction> Client;
 
struct Mycar
{
    float x;
    float y;
    float theta;
}car_present_pose, car_current_pose;
 
// 当action完成后会调用该回调函数一次
void doneCb(const actionlib::SimpleClientGoalState& state,
        const global_vision_position::MoveResultConstPtr& result)
{
    ROS_INFO("Yay! The carMove is finished!");
    ros::shutdown();
}
 
// 当action激活后会调用该回调函数一次
void activeCb()
{
    ROS_INFO("Goal just went active");
}
 
// 收到feedback后调用该回调函数
void feedbackCb(const global_vision_position::MoveFeedbackConstPtr& feedback)
{
    ROS_INFO(" present_pose : %f  %f  %f", feedback->present_car_x,feedback->present_car_y,feedback->present_car_theta);
}
 
int main(int argc, char** argv)
{
    ros::init(argc, argv, "Car_Move_client");
 
    // 定义一个客户端
    Client client("carMove", true);
 
    ros::NodeHandle n;

    // 等待服务器端
    ROS_INFO("Waiting for action server to start.");
    client.waitForServer();
    ROS_INFO("Action server started, sending goal.");
    // ros::Subscriber sub = n.subscribe("car1/pose",10,&posecallback); //订阅小乌龟的位置信息
    // sleep(2);5
    // 创建一个action的goal
    global_vision_position::MoveGoal goal;
    ROS_INFO("x = %f", car_current_pose.x);
    goal.car_target_x = 0.8;
    goal.car_target_y = 1.0;
    goal.car_target_theta = 0;
    client.sendGoal(goal, &doneCb, &activeCb, &feedbackCb);
    
    /*double th = 0.0;
    double radius = 1.0 ;
	
    for (double th = -1.57; th < 4.71; th = th + 0.1)
    {
	goal.car_target_x = 5.0 + radius * cos(th);
	goal.car_target_y = 5.0 + radius * sin(th);
	goal.car_target_theta = th;
	client.sendGoal(goal,  &doneCb, &activeCb, &feedbackCb);
	sleep(1);
    }
  */
 
    ros::spin();
 
    return 0;
}