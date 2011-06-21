#pragma once

#include "big_int.h"
#include <string>
#include <map>

struct factory //patterns: singletone, factory method
{
private:
	typedef big_int (*function)(const big_int&);
public:
	bool register_function(const std::string&, function);
	big_int calculate(const std::string&, const big_int&);
	static factory& instance();

private:
	typedef std::map<std::string, function> assoc_map;
	assoc_map associations_;
	factory();
	factory(const factory&);
	factory& operator=(const factory&);
	~factory();
};
