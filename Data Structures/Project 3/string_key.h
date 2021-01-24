#ifndef __STRING_KEY_H__
#define __STRING_KEY_H__

#include <stdlib.h>

size_t hash29(const void *key);

void *duplicate(const void *key);

int compare_keys(const void *k1, const void *k2);

#endif
