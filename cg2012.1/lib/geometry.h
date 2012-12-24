#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iosfwd>

namespace geometry
{

template<typename T>
struct base_point
{
    T x;
    T y;
    base_point()
    {}
    base_point(T x, T y)
        : x(x)
        , y(y)
    {}
    bool operator==(base_point<T> const& o) const
    {
        return x == o.x && y == o.y;
    }
    bool operator!=(base_point<T> const& o) const
    {
        return !(*this == o);
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, base_point<T> const& p)
{
    stream << p.x << ' ' << p.y;
    return stream;
}

template<typename T>
std::istream& operator>>(std::istream& stream, base_point<T>& p)
{
    stream >> p.x >> p.y;
    return stream;
}

// Needs some renaming (later).
typedef base_point<double> point;
typedef base_point<int> cell;

// Returns sign([a1b, a1a2])
int left_turn(point const a1, point const a2, point const b);

// Checks whether segments a1a2 and b1b2 intersect.
// Returns true if they do and false otherwise.
bool intersect(point const a1, point const a2, point const b1, point const b2);

// Returns true if b farther from line a1a2 than c.
bool farther_than(point const a1, point const a2, point const b, point const c);

template<typename BidirectionalIterator, typename OutputIterator>
OutputIterator convex_hull(
        BidirectionalIterator first,
        BidirectionalIterator last,
        OutputIterator out);


template<typename OutputIterator>
OutputIterator rasterize(point a, point b, OutputIterator out);

} // namespace geometry


#include "convex_hull.impl.h"
#include "rasterize.impl.h"

#endif
