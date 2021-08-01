#include "image_process.h"

int thresh = 30; // gray
// int thresh = 255; // colour
bool init_slope_flag = true;
RNG rng(12345);
Mat src_gray;
Mat cimage;

float medium_filter(float input, float x[], int n)
{

// update data
for (uint8_t i = n - 1; i > 0; i--)
    {
        x[i] = x[i - 1];
	//cout << "update " << x[i] << endl;
    }
    x[0] = input;
    /* 输入缓冲区拷贝，排序会被打乱顺序 */
    float x_copy[n] = {0};
    memcpy(x_copy, x, sizeof(float) * n);
    /* 升序冒泡排序 */
    for (uint8_t j = 0; j < n - 1; j++)
    {
        for (uint8_t i = 0; i < n - j - 1; i++)
        {
            if (x_copy[i] > x_copy[i + 1])
            {
                float temp = x_copy[i];
                x_copy[i] = x_copy[i + 1];
                x_copy[i + 1] = temp;
            }
        }
    }
 
    /* 窗口大小为偶数个 */
    if (n % 2 == 0)
    {
        return (x_copy[n / 2] + x_copy[n / 2 - 1]) / 2.0f;
    }
    /* 窗口大小为奇数个 */
    else
    {
        return x_copy[(n - 1) / 2];
    }
}

void car_set_fusion(vector<Car> &car_set, vector<Car> &car_set_left,
        vector<Car> &car_set_right, int &getCarLeft, int &getCarRight)
{
    // only right camera has cars
    if((getCarLeft == 0)&&(getCarRight == 1))
    {
        for (auto iter = car_set.begin(), iter_right = car_set_right.begin(); 
                iter != car_set.end();)
        {
            Point2f median_point = (*iter_right).get_median_point();
            float slope = (*iter_right).get_slope();
            // complete the initial state depend on the current position 
            if(median_point != Point2f(0, 0))
            {
                (*iter).set_median_point(median_point);
                (*iter).set_slope(slope);
            }
            iter_right ++;
            iter ++;
        }
    }
    // only left camera has cars
    if((getCarLeft == 1)&&(getCarRight == 0))
    {
        for (auto iter = car_set.begin(), iter_left = car_set_left.begin(); 
                iter != car_set.end();)
        {
            Point2f median_point = (*iter_left).get_median_point();
            float slope = (*iter_left).get_slope();
            // complete the initial state depend on the current position 
            if(median_point != Point2f(0, 0))
            {
                (*iter).set_median_point(median_point);
                (*iter).set_slope(slope);
            }
            iter_left ++;
            iter ++;
        }
    }
    // both cameras view cars
    if((getCarLeft == 1)&&(getCarRight == 1))
    {
        for (auto iter = car_set.begin(), iter_left = car_set_left.begin(),
                iter_right = car_set_right.begin(); iter != car_set.end();)
        {
            Point2f median_point_left = (*iter_left).get_median_point();
            Point2f median_point_right = (*iter_right).get_median_point();
            float slope_left = (*iter_left).get_slope();
            float slope_right = (*iter_right).get_slope();
            // if only the left camera see the particular car
            if((median_point_left != Point2f(0, 0)) 
                    &&(median_point_right == Point2f(0, 0)))
            {                   
                (*iter).set_median_point(median_point_left);
                (*iter).set_slope(slope_left);  
            }
            // if only the right camera see the particular car
            else if((median_point_left == Point2f(0, 0)) 
                    &&(median_point_right != Point2f(0, 0)))
            {
                (*iter).set_median_point(median_point_right);
                (*iter).set_slope(slope_right);
            }
            // the remaining possible only is the car 
            // has been viewed together
            else if((median_point_left != Point2f(0, 0)) 
                    &&(median_point_right != Point2f(0, 0)))
            {
                Point2f fused_median_point = Point2f((median_point_left.x + 
                        median_point_left.x) / 2, (median_point_left.y + 
                        median_point_right.y) / 2);
                float fused_slope = (slope_left + slope_right) / 2;
                (*iter).set_median_point(fused_median_point);
                (*iter).set_slope(fused_slope);
            
            }
            iter ++;
            iter_left ++;
            iter_right ++;
        }
    }
}

