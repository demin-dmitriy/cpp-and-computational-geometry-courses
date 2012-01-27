#include "arithmetic.h"
struct point
{
	double x;
	double y;
	point(double const x, double const y):
		x(x),
		y(y)
	{}
};

int turn(point const& a1, point const& a2, point const& b);
bool is_intersect(point const a1, point const a2, point const b1, point const b2);