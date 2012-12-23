#include "geometry.h"

#include <cfloat>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "robust_arithmetic.h"

using namespace geometry;

namespace
{

// Returns sign of the expression
// a2.x * b.y - a2.x * a1.y - a1.x * b.y - a2.y * b.x + a2.y * a1.x + a1.y * b.x
int robust_turn(point const a1, point const a2, point const b)
{
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

int fast_turn(point const a1, point const a2, point const b)
{
    double t1 = (a2.x - a1.x) * (b.y - a1.y);
    double t2 = (a2.y - a1.y) * (b.x - a1.x);
    if (abs(t1 - t2) > 4 * DBL_EPSILON * (abs(t1) + abs(t2)))
    {
        return (t1 > t2) ? 1 : -1;
    }
    return 0; 
}

bool check_bounding_box(
        point const a1, point const a2, point const b1, point const b2)
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

    return !((max_x2 < min_x1) || (max_x1 < min_x2))
                && !((max_y2 < min_y1) || (max_y1 < min_y2));
}

// Returns
// (a2.x * b.y - a2.x * c.y - a1.x * b.y + a1.x * c.y - a2.y * b.x + a2.y * c.x
//          + a1.y * b.x - a1.y * c.x > 0)
bool robust_farther_than(
        point const a1, point const a2, point const b, point const c)
{
    using namespace adaptive_arithmetic;
    double result[16];
    
    two_product(a2.x, b.y, result[0], result[1]);
    two_product(-a2.x, c.y, result[2], result[3]);
    two_product(-a1.x, b.y, result[4], result[5]);
    two_product(a1.x, c.y, result[6], result[7]);
    two_product(-a2.y, b.x, result[8], result[9]);
    two_product(a2.y, c.x, result[10], result[11]);
    two_product(a1.y, b.x, result[12], result[13]);
    two_product(-a1.y, c.x, result[14], result[15]);
    
    expansion_sum<2, 2>(result, result + 2);
    expansion_sum<2, 2>(result + 4, result + 6);
    expansion_sum<2, 2>(result + 8, result + 10);
    expansion_sum<2, 2>(result + 12, result + 14);
    
    expansion_sum<4, 4>(result, result + 4);
    expansion_sum<4, 4>(result + 8, result + 12);
    expansion_sum<8, 8>(result, result + 8);

    return sign<16>(result) == 1;
}

int fast_farther_than(
        point const a1, point const a2, point const b, point const c)
{
    double t1 = (a2.x - a1.x) * (b.y - c.y);
    double t2 = (a2.y - a1.y) * (b.x - c.x);
    if (abs(t1 - t2) > 4 * DBL_EPSILON * (abs(t1) + abs(t2)))
    {
        return (t1 > t2) ? true : false;
    }
    return -1; 
}

} // end of namespace

// Returns orientation of point b relative to vector a2a1.
int geometry::left_turn(point const a1, point const a2, point const b)
{
    int res = fast_turn(a1, a2, b);
    if (res == 0)
    {
        res = robust_turn(a1, a2, b);
    }
    return res;
}

bool geometry::intersect(
        point const a1, point const a2, point const b1, point const b2)
{
    if (check_bounding_box(a1, a2, b1, b2)
            && left_turn(a1, a2, b1) * left_turn(a1, a2, b2) <= 0
            && left_turn(b1, b2, a1) * left_turn(b1, b2, a2) <= 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool geometry::farther_than(
        point const a1, point const a2, point const b, point const c)
{
    int res = fast_farther_than(a1, a2, b, c);
    if (res == -1)
    {
        res = robust_farther_than(a1, a2, b, c);
    }
    return res;
}

std::ostream& operator<<(std::ostream& stream, point const& p)
{
    stream << p.x << ' ' << p.y;
    return stream;
}

std::istream& operator>>(std::istream& stream, point& p)
{
    stream >> p.x >> p.y;
    return stream;
}
