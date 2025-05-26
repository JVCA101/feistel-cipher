#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Type definitions for 32-bit and 64-bit words
typedef uint32_t word32;
typedef uint64_t word64;

//***************** AUXILIARY FUNCTIONS *****************//
/* Makes an Exclusive OR between 2 words of 32 bits each */
word32 XOR(word32 a, word32 b) {
    return a ^ b;
}

/* Makes a Circular Right Shift on the bits of a 32 bits word */
word32 rshift_circular32(word32 x) {
    return (x >> 1) | ((x & 1) << 31); // a bitwise OR with the rightshifted 1 time value and the most right bit shifted 31 times to the left
}

/* Makes a Circular Left Shift on the bits of a 32 bits word */
word32 lshift_circular32(word32 x) {
    return (x << 1) | ((x & (1 << 31)) >> 31); // a bitwise OR with the leftshifted 1 time value and the most left bit shifted 31 times to the right
}

/* Generates a key of 32 bits for the feistel cipher */
word32 generate_key(word32 k) {
    return rshift_circular32(k);
}

/* Generates the inverse key for the feistel cipher */
word32 generate_key_inverse(word32 k) {
    return lshift_circular32(k);
}

/* Compute function for the feistel cipher */
word32 F(word32 x, word32 k) {
    return XOR(x, k);
}

/* Compute the inverse function for the feistel cipher */
word32 F_inverse(word32 x, word32 k) {
    return XOR(x, k);
}
//***************** END - AUXILIARY FUNCTIONS *****************//


//***************** FEISTEL FUNCTIONS *****************//

/**
 * @brief Encrypts a 64-bit word using the feistel cipher.
 * 
 * @param left the left 32 bits of the 64-bit word
 * @param right the right 32 bits of the 64-bit word
 * @param k key to encrypt the word
 * @param encrypted_left pointer to store the encrypted left
 * @param encrypted_right pointer to store the encrypted right
 * @return word32 last key used, can be used for decryption
 */
word32 feistel_cipher(word32 left, word32 right, word32 k, word32* encrypted_left, word32* encrypted_right)
{
    // Initialize the key and old values
    word32 key = generate_key(k), 
    old_left   = left, 
    old_right  = right;

    // 16 rounds of encryption
    for(size_t i = 0; i < 16; i++)
    {
        // update left
        left  = old_right;
        // update right using the exclusive OR between the previous left and F
        right = XOR(old_left, F(old_right, key));

        // If it's not the last round, continue normally
        if(i == 15)
            break;
        
        // Generate the next key
        key = generate_key(key);

        // Update old values
        old_left  = left;
        old_right = right;
    }

    // Store the encrypted values and return the lalst key
    *encrypted_left  = right;
    *encrypted_right = left;
    return key;
}

/**
 * @brief Decrypts a 64-bit word using the feistel cipher
 * 
 * @param left the left 32 bits of the 64-bit word
 * @param right the right 32 bits of the 64-bit word
 * @param k key to decrypt the word
 * @param decrypted_left pointer to store the decrypted left
 * @param decrypted_right pointer to store the decrypted right
 */
void feistel_cipher_inverse(word32 left, word32 right, word32 k, word32* decrypted_left, word32* decrypted_right)
{
    // Initialize the key and old values
    word32 key = k,
    old_left   = left,
    old_right  = right;

    // 16 rounds of decryption
    for(size_t i = 0; i < 16; i++)
    {
        // update left
        left = old_right;
        // update right using an exclusive OR between the previous left and the inverse of F
        right = XOR(old_left, F_inverse(old_right, key));

        // If it's not the last round, continue normally
        if(i == 15)
            break;
        
        // Generate the next key
        key = generate_key_inverse(key);

        // update old values
        old_left  = left;
        old_right = right;
    }

    // Store the decrypted values, swapping left and right
    *decrypted_left  = right;
    *decrypted_right = left;
}
//***************** END - FEISTEL FUNCTIONS *****************//

//***************** MAIN FUNCTION *****************//
int main(int argc, char const *argv[])
{
    // Verify if the call has the key as an argument
    if(argc < 2 || argv[1] == NULL || argv[1][0] == '\0') {
        printf("Usage: %s <key>\n", argv[0]);
        return 1;
    }
    
    // Declare variables
    word32 k, right, left;
    word64 whole_word;
    
    // Convert the key from string to a 32-bit word
    k = (word32)strtoul(argv[1], NULL, 10);
    if(k == 0) {
        printf("Invalid key. Please enter a valid 32-bit unsigned integer.\n");
        return 1;
    }

    // read a 64-bit string from the keyboard
    char whole_word_str[9];
    printf("Enter a 64-bit number(8 bytes): ");
    fgets(whole_word_str, sizeof(whole_word_str), stdin);
    whole_word = (word64)(whole_word_str);

    // Split the 64-bit word into two 32-bit words
    left = (word32)(whole_word >> 32);
    right = (word32)(whole_word & 0xFFFFFFFF);
    printf("Initial Values:\n Left: %u, Right: %u, Key: %u\n", left, right, k);

    
    //* Encryption
    word32 encrypted_left, encrypted_right;
    k = feistel_cipher(left, right, k, &encrypted_left, &encrypted_right);
    printf("Encrypted Values:\n Left: %u, Right: %u, Key: %u\n", encrypted_left, encrypted_right, k);
    
    //* Decryption
    word32 decrypted_left, decrypted_right;
    feistel_cipher_inverse(encrypted_left, encrypted_right, k, &decrypted_left, &decrypted_right);
    printf("Decrypted Values:\n Left: %u, Right: %u\n", decrypted_left, decrypted_right);

    // Combine the decrypted left and right parts into a 64-bit word
    word64 decrypted_whole_word = ((word64)decrypted_left << 32) | (word64)decrypted_right;
    printf("Decrypted 64-bit number: %lu\n", decrypted_whole_word);
    printf("Decrypted in string: %s\n", (char*)decrypted_whole_word);

    return 0;
}