// fuse the left and right barrier images in one bigger image
void barriers_image_fusion(Mat &barriers_image, const Mat
        &barriers_image_left, const Mat &barriers_image_right)
{
    if (barriers_image_left.empty())
    {
        cout << "left image empty!" << endl;
    }
    else
    {
    	Mat r1 = barriers_image(Rect(0, 0, barriers_image_left.cols,
	         barriers_image_left.rows));
        barriers_image_left.copyTo(r1);
    }
    if (barriers_image_right.empty())
    {
	//cout << "right image empty!" << endl;
	return;
    }
    Mat temp_image = barriers_image_right(Rect(PIXEL_GAP_X, 
	    0, barriers_image_right.cols - PIXEL_GAP_X, barriers_image_right.rows));  
    Mat r2 = barriers_image(Rect(barriers_image_left.cols, 0,
	    barriers_image_right.cols - PIXEL_GAP_X, barriers_image.rows));
    temp_image.copyTo(r2);
}

void capture_handle_image(Mat &src_left, Mat &barriers_image_left,
        vector<Car> &car_set_left, int &getCarLeft, Mat &cameraMatrix,
        Mat &distCoeffs, int nIndex)
{
    struct timeval lastTime, currentTime;
    float consumeTime;
    gettimeofday(&lastTime,NULL);
    while(1)
    {
	if (nIndex == 0)
	        StartGrabStream(src_left);
	else if (nIndex == 1)
		StartGrabStream1(src_left);
        if (src_left.empty())
        {
            cout << "could not read the image." << endl;
	    break;
        }
        Mat src_thresh_left, dst;
		//namedWindow( "src_left", 0 );
       // imshow("src_left", src_left);
        threshold(src_left, src_thresh_left, 210, 255, 
                THRESH_BINARY);
	//	namedWindow( "src_thresh_left", 0 );
       // imshow("src_thresh_left", src_thresh_left);
	Mat erodeStruct = getStructuringElement(2, Size(3, 3));
    	dilate(src_thresh_left, dst, erodeStruct);
	//namedWindow( "dst", 0 );
	////imshow("dst", dst);
        //waitKey(5);
        vector<Point2f> pointSet;
        barriers_image_left = Mat::zeros(src_left.size(), CV_8UC1);
        AnalyzeContours(dst, barriers_image_left, pointSet,
        		cameraMatrix, distCoeffs);
        for(;;) 
        {
            if (pointSet.size() != 0){
                ClassificationCar(&pointSet, car_set_left, nIndex);
		}
            else
               break;
        }
        getCarLeft = 0;
        for (auto iter = car_set_left.begin(); 
                iter != car_set_left.end();)
        {
            GetCarKeyAttribution(*iter);
            // complete the initial state depend on the current position 
            if((*iter).get_median_point() != Point2f(0,0))
                getCarLeft = 1;
            iter ++;
        }
	// consume time 
        //clock_t currentTime = clock();
//        cout << "lastTime: " << lastTime.tv_usec << endl;

        gettimeofday(&currentTime, NULL);
        consumeTime = (currentTime.tv_sec - lastTime.tv_sec) + 
                (float)(currentTime.tv_usec - lastTime.tv_usec) / 1000000.0;
        cout << "fps: " << 1 / consumeTime << "Hz" << endl;
	// update the last state
        lastTime = currentTime;
        
    }
}

