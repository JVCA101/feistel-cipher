#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define word32 uint32_t
#define word64 uint64_t

#define XOR(a, b) (a ^ b)
#define CIRCULAR_RSHIFT(x) ((x >> 1) | ((x & 1) << 31))
#define CIRCULAR_LSHIFT(x) ((x << 1) | ((x & (1 << 31)) >> 31))

#define F_(x, k)        (XOR(CIRCULAR_LSHIFT(x), k))
#define F_inverse(x, k) (CIRCULAR_RSHIFT(XOR(x, k)))

void generate_keys(word32* keys, const size_t num_keys)
{
    printf("Key 0: %d\n", keys[0]);
    for(size_t i = 1; i < num_keys; i++){
        keys[i] = CIRCULAR_RSHIFT(keys[i - 1]);
        printf("Key %ld: %d\n", i, keys[i]);
    }
}

word32 feistel_cipher(word32 old_left, word32 old_right, word32 k, word32 *left, word32 *right)
{

    word32 keys[16];
    keys[0] = k;
    generate_keys(keys, 16);

    for(size_t i = 1; i < 16; i++)
        printf("Key %ld decrypted: %d\n", (i-1), CIRCULAR_LSHIFT(keys[i]));

    f(old_right, k);
}

int main(int argc, char const *argv[])
{
    feistel_cipher(12, 13, 30, NULL, NULL);
    return 0;
}
