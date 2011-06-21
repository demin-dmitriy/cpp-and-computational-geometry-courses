#include "big_int.h"

big_int abs(big_int x){
	x.negative_ = false;
	return x;
}

std::istream& operator>>(std::istream& stream, big_int& target)
{
	while(isspace(stream.peek()))
	{
		stream.get();
	}
	
	if (stream.eof())
	{
		stream.setstate(std::ios::failbit);
		return stream;
	}

	if (stream.peek() == '-' || stream.peek() == '+')
	{
		if (stream.get() == '-')
		{
			target.negative_ = true;
		}
		else
		{
			target.negative_ = false;
		}
	}

	if (!isdigit(stream.peek()))
	{
		stream.setstate(std::ios::failbit);
		return stream;
	}

	std::string str;
	while (isdigit(stream.peek()))
	{
		str.push_back(stream.get());
	}

	target.digits_.clear();
	size_t length = str.size();

	char buf[base_amount];
	while (length >= base_amount)			
	{
		length -= base_amount;
		for (size_t i = 0; i != base_amount; ++i)
		{
			buf[i] = str[length + i]; 
		}
		target.digits_.push_back(atol(buf));
	}
	for (size_t i = 0; i != length; ++i) //rest of number
	{
		buf[i] = str[i]; 
	}

	if (length != 0)
	{
		buf[length] = '\0';
		target.digits_.push_back(atol(buf));
	}
	target.delete_leading_zeros();

	return stream;
}

std::ostream& operator<<(std::ostream& stream, const big_int& target)
{
	if (target.negative_)
	{
		stream << '-';
	}
	std::streamsize tmp_s = stream.width();
	char tmp_f = stream.fill();
	stream << target.digits_[target.digits_.size() - 1];
	stream.fill('0');
	for (size_t i = target.digits_.size() - 2; i != -1; --i)
	{
		stream.width(base_amount);
		stream << target.digits_[i];
	}
	stream.width(tmp_s);
	stream.fill(tmp_f);
	return stream;
}

inline const ptype safe_subtract(const ptype a, const ptype b, bool& carry)
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

inline const ptype safe_sum(const ptype a, const ptype b, bool& carry) 
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

big_int& big_int::operator+=(const big_int &x)
{
	size_t x_size = x.digits_.size();
	size_t this_size = this->digits_.size();
		
	bool carry = false; //dot and carry one
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
	this->delete_leading_zeros();
	return *this;
}

big_int& big_int::operator-=(const big_int& x)
{
	this->negative_ = !this->negative_;
	*this += x;
	if (*this != 0) 
	{
		this->negative_ = !this->negative_;
	}
	return *this;
}

big_int& big_int::operator*=(const big_int& x)
{
	big_int result = 0;
	size_t this_size = this->digits_.size();
	size_t x_size = x.digits_.size();
	for (size_t i = 0; i < x_size; ++i)
	{
		big_int tmp;
		tmp.digits_.assign(this_size + i + 1, 0);
		ptype carry = 0; //dot and carry one
		for (size_t k = 0; k < this_size; ++k)
		{
			long long long_tmp = static_cast<unsigned long long>(this->digits_[k]) * 
				static_cast<unsigned long long>(x.digits_[i]) + carry;
			
			tmp.digits_[i + k] = long_tmp % base;
			carry = static_cast<ptype>(long_tmp / base);	
		}
		tmp.digits_[i + this_size] = carry;
		tmp.delete_leading_zeros();
		result += tmp;
	}
	if (result == 0)
	{
		result.negative_ = false;
	}
	else
	{
		result.negative_ = (x.negative_ ^ this->negative_);
	}
	*this = result;
	return *this;
}

std::pair<big_int, big_int> divmod(const big_int& x, const big_int& y)
{
	big_int result = 0;
	big_int rest = 0;

	big_int devider = y;
	devider.negative_ = false;

	for (size_t i = x.digits_.size() - 1; i != -1; --i)
	{
		rest *= base;
		rest += x.digits_[i];
		if (rest >= devider)
		{
			//Bin-search for appropriate factor
			ptype a = 0, b = base, c;
			while (a + 1 < b)
			{
				c = a + (b - a) / 2;
				if (rest < c * devider) b = c;
				else a = c;
			}
			rest -= a * devider;
			result.digits_.push_back(a);
		}
		else //Very common situation. At previous case will still work correct but slowly.
		{
			result.digits_.push_back(0);
		}
	}
	reverse(result.digits_.begin(), result.digits_.end());
	result.negative_ = x.negative_ ^ y.negative_;
	rest.negative_ = x.negative_;
	result.delete_leading_zeros();
	rest.delete_leading_zeros();
	return std::make_pair<big_int, big_int>(result, rest);
}

big_int& big_int::operator/=(const big_int &x)
{
	return *this = divmod(*this, x).first;
}

big_int& big_int::operator%=(const big_int &x)
{
	return *this = divmod(*this, x).second;
}

big_int::big_int(const long long x):
	negative_(x < 0)
{
	ptype t = static_cast<ptype>(abs(x));
	while (t > base)
	{
		this->digits_.push_back(t % base);
		t /= base;
	} 
	this->digits_.push_back(t);
}

void big_int::delete_leading_zeros()
{
	if (digits_.size() == 0)
	{
		return;
	}
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

big_int operator+(const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp += y;
}

big_int operator-(const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp -= y;
}

big_int operator*(const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp *= y;
}

big_int operator/(const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp /= y;
}

big_int operator%(const big_int &x, const big_int &y)
{
	big_int tmp = x;
	return tmp %= y;
}

bool const big_int::operator<(const big_int &x) const
{
	if (this->negative_ == true && x.negative_ == false)
	{
		return true;
	}
	else if (this->negative_ == false && x.negative_ == true)
	{
		return false;
	}
	else if (this->negative_ == false && x.negative_ == false)
	{
		return (abs_compare(*this, x) == -1);
	}
	else //if (this->negative_ == true && x.negative_ == true)
	{
		return (abs_compare(*this, x) == +1);
	}
}

// x < y => -1; x == y => 0; x > y => 1. Doesn't use sign field (negative_)  
int abs_compare(const big_int& x, const big_int& y)
{
	size_t x_size = x.digits_.size();
	size_t y_size = y.digits_.size();
	
	if (x_size > y_size)
	{
		return +1;
	}
	else if (x_size < y_size)
	{
		return -1;
	}
	else 
	{
		for (size_t i = y_size - 1; i != -1 ; --i)
		{
			if (x.digits_[i] > y.digits_[i])
			{
				return +1;
			}
			else if (x.digits_[i] < y.digits_[i])
			{
				return -1;
			}
		}
	}
	return 0;
}

const bool operator>(const big_int &x, const big_int &y)
{
	return (y < x);
}

const bool big_int::operator==(const big_int &y) const
{
	return (this->negative_ == y.negative_) && (abs_compare(*this, y) == 0);
}
const bool operator<=(const big_int& x, const big_int& y)
{
	return !(x > y);
}

const bool operator>=(const big_int& x, const big_int& y)
{
	return !(x < y);
}

big_int big_int::operator-()
{
	big_int result = *this;
	if (result != 0)
	{
		result.negative_ = !result.negative_;
	}
	return result;
}

const bool operator!=(const big_int& x, const big_int& y)
{
	return !(x == y);
}

void big_int::swap(big_int& x)
{
	std::swap(this->negative_, x.negative_);
	std::swap<std::vector<ptype>>(digits_, x.digits_);
}