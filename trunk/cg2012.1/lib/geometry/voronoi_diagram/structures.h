// Here defined beachline and event queue for voronoi diagram.

#ifndef GEOMETRY_VORONOI_DIAGRAM_STRUCTURES_H
#define GEOMETRY_VORONOI_DIAGRAM_STRUCTURES_H

#include "geometry/voronoi_diagram.h"

#include <cassert>
#include <cfloat>
#include <cmath>

#include <array>
#include <set>

#include "geometry/workaround/boost_intrusive_set.hpp"
#include <boost/container/set.hpp>
#include <boost/variant.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/math/special_functions.hpp>
#include <boost/optional.hpp>
#include <gmpxx.h>

#include "geometry/predicate.h"
#include "geometry/predicate/functions.h"
#include "geometry/voronoi_diagram/predicates.h"

namespace geometry
{
namespace detail
{

using site_t = voronoi_diagram::site_t;

struct circle_event_t;
using event_t = boost::variant<site_t const&, circle_event_t>;

struct event_comparator;
using event_queue_t = std::multiset<event_t, event_comparator>;

using pre_vertex_t = std::array<site_t const*, 3>;

using half_edge_t = voronoi_diagram::half_edge_base_t<pre_vertex_t>;

using boost::numeric::interval;

template<typename T>
using unprotected_interval = typename boost::numeric::interval_lib::unprotect<
                             interval<T>>::type;

struct node_t;

// std::multiset is not enough because of it's lack of flexibility of its insert
// methods. Motivation explained in detail in "advanced_lookups_insertions"
// section of boost/intrusive documentation. The intrusive properties of this
// container by themselves are actually not required.
using beachline_t = boost::intrusive::multiset<node_t>;

// For some reason boost::numeric::sqrt produces results where interval bounds
// are rounded to nearest double which is incorrect.
// It possibly could be compiler issue or compiler's flags' issue.
// Luckily it's easy to write own implementation without significant loss of
// precision.
template<typename Interval>
Interval my_sqrt(Interval const& i)
{
    assert(i.lower() >= 0);
    using std::nexttoward;
    return Interval(nexttoward(sqrt(i.lower()), 0.),
                            nexttoward(sqrt(i.upper()), DBL_MAX));
}

// todo: possibly refractor to highlight determinant.
template<typename Type>
void calculate_vertex_parameters(
        pre_vertex_t const& vertex,
        Type& a_out,
        Type& bx_out,
        Type& d_out)
{
   /*
    * Using circle equation:
    * |  x^2+y^2   x   y   1 |
    * | x1^2+y1^2  x1  y1  1 |
    * | x2^2+y2^2  x2  y2  1 | = 0
    * | x3^2+y3^2  x3  y3  1 |
    * Then expand determinant along first row and denote minors as a, bx, by
    * and c. Then we have a*(x^2+y^2) + bx*x + by*y + c = 0. From that follows
    * that circle center coordinates are x0=-bx/(2a), y0=-by/(2a). Substitute
    * y0 for y and solve for x:
    * x = (-bx +- sqrt(d))/(2a) where d = bx^2 + by^2 - 4a*c.
    */
    using std::get;
    Type x1 = get<0>(vertex)->x;
    Type x2 = get<1>(vertex)->x;
    Type x3 = get<2>(vertex)->x;
    Type y1 = get<0>(vertex)->y;
    Type y2 = get<1>(vertex)->y;
    Type y3 = get<2>(vertex)->y;

    Type x2_m_x1 = x2 - x1;
    Type y2_m_y1 = y2 - y1;
    Type x3_m_x1 = x3 - x1;
    Type y3_m_y1 = y3 - y1;

    a_out = x2_m_x1 * y3_m_y1 - x3_m_x1 * y2_m_y1;
    Type x1x1_p_y1y1 = x1 * x1 + y1 * y1;
    Type x2x2_p_y2y2 = x2 * x2 + y2 * y2;
    Type x3x3_p_y3y3 = x3 * x3 + y3 * y3;
    Type x2x2_p_y2y2_m_x1x1_m_y1y1 = x2x2_p_y2y2 - x1x1_p_y1y1;
    Type x3x3_p_y3y3_m_x1x1_m_y1y1 = x3x3_p_y3y3 - x1x1_p_y1y1;
    bx_out = -x2x2_p_y2y2_m_x1x1_m_y1y1 * y3_m_y1
            + x3x3_p_y3y3_m_x1x1_m_y1y1 * y2_m_y1;
    Type by = x2x2_p_y2y2_m_x1x1_m_y1y1 * x3_m_x1
            - x3x3_p_y3y3_m_x1x1_m_y1y1 * x2_m_x1;
    Type c = -x1x1_p_y1y1 * (x2 * y3 - x3 * y2)
            + x2x2_p_y2y2 * (x1 * y3 - x3 * y1)
            - x3x3_p_y3y3 * (x1 * y2 - x2 * y1);
    d_out = bx_out * bx_out + by * by - 4. * a_out * c;
}

// Defines vertex as center of circumcircle of three points.
struct circle_event_t
{
    // Requires points in vertex to be in counterclockwise order.
    circle_event_t(pre_vertex_t const& vertex, node_t& node)
        : vertex(vertex)
        , node(&node)
        , gmp_cache_initialized(false)
    {
        assert(left == turn(*std::get<0>(vertex),
                            *std::get<1>(vertex),
                            *std::get<2>(vertex)));

        // Save and initialize the rounding mode
        typename interval<double>::traits_type::rounding rnd;
        // Silence "unused variable" warning
        (void) rnd;
        unprotected_interval<double> a, bx, d;
        calculate_vertex_parameters(vertex, a, bx, d);
        // Known for sure that a >= 0 because points are in counterclockwise
        // order.
        assert(a.upper() > 0);
        a = abs(a);
        // d > 0 because it defines radius of some circle.
        assert(d.upper() > 0);
        d = abs(d);
        event_x = (-bx + my_sqrt(d)) / (2. * a);
    }

