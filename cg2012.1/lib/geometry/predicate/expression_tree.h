#ifndef GEOMETRY_PREDICATE_EXPRESSION_TREE_H
#define GEOMETRY_PREDICATE_EXPRESSION_TREE_H

#include <cmath>

#include <tuple>

namespace geometry
{

namespace predicate
{

namespace detail
{

template<typename T>
struct expression_t
{
    typedef T Tree;
};

template<int n>
struct variable_t
{
    variable_t() = delete;

    template<typename Type, typename Info>
    constexpr static Type eval(Info& info)
    {
        return std::get<n>(info.vars);
    }
};

template<int n>
struct constant_t
{
    template<typename Type, typename Info>
    constexpr static Type eval(Info&)
    {
        return static_cast<Type>(n);
    }
};

template<typename L, typename R>
struct sum_t
{
    typedef L Left;
    typedef R Right;

    template<typename Type, typename Info>
    constexpr static Type eval(Info& info)
    {
        return Left::template eval<Type, Info>(info)
                + Right::template eval<Type, Info>(info);
    }
};

template<typename L, typename R>
struct sub_t
{
    typedef L Left;
    typedef R Right;

    template<typename Type, typename Info>
    constexpr static Type eval(Info& info)
    {
        return Left::template eval<Type, Info>(info)
                - Right::template eval<Type, Info>(info);
    }
};

template<typename L, typename R>
struct mul_t
{
    typedef L Left;
    typedef R Right;

    template<typename Type, typename Info>
    constexpr static Type eval(Info& info)
    {
        return Left::template eval<Type, Info>(info)
                * Right::template eval<Type, Info>(info);
    }
};

template<typename L, typename R>
struct div_t
{
    typedef L Left;
    typedef R Right;

    template<typename Type, typename Info>
    constexpr static Type eval(Info& info)
    {
        return Left::template eval<Type, Info>(info)
                / Right::template eval<Type, Info>(info);
    }
};

template<typename A>
struct abs_t
{
    typedef A Arg;

    template<typename Type, typename Info>
    constexpr static Type eval(Info& info)
    {
        return std::abs(Arg::template eval<Type, Info>(info));
    }
};

// Don't need actual implementations for operators.
template<typename U, typename V>
auto operator+(expression_t<U>, expression_t<V>)
    -> expression_t<sum_t<U, V>>;

template<typename U, typename V>
auto operator-(expression_t<U>, expression_t<V>)
    -> expression_t<sub_t<U, V>>;

template<typename U, typename V>
auto operator*(expression_t<U>, expression_t<V>)
    -> expression_t<mul_t<U, V>>;

/*
 * Note: epsilon calculation is not supported for operator/ (though it's not
 * necessary impossible).
 */
template<typename U, typename V>
auto operator/(expression_t<U>, expression_t<V>)
    -> expression_t<div_t<U, V>>;


template<typename>
struct vars_amount_t;

template<typename L, typename R>
struct vars_amount_t<sum_t<L, R>>
{
    static const int value = max(vars_amount_t<L>::value,
                                 vars_amount_t<R>::value);
};

template<typename L, typename R>
struct vars_amount_t<sub_t<L, R>>
{
    static const int value = max(vars_amount_t<L>::value,
                                 vars_amount_t<R>::value);
};

template<typename L, typename R>
struct vars_amount_t<mul_t<L, R>>
{
    static const int value = max(vars_amount_t<L>::value,
                                 vars_amount_t<R>::value);
};

template<typename L, typename R>
struct vars_amount_t<div_t<L, R>>
{
    static const int value = max(vars_amount_t<L>::value,
                                 vars_amount_t<R>::value);
};

template<int n>
struct vars_amount_t<variable_t<n>>
{
    static const int value = n + 1;
};

template<int n>
struct vars_amount_t<constant_t<n>>
{
    static const int value = 0;
};

} // end of namespace geometry

} // end of namespace predicate

} // end of namespace detail

#endif // GEOMETRY_PREDICATE_EXPRESSION_TREE_H
