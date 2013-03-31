#ifndef GEOMETRY_PREDICATE_UTIL_H
#define GEOMETRY_PREDICATE_UTIL_H

#include <cmath>

#include <limits>
#include <tuple>

namespace geometry
{

namespace predicate
{

namespace detail
{

// std::max is not constexpr.
constexpr int max(int x, int y)
{
    return (x > y) ? x : y;
}

constexpr double one_of_two(double x, double next_smaller, double next_bigger)
{
    return (next_smaller > x) ? next_smaller : next_bigger;
}

double const epsilon = std::numeric_limits<double>::epsilon() / 2;

// For normalized and positive x, returns minimum double bigger than x.
// Subcase of std::nextafter(x, DBL_MAX), but can work at compile-time.
constexpr double next_double(double x)
{
    return one_of_two(x, x + epsilon * x, x + 2. * epsilon * x);
}

constexpr bool is_sum_exact(double x, double y)
{
    // Note: std::abs seems not to be constexpr. Anyway this will be an easy
    // fix if it become an issue in compiler other than gcc.
    return (std::abs(x) > std::abs(y)) ? (x + y - x == y) : (x + y - y == x);
}

// Sums two doubles and returns value no less than exact result.
constexpr double sum(double x, double y)
{
    return (is_sum_exact(x, y)) ? (x + y) : next_double(x + y);
}

// Generates std::tuple of n times Type.
template<typename Type, int n, typename... Types>
struct ntuple_t
{
    typedef typename ntuple_t<Type, n - 1, Type, Types...>::type type;
};

template<typename Type, typename... Types>
struct ntuple_t<Type, 0, Types...>
{
    typedef std::tuple<Types...> type;
};

template<int n, int k>
struct binomial_coefficient_t
{
    constexpr static double value =
            sum(binomial_coefficient_t<n - 1, k - 1>::value,
                binomial_coefficient_t<n - 1, k    >::value);
};

template<int n>
struct binomial_coefficient_t<n, 0>
{
    constexpr static double value = 1.;
};

template<>
struct binomial_coefficient_t<0, 0>
{
    constexpr static double value = 1.;
};

template<int k>
struct binomial_coefficient_t<0, k>
{
    constexpr static double value = 0.;
};

constexpr bool is_power_of_2(int x)
{
    return (x > 0) && ((x & (x - 1)) == 0);
}

} // end of namespace detail

} // end of namespace predicate

} // end of namespace geometry

#endif // GEOMETRY_PREDICATE_UTIL_H
