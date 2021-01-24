#ifndef __BLOTTO_H__
#define __BLOTTO_H__

#include <stddef.h>
/**
 * Determines the result of playing the two given allocations of units
 * against each other.  The return value is 1.0 if the first allocation
 * wins, 0.0 if the second wins, and 0.5 for a draw.
 *
 * @param n a positive integer
 * @param a1 a pointer to an array of n nonnegative integers, non-NULL
 * @param a2 a pointer to an array of n nonnegative integers, non-NULL
 * @param v a pointer to an array of n positive doubles, non-NULL
 */
double blotto_play_game(size_t n, const int *a1, const int *a2, const double *v);
#endif
