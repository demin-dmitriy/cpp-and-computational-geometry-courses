#ifndef GEOMETRY_STRUCTURES_H
#define GEOMETRY_STRUCTURES_H

/*
 * Defines segments_status_t and segments_event_queue_t.
 */

#include <cassert>

#include <algorithm>
#include <functional>
#include <limits>
#include <map>
#include <tuple>

#include "geometry/workaround/boost_intrusive_set.hpp"
#include <boost/numeric/interval.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <gmpxx.h>

#include "geometry/geometry_fwd.h"
#include "geometry/math.h"
#include "geometry/point_base.h"
#include "geometry/segment_base.h"

namespace geometry
{

// Change to typedef?
struct intersection_t
{
    // Change pointers to iterators?
    std::vector<segment_t const*> segments;
};

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

inline bool interior_intersect(
        ordered_segment_t const& a, ordered_segment_t const& b)
{
    return intersecting == intersect(a.l, a.r, b.l, b.r);
}

template<typename Type>
void calculate_intersection_parameters_aux(
        ordered_segment_t const& a,
        ordered_segment_t const& b,
        Type& A1,
        Type& B1,
        Type& A2,
        Type& B2,
        Type& numerator_x,
        Type& numerator_y,
        Type& denumerator)
{
    /*
     * Two lines are defined as:
     * A1*x + B1*y + C1 = 0
     * A2*x + B2*y + C2 = 0
     * where:
     * A1 = a.l.y - a.r.y
     * B1 = a.r.x - a.l.x
     * C1 = a.l.x * (a.r.y - a.l.y) - a.r.y * (a.r.x - a.l.x)
     * and similar for A2, B2 and C2 (replace 'a' with 'b').
     * Solving linear system, we have:
     *       | C1 B1 |
     *       | C2 B2 |
     * x = - ---------
     *       | A1 B1 |
     *       | A2 B2 |
     *
     *       | A1 C1 |
     *       | A2 C2 |
     * y = - ---------
     *       | A1 B1 |
     *       | A2 B2 |
     * If denumerator == 0 thеn two lines are either collinear or coincide.
     * However this function used so that this is never a case.
     */
    Type alx = a.l.x;
    Type aly = a.l.y;
    Type arx = a.r.x;
    Type ary = a.r.y;
    Type blx = b.l.x;
    Type bly = b.l.y;
    Type brx = b.r.x;
    Type bry = b.r.y;

    A1 = aly - ary;
    B1 = arx - alx;
    Type C1 = alx * ary - aly * arx;
    A2 = bly - bry;
    B2 = brx - blx;
    Type C2 = blx * bry - bly * brx;

    numerator_x = -determinant(C1, B1,
                               C2, B2);
    numerator_y = -determinant(A1, C1,
                               A2, C2);
    denumerator = determinant(A1, B1,
                              A2, B2);
}

// todo: Check if it is all will be inlined.
template<typename Type>
void calculate_intersection_parameters(
        ordered_segment_t const& a,
        ordered_segment_t const& b,
        Type& numerator_x_out,
        Type& numerator_y_out,
        Type& denumerator_out)
{
    Type A1, B1, A2, B2;
    calculate_intersection_parameters_aux(a, b, A1, B1, A2, B2,
                                          numerator_x_out,
                                          numerator_y_out,
                                          denumerator_out);
}

inline void calculate_intersection_parameters(
        ordered_segment_t const& a,
        ordered_segment_t const& b,
        double& numerator_x_out,
        double& numerator_y_out,
        double& denumerator_out,
        double& numerator_x_roundoff_out,
        double& numerator_y_roundoff_out,
        double& denumerator_roundoff_out)
{
    using std::abs;
    double A1, B1, A2, B2;
    calculate_intersection_parameters_aux(a, b, A1, B1, A2, B2,
                                          numerator_x_out,
                                          numerator_y_out,
                                          denumerator_out);
    double C1_roundoff = abs(a.l.x * a.r.y) + abs(a.l.y * a.r.x);
    double C2_roundoff = abs(b.l.x * b.r.y) + abs(b.l.y * b.r.x);
    numerator_x_roundoff_out = determinant_roundoff(C1_roundoff, B1,
                                                    C2_roundoff, B2);
    numerator_y_roundoff_out = determinant_roundoff(A1, C1_roundoff,
                                                    A2, C2_roundoff);
    denumerator_roundoff_out = determinant_roundoff(A1, B1,
                                                    A2, B2);
}

struct intersection_descriptor_t
{
    intersection_descriptor_t(ordered_segment_t const* a,
                              ordered_segment_t const* b)
        : a(a)
        , b(b)
    {
        // Guarantee that denumerator will be positive.
        // todo: some explanations why so.
        if (right == turn(a->l, a->r, b->r))
        {
            std::swap(a, b);
        }
        calculate_intersection_parameters(*a, *b, cache_double.numerator_x,
                                          cache_double.numerator_y,
                                          cache_double.denumerator,
                                          cache_roundoff.numerator_x,
                                          cache_roundoff.numerator_y,
                                          cache_roundoff.denumerator);
        assert(cache_double.denumerator >= -cache_roundoff.denumerator);
    }

