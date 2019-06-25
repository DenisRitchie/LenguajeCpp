#include <stdio.h>
#include <setjmp.h>

// https://es.wikipedia.org/wiki/Setjmp.h

static jmp_buf buf;

void second()
{
    printf("%s\n", __func__); // Impresiones
    longjmp(buf, 1);          // salta de nuevo de donde fue llamado setjmp
}

void first()
{
    second();
    printf_s("%s\n", __func__); // No se llama
}

void setjmp_example()
{
    if (!setjmp(buf))
    {
        first(); // Cuando se ejecuta setjmp devuelve 0
    }
    else
    {
        printf_s("%S\n", __FUNCTIONW__); // Impresiones
    }
}

