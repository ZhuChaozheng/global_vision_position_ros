# global_vision_position_ros
Now, we are pursuing 2.0, which uses novel architecture and distinguishes it from the first version. In the first version, we design all the control commands in the computer, especially the PID control of each car. As for the second, we fuse the localization with upload data from each car and then publish the topic "/odom_". 

Each car only has a stm32 chip and not any powerful ship. It uploads the status to the core computer and owns an independent PID control system. We also update the localization data combined with the global camera and upload status.

### protocol coming...
