#include "bind.h"
#include <iostream>
#include <assert.h>

int f(int a, int b, int c)
{
	return a + b + c;
}

int main()
{
	int a = bind(&f, -10, _2, _3)(1, 10, -3);
	std::cout << a << std::endl;
	assert(a == -3);

	int b = bind([](int a, int b, int c) {return a + b + c; }, _1, _2, _3)(1, 10, -3);
	std::cout << b << std::endl;
	assert(b == 8);
	
	int c = bind(&f, bind(&f, _1, _1, _1), _2, _3)(1, 10, -3);
	std::cout << c << std::endl;
	assert(c == 10);
	
	std::cin >> a;
}