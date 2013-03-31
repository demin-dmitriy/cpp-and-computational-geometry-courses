#ifndef GEOMETRY_VORONOI_DIAGRAM_H
#define GEOMETRY_VORONOI_DIAGRAM_H

#include <vector>

#include "geometry/point_base.h"

namespace geometry
{

// Every unique edge, site or vertex has unique address. That's it: if any two
// objects in this diagram are equal (but might be recieved using different
// methods) than they have the same address.
struct voronoi_diagram
{
    using site_t = point_t;

    struct vertex_t
    {
        std::vector<site_t const*> sites; // Sites form adjacent faces

        vertex_t() = default;

        template<typename Iterator>
        vertex_t(Iterator first, Iterator last)
            : sites(first, last)
        {}
    };

    template<typename VertexType>
    struct half_edge_base_t
    {
        VertexType const* target;
        site_t const* face;
        half_edge_base_t<VertexType>* twin;

        half_edge_base_t(site_t const* face)
            : target(nullptr)
            , face(face)
            , twin(nullptr)
        {}
    };

    using half_edge_t              = half_edge_base_t<vertex_t>;
    using site_const_iterator      = std::vector<site_t>::const_iterator;
    using vertex_const_iterator    = std::vector<vertex_t>::const_iterator;
    using half_edge_const_iterator = std::vector<half_edge_t>::const_iterator;

    template<typename ForwardIterator>
    voronoi_diagram(ForwardIterator begin, ForwardIterator end)
        : sites_(begin, end)
    {
        process();
    }

    // Half edges are grouped by pairs of twins during iteration. It could be
    // used to efficiently iterate only trough unique edges by skipping every
    // second halfedge.
    half_edge_const_iterator half_edges_begin() const;
    half_edge_const_iterator half_edges_end() const;

    // Sites are ordered in the same order as they were given in the
    // constructor.
    site_const_iterator sites_begin() const;
    site_const_iterator sites_end() const;

    vertex_const_iterator vertices_begin() const;
    vertex_const_iterator vertices_end() const;

    // Invariant: 2 * edges_count() == half_edges_count()
    size_t edges_count() const;
    size_t half_edges_count() const;
    size_t vertices_count() const;
    size_t sites_count() const;

    void swap(voronoi_diagram& o);

private:
    void process();

    std::vector<half_edge_t> edges_;
    std::vector<site_t> sites_;
    std::vector<vertex_t> vertices_;
};

} // end of namespace geometry

#endif // GEOMETRY_VORONOI_DIAGRAM_H
