// Testgen for generating set of points.
//
// To simplify things put all in one file.

#include <cassert>
#include <cmath>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

constexpr double pi = 4 * atan(1.);
constexpr int default_precision = 50;
constexpr double epsilon = std::numeric_limits<double>::epsilon() / 2;

struct point_t
{
    double x;
    double y;
    point_t() = default;
    point_t(double x, double y)
        : x(x)
        , y(y)
    {}
    void set(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
    bool operator==(point_t const& o) const
    {
        return x == o.x && y == o.y;
    }
};

std::ostream& operator<<(std::ostream& stream, point_t const& p)
{
    stream << p.x << ' ' << p.y;
    return stream;
}

struct interval_t
{
    double lower;
    double upper;
    interval_t(double lower, double upper)
        : lower(lower)
        , upper(upper)
    {}
    interval_t(double single)
        : lower(single)
        , upper(single)
    {}
};

struct area_t
{
    interval_t x;
    interval_t y;
    area_t(interval_t x, interval_t y)
        : x(x)
        , y(y)
    {}
};

struct test_t
{
    void add(point_t const& p)
    {
        data.push_back(p);
    }

    typedef std::vector<point_t> data_t;
    typedef data_t::iterator iterator;

    iterator begin()
    {
        return data.begin();
    }

    iterator end()
    {
        return data.end();
    }

