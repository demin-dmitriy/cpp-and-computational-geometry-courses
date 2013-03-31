#ifndef GEOMETRY_SEGMENTS_INTERSECTION_H
#define GEOMETRY_SEGMENTS_INTERSECTION_H

#include <algorithm>
#include <vector>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include "geometry_fwd.h"
#include "geometry/segment_intersections/structures.h"
#include "segment_intersections/segment_to_event_compare.h"

namespace geometry
{

namespace detail
{

// Here will be stored all segments that will latter be in status tree (which is
// internally is boost::intrusive::set)
struct segment_store_t
{
    struct get_ptr
    {
        typedef segment_t const* result_type;
        segment_t const* operator()(segment_t const& s) const
        {
            return &s;
        }
    };

    typedef std::vector<ordered_segment_t> segments_store_t;
    typedef std::vector<segment_t const*> segments_source_t;
    typedef segments_store_t::iterator iterator;
    typedef boost::indirect_iterator<segments_source_t::iterator,
                                     segment_t const> indirect_it;

    template<typename ForwardIterator>
    segment_store_t(ForwardIterator first, ForwardIterator last)
        : segments_source(boost::make_transform_iterator(first, get_ptr()),
                          boost::make_transform_iterator(last, get_ptr()))
        , segments_store(indirect_it(segments_source.begin()),
                         indirect_it(segments_source.end()))
    {}

    iterator begin()
    {
        return segments_store.begin();
    }

    iterator end()
    {
        return segments_store.end();
    }

    segment_t const* get(ordered_segment_t& segment)
    {
        return segments_source[std::distance(&segments_store[0], &segment)];
    }

    segments_source_t segments_source;
    segments_store_t segments_store;
};

inline bool will_intersect(
        ordered_segment_t const& a, ordered_segment_t const& b)
{
    // if turn(...) == left then segments are diverge.
    return right == turn (a.l, a.r, b.l, b.r)
            && intersecting == intersect(a.l, a.r, b.l, b.r);
}

inline void check_for_new_event(segments_status_t::iterator it,
                            segments_event_queue_t& events)
{
    auto const& upper_segment = *it;
    auto const& lower_segment = *--it;
    if (will_intersect(lower_segment, upper_segment))
    {
        events[intersection_descriptor_t(&upper_segment, &lower_segment)];
    }
}

// At event point we have some segments that cross event point and some segments
// that start at this event point. Segments that cross the event point will
// reverse their order and segments that start at this point will be somewhere
// in between of first ones. We trying for one pass reverse order of first ones
// and merge in the second ones.
template<typename Iterator1, typename Iterator2>
Iterator1 reverse_segments_and_add_new(
        segments_status_t& status,
        Iterator1 to_reverse_begin, Iterator1 to_reverse_end,
        Iterator2 to_add_begin, Iterator2 to_add_end)
{
    auto prev_end = to_reverse_end;
    // Adds new segment in status tree.
    auto start_new_segment = [&]()
    {
        to_reverse_end = status.insert_before(to_reverse_end, *to_add_begin);
        ++to_add_begin;
    };

    // Takes segment from bottom and puts it on top.
    auto flip_segment = [&]()
    {
        auto& segment_ref = *to_reverse_begin;
        to_reverse_begin = status.erase(to_reverse_begin);
        prev_end = to_reverse_end;
        to_reverse_end = status.insert_before(to_reverse_end, segment_ref);
    };

    while (to_reverse_begin != prev_end && to_add_begin != to_add_end)
    {
        // Sort segments by angle.
        if (left == turn(to_add_begin->l,
                         to_add_begin->r,
                         to_reverse_begin->l,
                         to_reverse_begin->r))
        {
            start_new_segment();
        }
        else
        {
            flip_segment();
        }
    }
    while (to_add_begin != to_add_end)
    {
        start_new_segment();
    }
    while (to_reverse_begin != prev_end)
    {
        flip_segment();
    }
    // Return iterator to the most bottom segment.
    return to_reverse_end;
}

struct turn_comparator_t
{
    bool operator()(detail::ordered_segment_t const* a,
                    detail::ordered_segment_t const* b) const
    {
        return left == turn(a->l, a->r, b->l, b->r);
    }
};

} // end of namespace detail

template<typename ForwardIterator, typename OutputIterator>
OutputIterator find_intersections(ForwardIterator first,
                                  ForwardIterator last,
                                  OutputIterator out)
{
    detail::segment_store_t segments(first, last);

    segments_event_queue_t event_queue;
    for (auto& segment: segments)
    {
        event_queue[segment.l].left_endpoints.push_back(&segment);
        event_queue[segment.r]; // Insert if wasn't there.
    }

    segments_status_t status;
    for (auto event = event_queue.begin(), end = event_queue.end();
         event != end;
         event = event_queue.erase(event))
    {
        auto& event_point = event->first;
        auto& left_endpoints = event->second.left_endpoints;

        // Select all segments that end or cross the event point
        auto range = status.equal_range(event->first,
                                        detail::segment_to_event_compare_t());

        intersection_t intersection;

        // If this is point event (not intersection one), than there might
        // be segments that end there. So remove them and add to intersection.
        if (event_point.which() == detail::point_event)
        {
            point_t p = boost::get<point_t>(event->first);
            auto remove_segment = [&](decltype(range.first)& it)
            {
                intersection.segments.push_back(segments.get(*it));
                it = status.erase(it);
            };

            // Move range lower bound while removing segments from status tree.
            while (range.first != range.second
                   && range.first->r == p)
            {
                remove_segment(range.first);
            }
            // Remove all other segments.
            for (auto segment = range.first; segment != range.second;)
            {
                if (segment->r == p)
                {
                    remove_segment(segment);
                }
                else
                {
                    ++segment;
                }
            }
        }

        // Add segments to intersection
        for (auto& segment: left_endpoints)
        {
            intersection.segments.push_back(segments.get(*segment));
        }
        for (auto segment = range.first; segment!= range.second; ++segment)
        {
            intersection.segments.push_back(segments.get(*segment));
        }
        if (intersection.segments.size() > 1)
        {
            out++ = intersection;
        }
        assert(event_point.which() == 0 || intersection.segments.size() > 1);

        std::sort(left_endpoints.begin(),
                  left_endpoints.end(),
                  detail::turn_comparator_t());

        // Reorder crossing segments and add new ones.
        auto lowest = reverse_segments_and_add_new(
                    status,
                    range.first,
                    range.second,
                    boost::make_indirect_iterator(left_endpoints.begin()),
                    boost::make_indirect_iterator(left_endpoints.end()));

        auto highest = range.second;

        // Check for new intersections of segments that just became adjacent.
        if (lowest == highest)
        {
            if (lowest != status.begin() && lowest != status.end())
            {
                check_for_new_event(lowest, event_queue);
            }
        }
        else
        {
            if (lowest != status.begin())
            {
                check_for_new_event(lowest, event_queue);
            }
            if (highest != status.end())
            {
                check_for_new_event(highest, event_queue);
            }
        }
    }
    assert(event_queue.empty());
    return out;
}

} // end of namespace geometry

#endif // GEOMETRY_SEGMENTS_INTERSECTION_H
