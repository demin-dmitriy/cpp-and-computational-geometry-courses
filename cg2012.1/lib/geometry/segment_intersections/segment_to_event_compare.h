#ifndef GEOMETRY_SEGMENTS_INTERSECTIONS_SEGMENT_TO_EVENT_COMPARE_H
#define GEOMETRY_SEGMENTS_INTERSECTIONS_SEGMENT_TO_EVENT_COMPARE_H

#include <functional>

#include "geometry/segment_intersections/event_queue.h"
#include "geometry/predicate.h"

namespace geometry
{

namespace detail
{

// Compares y-coordinate of point where event takes place and y-coordinate of
// segment's point that has the same x-coordinate.
struct segment_to_event_compare_t : boost::static_visitor<int>
{
    bool operator()(ordered_segment_t const& segment,
                    event_point_t const& point) const
    {
        return 1 == compare(point, segment);
    }

    bool operator()(event_point_t const& point,
                    ordered_segment_t const& segment) const
    {
        return -1 == compare(point, segment);
    }

    int compare(event_point_t const& point,
                ordered_segment_t const& segment) const
    {
        using namespace std::placeholders;
        return boost::apply_visitor(std::bind(*this, _1, segment), point);
    }

    int operator()(point_t const& point, ordered_segment_t const& segment) const
    {
        return turn(segment.r, segment.l, point);
    }

    int operator()(intersection_descriptor_t const& i,
                   ordered_segment_t const& s) const
    {
        using namespace geometry::predicate;
        auto expression = []()
        {
            variable_t<0> alx;
            variable_t<1> aly;
            variable_t<2> arx;
            variable_t<3> ary;
            variable_t<4> blx;
            variable_t<5> bly;
            variable_t<6> brx;
            variable_t<7> bry;
            variable_t<8> slx;
            variable_t<9> sly;
            variable_t<10> srx;
            variable_t<11> sry;

            decltype(get_intersection_parameters(
                         alx, aly, arx, ary,
                         blx, bly, brx, bry)) intersection;
            decltype(intersection.numerator_x) num_x;
            decltype(intersection.numerator_y) num_y;
            decltype(intersection.denumerator) den;
            return determinant(srx - slx        , sry - sly,
                               num_x - den * slx, num_y - den * sly);
        };

        predicate_t<decltype(expression()),
                double_step_t,
                interval_step_t,
                gmp_step_t> p;
        return p(i.a->l.x, i.a->l.y, i.a->r.x, i.a->r.y,
                 i.b->l.x, i.b->l.y, i.b->r.x, i.b->r.y,
                 s.l.x, s.l.y, s.r.x, s.r.y);
    }
};

} // end of namespace detail

} // end of namespace geometry

#endif // GEOMETRY_SEGMENTS_INTERSECTIONS_SEGMENT_TO_EVENT_COMPARE_H
