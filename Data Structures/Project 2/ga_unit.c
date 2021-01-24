#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "strategy.h"
#include "population.h"
#include "ga_helpers.h"

#define COLUMNS (6)
const int distros[][COLUMNS] = {
			  {7, 0, 2, 1, 0, 0},
			  {0, 5, 3, 2, 0, 0},
			  {4, 6, 0, 0, 0, 0},
			  {10, 0, 0, 0, 0, 0},
			  {0, 0, 4, 3, 2, 1}
};

const size_t ROWS = sizeof(distros) / (sizeof(int) * COLUMNS);
const double weights[] = {0.5, 0.3, 0.2, 0.1, 0.2};
const double values1[] = {6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
const double values2[] = {20.0, 5.0, 4.0, 3.0, 2.0, 1.0};

/**
 * Creates and returns a strategy using distributions read from the
 * given array.  Returns NULL and prints an error message to standard
 * output if there is a problem creating the strategy.  It is the
 * caller's responsibility to destroy the strategy if the return value
 * is non-NULL.
 * 
 * @param rows a nonnegative integer
 * @param cols a nonnegative integer
 * @param distros a 2-D array of nonnegative integers with at least
 * max(1, rows) rows all having the same sum
 * @param weights a pointer to an array of cols nonnegative numbers
 * @return a pointer to a strategy, or NULL
 */
strategy *initialize_strategy(size_t rows, size_t cols, const int distros[][cols], const double *weights);

/**
 * Adds distributions from the given array to the given strategy.
 * Returns false and prints an error message to standard output if
 * there is a problem adding the strategies.  Returns true otherwise.
 * 
 * @param rows a nonnegative integer
 * @param cols a nonnegative integer
 * @param distros a rows-by-cols array of nonnegative integers with all
 * rows having the same sum
 * @param weights a pointer to an array of cols nonnegative numbers
 * @return true if and only if all distrbutions were added
 */
bool add_distributions(strategy *s, size_t rows, size_t cols, const int distros[][cols], const double *weights);

population *create_population(size_t n, strategy **to_add);
population *create_population_many(size_t n, size_t cols, const int distros[][cols], const double *weights);

void test_create_destroy();
void test_game_parameters();
void test_add_one();
void test_add_copies_distribution();
void test_add_many(size_t rows, size_t cols, const int distros[][cols], const double *weights);
void test_add_duplicates(size_t rows, size_t cols, const int distros[][cols], const double *weights, int dups);
void test_expected_wins(size_t cols, size_t rows1, const int d1[][cols], const double *w1, size_t rows2, const int d2[][cols], const double *w2, const double *values);
void test_crossover(size_t cols, size_t rows1, const int d1[][cols], const double *w1, size_t rows2, const int d2[][cols], const double *w2);
void test_copy(size_t rows1, size_t cols, const int distros[][cols], const double *weights);
void test_copy_no_alias(size_t cols, size_t rows1, const int d1[][cols], const double *w1, size_t rows2, const int d2[][cols], const double *w2);
void test_copy_deep(size_t rows1, size_t cols, const int distros[][cols], const double *weights);

void test_empty_population();
void test_pop_add_one(size_t rows, size_t cols, const int distros[][cols], const double *weights);
void test_pop_get_one(size_t rows, size_t cols, const int distros[][cols], const double *weights);
void test_pop_aliases(size_t cols, const int distros[][cols], const double *weights);
void test_pop_add_many(size_t n, size_t cols, const int distros[][cols], const double *weights);
void test_pop_remove(size_t n, size_t cols, const int distros[][cols], const double *weights, size_t remove_count);
void test_pop_order(size_t n, size_t cols, const int distros[][cols], const double *weights, const double *values);

int main(int argc, char **argv)
{
   if (argc < 2)
    {
      fprintf(stderr, "USAGE: %s test-number [test-arguments...]\n", argv[0]);
      return 1;
    }

  int test_num = atoi(argv[1]);

  switch (test_num)
    {
    case 1:
      test_create_destroy();
      break;

    case 2:
      test_game_parameters();
      break;
      
    case 3:
      test_add_one();
      break;

    case 4:
      test_add_many(4, COLUMNS, distros, weights);
      break;

    case 5:
      test_add_duplicates(4, COLUMNS, distros, weights, 2);
      break;

    case 6:
      // expected wins for pure strategies
      test_expected_wins(COLUMNS, 1, distros, weights, 1, &distros[0] + 1, weights + 1, values1);
      test_expected_wins(COLUMNS, 1, distros, weights, 1, &distros[0] + 1, weights + 1, values2);
      break;

    case 7:
      // expected wins for mixed strategies
      test_expected_wins(COLUMNS, 2, distros, weights, 2, &distros[0] + 2, weights + 2, values1);
      test_expected_wins(COLUMNS, 2, distros, weights, 2, &distros[0] + 2, weights + 2, values2);
      break;

    case 8:
      test_crossover(COLUMNS, 1, distros, weights, 1, &distros[0] + 1, weights + 1);
      break;

    case 9:
      test_crossover(COLUMNS, 3, distros, weights, 2, &distros[0] + 3, weights + 3);
      break;

    case 10:
      test_copy(3, COLUMNS, distros, weights);
      break;

    case 11:
      test_empty_population();
      break;
      
    case 12:
      // add one pure strategy
      test_pop_add_one(1, COLUMNS, distros, weights);
      break;

    case 13:
      // add one mixed strategy
      test_pop_add_one(3, COLUMNS, distros, weights);
      break;

    case 14:
      // test aliasing for pure strategies
      test_pop_aliases(COLUMNS, distros, weights);
      break;

    case 15:
      // add ROWS pure strategies
      test_pop_add_many(ROWS, COLUMNS, distros, weights);
      break;

    case 16:
      test_pop_remove(ROWS, COLUMNS, distros, weights, 2);
      break;

    case 17:
      // order ROWS pure strategies
      test_pop_order(ROWS, COLUMNS, distros, weights, values1);
      break;

    default:
      fprintf(stderr, "%s: invalid test number %s\n", argv[0], argv[1]);
      return 1;
    }

  return 0;
}

void test_game_parameters()
{
  strategy *s = strategy_create(10, 25);

  if (s != NULL)
    {
      printf("locations = %lu, units = %d\n",
	     strategy_count_locations(s),
	     strategy_count_units(s));
      
      strategy_destroy(s);
    }
  else
    {
      printf("FAILED -- could not create strategy\n");
    }
}

void test_create_destroy()
{
  strategy *s = strategy_create(6, 10);
  if (s != NULL)
    {
      strategy_destroy(s);
      printf("PASSED\n");
    }
  else
    {
      printf("FAILED -- could not create strategy\n");
    }
}

void test_add_one()
{
  strategy *s = initialize_strategy(1, COLUMNS, distros, weights);
  if (s != NULL)
    {
      strategy_print(stdout, s);
      strategy_destroy(s);
    }
}

void test_add_copies_distribution()
{
  int distribution[] = {2, 1, 1, 1, 1, 1};
  strategy *s = strategy_create(6, 7);

  if (s == NULL)
    {
      printf("FAILED -- could not create strategy");
      return;
    }

  if (!strategy_add_distribution(s, distribution, 1.0))
    {
      printf("FAILED -- could not add distribution\n");
      strategy_destroy(s);
      return;
    }
  
  printf("BEFORE CHANGING ARRAY\n");
  strategy_print(stdout, s);

  distribution[0]++;
  printf("AFTER CHANGING ARRAY\n");
  strategy_print(stdout, s);
      
  strategy_destroy(s);
}

void test_add_many(size_t rows, size_t cols, const int distros[][cols], const double *weights)
{
  strategy *s = initialize_strategy(rows, cols, distros, weights);
  if (s != NULL)
    {
      strategy_print(stdout, s);
      strategy_destroy(s);
    }
}

void test_add_duplicates(size_t rows, size_t cols, const int distros[][cols], const double *weights, int dups)
{
  strategy *s = initialize_strategy(rows, cols, distros, weights);

  if (s != NULL)
    {
      for (int d = 1; d < dups; d++)
	{
	  if (!add_distributions(s, rows, cols, distros, weights))
	    {
	      printf("FAILED -- could not add duplicates\n");
	      strategy_destroy(s);
	      return;
	    }
	}
      strategy_print(stdout, s);
      strategy_destroy(s);
    }
}

strategy *initialize_strategy(size_t rows, size_t cols, const int distros[][cols], const double *weights)
{
  // determine number of units in input array
  int units = 0;
  for (size_t c = 0; c < cols; c++)
    {
      units += distros[0][c];
    }
  
  strategy *s = strategy_create(cols, units);
  if (s == NULL)
    {
      printf("FAILED -- could not create strategy\n");
      return NULL;
    }

  if (!add_distributions(s, rows, cols, distros, weights))
    {
      strategy_destroy(s);
      return NULL;
    }

  return s;
}

bool add_distributions(strategy *s, size_t rows, size_t cols, const int distros[][cols], const double *weights)
{
  for (size_t r = 0; r < rows; r++)
    {
      if (!strategy_add_distribution(s, distros[r], weights[r]))
	{
	  printf("FAILED -- could not add distribution\n");
	  return false;
	}
    }

  return true;
}

void test_expected_wins(size_t cols, size_t rows1, const int d1[][cols], const double *w1, size_t rows2, const int d2[][cols], const double *w2, const double *values)
{
  strategy *s1 = initialize_strategy(rows1, cols, d1, w1);
  if (s1 == NULL)
    {
      printf("FAILED -- could not create strategy\n");
      return;
    }

  strategy *s2 = initialize_strategy(rows2, cols, d2, w2);
  if (s2 == NULL)
    {
      printf("FAILED -- could not create strategy\n");
      strategy_destroy(s1);
      return;
    }

  printf("%lf\n", strategy_expected_wins(s1, s2, values));

  strategy_destroy(s1);
  strategy_destroy(s2);
}
  
void test_crossover(size_t cols, size_t rows1, const int d1[][cols], const double *w1, size_t rows2, const int d2[][cols], const double *w2)
{
  strategy *s1 = initialize_strategy(rows1, cols, d1, w1);
  if (s1 == NULL)
    {
      printf("FAILED -- could not create strategy\n");
      return;
    }

  strategy *s2 = initialize_strategy(rows2, cols, d2, w2);
  if (s2 == NULL)
    {
      printf("FAILED -- could not create strategy\n");
      strategy_destroy(s1);
      return;
    }

  strategy **offspring = strategy_crossover(s1, s2);
  if (offspring == NULL)
    {
      printf("FAILED -- could not create offspring\n");
      return;
    }
  if (offspring[0] == NULL || offspring[1] == NULL)
    {
      printf("FAILED -- offspring array contains NULL\n");
      for (size_t i = 0; i < 2; i++)
	{
	  if (offspring[i] != NULL)
	    {
	      strategy_destroy(offspring[i]);
	    }
	}
      free(offspring);
      return;
    }

  for (size_t i = 0; i < 2; i++)
    {
      printf("OFFSPRING %lu\n", i);
      strategy_print(stdout,offspring[i]);
      strategy_destroy(offspring[i]);
    }
  free(offspring);
  strategy_destroy(s1);
  strategy_destroy(s2);
}

void test_copy(size_t rows, size_t cols, const int distros[][cols], const double *weights)
{
  strategy *s1 = initialize_strategy(rows, cols, distros, weights);
  if (s1 == NULL)
    {
      printf("FAILED -- could not create strategy\n");
      return;
    }

  strategy *s2 = strategy_copy(s1);
  if (s2 == NULL)
    {
      printf("FAILED -- could not create copy\n");
      strategy_destroy(s1);
      return;
    }

  printf("ORIGINAL\n");
  strategy_print(stdout, s1);
  printf("COPY\n");
  strategy_print(stdout, s2);
  
  strategy_destroy(s1);
  strategy_destroy(s2);

}

void test_copy_no_alias(size_t cols, size_t rows1, const int d1[][cols], const double *w1, size_t rows2, const int d2[][cols], const double *w2)
{
  strategy *s1 = initialize_strategy(rows1, cols, d1, w1);
  if (s1 == NULL)
    {
      printf("FAILED -- could not create strategy\n");
      return;
    }

  strategy *s2 = strategy_copy(s1);
  if (s2 == NULL)
    {
      printf("FAILED -- could not create copy\n");
      strategy_destroy(s1);
      return;
    }

  add_distributions(s2, rows2, cols, d2, w2);

  printf("ORIGINAL\n");
  strategy_print(stdout, s1);
  printf("MODIFIED COPY\n");
  strategy_print(stdout, s2);
  
  strategy_destroy(s1);
  strategy_destroy(s2);
}

void test_copy_deep(size_t rows, size_t cols, const int distros[][cols], const double *weights)
{
  strategy *s1 = initialize_strategy(rows, cols, distros, weights);
  if (s1 == NULL)
    {
      printf("FAILED -- could not create strategy\n");
      return;
    }

  strategy *s2 = strategy_copy(s1);
  if (s2 == NULL)
    {
      printf("FAILED -- could not create copy\n");
      strategy_destroy(s1);
      return;
    }

  printf("ORIGINAL\n");
  strategy_print(stdout, s1);
  
  strategy_destroy(s1);

  printf("COPY\n");
  strategy_print(stdout, s2);
  
  strategy_destroy(s2);
}

void test_empty_population()
{
  population *pop = population_create();
  if (pop == NULL)
    {
      printf("FAILED -- could not create population\n");
      return;
    }

  size_t size = population_size(pop);
  if (size != 0)
    {
      printf("FAILED -- newly created population has size %lu\n", size);
      return;
    }

  population_destroy(pop);
  printf("PASSED\n");
}

population *create_population(size_t n, strategy **to_add)
{
  population *pop = population_create();
  if (pop == NULL)
    {
      printf("FAILED -- could not create population\n");
      for (size_t i = 0; i < n; i++)
	{
	  if (to_add[i] != NULL)
	    {
	      strategy_destroy(to_add[i]);
	    }
	}
      return NULL;
    }

  for (size_t i = 0; i < n; i++)
    {
      if (to_add[i] == NULL || !population_add_strategy(pop, to_add[i]))
	{
	  printf("FAILED -- could not add strategy\n");
	  for (size_t j = i; j < n; j++)
	    {
	      if (to_add[j] != NULL)
		{
		  strategy_destroy(to_add[j]);
		}
	    }
	  population_destroy(pop);
	  return NULL;
	}
    }
  
  return pop;
}


void test_pop_add_one(size_t rows, size_t cols, const int distros[][cols], const double *weights)
{
  strategy *s = initialize_strategy(rows, cols, distros, weights);
  if (s == NULL)
    {
      printf("FAILED -- could not create strategy to add\n");
      return;
    }
  
  strategy *to_add[] = {s};

  population *pop = create_population(1, to_add);

  if (pop == NULL)
    {
      // message printed in create
      return;
    }

  size_t size = population_size(pop);
  if (size != 1)
    {
      printf("FAILED -- population has size %lu after adding\n", size);
      return;
    }

  population_destroy(pop);
  printf("PASSED\n");
}

void test_pop_get_one(size_t rows, size_t cols, const int distros[][cols], const double *weights)
{
  strategy *s = initialize_strategy(rows, cols, distros, weights);
  if (s == NULL)
    {
      printf("FAILED -- could not create strategy to add\n");
      return;
    }
  
  strategy *to_add[] = {s};

  population *pop = create_population(1, to_add);

  if (pop == NULL)
    {
      // message printed in create
      return;
    }

  strategy *gotten = population_get(pop, 0);
  if (gotten == NULL)
    {
      printf("FAILED -- could not get strategy\n");
      population_destroy(pop);
      return;
    }

  strategy_print(stdout, gotten);
  population_destroy(pop);
}

void test_pop_aliases(size_t cols, const int distros[][cols], const double *weights)
{
  strategy *s = initialize_strategy(1, cols, distros, weights);
  if (s == NULL)
    {
      printf("FAILED -- could not create strategy to add\n");
      return;
    }
  
  strategy *to_add[] = {s};
  
  population *pop = create_population(1, to_add);
  
  if (pop == NULL)
    {
      // message printed in create
      return;
    }
  if (!strategy_add_distribution(s, distros[1], weights[1]))
    {
      printf("FAILED -- could not add distribution to original strategy\n");
      population_destroy(pop);
      return;
    }
    
  strategy *gotten = population_get(pop, 0);
  if (gotten == NULL)
    {
      printf("FAILED -- could not get strategy\n");
      population_destroy(pop);
      return;
    }
  printf("AFTER MODIFYING ORIGINAL STRATEGY\n");
  strategy_print(stdout, gotten);

  if (!strategy_add_distribution(gotten, distros[2], weights[2]))
    {
      printf("FAILED -- could not add distribution to retrieved strategt\n");
      population_destroy(pop);
      return;
    }

  // re-retrieve -- individual in population should have changed too since
  // they are supposed to be the same individual
  gotten = population_get(pop, 0);
  if (gotten == NULL)
    {
      printf("FAILED -- could not get strategy\n");
      population_destroy(pop);
      return;
    }
  printf("AFTER MODIFYING RETRIEVED STRATEGY\n");
  strategy_print(stdout, gotten);

  population_destroy(pop);
}

population *create_population_many(size_t n, size_t cols, const int distros[][cols], const double *weights)
{
  population *pop = population_create();
  if (pop == NULL)
    {
      printf("FAILED -- could not create population\n");      
      return NULL;
    }

  // make a pure strategy from each row of the array passed in and add it
  for (size_t i = 0; i < n; i++)
    {
      strategy *s = initialize_strategy(1, cols, &distros[0] + i, weights + i);
      if (s == NULL)
	{
	  // message printed in initialize
	  population_destroy(pop);
	  return NULL;
	}

      if (!population_add_strategy(pop, s))
	{
	  printf("FAILED -- could not add strategy\n");
	  strategy_destroy(s);
	  population_destroy(pop);
	  return NULL;
	}
    }

  return pop;
}  

void test_pop_add_many(size_t n, size_t cols, const int distros[][cols], const double *weights)
{
  population *pop = create_population_many(n, cols, distros, weights);
  if (pop == NULL)
    {
      // message printed in create
      return;
    }

  size_t size = population_size(pop);
  if (size != n)
    {
      printf("FAILED -- size is %lu; should be %lu\n", size, n);
      population_destroy(pop);
      return;
    }

  if (!print_population(pop))
    {
      // message printed in print
      population_destroy(pop);
      return;
    }

  population_destroy(pop);
}

void test_pop_remove(size_t n, size_t cols, const int distros[][cols], const double *weights, size_t remove_count)
{
  population *pop = create_population_many(n, cols, distros, weights);
  if (pop == NULL)
    {
      // message printed in create
      return;
    }

  if (remove_count <= 0 || remove_count > population_size(pop))
    {
      printf("FAILED -- bad number to remove %lu\n", remove_count);
    }

  strategy **removed = population_remove_last(pop, remove_count);

  if (removed == NULL)
    {
      printf("FAILED -- could not get array of removed strategies\n");
      population_destroy(pop);
      return;
    }
  
  size_t size = population_size(pop);
  if (size != n - remove_count)
    {
      printf("FAILED -- size after remove is %lu; should be %lu\n", size, n - remove_count);
      return;
    }

  for (size_t i = 0; i < remove_count; i++)
    {
      if (removed[i] == NULL)
	{
	  printf("FAILED -- removed strategy is NULL at index %lu\n", i);
	  for (int j = i + 1; i < remove_count; j++)
	    {
	      if (removed[j] != NULL)
		{
		  strategy_destroy(removed[j]);
		}
	      free(removed);
	    }
	  return;
	}
      printf("REMOVED INDIVIDUAL %lu\n", i);
      strategy_print(stdout, removed[i]);
      strategy_destroy(removed[i]);
    }
  free(removed);
  
  if (!print_population(pop))
    {
      // message printed in print
      population_destroy(pop);
    }
  population_destroy(pop);
}

void test_pop_order(size_t n, size_t cols, const int distros[][cols], const double *weights, const double *values)
{
  population *pop = create_population_many(n, cols, distros, weights);
  if (pop == NULL)
    {
      // message printed in create
      return;
    }

  population_order(pop, values);
  
  if (!print_population(pop))
    {
      // message printed in print
      population_destroy(pop);
      return;
    }
  
  population_destroy(pop);
}

