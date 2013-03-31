#ifndef GEOMETRY_RASTERIZE_H
#define GEOMETRY_RASTERIZE_H

#include <cmath>

#include <algorithm>

#include "geometry/geometry_fwd.h"
#include "geometry/point_base.h"

namespace geometry
{

template<typename OutputIterator>
OutputIterator rasterize(point_t a, point_t b, OutputIterator out)
{
    if (b.x < a.x)
    {
        std::swap(a, b);
    }
    int end_x = static_cast<int>(floor(b.x));
    int end_y = static_cast<int>(floor(b.y));
    int current_y = static_cast<int>(floor(a.y));
    if (a.y <= b.y)
    {
        for (int i = static_cast<int>(floor(a.x)); i != end_x; ++i)
        {
            *out++ = cell_t(i, current_y);
            while (true)
            {
                turn_t orient = turn(b, a, point_t(i + 1, current_y + 1));
                if (right == orient)
                {
                    break;
                }
                ++current_y;
                if (collinear != orient)
                {
                    *out++ = cell_t(i, current_y);
                }
            }
        }
        while (current_y != end_y)
        {
            *out++ = cell_t(end_x, current_y);
            ++current_y;
        }
    }
    else
    {
        for (int i = static_cast<int>(floor(a.x)); i != end_x; ++i)
        {
            *out++ = cell_t(i, current_y);
            while (right == turn(b, a, point_t(i + 1, current_y)))
            {
                --current_y;
                *out++ = cell_t(i, current_y);
            }
        }
        while (current_y != end_y)
        {
            *out++ = cell_t(end_x, current_y);
            --current_y;
        }
    }
    *out++ = cell_t(end_x, end_y);
    return out;
}

} // end of namespace geometry

#endif // GEOMETRY_RASTERIZE_H
