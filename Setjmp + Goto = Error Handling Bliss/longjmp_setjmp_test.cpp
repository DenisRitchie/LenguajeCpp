#include <iostream>
#include <csetjmp>

static std::jmp_buf jump_buffer;

[[noreturn]] static void function_test(int count)
{
    std::cout << __func__ << "(" << count << ") called\n";
    std::longjmp(jump_buffer, count + 1); // setjmp() will return count + 1
}

void longjmp_and_setjmp_test1()
{
    volatile int count = 0;  // local variables must be volatile for setjmp

    if (setjmp(jump_buffer) != 9)
        function_test(count++); // this will cause setjmp() to exit
}