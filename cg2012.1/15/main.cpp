#include "geometry/segments_intersection.h"

#include <iostream>
#include <iterator>
#include <vector>

using namespace geometry;

int run(std::istream& in, std::ostream& out)
{
    int n;
    in >> n;
    std::vector<segment_t> segments(n);
    for (auto& s: segments)
    {
        in >> s;
    }
    std::vector<intersection_t> intersections;
    find_intersections(segments.begin(),
                       segments.end(),
                       std::back_inserter(intersections));
    out << intersections.size() << '\n';
    segment_t const* first = &segments.front();
    for (auto& intersection: intersections)
    {
        out << intersection.segments.size();
        for (auto segment: intersection.segments)
        {
            out << ' ' << 1 + std::distance(first, segment);
        }
        out << '\n';
    }
    return 0;
}

int main()
{
    return run(std::cin, std::cout);
}
