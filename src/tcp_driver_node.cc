#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include "tcp.h"
extern car car_[10];
/**
 * convert the car status to /comm topic
 */
int main(int argc, char **argv)
{
    create_server_and_update_data(); // update struct car[10]
    ros::init(argc, argv, "talker");
    ros::NodeHandle n;
    ros::Publisher chatter_pub = n.advertise<std_msgs::String>(
          "comm", 1000);
    ros::Rate loop_rate(20);    

    while (ros::ok())
    {
        std_msgs::String msg;
        std::stringstream ss;
        for (int i = 0; i <= 9; i++)
            ss << car_[i].sockfd << " " << car_[i].velocity << " " 
                  << car_[i].angular_velocity << " ";
        msg.data = ss.str();
        ROS_INFO("%s", msg.data.c_str());
        chatter_pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
