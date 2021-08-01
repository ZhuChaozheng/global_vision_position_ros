#include "map_pub.h"
#include <Eigen/Core>
#include <Eigen/Dense>

MapPub::MapPub(ros::NodeHandle& nh, float map_reso,
            int map_width, int map_height,
            float posx, float posy, float posz,
            float orientx, float orienty,
            float orientz, float orientw) :
            nh_(nh), map_reso(map_reso), map_width(map_width),
            map_height(map_height), posx(posx), posy(posy),
            posz(posz), orientx(orientx), orienty(orienty),
            orientz(orientz), orientw(orientw)
{
    mapdata_init();
    // MapPub_init();  
}

MapPub::~MapPub()
{

}

void MapPub::mapdata_init()
{
    
    datainfo_.resolution = map_reso;
    datainfo_.width = map_width;
    datainfo_.height = map_height;
    datainfo_.origin.position.x = posx;
    datainfo_.origin.position.y = posy;
    datainfo_.origin.position.z = posz;
    datainfo_.origin.orientation.x = orientx;
    datainfo_.origin.orientation.y = orienty;
    datainfo_.origin.orientation.z = orientz;
    datainfo_.origin.orientation.w = orientw;
}

void MapPub::pose_update(vector<Car> &car_set)
{
    geometry_msgs::PoseStamped pose_stamped;
    
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        int marker = (*iter).get_marker();
        Point2f medianPoint = (*iter).get_median_point();
        float slope = (*iter).get_slope();
        // eularAngel to Quaternion
        Eigen::AngleAxisd rollAngle(0, Eigen::Vector3d::UnitZ());
        Eigen::AngleAxisd yawAngle(slope, Eigen::Vector3d::UnitY());
        Eigen::AngleAxisd pitchAngle(0, Eigen::Vector3d::UnitX());

        Eigen::Quaternion<double> q = rollAngle * yawAngle * pitchAngle;
        // cout << q.coeffs() << endl;
        // pulish through ros
        pose_stamped.header.frame_id = "/map";
        pose_stamped.header.stamp = ros::Time::now();
        pose_stamped.pose.position.x = medianPoint.x;
        pose_stamped.pose.position.y = medianPoint.y;
        pose_stamped.pose.position.z = 0;

        pose_stamped.pose.orientation.x = q.x();
        pose_stamped.pose.orientation.y = q.y();
        pose_stamped.pose.orientation.z = q.z();
        pose_stamped.pose.orientation.w = q.w();
        // pose_stamped.pose.orientation.x = 0;
        // pose_stamped.pose.orientation.y = 0;
        // pose_stamped.pose.orientation.z = 0;
        // pose_stamped.pose.orientation.w = 0;

        for(auto pose_publisher = pose_publisher_set_.begin(); 
                    pose_publisher != pose_publisher_set_.end();)
        {
            string front_str = "/marker_";
            string end_str = "/pose";
            stringstream ss;    
            ss << front_str << marker << end_str;
            string topic = ss.str();            
            if ((*pose_publisher).getTopic() == topic)
            {
                (*pose_publisher).publish(pose_stamped);
            }
            pose_publisher ++;
        }
        iter ++;
    }
}

void MapPub::mappub_init(Mat &barriers_image, vector<Car> &car_set)
{    
    ros::Rate rate(40);
    mappub_ = nh_.advertise<nav_msgs::OccupancyGrid>("map", 1000);

    // batch initilization the topic vector
    for (auto iter = car_set.begin(); iter != car_set.end();)
    {
        int marker = (*iter).get_marker();    
        string front_str = "/marker_";
        string end_str = "/pose";
        stringstream ss;    
        ss << front_str << marker << end_str;
        string topic = ss.str();
        pose_publisher_ = nh_.advertise<geometry_msgs::PoseStamped>(topic, 1000);
        pose_publisher_set_.push_back(pose_publisher_);
        iter ++;
    }
    while(nh_.ok())
    {
        // publish map topic
        mappub_.publish(mapdata_);
        mappub_update(barriers_image);
        pose_update(car_set);
        rate.sleep();
    }
}

void MapPub::mappub_update(Mat &barriers_image)
{   
    ros::Time maptime = ros::Time::now();
    int datasize = map_width * map_height;
    mapdata_.header.stamp = maptime;
    mapdata_.header.frame_id = "map";
    mapdata_.info = datainfo_;
    mapdata_.data.resize(datasize);
    if (barriers_image.cols > 0)
    {
        // copy to publish topic
        for (size_t y = 0; y < map_height; y++)
        {
            for (size_t x = 0; x < map_width; x++)
            {
                unsigned char* row_ptr = 
                        barriers_image.ptr<unsigned char> (y);
                unsigned char* data_ptr = 
                        &row_ptr[x * barriers_image.channels()];
                // data_ptr[0] is a gray image channel
                mapdata_.data[y * map_width + x] = data_ptr[0];
            }
        }
    }
    draw_edge();
    
}

void MapPub::draw_edge()
{
    int datasize = map_width * map_height;
    // // draw edge
    // for (int i = 0; i < datasize; i ++)
    // {
    //     // 0 white
    //     mapdata_.data[i] = 0;
    // }   
    // two width edge
    for (int j = 1; j < map_width; j ++)
    {
        mapdata_.data[j - 1] = 100;
        mapdata_.data[datasize - j] = 100;
    }
    // two height edge
    for (int j = 1; j < map_height; j ++)
    {
        mapdata_.data[j * map_width] = 100; // 
        mapdata_.data[(j + 1) * map_width - 1] = 100;
    }
    // draw initial point
    for(int i = 0; i <= 25; i++)
        for(int j = 0; j <= 25; j ++)
        {
            int k = i + (map_width * j);
            mapdata_.data[k] = 100;
        }
}
