#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctype.h>
#include "car.h"
#include "ros/ros.h"

using namespace cv;
using namespace std;

void ConfigCacheAreaRead(Point2f &left_up, Point2f &right_down);

void ConfigFileRead(Mat rvecM1, Mat tvec1, Mat cameraMatrix, float s);
 
void ConfigFileWrite(const Mat& rvecM1, const Mat& tvec1,
                     const Mat& cameraMatrix, const float& s);

void ConfigParamtersRead(vector<Car> &car_set);
#endif // FILE_OPERATION_H
