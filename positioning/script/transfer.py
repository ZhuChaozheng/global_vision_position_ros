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


# imu_data_pub = rospy.Publisher(
#     ('robot_'+str(i+1)+'/imu'), Imu, queue_size=100)
# od_data_pub = rospy.Publisher(
#     ('robot_'+str(i+1)+'/odometry'), Twist, queue_size=100)
# encoder_data_pub = rospy.Publisher(
    # ('robot_'+str(i+1)+'/encoder'), Encoder, queue_size=100)
imu_data_pub = rospy.Publisher(
    ('robot_1/imu'), Imu, queue_size=100)
od_data_pub = rospy.Publisher(
    ('robot_1/odometry'), Twist, queue_size=100)
encoder_data_pub = rospy.Publisher(
    ('robot_1/encoder'), Encoder, queue_size=100)

imu_msg = Imu()
od_msg = Twist()
encoder_msg = Encoder()

def callback(data):
    rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)
    car_data = data.data.split(" ")

# 创建一个Publisher，发布名为/imu_data的topic，消息类型为laneline_publisher::laneline，队列长度10
    i = 0
    stamp = rospy.get_rostime()
    imu_msg.header.stamp = stamp
    imu_msg.linear_acceleration.x = atof(car_data[i*7+4])
    imu_msg.linear_acceleration.y = atof(car_data[i*7+5])
    imu_msg.angular_velocity.z = atof(car_data[i*7+6])

    od_msg.linear.x = atof(car_data[i*7])
    od_msg.angular.z = atof(car_data[i*7+1])

    encoder_msg.fr_speed_rpm = atof(car_data[i*7+3])
    encoder_msg.fl_speed_rpm = atof(car_data[i*7+2])
    
    
    
def listener():

    # In ROS, nodes are uniquely named. If two nodes with the same
    # name are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'listener' node so that multiple listeners can
    # run simultaneously.

    rospy.init_node('listener', anonymous=True)

    rospy.Subscriber("comm", String, callback)
    # 设置循环的频率
    rate = rospy.Rate(15)
    while not rospy.is_shutdown():
        imu_data_pub.publish(imu_msg)
        # rospy.loginfo(imu_msg)
        od_data_pub.publish(od_msg)
        # rospy.loginfo(od_msg)
        encoder_data_pub.publish(encoder_msg)
        # rospy.loginfo(encoder_msg)

        rate.sleep()


    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
