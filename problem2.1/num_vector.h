#pragma once

#include <algorithm>
#include <assert.h>

typedef unsigned long ptype;

struct num_vector
{
public:
	num_vector(ptype default_value = 0, size_t size = 0);
	num_vector(const num_vector&);
	~num_vector();

	void push_back(const ptype& element);
	void pop_back();
	void clear();
	void assign(size_t count, ptype value);
	void reverse();
	size_t size() const;
	void swap(num_vector&);

	ptype& operator[](const size_t index);
	ptype operator[](const size_t index) const;
	num_vector& operator=(const num_vector&);

	void change_capacity(size_t new_capacity);
private:
	union digit_holder
	{
		ptype* digits_; //size_ > 1
		ptype number_; //size_ == 1
	} digit_holder_;
	size_t size_;
	size_t capacity_; // > 0; size_ == 1 => capacity == 1
};