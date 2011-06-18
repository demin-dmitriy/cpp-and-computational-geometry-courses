#include "main.h"
#include <math.h>

int main()
{
	
	big_int s = 5;
	big_int a = 1;
	big_int b = 1;
	{timer_t timer;
	for (int i = 0; i < 1000000; ++i)
	{
		for (int j = 0; j < 1; ++j)
		{
			s *= (i % 2 == 0) ? a: b;
		}
	}
	}
	
	std::ifstream file_in("in.txt");
	std::ofstream file_out("out.txt");
	big_int x, y;
	while(file_in >> x >> y)
	{
		timer_t timer;
		if (y == 0)
		{
			file_out << "<error>" << "\n\n\n";
		}
		else
		{
			file_out << x / y << '\n' << x % y << "\n\n";
		}
	}
	// hello
	system("pause");

	return 0;
}