#include <fstream>
#include <vector>

#include "geometry.h"

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
    int triangles_count = 0;
    for (auto v_it = vd.vertices_begin(), end = vd.vertices_end();
         v_it != end; ++v_it)
    {
        triangles_count += v_it->sites.size() - 2;
    }
    out << triangles_count << '\n';

    // Will order points counter-clockwise.
    using geometry::point_t;
    struct turn_pred
    {
        point_t const* control_point;
        turn_pred(point_t const* control_point)
            : control_point(control_point)
        {}
        bool operator()(point_t const* l, point_t const* r) const
        {
            return -1 == geometry::left_turn(*control_point, *r, *l);
        }
    };

    point_t const* first = &*vd.sites_begin();
    auto get_index = [first](point_t const* p) {
        return p - first + 1;
    };

    for (auto v_it = vd.vertices_begin(), end = vd.vertices_end();
         v_it != end; ++v_it)
    {
        auto& vertex = *v_it;
        auto site = vertex.sites.front();
        std::vector<point_t const*> points(++vertex.sites.begin(),
                                           vertex.sites.end());
        sort(points.begin(), points.end(), turn_pred(site));
        point_t const* prev = points.front();
        for (auto it = ++points.begin(), end = points.end(); it != end; ++it)
        {
            out << get_index(site) << ' ' << get_index(prev);
            out << ' ' << get_index(*it) << '\n';
            prev = *it;
        }
    }

    return 0;
}

int main()
{
    return run(std::cin, std::cout);
}
