#include <ostream>

struct cordinate
{
	long x;
	long y;

	cordinate(long x0, long y0):
		x(x0),
		y(y0)
	{}

	friend std::ostream& operator<<(std::ostream& stream, cordinate cord)
	{
		return stream << cord.x << ' ' << cord.y;
	}
};