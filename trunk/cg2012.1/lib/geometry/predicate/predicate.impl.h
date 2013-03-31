#ifndef GEOMETRY_PREDICATE_PREDICATE_IMPL_H
#define GEOMETRY_PREDICATE_PREDICATE_IMPL_H

#include "geometry/predicate/error_bound.h"
#include "geometry/predicate/expression_tree.h"
#include "geometry/predicate/util.h"

namespace geometry
{

namespace predicate
{

enum predicate_result_t
{
    positive = 1,
    zero = 0,
    negative = -1,
    uncertain = -3
};

// One of the steps of filtered predicate.
template<typename Expression, typename StepTag>
struct step_t;

namespace detail
{

template<typename, typename...>
struct steps_t;

template<typename Expression, typename Step, typename... Steps>
struct steps_t<Expression, Step, Steps...>
{
    template<typename Info>
    static predicate_result_t run(Info& info)
    {
        predicate_result_t res = step_t<Expression, Step>::eval(info);
        if (res == uncertain)
        {
            return steps_t<Expression, Steps...>::run(info);
        }
        return res;
    }
};

template<typename Expression>
struct steps_t<Expression>
{
    template<typename Info>
    static predicate_result_t run(Info& info)
    {
        return uncertain;
    }
};

// Carries local information such as variables' input values.
template<typename T, typename... Steps>
struct info_t
{
    typedef typename ntuple_t<double, vars_amount_t<T>::value>::type vars_t;
    vars_t vars;

    template<typename... Args>
    info_t(Args&&... vars)
        : vars(std::forward<Args>(vars)...)
    {}
};

template<typename, typename...>
struct predicate_t;

template<typename T, typename... Steps>
struct predicate_t<expression_t<T>, Steps...>
{
    template<typename... Args>
    predicate_result_t operator()(Args&&... args)
    {
        static_assert(sizeof...(Args) == vars_amount_t<T>::value,
                      "Wrong number of arguments passed to predicate.");
        info_t<T, Steps...> info(std::forward<Args>(args)...);
        return steps_t<expression_t<T>, Steps...>::run(info);
    }
};

} // end of namespace detail

} // end of namespace predicate

} // end of namespace geometry

#endif // GEOMETRY_PREDICATE_PREDICATE_IMPL_H
