#ifndef __POPULATION_H__
#define __POPULATION_H__

#include <stdlib.h>
#include <stdbool.h>

#include "strategy.h"

/**
 * A population of Blotto strategies.
 */
typedef struct _population population;

/**
 * Returns an empty population.  It is the caller's responsibility to
 * destroy the returned population.
 *
 * @return a pointer to a population
 */
population *population_create();

/**
 * Returns the number of strategies in this population.
 *
 * @param pop a pointer to a population, non-NULL
 * @return a nonnegative integer
 */
size_t population_size(const population *pop);

/**
 * Attempts to add the given strategy to the end of the given population.
 * The strategy is not added if the population is non-empty and the
 * new strategy plays a Blotto game of a different size and/or
 * different total number of units than the strategies already in the
 * population.  The return value is true if the strategy was added and
 * false otherwise.  Ownership of the strategy is transferred to the
 * population and changes to the strategy after adds are reflected in
 * the population.
 *
 * @param pop a pointer to a population, non-NULL
 * @param s a pointer to a strategy not in pop, non-NULL
 * @return true if and only if the strategy was added
 */
bool population_add_strategy(population *pop, strategy *s);

/**
 * Returns the strategy at the given index in the given population.
 * Modifications to the returned strategy are reflected in the
 * population, but the population retains ownership of the strategy.
 *
 * @param pop a population, non-NULL
 * @param i a nonnegative integer no greater than the size of pop
 * @return a pointer to a strategy, non-NULL
 */
strategy *population_get(population *pop, size_t i);

/**
 * Removes and returns the last strategies in this population.  The
 * strategies are ordered in the same array as they were ordered in
 * the population.  It is the caller's responsibility to destroy the
 * strategies and to free the returned array.
 *
 * @param pop a pointer to a population, non-NULL
 * @param n a positive integer no greater than the size of pop
 * @return an array containing distinct pointers to non-NULL strategies
 */
strategy **population_remove_last(population *pop, size_t n);

/**
 * Reorders the given population in descending order of expected wins
 * when each contained strategy plays one game against each other
 * contained strategy in a game using the location values in the given
 * array.  Ties are broken arbitrarily.
 *
 * @param pop a pointer to a population, non-NULL
 * @param values a pointer to an array of doubles of size equal to the
 * size of the games played by all the strategies in pop, non-NULL
 */
void population_order(population *pop, const double *values);

/**
 * Destroys the given population and releases all the resources held
 * by it and the contained strategies.
 *
 * @pop a pointer to a population, non-NULL
 */
void population_destroy(population *pop);

#endif
