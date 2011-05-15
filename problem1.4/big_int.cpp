#include "big_int.h"

const big_int abs(big_int x){
	x.negative_ = false;
	return x;
}

std::istream& operator>> (std::istream& stream, big_int& target)
{
	std::string str;
	stream >> str;
	target.digits_.clear();
	size_t length = str.size();

	char buf[base_amount];
	while(length >= base_amount)			
	{
		length -= base_amount;
		for(size_t i = 0; i != base_amount; ++i)
		{
			buf[i] = str[length + i]; 
		}
		target.digits_.push_back(atol(buf));
	}
	for(size_t i = 0; i != length; ++i)	//rest of number
	{
		buf[i] = str[i]; 
	}
	buf[length] = '\0';
	target.digits_.push_back(atol(buf));

	return stream;
}

std::ostream& operator<< (std::ostream& stream, const big_int& target)
{
	if (target.negative_)
	{
		stream << '-';
	}
	std::streamsize tmp_s = stream.width();
	char tmp_f = stream.fill();
	stream << target.digits_[target.digits_.size() - 1];
	stream.fill('0');
	stream.width(base_amount);
	for (size_t i = target.digits_.size() - 2; i != -1; --i)
	{
		stream << target.digits_[i];
	}
	stream.width(tmp_s);
	stream.fill(tmp_f);
	return stream;
}

inline const ptype safe_subtract(const ptype a, const ptype b, bool& carry) //safe for unsigned type
{
	if (a >= b)
	{
		carry = false;
		return a - b;
	}
	else
	{
		carry = true;
		return a + (base - b);
	}
}

inline const ptype safe_sum(const ptype a, const ptype b, bool& carry) //safe for overflow
{
	if (a < base - b)
	{
		carry = false;
		return (a + b);
	}
	else
	{
		carry = true;
		return a - (base - b);
	}
}

const big_int& big_int::operator+= (const big_int &x)
{
	size_t x_size = x.digits_.size();
	size_t this_size = this->digits_.size();
		
	bool carry = false; //перенос в следующий разряд
	if (x.negative_ == this->negative_)
	{
		for (size_t i =	0; carry || i < x_size; ++i)
		{
			if (i >= this_size) this->digits_.push_back(0);	
			
			if (i < x_size)
			{
				this->digits_[i] = safe_sum(this->digits_[i], x.digits_[i] + ((carry) ? (1) : (0)), carry);
			}
			else
			{
				this->digits_[i] = safe_sum(this->digits_[i], ((carry) ? (1) : (0)), carry);
			}
		}
	}
	else
	{
		const big_int *larger, *smaller;
		if (abs(*this) > abs(x))
		{
			larger = this;
			smaller = &x;
		}
		else
		{
			larger = &x;
			smaller = this;
			this->negative_ = x.negative_;
		}
		size_t larger_size  = larger->digits_.size();
		size_t smaller_size = smaller->digits_.size();

		for (size_t i =	0; i < larger_size; ++i)
		{
			if (i >= this_size)	this->digits_.push_back(0);	

			if (i < smaller_size)
			{
				this->digits_[i] = safe_subtract(larger->digits_[i], smaller->digits_[i] + ((carry) ? (1) : (0)), carry);
			}
			else
			{
				this->digits_[i] = safe_subtract(larger->digits_[i], ((carry) ? (1) : (0)), carry);
			}
		}
	}
	return *this;
}

const big_int& big_int::operator-= (const big_int& x)
{
	this->negative_ = !this->negative_;
	*this += x;
	this->negative_ = !this->negative_;
	return *this;
}

const big_int& big_int::operator*= (const big_int& x)
{
	big_int result = 0;
	size_t this_size = this->digits_.size();
	size_t x_size = x.digits_.size();
	for (size_t i = 0; i < x_size; ++i)
	{
		big_int tmp;
		tmp.digits_.assign(this_size + i + 1, 0);
		ptype carry = 0; //перенос
		for (size_t k = 0; k < this_size; ++k)
		{
			long long long_tmp = (long long(this->digits_[k])) * (long long(x.digits_[i])) + carry;
			tmp.digits_[i + k] = long_tmp % base;
			carry = long_tmp / base;	
		}
		tmp.digits_[i + this_size] = carry;
		tmp.delete_lead_zero();
		result += tmp;
	}
	*this = result;
	return *this;
}

void devide_helper(const big_int& x, const big_int& y, big_int& result, big_int& rest)
{
	size_t this_size = x.digits_.size();
	result = 0;
	rest = 0;
	for (size_t i = this_size - 1; i != -1; --i)
	{
		rest *= base;
		rest += x.digits_[i];
		//bin-search for appropriate factor
		ptype a = 0, b = base, c;
		while (a + 1 < b)
		{
			c = a + (b - a) / 2;
			if (rest < c * y) b = c;
			else a = c;
		}
		rest -= a * y;
		result *= base;
		result += a;
	}
	result.negative_  = x.negative_ ^ y.negative_;
	result.delete_lead_zero();
	rest.delete_lead_zero();
}

const big_int& big_int::operator/= (const big_int &x)
{
	big_int result;
	big_int rest;
	devide_helper(*this, x, result, rest);
	return *this = result;
}

const big_int& big_int::operator%= (const big_int &x)
{
	big_int result;
	big_int rest;
	devide_helper(*this, x, result, rest);
	return *this = rest;
}

bool const big_int::operator< (const big_int &x) const
{
	size_t this_size = this->digits_.size();
	size_t x_size = x.digits_.size();
	if (this_size > x_size)
	{
		return false;
	}
	else if (this_size < x_size)
	{
		return true;
	}
	else 
	{
		for (size_t i = x_size - 1; i != -1 ; --i)
		{
			if (this->digits_[i] > x.digits_[i])
			{
				return false;
			}
			else if (this->digits_[i] < x.digits_[i])
			{
				return true;
			}
		}
	}
	return false;
}

big_int::big_int (const ptype &x)
{
	ptype t = x;
	while (t > base)
	{
		this->digits_.push_back(t % base);
		t /= base;
	} 
	this->digits_.push_back(t);
	this->negative_ = (x < 0);
}

void big_int::delete_lead_zero()
{
	for (size_t i = this->digits_.size() - 1; i != 0; --i)
	{
		if (this->digits_[i] == 0)
		{
			this->digits_.pop_back();
		}
		else
		{
			return;
		}
	}
	if (this->digits_[0] == 0) this->negative_ = false;
}

const big_int operator+ (const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp += y;
}

const big_int operator- (const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp -= y;
}

const big_int operator* (const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp *= y;
}

const big_int operator/ (const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp /= y;
}

const big_int operator% (const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp %= y;
}

const bool operator> (const big_int &x, const big_int &y)
{
	return (y < x);
}

const bool operator==(const big_int &x, const big_int &y)
{
	return !(x > y) && !(x < y);
}

const bool operator<=(const big_int& x, const big_int& y)
{
	return !(x > y);
}

const bool operator>=(const big_int& x, const big_int& y)
{
	return !(x < y);
}