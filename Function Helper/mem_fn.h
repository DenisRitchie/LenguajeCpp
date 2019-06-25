#pragma once
#include <functional>
#include <iostream>

using namespace std;

namespace test::_mem_fn
{
    class funcs
    {
    public:
        void square(double value)
        {
            std::cout << value << "^2 == " << value * value << std::endl;
        }

        void product(double value1, double value2)
        {
            std::cout << value1 << "*" << value2 << " == " << value1 * value2 << std::endl;
        }
    };

    void run()
    {
        funcs funcs;
        std::mem_fn(&funcs::square)(funcs, 3.0);
        std::mem_fn(&funcs::product)(funcs, 3.0, 2.0);
    }
}




