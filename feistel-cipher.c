#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define word32 uint32_t
#define word64 uint64_t

#define XOR(a, b) (a ^ b) // XOR operation

#define CIRCULAR_RSHIFT32(x) ((x >> 1) | ((x & 1) << 31))          // it does a bitwise OR with the right shifted value and the left  shifted 31 times value
#define CIRCULAR_LSHIFT32(x) ((x << 1) | ((x & (1 << 31)) >> 31))  // it does a bitwise OR with the left  shifted value and the right shifted 31 times value

word32 generate_key(word32 k) {
    return CIRCULAR_RSHIFT32(k);
}

word32 generate_key_inverse(word32 k) {
    return CIRCULAR_LSHIFT32(k);
}

word32 F(word32 x, word32 k) {
    return XOR(CIRCULAR_LSHIFT32(x), k);
}

word32 F_inverse(word32 x, word32 k) {
    return CIRCULAR_RSHIFT32(XOR(x, k));
}

void swap(word32* a, word32* b)
{
    word32 temp = *a;
    *a = *b;
    *b = temp;
}

word32 feistel_cipher(word32 left, word32 right, word32 k, word32* new_left, word32* new_right)
{
    word32 key = k, 
    old_left   = left, 
    old_right  = right;
    for(size_t i = 0; i < 16; i++)
    {
        *new_left  = old_right;
        *new_right = XOR(old_left, F(old_right, key));
        if(i == 15)
            break;
        
        key = generate_key(key);

        old_left  = *new_left;
        old_right = *new_right;

        printf("Round %zu: left: %u, right: %u, key: %u\n", i + 1, old_left, old_right, key);
    }
    
    swap(new_left, new_right);
    printf("Final Round: left: %u, right: %u, key: %u\n", *new_left, *new_right, key);
    return key;
}

void feistel_cipher_inverse(word32 left, word32 right, word32 k, word32* decrypted_left, word32* decrypted_right)
{
    // swap(decrypted_left, decrypted_right);
    word32 key = k,
    old_left   = left,
    old_right  = right;

    printf("Decryption: left: %u, right: %u, key: %u\n", old_left, old_right, key);

    for(size_t i = 0; i < 16; i++)
    {
        *decrypted_left  = old_right;
        *decrypted_right = XOR(old_left, F_inverse(old_right, key));
        
        key = generate_key_inverse(key);

        if(i == 15)
            break;
        printf("Round %zu: left: %u, right: %u, key: %u\n", i + 1, *decrypted_left, *decrypted_right, key);

        old_left  = *decrypted_left;
        old_right = *decrypted_right;
    }

    printf("Final Round: left: %u, right: %u, key: %u\n", *decrypted_left, *decrypted_right, key);
}

int main(int argc, char const *argv[])
{
    word64 whole_word = 1050;
    word32 left = (word32)(whole_word >> 32);
    word32 right = (word32)(whole_word & 0x00000000FFFFFFFF);
    printf("Original: %u %u\n", left, right);
    printf("Original whole word: %lu\n", whole_word);
    word32 k = 30;
    word32 new_left = 0;
    word32 new_right = 0;
    word32 output_key = feistel_cipher(left, right, k, &new_left, &new_right);

    printf("Encrypted: %u %u\n", new_left, new_right);
    printf("Key: %u\n", output_key);

    word32 decrypted_left = 0;
    word32 decrypted_right = 0;
    feistel_cipher_inverse(new_left, new_right, output_key, &decrypted_left, &decrypted_right);
    word64 whole_decrypted_word = ((word64)decrypted_left << 32) | ((word64)decrypted_right);
    printf("Decrypted: %u %u\n", decrypted_left, decrypted_right);
    printf("Decrypted whole word: %lu\n", whole_decrypted_word);

    return 0;
}
