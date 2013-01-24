#include <algorithm>
#include <iostream>
#include <iterator>
#include <unordered_map>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT> AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT,AT,AP> VD;

typedef AT::Site_2 Site_2;
typedef AT::Point_2 Point_2;

struct site_hash
{
    std::hash<double> hash;
    size_t operator()(Site_2 const& s) const
    {
        return hash(s.x()) + hash(s.y());
    }
};

struct vertex_handle_hash
{
    site_hash hash;
    size_t operator()(VD::Vertex_handle const& vh) const
    {
        auto e = vh->dual();
        return hash(e->vertex(0)->point())
                + hash(e->vertex(1)->point())
                + hash(e->vertex(2)->point());
    }
};

int run(std::istream& in, std::ostream& out)
{

    int n;
    in >> n;
    VD vd;
    std::unordered_map<Site_2, int, site_hash> s_index;
    for (int i = 0; i != n; ++i)
    {
        Site_2 site;
        in >> site;
        s_index[site] = i + 1;
        vd.insert(site);
    }
    out << vd.number_of_vertices() << '\n';
    std::unordered_map<VD::Vertex_handle, int, vertex_handle_hash> v_index;
    int i = 0;
    for (auto v = vd.vertices_begin(), end = vd.vertices_end(); v != end; ++v)
    {
        v_index[v] = ++i;
        out << v->degree();
        auto e_begin = v->incident_halfedges();
        auto e = e_begin;
        do
        {
            out << ' ' << s_index[e->face()->dual()->point()];
            ++e;
        } while (e != e_begin);
        out << '\n';
    }
    out << vd.number_of_halfedges() / 2 << '\n';
    for (auto e = vd.edges_begin(), end = vd.edges_end(); e != end; ++e)
    {
        int s = 0;
        if (e->has_source())
        {
            s = v_index[e->source()];
        }
        int t = 0;
        if (e->has_target())
        {
            t = v_index[e->target()];
        }
        int a = 0, b = 0;
        if (s == 0 && t != 0)
        {
            std::swap(s, t);
            a = s_index[e->twin()->face()->dual()->point()];
            b = s_index[e->face()->dual()->point()];
        }
        else if (t == 0)
        {
            a = s_index[e->face()->dual()->point()];
            b = s_index[e->twin()->face()->dual()->point()];
        }
        if (s == 0)
        {
            out << "infinity ";
        }
        else
        {
            out << s << ' ';
        }
        if (t == 0)
        {
            out << "infinity " << a << ' ' << b;
        }
        else
        {
            out << t;
        }
        out << '\n';

    }
    return 0;
}

int main()
{
    return run(std::cin, std::cout);
}
