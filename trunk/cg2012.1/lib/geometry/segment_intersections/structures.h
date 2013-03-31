#ifndef GEOMETRY_STRUCTURES_H
#define GEOMETRY_STRUCTURES_H

#include "geometry/workaround/boost_intrusive_set.hpp"
#include "geometry/segment_intersections/ordered_segment.h"
#include "geometry/segment_intersections/event_queue.h"

namespace geometry
{

struct intersection_t
{
    std::vector<segment_t const*> segments;
};

using detail::ordered_segment_t;

typedef boost::intrusive::multiset<ordered_segment_t> segments_status_t;

using detail::segments_event_queue_t;

} // end of namespace geometry

#endif // GEOMETRY_STRUCTURES_H
