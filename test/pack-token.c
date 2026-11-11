#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define NUMBER_TOKEN 0xe739ce739ce739c
#define INVALID_TOKEN 0xEF7BDEF7BDEF7BD
#define PROG_POINTER_TOKEN 0xF7BDEF7BDEF7BDE
#define VARIABLE_TOKEN     0xD6B5AD6B5AD6B5A

/*
 * Given a token to pack, this function returns a
 * 64bit integer value containing all characters
 * that the function can fit in there through optimization.
 * Note that if the token is longer than 12 characters, the
 * function will cut it short.
 *
 * In case the token is too long, then UINT64_MAX is returned.
*/
uint64_t pack_token(const char* token, size_t bufsize) {
    uint64_t output = 0;

    if(bufsize == 0) return INVALID_TOKEN;

    // Check if the current token is a program pointer before checking the size
    if(token[0] == ':') return PROG_POINTER_TOKEN;
    // The dollar sign is the variable identifier and must be at the start
    if(token[0] == '$') return VARIABLE_TOKEN;

    // Check the length of the token passed, if too long return all f's
    if(bufsize > 12) return UINT64_MAX;

    /* 
     * Iterate through each character and pack it in 
     * the output.
     * A maximum of 12 characters can fit in the 64bit
     * space.
     */
    for(int i = 0; i < bufsize; i++) {
        // Pack the current character
        char current_char = token[i];
        if(current_char >= 'A' && current_char <= 'Z')
            current_char -= 'A'; // A-Z are the numbers 0-25
        else if(current_char >= '0' && current_char <= '9' || current_char == '.')
            return NUMBER_TOKEN; // In case of any number return the number token
        else return INVALID_TOKEN;

        // Cast the last 5 bits of the char into the output
        output = output << 5;
        output |= (current_char & 31);
    }

    return output;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "Missing string!\nUsage: pack-token [string-to-pack]");
        return 10;
    }
    if(argc > 2) {
        fprintf(stderr, "Invalid input!\nUsage: pack-token [string-to-pack]");
        return 20;
    }

    uint64_t packed_token = pack_token(argv[1], strlen(argv[1]));

    if(packed_token == UINT64_MAX) {
        printf("The string passed is too long to be packed into a 64bit token!\nThe packed token will be UINT64_MAX\n\n");
    } else if(packed_token == NUMBER_TOKEN) {
        printf("A string which contains numbers has been entered\nThe packed token will be NUMBER_TOKEN\n\n");
    } else if(packed_token == INVALID_TOKEN) {
        printf("A string with invalid characters has been entered\nThe packed token will be INVALID_TOKEN\n\n");
    } else if(packed_token == PROG_POINTER_TOKEN) {
        printf("A string with a program pointer has been entered\nThe packed token will be PROG_POINTER_TOKEN\n\n");
    } else if(packed_token == VARIABLE_TOKEN) {
        printf("A string with a variable has been entered\nThe packed token will be VARIABLE_TOKEN\n\n");
    }

    printf("Packed Token: 0x%llX\n", packed_token);

    return 0;
}
