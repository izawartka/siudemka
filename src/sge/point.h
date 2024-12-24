#pragma once
#include "common.h"

struct SGE_Point {
	double x;
	double y;
	double z;

	SGE_Point() : x(0), y(0), z(0) {}
	SGE_Point(double x, double y, double z) : x(x), y(y), z(z) {}
};
