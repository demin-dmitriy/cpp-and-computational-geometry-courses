// Fortune's algorigthm used.
// todo разбить на файлы

#include "geometry.h"

#include <cassert>
#include <cfloat>
#include <cmath>

#include <algorithm>
#include <array>
#include <iterator>
#include <set>
//#include <unordered_set>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
    #include <boost/intrusive/set.hpp>
#pragma GCC diagnostic pop

#include <boost/iterator/counting_iterator.hpp>
#include <boost/math/special_functions.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/optional.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include <boost/variant.hpp>
#include <gmpxx.h>

#include "robust_arithmetic.h"

namespace
{
using namespace geometry;

using vd_t = voronoi_diagram;
using site_t = vd_t::site_t;
struct circle_event_t;
using event_t = boost::variant<site_t const&, circle_event_t>;
struct event_comparator;
using event_queue_t = std::multiset<event_t, event_comparator>;
using pre_vertex_t = std::array<site_t const*, 3>;
using half_edge_t = vd_t::half_edge_base_t<pre_vertex_t>;
using boost::numeric::interval;
struct node_t;

// std::set is not enough because of it's lack of flexibility of its insert
// methods. Motivation explained in detail in "advanced_lookups_insertions"
// section of boost/intrusive documentation.
using beachline_t = boost::intrusive::multiset<node_t>;

// Corresponds to what event_t::which() returns.
enum event_type_t
{
    site_event = 0,
    circle_event = 1
};

// For some reason boost::numeric::sqrt produces results where interval bounds
// are rounded to nearest double which is incorrect.
// This is most likely compiler issue or compiler's flags' issue.
// Luckily it's easy to write own implementation without significant loss of
// precision.
interval<double> my_sqrt(interval<double> const& i)
{
    assert(i.lower() >= 0);
    using std::max;
    using std::nexttoward;
    return interval<double>(max(0., nexttoward(sqrt(i.lower()), DBL_MIN)),
                            nexttoward(sqrt(i.upper()), DBL_MAX));
}

// The same code for interval and rational arithmetic.
template<typename Type>
void calculate_vertex_parameters(
        pre_vertex_t const& vertex,
        Type& a_out,
        Type& bx_out,
        Type& d_out)
{
    using std::get;
    Type x1 = get<0>(vertex)->x;
    Type x2 = get<1>(vertex)->x;
    Type x3 = get<2>(vertex)->x;
    Type y1 = get<0>(vertex)->y;
    Type y2 = get<1>(vertex)->y;
    Type y3 = get<2>(vertex)->y;

    Type x2_m_x1 = x2 - x1;
    Type y2_m_y1 = y2 - y1;
    Type x3_m_x1 = x3 - x1;
    Type y3_m_y1 = y3 - y1;

    a_out = x2_m_x1 * y3_m_y1 - x3_m_x1 * y2_m_y1;
    Type x1x1_p_y1y1 = x1 * x1 + y1 * y1;
    Type x2x2_p_y2y2 = x2 * x2 + y2 * y2;
    Type x3x3_p_y3y3 = x3 * x3 + y3 * y3;
    Type x2x2_p_y2y2_m_x1x1_m_y1y1 = x2x2_p_y2y2 - x1x1_p_y1y1;
    Type x3x3_p_y3y3_m_x1x1_m_y1y1 = x3x3_p_y3y3 - x1x1_p_y1y1;
    bx_out = -x2x2_p_y2y2_m_x1x1_m_y1y1 * y3_m_y1
            + x3x3_p_y3y3_m_x1x1_m_y1y1 * y2_m_y1;
    Type by = x2x2_p_y2y2_m_x1x1_m_y1y1 * x3_m_x1
            - x3x3_p_y3y3_m_x1x1_m_y1y1 * x2_m_x1;
    Type c = -x1x1_p_y1y1 * (x2 * y3 - x3 * y2)
            + x2x2_p_y2y2 * (x1 * y3 - x3 * y1)
            - x3x3_p_y3y3 * (x1 * y2 - x2 * y1);
    d_out = bx_out * bx_out + by * by - 4. * a_out * c;
}

// Defines vertex as center of circumcircle of three points.
struct circle_event_t
{
    // Requires points in vertex to be in counterclockwise order.
    circle_event_t(pre_vertex_t const& vertex, node_t& node)
        : vertex(vertex)
        , node(&node)
        , event_x()
    {
        assert(1 == left_turn(*get<0>(vertex),
                              *get<1>(vertex),
                              *get<2>(vertex)));
        interval<double> a, bx, d;
        calculate_vertex_parameters(vertex, a, bx, d);
        // Known for sure that a >= 0 because points are in counterclockwise
        // order.
        assert(a.upper() > 0);
        a = abs(a);
        // d >= 0 because it defines radius of some circle.
        assert(d.upper() > 0);
        d = abs(d);
        event_x = (-bx + my_sqrt(d)) / (2. * a);
    }

