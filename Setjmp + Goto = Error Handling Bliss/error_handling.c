//#include <varargs.h>
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdlib.h>
#include <conio.h>

static jmp_buf error_jmp;

static void parse_error(char const* format, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, _countof(buf), format, args);
    buf[_countof(buf) - 1] = 0;
    fprintf(stderr, "error: %s\n", buf);
    va_end(args);
    longjmp(error_jmp, 1);
}

int process_data()
{
    volatile int error_count = 0;
retry:

    if (0 != setjmp(error_jmp))
    {
        ++error_count;
        goto retry;
    }

    int value;
    printf("write data: ");
    scanf_s("%d", &value);

    if (value < 1)
        parse_error("the expected value must be greater than zero");

    printf("data: %d", value);

    return error_count;
}
