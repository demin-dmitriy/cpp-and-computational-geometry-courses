#include "calculator.h"


big_int calculate(const std::string& expression)
{
	std::string cleaned_expr;
	for (size_t end = expression.size(), i = 0; i < end; ++i)
	{
		char tmp = expression[i];
		if (isgraph(tmp))
		{
			cleaned_expr.push_back(tmp);
		}
	}

	size_t pos = 0;
	big_int result = count_sum(cleaned_expr, pos);

	if (pos == cleaned_expr.size())
	{
		return result;
	}
	else
	{
		throw std::runtime_error("Incorrect input");
	}
}

big_int count_sum(const std::string& expression, size_t& pos)
{
	big_int current_sum = count_factor(expression, pos);
	size_t end = expression.size(); 
	while (pos < end)
	{
		switch (expression[pos])
		{
		case '+':
			++pos;
			current_sum += count_factor(expression, pos);
			break;
		case '-':
			++pos;
			current_sum -= count_factor(expression, pos);
			break;
		default:
			return current_sum;
		}
	}
	return current_sum;
}

big_int count_factor(const std::string& expression, size_t& pos)
{
	big_int current_mult = count_exp(expression, pos);
	size_t end = expression.size(); 
	while (pos < end)
	{
		switch (expression[pos])
		{
		case '*':
			++pos;
			current_mult *= count_exp(expression, pos);
			break;
		case '/':
			++pos;
			current_mult /= count_exp(expression, pos);
			break;
		case ' ':
			++pos;
			break;
		default:
			return current_mult;
		}
	}
	return current_mult;
}

big_int count_exp(const std::string& expression, size_t& pos)
{
	big_int tmp = count_token(expression, pos);
	if (expression[pos] == '^')
	{
		++pos;
		return pow(tmp, count_exp(expression, pos));
	}
	else
	{
		return tmp;
	}
}

big_int count_token(const std::string& expression, size_t& pos)
{
	char tmp = expression[pos];
	if (tmp == '-')
	{
		++pos;
		return -1 * count_token(expression, pos);
	}
	else if (isdigit(tmp)) //there is a number
	{
		std::stringstream str;
		str << tmp;
		++pos;
		tmp = expression[pos];
		while (isdigit(tmp))
		{
			str << tmp;
			++pos;
			tmp = expression[pos];
		}
		big_int result;
		str >> result;
		return result;
	}
	else if (isalpha(tmp)) //we met a function
	{
		size_t expr_size = expression.size();
		size_t begin = pos;
		while (pos < expr_size && expression[pos] != '(')
		{
			++pos;
		}
		size_t length = pos - begin;
		++pos;
		big_int arg = count_sum(expression, pos);
		++pos;
		return factory::instance().calculate(expression.substr(begin, length), arg);
	}
	else if (tmp == '(') //brackets
	{
		++pos;
		big_int result = count_sum(expression, pos);
		if (expression[pos] == ')')
		{
			++pos;
			return result;
		}
		else
		{
			throw std::runtime_error("Unexpected symbol");
		}
	}
	else
	{
		throw std::runtime_error("Unexpected symbol at " + pos);
	}
	return 0;
}