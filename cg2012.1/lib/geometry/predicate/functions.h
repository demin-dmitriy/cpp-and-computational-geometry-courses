/*
 * Some functions for generating predicates.
 *
 * They don't have body they only construct required expression type.
 *
 */

#ifndef GEOMETRY_PREDICATE_FUNCTIONS_H
#define GEOMETRY_PREDICATE_FUNCTIONS_H

#include "geometry/predicate.h"

namespace geometry
{

namespace predicate
{

// General approach is possible but unnecessary.
template<typename X11, typename X12,
         typename X21, typename X22>
auto determinant(X11 x11, X12 x12,
                 X21 x21, X22 x22)
    -> decltype(x11 * x22 - x12 * x21);

namespace detail
{

template<typename X11, typename X12, typename X13,
         typename X21, typename X22, typename X23,
         typename X31, typename X32, typename X33>
struct determinant3_t
{
    X11 x11; X12 x12; X13 x13;
    X21 x21; X22 x22; X23 x23;
    X31 x31; X32 x32; X33 x33;
    decltype(determinant(x22, x23,
                         x32, x33)) minor11;
    decltype(determinant(x12, x13,
                         x32, x33)) minor21;
    decltype(determinant(x12, x13,
                         x22, x23)) minor31;
    decltype(x11 * minor11 - x21 * minor21 + x31 * minor31) result;
};

} // end of namespace detail

template<typename X11, typename X12, typename X13,
         typename X21, typename X22, typename X23,
         typename X31, typename X32, typename X33>
auto determinant(X11, X12, X13,
                 X21, X22, X23,
                 X31, X32, X33)
    -> decltype(detail::determinant3_t<X11, X12, X13,
                                       X21, X22, X23,
                                       X31, X32, X33>::result);

template<typename A>
auto sqr(A arg)
    -> decltype(arg * arg);

template<typename Ax, typename Ay, typename Bx, typename By>
auto dot_product(Ax ax, Ay ay, Bx bx, By by)
    -> decltype(ax * bx + ay * by);

} // end of namespace predicate

} // end of namespace geometry

#endif // GEOMETRY_PREDICATE_FUNCTIONS_H
