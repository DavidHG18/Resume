#ifndef __STRATEGY_H__
#define __STRATEGY_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "blotto.h"
#include "arrays.h"

/**
 * A Blotto strategy.  A Blotto strategy is a list of possible
 * resource distributions, each of which is a list of nonnegative
 * integers, one per location in the Blotto game the strategy is for,
 * and with sum equal to the total number of units allowed in game.
 * Each resource distribution has an associated nonnegative weight.
 * The strategy plays one of those distributions with probability
 * proportional to their weights.
 */
typedef struct _strategy strategy;

/**
 * Creates an empty strategy for a Blotto game with the given number
 * of locations to allocate resources to and the given total number of
 * units to allocate.  It is the caller's responsibility to eventually
 * destroy the strategy with strategy_destroy.
 *
 * @param num_locations a nonnegative integer
 * @param num_units a nonnegative integer
 * @return a pointer to a strategy
 */
strategy *strategy_create(size_t num_locations, size_t num_units);

/**
 * Returns the number of locations in the Blotto game the given strategy
 * plays.
 *
 * @param s a pointer to a strategy, non-NULL
 * @return the number of locations in the game s plays
 */
size_t strategy_count_locations(const strategy *s);

/**
 * Returns the total number of units in the Blotto game the given strategy
 * plays.
 *
 * @param s a pointer to a strategy, non-NULL
 * @return the number of units in the game s plays
 */
int strategy_count_units(const strategy *s);

/**
 * Adds a copy of the given distribution to the given strategy with
 * the given weight.  If the distribution is already present then the
 * weight is added to the weight of the existing distibution and the
 * distribution is not added (this should always succeed).  The return
 * value is true if the distribution was successfully added or it the
 * distribution was already present.  If the distribution was not
 * added (because, for example, the total number of units was
 * incorrect, or the weight was not positive) then the return value is
 * false and there is no effect on the strategy.
 *
 * @param s a pointer to a strategy, non-NULL
 * @param dist a pointer to an array of non-negative integers
 * of the same size as the Blotto game this strategy plays, and with
 * sum equal to the total number of resources this strategy deploys,
 * non-NULL
 * @param w a positive number
 */
bool strategy_add_distribution(strategy *s, const int *dist, double w);

/**
 * Returns the expected number of wins for the first strategy when it plays
 * against the second strategy in a Blotto game with the given
 * values for each location.  For non-empty strategies,
 * the expected number of wins is
 * determined by evaluating each possible combination of distributions
 * played by the two strategies, with the probability of a particular
 * distribution being played by one strategy equal to the weight of that
 * distribution divided by the total weight of all the distributions for
 * the strategy.  A tie counts as half a win.  If only one strategy
 * is empty, the non-empty strategy loses.  If both are empty then
 * the result is a tie.
 *
 * @param s1 a pointer to a strategy, non-NULL
 * @param s2 a pointer to a strategy playing a game of the same size
 * and number of units as the game s1 plays, non-NULL
 * @param values a pointer to an array of positive doubles of size equal to the
 * size of the game the strategies play
 */
double strategy_expected_wins(const strategy *s1, const strategy *s2, const double *values);

/**
 * Returns an array containing the two possible outcomes of performing
 * crossover between the two given strategies.  One possible outcome
 * is the first, third, ... distributions and associated weights from
 * the first strategy with the second, fourth, ... distributions and
 * weights from the second; the other outcome is the other way around.
 * In both cases, the ordering of the distributions within a single
 * strategy is considered to be by lexicographic order.  If the two
 * outcomes are the same, then the array still contains two separate
 * strategies.  It is the caller's responsibility to free the array
 * and destroy the strategies it contains.
 *
 * @param s1 a pointer to a strategy, non-NULL
 * @param s2 a pointer to a strategy playing a game of the same size as
 * the game s1 plays with the same number of units, non-NULL
 * @return a pointer to an array of two non-NULL strategies
 */
strategy **strategy_crossover(const strategy *s1, const strategy *s2);

/**
 * Returns a deep copy of the given strategy.  It is the caller's
 * responsibility to destroy the returned strategy.
 *
 * @param s a pointer to a strategy, non-NULL
 * @return a pointer a a strategy
 */
strategy *strategy_copy(const strategy *s);

/**
 * Prints the given strategy to the given file.  Each distribution
 * is printed on a separate line, with the line containing
 * a comma-and-space-separated, square-bracket-enclosed
 * list of the units allocated to each location, followed by a
 * single space and the weight to three decimal places.
 * The distributions are output in lexicographic order.  For example,
 * one strategy with three distributions could be
 *
 * [1, 1, 0, 10] 0.250
 * [3, 3, 3, 3] 0.500
 * [6, 4, 2, 0] 0.250
 *
 * @param out a pointer to a FILE, non-NULL
 * @param s a pointer to a strategy, non-NULL
 */
void strategy_print(FILE *out, const strategy *s);

/**
 * Destroys the given strategy, releasing all resources it holds.
 *
 * @param s a pointer to a strategy, non-NULL
 */
void strategy_destroy(strategy *s);

#endif
