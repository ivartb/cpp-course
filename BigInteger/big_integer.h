#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

struct big_integer
{
    big_integer();//done
    big_integer(big_integer const& other);//done
    big_integer(int a);//done
	big_integer(long long a);//done
	explicit big_integer(std::string const& str);//done
    ~big_integer();//done

    big_integer& operator=(big_integer const& other);//done

    big_integer& operator+=(big_integer const& rhs);//done
    big_integer& operator-=(big_integer const& rhs);//done
    big_integer& operator*=(big_integer const& rhs);//done
    big_integer& operator/=(big_integer const& rhs);//done
    big_integer& operator%=(big_integer const& rhs);//done

    big_integer& operator&=(big_integer const& rhs);//done
    big_integer& operator|=(big_integer const& rhs);//done
    big_integer& operator^=(big_integer const& rhs);//done

    big_integer& operator<<=(int rhs);//done
    big_integer& operator>>=(int rhs);//done

    big_integer operator+() const;//done
    big_integer operator-() const;//done
    big_integer operator~() const;//done

    big_integer& operator++();//done
    big_integer operator++(int);//done

    big_integer& operator--();//done
    big_integer operator--(int);//done

    friend bool operator==(big_integer const& a, big_integer const& b);//done
    friend bool operator!=(big_integer const& a, big_integer const& b);//done
    friend bool operator<(big_integer const& a, big_integer const& b);//done
    friend bool operator>(big_integer const& a, big_integer const& b);//done
    friend bool operator<=(big_integer const& a, big_integer const& b);//done
    friend bool operator>=(big_integer const& a, big_integer const& b);//done

    friend std::string to_string(big_integer const& a);//done

	void toAddCode();//done

private:
    char sign;
    std::vector<unsigned> data;
    static const unsigned int base = 4294967295;
};

big_integer operator+(big_integer a, big_integer const& b);//done
big_integer operator-(big_integer a, big_integer const& b);//done
big_integer operator*(big_integer a, big_integer const& b);//done
big_integer operator/(big_integer a, big_integer const& b);//done
big_integer operator%(big_integer a, big_integer const& b);//done

big_integer operator&(big_integer a, big_integer const& b);//done
big_integer operator|(big_integer a, big_integer const& b);//done
big_integer operator^(big_integer a, big_integer const& b);//done

big_integer operator<<(big_integer a, int b);//done
big_integer operator>>(big_integer a, int b);//done

bool operator==(big_integer const& a, big_integer const& b);//done
bool operator!=(big_integer const& a, big_integer const& b);//done
bool operator<(big_integer const& a, big_integer const& b);//done
bool operator>(big_integer const& a, big_integer const& b);//done
bool operator<=(big_integer const& a, big_integer const& b);//done
bool operator>=(big_integer const& a, big_integer const& b);//done

std::string to_string(big_integer const& a);//done
std::ostream& operator<<(std::ostream& s, big_integer const& a);//done

#endif // BIG_INTEGER_H