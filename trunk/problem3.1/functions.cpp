#include "func_factory.h"

big_int func_sqr(const big_int& x)
{
	return x * x;
}

big_int func_digits(const big_int& x)
{

	return base_amount * digits_amount(x);
}

big_int func_fact(const big_int& n)
{
	big_int result = 1;
	for (big_int i = 2; i <= n; i += 1)
	{
		result *= i;
	}
	return result;
}

namespace
{
	const bool reg_func_sqr = factory::instance().register_function("sqr", func_sqr);
	const bool reg_func_digits = factory::instance().register_function("digits", func_digits);
	const bool reg_func_fact = factory::instance().register_function("fact", func_fact);
}