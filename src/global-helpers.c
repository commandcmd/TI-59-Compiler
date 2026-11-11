#include "global-helpers.h"

/*
 * Non null-terminated string compare function.
 * This function compares a non null-terminated string
 * against a null-terminated one, returning a value different
 * from zero when the two strings' contents match.
*/
int nnt_strcmp(const char* str, const char* nnt_compare, size_t nnt_size) {
    size_t str_counter = 0;
    while(1) {
        if(str[str_counter] == '\0' || str_counter >= nnt_size) {
            /* BOTH of these conditions have to be met concurrently
             * for the strings to be of the same size thus equal.*/
            if(str[str_counter] != '\0' || str_counter < nnt_size) return 0;
            else return 1;
        }

        if(str[str_counter] != nnt_compare[str_counter]) return 0;

        str_counter++;
    }
}