#ifndef IMAGE_TRANSFORMATION_H
#define IMAGE_TRANSFORMATION_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctype.h>
#include "file_operation.h"
using namespace cv;
using namespace std;

Mat cameraMatrix = Mat::eye(3, 3, 
		cv::DataType<float>::type);  //intrinsic of camera
Mat rvecM1 = Mat::eye(3, 3, 
		cv::DataType<float>::type);  //rotation matrix
Mat tvec1 = Mat::eye(3, 1, 
		cv::DataType<float>::type);  // translation vector
float s = 203067;

void PointToWorld(const Point& point, Point3f& worldPoint,
                  const Mat& rvecM1, const Mat& tvec1,
                  const Mat& cameraMatrix, const float& s);
#endif //IMAGE_TRANSFORMATION_H