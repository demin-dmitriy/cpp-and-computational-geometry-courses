#include "geometry/geometry_fwd.h"

#include <cfloat>
#include <cmath>

#include <tuple>

#include "geometry/point_base.h"
#include "geometry/predicate.h"
#include "geometry/predicate/functions.h"
#include "geometry/robust_arithmetic.h"

namespace geometry
{

namespace predicate
{

// turn predicate
static auto turn_expression = []()
{
    // Actually macroses could help a lot with these bulky definitions but they
    // are taboo.
    variable_t<0> a1x;
    variable_t<1> a1y;
    variable_t<2> a2x;
    variable_t<3> a2y;
    variable_t<4> bx;
    variable_t<5> by;
    return determinant(bx  - a1x, by  - a1y,
                       a2x - a1x, a2y - a1y);
};

typedef decltype(turn_expression()) turn_expression_t;

template<>
struct step_t<turn_expression_t, custom_step_t>
{
    // a1x*by - a1x*a2y - a2x*by - a1y*bx + a1y*a2x + a2y*bx
    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        double a1x, a1y, a2x, a2y, bx, by;
        std::tie(a1x, a1y, a2x, a2y, bx, by) = info.vars;

        using namespace adaptive_arithmetic;
        double result[12];

        two_product(a1x, by, result[0], result[1]);
        two_product(-a1x, a2y, result[2], result[3]);
        two_product(-a2x, by, result[4], result[5]);
        two_product(-a1y, bx, result[6], result[7]);
        two_product(a1y, a2x, result[8], result[9]);
        two_product(a2y, bx, result[10], result[11]);

        expansion_sum<2, 2>(result, result + 2);
        expansion_sum<2, 2>(result + 4, result + 6);
        expansion_sum<2, 2>(result + 8, result + 10);

        expansion_sum<4, 4>(result, result + 4);
        expansion_sum<8, 4>(result, result + 8);
        return static_cast<predicate_result_t>(sign<12>(result));
    }
};

// in_circle predicate for the circle that defined on two diameter points
static auto in_circle_expression = []()
{
    variable_t<0> ax;
    variable_t<1> ay;
    variable_t<2> bx;
    variable_t<3> by;
    variable_t<4> zx;
    variable_t<5> zy;
    return dot_product(ax - zx, ay - zy, bx - zx, by - zy);
};

typedef decltype(in_circle_expression()) in_circle_expression_t;

template<>
struct step_t<in_circle_expression_t, custom_step_t>
{
    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        double ax, ay, bx, by, zx, zy;
        std::tie(ax, ay, bx, by, zx, zy) = info.vars;

        using namespace adaptive_arithmetic;
        double result[16];

        two_product(ax, bx, result[0], result[1]);
        two_product(ax, -zx, result[2], result[3]);
        two_product(-zx, bx, result[4], result[5]);
        two_product(zx, zx, result[6], result[7]);
        two_product(ay, by, result[8], result[9]);
        two_product(ay, -zx, result[10], result[11]);
        two_product(-zy, by, result[12], result[13]);
        two_product(zy, zy, result[14], result[15]);

        expansion_sum<2, 2>(result, result + 2);
        expansion_sum<2, 2>(result + 4, result + 6);
        expansion_sum<2, 2>(result + 8, result + 10);
        expansion_sum<2, 2>(result + 12, result + 14);
        expansion_sum<4, 4>(result, result + 4);
        expansion_sum<4, 4>(result + 8, result + 12);
        expansion_sum<8, 8>(result, result + 8);

        return static_cast<predicate_result_t>(sign<16>(result));
    }
};

// compare_distance predicate for comparing distance between points a1 and a2,
// to distance between points b1 and b2.
static auto compare_distance_expression = []()
{
    variable_t<0> a1x;
    variable_t<1> a1y;
    variable_t<2> a2x;
    variable_t<3> a2y;
    variable_t<4> b1x;
    variable_t<5> b1y;
    variable_t<6> b2x;
    variable_t<7> b2y;
    return sqr(a1x - a2x) + sqr(a1y - a2y) - (sqr(b1x - b2x) + sqr(b1y - b2y));
};