    data_t data;
};

std::ostream& operator<<(std::ostream& out, test_t const& test)
{
    auto exceptions = out.exceptions();
    out.exceptions(std::ios_base::failbit | std::ios_base::badbit);
    out << test.data.size() << '\n';
    std::copy(test.data.begin(),
              test.data.end(),
              std::ostream_iterator<point_t>(out, "\n"));
    out.exceptions(exceptions);
    return out;
}

template<typename Generator>
double random_double(interval_t interval, Generator& rd)
{
    std::uniform_real_distribution<> rnd(interval.lower, interval.upper);
    return rnd(rd);
}

template<typename Generator>
point_t random_point(area_t area, Generator& rd)
{
    point_t point;
    point.x = random_double(area.x, rd);
    point.y = random_double(area.y, rd);
    return point;
}

template<typename Generator>
test_t random_test(area_t area, int n, Generator& rd)
{
    assert(n >= 0);
    test_t test;
    for (int i = 0; i != n; ++i)
    {
        test.add(random_point(area, rd));
    }
    return test;
}

point_t& rotate(double angle, point_t& p)
{
    double x = p.x * std::cos(angle) - p.y * std::sin(angle);
    double y = p.x * std::sin(angle) + p.y * std::cos(angle);
    p.set(x, y);
    return p;
}

// Rotates every point in test by specified angle.
void rotate(double angle, test_t& test)
{
    for (auto& point: test)
    {
        rotate(angle, point);
    }
}

// All points uniformly lying on circle: special case for some algorithms.
// (Not really on circle because of roundoff error but very close).
test_t circle_test(double radius, int n)
{
    test_t test;
    for (int i = 0; i != n; ++i)
    {
        point_t p(radius, 0);
        double angle = 2 * pi * i / n;
        test.add(rotate(angle, p));
    }
    return test;
}

// All points randomly distributed on circle
template<typename Generator>
test_t random_circle_test(double radius, int n, Generator& rd)
{
    std::uniform_real_distribution<> rnd_angle(0, 2 * pi);
    test_t test;
    for (int i = 0; i != n; ++i)
    {
        point_t p(radius, 0);
        test.add(rotate(rnd_angle(rd), p));
    }
    return test;
}

template<typename T>
constexpr T sqr(T value)
{
    return value * value;
}

template<int N>
constexpr bool check_pythagorean_triples(int const (&array)[N][2],
                                         int index = 0)
{
    return (N == index)
           ? true
           : sqr(array[index][0]) + sqr(array[index][1])
                    == sqr(array[0][0]) + sqr(array[0][1])
            && check_pythagorean_triples(array, index + 1);
}

// This uses pythagorean triples
test_t perfect_circle_test()
{
    static constexpr int pythagorean_numbers[][2] = { {1264, 6177}
                                                    , {1887, 6016}
                                                    , {2943, 5576}
                                                    , {3496, 5247}
                                                    };
    static_assert(check_pythagorean_triples(pythagorean_numbers),
                  "Numbers are not pythagorean.");
    test_t test;
    for (auto& p: pythagorean_numbers)
    {
        test.add(point_t(p[0], p[1]));
        test.add(point_t(p[1], p[0]));
        test.add(point_t(-p[0], p[1]));
        test.add(point_t(p[1], -p[0]));
        test.add(point_t(p[0], -p[1]));
        test.add(point_t(-p[1], p[0]));
        test.add(point_t(-p[0], -p[1]));
        test.add(point_t(-p[1], -p[0]));
    }
    int hypotenuse_square = sqr(pythagorean_numbers[0][0])
                            + sqr(pythagorean_numbers[0][1]);
    int hypotenuse = static_cast<int>(sqrt(hypotenuse_square));
    if (hypotenuse * hypotenuse == hypotenuse_square)
    {
        test.add(point_t(hypotenuse, 0));
        test.add(point_t(0, hypotenuse));
        test.add(point_t(-hypotenuse, 0));
        test.add(point_t(0, -hypotenuse));
    }
    return test;
}

template<typename Generator>
void distort(double deviation, point_t& point, Generator& rd)
{
    // Unit in first place
    auto ufp = [](double x)
    {
        int exp;
        std::frexp(x, &exp);
        return ldexp(1., exp);
    };
    std::uniform_real_distribution<> rnd_deviation(-deviation, deviation);
    point.x += ufp(point.x) * rnd_deviation(rd);
    point.y += ufp(point.y) * rnd_deviation(rd);
}

// Makes small changes in points' coordinates.
template<typename Generator>
void distort(double deviation, test_t& test, Generator& rd)
{
    for (auto& point: test)
    {
        distort(deviation, point, rd);
    }
}

// All points on one line.
template<typename Generator>
test_t line_test(area_t area, int n, Generator& rd)
{
    point_t begin = random_point(area, rd);
    point_t end = random_point(area, rd);
    test_t test;
    test.add(begin);
    test.add(end);
    std::uniform_real_distribution<> rnd_param(0, 1);
    for (int i = 0; i != n - 2; ++i)
    {
        point_t p;
        double t = rnd_param(rd);
        p.x = t * begin.x + (1 - t) * end.x;
        p.y = t * begin.y + (1 - t) * end.y;
        test.add(p);
    }
    return test;
}

point_t& move(point_t& p, point_t direction)
{
    p.x += direction.x;
    p.y += direction.y;
    return p;
}

void move_points(test_t& test, point_t direction)
{
    for (auto& point: test)
    {
        move(point, direction);
    }
}

// Copies every point in test, rotates copies and moves them by vector.
void copy_and_move(double angle, point_t direction, test_t& test)
{
    test.data.reserve(2 * test.data.size());
    for (auto point: test)
    {
        rotate(angle, point);
        test.add(move(point, direction));
    }
}

// All points lying in nodes of restangular grid.
template<typename Generator>
test_t grid_test(area_t area,
                 interval_t x_interval,
                 interval_t y_interval,
                 int n,
                 int m,
                 Generator& rd)
{
    test_t test;
    double x_step = random_double(x_interval, rd);
    double y_step = random_double(y_interval, rd);
    point_t first_in_row = random_point(area, rd);
    for (int i = 0; i != m; ++i)
    {
        point_t current = first_in_row;
        for (int j = 0; j != n; ++j)
        {
            test.add(current);
            current.x += x_step;
        }
        first_in_row.y += y_step;
    }
    return test;
}

template<typename Generator>
void shuffle_points(test_t& test, Generator& rd)
{
    std::shuffle(test.begin(), test.end(), rd);
}

// Points lie on a spiral (Archimedean)
template<typename Generator>
test_t spiral_test(area_t area, interval_t step, int n, Generator& rd)
{
    test_t test;
    double param = 0.;
    point_t center = random_point(area, rd);
    for (int i = 0; i != n; ++i)
    {
        param += random_double(step, rd);
        point_t point = center;
        const double magic = 0.125;
        point.x += magic * param;
        rotate(param, point);
        test.add(point);
    }
    return test;
}

void remove_duplicates(test_t& test)
{
    struct point_hash
    {
        std::hash<double> hash;
        size_t operator()(point_t const& point) const
        {
            return hash(point.x) + hash(point.y);
        }
    };
    test_t::data_t new_data;
    std::unordered_set<point_t, point_hash> data_elements;
    for (auto point: test.data)
    {
        bool exist;
        std::tie(std::ignore, exist) = data_elements.insert(point);
        if (!exist)
        {
            new_data.push_back(point);
        }
    }
    test.data.swap(new_data);
}

// All points coincide
template<typename Generator>
test_t single_point_test(area_t area, int n, Generator& rd)
{
    point_t point = random_point(area, rd);
    test_t test;
    for (int i = 0; i != n; ++i)
    {
        test.add(point);
    }
    return test;
}

void merge(test_t& to, test_t const& from)
{
    to.data.insert(to.data.end(), from.data.begin(), from.data.end());
}

// std::to_string doesn't work in gcc yet.
template<typename T>
std::string to_string(T value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

struct output_t
{
    output_t(std::string directory)
        : directory(directory)
        , test_count(0)
    {}

    void next(test_t const& test)
    {
        std::ofstream file(directory + '/' + to_string(test_count++) + ".in");
        if (!file)
        {
            std::cerr << "Can't create file. (Maybe output directory '"
                      << directory << "' doesn't exist?)";
            throw std::ios_base::failure("Can't create file.");
        }
        file.precision(default_precision);
        file << test;
    }

    std::string directory;
    int test_count;
};

int run()
{
    output_t output("tests");
    std::mt19937_64 rd;
    area_t small_area(interval_t(-1, 1),
                      interval_t(-1, 1));
    area_t medium_area(interval_t(-100, +100),
                       interval_t(-100, +100));
    area_t large_area(interval_t(-pow(2, 100), pow(2, 100)),
                      interval_t(-pow(2, 100), pow(2, 100)));
    // These sometimes might be special case for algorithm.
    output.next(random_test(medium_area, 0, rd)); // empty test;
    output.next(random_test(medium_area, 1, rd));
    output.next(random_test(medium_area, 2, rd));
    output.next(random_test(medium_area, 3, rd));
    output.next(random_test(medium_area, 4, rd));
    std::uniform_int_distribution<> small_test_size(10, 500);
    std::uniform_int_distribution<> medium_test_size(50, 5000);
    std::uniform_int_distribution<> large_test_size(4000, 40000);
    // Simple random tests.
    int const random_test_count = 5;
    for (int i = 0; i != random_test_count; ++i)
    {
        output.next(random_test(medium_area, medium_test_size(rd), rd));
    }
    output.next(circle_test(10, 4));
    output.next(circle_test(15, 8));
    // All points on circle
    int const random_circle_test_count = 3;
    for (int i = 0; i != random_circle_test_count; ++i)
    {
        double radius = random_double(interval_t(50, 500), rd);
        output.next(random_circle_test(radius, medium_test_size(rd), rd));
    }
    // Small radius circle
    double small_radius = 5 * epsilon + 375 * epsilon * epsilon;
    output.next(random_circle_test(small_radius, small_test_size(rd), rd));
    output.next(random_circle_test(13 * small_radius, small_test_size(rd), rd));

    output.next(perfect_circle_test());

    // All points in one place
    output.next(single_point_test(medium_area, small_test_size(rd), rd));

    // Line tests
    int const line_test_count = 5;
    for (int i = 0; i != line_test_count; ++ i)
    {
        output.next(line_test(medium_area, medium_test_size(rd), rd));
    }
    // Horizontal line test
    area_t horizontal_area(
                interval_t(-100, 100),
                interval_t(random_double(interval_t(-100, 100), rd)));
    output.next(line_test(horizontal_area, medium_test_size(rd), rd));
    // Vertical line test
    area_t vertical_area(interval_t(random_double(interval_t(-100, 100), rd)),
                         interval_t(-100, 100));
    output.next(line_test(vertical_area, medium_test_size(rd), rd));
    {
        test_t two_lines_test = line_test(vertical_area,
                                          medium_test_size(rd),
                                          rd);
        copy_and_move(0, random_point(medium_area, rd), two_lines_test);
        output.next(two_lines_test);
        test_t two_lines_small_move_test = line_test(horizontal_area,
                                                     medium_test_size(rd),
                                                     rd);
        copy_and_move(0,
                      random_point(small_area, rd),
                      two_lines_small_move_test);
        output.next(two_lines_small_move_test);
        test_t two_lines_turn_test = line_test(vertical_area,
                                               medium_test_size(rd),
                                               rd);
        copy_and_move(80 * epsilon,
                      random_point(small_area, rd),
                      two_lines_turn_test);
        output.next(two_lines_turn_test);
        copy_and_move(0, random_point(medium_area, rd), two_lines_turn_test);
        output.next(two_lines_turn_test);
    }
    // Grid tests
    // These tests might be intresting for sweep line algorithms
    std::uniform_int_distribution<> grid_size(1, 100);
    int const grid_test_count = 3;
    for (int i = 0; i != grid_test_count; ++i)
    {
        int n = grid_size(rd);
        int m = grid_size(rd);
        test_t test = grid_test(medium_area,
                                interval_t(1, 100),
                                interval_t(1, 100),
                                n,
                                m,
                                rd);
        shuffle_points(test, rd);
        output.next(test);
    }
    int const rotated_grid_test_count = 2;
    for (int i = 0; i != rotated_grid_test_count; ++i)
    {
        int n = grid_size(rd);
        int m = grid_size(rd);
        test_t test = grid_test(medium_area,
                                interval_t(1, 100),
                                interval_t(1, 100),
                                n,
                                m,
                                rd);
        shuffle_points(test, rd);
        rotate(random_double(interval_t(0, 2 * pi), rd), test);
        output.next(test);
    }
    // Spiral tests
    // This is just for fun
    int const spiral_test_count = 3;
    for (int i = 0; i != spiral_test_count; ++i)
    {
        output.next(spiral_test(medium_area,
                                interval_t(epsilon, 100),
                                medium_test_size(rd),
                                rd));
    }

    // Duplicate and distort
    int const random_duplicate_distort_test_count = 5;
    for (int i = 0; i != random_duplicate_distort_test_count; ++i)
    {
        test_t test = random_test(medium_area, medium_test_size(rd), rd);
        copy_and_move(0, point_t(0, 0), test); // Don't move, just copy
        distort(128 * epsilon, test, rd);
        output.next(test);
    }

    // Same with some circles
    int const random_duplicate_distort_circle_test_count = 3;
    for (int i = 0; i!= random_duplicate_distort_circle_test_count; ++i)
    {
        double radius = random_double(interval_t(50, 500), rd);
        test_t test = random_circle_test(radius, medium_test_size(rd), rd);
        copy_and_move(0, point_t(0, 0), test);
        distort(128 * epsilon, test, rd);
        output.next(test);
    }

    // Same with lines
    int const line_distorted_test_count = 5;
    for (int i = 0; i != line_distorted_test_count; ++ i)
    {
        test_t test = line_test(medium_area, medium_test_size(rd), rd);
        copy_and_move(0, point_t(0, 0), test);
        distort(128 * epsilon, test, rd);
        output.next(test);
    }

    {
        test_t horizontal_distorted_test = line_test(
                    horizontal_area, medium_test_size(rd), rd);
        copy_and_move(0, point_t(0, 0), horizontal_distorted_test);
        distort(128 * epsilon, horizontal_distorted_test, rd);
        output.next(horizontal_distorted_test);
        test_t vertical_distorted_test = line_test(
                    vertical_area, medium_test_size(rd), rd);
        copy_and_move(0, point_t(0, 0), vertical_distorted_test);
        distort(128 * epsilon, vertical_distorted_test, rd);
        output.next(vertical_distorted_test);
    }

    // Same with grid
    int const distorted_grid_test_count = 3;
    for (int i = 0; i != distorted_grid_test_count; ++i)
    {
        int n = grid_size(rd);
        int m = grid_size(rd);
        test_t test = grid_test(medium_area,
                                interval_t(1, 100),
                                interval_t(1, 100),
                                n,
                                m,
                                rd);
        copy_and_move(0, point_t(0, 0), test);
        distort(128 * epsilon, test, rd);
        shuffle_points(test, rd);
        output.next(test);
    }

    // Many points close to eachother
    {
        test_t test = single_point_test(area_t(interval_t(1),
                                               interval_t(1)),
                                        small_test_size(rd),
                                        rd);
        distort(64 * epsilon, test, rd);
        output.next(test);
    }

    // Distort circle
    {
        test_t test = perfect_circle_test();
        distort(128 * epsilon, test, rd);
        output.next(test);
    }

    // Tests with several circles
    int several_circles_tests_count = 5;
    std::uniform_int_distribution<> circles_amount(5, 15);
    for (int i = 0; i != several_circles_tests_count; ++i)
    {
        double radius = random_double(interval_t(50, 500), rd);
        test_t circles = random_circle_test(radius, small_test_size(rd), rd);
        int n = circles_amount(rd);
        for (int i = 0; i != n; ++i)
        {
            test_t c = random_circle_test(radius, small_test_size(rd), rd);
            move_points(c, random_point(medium_area, rd));
            merge(circles, c);
        }
        output.next(circles);
    }

    // Two grids
    {
        int n = grid_size(rd);
        int m = grid_size(rd);
        test_t grids = grid_test(medium_area,
                                 interval_t(1, 100),
                                 interval_t(1, 100),
                                 n,
                                 m,
                                 rd);
        n = grid_size(rd);
        m = grid_size(rd);
        test_t other_grid = grid_test(medium_area,
                                      interval_t(1, 100),
                                      interval_t(1, 100),
                                      n,
                                      m,
                                      rd);
        rotate(random_double(interval_t(0, 2 * pi), rd), grids);
        rotate(random_double(interval_t(0, 2 * pi), rd), other_grid);
        merge(grids, other_grid);
        output.next(grids);
    }

    // Grid and circle
    {
        int n = grid_size(rd);
        int m = grid_size(rd);
        test_t test = grid_test(medium_area,
                                interval_t(1, 100),
                                interval_t(1, 100),
                                n,
                                m,
                                rd);
        test_t circle = random_circle_test(
                    small_radius, small_test_size(rd), rd);
        merge(test, circle);
        output.next(test);
    }

    // Several distributed condensed set of points
    {
        int const distributed_points_test_count = 3;
        std::uniform_int_distribution<> point_count(10, 100);
        for (int i = 0; i != distributed_points_test_count; ++i)
        {
            test_t points;
            int n = point_count(rd);
            for (int j = 0; j != n; ++j)
            {
                merge(points, single_point_test(large_area, 50, rd));
            }
            distort(32 * epsilon, points, rd);
            output.next(points);
        }
    }

    // Big range
    int const large_area_test_count = 5;
    for (int i = 0; i != large_area_test_count ; ++i)
    {
        output.next(random_test(large_area, large_test_size(rd), rd));
    }
    // Small range
    int const small_area_test_count = 5;
    for (int i = 0; i != small_area_test_count; ++i)
    {
        output.next(random_test(small_area, large_test_size(rd), rd));
    }
    return 0;
}

int main(int argc, char* argv[])
{
    return run();
}
