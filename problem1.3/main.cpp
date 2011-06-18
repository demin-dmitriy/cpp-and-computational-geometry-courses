//taskC
#include <fstream>

// http://www.e-maxx.ru/algo/extended_euclid_algorithm
long long gcd(long long a, long long b, long long& x, long long& y)
{
	if (a == 0)
	{
		x = 0;
		y = 1;
		return b;
	}
	long long x1, y1;
	long long d = gcd(b % a, a, x1, y1);
	x = y1 - (b / a) * x1;
	y = x1;
	return d;
}

int main()
{
	std::ifstream file_in("in.txt");
	std::ofstream file_out("out.txt");
	
	long long a, b, c;

	while(file_in >> a >> b >> c)
	{	
		if (a == 0 && b == 0) 
		{
			if (c == 0)
			{
				file_out << "0 0";
			}
			else
			{
				file_out << "<none>";
			}			
		}
		else 
		{
			long long x, y;
			long long d = gcd(a, b, x, y);
			if (c % d == 0)
			{
				file_out << x * (c / d) << ' ' << y * (c / d);
			}
			else
			{
				file_out << "<none>";
			}
		}
		file_out << '\n';
	}
	return 0;
}
