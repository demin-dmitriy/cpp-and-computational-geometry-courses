#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iosfwd>
#include <utility>

namespace geometry
{

template<typename T>
struct base_point;

template<typename T>
std::ostream& operator<<(std::ostream& stream, base_point<T> const& p);

template<typename T>
std::istream& operator>>(std::istream& stream, base_point<T>& p);

// Probably needs some renaming (later).
typedef base_point<double> point;

typedef base_point<int> cell;

// <todo: Fix description>.
// Returns sign([a1b; a1a2])
int left_turn(point a1, point a2, point b);

// Checks whether segments a1a2 and b1b2 intersect.
// Returns true if they do and false otherwise.
bool intersect(point a1, point a2, point b1, point b2);

// Returns true if b farther from line a1a2 than c and false otherwise.
bool farther_than(point a1, point a2, point b, point c);

// Compares lengths of a1a2 and b1b2.
// Returns sign(length(a1,a2) - length(b1,b2))
int compare_distance(point a1, point a2, point b1, point b2);

// Calculates a minimal set of points forming convex hull of points in
// [first; last). Puts its result into a range beginning at out and returns
// an iterator pointing to the past-the-end element in the resulting sequence.
//
// As a side-effect it changes the order of elements in [first; last).
//
// Points in resulting sequence a ordered in clockwise order begining from the
// leftmost point.
template<typename BidirectionalIterator, typename OutputIterator>
OutputIterator convex_hull(
        BidirectionalIterator first,
        BidirectionalIterator last,
        OutputIterator out);

// Finds all cells which segment ab intersects and puts them into a range
// beginning at out.
//
// Returns an iterator pointing to the past-the-end element in the resulting
// sequence.
//
// Сells are ordered in ascending order of coordinate x and in the same order
// that segment ab intersecets them.
template<typename OutputIterator>
OutputIterator rasterize(point a, point b, OutputIterator out);

// Finds a pair of two most distant points in range [first; last).
template<typename BidirectionalIterator>
std::pair<point, point> diameter(
        BidirectionalIterator first,
        BidirectionalIterator last);

} // namespace geometry

#include "base_point.impl.h"
#include "convex_hull.impl.h"
#include "rasterize.impl.h"
#include "diameter.impl.h"

#endif // GEOMETRY_H
