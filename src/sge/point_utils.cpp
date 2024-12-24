#include "point_utils.h"
#include "defs.h"

const double yScale = cos(SGE_CAMERA_ISO_ANGLE);

void SGE_PointUtils::worldToScreen(SGE_Point& point)
{
	double x = point.x;
	double y = point.y;
	double z = point.z;

	point.x = x * SGE_WORLD_SCALE;
	point.y = (y * yScale - z) * SGE_WORLD_SCALE;
	point.z = y + z; /// fix
}

double SGE_PointUtils::getDistance(SGE_Point& p1, SGE_Point& p2)
{
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	double dz = p2.z - p1.z;

	return sqrt(dx * dx + dy * dy + dz * dz);
}

double SGE_PointUtils::getAngleXY(SGE_Point& p1, SGE_Point& p2)
{
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;

	return atan2(dy, dx);
}

SGE_Point SGE_PointUtils::getMiddlePoint(SGE_Point& p1, SGE_Point& p2)
{
	SGE_Point point;
	point.x = (p1.x + p2.x) / 2;
	point.y = (p1.y + p2.y) / 2;
	point.z = (p1.z + p2.z) / 2;

	return point;
}

void SGE_PointUtils::rotatePoint(SGE_Point& point, double angle)
{
	double x = point.x;
	double y = point.y;

	point.x = x * cos(angle) - y * sin(angle);
	point.y = x * sin(angle) + y * cos(angle);
}

SGE_Point SGE_PointUtils::add(SGE_Point& p1, SGE_Point& p2)
{
	SGE_Point point;
	point.x = p1.x + p2.x;
	point.y = p1.y + p2.y;
	point.z = p1.z + p2.z;

	return point;
}

bool SGE_PointUtils::equals(SGE_Point& p1, SGE_Point& p2)
{
	return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
}

SGE_Point SGE_PointUtils::lerp(SGE_Point& p1, SGE_Point& p2, double t)
{
	SGE_Point point;
	point.x = p1.x + (p2.x - p1.x) * t;
	point.y = p1.y + (p2.y - p1.y) * t;
	point.z = p1.z + (p2.z - p1.z) * t;

	return point;
}
