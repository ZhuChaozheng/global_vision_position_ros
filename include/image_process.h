/**
 * created by Jason 
 * 2020.6.13
 * fetch the blocks from gray gimage
 */

#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include <math.h> // atan
#include "car.h"
#include "init_camera.h"

#define PI 3.14159265

// define the bound of base
#define MAX_DISTANCE 29.3
#define MIN_DISTANCE 24.3
#define MINIMUM 1
#define PIXEL_GAP_X 762
#define PIXEL_GAP_Y 62

typedef enum feature {SIDE, BOTTOM};

using namespace cv;
using namespace std;

class PointAttri
{
public:
	Point2f point;
	int value;
};

// define graph
typedef Point2f Vertex; // vertex is point type
typedef float WeightValue; // weight value

extern int frame_type; // key_frame = 0, update_frame = 1
extern RotatedRect boundary_rect;
class EdgeNode
{
public:
	Vertex V1, V2; // full undirected graph
	WeightValue Weight;
};

class point {
        public:
        point(float a, float b):first(a), second(b){}
        float first;
        float second;
        bool operator < (const point &m)const {
                return first < m.first;
        }
};
float reverse_slope(float slope);
bool Cmp(const Point2f a, const Point2f b);
void barriers_image_fusion(Mat &barriers_image, const Mat
        &barriers_image_left, const Mat &barriers_image_right);
void capture_handle_image(Mat &src_left, Mat &barriers_image_left,
        vector<Car> &car_set_left, int &getCarLeft, Mat &cameraMatrix,
        Mat &distCoeffs, int nIndex);
void AnalyzeContours(const Mat &src_gray, Mat &barriers_image,
		vector<Point2f> &pointSet, Mat &cameraMatrix, Mat &distCoeffs);
void ThreshCallBack(const Mat &src_gray, 
		vector<Point2f> &pointSet);
void car_set_fusion(vector<Car> &car_set, vector<Car> &car_set_left,
        vector<Car> &car_set_right, int &getCarLeft, int &getCarRight);
float GetCross(Point2f &p1, Point2f &p2, Point2f &p);
bool IsPointInRotatedRect(RotatedRect &rotated_rect, Point2f &p);
// bool PointNeighbourVector(const Point2f currentPoint, 
// 		vector<Point2f>	float_dup_point_set);
bool Exist(Car& car, vector<Car>& carStateSet, Car& lastCar);
bool NeighbourPoint(Point2f pointA, Point2f pointB);
void ClassificationCar(vector<Point2f> *pointSet, 
		vector<Car> &carSet, int camera);
float GetPixelDistance(Point2f pointA, Point2f pointB);
void GetCarKeyAttribution(Car& car);
int FindKeyPoint(Point2f& point, Point2f& tempPoint, 
		vector<Point2f>& pointSet);

void DetermineTriangleVertex(vector<Point2f>& 
		pointSet, Car& car);
void GetAbsoluteOrientationAndPosition(vector<Point2f>& 
		pointSet, Car& car);
float GetSlope(Point2f first, Point2f second);

float Get3dSlope(Point3f first, Point3f second);

void DeleteCar(Car& car, vector<Car>& carStateSet);

#endif //IMAGE_PROCESS_H
