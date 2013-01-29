#include "geometry.h"

#include <cassert>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>

using namespace geometry;
using namespace std;

int run(istream& in, ostream& out)
{
    int n;
    in >> n;
    vector<point_t> points(n);
    assert(n > 0);
    for (auto& p: points)
    {
        in >> p;
    }
    vector<point_t> out_v;
    convex_hull(points.begin(), points.end(), back_inserter(out_v));
    out << out_v.size() << '\n';
    out.precision(50);
    copy(out_v.begin(), out_v.end(), ostream_iterator<point_t>(out, "\n"));
    return 0;
}

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        ifstream in(argv[1]);
        return run(in, cout);
    }
    return run(cin, cout);
}
