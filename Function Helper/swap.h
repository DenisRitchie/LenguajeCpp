#pragma once
#include <functional>
#include <iostream>

namespace test::_swap
{
    inline constexpr int negation(const int &value)
    {
        return -value;
    }

    void run()
    {
        std::function<int(int)> fn0(negation);
        std::cout << std::boolalpha << "empty == " << !fn0 << std::endl;
        std::cout << "value == " << fn0(3) << std::endl;

        std::function<int(int)> fn1;
        std::cout << std::boolalpha << "empty == " << !fn1 << std::endl;
        std::cout << std::endl;

        std::swap(fn0, fn1);
        std::cout << std::boolalpha << "empty == " << !fn0 << std::endl;
        std::cout << std::boolalpha << "empty == " << !fn1 << std::endl;
        std::cout << "value == " << fn1(3) << std::endl;
    }
}