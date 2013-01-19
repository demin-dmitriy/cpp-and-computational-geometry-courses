#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iosfwd>
#include <utility>

namespace geometry
{

template<typename T>
struct base_point_t;

template<typename T>
std::ostream& operator<<(std::ostream& stream, base_point_t<T> const& p);

template<typename T>
std::istream& operator>>(std::istream& stream, base_point_t<T>& p);

// Probably needs some renaming (later).
typedef base_point_t<double> point;

typedef base_point_t<int> cell;

struct circle_t;

// <todo: Fix description>.
// <todo: rename point to point_t and cell to cell_t>
// <todo: replace integer results with enum values>

/*
 * Returns sign([a1b; a1a2])
 *
 * Robust. uses adaptive precision.
 */
int left_turn(point a1, point a2, point b);

/*
 * Checks whether segments a1a2 and b1b2 intersect.
 * Returns true if they do and false otherwise.
 *
 * Robust. Uses adaptive precision.
 */
bool intersect(point a1, point a2, point b1, point b2);

/*
 * Returns true if b farther from line a1a2 than c and false otherwise.
 *
 * Robust. Uses adaptive precision.
 */
bool farther_than(point a1, point a2, point b, point c);

/*
 * Compares lengths of a1a2 and b1b2.
 * Returns sign(length(a1,a2) - length(b1,b2))
 *
 * Robust. Uses adaptive precision.
 */
int compare_distance(point a1, point a2, point b1, point b2);

enum in_circle_result
{
    outside = -1,
    on_border = 0,
    inside = 1,
    inexact = -3
};

/*
 * Finds position of point x relative to closed circle constructed by points
 * a and b where segment ab is a diameter.
 *
 * Robust. Uses adaptive precision.
 */
in_circle_result in_circle(point a, point b, point x);

/*
 * Finds position of point x relative to closed oriented circle constructed by
 * points a, b and c. Oriented circle means that points a, b and c should be
 * ordered counterclockwise to make this function produce correct results.
 * If points ordered clockwise than it will reverse output so that outside
 * will become inside and vice versa.
 *
 * Robust. Uses GMP.
 */
in_circle_result in_circle(point a, point b, point c, point x);

/*
 * Calculates a minimal set of points forming convex hull of points in
 * [first; last). Puts its result into a range beginning at out and returns
 * an iterator pointing to the past-the-end element in the resulting sequence.
 *
 * As a side-effect it changes the order of elements in [first; last).
 *
 * Points in resulting sequence a ordered in clockwise order begining from the
 * leftmost point.
 *
 * Algorithm: quick hull.
 * Time complexity: O(n log(n)) expected time. O(n ^ 2) in worst case.
 * Memory usage: O(log(n)) expected. O(n) in worst case.
 */
template<typename BidirectionalIterator, typename OutputIterator>
OutputIterator convex_hull(
        BidirectionalIterator first,
        BidirectionalIterator last,
        OutputIterator out);

/*
 * Finds all cells which segment ab intersects and puts them into a range
 * beginning at out.
 *
 * Returns an iterator pointing to the past-the-end element in the resulting
 * sequence.
 *
 * Сells are ordered in ascending order of coordinate x and in the same order
 * that segment ab intersecets them.
 *
 * Time complexity: O(cell amount) (equals |b.x - a.x| + |b.y - a.y| + 2).
 * Memory usage: O(1).
 */
template<typename OutputIterator>
OutputIterator rasterize(point a, point b, OutputIterator out);

/*
 * Finds a pair of two most distant points from range [first; last).
 *
 * It uses convex_hull as subprocedure therefore it changes order of elements
 * in [first; last)
 *
 * Time complexity O(convex_hull) + O(n). (O(n log(n)) expected time).
 * Memory usage: O(convex hull size) + O(1).
 */
template<typename BidirectionalIterator>
std::pair<point, point> diameter(
        BidirectionalIterator first,
        BidirectionalIterator last);
/*
 *
 */
template<typename RandomAccessIterator>
circle_t mindisk(RandomAccessIterator first, RandomAccessIterator last);

/*
 *
 */
template<typename ForwardIterator>
circle_t b_mindisk(ForwardIterator first, ForwardIterator last, int n = 0);

} // namespace geometry

#include "base_point.impl.h"
#include "circle.impl.h"
#include "convex_hull.impl.h"
#include "rasterize.impl.h"
#include "diameter.impl.h"
#include "mindisk.impl.h"

#endif // GEOMETRY_H
