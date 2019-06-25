#include <stdio.h>
#include <setjmpex.h>

// Exceptions in C
// http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html

// duff device
// http://www.catb.org/jargon/html/D/Duffs-device.html

#if 0 // Basic Try-Catch

#define TRY do { jmp_buf ex_buf__; if ( !setjmp(ex_buf__) ) {
#define CATCH } else {
#define ETRY } } while (0)
#define THROW longjmp(ex_buf__, 1)

void try_catch_test()
{
    //TRY
    //{
    //    printf_s("In Try Statement\n");
    //    THROW;
    //    printf_s("I do not appear\n");
    //}
    //CATCH
    //{
    //    printf_s("Got Exception!\n");
    //}
    //ETRY;

    // TRY
    do {
        jmp_buf ex_buf__;
        if (!setjmp(ex_buf__))
        {
            // TRY
            {
                printf_s("In Try Statement\n");
                longjmp(ex_buf__, 1); // THROW;
                printf_s("I do not appear\n");
            }
            // CATCH
        }
        else
        {
            // CATCH
            {
                printf_s("Got Exception!\n");
            }
            // ETRY
        }
    } while (0);
    // ETRY
}

#endif

#if 0 // Adding Exceptions

#define TRY do { jmp_buf ex_buf__; switch ( setjmp(ex_buf__) ) { case 0:
#define CATCH(X) break; case X:
#define ETRY } } while (0)
#define THROW(X) longjmp(ex_buf__, X)

constexpr auto FOO_EXCEPTION = (1);
constexpr auto BAR_EXCEPTION = (2);
constexpr auto BAZ_EXCEPTION = (3);

void try_catch_test()
{
    TRY
    {
        printf_s("In Try Statement\n");
        THROW(BAR_EXCEPTION);
        printf_s("I do not appear\n");
    }
    CATCH(FOO_EXCEPTION)
    {
        printf_s("Go Foo!\n");
    }
    CATCH(BAR_EXCEPTION)
    {
        printf_s("Go Bar!\n");
    }
    CATCH(BAZ_EXCEPTION)
    {
        printf_s("Go Baz!\n");
    }
    ETRY;
}

#endif

#if 0 // Adding Finally-Block

// The Need for the FINALLY Block
// Error to remove FINALLY Block

#define TRY do { jmp_buf ex_buf__; switch ( setjmp(ex_buf__) ) { case 0: while (1) {
#define CATCH(X) break; case X:
#define FINALLY break; } default:
#define ETRY } } while (0)
#define THROW(X) longjmp(ex_buf__, X)

constexpr auto FOO_EXCEPTION = (1);
constexpr auto BAR_EXCEPTION = (2);
constexpr auto BAZ_EXCEPTION = (3);

void try_catch_test()
{
    TRY
    {
        printf_s("In Try Statement\n");
        THROW(BAR_EXCEPTION);
        printf_s("I do not appear\n");
    }
    CATCH(FOO_EXCEPTION)
    {
        printf_s("Go Foo!\n");
    }
    CATCH(BAR_EXCEPTION)
    {
        printf_s("Go Bar!\n");
    }
    CATCH(BAZ_EXCEPTION)
    {
        printf_s("Go Baz!\n");
    }
    FINALLY
    {
        printf_s("...et in arcadia Ego\n");
    }
    ETRY;
}

#endif

#if 0

#define TRY do { jmp_buf ex_buf__; switch( setjmp(ex_buf__) ) { case 0: while (1) {
#define CATCH(x) break; case x:
#define FINALLY break; } default: {
#define ETRY break; } } } while(0)
#define THROW(x) longjmp(ex_buf__, x)

constexpr auto FOO_EXCEPTION = (1);
constexpr auto BAR_EXCEPTION = (2);
constexpr auto BAZ_EXCEPTION = (3);

void try_catch_test()
{
    TRY
    {
       printf("In Try Statement\n");
       THROW( BAZ_EXCEPTION );
       printf("I do not appear\n");
    }
    /*CATCH( FOO_EXCEPTION )
    {
        printf("Got Foo!\n");
    }
    CATCH( BAR_EXCEPTION )
    {
        printf("Got Bar!\n");
    }
    CATCH( BAZ_EXCEPTION )
    {
        printf("Got Baz!\n");
    }*/
    FINALLY
    {
       printf("...et in arcadia Ego\n");
    }
    ETRY;
}

#endif

/* Copyright (C) 2009-2015 Francesco Nidito
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _TRY_THROW_CATCH_H_
#define _TRY_THROW_CATCH_H_

#include <stdio.h>
#include <setjmp.h>

 /* For the full documentation and explanation of the code below, please refer to
  * http://www.di.unipi.it/~nids/docs/longjump_try_trow_catch.html
  */

#define TRY do { jmp_buf ex_buf__; switch( setjmp(ex_buf__) ) { case 0: while(1) {
#define CATCH(x) break; case x:
#define FINALLY break; } default: {
#define ETRY break; } } }while(0)
#define THROW(x) longjmp(ex_buf__, x)

#endif /*!_TRY_THROW_CATCH_H_*/

void try_catch_test()
{
    TRY
    {
        printf_s("Dentro de Try V1!\n");
    }
    FINALLY
    {
        printf_s("Dentro de Finally V1!\n");
    }
    ETRY;

    TRY
    {
        printf_s("Dentro de Try V2!\n");
        THROW( 10 );
    }
    CATCH( 10 )
    {
        printf_s("Dentro de Catch V1!\n");
    }
    ETRY;

    TRY
    {
        printf_s("Dentro de Try V3!\n");
        THROW( 15 );
    }
    CATCH( 15 )
    {
        printf_s("Dentro de Catch V2!\n");
    }
    FINALLY
    {
        printf_s("Dentro de Finally V2!\n");
    }
    ETRY;
}