    pre_vertex_t vertex;

    // Disapearing node in beachline.
    node_t* node;

    // Approximate x coordinate value of event. Also rightmost point coordinate
    // of enclosing circle.
    interval<double> event_x;
};

// todo: Разбить на два класса при рефракторинге
struct event_comparator : public boost::static_visitor<bool>
{
    bool operator() (event_t const& a, event_t const& b) const
    {
        return boost::apply_visitor(*this, a, b);
    }

    int comparator(site_t const& s, circle_event_t const& v) const
    {
        if (s.x < v.event_x.lower())
        {
            return -1;
        }
        if (s.x >= v.event_x.upper())
        {
            return 1;
        }
        //std::cerr << "%";
        mpq_class a, bx, d;
        calculate_vertex_parameters(v.vertex, a, bx, d);
        assert(a > 0);
        mpq_class t = 2 * a * s.x + bx;
        if (t < 0)
        {
            return -1;
        }
        assert(d > 0);
        int cmp_res = boost::math::sign(cmp(t * t, d));
        return cmp_res;
    }

    bool operator()(site_t const& s, circle_event_t const& v) const
    {
        return -1 == comparator(s, v);
    }

    bool operator()(circle_event_t const& v, site_t const& s) const
    {
        return 1 == comparator(s, v);
    }

    bool operator()(circle_event_t const& a, circle_event_t const& b) const
    {
        if (a.event_x.upper() < b.event_x.lower())
        {
            return true;
        }
        if (b.event_x.upper() <= a.event_x.lower())
        {
            return false;
        }
        //std::cerr << '$';
        mpq_class a1, bx1, d1;
        calculate_vertex_parameters(a.vertex, a1, bx1, d1);
        assert(a1 > 0);
        mpq_class a2, bx2, d2;
        calculate_vertex_parameters(b.vertex, a2, bx2, d2);
        assert(a2 > 0);
        mpq_class a1b2_m_a2b1 = a1 * bx2 - a2 * bx1;
        assert(d1 > 0);
        assert(d2 > 0);
        mpq_class a1a1d2 = a1 * a1 * d2;
        mpq_class a2a2d1 = a2 * a2 * d1;
        bool reversed = (a1b2_m_a2b1 < 0);
        if (reversed ^ (a1a1d2 < a2a2d1))
        {
            return reversed;
        }
        mpq_class t1 = a1a1d2 + a2a2d1 - a1b2_m_a2b1 * a1b2_m_a2b1;
        if (t1 < 0)
        {
            return reversed;
        }
        int sign = cmp(4 * a1a1d2 * a2a2d1, t1 * t1);
        if (!reversed)
        {
            return sign < 0;
        }
        else
        {
            return sign > 0;
        }
    }

    bool operator()(site_t const& a, site_t const& b) const
    {
        return a.x < b.x;
    }
};

struct node_t : public boost::intrusive::set_base_hook<>
{
    site_t const* site;
    // todo Переименовать
    node_t* node_to_left;
    boost::optional<event_queue_t::iterator> event;
    half_edge_t* half_edge;

    node_t(site_t const& site, node_t* node_to_left = nullptr)
        : site(&site)
        , node_to_left(node_to_left)
        , event()
        , half_edge(nullptr)
    {}
};

struct site_to_node_comp
{
    bool operator()(site_t const& key, node_t const& node) const
    {
        if (node.node_to_left == nullptr) // Then it is the leftmost arc.
        {
            return false;
        }
        return impl(key, *node.node_to_left->site, *node.site)();
    }

    struct impl
    {
        site_t const& key;
        site_t const& l;
        site_t const& r;
        impl(site_t const& key, site_t const& l, site_t const& r)
            : key(key)
            , l(l)
            , r(r)
        {}

