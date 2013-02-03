#include "geometry.h"

#include <cassert>

#include <algorithm>
#include <array>
#include <set>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    #include <boost/intrusive/set.hpp>
#pragma GCC diagnostic pop

#include <boost/numeric/interval.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace
{
using namespace geometry;
using namespace boost::numeric;
using namespace interval_lib;
typedef voronoi_diagram vd_t;
typedef vd_t::site_t site_t;
typedef vd_t::half_edge_t half_edge_t;

// Defines vertex as center of circumcircle of three points.
struct pre_vertex_t
{
    std::array<point_t, 3> points;

    // x coordinate of event. Also coordinate of rightmost point of
    // enclosing circle.
    interval<double> event_x;

    pre_vertex_t(std::array<point_t, 3> const& points)
        : points(points)
    {
        std::array<interval<double>, 3> px
                = {{points[0].x, points[1].x, points[2].x}};
        std::array<interval<double>, 3> py
                = {{points[0].y, points[1].y, points[2].y}};

        interval<double> x2_m_x1 = px[2] - px[1];
        interval<double> y2_m_y1 = py[2] - py[1];
        interval<double> x3_m_x1 = px[3] - px[1];
        interval<double> y3_m_y1 = py[3] - py[1];

        interval<double> a = x2_m_x1 * y3_m_y1 - x3_m_x1 * y2_m_y1;
        interval<double> x1x1_p_y1y1 = px[1] * px[1] + py[1] * py[1];
        interval<double> x2x2_p_y2y2 = px[2] * px[2] + py[2] * py[2];
        interval<double> x3x3_p_y3y3 = px[3] * px[3] + py[3] * py[3];
        interval<double> x2x2_p_y2y2_m_x1x1_m_y1y1
                = x2x2_p_y2y2 - x1x1_p_y1y1;
        interval<double> x3x3_p_y3y3_m_x1x1_m_y1y1
                = x3x3_p_y3y3 - x1x1_p_y1y1;
        interval<double> bx = -x2x2_p_y2y2_m_x1x1_m_y1y1 * y3_m_y1
                + x3x3_p_y3y3_m_x1x1_m_y1y1 * y2_m_y1;
        interval<double> by = x2x2_p_y2y2_m_x1x1_m_y1y1 * x3_m_x1
                - x3x3_p_y3y3_m_x1x1_m_y1y1 * x3_m_x1;
        interval<double> c = -x1x1_p_y1y1 * (px[2] * py[3] - px[3] * py[2])
                + x2x2_p_y2y2 * (px[1] * py[3] - px[3] * py[1])
                - x3x3_p_y3y3 * (px[1] * py[2] - px[2] * py[1]);
        event_x = (-bx + sqrt(bx * bx + by * by - 4. * a * c)) / (2. * a);
    }
};

typedef boost::variant<site_t, pre_vertex_t> event_t;

// Corresponds to what event_t::which() returns.
enum event_type_t
{
    site_event = 0,
    circle_event = 1
};

struct event_comparator : public boost::static_visitor<bool>
{
    bool operator() (event_t const& a, event_t const& b) const
    {
        return boost::apply_visitor(*this, a, b);
    }

    bool operator()(site_t const& s, pre_vertex_t const& v) const
    {
        using namespace compare::certain;
        if (s.x < v.event_x.lower())
        {
            return true;
        }
        if (s.x > v.event_x.upper())
        {
            return false;
        }
        std::cerr << "Not enough precision";
        // rational arithmetic here
        throw "Not implemented jet";
    }

    bool operator()(pre_vertex_t const& v, site_t const& s) const
    {
        using namespace compare::certain;
        if (v.event_x.upper() < s.x)
        {
            return true;
        }
        if (v.event_x.lower() > s.x)
        {
            return false;
        }
        std::cerr << "Not enough precision";
        // rational arithmetic here
        throw "Not implemented jet";
    }

    bool operator()(pre_vertex_t const& a, pre_vertex_t const& b) const
    {
        using namespace compare::certain;
        if (a.event_x < b.event_x)
        {
            return true;
        }
        if (b.event_x < a.event_x)
        {
            return false;
        }
        std::cerr << "Not enough precision";
        // rational arithmetic here
        throw "Not implemented jet";
    }

    bool operator()(site_t const& a, site_t const& b) const
    {
        return a.x < b.x;
    }
};

typedef std::multiset<event_t, event_comparator> event_queue_t;

struct node_t : public boost::intrusive::set_base_hook<>
{
    site_t site;
    node_t* node_to_left;
    boost::optional<event_queue_t::iterator> event;
    half_edge_t* half_edge;

    node_t(site_t const& site, node_t* node_to_left)
        : site(site)
        , node_to_left(node_to_left)
        , event()
        , half_edge(nullptr)
    {}
};

struct voronoi_diagram_construction
{
    voronoi_diagram_construction(std::vector<site_t> const& sites)
        : events(sites.begin(), sites.end())
    {
        int n = sites.size();
        assert(n >= 3); // Do n < 3 case later.
        // Preallocating space using upper bounds.
        edges.reserve(6 * n - 12);
        pre_vertices.reserve(4 * n - 10);
        node_store.reserve(2 * n - 1);
        assert(edges.capacity() == (size_t)(6 * n - 12));
        assert(pre_vertices.capacity() == (size_t)(4 * n - 10));
        assert(node_store.capacity() == (size_t)(2 * n - 1));
        construct();
        // No memory allocations should have happend during voronoi diagram
        // construction.
        assert(edges.capacity() == (size_t)(6 * n - 12));
        assert(pre_vertices.capacity() == (size_t)(4 * n - 10));
        assert(node_store.capacity() == (size_t)(2 * n - 1));
    }

    void construct()
    {
        auto event = events.begin();
        node_t* prev = nullptr;
        for (auto equal_end = events.upper_bound(*event);
             event != equal_end;
             event = events.erase(event))
        {
            assert(event->which() == site_event);
            site_t site = boost::get<site_t>(*event);
            node_store.push_back(node_t(site, prev));
            auto& new_node = node_store.back();
            if (prev != nullptr)
            {
                edges.push_back(half_edge_t(&new_node.site));
                auto& edge = edges.back();
                new_node.half_edge = &edge;
                edges.push_back(half_edge_t(&prev->site));
                edges.back().twin = &edge;
                edge.twin = &edges.back();
            }
            prev = &node_store.back();
        }
        auto y_comp = [](node_t const& a, node_t const& b) {
            return a.site.y < b.site.y;
        };
        std::sort(node_store.begin(), node_store.end(), y_comp);
        for (auto& node: nodes)
        {
            nodes.push_back(node);
        }

        for (auto end = events.end(); event != end; event = events.erase(event))
        {
            switch (event->which())
            {
            default:
                assert(false);
            case site_event:
                handle_site_event(boost::get<site_t>(*event));
            case circle_event:
                handle_circle_event(boost::get<pre_vertex_t>(*event));
            }
        }
        assert(events.empty());
    }

    void handle_site_event(site_t const& event)
    {
        auto comp = [] (site_t const& key, node_t const& node) {
            throw "Not implemented jet";
            return true;
        };
        auto end_node = nodes.upper_bound(event, comp);
        auto left_node = end_node;
        --left_node;
        if (left_node->event)
        {
            events.erase(left_node->*event);
            left_node->event = boost::none;
        }
        auto middle_node = nodes.insert_before(
                    end_node, node_t(event, &*left_node));
        edges.push_back(half_edge_t(&middle_node->site));
        auto& edge = edges.back();
        middle_node->half_edge = &edge;
        auto right_node = nodes.insert_before(end_node, &*middle_node);
        edges.push_back(half_edge_t(&right_node->site));
        edges.back().twin = edge;
        edge.twin = &edges.back();
        right_node->half_edge = edges.back();
        if (end_node != nodes.end())
        {
            end_node->node_to_left = &*right_node;
        }
        throw "Not implemented jet";
    }

    void handle_circle_event(pre_vertex_t const& event)
    {
        throw "Not implemented jet";
    }

    std::vector<half_edge_t> edges;
    std::vector<pre_vertex_t> pre_vertices;
    event_queue_t events;
    std::vector<node_t> node_store;

    // std::set is not enough because its lack of flexibility of its insert
    // methods. Motivation explained in detail in "advanced_lookups_insertions"
    // section of boost/intrusive documentation.
    // Due to its specific it should be destroyed before node_store.
    boost::intrusive::multiset<node_t> nodes;
};

} // end of anonymous namespace

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

void vd_t::swap(vd_t& o)
{
    edges_.swap(o.edges_);
    sites_.swap(o.sites_);
    vertices_.swap(o.vertices_);
}

void vd_t::process()
{
    voronoi_diagram_construction vdc(sites_);
    throw "Not implemented jet";
}

} // end of namespace geometry
