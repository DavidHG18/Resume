#ifndef __GA_HELPERS_H__
#define __GA_HELPERS_H__

#include <stdbool.h>

#include "strategy.h"
#include "population.h"

/**
 * Prints the given population to standard output.  Each individual is
 * output in order using strategy_print following a line containing
 * "INDIVIDUAL N" where N is the index in the population. If there is
 * an error retrieving any individual in the population then the
 * return value is false and the output is undefined.
 *
 * @param pop a pointer to a population, non-NULL
 * @return true if and only if the entire population was printed
 */
bool print_population(population *pop);

#endif