typedef decltype(compare_distance_expression()) compare_distance_expression_t;

template<>
struct step_t<compare_distance_expression_t, custom_step_t>
{
    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        double a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y;
        std::tie(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y) = info.vars;

        using namespace adaptive_arithmetic;
        double result[24];
        two_product(a2x, a2x, result[0], result[1]);
        two_product(-2 * a2x, a1x, result[2], result[3]);
        two_product(a1x, a1x, result[4], result[5]);
        two_product(a2y, a2y, result[6], result[7]);
        two_product(-2 * a2y, a1y, result[8], result[9]);
        two_product(a1y, a1y, result[10], result[11]);

        two_product(-b2x, b2x, result[12], result[13]);
        two_product(2 * b2x, b1x, result[14], result[15]);
        two_product(-b1x, b1x, result[16], result[17]);
        two_product(-b2y, b2y, result[18], result[19]);
        two_product(2 * b2y, b1y, result[20], result[21]);
        two_product(-b1y, b1y, result[22], result[23]);

        expansion_sum<2, 2>(result, result + 2);
        expansion_sum<2, 2>(result + 4, result + 6);
        expansion_sum<2, 2>(result + 8, result + 10);
        expansion_sum<2, 2>(result + 12, result + 14);
        expansion_sum<2, 2>(result + 16, result + 18);
        expansion_sum<2, 2>(result + 20, result + 22);

        expansion_sum<4, 4>(result, result + 4);
        expansion_sum<4, 4>(result + 8, result + 12);
        expansion_sum<4, 4>(result + 16, result + 20);
        expansion_sum<8, 8>(result, result + 8);
        expansion_sum<16, 8>(result, result + 16);

        return static_cast<predicate_result_t>(sign<24>(result));
    }
};

// independent_turn predicate returns orientation of vector b1b2 relative to
// vector a1a2.
// turn(a1, a2, x) == turn(a1, a2, a1, x)
static auto independent_turn_expression = []()
{
    variable_t<0> a1x;
    variable_t<1> a1y;
    variable_t<2> a2x;
    variable_t<3> a2y;
    variable_t<4> b1x;
    variable_t<5> b1y;
    variable_t<6> b2x;
    variable_t<7> b2y;
    return determinant(b2x - b1x, b2y - b1y,
                       a2x - a1x, a2y - a1y);
};

typedef decltype(independent_turn_expression()) independent_turn_expression_t;

template<>
struct step_t<independent_turn_expression_t, custom_step_t>
{
    template<typename Info>
    static predicate_result_t eval(Info& info)
    {
        double a1x, a1y, a2x, a2y, bx, by, b2x, b2y;
        std::tie(a1x, a1y, a2x, a2y, bx, by, b2x, b2y) = info.vars;

        // a2x*b1y - a2x*b2y - a1x* b1y + a1x*b2y
        // - a2y*b1x + a2y*b2x + a1y*b1x - a1y*b2x
        using namespace adaptive_arithmetic;
        double result[16];

        two_product(a2x, by, result[0], result[1]);
        two_product(-a2x, b2y, result[2], result[3]);
        two_product(-a1x, by, result[4], result[5]);
        two_product(a1x, b2y, result[6], result[7]);
        two_product(-a2y, bx, result[8], result[9]);
        two_product(a2y, b2x, result[10], result[11]);
        two_product(a1y, bx, result[12], result[13]);
        two_product(-a1y, b2x, result[14], result[15]);

        expansion_sum<2, 2>(result, result + 2);
        expansion_sum<2, 2>(result + 4, result + 6);
        expansion_sum<2, 2>(result + 8, result + 10);
        expansion_sum<2, 2>(result + 12, result + 14);

        expansion_sum<4, 4>(result, result + 4);
        expansion_sum<4, 4>(result + 8, result + 12);
        expansion_sum<8, 8>(result, result + 8);

        return sign<16>(result);
    }
};

} // end of namespace predicate

