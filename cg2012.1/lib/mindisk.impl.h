#include <cassert>

#include <algorithm>
#include <iterator>

namespace geometry
{

template<typename ForwardIterator>
circle_t make_disk_out_of(ForwardIterator it, int const n = 0)
{
    circle_t disk;
    for (int i = 0; i != n && !disk.contains(*it); ++i, ++it)
    {
        disk.add_point(*it);
    }
    return disk;
}

template<typename ForwardIterator>
circle_t b_mindisk(ForwardIterator first, ForwardIterator last, int const n = 0)
{
    circle_t disk = make_disk_out_of(first, n);
    if (n == 3 || first == last)
    {
        return disk;
    }
    auto it = first;
    std::advance(it, n);
    auto to_swap = it;
    while (it != last)
    {
        if (!disk.contains(*it))
        {
            std::swap(*it, *to_swap);
            ++it;
            disk = b_mindisk(first, it, n + 1);
        }
        else
        {
            ++it;
        }
    }
    return disk;
}

template<typename RandomAccessIterator>
circle_t mindisk(RandomAccessIterator first, RandomAccessIterator last)
{
    random_shuffle(first, last);
    return b_mindisk(first, last, 0);
}

} // end of namespace geometry
