#include <stdlib.h>
#include <string.h>

#include "gmap_test_functions.h"

#define MAX_INT_DIGITS 10

char **make_words(const char *prefix, size_t n)
{
  size_t prefix_len = strlen(prefix);
  char **arr = malloc(sizeof(char *) * n);
  for (size_t i = 0; i < n; i++)
    {
      arr[i] = malloc(sizeof(char) * (prefix_len + MAX_INT_DIGITS + 1));
      sprintf(arr[i], "%s%lu", prefix, i);
    }

  return arr;
}

char **make_words_select(char * const *words, size_t *indices, size_t n)
{
  char **arr = malloc(sizeof(char *) * n);
  for (size_t i = 0; i < n; i++)
    {
      arr[i] = malloc(sizeof(char) * (strlen(words[indices[i]]) + 1));
      strcpy(arr[i], words[indices[i]]);
    }

  return arr;
}

char **make_words_concat(const char *prefix, const int *suffixes, size_t n)
{
  size_t prefix_len = strlen(prefix);
  char **arr = malloc(sizeof(char *) * n);
  for (size_t i = 0; i < n; i++)
    {
      arr[i] = malloc(sizeof(char) * (prefix_len + MAX_INT_DIGITS + 1));
      sprintf(arr[i], "%s%d", prefix, suffixes[i]);
    }

  return arr;
}

char **make_random_words(size_t len, size_t n)
{
  const char *alphabet = "ABCDEFGHIJKLMNIPQRSTUVWXYZ";
  size_t alphabet_len = strlen(alphabet);
  
  char **arr = malloc(sizeof(char *) * n);
  for (size_t i = 0; i < n; i++)
    {
      arr[i] = malloc(sizeof(char) * (len + 1));
      for (size_t j = 0; j < len; j++)
	{
	  arr[i][j] = alphabet[rand() % alphabet_len];
	}
      arr[i][len] = '\0';
    }
  
  return arr;
}

char **copy_words(char * const *words, size_t n)
{
  char **arr = malloc(sizeof(char *) * n);
  for (size_t i = 0; i < n; i++)
    {
      arr[i] = malloc(sizeof(char) * (strlen(words[i]) + 1));
      strcpy(arr[i], words[i]);
    }
  
  return arr;
}

void free_words(char **arr, size_t n)
{
  for (size_t i = 0; i < n; i++)
    {
      free(arr[i]);
    }
  free(arr);
}

void free_values(gmap *m, char **arr, size_t n)
{
  for (size_t i = 0; i < n; i++)
    {
      free(gmap_get(m, arr[i]));
    }
}

size_t java_hash_string(const void *key)
{
  const char *s = key;
  
  // from Java's String.hashCode()
  // https://docs.oracle.com/javase/7/docs/api/java/lang/String.html#hashCode()
  if (s == NULL)
    {
      return 0;
    }
  
  size_t power = 31;
  size_t hash = 0;
  const char *p = s + strlen(s) - 1;
  while (p >= s)
    {
      hash += power * *p;
      power *= 31;
      p--;
    }

  return hash;
}