    // Should be called whenever you are not sure if gmp_cache is initialized.
    void init_parameters() const
    {
        if (!gmp_cache_initialized)
        {
            calculate_vertex_parameters(vertex,
                                        gmp_cache.a,
                                        gmp_cache.b,
                                        gmp_cache.d);
            gmp_cache_initialized = true;
            assert(gmp_cache.a > 0);
            assert(gmp_cache.d > 0);
        }
    }

    pre_vertex_t vertex;

    // Disapearing node in beachline.
    node_t* node;

    // Approximate x coordinate value of event. Also rightmost point coordinate
    // of enclosing circle.
    unprotected_interval<double> event_x;

    template<typename Type>
    struct vertex_parameter_cache
    {
        Type a;
        Type b;
        Type d;
    };
    // Using boost::optional here slows down the program.
    mutable bool gmp_cache_initialized;
    mutable vertex_parameter_cache<mpq_class> gmp_cache;
};

// TODO : Разбить на два класса при рефракторинге
struct event_comparator : public boost::static_visitor<bool>
{
    bool operator() (event_t const& a, event_t const& b) const
    {
        return boost::apply_visitor(*this, a, b);
    }

    int comparator(site_t const& s, circle_event_t const& v) const
    {
        if (s.x < v.event_x.lower())
        {
            return -1;
        }
        if (s.x >= v.event_x.upper())
        {
            return 1;
        }
        v.init_parameters();
        mpq_class& a = v.gmp_cache.a;
        mpq_class& bx = v.gmp_cache.b;
        mpq_class& d = v.gmp_cache.d;
        mpq_class t = 2 * a * s.x + bx;
        if (sgn(t) < 0)
        {
            return -1;
        }
        return boost::math::sign(cmp(t * t, d));
    }

    bool operator()(site_t const& s, circle_event_t const& v) const
    {
        return -1 == comparator(s, v);
    }

    bool operator()(circle_event_t const& v, site_t const& s) const
    {
        return 1 == comparator(s, v);
    }

