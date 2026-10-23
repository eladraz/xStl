/**
 * @example endian_example.cpp
 *
 * Serialize numbers into a byte buffer with the little-endian and big-endian
 * helpers of xStl.
 */
#include "xStl/types.h"
#include "xStl/data/endian.h"
#include <stdio.h>

int main()
{
    uint8 buffer[4];

    // 0x12345678 is stored as 78 56 34 12 in little-endian
    cLittleEndian::writeUint32(buffer, 0x12345678);
    printf("little-endian: %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3]);

    // ... and as 12 34 56 78 in big-endian
    cBigEndian::writeUint32(buffer, 0x12345678);
    printf("big-endian:    %02x %02x %02x %02x\n", buffer[0], buffer[1], buffer[2], buffer[3]);

    // Read it back, choosing the byte order at run-time
    printf("read back:     %08x\n", (unsigned)cEndian::readUint32(buffer, false));
    return 0;
}
