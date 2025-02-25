#pragma once
#include "common.h"

struct SGE_Point {
	double x;
	double y;
	double z;

	SGE_Point() : x(0), y(0), z(0) {}
	SGE_Point(double x, double y, double z) : x(x), y(y), z(z) {}

	SGE_Point operator+(const SGE_Point& other) const {
		return SGE_Point(x + other.x, y + other.y, z + other.z);
	}

	SGE_Point operator-(const SGE_Point& other) const {
		return SGE_Point(x - other.x, y - other.y, z - other.z);
	}

	SGE_Point operator*(double scalar) const {
		return SGE_Point(x * scalar, y * scalar, z * scalar);
	}

	SGE_Point operator/(double scalar) const {
		return SGE_Point(x / scalar, y / scalar, z / scalar);
	}

	SGE_Point& operator+=(const SGE_Point& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	SGE_Point& operator-=(const SGE_Point& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	SGE_Point& operator*=(double scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	SGE_Point& operator/=(double scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	bool operator==(const SGE_Point& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	bool operator!=(const SGE_Point& other) const {
		return x != other.x || y != other.y || z != other.z;
	}
};
