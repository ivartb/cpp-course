#include "opt_vector.h"
#include <assert.h>
#include <algorithm>

opt_vector::opt_vector() : sz(0), n(0) {}

opt_vector::opt_vector(size_t size) : sz(size)
{
	if (sz < 2)
		n = 0;
	else
		v = std::make_shared<std::vector<unsigned>>(sz);
}

opt_vector::opt_vector(opt_vector const& other) : sz(other.sz)
{
	if (sz < 2)
		n = other.n;
	else
		v = other.v;
}

opt_vector& opt_vector::operator=(opt_vector const& other)
{
	if (*this == other)
		return *this;
	if (other.sz > 1)
		v = other.v;
	else
	{
		n = other.n;
		if (sz > 1)
			v.reset();
	}
	sz = other.sz;
	return *this;
}

opt_vector::~opt_vector() {}

void opt_vector::clear()
{
	sz = 0;
	v.reset();
}

void opt_vector::push_back(unsigned el)
{
	if (!v.unique())	
		makeCopy();
	sz++;
	if (sz < 2)
	{
		n = el;
		return;
	}
	if (sz == 2)
	{
		v = std::make_shared<std::vector<unsigned>>(1);
		(*v)[0] = n;
		(*v).push_back(el);
		return;
	}
	(*v).push_back(el);
}

void opt_vector::pop_back()
{
	if (!v.unique())	
		makeCopy();
	sz--;
	if (sz < 1)
		return;
	if (sz == 1)
	{
		n = (*v)[0];
		v.reset();
		return;
	}
	(*v).pop_back();
}

void opt_vector::resize(size_t size)
{
	if (!v.unique())
		makeCopy();
	if (sz == size)
		return;
	if (size == 0)
	{
		n = 0;
		v.reset();
		sz = size;
		return;
	}
	if (sz < 2 && size > 1)
	{
		v = std::make_shared<std::vector<unsigned>>(size);
		(*v)[0] = n;
		sz = size;
		return;
	}
	if (sz > 1 && size < 2)
	{
		n = (*v)[0];
		v.reset();
		sz = size;
		return;
	}
	sz = size;
	(*v).resize(size);
}

size_t opt_vector::size() const
{
	return sz;
}

unsigned const& opt_vector::operator[](size_t i) const
{
	assert(i < sz);
	if (sz < 2)
		return n;
	return (*v)[i];
}

unsigned& opt_vector::operator[](size_t i)
{
	assert(i < sz);
	if (!v.unique())
		makeCopy();
	if (sz < 2)
		return n;
	return (*v)[i];
}

unsigned const& opt_vector::back() const
{
	if (sz < 2)
		return n;
	return (*v).back();
}

bool operator==(opt_vector const& a, opt_vector const& b)
{
	if (a.sz != b.sz)
		return false;
	for (size_t i = 0; i != a.sz; i++)
		if (a[i] != b[i])
			return false;
	return true;
}

void reverse(opt_vector& a)
{
	if (a.sz < 2)
		return;
	a.makeCopy();
	std::reverse((*a.v).begin(), (*a.v).end());
}

void opt_vector::makeCopy()
{
    if (sz > 1)
	{
        std::shared_ptr<std::vector<unsigned>> tmp = std::make_shared<std::vector<unsigned>>(*v);
        v = tmp;
    }
}