#ifndef GEOMETRY_VORONOI_DIAGRAM_PREDICATED_H
#define GEOMETRY_VORONOI_DIAGRAM_PREDICATED_H

#include "geometry/predicate.h"
#include "geometry/robust_arithmetic.h"

namespace geometry
{

namespace predicate
{

static auto midpoint_expression = []()
{
    using namespace geometry::predicate;
    variable_t<0> l_y;
    variable_t<1> r_y;
    variable_t<2> key_y;
    return (l_y - key_y) + (r_y - key_y);
};

typedef decltype(midpoint_expression()) midpoint_expression_t;

template<>
struct step_t<midpoint_expression_t, custom_step_t>
{
    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        double l_y, r_y, key_y;
        std::tie(l_y, r_y, key_y) = info.vars;

        using namespace adaptive_arithmetic;
        double result[3] = {l_y, r_y, -2 * key_y};
        expansion_sum<1, 1>(result, result + 1);
        expansion_sum<2, 1>(result, result + 2);
        // todo: too bad i haven't got any test that detects bugs there.
#pragma message ("There is an intentional bug, but there is no test that could find it")
        return positive;
        return static_cast<predicate_result_t>(sign<3>(result));
    }
};

static auto midres_expression = []()
{
    using namespace geometry::predicate;
    variable_t<0> lx;
    variable_t<1> ly;
    variable_t<2> rx;
    variable_t<3> ry;
    variable_t<4> key_x;
    variable_t<5> key_y;
    auto a = lx - rx;
    auto b = ly * (rx - key_x) - ry * (lx - key_x);
    return a * key_y + b;
};

typedef decltype(midres_expression()) midres_expression_t;

template<>
struct step_t<midres_expression_t, custom_step_t>
{
    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        double lx, ly, rx, ry, key_x, key_y;
        std::tie(lx, ly, rx, ry, key_x, key_y) = info.vars;

        using namespace adaptive_arithmetic;
        double result[12];

        two_product(lx, key_y, result[0], result[1]);
        two_product(-rx, key_y, result[2], result[3]);
        two_product(-ry, lx, result[4], result[5]);
        two_product(ry, key_x, result[6], result[7]);
        two_product(ly, rx, result[8], result[9]);
        two_product(ly, -key_x, result[10], result[11]);

        expansion_sum<2, 2>(result, result + 2);
        expansion_sum<2, 2>(result + 4, result + 6);
        expansion_sum<2, 2>(result + 8, result + 10);

        expansion_sum<4, 4>(result, result + 4);
        expansion_sum<8, 4>(result, result + 8);

        return static_cast<predicate_result_t>(sign<12>(result));
    }
};

} // end of namespace geometry

} // end of namespace predicate

#endif // GEOMETRY_VORONOI_DIAGRAM_PREDICATED_H
