#include "geometry.h"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int run(istream& in, ostream& out)
{
    using geometry::point_t;
    int n;
    in >> n;
    vector<point_t> points(n);
    for (auto& p: points)
    {
        in >> p;
    }
    vector<point_t> points_copy(points);
    geometry::circle_t circle = geometry::mindisk(points.begin(), points.end());
    out << circle.points_used << '\n';
    for (int i = 0; i != circle.points_used; ++i)
    {
        int index =
                find(points_copy.begin(), points_copy.end(), circle.points[i])
                - points_copy.begin() + 1;
        out << index << ' ';
    }
    return 0;
}

int main()
{
    return run(cin, cout);
}
