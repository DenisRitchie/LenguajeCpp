#pragma once
#include <functional>
#include <iostream>

namespace test::_invoke
{
    struct demo
    {
        int m_value;

        demo(int const value) : m_value{ value } 
        {
        }

        void operator()(int const value1, int const value2) const
        {
            std::cout << "demo operator(" << value1 << ", " << value2 << ") is " << value1 * value2 << std::endl;
        }

        void difference(int const value)
        {
            std::cout << "demo.difference(" << value << ") is " << value - 1 << std::endl;
        }
    };

    void divisible_by_3(int const value)
    {
        std::cout << value << (value % 3 == 0 ? " is" : " isn\'t") << " divisible by 3.\n";
    }

    void run()
    {
        demo d{ 42 };
        demo* pd{ &d };

        void (demo::*pmf)(int const value) = &demo::difference;
        int demo::*pmd = &demo::m_value;

        // invoke a funtion object, like calling d(3, -7)
        std::invoke(d, 3, -7);

        // invoke a member function, like calling 
        // d.difference(29) or (d.*pmf)(29) or (pd->*pmf)(13)
        std::invoke(&demo::difference, d, 29);
        std::invoke(pmf, pd, 13);

        // invoke a data member, like access to d.m_value or d.*pmd
        std::cout << "d.m_value: " << std::invoke(&demo::m_value, d) << "\n";
        std::cout << "pd->m_value: " << std::invoke(pmd, pd) << "\n";

        // invoke a stand-alone (free) function
        std::invoke(divisible_by_3, 43);

        // invoke lambda
        auto divisible_by_7 = [](int const value) -> void
        {
            std::cout << value << (value % 7 ? " is" : " isn't") << " divisible by 7.\n";
        };

        std::invoke(divisible_by_7, 42);
    }
}