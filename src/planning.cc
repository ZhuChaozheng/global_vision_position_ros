




/*
 * define data space
 */
int boid_num = 10;
float pos_x, pos_y, vel_x, vel_y;
float *pos_x_array, *pos_y_array, *vel_x_array, *vel_y_array;
pos_x_array=(float*)malloc(boid_num*sizeof(float)); 
pos_y_array=(float*)malloc(boid_num*sizeof(float)); 
vel_x_array=(float*)malloc(boid_num*sizeof(float)); 
vel_y_array=(float*)malloc(boid_num*sizeof(float)); 


/*
 * define target information
 *
 */
for (int i = 0; i <= 9; i ++)
{
	tar_pos_x_array[i] = 1.23;
	tar_pos_y_array[i] = 2.31;
	tar_vel_x_array[i] = 0;
	tar_vel_y_array[i] = 0;
}

/*
 * define obtacles
 */
int ob_num = 1;
float ob_pos_x, ob_pos_y;
float *ob_pos_x_array, *ob_pos_y_array;
ob_pos_x_array=(float*)malloc(ob_num*sizeof(float)); 
ob_pos_y_array=(float*)malloc(ob_num*sizeof(float)); 

for(int j=0; j<ob_num; j++)
{
    ob_pos_x = 110.0;
    ob_pos_y = 75.0;        
    ob_pos_x_array[j] = ob_pos_x;
    ob_pos_y_array[j] = ob_pos_y;
    
}

/*
 * define output space
 *
 */
float *out_cmd_vel, *out_theta_cmd;
out_cmd_vel=(float*)malloc(boid_num*sizeof(float)); 
out_theta_cmd=(float*)malloc(boid_num*sizeof(float)); 

/* 
 * call back function
 */
void titianOdomCallback(int marker, const nav_msgs::OdometryConstPtr& boticelliOdometryMsg)
{
	int i = marker;
  	ROS_INFO("boticelli Position: %f, %f", boticelliOdometryMsg.pose.pose.
  			position.x, boticelliOdometryMsg.pose.pose.position.y);
  	ROS_INFO("boticelli Heading: linear:%f, angular:%f", boticelliOdometryMsg.
  			twist.twist.linear.x, boticelliOdometryMsg.twist.twist.angular.z);
  	pos_x_array[i] = boticelliOdometryMsg.pose.pose.position.x;
  	pos_y_array[i] = boticelliOdometryMsg.pose.pose.position.y;
  	vel_x_array[i] = boticelliOdometryMsg.twist.twist.linear.x;
  	vel_y_array[i] = boticelliOdometryMsg.twist.twist.linear.y;
}

int main() {
	// create ros node handle
	ros::NodeHandle n;
	ros::Rate rate(30);  
    // ros::Subscriber sub = n.subscribe("comm", 
    //         1000, comm_call_back);
    for (int i = 0; i <= 9; i ++)
        ros::Subscriber sub = nh.subscribe<nav_msgs::Odometry>("/odom_" + i, 
                100, titianOdomCallback(i));
	ros::spin();
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
        vel_pub_ = n.advertise<geometry_msgs::Twist>(topic, 15);
        vel_pub_set_.push_back(vel_pub_);
    }
	// loop
	while(n.ok())
    {
		getFlockVelCmd(boid_num, pos_x_array, pos_y_array, 
	                tar_pos_x_array, tar_pos_y_array, tar_vel_x_array, tar_vel_y_array,
	                ob_num, ob_pos_x_array, ob_pos_y_array, vel_x_array, vel_y_array,
	                out_cmd_vel, out_theta_cmd);
		// convert out_cmd_vel, out_theta_cmd to /cmd_vel
		for(int i = 0, auto vel_pub = vel_pub_set_.begin(); 
                    vel_pub != vel_pub_set_.end();)
        {
            geometry_msgs::Twist twist;
		  	twist.linear.x = out_cmd_vel[i];
		  	twist.linear.y = 0;
		  	twist.angular.z = out_theta_cmd[i];
		  	vel_pub.publish(twist); 
            vel_pub ++;
            i ++;
        }
		rate.sleep();
	}
	return 0;
}
	