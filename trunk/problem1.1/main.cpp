//task 1.1
#include "cordinate.h"
#include <fstream>

int main()
{
	std::ifstream file_in("in.txt");
	std::ofstream file_out("out.txt");
	
	cordinate direction(1, 0);
	cordinate pos(0, 0);

	long N;
	file_in >> N;
	for (long i = 0; i < N; ++i)
	{
		file_out << pos << '\n';
		pos.x += direction.x;
		pos.y += direction.y;

		std::swap<long>(direction.x, direction.y);

		if (direction.y == 0) //only in this case radius increase
		{
			direction.x += (direction.x > 0) ? (+1) : (-1);
			direction.x = -direction.x;
		}
	}
	return 0;
}