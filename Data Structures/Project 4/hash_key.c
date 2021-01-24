#include <string.h>
#include <stdio.h>

#include "hash_key.h"

int string_hash(const void *key)
{
  const char *s = key;
  int sum = 0;
  int factor = 29;
  while (s != NULL && *s != '\0')
    {
      sum += *s * factor;
      s++;
      factor *= 29;
    }

  return sum;
}

int int_hash(const void *x)
{
  // from StackOverflow user Thomas Mueller
  // https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
  unsigned int z = *(unsigned int *)x; // original was written for unsigned ints
  z = ((z >> 16) ^ z) * 0x119de1f3;
  z = ((z >> 16) ^ z) * 0x119de1f3;
  z = (z >> 16) ^ z;
  return (int)z;
}


void *duplicate(const void *key)
{
  char *s = malloc(strlen(key) + 1);
  if (s != NULL)
    {
      strcpy(s, key);
    }
  return s;
}

void *int_duplicate(const void *key)
{
  int *i = malloc(sizeof(int));
  if (i != NULL)
    {
      *i = *(int *)key;
    }
  return i;
}

int compare_int_keys(const void *k1, const void *k2)
{
  return *(int *)k1 - *(int *)k2;
}

int compare_string_keys(const void *k1, const void *k2)
{
  return strcmp(k1, k2);
}
