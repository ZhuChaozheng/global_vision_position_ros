#ifndef MAP_PUB_H
#define MAP_PUB_H


#include "ros/ros.h"
#include "nav_msgs/OccupancyGrid.h"
#include "nav_msgs/MapMetaData.h"
#include <tf/transform_listener.h>
#include <geometry_msgs/PoseStamped.h>
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <signal.h>
#include <opencv2/plot.hpp>
#include <cmath>

// #include <Eigen/Core>
// #include <Eigen/Dense>
// #include <Eigen/Geometry>
#include "car.h"


using namespace cv;
using namespace std;
class MapPub
{
public:
	MapPub();
	MapPub(ros::NodeHandle& ng, float map_reso,
			int map_width, int map_height,
			float posx, float posy, float posz,
			float orientx, float orienty,
			float orientz, float orientw);
	~MapPub();
	void mappub_init(Mat &barriers_image, vector<Car> &car_set);
	void mapdata_init();
	void mappub_update(Mat &barriers_image);
	void pose_update(vector<Car> &car_set);
	void draw_edge();
private:
	ros::NodeHandle nh_;
	ros::Publisher mappub_;
	vector<ros::Publisher> pose_publisher_set_;
	ros::Publisher pose_publisher_;
	nav_msgs::OccupancyGrid mapdata_;
	nav_msgs::MapMetaData datainfo_;
	float map_reso;
	int map_width, map_height;
	float posx, posy, posz;
	float orientx, orienty, orientz, orientw;
	string front_str = "/marker_";
    string end_str = "/pose";
    stringstream ss;
};

#endif //MAP_PUB_H
