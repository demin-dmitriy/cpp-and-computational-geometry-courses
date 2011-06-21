#include "num_vector.h"

num_vector::num_vector(ptype default_value, size_t size):
	size_(size),
	capacity_(size)
{
	if (capacity_ == 0)
	{
		capacity_ = 1;
	}

	if (size_ == 1) //capacity_ == size_
	{
		digit_holder_.number_ = default_value;
	}
	else if (size_ > 1)
	{
		capacity_ *= 2;
		digit_holder_.digits_ = new ptype[capacity_];
		for (size_t i = 0; i < size_; ++i)
		{
			digit_holder_.digits_[i] = default_value;
		}
	}
}

num_vector::num_vector(const num_vector& target):
	size_(target.size_),
	capacity_(target.capacity_)
{
	if (size_ == 1)
	{
		digit_holder_.number_ = target.digit_holder_.number_;
	}
	else if (size_ > 1)
	{
		digit_holder_.digits_ = new ptype[capacity_];
		for (size_t i = 0; i < size_; ++i)
		{
			digit_holder_.digits_[i] = target.digit_holder_.digits_[i];
		}
	}
}

num_vector::~num_vector()
{
	if (size_ > 1)
	{
		delete[] digit_holder_.digits_;
	}
}

void num_vector::push_back(const ptype& element)
{
	if (size_ == 0)
	{
		digit_holder_.number_ = element;
	}
	else
	{
		if (size_ == capacity_)
		{
			change_capacity(capacity_ * 2);
		}
		digit_holder_.digits_[size_] = element;
	}
	++size_;
}

void num_vector::pop_back()
{
	if (size_ < capacity_ / 2)
	{
		
		if (size_ == 2)
		{
			change_capacity(1);
		}
		else
		{
			change_capacity(capacity_ / 2);
		}
	}
	else if (size_ == 2)
	{
		capacity_ = 1;
		ptype tmp = digit_holder_.digits_[0];
		delete[] digit_holder_.digits_;
		digit_holder_.number_ = tmp;
	}
	--size_;
}

void num_vector::clear()
{
	if (size_ > 1) 
	{
		delete[] digit_holder_.digits_;
	}
	size_ = 0;
	capacity_ = 1;
}

void num_vector::assign(size_t count, ptype value)
{
	if (size_ > 1)
	{
		delete[] digit_holder_.digits_;
	}
	size_ = count;
	capacity_ = count;
	if (size_ == 1)
	{
		digit_holder_.number_ = value;
	}
	else
	{
		digit_holder_.digits_ = new ptype[count];
		for (size_t i = 0; i < size_; ++i)
		{
			digit_holder_.digits_[i] = value;
		}
	}
}

void num_vector::reverse()
{
	if (size_ > 1)
	{
		for (size_t i = 0; i < size_ / 2; ++i)
		{
			std::swap(digit_holder_.digits_[i], digit_holder_.digits_[size_ - i - 1]); 
		}
	}
}

size_t num_vector::size() const
{
	return size_;
}

ptype& num_vector::operator[](const size_t index)
{
	assert(index < size_);
	if (size_ == 1 && index == 0)
	{
		return digit_holder_.number_;
	}
	else
	{
		return digit_holder_.digits_[index];
	}
}

ptype num_vector::operator[](const size_t index) const
{
	assert(index < size_);
	if (size_ == 1 && index == 0)
	{
		return digit_holder_.number_;
	}
	else
	{
		return digit_holder_.digits_[index];
	}
}

num_vector& num_vector::operator=(const num_vector& target)
{
	num_vector tmp(target);
	swap(tmp);
	return *this;
}

void num_vector::swap(num_vector& target)
{
	std::swap(digit_holder_, target.digit_holder_);
	std::swap(size_, target.size_);
	std::swap(capacity_, target.capacity_);
}

void num_vector::change_capacity(size_t new_capacity)
{
	if (new_capacity != capacity_)
	{
		if (new_capacity == 1)
		{
			ptype tmp = digit_holder_.digits_[0];
			if (size_ > 1)
			{
				delete[] digit_holder_.digits_;
			}
			digit_holder_.number_ = tmp;
		}
		else
		{
			if (size_ == 1)
			{
				ptype tmp = digit_holder_.number_;
				capacity_ = new_capacity;
				digit_holder_.digits_ = new ptype[capacity_];
				digit_holder_.digits_[0] = tmp;
			}
			else
			{
				capacity_ = new_capacity;
				size_ = std::min(size_, capacity_);
				ptype* tmp = new ptype[capacity_];
				for (size_t i = 0; i < size_; ++i)
				{
					tmp[i] = digit_holder_.digits_[i];
				}
				delete[] digit_holder_.digits_;
				digit_holder_.digits_ = tmp;
			}
		}
	}
}
