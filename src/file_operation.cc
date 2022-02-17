#include "file_operation.h"

void ConfigParamtersRead(vector<Car>& car_set) {
  ros::NodeHandle private_nh("~");  // params
  // load cache area for navigation
  float cache_area_right_x, cache_area_right_y, cache_area_left_x,
      cache_area_left_y;
  private_nh.getParam("cache_area_left_x", cache_area_left_x);
  private_nh.getParam("cache_area_left_y", cache_area_left_y);
  private_nh.getParam("cache_area_right_x", cache_area_right_x);
  private_nh.getParam("cache_area_right_y", cache_area_right_y);
  // ConfigCacheAreaRead(left_up, right_down);
  Point2f left_up = Point2f(cache_area_left_x, cache_area_left_y);
  Point2f right_down = Point2f(cache_area_right_x, cache_area_right_x);
  cout << "cache_area left_up: " << left_up << endl;
  cout << "cache_area right_down: " << right_down << endl;
  string front_str;
  int marker_;
  string ip_;
  int port_;
  float target_angle_;
  float target_velocity_;
  float target_point_x_, target_point_y_;
  bool flag = false;  // match existing car
  // clear old car set

  // car_set.erase(car_set.begin(), car_set.end());
  for (int i = 1; i <= 9; i++) {
    string front_str = "marker_";
    string combined_str = front_str + to_string(i);
    // fs[combined_str] >> marker_;
    private_nh.getParam(combined_str, marker_);

    front_str = "ip_";
    combined_str = front_str + to_string(i);
    // fs[combined_str] >> ip_;
    private_nh.getParam(combined_str, ip_);

    front_str = "port_";
    combined_str = front_str + to_string(i);
    // fs[combined_str] >> port_;
    private_nh.getParam(combined_str, port_);

    front_str = "target_angle_";
    combined_str = front_str + to_string(i);
    // fs[combined_str] >> target_angle_;
    private_nh.getParam(combined_str, target_angle_);

    front_str = "target_velocity_";
    combined_str = front_str + to_string(i);
    // fs[combined_str] >> target_velocity_;
    private_nh.getParam(combined_str, target_velocity_);

    front_str = "target_point_x_";
    combined_str = front_str + to_string(i);
    // fs[combined_str] >> target_point_x_;
    private_nh.getParam(combined_str, target_point_x_);

    front_str = "target_point_y_";
    combined_str = front_str + to_string(i);
    // fs[combined_str] >> target_point_y_;
    private_nh.getParam(combined_str, target_point_y_);

    // cout << "read target_velocity_: " << target_velocity_ << endl;
    for (auto iter = car_set.begin(); iter != car_set.end();) {
      int marker = (*iter).get_marker();
      if (marker == i) {
        (*iter).update_parameters(ip_, target_angle_, target_velocity_,
                                  target_point_x_, target_point_y_);
        cout << "update target_angle: " << target_angle_ << endl;
        flag = true;
        break;
      }
      iter++;
    }
    if (!flag) {
      Car* car = new Car(marker_, ip_, port_, target_angle_, target_velocity_,
                         target_point_x_, target_point_y_);
      cout << marker_ << " update target_velocity: " << target_velocity_
           << endl;
      car_set.push_back(*car);
    }
  }
  // fs.release();
  std::cout << "File Read Finished!" << std::endl;
}

void ConfigCacheAreaRead(Point2f& left_up, Point2f& right_down) {
  FileStorage fs("../config/cache_area.yaml", FileStorage::READ);
  float cache_area_left_x, cache_area_left_y, cache_area_right_x,
      cache_area_right_y;
  fs["cache_area_left_x"] >> cache_area_left_x;
  fs["cache_area_left_y"] >> cache_area_left_y;
  fs["cache_area_right_x"] >> cache_area_right_x;
  fs["cache_area_right_y"] >> cache_area_right_y;
  fs.release();
  cout << "File Read Finished!" << endl;
  left_up = Point2f(cache_area_left_x, cache_area_left_y);
  right_down = Point2f(cache_area_right_x, cache_area_right_y);
}

void ConfigFileRead(Mat rvecM1, Mat tvec1, Mat cameraMatrix, float s) {
  FileStorage fs("../config/camera_intrinsic.yaml", FileStorage::READ);

  fs["rvecM1"] >> rvecM1;
  fs["tvec1"] >> tvec1;
  fs["cameraMatrix"] >> cameraMatrix;
  fs["s"] >> s;
  cout << "file s: " << s;
  fs.release();
  cout << "File Read Finished!" << endl;
}

void ConfigFileWrite(const Mat& rvecM1, const Mat& tvec1,
                     const Mat& cameraMatrix, const float& s) {
  // initilization
  FileStorage fs("../config/configure.yaml", FileStorage::WRITE);

  // start write
  // cameraMatrix = (Mat_<float>(3, 3) << 1000,
  //     0, 320, 0, 1000, 240, 0, 0, 1);
  // distCoeffs = (Mat_<float>(5, 1) << 0.1,
  //     0.01, -0.001, 0, 0);
  fs << "rvecM1" << rvecM1 << "tvec1" << tvec1 << "cameraMatrix" << cameraMatrix
     << "s" << s;

  fs.release();

  cout << "file write finished!" << endl;
}
