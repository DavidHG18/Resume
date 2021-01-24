#include <stdio.h>
#include <stdbool.h>

#include "ga_helpers.h"
#include "strategy.h"
#include "population.h"

bool print_population(population *pop)
{
  if (pop == NULL)
    {
      return false;
    }
  
  size_t n = population_size(pop);
  for (size_t i = 0; i < n; i++)
    {
      strategy *s = population_get(pop, i);
      if (s == NULL)
        {
          printf("FAILED -- could not retrieve strategy at index %lu\n", i);
          return false;
        }
      printf("INDIVIDUAL %lu\n", i);
      strategy_print(stdout, s);
    }
  return true;
}
