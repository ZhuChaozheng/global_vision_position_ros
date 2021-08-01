#include "image_transformation.h"

void PointToWorld(const Point& point, Point3f& worldPoint,
                  const Mat& rvecM1, const Mat& tvec1,
                  const Mat& cameraMatrix, const float& s)
{
    //////////////////////camera_coordinates////////////////
    // Mat camera_cordinates = -rvecM1.inv() * tvec1;
    /////////////////////2D to 3D///////////////////////
    Mat imagePoint = Mat::ones(3, 1, cv::DataType<float>::type); //u,v,1
    imagePoint.at<float>(0, 0) = point.x;
    imagePoint.at<float>(1, 0) = point.y; 
    // cameraMatrix 3*3
    Mat wcPoint = rvecM1.inv() * (cameraMatrix.inv() * s * imagePoint - tvec1);
    Point3f worldPoint1(wcPoint.at<float>(0, 0), wcPoint.at<float>(1, 0), wcPoint.at<float>(2, 0));
    worldPoint = worldPoint1 / 100;
}


void calibration()
{
    /**
     * fill in pixel coordinates and their responding world points,
     * solve R&T from PnP
     */
    Point2f point;
    vector<Point2f> boxPoints; //save pixel coordinates
    // Loading image
    Mat sourceImage = imread("../img/6.bmp", CV_32FC1);
    // namedWindow("Source", 1);
    ///// Setting box corners in image
    //////one Point////
    point = Point2f((float) 530, (float) 538); //
    boxPoints.push_back(point);
    circle(sourceImage, boxPoints[0], 3, Scalar(0, 255, 0), -1, 8);

    ////two Point////
    point = Point2f((float) 530, (float) 546); //
    boxPoints.push_back(point);
    circle(sourceImage, boxPoints[1], 3, Scalar(0, 255, 0), -1, 8);

    ////three Point////
    point = Point2f((float) 530, (float) 562); //
    boxPoints.push_back(point);
    circle(sourceImage, boxPoints[2], 3, Scalar(0, 255, 0), -1, 8);

    ////four Point////
    point = Point2f((float) 538, (float) 538); //
    boxPoints.push_back(point);
    circle(sourceImage, boxPoints[3], 3, Scalar(0, 255, 0), -1, 8);
    
     
    //////////Setting box corners in real world////////////////////
    vector<Point3f> worldBoxPoints;  //save world coordinates
    Point3f tmpPoint;

    tmpPoint = Point3f((float) 0, (float) 0, (float) 0);
    worldBoxPoints.push_back(tmpPoint);
    // 2000 -> 20mm
    tmpPoint = Point3f((float) 2000, (float) 0, (float) 0);
    worldBoxPoints.push_back(tmpPoint);
    tmpPoint = Point3f((float) 6000, (float) 0, (float) 0);
    worldBoxPoints.push_back(tmpPoint);
    tmpPoint = Point3f((float) 0, (float) 2000, (float) 0);
    worldBoxPoints.push_back(tmpPoint);
    //////camera  intristic///////////////////////////////
    
    cv::Mat distCoeffs1(5, 1, cv::DataType<float>::type);  // distortion parameter
    distCoeffs1.at<float>(0,0) = -9.6445698521454362e-02;
    distCoeffs1.at<float>(1,0) = 4.7244120474034097e-02;
    distCoeffs1.at<float>(2,0) = -3.0670069309505067e-04;
    distCoeffs1.at<float>(3,0) = 1.7385357668277593e-03;
    distCoeffs1.at<float>(4,0) = 1.5959246035337351e-02;

    //Taken from Mastring OpenCV d
    float fx = 8.2322484623389789e+02;
    float fy = 8.2322484623389789e+02;
    float cx = 6.5199360623872849e+02;
    float cy = 5.0598339959414085e+02;

    cameraMatrix.at<float>(0, 0) = fx;
    cameraMatrix.at<float>(1, 1) = fy;
    cameraMatrix.at<float>(0, 2) = cx;
    cameraMatrix.at<float>(1, 2) = cy;
    
   //////PnP solve R&T///////////////////////////////
    cv::Mat rvec1(3, 1, cv::DataType<float>::type);  // rotation vector
    cv::solvePnP(worldBoxPoints, boxPoints, cameraMatrix, distCoeffs1, rvec1, tvec1, false,CV_ITERATIVE);
    cv::Rodrigues(rvec1, rvecM1);

   // euler angle
    const float PI=3.1415926;
    float thetaZ=atan2(rvecM1.at<float>(1,0),rvecM1.at<float>(0,0))/PI*180;
    float thetaY=atan2(-1*rvecM1.at<float>(2,0),sqrt(rvecM1.at<float>(2,1)*rvecM1.at<float>(2,1)
            +rvecM1.at<float>(2,2)*rvecM1.at<float>(2,2)))/PI*180;
    float thetaX=atan2(rvecM1.at<float>(2,1),rvecM1.at<float>(2,2))/PI*180;
    cout<<"theta x  "<<thetaX<<endl<<"theta Y: "<<thetaY<<endl<<"theta Z: "<<thetaZ<<endl;

    ///////////////solve s/Zc from equation 2////////////////////////
    cv::Mat imagePoint = cv::Mat::ones(3, 1, cv::DataType<float>::type); 
    cv::Mat tempMat, tempMat2;
    //input pixel coordinate to derivate s
    imagePoint.at<float>(0,0) = 530;
    imagePoint.at<float>(1,0) = 538;
    float zConst = 0; // the world coordinate of z of that pixel 
    
    // calculate s
    tempMat = rvecM1.inv() * cameraMatrix.inv() * imagePoint;
    tempMat2 = rvecM1.inv() * tvec1;
    s = zConst + tempMat2.at<float>(2, 0);
    s /= tempMat.at<float>(2, 0);
    cout<<"s : "<<s<<endl;
    // write the core parameter to file
    ConfigFileWrite(rvecM1, tvec1, cameraMatrix, s);
}

