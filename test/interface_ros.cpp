#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{  
  ros::init(argc, argv, "talker");
  ros::NodeHandle n;
  ros::Publisher chatter_pub = n.advertise<std_msgs::String>(
        "comm", 1000);
  ros::Rate loop_rate(2);
  while (ros::ok())
  {
    std_msgs::String msg;
    std::stringstream ss;
   // for(int i = 0; i <= 9; i ++)
	   // point.x point.y speed    slope  stop_flag
    ss << " \
    0 0 0 270 0 \
    0 0 0 270 0 \
    0 0 0 270 0 \
    0 0 0 270 0 \
    0 0 0 270 0 \
    0 0 0 270 0 \
    0 0 0 270 0 \
    0 0 0 0 0 \
    0 0 0 90 0 ";
    ss << "0 0 0 70 0";
    msg.data = ss.str();
    ROS_INFO("%s", msg.data.c_str());
    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    chatter_pub.publish(msg);
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}
