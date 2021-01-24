#ifndef __STRING_HELPERS_H__
#define __STRING_HELPERS_H__

#include <stdio.h>
/**
 * Frees all the strings in the given array.  There is no effect
 * if the array is NULL.
 *
 * @param arr an array of strings
 * @param n the size of that array
 */
void string_array_free(char **arr, size_t n);

/**
 * Returns the hash value of the given string.
 * 
 * @param s a pointer to a string
 * @return the hash value of that string
 */
size_t string_hash_java(const void *s);

/**
 * Prints the strings in the given array, one per line.
 *
 * @param arr an array of non-NULL strings, non-NULL
 * @param n the size of that array
 */
void string_array_fprintf(FILE *fp, char **arr, size_t n);

/**
 * Returns a copy of the given string.  The return value is NULL
 * if there is an error allocating memory for the copy.  It is the
 * caller's responsibility to free the returned string if it is not
 * NULL.
 *
 * @param s a pointer to a string, non-NULL
 * @return a pointer to a string, or NULL
 */
void *string_copy(const void *s);

/**
 * Compares two strings using strcmp.
 *
 * @param p1 a pointer to a string, non-NULL
 * @param p2 a pointer to a string, non-NULL
 * @return negative if p1 < p2, positive if p1 > p2, zero if p1 = p2
 */
int string_compare(const void *p1, const void *p2);

#endif
