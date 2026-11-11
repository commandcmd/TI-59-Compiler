#ifndef GLOBAL_HELPERS_H
#define GLOBAL_HELPERS_H

#include <stddef.h>

/*
 * Non null-terminated string compare function.
 * This function compares a non null-terminated string
 * against a null-terminated one, returning a value different
 * from zero when the two strings' contents match.
*/
int nnt_strcmp(const char* str, const char* nnt_compare, size_t nnt_size);

#endif