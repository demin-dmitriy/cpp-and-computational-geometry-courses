#include "geometry.h"

#include <cmath>
#include <cfloat>
#include <cstdio>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;
using namespace geometry;

template<typename OutputIterator>
OutputIterator rasterize(point a, point b, OutputIterator out)
{
    if (b.x < a.x)
    {
        swap(a, b);
    }
    int end_x = floor(b.x);
    int end_y = floor(b.y);
    int current_y = floor(a.y);
    if (a.y <= b.y)
    {
        for (int i = floor(a.x); i != end_x; ++i)
        {
            *out++ = point(i, current_y);
            while (true)
            {
                double t1 = (current_y + 1 - a.y) * (b.x - a.x);
                double t2 = (i + 1 - a.x) * (b.y - a.y);
                if (abs(t1 - t2) < 4 * DBL_EPSILON * (abs(t1) + abs(t2)))
                {
                    clog << "ups";
                }
                double val = t1 - t2;
                if (val > 0)
                {
                    break;
                }
                ++current_y;
                if (val != 0)
                {
                    *out++ = point(i, current_y);
                }
            }
        }
        while (current_y != end_y)
        {
            *out++ = point(end_x, current_y);
            ++current_y;
        }
    }
    else
    {
        for (int i = floor(a.x); i != end_x; ++i)
        {
            *out++ = point(i, current_y);
            while (true)
            {
                double t1 = (current_y - a.y) * (b.x - a.x);
                double t2 = (i + 1 - a.x) * (b.y - a.y);
                if (abs(t1 - t2) < 4 * DBL_EPSILON * (abs(t1) + abs(t2)))
                {
                    clog << "ups";
                }
                double val = t1 - t2;
                if (val <= 0)
                {
                    break;
                }
                --current_y;
                *out++ = point(i, current_y);
            }
        }
        while (current_y != end_y)
        {
            *out++ = point(end_x, current_y);
            --current_y;
        }
    }
    *out++ = point(end_x, end_y);
    return out;
}

int run(istream& in, ostream& out)
{
    point a, b;
    in >> a >> b;
    vector<point> out_v;
    out_v.reserve(abs(floor(b.x) - floor(a.x)) + abs(floor(b.y) - floor(a.y)) + 2);
    rasterize(a, b, back_inserter(out_v));
    sort(out_v.begin(), out_v.end(), [](point a, point b){return (a.x != b.x) ? a.x < b.x : a.y < b.y;});
    out << out_v.size() << '\n';
    copy(out_v.begin(), out_v.end(), ostream_iterator<point>(out, "\n"));
    return 0;
}

int main()
{
    return run(cin, cout);
}
