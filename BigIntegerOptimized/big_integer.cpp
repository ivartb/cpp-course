#include "big_integer.h"
#include <assert.h>
#include <algorithm>

const big_integer ZERO = big_integer(0);
const big_integer ONE = big_integer(1);

big_integer::big_integer(): sign(1)
{
	data.clear();
	data.push_back(0);
}

big_integer::big_integer(int a): sign((a < 0) ? -1 : 1)
{
	data.clear();
	data.push_back(abs(a));
}

big_integer::big_integer(long long a)
{
	data.clear();
	if (a == 0)
	{
		sign = 1;
		data.push_back(0);
		return;
	}
	sign = (a > 0) ? 1 : -1;
	if (a < 0)
		a = -a;
	data.push_back(a % base);
	data.push_back((unsigned)(a / base));
	while (data.size() > 1 && data.back() == 0)
		data.pop_back();
}

big_integer::big_integer(big_integer const &other)
{
	*this = other;
}

big_integer::big_integer(std::string const &str)
{
	data.clear();
	sign = 1;
	int j = 0;
	if (str[0] == '-') 
		j++;
	big_integer mul(10);
	for (size_t i = j; i < str.length(); i++)
	{
		*this *= mul;
		*this += big_integer(str[i] - 48);
	}
	if (str[0] == '-' && !(data[0] == 0 && data.size()== 1))
		sign = -1;
}

big_integer::~big_integer()
{
	sign = 0;
	data.clear();
}

