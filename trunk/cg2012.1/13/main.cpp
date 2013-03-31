#include <algorithm>
#include <functional>
#include <fstream>
#include <vector>

#include "geometry/geometry_fwd.h"
#include "geometry/voronoi_diagram.h"

template<typename ForwardIterator>
int count_triangles(ForwardIterator begin, ForwardIterator end)
{
    int triangles_count = 0;
    for (; begin != end; ++begin)
    {
        triangles_count += begin->sites.size() - 2;
    }
    return triangles_count;
}

// Will order points counter-clockwise.
struct turn_pred
{
    typedef geometry::point_t point_t;
    point_t const* control_point;
    turn_pred(point_t const* control_point)
        : control_point(control_point)
    {}
    bool operator()(point_t const* l, point_t const* r) const
    {
        return geometry::left == geometry::turn(*control_point, *l, *r);
    }
};

int run(std::istream& in, std::ostream& out)
{
    int n;
    in >> n;
    std::vector<geometry::point_t> points(n);
    for (auto& p: points)
    {
        in >> p;
    }
    geometry::voronoi_diagram vd(points.begin(), points.end());

    out << count_triangles(vd.vertices_begin(), vd.vertices_end()) << '\n';

    using geometry::point_t;
    using namespace std::placeholders;
    auto get_index = std::bind(
                std::distance<point_t const*>, &*vd.sites_begin(), _1);

    for (auto v_it = vd.vertices_begin(), end = vd.vertices_end();
         v_it != end; ++v_it)
    {
        auto& vertex = *v_it;
        auto site = vertex.sites.front();
        std::vector<point_t const*> points(++vertex.sites.begin(),
                                           vertex.sites.end());
        std::sort(points.begin(), points.end(), turn_pred(site));
        point_t const* prev = points.front();
        for (auto it = ++points.begin(), end = points.end(); it != end; ++it)
        {
            out << 1 + get_index(site) << ' ' << 1 + get_index(prev);
            out << ' ' << 1 + get_index(*it) << '\n';
            prev = *it;
        }
    }

    return 0;
}

int main()
{
    return run(std::cin, std::cout);
}
