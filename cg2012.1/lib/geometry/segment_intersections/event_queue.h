#ifndef GEOMETRY_SEGMENTS_INTERSECTIONS_EVENT_QUEUE_H
#define GEOMETRY_SEGMENTS_INTERSECTIONS_EVENT_QUEUE_H

#include <vector>
#include <map>

#include <boost/variant.hpp>

#include "geometry/point_base.h"
#include "geometry/predicate.h"
#include "geometry/segment_intersections/intersection_descriptor.h"

namespace geometry
{

namespace detail
{

typedef boost::variant<point_t, intersection_descriptor_t> event_point_t;

// Corresponds to what event_point_t::which() returns.
enum event_point_type_t
{
    point_event = 0,
    intersection_event = 1
};

// Comparator for points defined as events (either usual point_t or point of
// intersection of two segments.
struct event_point_comparator_t : public boost::static_visitor<bool>
{
    bool operator()(event_point_t const& a, event_point_t const& b) const
    {
        return boost::apply_visitor(*this, a, b);
    }

    bool operator()(point_t const& a, point_t const& b) const
    {
        return a < b;
    }

    bool operator()(point_t const& p, intersection_descriptor_t const& i) const
    {
        return -1 == point_to_intersection_compare(p, i);
    }

    bool operator()(intersection_descriptor_t const& i, point_t const& p) const
    {
        return 1 == point_to_intersection_compare(p, i);
    }

    bool operator()(intersection_descriptor_t const& i1,
                    intersection_descriptor_t const& i2) const
    {
        using namespace geometry::predicate;
        auto expression_x = []()
        {
            variable_t<0> i1alx;
            variable_t<1> i1aly;
            variable_t<2> i1arx;
            variable_t<3> i1ary;
            variable_t<4> i1blx;
            variable_t<5> i1bly;
            variable_t<6> i1brx;
            variable_t<7> i1bry;

            variable_t<8> i2alx;
            variable_t<9> i2aly;
            variable_t<10> i2arx;
            variable_t<11> i2ary;
            variable_t<12> i2blx;
            variable_t<13> i2bly;
            variable_t<14> i2brx;
            variable_t<15> i2bry;

            decltype(get_intersection_parameters(
                         i1alx, i1aly, i1arx, i1ary,
                         i1blx, i1bly, i1brx, i1bry)) i1;
            decltype(get_intersection_parameters(
                         i2alx, i2aly, i2arx, i2ary,
                         i2blx, i2bly, i2brx, i2bry)) i2;
            return i1.numerator_x * i2.denumerator
                    - i2.numerator_x * i1.denumerator;
        };
        auto expression_y = []()
        {
            variable_t<0> i1alx;
            variable_t<1> i1aly;
            variable_t<2> i1arx;
            variable_t<3> i1ary;
            variable_t<4> i1blx;
            variable_t<5> i1bly;
            variable_t<6> i1brx;
            variable_t<7> i1bry;

            variable_t<8> i2alx;
            variable_t<9> i2aly;
            variable_t<10> i2arx;
            variable_t<11> i2ary;
            variable_t<12> i2blx;
            variable_t<13> i2bly;
            variable_t<14> i2brx;
            variable_t<15> i2bry;

            decltype(get_intersection_parameters(
                         i1alx, i1aly, i1arx, i1ary,
                         i1blx, i1bly, i1brx, i1bry)) i1;
            decltype(get_intersection_parameters(
                         i2alx, i2aly, i2arx, i2ary,
                         i2blx, i2bly, i2brx, i2bry)) i2;
            return i1.numerator_y * i2.denumerator
                    - i2.numerator_y * i1.denumerator;
        };

        predicate_t<decltype(expression_x()),
                double_step_t,
                interval_step_t,
                gmp_step_t> p_x;
        predicate_result_t res_x = p_x(
                    i1.a->l.x, i1.a->l.y, i1.a->r.x, i1.a->r.y,
                    i1.b->l.x, i1.b->l.y, i1.b->r.x, i1.b->r.y,
                    i2.a->l.x, i2.a->l.y, i2.a->r.x, i2.a->r.y,
                    i2.b->l.x, i2.b->l.y, i2.b->r.x, i2.b->r.y);
        if (zero != res_x)
        {
            return negative == res_x;
        }

        predicate_t<decltype(expression_y()),
                double_step_t,
                interval_step_t,
                gmp_step_t> p_y;
        return negative == p_y(
                    i1.a->l.x, i1.a->l.y, i1.a->r.x, i1.a->r.y,
                    i1.b->l.x, i1.b->l.y, i1.b->r.x, i1.b->r.y,
                    i2.a->l.x, i2.a->l.y, i2.a->r.x, i2.a->r.y,
                    i2.b->l.x, i2.b->l.y, i2.b->r.x, i2.b->r.y);
    }

    int point_to_intersection_compare(
            point_t const& point,
            intersection_descriptor_t const& i) const
    {
        using namespace geometry::predicate;
        auto expression_x = []()
        {
            variable_t<0> px;
            variable_t<1> alx;
            variable_t<2> aly;
            variable_t<3> arx;
            variable_t<4> ary;
            variable_t<5> blx;
            variable_t<6> bly;
            variable_t<7> brx;
            variable_t<8> bry;

            decltype(get_intersection_parameters(
                         alx, aly, arx, ary,
                         blx, bly, brx, bry)) intersection;
            return px * intersection.denumerator - intersection.numerator_x;
        };

        auto expression_y = []()
        {
            variable_t<0> py;
            variable_t<1> alx;
            variable_t<2> aly;
            variable_t<3> arx;
            variable_t<4> ary;
            variable_t<5> blx;
            variable_t<6> bly;
            variable_t<7> brx;
            variable_t<8> bry;

            decltype(get_intersection_parameters(
                         alx, aly, arx, ary,
                         blx, bly, brx, bry)) intersection;
            return py * intersection.denumerator - intersection.numerator_y;
        };

        predicate_t<decltype(expression_x()),
                double_step_t,
                interval_step_t,
                gmp_step_t> p_x;
        predicate_result_t res_x = p_x(
                    point.x,
                    i.a->l.x, i.a->l.y, i.a->r.x, i.a->r.y,
                    i.b->l.x, i.b->l.y, i.b->r.x, i.b->r.y);
        if (zero != res_x)
        {
            return res_x;
        }

        predicate_t<decltype(expression_y()),
                double_step_t,
                interval_step_t,
                gmp_step_t> p_y;
        predicate_result_t res_y = p_y(
                    point.y,
                    i.a->l.x, i.a->l.y, i.a->r.x, i.a->r.y,
                    i.b->l.x, i.b->l.y, i.b->r.x, i.b->r.y);
        return res_y;
    }

};

struct event_info_t
{
    std::vector<ordered_segment_t*> left_endpoints;
};

typedef std::map< event_point_t
                , event_info_t
                , event_point_comparator_t
                > segments_event_queue_t;

} // end of namespace detail

} // end of namespace geometry

#endif // GEOMETRY_SEGMENTS_INTERSECTIONS_EVENT_QUEUE_H
