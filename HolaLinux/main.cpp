#include <stdio.h>
#include <setjmp.h>

// https://www.embeddedrelated.com/showarticle/455.php

#if __linux__

typedef struct
{
    jmp_buf callee_context;
    jmp_buf caller_context;
} coroutine_t;

typedef void (*func_t)(void*);

void start(coroutine_t* c, func_t, void* arg, void* sp);
void yield(coroutine_t* c);
int next(coroutine_t* c);

enum { WORKING = 1, DONE };

void yield(coroutine_t* c)
{
    if (!setjmp(c->callee_context)) {
        longjmp(c->caller_context, WORKING);
    }
}

int next(coroutine_t* c)
{
    int ret = setjmp(c->caller_context);

    if (!ret) {
        longjmp(c->callee_context, 1);
    }
    else {
        return ret == WORKING;
    }
}

typedef struct
{
    coroutine_t* c;
    func_t f;
    void* arg;
    void* old_sp;
    void* old_fp;
} start_params;

#define get_sp(p) \
  asm volatile("movq %%rsp, %0" : "=r" (p))

#define get_fp(p) \
  asm volatile("movq %%rbp, %0" : "=r" (p))

#define set_sp(p) \
  asm volatile("movq %0, %%rsp" : : "r" (p))

#define set_fp(p) \
  asm volatile("movq %0, %%rbp" : : "r" (p))

enum { FRAME_SZ = 5 }; // fairly arbitrary

void start(coroutine_t* c, func_t f, void* arg, void* sp)
{
    start_params* p = ((start_params*)sp) - 1;

    // save params before stack switching
    p->c = c;
    p->f = f;
    p->arg = arg;
    get_sp(p->old_sp);
    get_fp(p->old_fp);

    set_sp(p - FRAME_SZ);
    set_fp(p); // effectively clobbers p
    // (and all other locals)
    get_fp(p); // …so we read p back from $fp

    // and now we read our params from p
    if (!setjmp(p->c->callee_context))
    {
        set_sp(p->old_sp);
        set_fp(p->old_fp);
        return;
    }

    (*p->f)(p->arg);
    longjmp(p->c->caller_context, DONE);
}

/////////////////////////////////////////////////////

typedef struct
{
    coroutine_t* c;
    int max_x, max_y;
    int x, y;
} iter;

void iterate(void* p)
{
    iter* it = (iter*)p;
    int x, y;
    for (x = 0; x < it->max_x; x++)
    {
        for (y = 0; y < it->max_y; y++)
        {
            it->x = x;
            it->y = y;
            yield(it->c);
        }
    }
}

constexpr auto N = 1024;

void test_coroutines()
{
    coroutine_t c;
    int stack[N];
    iter it = { &c, 2, 2 };
    start(&c, &iterate, &it, stack + N);

    while (next(&c))
    {
        printf("%d %d\n", it.x, it.y);
    }
}

int main()
{
    puts("\n");
    test_coroutines();
    puts("\n\n");
    return 0;
}

#endif
