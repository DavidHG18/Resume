#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "string_helpers.h"

void string_array_free(char **arr, size_t n)
{
  if (arr != NULL)
    {
      for (size_t i = 0; i < n; i++)
	{
	  free(arr[i]);
	}
      free(arr);
    }
}

size_t string_hash_java(const void *ptr)
{
  const char *s = ptr;
  
  // from Java's String.hashCode()
  // https://docs.oracle.com/javase/7/docs/api/java/lang/String.html#hashCode()
  if (s == NULL)
    {
      return 0;
    }
  
  size_t power = 31;
  size_t hash = 0;
  const char *p = s + strlen(s) - 1; // start at end of string
  while (p >= s)
    {
      hash += power * *p;
      power *= 31;
      p--;
    }

  return hash;
}

void string_array_fprintf(FILE *fp, char **arr, size_t n)
{
  if (arr != NULL)
    {
      for (int i = 0; i < n; i++)
	{
	  fprintf(fp, "%s\n", arr[i]);
	}
    }
}

void *string_copy(const void *s)
{
  char *copy = malloc(strlen(s) + 1);
  if (copy != NULL)
    {
      strcpy(copy, s);
    }
  return copy;
}

int string_compare(const void *p1, const void *p2)
{
  const char *s1 = p1;
  const char *s2 = p2;

  return strcmp(s1, s2);
}

  
