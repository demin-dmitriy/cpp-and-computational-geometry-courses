#ifndef GEOMETRY_SEGMENTS_INTERSECTIONS_INTERSECTION_DESCRIPTOR_H
#define GEOMETRY_SEGMENTS_INTERSECTIONS_INTERSECTION_DESCRIPTOR_H

#include <algorithm>

#include "geometry/geometry_fwd.h"
#include "geometry/predicate/functions.h"
#include "geometry/segment_intersections/ordered_segment.h"

namespace geometry
{

namespace detail
{

struct intersection_descriptor_t
{
    intersection_descriptor_t(ordered_segment_t const* a_param,
                              ordered_segment_t const* b_param)
        : a(a_param)
        , b(b_param)
    {
        // Guarantee that denumerator will be positive.
        // todo: some explanations why so.
        if (right == turn(a->l, a->r, b->r))
        {
            std::swap(a, b);
        }
    }

    ordered_segment_t const* a;
    ordered_segment_t const* b;
};

template<typename Lx, typename Ly, typename Rx, typename Ry>
struct segment_parameters_t
{
    Lx lx; Ly ly;
    Rx rx; Ry ry;
    // For the line defined by two points l and r write an equation:
    // Ax + By + C = 0
    // Then A = l.y - r.y, B = r.x - l.x, C = l.x * r.y - l.y * r.x.
    typedef decltype(ly - ry) A;
    typedef decltype(rx - lx) B;
    typedef decltype(lx * ry - ly * rx) C;
};

template<typename Alx, typename Aly, typename Arx, typename Ary,
         typename Blx, typename Bly, typename Brx, typename Bry>
struct intersection_parameters_t
{
    typedef segment_parameters_t<Alx, Aly, Arx, Ary> segment_a;
    typedef segment_parameters_t<Blx, Bly, Brx, Bry> segment_b;
    typename segment_a::A A1;
    typename segment_a::B B1;
    typename segment_a::C C1;
    typename segment_b::A A2;
    typename segment_b::B B2;
    typename segment_b::C C2;
    /* Two segments a and b defined by equations:
     * A1*x + B1*y + C1 = 0
     * A2*x + B2*y + C2 = 0
     * To find their intersection solve linnear system and we'll have:
     *     | B1 C1 |
     *     | B2 C2 |
     * x = ---------
     *     | A1 B1 |
     *     | A2 B2 |
     *
     *     | C1 A1 |
     *     | C2 A2 |
     * y = ---------
     *     | A1 B1 |
     *     | A2 B2 |
     * If denumerator equals 0 thеn two lines are either collinear or coincide.
     * For intersection_descriptor_t of segments a and b denumerator guaranteed
     * to be non-negative. Moreover find_intersections algorithm doesn't count
     * as intersection segments that are collinear or concide.
     */
    decltype(predicate::determinant(B1, C1,
                                    B2, C2)) numerator_x;
    decltype(predicate::determinant(C1, A1,
                                    C2, A2)) numerator_y;
    decltype(predicate::determinant(A1, B1,
                                    A2, B2)) denumerator;
};

template<typename Alx, typename Aly, typename Arx, typename Ary,
         typename Blx, typename Bly, typename Brx, typename Bry>
auto get_intersection_parameters(Alx, Aly, Arx, Ary,
                                 Blx, Bly, Brx, Bry)
    -> intersection_parameters_t<Alx, Aly, Arx, Ary,
                                 Blx, Bly, Brx, Bry>;

} // end of namespace detail

} // end of namespace geometry

#endif // GEOMETRY_SEGMENTS_INTERSECTIONS_INTERSECTION_DESCRIPTOR_H
