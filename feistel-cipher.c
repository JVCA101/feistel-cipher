#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Macros for 32-bit and 64-bit words
#define word32 uint32_t
#define word64 uint64_t

//***************** AUXILIARY FUNCTIONS *****************//
word32 XOR(word32 a, word32 b) {
    return a ^ b;
}

word32 rshift_circular32(word32 x) {
    return (x >> 1) | ((x & 1) << 31);
}

word32 lshift_circular32(word32 x) {
    return (x << 1) | ((x & (1 << 31)) >> 31);
}

word32 generate_key(word32 k) {
    return rshift_circular32(k);
}

word32 generate_key_inverse(word32 k) {
    return lshift_circular32(k);
}

word32 F(word32 x, word32 k) {
    return XOR(x, k);
}

word32 F_inverse(word32 x, word32 k) {
    return XOR(x, k);
}
//***************** END - AUXILIARY FUNCTIONS *****************//


//***************** FEISTEL FUNCTIONS *****************//

word32 feistel_cipher(word32 left, word32 right, word32 k, word32* encrypted_left, word32* encrypted_right)
{
    word32 key = generate_key(k), 
    old_left   = left, 
    old_right  = right;

    for(size_t i = 0; i < 16; i++)
    {
        left  = old_right;
        right = XOR(old_left, F(old_right, key));

        if(i == 15)
            break;
        
        key = generate_key(key);

        old_left  = left;
        old_right = right;
    }

    *encrypted_left  = right;
    *encrypted_right = left;
    return key;
}

void feistel_cipher_inverse(word32 left, word32 right, word32 k, word32* decrypted_left, word32* decrypted_right)
{
    word32 key = k,
    old_left   = left,
    old_right  = right;

    
    for(size_t i = 0; i < 16; i++)
    {
        left = old_right;
        right = XOR(old_left, F_inverse(old_right, key));

        if(i == 15)
            break;
        
        key = generate_key_inverse(key);

        old_left  = left;
        old_right = right;
    }

    *decrypted_left  = right;
    *decrypted_right = left;
}
//***************** END - FEISTEL FUNCTIONS *****************//

//***************** MAIN FUNCTION *****************//
int main(int argc, char const *argv[])
{
    word32 k = 30, right, left;
    word64 whole_word = 10988738201;

    // char whole_word_str[8];
    // printf("Enter a 64-bit number (8 bytes): ");
    // fgets(whole_word_str, sizeof(whole_word_str), stdin);
    // whole_word = (word64)(whole_word_str);

    left = (word32)(whole_word >> 32);
    right = (word32)(whole_word & 0xFFFFFFFF);
    word32 encrypted_left, encrypted_right;
    word32 decrypted_left, decrypted_right;

    printf("Initial Values:\n Left: %u, Right: %u, Key: %u\n", left, right, k);

    k = feistel_cipher(left, right, k, &encrypted_left, &encrypted_right);
    printf("Encrypted Values:\n Left: %u, Right: %u, Key: %u\n", encrypted_left, encrypted_right, k);
    feistel_cipher_inverse(encrypted_left, encrypted_right, k, &decrypted_left, &decrypted_right);
    printf("Decrypted Values:\n Left: %u, Right: %u\n", decrypted_left, decrypted_right);

    // word64 decrypted_whole_word = ((word64)decrypted_left << 32) | (word64)decrypted_right;
    // printf("Decrypted 64-bit number: %lu\n", decrypted_whole_word);
    // printf("Decrypted in string: %s\n", (char*)decrypted_whole_word);

    return 0;
}
