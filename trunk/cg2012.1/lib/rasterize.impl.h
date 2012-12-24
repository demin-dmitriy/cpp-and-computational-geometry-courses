#include <cmath>

#include <algorithm>

namespace geometry
{

template<typename OutputIterator>
OutputIterator rasterize(point a, point b, OutputIterator out)
{
    if (b.x < a.x)
    {
        std::swap(a, b);
    }
    int end_x = floor(b.x);
    int end_y = floor(b.y);
    int current_y = floor(a.y);
    if (a.y <= b.y)
    {
        for (int i = floor(a.x); i != end_x; ++i)
        {
            *out++ = cell(i, current_y);
            while (true)
            {
                int turn = left_turn(b, a, point(i + 1, current_y + 1));
                if (turn < 0)
                {
                    break;
                }
                ++current_y;
                if (turn != 0)
                {
                    *out++ = cell(i, current_y);
                }
            }
        }
        while (current_y != end_y)
        {
            *out++ = cell(end_x, current_y);
            ++current_y;
        }
    }
    else
    {
        for (int i = floor(a.x); i != end_x; ++i)
        {
            *out++ = cell(i, current_y);
            while (left_turn(b, a, point(i + 1, current_y)) < 0)
            {
                --current_y;
                *out++ = cell(i, current_y);
            }
        }
        while (current_y != end_y)
        {
            *out++ = cell(end_x, current_y);
            --current_y;
        }
    }
    *out++ = cell(end_x, end_y);
    return out;
}

}
