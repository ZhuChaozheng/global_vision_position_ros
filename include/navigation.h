#ifndef NAVIGATION_H
#define NAVIGATION_H
#include "car.h"

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
int NavigateTargetPoint(Car &car);


#endif //NAVIGATION_H
