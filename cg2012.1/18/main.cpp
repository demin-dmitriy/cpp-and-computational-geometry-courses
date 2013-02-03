#include "geometry.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace geometry;

int run(istream& in, ostream& out)
{
    int n;
    in >> n;
    vector<point_t> points(n);
    for (auto p: points)
    {
        in >> p;
    }
    voronoi_diagram vd(points.begin(), points.end());

    // output
    throw "Not implemented jet";
    return 0;
}

int main()
{
    return run(cin, cout);
}
