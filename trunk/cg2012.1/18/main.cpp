#include "geometry/voronoi_diagram.h"

#include <iostream>
#include <iterator>
#include <vector>
#include <fstream>

using namespace std;
using namespace geometry;

int run(istream& in, ostream& out)
{
    int n;
    in >> n;
    vector<point_t> points(n);
    for (auto& p: points)
    {
        in >> p;
    }
    voronoi_diagram vd(points.begin(), points.end());
    out << vd.vertices_count() << '\n';
    auto sites_begin_p = &*vd.sites_begin();
    auto get_site_index = [&](voronoi_diagram::site_t const* site) -> int
    {
        return (site - sites_begin_p) + 1;
    };
    for (auto it = vd.vertices_begin(), end = vd.vertices_end();
         it != end; ++it)
    {
        out << it->sites.size();
        for (auto site: it->sites)
        {
            out << ' ' << get_site_index(site);
        }
        out << '\n';
    }

    out << vd.edges_count() << '\n';
    auto vertices_begin_p = &*vd.vertices_begin();
    auto get_vertex_index = [&](voronoi_diagram::vertex_t const* v) -> int
    {
        return (v - vertices_begin_p) + 1;
    };

    for (auto it = vd.half_edges_begin(), end = vd.half_edges_end();
         it != end; std::advance(it, 2))
    {
        typedef voronoi_diagram::half_edge_t half_edge_t;
        half_edge_t const* edge = &*it;
        if (edge->target == nullptr)
        {
            edge = edge->twin;
        }
        if (edge->target == nullptr)
        {
            out << "infinity infinity " << get_site_index(edge->face);
            out << ' ' << get_site_index(edge->twin->face);
        }
        else
        {
            out << get_vertex_index(edge->target) << ' ';
            if (edge->twin->target == nullptr)
            {
                out << "infinity " << get_site_index(edge->twin->face);
                out << ' ' << get_site_index(edge->face);
            }
            else
            {
                out << get_vertex_index(edge->twin->target);
            }
        }
        out << '\n';
    }
    return 0;
}

int main()
{
    return run(cin, cout);
}
