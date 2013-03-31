#ifndef GEOMETRY_CONVEX_HULL_H
#define GEOMETRY_CONVEX_HULL_H

#include <algorithm>
#include <functional>

#include "geometry/geometry_fwd.h"
#include "geometry/point_base.h"

namespace geometry
{

namespace detail
{

struct left_turn_pred
{
    bool operator()(point_t const& p)
    {
        return left == turn(p1, p2, p);
    }

    left_turn_pred(point_t const& p1, point_t const& p2)
        : p1(p1)
        , p2(p2)
    {}

    point_t const& p1;
    point_t const& p2;
};

} // end of namespace detail

template<typename BidirectionalIterator, typename OutputIterator>
void quick_hull_iteration_(
        BidirectionalIterator first,
        BidirectionalIterator last,
        point_t const p1,
        point_t const p2,
        OutputIterator& out)
{
    using namespace std;
    if (first != last)
    {
        point_t const mid_p = *std::max_element(
                    first,
                    last,
                    [&](point_t const a, point_t const b)
                    {
                        return closer == compare_distance_to_line(p2, p1, a, b);
                    });
        if (right == turn(p1, p2, mid_p))
        {
            auto const mid = std::partition(
                    first, last, detail::left_turn_pred(mid_p, p1));
            quick_hull_iteration_(first, mid, p1, mid_p, out);
            *out++ = mid_p;
            quick_hull_iteration_(mid, last, mid_p, p2, out);
        }
    }
}

template<typename BidirectionalIterator, typename OutputIterator>
OutputIterator convex_hull(
        BidirectionalIterator first,
        BidirectionalIterator last,
        OutputIterator out)
{
    if (first != last)
    {
        point_t const p1 = *std::min_element(first, last);
        point_t const p2 = *std::max_element(first, last);
        *out++ = p1;
        if (p1 != p2)
        {
            auto const mid = std::partition(
                        first, last, detail::left_turn_pred(p2, p1));
            quick_hull_iteration_(first, mid, p1, p2, out);
            *out++ = p2;
            quick_hull_iteration_(mid, last, p2, p1, out);
        }
    }
    return out;
}

} // end of namespace geometry

#endif // GEOMETRY_CONVEX_HULL_H
