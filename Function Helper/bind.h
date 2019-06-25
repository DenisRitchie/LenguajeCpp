#pragma once
#include <cstdio>
#include <functional>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace std::placeholders;

namespace test::_bind
{
    void square(double value)
    {
        std::cout << value << "^2 == " << value * value << std::endl;
    }

    void product(double value1, double value2)
    {
        std::cout << value1 << "*" << value2 << " == " << value1 * value2 << std::endl;
    }

    void run()
    {
        double arg[] = { 1, 2, 3 };
        std::for_each(arg, arg + _countof(arg), square);
        std::cout << std::endl;

        std::for_each(arg, arg + _countof(arg), std::bind(product, _1, 2));
        std::cout << std::endl;

        std::for_each(arg, arg + _countof(arg), std::bind(square, _1));
        std::cout << std::endl;
    }
}
