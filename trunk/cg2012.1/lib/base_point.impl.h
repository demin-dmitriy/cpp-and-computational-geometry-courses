#include <iostream>

namespace geometry
{

template<typename T>
struct base_point_t
{
    T x;
    T y;
    base_point_t()
    {}
    base_point_t(T x, T y)
        : x(x)
        , y(y)
    {}
    bool operator==(base_point_t<T> const& o) const
    {
        return x == o.x && y == o.y;
    }
    bool operator!=(base_point_t<T> const& o) const
    {
        return !(*this == o);
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, base_point_t<T> const& p)
{
    stream << p.x << ' ' << p.y;
    return stream;
}

template<typename T>
std::istream& operator>>(std::istream& stream, base_point_t<T>& p)
{
    stream >> p.x >> p.y;
    return stream;
}

} // end of namespace geometry
