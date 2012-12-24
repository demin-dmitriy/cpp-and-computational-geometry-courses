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
    vector<point> points(n);
    assert(n > 0);
    for (int i = 0; i != n; ++i)
    {
        in >> points[i];
    }
    vector<point> out_v;
    convex_hull(points.begin(), points.end(), back_inserter(out_v));
    int out_n = out_v.size();
    out << out_n << '\n';
    out.precision(50);
    copy(out_v.begin(), out_v.end(), ostream_iterator<point>(out, "\n"));
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
