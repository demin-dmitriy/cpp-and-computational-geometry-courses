#include <cassert>

#include <algorithm>
#include <iterator>
#include <fstream>
#include <vector>

#include "geometry/convex_hull.h"

using namespace geometry;
using namespace std;

streamsize const output_precision = 50;

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
    out.precision(output_precision);
    copy(out_v.begin(), out_v.end(), ostream_iterator<point_t>(out, "\n"));
    return 0;
}

int main(int argc, char** argv)
{
    return run(cin, cout);
}
