#include <vector>

namespace geometry
{

struct voronoi_diagram
{
    typedef point_t site_t;

    struct vertex_t
    {
        std::vector<site_t const*> sites; // Sites form adjacent faces

        vertex_t(){}

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

    typedef half_edge_base_t<vertex_t> half_edge_t;
    typedef std::vector<site_t>::const_iterator site_const_iterator;
    typedef std::vector<vertex_t>::const_iterator vertex_const_iterator;
    typedef std::vector<half_edge_t>::const_iterator half_edge_const_iterator;

    template<typename ForwardIterator>
    voronoi_diagram(ForwardIterator begin, ForwardIterator end)
        : sites_(begin, end)
    {
        process();
    }

    // Half edges are grouped by pairs of twins during iteration.
    half_edge_const_iterator half_edges_begin() const;
    half_edge_const_iterator half_edges_end() const;

    site_const_iterator sites_begin() const;
    site_const_iterator sites_end() const;

    vertex_const_iterator vertices_begin() const;
    vertex_const_iterator vertices_end() const;

    // 2 * edges_count() == half_edges_count()
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
