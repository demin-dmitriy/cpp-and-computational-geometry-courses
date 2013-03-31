#ifndef GEOMETRY_MATH_H
#define GEOMETRY_MATH_H

#include <cmath>

namespace geometry
{
// Must be unnecessary.
template<typename Type>
Type determinant(Type const& x11, Type const& x12,
                 Type const& x21, Type const& x22)
{
    return x11 * x22 - x12 * x21;
}

// Deprecated with apperearance of predicate module
inline double determinant_roundoff(double const x11, double const x12,
                            double const x21, double const x22)
{
    using std::abs;
    return abs(x11 * x22) + abs(x12 * x21);
}

} // end of namespace geometry

#endif // GEOMETRY_MATH_H
