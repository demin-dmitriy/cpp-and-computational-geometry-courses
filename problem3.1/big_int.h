#pragma once

#include <string>
#include <cstdlib>
#include <vector>
#include <istream>
#include <ostream>
#include <algorithm>

typedef unsigned long ptype;
const ptype base = 1000000000;      //10^9 - radix
const unsigned int base_amount = 9; //log10(base)

struct big_int
{
public:
	big_int& operator+=(const big_int&);
	big_int& operator-=(const big_int&);
	big_int& operator*=(const big_int&);
	big_int& operator/=(const big_int&);
	big_int& operator%=(const big_int&);
	big_int operator-();

	const bool operator<(const big_int&) const;
	const bool operator==(const big_int&) const;
	friend int abs_compare(const big_int&, const big_int&);
	big_int(const long long = 0);

	void swap(big_int&);
	friend big_int abs(big_int);
	friend std::pair<big_int, big_int> divmod(const big_int& x, const big_int& y);
	friend int digits_amount(const big_int&);

	friend std::istream& operator>>(std::istream&, big_int&);
	friend std::ostream& operator<<(std::ostream&, const big_int&);
protected:
	void delete_leading_zeros();
	std::vector<ptype> digits_; // digits here are in reverse order 
	bool negative_;
};

big_int operator+(const big_int&, const big_int&);
big_int operator-(const big_int&, const big_int&);
big_int operator*(const big_int&, const big_int&);
big_int operator/(const big_int&, const big_int&);
big_int operator%(const big_int&, const big_int&);
const bool operator>(const big_int&, const big_int&);
const bool operator!=(const big_int&, const big_int&);
const bool operator<=(const big_int&, const big_int&);
const bool operator>=(const big_int&, const big_int&);

big_int abs(big_int);
big_int pow(big_int, big_int);
std::istream& operator>>(std::istream&, big_int&);
std::ostream& operator<<(std::ostream&, const big_int&);

inline const ptype safe_subtract(const ptype a, const ptype b, bool& carry); //safe for unsigned type
inline const ptype safe_sum(const ptype a, const ptype b, bool& carry); //safe for overflow
