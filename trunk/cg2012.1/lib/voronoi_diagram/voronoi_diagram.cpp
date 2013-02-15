#include "geometry.h"
#include "voronoi_diagram_construction.h"

#include <cassert>

#include <set>

#include <boost/iterator/counting_iterator.hpp>
#include <boost/pending/disjoint_sets.hpp>

namespace
{

using pre_vertex_t = geometry::detail::pre_vertex_t;
using vd_t = geometry::voronoi_diagram;

bool coincide(pre_vertex_t const& a, pre_vertex_t const& b)
{
    using namespace geometry;
    for (size_t i = 0; i != b.size(); ++i)
    {
        if (on_border != in_circle(*a[0], *a[1], *a[2], *b[i]))
        {
            return false;
        }
    }
    return true;
}

}

namespace geometry
{

vd_t::half_edge_const_iterator vd_t::half_edges_begin() const
{
    return edges_.cbegin();
}

vd_t::half_edge_const_iterator vd_t::half_edges_end() const
{
    return edges_.cend();
}

vd_t::site_const_iterator vd_t::sites_begin() const
{
    return sites_.cbegin();
}

vd_t::site_const_iterator vd_t::sites_end() const
{
    return sites_.cend();
}

vd_t::vertex_const_iterator vd_t::vertices_begin() const
{
    return vertices_.cbegin();
}

vd_t::vertex_const_iterator vd_t::vertices_end() const
{
    return vertices_.cend();
}

size_t vd_t::edges_count() const
{
    return edges_.size() / 2;
}

size_t vd_t::half_edges_count() const
{
    return edges_.size();
}

size_t vd_t::vertices_count() const
{
    return vertices_.size();
}

size_t vd_t::sites_count() const
{
    return sites_.size();
}

void vd_t::swap(vd_t& o)
{
    edges_.swap(o.edges_);
    sites_.swap(o.sites_);
    vertices_.swap(o.vertices_);
}

void vd_t::process()
{
    geometry::detail::voronoi_diagram_construction vdc(sites_);
    // Merge vertecies that connected with zero-length edge.
    pre_vertex_t const* p_begin = &vdc.pre_vertices.front();
    auto get_index = [&](pre_vertex_t const* p) -> size_t {
        return (p - p_begin);
    };
    int n = vdc.pre_vertices.size();
    std::vector<int> rank(n);
    std::vector<int> parent(n);
    boost::disjoint_sets<int*, int*> ds(&rank[0], &parent[0]);
    for (int i = 0; i != n; ++i)
    {
        ds.make_set(i);
    }
    using mybool = char;
    size_t vdc_edges_size = vdc.edges.size();
    assert(vdc_edges_size % 2 == 0);
    std::vector<mybool> zero_edges(vdc_edges_size, false);
    int edges_count = 0; // Amount of non-zero legth edges.
    for (size_t i = 0; i != vdc_edges_size; i += 2)
    {
        auto& edge = vdc.edges[i];
        if (edge.target && edge.twin->target
                && coincide(*edge.target, *edge.twin->target))
        {
            size_t a = get_index(edge.target);
            size_t b = get_index(edge.twin->target);
            ds.union_set(a, b);
            zero_edges[i] = true;
            zero_edges[i + 1] = true;
        }
        else
        {
            edges_count += 2;
        }
    }
    int vertices_count = ds.count_sets(boost::make_counting_iterator<int>(0),
                                       boost::make_counting_iterator<int>(n));
    // Map from old vertex index to new vertex index.
    std::vector<int> translation(n, -1);
    // Set of sites adjacent to particular vertex.
    std::vector<std::set<site_t const*> > site_sets(vertices_count);
    int last_vertex_index = 0;
    for (int i = 0; i != n; ++i)
    {
        int representative = ds.find_set(i);
        if (translation[representative] == -1)
        {
            translation[representative] = last_vertex_index++;
        }
        int trans = translation[representative];
        translation[i] = trans;
        for (auto& site: vdc.pre_vertices[i])
        {
            site_sets[trans].insert(site);
        }
    }
    assert(last_vertex_index == vertices_count);
    vertices_.reserve(vertices_count);
    for (auto& set: site_sets)
    {
        vertices_.push_back(vertex_t(set.begin(), set.end()));
    }
    edges_.reserve(edges_count);
    // Add all non-zero edges pointing to appropriate vertices.
    for (size_t i = 0; i != vdc_edges_size; i += 2)
    {
        if (!zero_edges[i])
        {
            edges_.push_back(vdc.edges[i].face);
            auto& edge = edges_.back();
            edges_.push_back(vdc.edges[i + 1].face);
            auto& twin_edge = edges_.back();
            edge.twin = &twin_edge;
            twin_edge.twin = &edge;
            auto target = vdc.edges[i].target;
            if (target)
            {
                edge.target = &vertices_[translation[get_index(target)]];
            }
            auto twin_target = vdc.edges[i + 1].target;
            if (twin_target)
            {
                twin_edge.target =
                        &vertices_[translation[get_index(twin_target)]];
            }
        }
    }
}

} // end of namespace geometry
