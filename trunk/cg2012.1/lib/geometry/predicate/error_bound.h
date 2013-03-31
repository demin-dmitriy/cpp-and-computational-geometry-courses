#ifndef GEOMETRY_PREDICATE_ERROR_BOUND_H
#define GEOMETRY_PREDICATE_ERROR_BOUND_H

#include "geometry/predicate/util.h"
#include "geometry/predicate/expression_tree.h"
#include <boost/mpl/and.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/or.hpp>

namespace geometry
{

namespace predicate
{

namespace detail
{

using namespace boost::mpl;

template<typename T>
struct calc_epsilon_aux_t;

template<int n>
struct calc_epsilon_aux_t<variable_t<n>>
{
    static int const eps_power = 0;
    typedef abs_t<variable_t<n>> eps_tree;
};

template<int n>
struct calc_epsilon_aux_t<constant_t<n>>
{
    static int const eps_power = 0;
    typedef abs_t<constant_t<n>> eps_tree;
};

// todo: think out beter name for this.
template<typename Cond, typename LTree, typename RTree>
struct if_eps_equal_zero_sum_t;

template<typename LTree, typename RTree>
struct if_eps_equal_zero_sum_t<true_, LTree, RTree>
{
    typedef abs_t<sum_t<typename LTree::Arg, typename RTree::Arg>> res;
};

template<typename LTree, typename RTree>
struct if_eps_equal_zero_sum_t<false_, LTree, RTree>
{
    typedef sum_t<LTree, RTree> res;
};

template<typename L, typename R>
struct calc_epsilon_aux_t<sum_t<L, R>>
{
    typedef calc_epsilon_aux_t<L> res_l;
    typedef calc_epsilon_aux_t<R> res_r;

    static int const eps_power = max(res_l::eps_power, res_r::eps_power) + 1;

    typedef typename if_eps_equal_zero_sum_t<
                typename and_<
                    equal_to<int_<res_l::eps_power>, int_<0>>,
                    equal_to<int_<res_r::eps_power>, int_<0>>
                >::type,
                typename res_l::eps_tree, typename res_r::eps_tree
            >::res eps_tree;
};

template<typename Cond, typename LTree, typename RTree>
struct if_eps_equal_zero_sub_t;

template<typename LTree, typename RTree>
struct if_eps_equal_zero_sub_t<true_, LTree, RTree>
{
    typedef abs_t<sub_t<typename LTree::Arg, typename RTree::Arg>> res;
};

template<typename LTree, typename RTree>
struct if_eps_equal_zero_sub_t<false_, LTree, RTree>
{
    typedef sum_t<LTree, RTree> res;
};

template<typename L, typename R>
struct calc_epsilon_aux_t<sub_t<L, R>>
{
    typedef calc_epsilon_aux_t<L> res_l;
    typedef calc_epsilon_aux_t<R> res_r;

    static int const eps_power = max(res_l::eps_power, res_r::eps_power) + 1;

    typedef typename if_eps_equal_zero_sub_t<
                typename and_<
                    equal_to<int_<res_l::eps_power>, int_<0>>,
                    equal_to<int_<res_r::eps_power>, int_<0>>
                >::type,
                typename res_l::eps_tree, typename res_r::eps_tree
            >::res eps_tree;
};

template<typename L, typename R>
struct calc_epsilon_mul_helper_t
{
    typedef mul_t<L, R> res;
};

template<typename L, typename R>
struct calc_epsilon_mul_helper_t<abs_t<L>, abs_t<R>>
{
    typedef abs_t<mul_t<L, R>> res;
};

template<typename E>
struct is_power2_constant
{
    typedef false_ type;
};

template<int n>
struct is_power2_constant<constant_t<n>>
{
    typedef typename if_c<is_power_of_2(n), true_, false_>::type type;
};

template<typename L, typename R>
struct calc_epsilon_aux_t<mul_t<L, R>>
{
    typedef calc_epsilon_aux_t<L> res_l;
    typedef calc_epsilon_aux_t<R> res_r;

    typedef typename res_l::eps_tree l_tree;
    typedef typename res_r::eps_tree r_tree;

    static int const eps_power =
            res_l::eps_power
            + res_r::eps_power
            + if_<or_<is_power2_constant<L>, is_power2_constant<R>>,
                int_<0>,
                int_<1>
            >::type::value;
    typedef typename calc_epsilon_mul_helper_t<l_tree, r_tree>::res eps_tree;
};

// Expression = CalcExpression +- eps_factor * |AbsExpression|
// Expression -- exact result.
// CalcExpression -- computed result.
// eps_factor = f(eps)
//
// For any expression epsilon factor has form of (1+eps)^n - 1. We can
// reduce this number by doing a little trick in the end of calculation as
// showed in Richard Shewchuk's paper (in the example of calculation Orient2D).
//
// Then the epsilon factor of whole expression would be of this form:
// ((1+eps)^(n-1) - 1) * (1+eps)^2 / (1 - eps) that is less than:
// ((1+eps)^(n+1) - (1+eps)^2) * (1 + eps + 2*eps^2)  that is equal to:
// (C(n+1,1)-2)*eps + (C(n+1,2)-1)*eps^2 + C(n+1,3)*eps^3 + ...
//      ... + C(n+1,n+1)*eps^(n+1)) * (1 + eps + 2*eps^2)
//
// We cacluate upper bound of whole expression, though in practical cases,
// where n << 300 000 only first two summands will affect the value of
// eps_factor.

// Summands of not expanded expression disregard epsilon.
template<int n, int i>
struct summand_not_expanded_t
{
    constexpr static double value = binomial_coefficient_t<n, i>::value;
};

template<int n>
struct summand_not_expanded_t<n, 0>
{
    constexpr static double value = 0.;
};

template<int n>
struct summand_not_expanded_t<n, 1>
{
    constexpr static double value = binomial_coefficient_t<n, 1>::value - 2.;
};

template<int n>
struct summand_not_expanded_t<n, 2>
{
    constexpr static double value = binomial_coefficient_t<n, 2>::value - 1.;
};

template<int n>
struct eps_power_t
{
    constexpr static double value = epsilon * eps_power_t<n - 1>::value;
};

template<>
struct eps_power_t<0>
{
    constexpr static double value = 1.;
};

template<int n, int i>
struct summand_t
{
    template<int m, int j>
    using s = summand_not_expanded_t<m, j>;

    constexpr static double value = eps_power_t<i>::value
                                    * sum(s<n, i>::value,
                                          sum(s<n, i - 1>::value,
                                              2. * s<n, i - 2>::value));
};

// Assumes that k is bigger on for than it should be.
template<int n, int k>
struct calc_part_eps_sum_aux_t
{
    constexpr static double value =
            (sum(summand_t<n, k + 4>::value,
                 calc_part_eps_sum_aux_t<n, k + 1>::value));
};

template<int n>
struct calc_part_eps_sum_aux_t<n, n>
{
    constexpr static double value = 0.;
};

template<int n, int k>
struct calc_part_eps_sum_t
{
    constexpr static double value =
            calc_part_eps_sum_aux_t<n + 1, k - 4>::value;
};

template<typename T>
struct calculate_epsilon_t
{
    typedef calc_epsilon_aux_t<T> res;
    constexpr static double factor =
            calc_part_eps_sum_t<res::eps_power, 1>::value;
    typedef typename res::eps_tree tree;
};

} // end of namespace detail

} // end of namespace predicate

} // end of namespace geometry

#endif // GEOMETRY_PREDICATE_ERROR_BOUND_H