bool check_bounding_box(
        point_t const a1, point_t const a2, point_t const b1, point_t const b2)
{
    using namespace std;

    double const min_x1 = min(a1.x, a2.x);
    double const min_y1 = min(a1.y, a2.y);
    double const min_x2 = min(b1.x, b2.x);
    double const min_y2 = min(b1.y, b2.y);
    double const max_x1 = max(a1.x, a2.x);
    double const max_y1 = max(a1.y, a2.y);
    double const max_x2 = max(b1.x, b2.x);
    double const max_y2 = max(b1.y, b2.y);

    return !((max_x2 < min_x1) || (max_x1 < min_x2))
                && !((max_y2 < min_y1) || (max_y1 < min_y2));
}

// Returns orientation of point b relative to vector a1a2. ??
turn_t turn(point_t const a1, point_t const a2, point_t const b)
{
    using namespace geometry::predicate;
    predicate_t<turn_expression_t, double_step_t, custom_step_t> p;
    return static_cast<turn_t>(p(a1.x, a1.y, a2.x, a2.y, b.x, b.y));
}

in_circle_t in_circle(
        point_t const a, point_t const b, point_t const c, point_t const d)
{
    using namespace geometry::predicate;
    auto in_circle_expression = []()
    {
        variable_t<0> ax;
        variable_t<1> ay;
        variable_t<2> bx;
        variable_t<3> by;
        variable_t<4> cx;
        variable_t<5> cy;
        variable_t<6> dx;
        variable_t<7> dy;
        return determinant(sqr(ax - dx) + sqr(ay - dy), ax - dx, ay - dy,
                           sqr(bx - dx) + sqr(by - dy), bx - dx, by - dy,
                           sqr(cx - dx) + sqr(cy - dy), cx - dx, cy - dy);
    };
    predicate_t<decltype(in_circle_expression()),
            double_step_t,
            interval_step_t,
            gmp_step_t> p;
    return static_cast<in_circle_t>(p(a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y));
}

in_circle_t in_circle(point_t a, point_t b, point_t x)
{
    using namespace geometry::predicate;
    predicate_t<in_circle_expression_t, double_step_t, custom_step_t> p;
    return static_cast<in_circle_t>(-p(a.x, a.y, b.x, b.y, x.x, x.y));
}

turn_t turn(
        point_t const a1, point_t const a2, point_t const b1, point_t const b2)
{
    using namespace geometry::predicate;
    predicate_t<independent_turn_expression_t,
            double_step_t,
            interval_step_t,
            gmp_step_t> p;
    return static_cast<turn_t>(p(a1.x, a1.y, a2.x, a2.y,
                                 b1.x, b1.y, b2.x, b2.y));
}

compare_distance_t compare_distance(
        point_t const a1, point_t const a2, point_t const b1, point_t const b2)
{
    using namespace geometry::predicate;
    predicate_t<compare_distance_expression_t,
            double_step_t,
            interval_step_t,
            custom_step_t> p;
    return static_cast<compare_distance_t>(p(a1.x, a1.y, a2.x, a2.y,
                                             b1.x, b1.y, b2.x, b2.y));
}

intersect_t intersect(
        point_t const a1, point_t const a2, point_t const b1, point_t const b2)
{
    if (check_bounding_box(a1, a2, b1, b2))
    {
        int check1 = turn(a1, a2, b1) * turn(a1, a2, b2);
        if (check1 == 1)
        {
            return disjoint;
        }
        int check2 = turn(b1, b2, a1) * turn(b1, b2, a2);
        if (check2 == 1)
        {
            return disjoint;
        }
        if (check1 == 0 || check2 == 0)
        {
            return tangent;
        }
        return intersecting;
    }
    return disjoint;
}

compare_distance_t compare_distance_to_line(
        point_t const a1, point_t const a2, point_t const b, point_t const c)
{
    return static_cast<compare_distance_t>(turn(a1, a2, b, c));
}

} // end of namespace geometry
