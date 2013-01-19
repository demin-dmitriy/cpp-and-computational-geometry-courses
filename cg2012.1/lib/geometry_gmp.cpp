// This includes implementations of functions that require gmp.

#include "geometry.h"

#include <cfloat>

#include <gmp.h>

namespace
{

using geometry::point;
using geometry::in_circle_result;

// probably better to use another expression.
in_circle_result robust_in_circle(
        point const a, point const b, point const c, point const d)
{
    mpq_t adx, ady, bdx, bdy, cdx, cdy;
    mpq_inits(adx, ady, bdx, bdy, cdx, cdy, NULL);
    mpq_t tmp1, tmp2, tmp3;
    mpq_inits(tmp1, tmp2, tmp3, NULL);

    mpq_set_d(tmp2, d.x);
    mpq_set_d(tmp3, d.y);

    mpq_set_d(tmp1, a.x);
    mpq_sub(adx, tmp1, tmp2);

    mpq_set_d(tmp1, a.y);
    mpq_sub(ady, tmp1, tmp3);

    mpq_set_d(tmp1, b.x);
    mpq_sub(bdx, tmp1, tmp2);

    mpq_set_d(tmp1, b.y);
    mpq_sub(bdy, tmp1, tmp3);

    mpq_set_d(tmp1, c.x);
    mpq_sub(cdx, tmp1, tmp2);

    mpq_set_d(tmp1, c.y);
    mpq_sub(cdy, tmp1, tmp3);

    mpq_t adxy2, bdxy2, cdxy2;
    mpq_inits(adxy2, bdxy2, cdxy2, NULL);

    mpq_mul(adxy2, adx, adx);
    mpq_mul(tmp1, ady, ady);
    mpq_add(adxy2, adxy2, tmp1);

    mpq_mul(bdxy2, bdx, bdx);
    mpq_mul(tmp1, bdy, bdy);
    mpq_add(bdxy2, bdxy2, tmp1);

    mpq_mul(cdxy2, cdx, cdx);
    mpq_mul(tmp1, cdy, cdy);
    mpq_add(cdxy2, cdxy2, tmp1);

    mpq_t result;
    mpq_init(result);

    mpq_mul(tmp1, adx, bdy);
    mpq_mul(tmp2, bdx, ady);
    mpq_sub(tmp1, tmp1, tmp2);
    mpq_mul(tmp1, tmp1, cdxy2);
    mpq_add(result, result, tmp1);

    mpq_mul(tmp1, bdx, cdy);
    mpq_mul(tmp2, cdx, bdy);
    mpq_sub(tmp1, tmp1, tmp2);
    mpq_mul(tmp1, tmp1, adxy2);
    mpq_add(result, result, tmp1);

    mpq_mul(tmp1, adx, cdy);
    mpq_mul(tmp2, cdx, ady);
    mpq_sub(tmp1, tmp1, tmp2);
    mpq_mul(tmp1, tmp1, bdxy2);
    mpq_sub(result, result, tmp1);

    in_circle_result res = static_cast<in_circle_result>(mpq_sgn(result));
    mpq_clear(result);
    mpq_clears(tmp1, tmp2, tmp3, NULL);
    mpq_clears(adxy2, bdxy2, cdxy2, NULL);
    mpq_clears(adx, ady, bdx, bdy, cdx, cdy, NULL);
    return res;
}

in_circle_result fast_in_circle(
        point const a, point const b, point const c, point const d)
{
    using std::abs;
    double adx = a.x - d.x;
    double ady = a.y - d.y;
    double bdx = b.x - d.x;
    double bdy = b.y - d.y;
    double cdx = c.x - d.x;
    double cdy = c.y - d.y;

    double adxy2 = adx * adx + ady * ady;
    double bdxy2 = bdx * bdx + bdy * bdy;
    double cdxy2 = cdx * cdx + cdy * cdy;

    double bdxcdy = bdx * cdy;
    double bdycdx = bdy * cdx;
    double cdxady = cdx * ady;
    double cdyadx = cdy * adx;
    double adxbdy = adx * bdy;
    double adybdx = ady * bdx;

    double res = adxy2 * (bdxcdy - bdycdx)
            - bdxy2 * (cdyadx - cdxady)
            + cdxy2 * (adxbdy - adybdx);
    double errbound = 8 * DBL_EPSILON * (adxy2 * (abs(bdxcdy) + abs(bdycdx))
            + bdxy2 * (abs(cdxady) + abs(cdyadx))
            + cdxy2 * (abs(adxbdy) + abs(adybdx)));
    if (res > errbound)
    {
        return geometry::inside;
    }
    else if (res < errbound)
    {
        return geometry::outside;
    }

    return geometry::inexact;
}

} // end of anonymous namespace

namespace geometry
{

in_circle_result in_circle(
        point const a, point const b, point const c, point const d)
{
    in_circle_result res = fast_in_circle(a, b, c, d);
    if (res == geometry::inexact)
    {
        res = robust_in_circle(a, b, c , d);
    }
    return res;
}

} // end of namespace geometry
