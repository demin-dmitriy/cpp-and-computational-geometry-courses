#include <algorithm>
#include <functional>

namespace geometry
{

struct left_turn_pred_
{
    bool operator()(point_t const& p)
    {
        return left_turn(p1, p2, p) > 0;
    }

    left_turn_pred_(point_t const& p1, point_t const& p2)
        : p1(p1)
        , p2(p2)
    {}

    point_t const& p1;
    point_t const& p2;
};

template<typename BidirectionalIterator, typename OutputIterator>
void quick_hull_iteration_(
        BidirectionalIterator first,
        BidirectionalIterator last,
        point_t const p1,
        point_t const p2,
        OutputIterator& out)
{
    using namespace std;
    using namespace std::placeholders;
    if (first != last)
    {
        point_t const mid_p = *std::max_element(
                    first, last, std::bind(farther_than, p1, p2, _1, _2));
        if (left_turn(p1, p2, mid_p) == -1)
        {
            auto const mid = std::partition(
                    first, last, left_turn_pred_(mid_p, p1));
            quick_hull_iteration_(first, mid, p1, mid_p, out);
            *out++ = mid_p;
            quick_hull_iteration_(mid, last, mid_p, p2, out);
        }
    }
}

template<typename BidirectionalIterator, typename OutputIterator>
OutputIterator convex_hull(
        BidirectionalIterator first,
        BidirectionalIterator last,
        OutputIterator out)
{
    if (first != last)
    {
        auto const pred = [](point_t a, point_t b) {
            return (a.x != b.x) ? (a.x < b.x) : (a.y < b.y);
        };
        point_t const p1 = *std::min_element(first, last, pred);
        point_t const p2 = *std::max_element(first, last, pred);
        *out++ = p1;
        if (p1 != p2)
        {
            auto const mid = std::partition(
                        first, last, left_turn_pred_(p2, p1));
            quick_hull_iteration_(first, mid, p1, p2, out);
            *out++ = p2;
            quick_hull_iteration_(mid, last, p2, p1, out);
        }
    }
    return out;
}

} // end of namespace geometry
