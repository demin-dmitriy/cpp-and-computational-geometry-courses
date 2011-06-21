#include "func_factory.h"

bool factory::register_function(const std::string& name, function itself)
{
	return associations_.insert(assoc_map::value_type(name, itself)).second;
}

big_int factory::calculate(const std::string& name, const big_int& arg1)
{
	assoc_map::const_iterator i = associations_.find(name);
	if (i != associations_.end())
	{
		return (i->second)(arg1);
	}
	else
	{
		throw std::runtime_error("Unknown function '" + name + "'");
	}
}
factory& factory::instance()
{
	static factory instance;
	return instance;
}

factory::factory()
{}

factory::~factory()
{}