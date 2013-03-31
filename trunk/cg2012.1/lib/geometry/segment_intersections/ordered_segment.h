#ifndef GEOMETRY_SEGMENT_INTERSECTIONS_ORDERED_SEGMENT_H
#define GEOMETRY_SEGMENT_INTERSECTIONS_ORDERED_SEGMENT_H

#include <algorithm>
#include <tuple>

#include "geometry/workaround/boost_intrusive_set.hpp"

#include "geometry/segment_base.h"

namespace geometry
{

namespace detail
{

// Segment where point 'a' is less or equal to the point 'b'.
struct ordered_segment_t : public boost::intrusive::set_base_hook<>
{
    point_t l;
    point_t r;
    ordered_segment_t() = default;
    ordered_segment_t(ordered_segment_t const&) = default;
    ordered_segment_t(segment_t const& o)
    {
        std::tie(l, r) = std::minmax(o.a, o.b);
    }
};

} // end of namespace detail

} // end of namespace geometry

#endif // GEOMETRY_SEGMENT_INTERSECTIONS_ORDERED_SEGMENT_H
