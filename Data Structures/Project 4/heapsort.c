#include "heapsort.h"

#include <string.h>

#define HEAP_PARENT(n) ((n - 1) / 2)
#define HEAP_LEFT(n) (n * 2 + 1)
#define HEAP_RIGHT(n) (n * 2 + 2)

/**
 * Swaps the memory pointed to by p1 with the memory pointed to by p2.
 *
 * @param p1 a pointer to at least elt_size bytes, non-NULL
 * @param p2 a pointer to at least elt_size bytes, non-NULL
 * @param temp a pointer to at least elt_size bytes, non-NULL
 */
void heap_swap(void *p1, void *p2, void *temp, size_t elt_size);

void heapsort(size_t n, size_t elt_size, void *arr, int (*compare)(const void *, const void *))
{
  // turn the array into a heap
  for (size_t i = 1; i < n; i++)
    {
      heap_reheap_up(i, i + 1, elt_size, arr, compare);
    }

  // make some temporary space for swapping
  void *temp = malloc(elt_size);
  if (temp == NULL)
    {
      return;
    }
  
  // repeatedly remove the top (max) elt from the heap, swap it with
  // the last elt in the heap, and shrink and reheapify the heap
  for (size_t i = 1; i < n; i++)
    {
      heap_swap(arr, (char *)arr + elt_size * (n - i), temp, elt_size);

      heap_reheap_down(0, n - i, elt_size, arr, compare);
    }

  free(temp);
}

void heap_reheap_up(size_t i, size_t n, size_t elt_size, void *arr, int (*compare)(const void *, const void *))
{
  void *temp = malloc(elt_size);
  if (temp == NULL)
    {
      return;
    }
  
  while (i > 0 && compare((char *)arr + elt_size * i, (char *)arr + elt_size * HEAP_PARENT(i)) > 0) // if greater than parent
    {
      heap_swap((char *)arr + elt_size * i, (char *)arr + elt_size * HEAP_PARENT(i), temp, elt_size);
      i = HEAP_PARENT(i);
    }

  free(temp);
}

void heap_reheap_down(size_t i, size_t n, size_t elt_size, void *arr, int (*compare)(const void *, const void *))
{
  void *temp = malloc(elt_size);
  size_t max_child;
  if (temp == NULL)
    {
      return;
    }

  // TO DO: swap down until no children or larger than largest child
  while (HEAP_LEFT(i) < n) // checks if i has at least one child
    {
      if (HEAP_RIGHT(i) < n && compare((char *)arr + elt_size * HEAP_RIGHT(i), (char *)arr + elt_size * HEAP_LEFT(i)) > 0) // if i has two children and right is bigger than left
        {
          max_child = HEAP_RIGHT(i);                    
        }
      else 
        {
          max_child = HEAP_LEFT(i);
        }

      if (compare((char *)arr + elt_size * i, (char *)arr + elt_size * max_child) >= 0) // if i is larger than it's largest child
        {
          break;
        }
      else 
        {
          heap_swap((char *)arr + elt_size * i, (char *)arr + elt_size * max_child, temp, elt_size);
          i = max_child;
        }
    }

  free(temp);
}

void heap_swap(void *p1, void *p2, void *temp, size_t elt_size)
{
  memcpy(temp, p1, elt_size);
  memcpy(p1, p2, elt_size);
  memcpy(p2, temp, elt_size);
}
