#ifndef GEOMETRY_CIRCLE_H
#define GEOMETRY_CIRCLE_H

#include "geometry/geometry_fwd.h"
#include "geometry/point_base.h"

namespace geometry
{

struct circle_t
{
    point_t points[3];
    int points_used;

    circle_t(point_t a, point_t b, point_t c)
        : points_used(3)
    {
        points[0] = a;
        if (right == turn(a, b, c))
        {
            points[1] = c;
            points[2] = b;
        }
        else
        {
            points[1] = b;
            points[2] = c;
        }
    }

    circle_t(point_t a, point_t b)
        : points({a, b})
        , points_used(2)
    {}

    circle_t(point_t a)
        : points({a})
        , points_used(1)
    {
        points[0] = a;
    }

    circle_t()
        : points({point_t(0, 0)})
        , points_used(0)
    {}

    void add_point(point_t const& d)
    {
        //assert(points_used < 3);
        if (points_used == 2 && right == turn(points[0], points[1], d))
        {
            points[2] = points[1];
            points[1] = d;
        }
        else
        {
            points[points_used] = d;
        }
        ++points_used;
    }

    bool contains(point_t x)
    {
        switch (points_used)
        {
        default:
        case 0:
            return false;
        case 1:
            return x == points[0];
        case 2:
            return in_circle(points[0], points[1], x) != outside;
        case 3:
            return in_circle(points[0], points[1], points[2], x) != outside;
        }
    }
};

} // end of namespace geometry

#endif // GEOMETRY_CIRCLE_H
