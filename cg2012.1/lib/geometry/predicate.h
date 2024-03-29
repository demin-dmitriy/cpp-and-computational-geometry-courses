/*
 * A module for automatizating predicate creation process.
 *
 * There still a lot of room for improvement.
 */

/*
 * Example of predicate orientation2d:
 *
 * int orient2d(point_t a1, point_t a2, point_t b) {
 * // Take advantage of lambda's ability deduce it's return type.
 * auto expression = []()
 * {
 *     // This code can't actually execute (it won't compile if you try to call
 *     // this lambda function) but all types can be deduced.
 *
 *     // variable_t<n> is abstract number.
 *     // n is the number of argument in function call.
 *     variable_t<0> a1x;
 *     variable_t<1> a1y;
 *     variable_t<2> a2x;
 *     variable_t<3> a2y;
 *     variable_t<4> bx;
 *     variable_t<5> by;
 *     // For variable_t operators + - * / declared, but actually not defined
 *     // (the body is not defined). Operators construct expression tree
 *     // so that you don't have to manipulate with templates directly.
 *     return (bx - a1x) * (a2y - a1y) - (a2x - a1x) * (by - a1y);
 * };
 *
 * // Note: without decltype you'll have no way but to write something monstrous
 * // like:
 * // typedef sub_t<mul_t<sub_t<variable_t<4>, variable_t<0>>, ... ;
 * typedef decltype(expression()) expr_t;
 * predicate_t<expr_t, double_step_t, interval_step_t, gmp_step_t> p;
 *
 * return p(a1.x, a1.y, a2.x, a2.y, b.x, b.y);
 * }
 *
 */

#ifndef GEOMETRY_PREDICATE_H
#define GEOMETRY_PREDICATE_H

#include "predicate/predicate.impl.h"

#include <cfenv>

#include <boost/math/special_functions/sign.hpp>
#include <boost/numeric/interval.hpp>

#include <gmpxx.h>

namespace geometry
{

namespace predicate
{

using detail::predicate_t;

using detail::expression_t;

template<int n>
using variable_t = detail::expression_t<detail::variable_t<n>>;

template<int n>
using constant_t = detail::expression_t<detail::constant_t<n>>;

// *_step_t is a tag for step_t struct.

// double_step_t performes calculations using double arithmetic and checks
// validity of result using error bound analysis. (Calculation of error bound is
// simmilar to what could be found in Richard Shewchuk's pape - Adaptive
// Precision Floating-Point Arithmetic and Fast Robust Geometric Predicates.
// In particular, resulting error bounds for Orient2D here and in paper are
// coincide.)
struct double_step_t;

// interval_step_t performes calculations using boost::numeric::interval<double>
// class. Yields uncertain if 0 belongs to resulting interval.
struct interval_step_t;

// gmp_step_t performes calculations using mpq_class_t. It never yields
// uncertain.
struct gmp_step_t;

// custom_step_t is left for user specialization (for example to replace gmp
// arithmetic with adaptive).
struct custom_step_t;

template<typename T>
struct step_t<expression_t<T>, double_step_t>
{
    typedef detail::calculate_epsilon_t<T> eps;

    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        std::feclearexcept(FE_OVERFLOW | FE_UNDERFLOW);
        // It's important, that these calculations won't be reordered with
        // instructions that test fpu flags therefore they declared volatile.
        volatile double res = T::template eval<double>(info);
        volatile double roundoff
                = eps::factor * eps::tree::template eval<double>(info);
        if (!std::fetestexcept(FE_OVERFLOW | FE_UNDERFLOW)
                && std::abs(res) >= roundoff)
        {
            return static_cast<predicate_result_t>(boost::math::sign(res));
        }
        return uncertain;
    }
};

template<typename T>
struct step_t<expression_t<T>, interval_step_t>
{
    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        typedef boost::numeric::interval<double> interval_t;
        interval_t res = T::template eval<interval_t>(info);
        if (res.upper() < 0.)
        {
            return negative;
        }
        if (res.lower() > 0.)
        {
            return positive;
        }
        if (res.upper() == res.lower())
        {
            return zero;
        }
        return uncertain;
    }
};

template<typename T>
struct step_t<expression_t<T>, gmp_step_t>
{
    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        return static_cast<predicate_result_t>(
                    sgn(T::template eval<mpq_class>(info)));
    }
};

} // end of namespace predicate

} // end of namespace geometry

#endif // GEOMETRY_PREDICATE_H