    bool operator()(circle_event_t const& a, circle_event_t const& b) const
    {
        if (a.event_x.upper() < b.event_x.lower())
        {
            return true;
        }
        if (b.event_x.upper() <= a.event_x.lower())
        {
            return false;
        }
        // todo: пояснить вычисления?
        a.init_parameters();
        mpq_class& a1 = a.gmp_cache.a;
        mpq_class& bx1 = a.gmp_cache.b;
        mpq_class& d1 = a.gmp_cache.d;
        b.init_parameters();
        mpq_class& a2 = b.gmp_cache.a;
        mpq_class& bx2 = b.gmp_cache.b;
        mpq_class& d2 = b.gmp_cache.d;
        mpq_class a1b2_m_a2b1 = a1 * bx2 - a2 * bx1;
        mpq_class a1a1d2 = a1 * a1 * d2;
        mpq_class a2a2d1 = a2 * a2 * d1;
        int sign_left = sgn(a1b2_m_a2b1);
        int sign_right = boost::math::sign(cmp(a1a1d2, a2a2d1));
        if ((sign_left != sign_right) | (sign_left == 0))
        {
            return sign_left < sign_right;
        }
        // Here we have sign_left == sign_right & sign_left != 0
        mpq_class t1 = a1a1d2 + a2a2d1 - a1b2_m_a2b1 * a1b2_m_a2b1;
        int sgn_t1 = sgn(t1);
        if (sgn_t1 < 0)
        {
            return sign_left == -1;
        }
        else if(sgn_t1 == 0)
        {
            return false;
        }
        int cmp_res = cmp(4 * a1a1d2 * a2a2d1, t1 * t1);
        if (sign_left > 0)
        {
            return cmp_res < 0;
        }
        else
        {
            return cmp_res > 0;
        }
    }

    bool operator()(site_t const& a, site_t const& b) const
    {
        return a.x < b.x;
    }
};

struct node_t : public boost::intrusive::set_base_hook<>
{
    site_t const* site;
    node_t* node_to_left;
    boost::optional<event_queue_t::iterator> event;
    half_edge_t* half_edge;

    node_t(site_t const& site, node_t* node_to_left = nullptr)
        : site(&site)
        , node_to_left(node_to_left)
        , half_edge(nullptr)
    {}
};


struct site_to_node_comp
{
    bool operator()(site_t const& key, node_t const& node) const
    {
        if (node.node_to_left == nullptr) // Then it is the leftmost arc.
        {
            return false;
        }
        return compare(key, *node.node_to_left->site, *node.site);
    }

    int midpoint_compare(
            site_t const& key, site_t const& l, site_t const& r) const
    {
        using namespace geometry::predicate;
        predicate_t<midpoint_expression_t, double_step_t, custom_step_t> p;
        return p(l.y, r.y, key.y);
    }

    // todo: Плохое имя
    bool mid_res_compare(
            site_t const& key, site_t const& l, site_t const& r) const
    {
        using namespace geometry::predicate;
        predicate_t<midres_expression_t,
                double_step_t,
                interval_step_t,
                custom_step_t> p;
        return negative == p(l.x, l.y, r.x, r.y, key.x, key.y);
    }

    // todo: Плохое имя
    // todo: Пояснить вычисления
    bool res(site_t const& key, site_t const& l, site_t const& r) const
    {
        using namespace geometry::predicate;
        auto res_expression = []()
        {
            variable_t<0> lx;
            variable_t<1> ly;
            variable_t<2> rx;
            variable_t<3> ry;
            variable_t<4> key_x;
            variable_t<5> key_y;
            auto c = (sqr(ry) + sqr(rx) - sqr(key_x)) * (lx - key_x)
                    - (sqr(ly) + sqr(lx) - sqr(key_x)) * (rx - key_x);
            auto a = lx - rx;
            auto b = (ly * (rx - key_x) - ry * (lx - key_x));
            return c + key_y * (a * key_y + constant_t<2>() * b);
        };
        typedef decltype(res_expression()) res_expression_t;
        predicate_t<res_expression_t, double_step_t, gmp_step_t> p;
        return negative == p(l.x, l.y, r.x, r.y, key.x, key.y);
    }

    bool compare(site_t const& key, site_t const& l, site_t const& r) const
    {
        assert(!(l.x == key.x && r.x == key.x));
        if (l.x == key.x)
        {
            return key.y < l.y;
        }
        if (r.x == key.x)
        {
            return key.y < r.y;
        }
        if (l.x == r.x)
        {
            return 1 == midpoint_compare(key, l, r);
        }
        if (mid_res_compare(key, l, r))
        {
            return l.x > r.x;
        }
        return res(key, l, r);
    }

};

} // end of namespace detail

} // end of namespace geometry

#endif // GEOMETRY_VORONOI_DIAGRAM_STRUCTURES_H
