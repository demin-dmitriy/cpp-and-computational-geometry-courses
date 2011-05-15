#include <fstream>

struct cordinate
{
	long x;
	long y;

	explicit cordinate (long x, long y)
	{
		this->x = x;
		this->y = y;
	}
	friend std::ostream& operator<< (std::ostream& stream, cordinate cord)
	{
		return stream << cord.x << ' ' << cord.y;
	}
};