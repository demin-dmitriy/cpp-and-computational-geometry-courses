#ifndef GEOMETRY_GEOMETRY_FWD_H
#define GEOMETRY_GEOMETRY_FWD_H

#include <iosfwd>
#include <utility>

namespace geometry
{

template<typename T>
struct point_base_t;

typedef point_base_t<double> point_t;

typedef point_base_t<int> cell_t;

struct circle_t;

/*
 * Checks if rectangles defined by segments a1a2 and b1b2 intersect.
 */
bool check_bounding_box(
        point_t const a1, point_t const a2, point_t const b1, point_t const b2);

enum turn_t
{
    left = -1,
    collinear = 0,
    right = 1
};

/*
 * Returns orientation of point b relative to a1a2.
 */
turn_t turn(point_t a1, point_t a2, point_t b);

/*
 * Return orientation of vector b1b2 relative to vector a1a2
 */
turn_t turn(point_t a1, point_t a2, point_t b1, point_t b2);

enum intersect_t
{
    intersecting = -1, // segments intersect with their interior
    tangent = 0,
    disjoint = 1
};

/*
 * Checks whether segments a1a2 and b1b2 intersect.
 * Returns true if they do and false otherwise.
 */
intersect_t intersect(point_t a1, point_t a2, point_t b1, point_t b2);

enum compare_distance_t
{
    closer = -1,
    equidistant = 0,
    farther = 1
};

/*
 * Compares if point b is closer to the line a1a2 than point c.
 */
compare_distance_t compare_distance_to_line(
        point_t a1, point_t a2, point_t b, point_t c);

/*
 * Compares lengths of segments a1a2 and b1b2.
 */
compare_distance_t compare_distance(
        point_t a1, point_t a2, point_t b1, point_t b2);

enum in_circle_t
{
    outside = -1,
    on_border = 0,
    inside = 1
};

/*
 * Finds position of point x relative to closed circle constructed by points
 * a and b where segment ab is a diameter.
 */
in_circle_t in_circle(point_t a, point_t b, point_t x);

/*
 * Finds position of point x relative to closed oriented circle constructed by
 * points a, b and c. Oriented circle means that points a, b and c should be
 * ordered counterclockwise to make this function produce correct results.
 * If points ordered clockwise than it will reverse output so that outside
 * will become inside and vice versa.
 */
in_circle_t in_circle(point_t a, point_t b, point_t c, point_t x);

/*
 * Calculates minimal set of points forming convex hull of points in
 * [first; last). Puts its result into a range beginning at out and returns an
 * iterator pointing to the past-the-end element in the resulting sequence.
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
OutputIterator rasterize(point_t a, point_t b, OutputIterator out);

/*
 * Finds a pair of two most distant points from range [first; last).
 *
 * It uses convex_hull as subprocedure therefore it changes order of elements
 * in [first; last)
 *
 * Time complexity: O(convex_hull) + O(n). (O(n log(n)) expected time).
 * Memory usage: O(convex hull size) + O(1).
 */
template<typename BidirectionalIterator>
std::pair<point_t, point_t> diameter(
        BidirectionalIterator first,
        BidirectionalIterator last);

/*
 * Finds minimum circle containing all points in range [first; last).
 * Randomizes points order on enter. If sequence of point are known to be random
 * or there is no random access iterator avaliable then b_mindisk(first, last)
 * could be used instead.
 *
 * Order of points in [first; last) is not preserved.
 *
 * Returns minimum circumcircle.
 *
 * Time complexity: O(n) expected time.
 * Memory usage: O(1).
 */
template<typename RandomAccessIterator>
circle_t mindisk(RandomAccessIterator first, RandomAccessIterator last);

/*
 * Finds minimum circle containing all points in range [first; last).
 * Parameter n is for internal use and should be left default.
 *
 * As side effect it shuffles order of points in [first; last) during it's work.
 * Order of points in [first; last) should be randomized before call to expect
 * consumed time to be O(n).
 *
 * Returns minimum circumcircle.
 *
 * Time complexity: O(n) expected time.
 * Memory usage: O(1)
 */
template<typename ForwardIterator>
circle_t b_mindisk(ForwardIterator first, ForwardIterator last, int n = 0);

/*
 * Calculates voronoi diagram for set of points in range [first; last).
 * Constructor:
 *  template<typename ForwardIterator>
 *  voronoi_diagram(ForwardIterator begin, ForwardIterator end);
 *
 * Prerequirement: points in range [first; last) shouldn't have duplicates.
 *
 * Voronoi diagram is constructed once at voronoi_diagram object creation and
 * couldn't be modified or recalculated during it's lifetime.
 *
 * You can traverse through sites, half_edges and vertices.
 * See voronoi_diagram.impl.h file for avaliable interface methods and
 * structures.
 *
 * Fortune's algorithm implemented.
 *
 * Time complexity: O(n log(n)) in worst case.
 * Memory usage: O(n).
 */
struct voronoi_diagram;

/*
 * For given set of segments in range [first; last) finds all intersections and
 * puts them in a range beggining at out.
 *
 * Bentley–Ottmann algorithm implemented.
 *
 * Time complexity: O((n + k) log(n)) in worst case where k is amount of
 * intersections.
 * Memory usage: O(n + k).
 */
template<typename InputIterator, typename OutputIterator>
OutputIterator find_intersections(InputIterator first,
                                  InputIterator last,
                                  OutputIterator out);
} // end of namespace geometry

#endif // GEOMETRY_GEOMETRY_FWD_H
