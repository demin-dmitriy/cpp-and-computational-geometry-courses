// See description of used algorighm at http://www.cs.cmu.edu/~quake/robust.html

#ifndef GEOMETRY_ROBUST_ARITHMETIC_H
#define GEOMETRY_ROBUST_ARITHMETIC_H

#include <cstddef>
#include <limits>

#ifndef __GNUC__
    #pragma message ("You are not using GCC compiler. Make sure that code in " \
    "this header optimized correctly so you might need to use an option " \
    "similar to -ffloat-store from GCC compiler.")
#else
    // "#pragma GCC optimize" is broken in gcc.
    #define float_store_option __attribute__((optimize("-ffloat-store")))

#endif

namespace adaptive_arithmetic
{

// todo: Doesn't work.
void fast_two_sum(const double a, const double b, double &x, double &y) float_store_option;
void two_sum(const double a, const double b, double &x, double &y) float_store_option;
template<int M> void grow_expansion(double const e[M], double const b, double h[M + 1]) float_store_option;
template <int M, int N> void expansion_sum(double e[N + M], double const f[N]) float_store_option;
template<size_t s> void split(double const a, double& a_hi, double& a_lo) float_store_option;
void two_product(double const a, double const b, double& x, double& y) float_store_option;
template<size_t N> int sign(double const expansion[N]) float_store_option;
template<size_t M> void scale_expansion(double e[M], double b, double h[2 * M]) float_store_option;

//requires |a| >= |b|
inline void fast_two_sum(
        double const a, double const b, double& x, double& y)
{
    x = a + b;
    double b_virtual = x - a;
    y = b - b_virtual;
}

inline void two_sum(double const a, double const b, double& x, double& y)
{
    x = a + b;
    double b_virtual = x - a;
    double a_virtual = x - b_virtual;
    double b_roundoff = b - b_virtual;
    double a_roundoff = a - a_virtual;
    y = a_roundoff + b_roundoff;
}

template<int M>
void grow_expansion(
        double const e[M], double const b, double h[M + 1])
{
    double q = b;
    for (int i = 0; i < M; ++i)
    {
        two_sum(q, e[i], q, h[i]);
    }
    h[M] = q;
}

/* There exists faster version of expansion sum
 * which works at linear time.
 * However, at small arrays and with unrolling cycles
 * optimization, this one can produce better results.
 * However it needs to be tested.
 */
template <int M, int N>
void expansion_sum(double e[N + M], double const f[N])
{
    for (int i = 0; i < N; ++i)
    {
        grow_expansion<M>(e + i, f[i], e + i);
    }
}

template<size_t s>
void split(double const a, double& a_hi, double& a_lo)
{
    static_assert(
        std::numeric_limits<double>::digits / 2 <= s
                && s <= std::numeric_limits<double>::digits - 1
        , "Bad parameter for split function");
    double c = ((1 << s) + 1) * a;
    double a_big = c - a;
    a_hi = c - a_big;
    a_lo = a - a_hi;
}

inline void two_product(
        double const a, double const b, double& x, double& y)
{
    x = a * b;
    double a_hi, a_lo;
    double b_hi, b_lo;
    split<(std::numeric_limits<double>::digits + 1) / 2>(a, a_hi, a_lo);
    split<(std::numeric_limits<double>::digits + 1) / 2>(b, b_hi, b_lo);
    double err1 = x - (a_hi * b_hi);
    double err2 = err1 - (a_lo * b_hi);
    double err3 = err2 - (a_hi * b_lo);
    y = (a_lo * b_lo) - err3;
}

template<size_t N>
int sign(double const expansion[N])
{
    for (int i = N - 1; i >= 0; --i)
    {
        if (expansion[i] > 0)
        {
            return 1;
        }
        else if (expansion[i] < 0)
        {
            return -1;
        }
    }
    return 0;
}

template<size_t M>
void scale_expansion(double e[M], double b, double h[2 * M])
{
    double Q;
    two_product(e[0], b, Q, h[0]);
    size_t h_index = 1;
    for (size_t e_index = 1; e_index < M; ++e_index)
    {
        double T, t;
        two_product(e[e_index], b, T, t);
        two_sum(Q, t, Q, h[h_index]);
        ++h_index;
        fast_two_sum(T, Q, Q, h[h_index]);
        ++h_index;
    }
    h[h_index] = Q;
}

} // end of namespace adaptive_arithmetic

#endif // GEOMETRY_ROBUST_ARITHMETIC_H
