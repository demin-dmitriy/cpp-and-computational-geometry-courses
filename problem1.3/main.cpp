//taskC
#include "main.h"

// http://www.e-maxx.ru/algo/extended_euclid_algorithm
int gcd(int a, int b, int& x, int& y)
{
	if (a == 0)
	{
		x = 0;
		y = 1;
		return b;
	}
	int x1, y1;
	int d = gcd(b % a, a, x1, y1);
	x = y1 - (b / a) * x1;
	y = x1;
	return d;
}

int main()
{
	std::ifstream file_in("in.txt");
	std::ofstream file_out("out.txt");
	
	long a, b, c;

	while(file_in >> a >> b >> c)
	{	
		if (a == 0 && b == 0) 
		{
			if (c == 0)
			{
				file_out << "0 0\n";
			}
			else
			{
				file_out << "<none>\n";
			}			
		}
		else 
		{
			int x, y;
			int d = gcd(a, b, x, y);
			if (c % d == 0)
			{
				file_out << x * (c / d) << ' ' << y * (c / d) << '\n';
			}
			else
			{
				file_out << "<none>\n";
			}
		}
	}
	return 0;
}
