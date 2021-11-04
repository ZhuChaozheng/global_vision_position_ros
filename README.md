# global_vision_position_ros
Now, we are pursuing 2.0, which uses novel architecture and distinguishes it from the first version. In the first version, we design all the control commands in the computer, especially the PID control of each car. As for the second, we fuse the localization with upload data from each car and then publish the topic "/odom_". 

Each car only has a stm32 chip and not any powerful ship. It uploads the status to the core computer and owns an independent PID control system. We also update the localization data combined with the global camera and upload status.

### protocol coming...
Publisher: 

planning.cc /marker_0/cmd\_vel|/marker\_1/cmd\_vel...|/marker\_9/cmd\_vel

odometry\_pub.c  /odom\_0|/odom\_1|/odom\_2...|/odom\_9

tcp\_driver\_node.cc /comm

Receiver:

tcp\_driver\_node.cc /marker\_0/cmd\_vel|/marker\_1/cmd\_vel...|/marker\_9/cmd\_vel
Attention: in the topic of /marker\_0/cmd\_vel, linear.x is equal to the velocity, angular.z is equal to the angluar velocity.

planning.cc  /odom\_0|/odom\_1|/odom\_2...|/odom\_9

odometry\_pub.cc /comm

car layer:
upload
Frame Head id length status linear velocity speed'y angluar velocity
---------- -- ------ ------ --------------- ------- ----------------- -- --
7B   7B    ID   1B   status     H      L      H   L      H      L

command
Frame Head
----------
7D    7A
