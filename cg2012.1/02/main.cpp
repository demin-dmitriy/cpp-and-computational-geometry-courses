#include "geometry.h"

#include <cmath>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

int run(istream& in, ostream& out)
{
    typedef geometry::cell cell;
    geometry::point a, b;
    in >> a >> b;
    vector<cell> out_v;
    out_v.reserve(abs(static_cast<int>(floor(b.x))
                      - static_cast<int>(floor(a.x)))
                  + abs(static_cast<int>(floor(b.y))
                        - static_cast<int>(floor(a.y)))
                  + 2);
    rasterize(a, b, back_inserter(out_v));
    sort(out_v.begin(), out_v.end(),
         [](cell a, cell b){return (a.x != b.x) ? a.x < b.x : a.y < b.y;});
    out << out_v.size() << '\n';
    copy(out_v.begin(), out_v.end(), ostream_iterator<cell>(out, "\n"));
    return 0;
}

int main()
{
    return run(cin, cout);
}
