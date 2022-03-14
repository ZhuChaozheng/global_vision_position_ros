#!/usr/bin/env python
# encoding: UTF-8
import rospy
from std_msgs.msg import String
from locale import atof
import queue
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Twist
from positioning.msg import Encoder

imu_msg = Imu()
od_msg = Twist()
encoder_msg = Encoder()

def callback(data):
    #rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)
    car_data = data.data.split(" ")

    for i in range(10):
        stamp = rospy.get_rostime()
        # publish imu data
        imu_msg.header.stamp = stamp
        imu_msg.linear_acceleration.x = atof(car_data[i * 8 + 5])
        imu_msg.linear_acceleration.y = atof(car_data[i * 8 + 6])
        imu_msg.angular_velocity.z = atof(car_data[i * 8 + 7])
        imu_data_pub = rospy.Publisher(
            ('robot_'+str(i+1)+'/imu'), Imu, queue_size=100)
        imu_data_pub.publish(imu_msg)
        # publish odometry
        od_msg.linear.x = atof(car_data[i * 8 + 1])
        od_msg.angular.z = atof(car_data[i * 8 + 2])
        od_data_pub = rospy.Publisher(
            ('robot_'+str(i+1)+'/odometry'), Twist, queue_size=100)
        od_data_pub.publish(od_msg)
        # publish customize message of encoder
        encoder_msg.fr_speed_rpm = atof(car_data[i * 8 + 4])
        encoder_msg.fl_speed_rpm = atof(car_data[i * 8 + 3])
        encoder_data_pub = rospy.Publisher(
            ('robot_'+str(i+1)+'/encoder'), Encoder, queue_size=100)
        encoder_data_pub.publish(encoder_msg)
    
    
def listener():

    rospy.init_node('listener', anonymous=True)

    rospy.Subscriber("comm", String, callback)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