void AnalyzeContours(const Mat &src_gray, Mat &barriers_image,
		vector<Point2f> &pointSet, Mat &cameraMatrix, Mat &distCoeffs)
{
	Mat thresh_output;
	thresh_output = src_gray.clone();
    // find contours
    vector<vector<Point>> contours;
    findContours(src_gray, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    cimage = Mat::zeros(thresh_output.size(), CV_8UC3); // global
    vector<RotatedRect> dup_rectangle_set;
    vector<vector<Point>> next_notice_contours;
    for (size_t t = 0; t < contours.size(); t++) 
    {
	// red
    	//drawContours(cimage, contours, static_cast<int>(t), 
	  //               Scalar(255, 0, 255), 2, 8);   
        // only select specific area
        RotatedRect rotatedRect = minAreaRect(contours[t]);
        // bigger rect contains rotated rect
        Rect rect = rotatedRect.boundingRect();
        
        // geometry analysis
        float w = rotatedRect.size.width;

        float h = rotatedRect.size.height;
        float areaRect = w * h;
        float rate = w / h;

        float perimeter = arcLength(contours[t], true);
	if (perimeter > 3200)
	{
		// boundary
        	//cout << "boundary: " << perimeter << endl;
        	boundary_rect = rotatedRect;
	}
        // the minimum perimeter is 240
        if (perimeter > 500)
        {
        	
        	// barriers
        	// drawContours(barriers_image, contours, static_cast<int>(t), 
	        //         black, 2, 8);  
		// red
	        drawContours(barriers_image, contours, static_cast<int>(t), 
	                Scalar(255, 0, 255), 2, 8); 
         	//namedWindow( "barriers_image", 0 );
    		//imshow("barriers_image", barriers_image);
		//waitKey(5);
        }
		// if (perimeter < 130 | rate < 0.85)
        // 	continue;
        else if (perimeter > 130)
        
        {
        	Point2f currentPoint = rotatedRect.center;
	        // TODO
	        // remove the shadow of wheels
	        // remove duplicate center of rectangle
	        if (dup_rectangle_set.size() == 0)
	        {
	        	dup_rectangle_set.push_back(rotatedRect);
	        	continue;
	        }
	        // copy vector
	        vector<RotatedRect> float_dup_rectangle_set;
	        for(auto iter = dup_rectangle_set.begin(); 
	        		iter != dup_rectangle_set.end();)
	        {

	        	float_dup_rectangle_set.push_back(*iter);
	        	iter ++;
	        }
	        /*
	         * here, should be replaced with a special function
	         * bool PointNeighbourVector(Point2f currentPoint, 
	         *  vector<Point2f> float_dup_point_set)
	         *
	         */
	    	bool flag = true;
	        for(auto iter = float_dup_rectangle_set.begin(); 
	        		iter != float_dup_rectangle_set.end();)
	        {
	        	Point2f tempPoint = (*iter).center;	        	
	        	if (!NeighbourPoint(tempPoint, currentPoint))
				{
					flag = true;
				}
				else
				{
					flag = false;
					break;
				}
				iter ++;
	        }
		    if (flag)
	        	dup_rectangle_set.push_back(rotatedRect);
	        //float_dup_rectangle_set.clear();
		vector<RotatedRect>().swap(float_dup_rectangle_set);
		//float_dup_rectangle_set.swap();
        }
        else
        {
        	// next notice points
        	next_notice_contours.push_back(contours[t]);
        	// cout << "next: " << next_notice_contours.size() << endl;
        }
    }

    float radius = 4;
    int i = 0;
    for (size_t t = 0; t < next_notice_contours.size(); t++) 
    {
        float area = contourArea(next_notice_contours[t]);
        RotatedRect rotated_rect = minAreaRect(next_notice_contours[t]);
        	for (auto iter = dup_rectangle_set.begin();
		    			iter != dup_rectangle_set.end();)
		    {
		    	Point2f observedPoint = rotated_rect.center;
		    	// iter is RotatedRect class
		    	// display the position of big square
		    	if (IsPointInRotatedRect((*iter), observedPoint))
		        {
		        	// rotated_rect is small square
		        	pointSet.push_back(observedPoint);
				// blue				
		        	circle(cimage, observedPoint, cvRound(radius), 
						Scalar(255, 0, 0), 2, LINE_AA);
		        }
		    	iter ++;
		    }
    }
//namedWindow( "thresh_output", 0 );
  //  imshow("thresh_output", cimage);
    waitKey(5);
    vector<Point2f> idealPoints;
/*
    // rectify the pixel in instrinsic
	if (pointSet.size() > 0)
		undistortPoints(pointSet, idealPoints, cameraMatrix, distCoeffs, noArray(), cameraMatrix);
*/
/*
	for (auto iter = idealPoints.begin(); iter != idealPoints.end();)
	{
		cout << (*iter) << endl;
		iter ++;
	}
*/
	//cout << "point size: " << idealPoints.size() << endl;

}

float reverse_slope(float slope)
{
    float temp = slope + 180;
    if (temp < 360)
        return temp;
    else
        return temp - 360;
}

float GetCross(Point2f &p1, Point2f &p2, Point2f &p)    ///
{
	return (p2.x - p1.x) * (p.y - p1.y)
			- (p.x - p1.x) * (p2.y - p1.y);
}

bool IsPointInRotatedRect(RotatedRect &rotated_rect, Point2f &p)
{
	Point2f vtx[4];
	rotated_rect.points(vtx);

    // Draw the bounding box
    for( int i = 0; i < 4; i++ )
    {
    	line(cimage, vtx[i], vtx[(i+1)%4], 
        		Scalar(0, 255, 0), 1, LINE_AA); // green
    }

	return (GetCross(vtx[0], vtx[1], p) * 
			GetCross(vtx[2], vtx[3], p) >= 0 )
			&& (GetCross(vtx[1], vtx[2], p) * 
			GetCross(vtx[3], vtx[0], p) >= 0);
	
}

// return w((x_1 - x_2)^2 + (y_1 - y_2)^2)^(1/2)
float GetPixelDistance(Point2f pointA, Point2f pointB)
{
    return sqrt((pointA.x - pointB.x) * (pointA.x - pointB.x) 
    	+ (pointA.y - pointB.y) * (pointA.y - pointB.y));
}

/**
 *
 * input: pointSet
 * output: car_set
 * based on the distance among blocks, 35 is the max distance of car
 *
 */
void ClassificationCar(vector<Point2f> *pointSet, vector<Car> &car_set, int camera = 0)
{	
	// if you remount the camera, may be this parameter should be tuned
    // float ridus = 35; // mini distance among color blocks of car
    //cout << "camera number before:" << camera << endl;
    float ridus = 40;
    vector<Point2f> point_set;
    int num = 0;
    Point2f firstPoint;   
    for (auto point = (*pointSet).begin(); 
    		point != (*pointSet).end();)
	{
		// flag of initialization
		if (num == 0) 
		{
			firstPoint = *point;
			num = 1;
			continue;
		}

		Point2f tempPoint = *point;
		// cout << tempPoint << endl;
		float distance = GetPixelDistance(firstPoint, tempPoint);
		// cout << distance << endl;
		if (distance < ridus) 
		{
			point_set.push_back(tempPoint);
			// delete point
			point = (*pointSet).erase(point);
		}
		else
		{
			point ++;
		}
	}
	//cout << "classificationCar: " << point_set.size() << endl;
	int marker = point_set.size() - 3;
	//cout <<"iamge_process marker   "<< marker << endl;
	if (marker >= 0)
	{	
		if(camera == 1){
			for(auto iterator = point_set.begin(); iterator != point_set.end();)
				{
					(*iterator).x = (*iterator).x + 1280 - PIXEL_GAP_X; // based on the real environment
					(*iterator).y = (*iterator).y - PIXEL_GAP_Y;
					iterator ++;
				}
		}
		// search 
		for (auto iter = car_set.begin(); iter != car_set.end();)
		{
			if ((*iter).get_marker() == marker)
			{
				// clear old pointset
				(*iter).pointSet.clear();
				// copy set from A to B
				for(auto iterator = point_set.begin(); 
						iterator != point_set.end();)
				{
					(*iter).pointSet.push_back(*iterator);
					iterator ++;
				}
			}
			iter ++;
		}
	}
}

int FindKeyPoint(Point2f& point, Point2f& tempPoint, vector<Point2f>& pointSet)
{	
	for (auto iter = pointSet.begin(); iter != pointSet.end();)
	{
		tempPoint = *iter;
		float distance = GetPixelDistance(point, tempPoint);
		
		if (distance < MAX_DISTANCE & distance > MIN_DISTANCE)
			return 0;
		iter ++;
	}
	// no proper
	return 1;
}

// return absolute orientation
// GetSlope(center, vertex);
float GetSlope(Point2f first, Point2f second)
{
	float y = float(first.y - second.y);
	float x = float(second.x - first.x);
	float slope = atan2(y, x) * 180 / PI;
	// 0~360
	if (slope < 0)
		slope = 360 + slope;
  	return slope;
}

float Get3dSlope(Point3f first, Point3f second)
{
	// float param = float(second.x - first.x) / float(first.y - second.y);	
	float y = float(first.y - second.y);
	float x = float(second.x - first.x);
	// distinguish atan & atan2
	float slope = atan2(y, x) * 180 / PI;
	// 0~360
	if (slope < 0)
		slope = 360 + slope;
  	return slope;
}

// return the median point between two points
Point2f GetmedianPoint(Point2f first, Point2f second)
{
	Point2f point;
	point.x = (first.x + second.x ) / 2;
	point.y = (first.y + second.y ) / 2;
	return point;
}

/*
 * 1. define a virtual circle center on car
 * 2. line between circle center and vertex
 * 3. draw a sine circle and calculate its slope in 
 * absolute orientation.
 *
 */
void GetAbsoluteOrientationAndPosition(vector<Point2f>& pointSet,
		Car& car)
{
	Point2f last_first = car.get_first();
	if (pointSet.size() == 0)
		return;
	DetermineTriangleVertex(pointSet, car);
    // Find the minimum area enclosing circle
    Point2f center;
    float radius = 0;
    minEnclosingCircle(pointSet, center, radius);
    Point2f first = car.get_first();
    float slope = GetSlope(center, first);
    
if (car.get_marker() ==6)
{
// green
circle(cimage, first, cvRound(2), 
						Scalar(255, 255, 0), 2, LINE_AA);
// red
circle(cimage, center, cvRound(2), 
						Scalar(255, 0, 255), 2, LINE_AA);

 //namedWindow( "thresh_output", 0 );
   // imshow("thresh_output", cimage);
//    waitKey(5);
}
    float last_slope = car.get_slope();    
    float distance = GetPixelDistance(last_first, first);
    if (distance > 50) // error recognise
        {
	car.set_slope(last_slope);
	car.set_median_point(center);
	return;
}
	
    //cout << car.get_marker() << "delta_slope: " << delta_slope << endl;
    //cout << "distance: " << distance << endl;
    //cout << "imgae_process.cc frame_type " << frame_type << endl;

    float filter_slope = medium_filter(slope, car.get_cache_slope_array(), 21);

    float delta_slope = fabs(car.get_target_slope() - last_slope);    
if (car.get_marker() == 6){
    cout << "filtered slope: " << filter_slope << endl;
    cout << "slope: " << slope << endl;
    cout << "delta_slope: " << delta_slope << endl;
    cout << "distance: " << distance << endl;
    cout << frame_type << endl;
}
    
if (delta_slope > 30) // roll action
    car.set_slope(slope);
else
 	     car.set_slope(filter_slope);
car.set_median_point(center);
/*
    // update state according to the frame type
    switch(frame_type)
	{
		case 1: // update frame
			// prevent the slope soar
			if ((delta_slope < 50 || delta_slope > 330) && distance < 10)
{
		    	     car.set_slope(filter_slope);
}
		    	//cout << "image_process.cc is updateframe "  << endl;
			break;
		case 0: // key frame
		    car.set_slope(filter_slope);
		    //cout << "image_process.cc is keyframe " << endl;
			break;
	}
   */
}


/*
 * return car's current point and slope through point set of car
 *
 */
void GetCarKeyAttribution(Car& car)
{
	vector<Point2f> pointSet = car.get_point_set();	
	if (pointSet.size() != 0)
	{
		GetAbsoluteOrientationAndPosition(pointSet, car); // stuff a triangle
		
		/*
		float lastSlope = car.get_slope();
		// limit the expectional error from recognition
		if (car.get_init_slope_flag())
		{
			car.set_slope(slope);
			car.set_init_slope_flag(false);
			return;
		}
		// cout << "slope - lastSlope: " << slope - lastSlope << endl;
		// cout << "slope - lastSlope: " << fabs(slope - lastSlope) << endl;
		if ((fabs(slope - lastSlope) < 50.0) ||(fabs(slope - lastSlope) >300.0))
		
			car.set_slope(slope);
		
		else			
		{
			// cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			car.set_slope(lastSlope);
		}
		*/
	}	
}

/**
 * define a undirected graph to save 
 * maximum distance of two vertex
 *
 */
EdgeNode FindMaxDistance(const vector<Point2f>& pointSet, 
		float maxDistance)
{
	float tempDistance = 0;
	EdgeNode edgeNode;
	// int num = 0;
	for (auto iter = pointSet.begin(); iter != pointSet.end();)
	{
		Point2f currentPoint = *iter;
		auto iterTemp = iter;
		for (iterTemp++; iterTemp != pointSet.end();)
		{

			Point2f tempPoint = *iterTemp;
			float distance = GetPixelDistance(
					currentPoint, tempPoint);
			if ((tempDistance < distance) & 
					(distance != maxDistance)) 
			{
				tempDistance = distance;
				// save in struct				
				edgeNode.V1 = currentPoint;
				edgeNode.V2 = tempPoint;
				// cout << "V2: " << tempPoint << endl;
				edgeNode.Weight = tempDistance;

			}
			iterTemp ++;
		}
		iter ++;
	}
	return edgeNode;
}
	

/**
 * find vertex in points
 * 
 * select top two distance edge, 
 * then determine vertex of triangle
 *
 */
void DetermineTriangleVertex(vector<Point2f>& pointSet, Car& car)
{
	// search max distance between points, 
	// equals to one isosceles
	EdgeNode edgeNode = FindMaxDistance(pointSet, 0);
	float maxDistance = edgeNode.Weight;
	// search second max distance between points,
	// equals to another isosceles
	EdgeNode edgeNodeAnother = FindMaxDistance(pointSet, 
			maxDistance);
	//feature = 0;
	if (edgeNode.V1 == edgeNodeAnother.V1 )
			{
				car.set_first(edgeNode.V1);
				car.set_second(edgeNode.V2);
				car.set_third(edgeNodeAnother.V2);
			}
			else if (edgeNode.V1 == edgeNodeAnother.V2 )
			{
				car.set_first(edgeNode.V1);
				car.set_second(edgeNode.V2);	
				car.set_third(edgeNodeAnother.V1);		
			}  
			else if (edgeNode.V2 == edgeNodeAnother.V2 )
			{
				car.set_first(edgeNode.V2);
				car.set_second(edgeNode.V1);	
				car.set_third(edgeNodeAnother.V1);		
			}  
			else if (edgeNode.V2 == edgeNodeAnother.V1 )
			{
				car.set_first(edgeNode.V2);
				car.set_second(edgeNode.V1);	
				car.set_third(edgeNodeAnother.V2);		
			}  

	/*
	 * if the side edge is the longest, then
	 * the bottom edge is third of length.
	 *
	 */
/*	switch(SIDE)
	{
		case SIDE:
			if (edgeNode.V1 == edgeNodeAnother.V1 |
					edgeNode.V1 == edgeNodeAnother.V2)
			{
				car.set_first(edgeNode.V1);
				car.set_second(edgeNode.V2);
			}
			else
			{
				car.set_first(edgeNode.V2);
				car.set_second(edgeNode.V1);			
			}  
			car.set_third(edgeNodeAnother.V2);
			break;
		case BOTTOM:
			if (edgeNodeAnother.V1 == edgeNode.V1 |
					edgeNodeAnother.V1 == edgeNode.V2)
			{
				car.set_first(edgeNodeAnother.V2);
			}
			else
			{
				car.set_first(edgeNodeAnother.V1);
			}
			car.set_second(edgeNode.V2);
			car.set_third(edgeNode.V1);
			break;
	}
*/	
}

// from distance to judge the neigbhouring points
bool NeighbourPoint(Point2f pointA, Point2f pointB)
{
	bool flag = true;
	float distance = GetPixelDistance(pointA, pointB);
	if (distance < 4)
		return flag;
	flag = false;
	return flag;
}

bool Cmp(const Point2f a, const Point2f b){
	if (a.x > b.x)
		return 0;
	if (a.y > b.y)
		return 0;
    return 1;
}

/**
 *
 * search the corresponding car in the set of car
 * output data: Car lastCar
 *
 */
bool Exist(Car& car, vector<Car>& carStateSet, Car& lastCar)
{
	for(auto iter = carStateSet.begin(); 
		iter != carStateSet.end();)
	{
		if (car.get_marker() == (*iter).get_marker())
		{
			lastCar = *iter;
			return true;
		}

		iter ++;
	}
	return false;
}

void DeleteCar(Car& car, vector<Car>& carStateSet)
{
    for(auto iter = carStateSet.begin(); 
            iter != carStateSet.end();)
    {
        if (car.get_marker() == (*iter).get_marker())
        {
            iter = carStateSet.erase(iter);
            return;
        }
        iter ++;
    }
}