    void init_interval_cache() const
    {
        if (!cache_interval)
        {
            cache_interval = parameter_cache_t<interval_t>();
            auto& cache = *cache_interval;
            calculate_intersection_parameters(*a, *b, cache.numerator_x,
                                              cache.numerator_y,
                                              cache.denumerator);
        }
    }

    void init_gmp_cache() const
    {
        if (!cache_gmp)
        {
            mpq_class x, y, d;
            calculate_intersection_parameters(*a, *b, x, y, d);
            cache_gmp = point_base_t<mpq_class>();
            cache_gmp->x = x / d;
            cache_gmp->y = y / d;
        }
    }

    ordered_segment_t const* a;
    ordered_segment_t const* b;

    template<typename Type>
    struct parameter_cache_t
    {
        Type numerator_x;
        Type numerator_y;
        Type denumerator;
    };

    parameter_cache_t<double> cache_double;
    parameter_cache_t<double> cache_roundoff;
    using interval_t = boost::numeric::interval<double>;
    // todo: Check if using bool flag is faster.
    mutable boost::optional<parameter_cache_t<interval_t>> cache_interval;
    mutable boost::optional<point_base_t<mpq_class>> cache_gmp;
};

typedef boost::intrusive::multiset<ordered_segment_t> segments_status_t;

typedef boost::variant<point_t, intersection_descriptor_t> event_point_t;

struct event_point_comparator_t : public boost::static_visitor<bool>
{
    // todo: Refine on a little bit maybe so that comparision for y could
    // happen before enabling gmp.
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
        double res = i1.cache_double.numerator_x * i2.cache_double.denumerator
                - i2.cache_double.numerator_x * i1.cache_double.denumerator;
        double roundoff = 16 * std::numeric_limits<double>::epsilon()
                * (i1.cache_roundoff.numerator_x * i2.cache_roundoff.denumerator
                + i2.cache_roundoff.numerator_x * i1.cache_roundoff.denumerator);
        if (res < -roundoff)
        {
            return true;
        }
        if (res > roundoff)
        {
            return false;
        }
        // todo: Interval step here.
        return intersection_compare_gmp(i1, i2);
    }

    bool intersection_compare_gmp(intersection_descriptor_t const& i1,
                                  intersection_descriptor_t const& i2) const
    {
        i1.init_gmp_cache();
        i2.init_gmp_cache();
        return *i1.cache_gmp < *i2.cache_gmp;
    }

    int point_to_intersection_compare(
            point_t const& point,
            intersection_descriptor_t const& intersection) const
    {
        using namespace boost::numeric::interval_lib::compare::certain;
        if (!intersection.cache_interval)
        {
            using std::abs;
            double res = point.x * intersection.cache_double.denumerator
                    - intersection.cache_double.numerator_x;
            double const epsilon = std::numeric_limits<double>::epsilon();
            double roundoff = 4 * epsilon * (
                        abs(point.x * intersection.cache_roundoff.denumerator)
                        + intersection.cache_roundoff.numerator_x);

            if (res < -roundoff)
            {
                return -1;
            }
            if (res > roundoff)
            {
                return 1;
            }
        }

        intersection.init_interval_cache();
        auto interval_res = point.x * intersection.cache_interval->denumerator;
        if (interval_res.upper() < intersection.cache_interval->numerator_x.lower())
        {
            return -1;
        }
        if (interval_res.lower() > intersection.cache_interval->numerator_x.upper())
        {
            return 1;
        }

        return point_to_intersection_compare_gmp(point, intersection);
    }

