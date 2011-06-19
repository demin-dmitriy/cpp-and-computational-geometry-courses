#include "main.h"

int main()
{	
	std::ifstream file_in("in.txt");
	std::ofstream file_out("out.txt");
	big_int x, y;
	std::pair<big_int, big_int> result;
	while (file_in >> x >> y)
	{
		if (y == 0)
		{
			file_out << "<error>" << "\n\n\n";
		}
		else
		{
			result = divmod(x, y);
			file_out << result.first << '\n' << result.second << "\n\n";
		}
	}

	return 0;
}