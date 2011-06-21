#include <fstream>
#include <string>

#include "calculator.h"

int main()
{
	std::ifstream file_in("in.txt");
	std::ofstream file_out("out.txt");
	std::string expression;
	while (getline(file_in, expression))
	{
		try
		{
			file_out << calculate(expression) << '\n';
		}
		catch (std::runtime_error& e)
		{
			file_out << e.what() << '\n';
		}
	}
	
	return 0;
}