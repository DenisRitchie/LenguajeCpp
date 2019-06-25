int main()
{
    extern int process_data();
    extern void longjmp_and_setjmp_test1();
    extern void try_catch_test();
    extern void setjmp_example();
    extern void test_coroutines();

    //process_data();
    //longjmp_and_setjmp_test1();
    //try_catch_test();
    setjmp_example();
    //test_coroutines(); // only linux

    return 0;
}