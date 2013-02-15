#include <cmath>

#include <algorithm>

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
                int turn = left_turn(b, a, point_t(i + 1, current_y + 1));
                if (turn < 0)
                {
                    break;
                }
                ++current_y;
                if (turn != 0)
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
            while (left_turn(b, a, point_t(i + 1, current_y)) < 0)
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