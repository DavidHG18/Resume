#ifndef __STRING_KEY_H__
#define __STRING_KEY_H__

#include <stdlib.h>

int string_hash(const void *key);
int int_hash(const void *x);

void *duplicate(const void *key);

int compare_int_keys(const void *k1, const void *k2);
int compare_string_keys(const void *k1, const void *k2);
void *int_duplicate(const void *key);

#endif
