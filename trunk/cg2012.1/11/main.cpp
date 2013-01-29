#include "geometry.h"

#include <iostream>
#include <vector>

using namespace geometry;
using namespace std;

int run(istream& in, ostream& out)
{
    int n;
    in >> n;
    vector<point_t> points(n);
    for (auto& p: points)
    {
        in >> p;
    }
    auto answer = diameter(points.begin(), points.end());
    out.precision(50);
    out << answer.first << '\n' << answer.second;
    return 0;
}

int main()
{
    return run(cin, cout);
}
