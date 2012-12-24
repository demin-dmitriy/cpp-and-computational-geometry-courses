#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iosfwd>

namespace geometry
{

struct point
{
    double x;
    double y;
    point()
    {}
    point(double const x, double const y)
        : x(x)
        , y(y)
    {}
    bool operator==(point const& o) const
    {
        return x == o.x && y == o.y;
    }
    bool operator!=(point const& o) const
    {
        return !(*this == o);
    }
};

std::ostream& operator<<(std::ostream&, geometry::point const&);

std::istream& operator>>(std::istream&, geometry::point&);

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

} // namespace geometry


#include "convex_hull.impl.h"

#endif
