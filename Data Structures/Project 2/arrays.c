#include <stdio.h>
#include <stdlib.h>

#include "arrays.h"

void arrays_print(FILE *out, size_t n, const int *a)
{
  if (out == NULL)
    {
      return;
    }

  if (a == NULL)
    {
      fprintf(out, "(null)");
      return;
    }

  fprintf(out, "[");
  for (size_t i = 0; i < n; i++)
    {
      if (i > 0)
        {
          fprintf(out, ", ");
        }
      fprintf(out, "%d", a[i]);
    }
  fprintf(out, "]");
}

void arrays_print_double(FILE *out, const char *fmt, size_t n, const double *a){
  if (out == NULL || fmt == NULL)
    {
      return;
    }

  if (a == NULL)
    {
      fprintf(out, "(null)");
      return;
    }

  fprintf(out, "[");
  for (size_t i = 0; i < n; i++)
    {
      if (i > 0)
        {
          fprintf(out, ", ");
        }
      fprintf(out, fmt, a[i]);
    }
  fprintf(out, "]");
}

int arrays_compare(size_t n, int *a1, int *a2)
{
  if (a1 == NULL && a2 == NULL)
    {
      return 0;
    }
  else if (a1 == NULL)
    {
      return -1;
    }
  else if (a2 == NULL)
    {
      return 1;
    }

  // sequential search for first difference
  size_t i = 0;
  while (i < n && a1[i] == a2[i])
    {
      i++;
    }

  if (i < n)
    {
      return a1[i] - a2[i];
    }
  else
    {
      return 0;
    }
}
