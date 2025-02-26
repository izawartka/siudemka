#pragma once
#include "common.h"
#include "point.h"

class SGE_PointUtils {
public:
	static void worldToScreen(SGE_Point& point);
	static double getDistance(SGE_Point& p1, SGE_Point& p2);
	static double getDistanceSq(SGE_Point& p1, SGE_Point& p2);
	static double getAngleXY(SGE_Point& p1, SGE_Point& p2);
	static SGE_Point getMiddlePoint(SGE_Point& p1, SGE_Point& p2);
	static void rotatePoint(SGE_Point& point, double angle);
	static SGE_Point add(SGE_Point& p1, SGE_Point& p2);
	static bool equals(SGE_Point& p1, SGE_Point& p2);
	static SGE_Point lerp(SGE_Point& p1, SGE_Point& p2, double t);
};
