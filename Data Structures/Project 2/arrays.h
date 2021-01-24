#ifndef __ARRAYS_H__
#define __ARRAYS_H__

#include <stdio.h>

/**
 * Prints a comma-and-space-separated, square-bracket-enclosed
 * list of the values in the given array to the given file.  There
 * is no newline printed at the end of the array.
 *
 * @param out a file, non-NULL
 * @param n a nonnegative integer
 * @param a a pointer to an array of n integers
 */
void arrays_print(FILE *out, size_t n, const int *a);

/**
 * Prints a comma-and-space-separated, square-bracket-enclosed
 * list of the values in the given array to the given file.  There
 * is no newline printed at the end of the array.
 *
 * @param out a file, non-NULL
 * @param fmt a printf format specifier for a double
 * @param n a nonnegative integer
 * @param a a pointer to an array of n double
 */
void arrays_print_double(FILE *out, const char *fmt, size_t n, const double *a);

/**
 * Compares the two arrays lexicographically.  The lexicographic order
 * is determined by the order in the first place the two arrays differ.
 * The return value is negative if the first array comes first, positive
 * if the second comes first, and zero if they are equal.
 * 
 * @param n a nonnegative integer
 * @param a1 a pointer to an array of n integers, non-NULL
 * @param a2 a pointer to an array of n integers, non-NULL
 * @return the result of comparing the two arrays
 */
int arrays_compare(size_t n, int *a1, int *a2);

#endif
