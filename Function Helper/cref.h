#pragma once
#include <functional>
#include <iostream>

namespace test::_cref
{
    constexpr int negation(int value) noexcept
    {
        return (-value);
    }

    void run()
    {
        int i = 1;
        std::cout << "i = " << i << std::endl;
        std::cout << "cref(i)" << std::cref(i) << std::endl;
        std::cout << "cref(negation)(i) = " << std::cref(negation)(i) << std::endl;
        std::cout << std::endl;
        std::reference_wrapper<const int> ref_i = std::cref(i);
        _ASSERT(std::addressof(ref_i.get()) == std::addressof(i));
    }
}