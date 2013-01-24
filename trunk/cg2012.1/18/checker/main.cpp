#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

struct vertex_t
{
    vector<int> points; // vertex itself is the center of circuscribed circle
                        // of these points.
    int index;
    bool operator<(vertex_t const& o) const
    {
        return lexicographical_compare(
                    points.begin(), points.end(),o.points.begin(),
                    o.points.end());
    }
    bool operator==(vertex_t const& o) const
    {
        return points == o.points;
    }
};
ostream& operator<<(ostream& out, vertex_t const& v)
{
    for (auto p: v.points)
    {
        out << p << ' ';
    }
    return out;
}

struct edge_t
{
    int a, b;
    int s1, s2;
    bool operator<(edge_t const& o) const
    {
        return a < o.a || (a == o.a && (b < o.b || (
                    (b == 0 && o.b == 0)
                    && (s1 < o.s1 || (s1 == o.s1 && s2 < o.s2)))));
    }
    bool operator==(edge_t const& o) const
    {
        return a == o.a && b == o.b && (b != 0 || (s1 == o.s1 && s2 == o.s2));
    }
};

bool loadfromstream(
        istream& in, vector<vertex_t>& vertices, vector<edge_t>& edges)
{
    int n;
    in >> n;
    if (n < 0 || !in)
    {
        return false;
    }

    vertices.resize(n);
    for (int i = 0; i != n; ++i)
    {
        int k;
        in >> k;
        if (k < 3 || !in)
        {
            return false;
        }
        vertices[i].points.reserve(k);
        for (int j = 0; j != k; ++j)
        {
            int v;
            in >> v;
            if (!(1 <= v))
            {
                return false;
            }
            vertices[i].points.push_back(v);
        }
        sort(vertices[i].points.begin(), vertices[i].points.end());
        vertices[i].index = i;
    }
    sort(vertices.begin(), vertices.end());
    vector<int> index_convert(vertices.size());
    for (int i = 0; i != n; ++i)
    {
        index_convert[vertices[i].index] = i;
    }

    int m;
    in >> m;
    if (m < 0 || !in)
    {
        return false;
    }
    edges.resize(m);
    for (int i = 0; i != m; ++i)
    {
        string tmp;
        in >> tmp;
        if (tmp == "infinity")
        {
            int s1, s2;
            in >> tmp >> s1 >> s2;
            if (tmp != "infinity")
            {
                // incorrect format
                cerr << __LINE__;
                return false;
            }
            if (!(1 <= s1 && 1 <= s2))
            {
                cerr << __LINE__;
                return false;
            }
            edges[i].a = 0;
            edges[i].b = 0;
            edges[i].s1 = min(s1, s2);
            edges[i].s2 = max(s1, s2);
        }
        else
        {
            istringstream sst(tmp);
            int a;
            sst >> a;
            if (!(1 <= a && a <= n))
            {
                cerr << __LINE__;
                return false;
            }
            edges[i].a = index_convert[a - 1];

            in >> tmp;
            if (tmp == "infinity")
            {
                int s1, s2;
                in >> s1 >> s2;
                if (!(1 <= s1 && 1 <= s2))
                {
                    cerr << __LINE__;
                    return false;
                }
                edges[i].b = 0;
                edges[i].s1 = s1;
                edges[i].s2 = s2;
            }
            else
            {
                istringstream sst(tmp);
                sst.str(tmp);
                int b;
                sst >> b;
                if (!(1 <= b && b <= n))
                {
                    return false;
                }
                edges[i].b = index_convert[b - 1];
                if (edges[i].a > edges[i].b)
                {
                    swap(edges[i].a, edges[i].b);
                }
            }
        }
    }
    sort(edges.begin(), edges.end());
    return true;
}

int run(int argc, char* argv[])
{
    if (argc != 3)
    {
        cerr << "Usage:\n"
                "  voronoi_diagram_checker file1 file2\n"
                "where file1 and file2 two file names with answers. Checker "
                "will check them for equality and return 0 if they equal and "
                "1 otherwise.\n";
        return 1;
    }
    ifstream file1(argv[1]);
    ifstream file2(argv[2]);
    if (!file1 || !file2)
    {
        cerr << "File not found.\n";
        return 1;
    }

    vector<vertex_t> vertices1;
    vector<edge_t> edges1;
    if (!loadfromstream(file1, vertices1, edges1))
    {
        cerr << "First not loaded\n";
        return 1;
    }
    vector<vertex_t> vertices2;
    vector<edge_t> edges2;
    if (!loadfromstream(file2, vertices2, edges2))
    {
        cerr << "Second not loaded\n";
        return 1;
    }
    if (vertices1 != vertices2 || edges1 != edges2)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    return run(argc, argv);
}
