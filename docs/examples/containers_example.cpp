/**
 * @example containers_example.cpp
 *
 * Use the xStl containers: cArray, cList, cHash and cSmartPtr.
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/data/list.h"
#include "xStl/data/hash.h"
#include "xStl/data/string.h"
#include "xStl/data/smartptr.h"
#include <stdio.h>

int main()
{
    // A dynamic array
    cArray<int> array(3);
    array[0] = 10; array[1] = 20; array[2] = 30;
    array.append(array);
    printf("array size: %u, array[5] = %d\n", (unsigned)array.getSize(), array[5]);

    // A linked list: append, insert at the front and iterate
    cList<int> list;
    list.append(2);
    list.append(3);
    list.insert(1);
    for (cList<int>::iterator i = list.begin(); i != list.end(); ++i)
        printf("list item: %d\n", *i);

    // A hash table from keys to values
    cHash<int, int> hash;
    hash.append(7, 49);
    printf("hash has key 7: %d, value %d\n", (int)hash.hasKey(7), hash[7]);

    // A reference counted pointer: the object is deleted by the last owner
    cSmartPtr<int> first(new int(5));
    cSmartPtr<int> second(first);
    printf("shared value: %d\n", *second);
    return 0;
}
