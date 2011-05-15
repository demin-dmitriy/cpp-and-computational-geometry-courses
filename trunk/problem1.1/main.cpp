//task 1.1
#include "main.h"

int main()
{
	std::ifstream file_in("in.txt");
	std::ofstream file_out("out.txt");
	
	cordinate direction(1, 0);
	cordinate pos(0, 0);

	long N;
	file_in >> N;
	for(long i = 0; i < N; ++i)
	{
		file_out << pos << '\n';
		pos.x += direction.x;
		pos.y += direction.y;

		long tmp;
		tmp = direction.x;
		direction.x = direction.y;
		direction.y = tmp;
		
		if ( direction.y == 0) //only in this case radius increase
		{
			direction.x += (direction.x > 0) ? (+1) : (-1);
			direction.x = -direction.x;
		}
	}
	return 0;
}