big_integer &big_integer::operator=(big_integer const &other)
{
	data = other.data;
	sign = other.sign;
	return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs)
{
	if ((*this) == -rhs)
		return *this = ZERO;
	if ((*this) ==  ZERO)
		return *this = rhs;
	big_integer b(rhs);
	if (sign * rhs.sign > 0)
	{
		int carry = 0;
		for (size_t i = 0; i < std::max(data.size(), rhs.data.size()) || carry; i++)
		{
			if (i == data.size())
				data.push_back(0);
			unsigned long long tmp = 1ull * data[i] + carry + (i < rhs.data.size() ? rhs.data[i] : 0);
			data[i] = tmp % base;
			carry = (int)(tmp / base);
		}
		while (data.size() > 1 && data.back() == 0)
			data.pop_back();
		return *this;
	}
	if (b.sign == -1 && sign == 1)
	{
		b.sign = 1;
		*this -= b;
		b.sign = -1;
		return *this;
	}
	b.sign = -1;
	*this -= b;
	b.sign = -1;
	return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {

	if ((*this) == rhs)
		return *this = ZERO;

	if (sign == 1 && rhs.sign == -1)
		return (*this) += (-rhs);
	if (sign == -1 && rhs.sign == 1)
	{
		*this = -(*this);
		*this += rhs;
		return *this = -(*this);
	}

	if ((*this < rhs && sign == 1) || (*this > rhs && sign == -1))
	{
		*this = -(rhs - *this);
		return *this;
	}

	int carry = 0;
	for (size_t i = 0; i < rhs.data.size() || carry; i++)
	{
		long long tmp = 1ll * data[i] - carry - (i < rhs.data.size() ? rhs.data[i] : 0);
		carry = tmp < 0;
		if (carry)
			tmp += base;
		data[i] = (unsigned)tmp;
	}
	while (data.size() > 1 && data.back() == 0)
		data.pop_back();
	return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs)
{
	big_integer bi;
	bi.data.resize(data.size() + rhs.data.size());
	bi.sign = sign * rhs.sign;
	if (*this == ZERO || rhs == ZERO)
		return *this = ZERO;

	if (data.size() == 1 && rhs.data.size() > 1)
		return *this = rhs * (*this);
	
	if (rhs.data.size() == 1)
	{
		unsigned long long carry = 0;
		for (size_t i = 0; i < data.size() || carry; i++)
		{
			if (i == data.size())
				data.push_back(0);
			unsigned long long tmp = carry + data[i] * 1ull * rhs.data[0];
			data[i] = tmp % base;
			carry = tmp / base;
		}
		while (data.size() > 1 && data.back() == 0)
			data.pop_back();
		sign = bi.sign;
		return *this;
	}

	for (size_t i = 0; i < data.size(); i++)
	{
		unsigned long long carry = 0;
		for (size_t j = 0; j < rhs.data.size() || carry; j++)
		{
			unsigned long long tmp = bi.data[i + j] * 1ull + data[i] * 1ull * (j < rhs.data.size() ? rhs.data[j] : 0) + carry;
			bi.data[i + j] = tmp % base;
			carry = tmp / base;
		}
	}

	while (bi.data.size() > 1 && bi.data.back() == 0)
		bi.data.pop_back();

	*this = bi;
	return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
	big_integer ans;
    big_integer b(rhs);
    ans.data.clear();
    ans.sign = sign * rhs.sign;
    sign = b.sign = 1;

    if (b == ONE || b == -ONE)
	{
        sign = ans.sign;
        return *this;
    }

    if (*this == b) 
	{
        *this = ONE;
        sign = ans.sign;
        return *this;
    }

    if (*this < b)
	{
        ans = ZERO;
        *this = ans;
        return *this;
    }

    if (b.data.size() == 1) 
	{
        unsigned long long carry = 0;
        for (int i = (int) data.size() - 1; i >= 0; i--) 
		{
            unsigned long long cur = 1ull * data[i] + carry * 1ull * base;
            data[i] = (cur / (1ull * b.data[0]));
            carry = cur % (1ull * b.data[0]);
        }
        while (data.size() > 1 && data.back() == 0)
            data.pop_back();
        sign = ans.sign;
        return *this;
    }

    int n = b.data.size();
    int m = data.size() - n;
    if (b.data[n - 1] <= base / 2)
	{
        long long sc = base / (b.data[n - 1] + 1);
        *this *= sc;
        b *= sc;
    }
    n = b.data.size();
    m = data.size() - n;

    for (long long i = m - 1; i >= 0; i--)
	{
        if (data.size() == 1 && data[0] == 0)
		{
            ans.data.push_back(0);
            continue;
        }
        unsigned long long qc = (data[n + i] * 1ull * base + 1ull * data[n + i - 1]);
        qc /= (1ull * (b.data[n - 1]));

        if (qc >= base) 
		{
            i++;
            qc /= base;
            qc++;
        }
        big_integer y = (b * (long long) qc);
        y <<= (i * 32);
        *this -= y;
        while (sign < 0)
		{
            *this += y;
            qc--;
            y = b * (long long) qc;
            y <<= (i * 32);
            *this -= y;
        }
        ans.data.push_back(qc);
    }

	reverse(ans.data);
    *this = ans;
    while (data.size() > 1 && data.back() == 0)
        data.pop_back();
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) 
{
	*this -= (*this / rhs) * rhs;
	return *this;
}

void big_integer::toAddCode()
{
	for (size_t i = 0; i < data.size(); i++)
		data[i] = ~data[i] + 1;
}

big_integer &big_integer::operator<<=(int rhs) {
	if (rhs < 0)
	{
		*this >>= -rhs;
		return *this;
	}
	if (sign < 0)
		toAddCode();
	
	size_t s = data.size();

	unsigned int t = rhs / 32;
	unsigned rem = rhs - t * 32;
	if (rem)
	{
		long long carry = 0;
		for (size_t i = 0; i < data.size() || carry; i++)
		{
			if (i == data.size())
				data.push_back(0);
			long long tmp = ((1ll * data[i]) << rem) + carry;
			data[i] = tmp & (1ll * base);
			carry = tmp >> 32;
		}
	}

	data.resize(data.size() + t);
	for (int i = data.size() - 1; i >= (int) t; i--)
		data[i] = data[i - t];
	for (int i = t - 1; i >= 0; i--)
		data[i] = 0;

	if (sign < 0)
		toAddCode();
	
	while (data.size() > 1 && data.back() == 0)
		data.pop_back();
	return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
	if (rhs < 0)
	{
		*this <<= -rhs;
		return *this;
	}
	if (sign < 0)
		toAddCode();
	
	size_t s = data.size();

	unsigned int t = rhs / 32;
	unsigned rem = rhs - t * 32;

	data.resize(data.size() - t);
	for (size_t i = t; i < data.size(); i++)
		data[i - t] = data[i];
	for (size_t i = data.size() - t; i < data.size(); i++)
		data[i] = 0;

	if (rem)
	{
		long long carry = 0;
		if (sign == -1)
			carry = base;
		for (int i = (int) data.size() - 1; i >= 0; i--)
		{
			long long shl = (1ll * data[i]) >> rem;
			long long tmp = shl + (carry << (32 - rem));
			carry = data[i] - (shl << rem);
			data[i] = tmp & (1ll * base);
		}
	}

	if (sign < 0)
		toAddCode();
	
	while (data.size() > 1 && data.back() == 0)
		data.pop_back();
	return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs)
{
	if (*this == ZERO || rhs == ZERO)
		return *this = ZERO;

	char nsign = (sign == -1 && rhs.sign == -1) ? -1 : 1;
	big_integer b = big_integer(rhs);
	
	if (b.data.size() > data.size())
		data.resize(b.data.size());
	else
		b.data.resize(data.size());

	if (sign == -1)
		toAddCode();
	if (b.sign == -1)
		b.toAddCode();

	for (size_t i = 0; i < data.size(); i++)
		data[i] &= b.data[i];
	sign = nsign;
	if (nsign == -1)
		toAddCode();

	while (data.size() > 1 && data.back() == 0)
		data.pop_back();
	return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs)
{
	if (*this == ZERO || rhs == ZERO)
		return *this = ZERO;

	char nsign = (sign == -1 || rhs.sign == -1) ? -1 : 1;
	big_integer b = big_integer(rhs);
	
	if (b.data.size() > data.size())
		data.resize(b.data.size());
	else
		b.data.resize(data.size());

	if (sign == -1)
		toAddCode();
	if (b.sign == -1)
		b.toAddCode();

	for (size_t i = 0; i < data.size(); i++)
		data[i] |= b.data[i];
	sign = nsign;
	if (nsign == -1)
		toAddCode();

	while (data.size() > 1 && data.back() == 0)
		data.pop_back();
	return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs)
{
	if (*this == ZERO || rhs == ZERO)
		return *this = ZERO;

	char nsign = ((sign == -1) ^ (rhs.sign == -1)) ? -1 : 1;
	big_integer b = big_integer(rhs);
	
	if (b.data.size() > data.size())
		data.resize(b.data.size());
	else
		b.data.resize(data.size());

	if (sign == -1)
		toAddCode();
	if (b.sign == -1)
		b.toAddCode();

	for (size_t i = 0; i < data.size(); i++)
		data[i] ^= b.data[i];
	sign = nsign;
	if (nsign == -1)
		toAddCode();

	while (data.size() > 1 && data.back() == 0)
		data.pop_back();
	return *this;
}

big_integer big_integer::operator+() const
{
	return *this;
}

big_integer big_integer::operator-() const
{
	big_integer r(*this);
	if (r != ZERO)
		r.sign *= -1;
	return r;
}

big_integer big_integer::operator~() const
{
	return -(*this + 1);
}

big_integer &big_integer::operator++()
{
	return *this += 1;
}

big_integer big_integer::operator++(int)
{
	big_integer r = *this;
	++(*this);
	return r;
}

big_integer &big_integer::operator--()
{
	return *this -= 1;
}

big_integer big_integer::operator--(int)
{
	big_integer r = *this;
	--(*this);
	return r;
}

bool operator==(big_integer const &a, big_integer const &b)
{
	if (a.sign != b.sign || a.data.size() != b.data.size())
		return false;
	for (size_t i = 0; i < a.data.size(); i++)
		if (a.data[i] != b.data[i])
			return false;
	return true;
}

bool operator!=(big_integer const &a, big_integer const &b)
{
	return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b)
{
	if (a.sign != b.sign)
		return a.sign < b.sign;
	if (a.data.size() != b.data.size())
		return a.sign == 1 ? a.data.size() < b.data.size() : a.data.size() > b.data.size();
	for (int i = (int) a.data.size() - 1; i >= 0; i--)
		if (a.data[i] != b.data[i])
			return a.sign == 1 ? a.data[i] < b.data[i] : a.data[i] > b.data[i];
	return false;
}

bool operator>(big_integer const &a, big_integer const &b)
{
	return !(a <= b);
}

bool operator<=(big_integer const &a, big_integer const &b)
{
	return (a < b) || (a == b);
}

bool operator>=(big_integer const &a, big_integer const &b)
{
	return !(a < b);
}

std::string to_string(big_integer const &a)
{
	if (a == ZERO)
		return "0";
	
	std::string ans = "";
	char sign = a.sign;
	big_integer b = big_integer(a);
	big_integer div = 10;
	while (b.data.back())
	{
		big_integer last_digit = b % div;
		ans += last_digit.data[0] + 48;
		b /= div;
	}
	if (sign == -1)
		ans += '-';

	std::reverse(ans.begin(), ans.end());
	return ans;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a)
{
	return s << to_string(a);
}

big_integer operator+(big_integer a, big_integer const &b)
{
	return a += b;
}

big_integer operator-(big_integer a, big_integer const &b)
{
	return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b)
{
	return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b)
{
	return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b)
{
	return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b)
{
	return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b)
{
	return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b)
{
	return a ^= b;
}

big_integer operator<<(big_integer a, int b)
{
	return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
	return a >>= b;
}