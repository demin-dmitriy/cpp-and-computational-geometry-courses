#pragma once

#include <string>
#include "big_int.h"
#include "func_factory.h"
#include <stdexcept>
#include <cctype>
#include <sstream>

big_int calculate(const std::string& expression);
big_int count_sum(const std::string& expression, size_t& pos);
big_int count_factor(const std::string& expression, size_t& pos);
big_int count_exp(const std::string& expression, size_t& pos);
big_int count_token(const std::string& expression, size_t& pos);
