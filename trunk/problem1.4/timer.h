#pragma once
#include <time.h>
#include <iostream>


class timer_t
{
	clock_t init_time;
	static int count;
public:
	timer_t(): init_time(clock()){}
	~timer_t()
	{
		std::cout << static_cast<double>(clock() - init_time) / CLOCKS_PER_SEC << ' ' << count++ << '\n';
	}
};
int timer_t::count = 0;
