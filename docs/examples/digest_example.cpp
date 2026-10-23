/**
 * @example digest_example.cpp
 *
 * Calculate the MD5 and SHA-1 digests of a message with the xStl digest
 * classes. The expected values are the RFC 1321 and FIPS 180 test vectors.
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/enc/digest/md5.h"
#include "xStl/enc/digest/sha1.h"
#include <stdio.h>

static void printHex(const char* name, const cBuffer& digest)
{
    printf("%s = ", name);
    for (uint i = 0; i < digest.getSize(); i++)
        printf("%02x", digest[i]);
    printf("\n");
}

int main()
{
    // MD5("abc") = 900150983cd24fb0d6963f7d28e17f72
    cMD5 md5;
    md5.update("abc", 3);
    printHex("MD5(abc)  ", md5.digest());

    // SHA1("abc") = a9993e364706816aba3e25717850c26c9cd0d89d
    cSHA1 sha1;
    sha1.update("abc", 3);
    printHex("SHA1(abc) ", sha1.digest());
    return 0;
}
