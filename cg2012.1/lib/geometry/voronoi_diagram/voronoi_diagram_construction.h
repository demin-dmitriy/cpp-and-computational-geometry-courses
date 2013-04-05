#ifndef GEOMETRY_VORONOI_DIAGRAM_VORONOI_DIAGRAM_CONSTRUCTION_H
#define GEOMETRY_VORONOI_DIAGRAM_VORONOI_DIAGRAM_CONSTRUCTION_H

#include <cassert>

#include <algorithm>
#include <functional>

#include "geometry/geometry_fwd.h"
#include "geometry/voronoi_diagram/structures.h"

namespace geometry
{
namespace detail
{

// Constructs voronoi diagram using Fortune's algorigthm but doesn't removes
// degeneracies so there exist zero-length edges. Also every vertex defined as
// circumcenter of exactly three sites. Here they are also called pre-vertices.
struct voronoi_diagram_construction
{
    voronoi_diagram_construction(std::vector<site_t> const& sites)
        : edges()
        , pre_vertices()
        , events(sites.begin(), sites.end())
        , node_store()
        , nodes()
    {
        int n = std::max((size_t)3, sites.size());
        // Preallocating space using upper bounds.
        edges.reserve(6 * n - 12);
        pre_vertices.reserve(4 * n - 10);
        node_store.reserve(2 * n - 1);
        construct();
        // No memory allocations should have happend during voronoi diagram
        // construction.
        assert(edges.size() <= (size_t)(6 * n - 12));
        assert(pre_vertices.size() <= (size_t)(4 * n - 10));
        assert(node_store.size() <= (size_t)(2 * n - 1));
    }

    void construct()
    {
        auto event = events.begin();
        for (auto equal_end = events.upper_bound(*event);
             event != equal_end;
             event = events.erase(event))
        {
            node_store.push_back(node_t(boost::get<site_t>(*event)));
        }
        auto y_comp = [](node_t const& a, node_t const& b) {
            return a.site->y < b.site->y;
        };
        std::sort(node_store.begin(), node_store.end(), y_comp);

        node_t* prev = nullptr;
        for (auto& node: node_store)
        {
            node.node_below = prev;
            if (prev != nullptr)
            {
                create_edge(node);
            }
            prev = &node;
        }

        for (auto& node: node_store)
        {
            nodes.push_back(node);
        }

        event_handler_t event_handler(this);
        for (auto end = events.end(); event != end; event = events.erase(event))
        {
            boost::apply_visitor(event_handler, *event);

            // multiset is guaranteed to keep stable sorting order since c++11
            // standart. Therefore events with same as current event
            // x-coordingate will be inserted after it.
            assert(event == events.begin());
        }
        assert(events.empty());
    }

    struct event_handler_t : boost::static_visitor<void>
    {
        event_handler_t(voronoi_diagram_construction* target)
            : target(target)
        {}

        template<typename EventType>
        void operator()(EventType const& event)
        {
            target->handle_event(event);
        }

        voronoi_diagram_construction* const target;
    };

    void create_edge(node_t& node)
    {
        assert(node.node_below);
        node_t& left_node = *node.node_below;
        edges.push_back(half_edge_t(node.site));
        auto& edge = edges.back();
        node.half_edge = &edge;
        edges.push_back(half_edge_t(left_node.site));
        auto& twin_edge = edges.back();
        edge.twin = &twin_edge;
        twin_edge.twin = &edge;
    }

    void check_for_circle_event(node_t& left, node_t& middle, node_t& right)
    {
        auto right_site = right.site;
        auto middle_site = middle.site;
        auto left_site = left.site;
        // If nodes are ordered counter-clockwise then their bisectors will
        // converge.
        if (geometry::left == turn(*right_site, *middle_site, *left_site))
        {
            auto event = events.insert(circle_event_t(
                    {{right_site, middle_site, left_site}}, middle));
            middle.event = event;
        }
    }

    void clear_event(node_t& node)
    {
        if (node.event)
        {
            events.erase(*node.event);
            node.event = boost::none;
        }
    }

    beachline_t::iterator insert_node_before(
            beachline_t::iterator node_it, site_t const& site)
    {
        node_store.push_back(node_t(site, &*std::prev(node_it)));
        auto new_node = nodes.insert_before(node_it, node_store.back());
        if (node_it != nodes.end())
        {
            node_it->node_below = &*new_node;
        }
        return new_node;
    }

    // Handle site event.
    void handle_event(site_t const& event)
    {
        // Find the first node 'higher' than event point.
        auto end_node = nodes.upper_bound(event, site_to_node_comp());
        auto left_node = std::prev(end_node);
        clear_event(*left_node);
        auto middle_node = insert_node_before(end_node, event);
        auto right_node = insert_node_before(end_node, *left_node->site);
        create_edge(*right_node);
        middle_node->half_edge = right_node->half_edge->twin;

        // Check for new circle events.
        if (end_node != nodes.end())
        {
            check_for_circle_event(*middle_node, *right_node, *end_node);
        }
        if (left_node != nodes.begin())
        {
            check_for_circle_event(*left_node->node_below,
                                   *left_node,
                                   *middle_node);
        }
    }

    // Handle circle event.
    void handle_event(circle_event_t const& event)
    {
        auto node = nodes.iterator_to(*event.node);
        auto left_node = std::prev(node);
        auto right_node = std::next(node);

        // Remove circle events(if there any) from neighbour nodes.
        clear_event(*left_node);
        clear_event(*right_node);

        // Create new vertex.
        pre_vertices.push_back(pre_vertex_t(event.vertex));
        auto& vertex = pre_vertices.back();
        right_node->half_edge->target = &vertex;
        node->half_edge->target = &vertex;

        // Remove disappearing node.
        right_node->node_below = &*left_node;
        create_edge(*right_node);
        right_node->half_edge->twin->target = &vertex;
        nodes.erase(node);

        // Check for new circle events.
        auto right_right_node = std::next(right_node);
        if (right_right_node != nodes.end())
        {
            check_for_circle_event(*left_node, *right_node, *right_right_node);
        }
        if (left_node != nodes.begin())
        {
            check_for_circle_event(*std::prev(left_node),
                                   *left_node,
                                   *right_node);
        }
    }

    std::vector<half_edge_t> edges;
    std::vector<pre_vertex_t> pre_vertices;
    event_queue_t events;
    std::vector<node_t> node_store;
    beachline_t nodes;
};

} // end of namespace detail
} // end of namespace geometry

#endif // GEOMETRY_VORONOI_DIAGRAM_VORONOI_DIAGRAM_CONSTRUCTION_H
