//task 1.2
#include <fstream>

int main()
{
	std::ifstream file_in("in.txt");
	std::ofstream file_out("out.txt");

	double x_old, y_old;
	double x_new, y_new;
	double area = 0;

	file_in >> x_old >> y_old;
	double x_first = x_old, y_first = y_old;

	while(file_in >> x_new >> y_new)
	{
		area += (x_new - x_old) * (y_new + y_old) / 2;
		x_old = x_new;
		y_old = y_new;
	}
	area += (x_first - x_old) * (y_first + y_old) / 2;
	if (area < 0)
	{
		area = -area;
	}
	file_out.precision(15);
	file_out << area;

	return 0;
}