    int point_to_intersection_compare_gmp(
            const point_t &point,
            const intersection_descriptor_t &intersection) const
    {
        intersection.init_gmp_cache();
        point_base_t<mpq_class> gmp_point(point);
        if (gmp_point < *intersection.cache_gmp)
        {
            return -1;
        }
        if (gmp_point == *intersection.cache_gmp)
        {
            return 0;
        }
        return 1;
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

// Compares y-coordinate of point where event takes place and y-coordinate of
// segment's point that has the same x-coordinate.
struct event_point_to_segment_compare_t : boost::static_visitor<int>
{
    int operator()(point_t const& point, ordered_segment_t const& segment) const
    {
        return turn(segment.r, segment.l, point);
    }

    // todo: Fix it.
    int operator()(intersection_descriptor_t const& intersection,
                   ordered_segment_t const& segment) const
    {
        using std::abs;
        auto& sl = segment.l;
        auto& sr = segment.r;
        auto& num_x = intersection.cache_double.numerator_x;
        auto& num_y = intersection.cache_double.numerator_y;
        auto& den = intersection.cache_double.denumerator;
        auto& num_x_err = intersection.cache_roundoff.numerator_x;
        auto& num_y_err = intersection.cache_roundoff.numerator_y;
        auto& den_err = intersection.cache_roundoff.denumerator;
        double const epsilon = std::numeric_limits<double>::epsilon();

        double res = determinant(sr.x - sl.x       , sr.y - sl.y,
                                 num_x - den * sl.x, num_y - den * sl.y);
        double roundoff = 16 * epsilon * determinant_roundoff(
                    sr.x - sl.x,
                    sr.y - sl.y,
                    num_x_err + den_err * abs(sl.x),
                    num_y_err + den_err * abs(sl.y));

        if (res > roundoff)
        {
            return 1;
        }
        if (res < -roundoff)
        {
            return -1;
        }

        // todo: refractor later
        // ИНДУССКИЙ КОД!
        // using namespace boost::numeric::interval_lib::compare::certain;
        intersection.init_interval_cache();
        typedef boost::numeric::interval<double> i_t;
        auto& i_num_x = intersection.cache_interval->numerator_x;
        auto& i_num_y = intersection.cache_interval->numerator_y;
        auto& i_den = intersection.cache_interval->denumerator;

        i_t i_res = determinant(
                    i_t(sr.x) - i_t(sl.x), i_t(sr.y) - i_t(sl.y),
                    i_num_x - i_den * sl.x, i_num_y - i_den * sl.y);
        if (i_res.lower() > 0.)
        {
            return 1;
        }
        if (i_res.upper() < 0.)
        {
            return -1;
        }

        intersection.init_gmp_cache();
        typedef mpq_class q_t;
        point_base_t<q_t>& q_p = *intersection.cache_gmp;
        point_base_t<q_t> q_sl(sl);
        q_t q_res = determinant<q_t>(q_t(sr.x) - q_sl.x, q_t(sr.y) - q_sl.y,
                                     q_p.x - q_sl.x    , q_p.y - q_sl.y);
        return sgn(q_res);
    }

};

struct segment_status_compare_t
{
    event_point_to_segment_compare_t comp;
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
        return boost::apply_visitor(std::bind(comp, _1, segment), point);
    }
};

} // end of namespace detail

typedef detail::ordered_segment_t ordered_segment_t;
typedef detail::segments_status_t segments_status_t;
typedef detail::segments_event_queue_t segments_event_queue_t;

} // end of namespace geometry

#endif // GEOMETRY_STRUCTURES_H
