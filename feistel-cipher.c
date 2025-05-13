#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define word32 uint32_t
#define word64 uint64_t
#define XOR(a, b) (a ^ b)

word32 f(word32 x, word32 k)
{
    return XOR(x, k);
}

word32 feistel_cipher(word32 old_left, word32 old_right, unsigned int k, word32 *left, word32 *right)
{
    unsigned int new_left, new_right;
    new_left = old_right;
    new_right = f(right, k) ^ old_left;
}
