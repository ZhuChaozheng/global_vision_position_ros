#ifndef INIT_CAMERA_H
#define INIT_CAMERA_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "MvCameraControl.h"
#include <opencv2/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;



bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);
bool EnumDevice(); //  infrequent
// thress core functions
bool InitCamera(); 
bool InitCamera1();
bool StartGrabStream(Mat& image);
bool StartGrabStream1(Mat& image);
bool Destroy();

#endif //INIT_CAMERA_H
