#!/usr/bin/env python
# license removed for brevity

from locale import atof
import queue
import string
from threading import local
from tokenize import String
import rospy
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Twist
from transfer.msg import Encoder


def trans_call_back(data):
    # ROS节点初始化
    car_data = data.data.split("")

# 创建一个Publisher，发布名为/imu_data的topic，消息类型为laneline_publisher::laneline，队列长度10
    for i in range(11):
        imu_data_pub = rospy.Publisher(
            ('robot_'+str(i+1)+'/imu'), Imu, queue_size=100)
        od_data_pub = rospy.Publisher(
            ('robot_'+str(i+1)+'/odometry'), Twist, queue_size=100)
        encoder_data_pub = rospy.Publisher(
            ('robot_'+str(i+1)+'/encoder'), Encoder, queue_size=100
        )
        # 设置循环的频率
        rate = rospy.Rate(15)
        while not rospy.is_shutdown():
            imu_msg = Imu()
            od_msg = Twist()
            encoder_msg = Encoder()

            stamp = rospy.get_rostime()
            imu_msg.header.stamp = stamp
            imu_msg.linear_acceleration.x = atof(car_data[i*7+4])
            imu_msg.linear_acceleration.y = atof(car_data[i*7+5])
            imu_msg.angular_velocity.z = atof(car_data[i*7+6])

            od_msg.header.stamp = stamp
            od_msg.linear.x = atof(car_data[i*7])
            od_msg.angular.z = atof(car_data[i*7+1])

            encoder_msg.fr_speed_rpm = atof(car_data[i*7+3])
            encoder_msg.fl_speed_rpm = atof(car_data[i*7+2])
            

        imu_data_pub.publish(imu_msg)
        rospy.loginfo(imu_msg)
        od_data_pub.publish(od_msg)
        rospy.loginfo(od_msg)
        encoder_data_pub.publish(encoder_msg)
        rospy.loginfo(encoder_msg)


def car_subscriber():
    # ROS节点初始化
    rospy.init_node('car_subscriber', anonymous=True)

# 创建一个Subscriber，订阅名为comm的topic，注册回调函数trans
    rospy.Subscriber("comm", String, trans_call_back)

# 循环等待回调函数
    rospy.spin()


if __name__ == '__main__':
    car_subscriber()
