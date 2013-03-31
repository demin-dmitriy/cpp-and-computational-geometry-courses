#ifndef GEOMETRY_SEGMENT_BASE_H
#define GEOMETRY_SEGMENT_BASE_H

#include "geometry/point_base.h"

namespace geometry
{

template<typename T>
struct segment_base_t
{
    point_base_t<T> a;
    point_base_t<T> b;
    segment_base_t() = default;
    segment_base_t(point_base_t<T> const& a, point_base_t<T> const& b)
        : a(a)
        , b(b)
    {}
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, segment_base_t<T> const& i)
{
    stream << i.a << ' ' << i.b;
    return stream;
}

template<typename T>
std::istream& operator>>(std::istream& stream, segment_base_t<T>& i)
{
    stream >> i.a >> i.b;
    return stream;
}

typedef segment_base_t<double> segment_t;

} // end of namespace geometry

#endif // GEOMETRY_SEGMENT_BASE_H
