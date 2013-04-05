#ifndef GEOMETRY_POINT_BASE_H
#define GEOMETRY_POINT_BASE_H

#include <iostream>

namespace geometry
{

template<typename T>
struct point_base_t
{
    T x;
    T y;
    point_base_t() = default;

    template<typename U>
    explicit point_base_t(point_base_t<U> const& o)
        : x(o.x)
        , y(o.y)
    {}

    template<typename U, typename V>
    point_base_t(U const& x, V const& y)
        : x(x)
        , y(y)
    {}
};

template<typename T, typename S>
bool operator<(point_base_t<T> const& a, point_base_t<S> const& b)
{
    return a.x < b.x || (a.x == b.x && a.y < b.y);
}

template<typename T, typename S>
bool operator<=(point_base_t<T> const& a, point_base_t<S> const& b)
{
    return a.x < b.x || (a.x == b.x && a.y <= b.y);
}

template<typename T, typename S>
bool operator>(point_base_t<T> const& a, point_base_t<S> const& b)
{
    return !(a <= b);
}

template<typename T, typename S>
bool operator>=(point_base_t<T> const& a, point_base_t<S> const& b)
{
    return !(a < b);
}

template<typename T, typename S>
bool operator==(point_base_t<T> const& a, point_base_t<S> const& b)
{
    return a.x == b.x && a.y == b.y;
}

template<typename T, typename S>
bool operator!=(point_base_t<T> const& a, point_base_t<S> const& b)
{
    return !(a == b);
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, point_base_t<T> const& p)
{
    stream << p.x << ' ' << p.y;
    return stream;
}

template<typename T>
std::istream& operator>>(std::istream& stream, point_base_t<T>& p)
{
    stream >> p.x >> p.y;
    return stream;
}

typedef point_base_t<double> point_t;

} // end of namespace geometry

#endif // GEOMETRY_POINT_BASE_H
