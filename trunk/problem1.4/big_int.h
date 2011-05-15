#include <string>
#include <stdlib.h>
#include <vector>
#include <istream>
#include <ostream>

typedef unsigned long ptype;
const ptype base = 1000000000;			//10^9 - основание системы счисления
const unsigned int base_amount = 9;		//log10(base)

using std::string;

struct big_int
{
public:
	const big_int& operator+=(const big_int&);
	const big_int& operator-=(const big_int&);
	const big_int& operator*=(const big_int&);
	const big_int& operator/=(const big_int&);
	const big_int& operator%=(const big_int&);

	const bool operator< (const big_int&) const;
	big_int (const ptype& = 0);

	friend const big_int abs(big_int);
	friend std::istream& operator>> (std::istream&, big_int&);
	friend std::ostream& operator<< (std::ostream&, const big_int&);
private:
	void delete_lead_zero();
	friend void devide_helper(const big_int& x, const big_int& y, big_int& result, big_int& rest);
	std::vector<ptype> digits_;    // digits here are in reverse order 
	bool negative_;
};

const big_int operator+ (const big_int&, const big_int&);
const big_int operator- (const big_int&, const big_int&);
const big_int operator* (const big_int&, const big_int&);
const big_int operator/ (const big_int&, const big_int&);
const big_int operator% (const big_int&, const big_int&);
const bool operator> (const big_int&, const big_int&);
const bool operator==(const big_int&, const big_int&);
const bool operator<=(const big_int&, const big_int&);
const bool operator>=(const big_int&, const big_int&);

const big_int abs(big_int);
std::istream& operator>> (std::istream&, big_int&);
std::ostream& operator<< (std::ostream&, const big_int&);

inline const ptype safe_subtract(const ptype a, const ptype b, bool& carry);
inline const ptype safe_sum(const ptype a, const ptype b, bool& carry);

