#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "strategy.h"
#include "blotto.h"
#include "arrays.h"

typedef struct _distribution // creating a structure for our distributions, each containing a weight and an array of allocated resources
{
  int *resources;
  double weight;
}distribution;

struct _strategy
{
  size_t num_locations;
  size_t num_units;
  distribution *distro; // pointer to a list of distributions
  size_t size;
  size_t capacity;
  double total_weight; // total weight of components (meant to be used for normalizing in expected value calculations)
};

#define STRATEGY_INITIAL_CAPACITY 3

strategy *strategy_create(size_t num_locations, size_t num_units) // function to create a strategy
{
  strategy *result = malloc(sizeof(strategy));

  if (result != NULL)
    {
      result->num_locations = num_locations;
      result->num_units = num_units;
      result->size = 0;
      result->distro = malloc(STRATEGY_INITIAL_CAPACITY * sizeof(distribution)); // a list of distributions
      if (result->distro == NULL)
        {
          return NULL;
        }
      result->capacity = STRATEGY_INITIAL_CAPACITY;
      result->total_weight = 0;
    }

  return result;
}

size_t strategy_count_locations(const strategy *s)
{
  if (s != NULL)
    {
      return s->num_locations;
    }
  else
    {
      return 0;
    }
}

int strategy_count_units(const strategy *s)
{
  if (s != NULL)
    {
      return s->num_units;
    }
  else
    {
      return 0;
    }
}

bool strategy_add_distribution(strategy *s, const int *dist, double w)
{
  if (s == NULL || w <= 0.0)
    {
      return false;
    }

  if (s->size == s->capacity)
    {
      // TO DO -- mixed strategy should never be full
      // dynamically resize array
      s->capacity *= 2;
      s->distro = realloc(s->distro, s->capacity * sizeof(distribution));
    }

  // make a copy of the given distribution
  int *copy = malloc(sizeof(int) * s->num_locations);

  if (copy == NULL) // if we failed to create an int
    {
      return false;
    }

  // compute the total number of units in the distribution
  int sum = 0;
  for (size_t i = 0; i < s->num_locations; i++)
    {
      sum += dist[i];
      copy[i] = dist[i];
    }

  // make sure the total is correct
  if (sum != s->num_units)
    {
      free(copy);
      return false;
    }

  // search current distro list for copy; if equal, just change weight, if not equal, then insert copy
  for (int i = 0; i < s->size; i++)
    {
      // first comparison is if they are equal
      if (arrays_compare(s->num_locations, s->distro[i].resources, copy) == 0)
        {
          s->distro[i].weight += w;
          s->total_weight += w;
          free(copy);
          return true;      
        }
      else if (arrays_compare(s->num_locations, s->distro[i].resources, copy) > 0)
        {
          // this means that copy needs to go before the current distribution
          for (int j = s->size; j > i; j--)
            {
              s->distro[j] = s->distro[j - 1];
            }
          s->distro[i].resources = copy;
          s->distro[i].weight = w;
          s->total_weight += w;
          s->size++;
          return true;
        }
    }
  
  // if we make it through this for loop then our new distro is in the last position, so we insert it there
  s->distro[s->size].resources = copy;
  s->distro[s->size].weight = w;
  s->size++;
  s->total_weight += w;
  return true;
}
      
double strategy_expected_wins(const strategy *s1, const strategy *s2, const double *values)
{
  if (s1 == NULL || s2 == NULL || values == NULL
      || s1->num_locations != s2->num_locations
      || s1->num_units != s2->num_units)
    {
      return 0.0;
    }

  if (s1->size == 0 && s2->size == 0)
    {
      return 0.5;
    }
  else if (s1->size == 0)
    {
      return 0.0;
    }
  else if (s2->size == 0)
    {
      return 1.0;
    }
  
  // if there are strategies for both, then we calculate the expected value of each pairing of distributions * weights
  // *NOTE* we need to normalize the weights in the strategy
  double game_value = 0.0;
  for (int i = 0; i < s1->size; i++)
    {
      for (int j = 0; j < s2->size; j++)
        {
          game_value += s1->distro[i].weight / s1->total_weight * s2->distro[j].weight / s2->total_weight * \
          blotto_play_game(s1->num_locations, s1->distro[i].resources, s2->distro[j].resources, values);
        }
    }
  return game_value;
}
strategy *strategy_copy(const strategy *s)
{
  if (s == NULL)
    {
      return NULL;
    }

  strategy *result = strategy_create(s->num_locations, s->num_units);
  if (result == NULL)
    {
      return NULL;
    }

  // TO DO -- this should perform a deep copy
  for (int i = 0; i < s->size; i++)
    {
      strategy_add_distribution(result, s->distro[i].resources, s->distro[i].weight);
    }
  
  return result;
}

strategy **strategy_crossover(const strategy *s1, const strategy *s2)
{
  if (s1 == NULL || s2 == NULL
      || s1->num_locations != s2->num_locations
      || s1->num_units != s2->num_units)
    {
      return NULL;
    }

  // make array to hold two offspring and make offspring as empty strategies
  strategy **offspring = malloc(sizeof(strategy *) * 2);
  strategy *strat1 = strategy_create(s1->num_locations, s1->num_units);
  strategy *strat2 = strategy_create(s2->num_locations, s2->num_units);
  if (offspring == NULL)
    {
      return NULL;
    }
  if (strat1 == NULL)
    {
      strategy_destroy(strat2);
      free(offspring);
      return NULL;
    }
  if (strat2 == NULL)
    {
      strategy_destroy(strat1);
      free(offspring);
      return NULL;
    }

  // 1st offspring is using the odd strategies / weights from player one and even weights from player two
  offspring[0] = strat1;
  // 2nd offspring is a copy of 2nd parent
  offspring[1] = strat2;
    
  for (int i = 0; i < s1->size; i+=2)
    {
      strategy_add_distribution(offspring[0], s1->distro[i].resources, s1->distro[i].weight);
      if (i < s1->size - 1)
        {
          strategy_add_distribution(offspring[1], s1->distro[i+1].resources, s1->distro[i+1].weight);
        }

    }
  for (int j = 0; j < s2->size; j += 2)
    {
      strategy_add_distribution(offspring[1], s2->distro[j].resources, s2->distro[j].weight);
      if (j < s2->size - 1)
        {
          strategy_add_distribution(offspring[0], s2->distro[j+1].resources, s2->distro[j+1].weight);
        }
    }

  return offspring;
}

void strategy_print(FILE *out, const strategy *s)
{
  if (out == NULL || s == NULL)
    {
      return;
    }

  for (size_t i = 0; i < s->size; i++)
    {
      arrays_print(out, s->num_locations, s->distro[i].resources);
      fprintf(out, " %.3f\n", s->distro[i].weight);
    }
      
}

void strategy_destroy(strategy *s)
{
  if (s != NULL)
    {
      for (size_t i = 0; i < s->size; i++)
        {
          free(s->distro[i].resources);
        }
      free(s->distro);
      free(s);
    }   
}

