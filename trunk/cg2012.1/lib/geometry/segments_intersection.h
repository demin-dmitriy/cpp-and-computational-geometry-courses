#ifndef GEOMETRY_SEGMENTS_INTERSECTION_H
#define GEOMETRY_SEGMENTS_INTERSECTION_H

#include <algorithm>
#include <vector>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include "geometry/segment_intersections/structures.h"
#include "segment_intersections/segment_to_event_compare.h"

// temp
#include <iostream>
namespace geometry
{

namespace detail
{

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

    ordered_segment_t front()
    {
        return segments_store.front();
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

inline void check_for_intersection(segments_status_t::iterator it,
                            segments_event_queue_t& events)
{
    auto const& upper_segment = *(it--);
    auto const& lower_segment = *it;
    if (will_intersect(lower_segment, upper_segment))
    {
        events[intersection_descriptor_t(&upper_segment, &lower_segment)];
    }
}

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
    int n = 0;
    for (auto event = event_queue.begin(), end = event_queue.end();
         event != end;
         event = event_queue.erase(event))
    {
        auto& event_point = event->first;
        auto& left_endpoints = event->second.left_endpoints;

        intersection_t intersection;
        for (auto& segment: left_endpoints)
        {
            intersection.segments.push_back(segments.get(*segment));
        }

        // --Separate--
        auto range = status.equal_range(event->first,
                                        detail::segment_to_event_compare_t());

        if (event_point.which() == 0) //todo: refractor
        {
            ++n;
            if (n % 4000 == 0)
            {
                std::cerr << n << '\n';
            }

            point_t p = boost::get<point_t>(event->first);
            while (range.first != range.second && range.first->r == p)
            {
                intersection.segments.push_back(segments.get(*range.first));
                range.first = status.erase(range.first);
            }
            for (auto segment = range.first; segment != range.second;)
            {
                if (segment->r == p) // Then segment disappears after this event
                {
                    intersection.segments.push_back(segments.get(*segment));
                    segment = status.erase(segment);
                }
                else
                {
                    ++segment;
                }
            }
        }

        auto& first = range.first;
        auto last = range.second;
        // dereference iterator?
        auto left_endpoint = left_endpoints.begin();
        auto left_endpoints_end = left_endpoints.end();
        while (first != last && left_endpoint != left_endpoints_end)
        {
            while (left == turn((*left_endpoint)->l, (*left_endpoint)->r,
                                first->l, first->r))
            {
                last = status.insert_before(last, **left_endpoint);
                ++left_endpoint;
            }
            auto& segment_ref = *first;
            intersection.segments.push_back(segments.get(segment_ref));
            first = status.erase(first);
            last = status.insert_before(last, segment_ref);
        }

        if (first != last)
        {
            decltype(last) after_last;
            do
            {
                auto& segment_ref = *first;
                intersection.segments.push_back(segments.get(segment_ref));
                first = status.erase(first);
                after_last = last;
                last = status.insert_before(last, segment_ref);
            }
            while (first != after_last);
        }
        else
        {
            while (left_endpoint != left_endpoints_end)
            {
                last = status.insert_before(last, **left_endpoint);
                ++left_endpoint;
            }
        }
        // -----------


        if (intersection.segments.size() > 1)
        {
            out++ = intersection;
        }
        assert(event_point.which() == 0 || intersection.segments.size() > 1);
        if (range.second == last)
        {
            if (last != status.begin() && last != status.end())
            {
                check_for_intersection(last, event_queue);
            }
        }
        else
        {
            if (last != status.begin())
            {
                check_for_intersection(last, event_queue);
            }
            if (range.second != status.end())
            {
                check_for_intersection(range.second, event_queue);
            }
        }
    }
    assert(event_queue.empty());
    return out;
}

} // end of namespace geometry

#endif // GEOMETRY_SEGMENTS_INTERSECTION_H
