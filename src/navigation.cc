#include "navigation.h"
/*
 * this function to realize navigating the specific point by
 * adjusting continually target slope and judging the distance.
 * Once the distance is less than 5px, the navigation will end 
 * and set the target point is (0, 0) 
 *
 * Also, if you want to close the navigation function, you just
 * set its target point is (0, 0).
 *
 */
int NavigateTargetPoint(Car &car)
{    
    // the center of circumcircle 
    Point2f current_point = car.get_median_point(); 
    Point2f target_point = car.get_target_point();
    // exit navigation 
    if (target_point.x == 0 & target_point.y == 0)
        return 0;
    // the median point between two points
    float orientation_slope = GetSlope(current_point, target_point);
    car.set_target_slope(orientation_slope);
    float distance = GetPixelDistance(current_point, target_point);
    if (distance < 5) // about 10mm, 1px = 2mm
    {
        cout << "Get Goal!" << endl;
        car.set_target_point(Point2f(0, 0));
        car.set_target_speed(0);
        return 0;
    }
    return -1;
}