        // todo: разбить на несколько функций
        bool operator()()
        {
            using std::abs;
            assert(!(l.x == key.x && r.x == key.x));
            if (l.x == key.x)
            {
                return key.y < l.y;
            }
            if (r.x == key.x)
            {
                return key.y < r.y;
            }
            if (l.x == r.x)
            {
                double res = l.y + r.y - 2 * key.y;
                if (abs(res) > 2 * DBL_EPSILON * (abs(l.y) + abs(r.y)
                                                  + 2 * abs(key.y)))
                {
                    return res > 0;
                }
                using namespace adaptive_arithmetic;
                double result[3] = {l.y, r.y, -2 * key.y};
                expansion_sum<1, 1>(result, result + 1);
                expansion_sum<2, 1>(result, result + 2);
                return sign<3>(result) == 1;;
            }

            double a = l.x - r.x;
            double b = -r.y * (l.x - key.x) + l.y * (r.x - key.x);
            double abs_b = abs(r.y * (l.x - key.x)) + abs(l.y * (r.x + key.x));
            double mid_res = a * key.y + b;
            if (abs(mid_res) >= 4 * DBL_EPSILON * (abs(key.y * a) + abs_b))
            {
                if (mid_res < 0)
                {
                    return l.x > r.x;
                }
            }
            else
            {
                using namespace adaptive_arithmetic;
                double result[12];

                two_product(l.x, key.y, result[0], result[1]);
                two_product(-r.x, key.y, result[2], result[3]);
                two_product(-r.y, l.x, result[4], result[5]);
                two_product(r.y, key.x, result[6], result[7]);
                two_product(l.y, r.x, result[8], result[9]);
                two_product(l.y, -key.x, result[10], result[11]);

                expansion_sum<2, 2>(result, result + 2);
                expansion_sum<2, 2>(result + 4, result + 6);
                expansion_sum<2, 2>(result + 8, result + 10);

                expansion_sum<4, 4>(result, result + 4);
                expansion_sum<8, 4>(result, result + 8);

                int res_sign = sign<12>(result);
                if (res_sign == -1)
                {
                    return l.x > r.x;
                }
            }

            double c = (r.y * r.y + r.x * r.x - key.x * key.x) * (l.x - key.x)
                    - (l.y * l.y + l.x * l.x - key.x * key.x) * (r.x - key.x);
            double abs_c = (abs(r.y * r.y) + abs(r.x * r.x)
                            + abs(key.x * key.x)) * abs(l.x - key.x)
                    + (abs(l.y * l.y) + abs(l.x * l.x)
                       + abs(key.x * key.x)) * abs(r.x - key.x);
            double end_res = c + key.y * (a * key.y + 2 * b);
            double end_roundoff = abs_c + abs(key.y) * (abs(a * key.y)
                                                        + 2 * abs_b);
            if (abs(end_res) >= 4 * DBL_EPSILON * end_roundoff)
            {
                return end_res < 0;
            }
            // Interval arithmetic here.

            mpq_class rx(r.x), ry(r.y), lx(l.x), ly(l.y);
            mpq_class key_x(key.x), key_y(key.y);

            int res_sign = sgn(
                (ry * ry + rx * rx - key_x * key_x) * (lx - key_x)
                 - (ly * ly + lx * lx - key_x * key_x) * (rx - key_x)
                 + key_y * ((lx - rx) * key_y
                            + 2 * (-ry * (lx - key_x) + ly * (rx - key_x))));

            return res_sign == -1;
        }
    };
};

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
            node.node_to_left = prev;
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

        for (auto end = events.end(); event != end; event = events.erase(event))
        {
            switch (event->which())
            {
            default:
                assert(false);
            case site_event:
                handle_site_event(boost::get<site_t>(*event));
                break;
            case circle_event:
                handle_circle_event(boost::get<circle_event_t>(*event));
                break;
            }
            // multiset is guaranteed to keep stable sorting order by c++11
            // standart. Therefore events equal to current event will be
            // inserted after it.
            assert(event == events.begin());
        }
        assert(events.empty());
    }

    void create_edge(node_t& node)
    {
        assert(node.node_to_left);
        node_t& left_node = *node.node_to_left;
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
        if (1 == left_turn(*right_site, *middle_site, *left_site))
        {
            auto event = events.insert(
                circle_event_t({{right_site, middle_site, left_site}}, middle));
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
            node_it->node_to_left = &*new_node;
        }
        return new_node;
    }

    void handle_site_event(site_t const& event)
    {   
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
            check_for_circle_event(
                        *left_node->node_to_left, *left_node, *middle_node);
        }
    }

    void handle_circle_event(circle_event_t const& event)
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
        right_node->node_to_left = &*left_node;
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
            check_for_circle_event(*std::prev(left_node), *left_node, *right_node);
        }
    }

    std::vector<half_edge_t> edges;
    std::vector<pre_vertex_t> pre_vertices;
    event_queue_t events;
    std::vector<node_t> node_store;
    beachline_t nodes;
};

bool coincide(pre_vertex_t const& a, pre_vertex_t const& b)
{
    for (size_t i = 0; i != b.size(); ++i)
    {
        if (on_border != in_circle(*a[0], *a[1], *a[2], *b[i]))
        {
            return false;
        }
    }
    return true;
}

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

// todo: улучшить?
void vd_t::process()
{
    voronoi_diagram_construction vdc(sites_);
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
    typedef char mybool;
    size_t vdc_edges_size = vdc.edges.size();
    assert(vdc_edges_size % 2 == 0);
    std::vector<mybool> zero_edges(vdc_edges_size, false);
    int edges_count = 0;
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
    std::vector<int> translation(n, -1);
    std::vector<std::set<site_t const*> > site_sets(vertices_count);
    int last_vertex_index = 0;
    for (int i = 0; i != n; ++i)
    {
        int repres = ds.find_set(i);
        if (translation[repres] == -1)
        {
            translation[repres] = last_vertex_index++;
        }
        int trans = translation[repres];
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
