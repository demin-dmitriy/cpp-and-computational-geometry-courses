#ifndef GEOMETRY_DIAMETER_H
#define GEOMETRY_DIAMETER_H

#include <iterator>
#include <vector>

#include "geometry/convex_hull.h"
#include "geometry/geometry_fwd.h"

namespace geometry
{

// todo: rewrite
template<typename BidirectionalIterator>
std::pair<point_t, point_t> diameter(
        BidirectionalIterator first,
        BidirectionalIterator last)
{
    std::vector<point_t> candidates;
    convex_hull(first, last, back_inserter(candidates));
    if (candidates.size() <= 2)
    {
        return std::make_pair(candidates.front(), candidates.back());
    }

    auto p = --candidates.end();
    auto p_next = candidates.begin();
    auto q = p_next;
    auto q_next = q;
    ++q_next;
    auto end = candidates.end();
    while (q_next != end
           && closer == compare_distance_to_line(*p, *p_next, *q, *q_next))
    {
        q = q_next;
        ++q_next;
    }

    auto p_max = p_next;
    auto q_max = p_next;
    while (true)
    {
        p = p_next;
        ++p_next;
        if (compare_distance(*p_max, *q_max, *p, *q) < 0)
        {
            p_max = p;
            q_max = q;
        }
        if (q_next == end)
        {
            return std::make_pair(*p_max, *q_max);
        }
        while (closer == compare_distance_to_line(*p, *p_next, *q, *q_next))
        {
            q = q_next;
            ++q_next;
            if (compare_distance(*p_max, *q_max, *p, *q) < 0)
            {
                p_max = p;
                q_max = q;
            }
            if (q_next == end)
            {
                break;
            }
        }
    }
}

} // end of namespace geometry

#endif // GEOMETRY_DIAMETER_H
