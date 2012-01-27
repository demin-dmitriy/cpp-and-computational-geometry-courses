#include "geometry.h"
#include <algorithm>
#include <math.h>
#include <cfloat>

int robust_turn(point const a1, point const a2, point const b)
{
	// a2.x * b.y - a2.x * a1.y - a1.x * b.y - a2.y * b.x + a2.y * a1.x + a1.y * b.x
	using namespace adaptive_arithmetic;
	double result[12];
	
	two_product(a2.x, b.y, result[0], result[1]);
	two_product(-a2.x, a1.y, result[2], result[3]);
	two_product(-a1.x, b.y, result[4], result[5]);
	two_product(-a2.y, b.x, result[6], result[7]);
	two_product(a2.y, a1.x, result[8], result[9]);
	two_product(a1.y, b.x, result[10], result[11]);

	expansion_sum<2, 2>(result, result + 2);
	expansion_sum<2, 2>(result + 4, result + 6);
	expansion_sum<2, 2>(result + 8, result + 10);
	
	expansion_sum<4, 4>(result, result + 4);
	expansion_sum<8, 4>(result, result + 8);

	return sign<12>(result);
}

int fast_turn(point const a1, point const a2, point const b, int& is_reliable)
{
	double t1 = (a2.x - a1.x) * (b.y - a1.y);
	double t2 = (a2.y - a1.y) * (b.x - a1.x);
	if (abs(t1 - t2) > 4 * DBL_EPSILON * (abs(t1) + abs(t2)))
	{
		return (t1 - t2 > 0) ? 1 : -1;
	}
	--is_reliable;
	return 0; 
}

bool check_bounding_box(point const a1, point const a2, point const b1, point const b2)
{
	using namespace std;

	double const min_x1 = min(a1.x, a2.x);
	double const min_y1 = min(a1.y, a2.y);
	double const min_x2 = min(b1.x, b2.x);
	double const min_y2 = min(b1.y, b2.y);

	double const max_x1 = max(a1.x, a2.x);
	double const max_y1 = max(a1.y, a2.y);
	double const max_x2 = max(b1.x, b2.x);
	double const max_y2 = max(b1.y, b2.y);

	return !((max_x2 < min_x1) || (max_x1 < min_x2)) && !((max_y2 < min_y1) || (max_y1 < min_y2));
}

bool is_intersect(point const a1, point const a2, point const b1, point const b2)
{
	if (check_bounding_box(a1, a2, b1, b2))
	{
		int reliable = 0;
		if (fast_turn(a1, a2, b1, reliable) * fast_turn(a1, a2, b2, reliable) <= 0 && fast_turn(b1, b2, a1, reliable) * fast_turn(b1, b2, a2, reliable) <= 0)
		{
			if (reliable == 0)
			{
				return true;
			}
		}
		else
		{
			if (reliable == 0)
			{
				return false;
			}
		}

		if (robust_turn(a1, a2, b1) * robust_turn(a1, a2, b2) <= 0 && robust_turn(b1, b2, a1) * robust_turn(b1, b2, a2) <= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}