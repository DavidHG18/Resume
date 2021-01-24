#ifndef __HEAP_H__
#define __HEAP_H__

#include <stdlib.h>

/**
 * Sorts the given array in increasing order according to the given
 * comparison function.
 *
 * @param n a nonnegative integer
 * @param elt_size the size of the elements in arr
 * @param arr an array of n elements
 * @param compare a function that takes pointers to two elements and compares
 * them, returning negative if the first comes before the second, positive
 * for the other way around, and 0 if they are the same
 */
void heapsort(size_t n, size_t elt_size, void *arr, int (*compare)(const void *, const void *));

/**
 * Creates a max-heap from the given array.
 *
 * @param i a nonnegative integer less than n
 * @param n a nonnegative integer
 * @param elt_size the size of the elements in arr
 * @param arr an array of n elements satisfying the max-heap order property
 * everywhere except possibly between the element at index i and its
 * parent
 * @param compare a function that takes pointers to two elements and compares
 * them, returning negative if the first comes before the second, positive
 * for the other way around, and 0 if they are the same
 */
void heap_reheap_up(size_t i, size_t n, size_t elt_size, void *arr, int (*compare)(const void *, const void *));

/**
 * Creates a max-heap from the given array.
 *
 * @param i a nonnegative integer less than n
 * @param n a nonnegative integer
 * @param elt_size the size of the elements in arr
 * @param arr an array of n elements satisfying the max-heap order property
 * everywhere except possibly between the element at index i and its
 * children
 * @param compare a function that takes pointers to two elements and compares
 * them, returning negative if the first comes before the second, positive
 * for the other way around, and 0 if they are the same
 */
void heap_reheap_down(size_t i, size_t n, size_t elt_size, void *arr, int (*compare)(const void *, const void *));

#endif
