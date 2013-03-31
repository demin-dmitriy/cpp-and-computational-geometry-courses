#ifndef GEOMETRY_DIAMETER_H
#define GEOMETRY_DIAMETER_H

#include <iterator>
#include <vector>

#include "geometry/convex_hull.h"
#include "geometry/geometry_fwd.h"

namespace geometry
{

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

    // Invariant: p_next == std::next(p) (except for the first time)
    auto p = std::prev(candidates.end());
    auto p_next = candidates.begin();
    // Invariant: q_next == std::next(q)
    auto q = p_next;
    auto q_next = std::next(q);
    auto end = candidates.end();

    while (q_next != end
           && closer == compare_distance_to_line(*p, *p_next, *q, *q_next))
    {
        q = q_next;
        ++q_next;
    }

    // p_max, q_max -- current most farthest points.
    auto p_max = p_next;
    auto q_max = p_next;

    auto update_maximum = [&]()
    {
        if (closer == compare_distance(*p_max, *q_max, *p, *q))
        {
            p_max = p;
            q_max = q;
        }
    };

    while (true)
    {
        p = p_next;
        ++p_next;
        update_maximum();
        if (q_next == end)
        {
            return std::make_pair(*p_max, *q_max);
        }
        while (closer == compare_distance_to_line(*p, *p_next, *q, *q_next))
        {
            q = q_next;
            ++q_next;
            update_maximum();
            if (q_next == end)
            {
                break;
            }
        }
    }
}

} // end of namespace geometry

#endif // GEOMETRY_DIAMETER_H
