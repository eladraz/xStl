/**
 * @example exceptions_example.cpp
 *
 * Throw and catch an xStl exception with the XSTL_THROW / XSTL_CATCH macros.
 */
#include "xStl/types.h"
#include "xStl/exceptions.h"
#include "xStl/except/exception.h"
#include "xStl/data/array.h"
#include <stdio.h>

int main()
{
    cArray<int> array(2);
    XSTL_TRY
    {
        // Accessing an element out of the array range throws an exception
        array[5] = 1;
    }
    XSTL_CATCH(cException& e)
    {
        printf("caught an xStl exception, id = %u\n", (unsigned)e.getID());
    }
    return 0;
}
