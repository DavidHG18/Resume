// THIS IS THE RIGHT VERSION



#include <stdlib.h>
#include <stdbool.h>

#include "population.h"
#include "strategy.h"

// please make this smaller!
#define POPULATION_INITIAL_CAPACITY 10

struct _population
{
  strategy **elements;
  size_t size;
  size_t capacity;
};

/**
 * Adds crossover strategies to the end of the population.
 * @param pop a population to be added to
 * @param n number of strategies to be crossed over
 * 
 */
population *pop_crossover(population *pop, size_t n);

/**
 * Resizes the array held by the given population.  (Since this is
 * private to this module, you can do whatever you want with this
 * function.)
 *
 * @param pop a pointer to a population, non-NULL
 */
void embiggen(population *pop);

population *population_create()
{
  population *result = malloc(sizeof(population));
  if (result != NULL)
    {
      result->elements = malloc(sizeof(strategy*) * POPULATION_INITIAL_CAPACITY); // initialize array of strategy pointers
      if (result->elements == NULL)
        {
          return NULL;
        }
      result->capacity = POPULATION_INITIAL_CAPACITY;
      result->size = 0;
    }
  return result;
}
/*
population *pop_crossover(population *pop, const size_t n)
  {
    if (pop != NULL && n <= pop->size)
      {
        for (int i = 0; i < n; i++)
          {
            for (int j = i + 1; j < n; j++)
              {
                population_add_strategy(pop, strategy_crossover(pop->elements[i], pop->elements[j]));
              }
          }
      }

    return pop;
  }
*/
size_t population_size(const population *pop)
{
  if (pop == NULL)
    {
      return 0;
    }
  else
    {
      return pop->size;
    }
}

bool population_add_strategy(population *pop, strategy *s)
{
  if (pop == NULL || s == NULL)
    {
      return false;
    }

  if (pop->size > 0 && (strategy_count_locations(s) != strategy_count_locations(pop->elements[0])
			|| strategy_count_units(s) != strategy_count_units(pop->elements[0])))
    {
      return false;
    }

  if (pop->size == pop->capacity)
    {
      embiggen(pop);
    }

  pop->elements[pop->size] = s;
  pop->size++;

  return true;
}

void embiggen(population *pop)
{
  // a noble spirit embiggens the smallest man
  // the embiggen function should resize the dynamically allocated array
  pop->capacity *= 2;
  pop->elements = realloc(pop->elements, pop->capacity * sizeof(strategy*));
}

strategy *population_get(population *pop, size_t i)
{
  if (pop == NULL || i < 0 || i >= pop->size)
    {
      return NULL;
    }
  else
    {
      return pop->elements[i];
    }
}

strategy **population_remove_last(population *pop, size_t n)
{
  if (pop == NULL || n <= 0)
    {
      return NULL;
    }
  else if (n > pop->size)
    {
      n = pop->size;
    }

  // FIX ME -- make array to return removed strategies in **** NEED TO FREE THIS LATER ****
  strategy **removed = malloc(n * sizeof(strategy*));
  size_t remove_index = 0;
  if (removed == NULL)
    {
      return NULL;
    }
  for (int i = pop->size - n; i < pop->size; i++)
    {
      // iterate starting at first strategy to remove
      removed[remove_index] = pop->elements[i];
      remove_index++;
    }

  pop->size -= n;
  return removed;
}

void population_order(population *pop, const double *values)
{
  double score[pop->size];

  for (size_t i = 0; i < pop->size; i++)
    {
      score[i] = 0.0;
    }

  // compute scores for all individuals in population
  for (size_t i = 0; i < pop->size; i++)
    {
      for (size_t j = i + 1; j < pop->size; j++)
        {
          double wins = strategy_expected_wins(pop->elements[i], pop->elements[j], values);
          score[i] += wins;
          score[j] += (1.0 - wins);
        }
    }

  // bubble sort meets the time bound since the above loop
  // already takes Theta(n^2 * m * p) time
  for (size_t pass = 0; pass < pop->size; pass++)
    {
      for (size_t i = 0; i < pop->size - 1 - pass; i++)
	{
	  if (score[i] < score[i + 1])
	    {
	      double temp_d = score[i];
	      strategy *temp_s = pop->elements[i];
	      score[i] = score[i + 1];
	      pop->elements[i] = pop->elements[i + 1];
	      score[i + 1] = temp_d;
	      pop->elements[i + 1] = temp_s;
	    }
	}
    }
}

void population_destroy(population *pop)
{
  if (pop != NULL)
    {
      for (size_t i = 0; i < pop->size; i++)
        {
          strategy_destroy(pop->elements[i]);
        }

    free(pop->elements);
    pop->size = 0;
    pop->capacity = 0;
    free(pop);
    }
}


