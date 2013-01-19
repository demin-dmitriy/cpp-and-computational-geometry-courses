namespace geometry
{

struct circle_t
{
    point points[3];
    int points_used;

    circle_t(point a, point b, point c)
        : points_used(3)
    {
        points[0] = a;
        if (left_turn(a, b, c) == -1)
        {
            points[1] = c;
            points[2] = b;
        }
        else
        {
            points[1] = b;
            points[2] = c;
        }
    }

    circle_t(point a, point b)
        : points({a, b})
        , points_used(2)
    {}

    circle_t(point a)
        : points({a})
        , points_used(1)
    {
        points[0] = a;
    }

    circle_t()
        : points({point(0, 0)})
        , points_used(0)
    {}

    void add_point(point const& d)
    {
        //assert(points_used < 3);
        if (points_used == 2 && left_turn(points[0], points[1], d) == -1)
        {
            points[2] = points[1];
            points[1] = d;
        }
        else
        {
            points[points_used] = d;
        }
        ++points_used;
    }

    bool contains(point x)
    {
        switch (points_used)
        {
        default:
        case 0:
            return false;
        case 1:
            return x == points[0];
        case 2:
            return in_circle(points[0], points[1], x) != outside;
        case 3:
            return in_circle(points[0], points[1], points[2], x) != outside;
        }
    }
};

} // end of namespace geometry
