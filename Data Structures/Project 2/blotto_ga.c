#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strategy.h"
#include "population.h"
#include "ga_helpers.h"
#include "arrays.h"


/**
 * Function to read in a strategy from a file
 * @param in, file
 * @param num_locations, the number of battlefields we have
 */
strategy *strategy_readin(FILE *in, size_t num_locations);

/**
 * Function to sum up the values of an array
 * @param arr, pointer to an array
 * @param num_locations,  size of the array
 */int sum_array(int *arr, size_t num_locations);


int main(int argc, char **argv)
{
  if (argc < 2)
    {
      fprintf(stderr, "USAGE: %s values... files... operations...\n", argv[0]);
      return 1;
    }

  //processing command-line arguments: start with inputting the board
  double *board = malloc(sizeof(double) * argc); //initialize the board to be the size of the arguments because we don't know how big it'll be
  int i = 1;
  size_t num_locations = 0;
  bool board_done = false;
  population *pop = population_create();
  while (i < argc)
    {
      // first we read through the board
      if ((atoi(argv[i]) != 0) && (board_done == false))
        {
          if (board_done == true) // inputting integers at the wrong time
            {
              fprintf(stderr, "USAGE: %s values... files... operations...\n", argv[0]);
              free(board);
              population_destroy(pop);
              return 1;
            }
          board[num_locations] = atof(argv[i]);
          num_locations++;
        }
      else // we have finished going through the integers on the board
        {
          board_done = true;
          if (argv[i][0] != '-') // this means that we are reading in individual strategy files
            {
              FILE *infile = fopen(argv[i], "r");

              if (!infile)
                {
                  fprintf(stderr, "%s: Failure opening file\n", argv[0]);
                  free(board);
                  population_destroy(pop);
                  return 1;
                }

              strategy *strat = strategy_readin(infile, num_locations);
              fclose(infile);
              if (strat == NULL)
                {
                  fprintf(stderr, "%s: issue reading in a strategy for %s", argv[0], argv[i]);
                  free(board);
                  population_destroy(pop);
                  return 1;
                }
              population_add_strategy(pop, strat);
            }
          else
            {
              if (strcmp(argv[i], "-o") == 0)
                {
                  // this means that we need to reorder the population
                  population_order(pop, board);
                }
              else if(strcmp(argv[i], "-d") == 0)
                {
                  // this means we have to remove (argv[i+1]) strategies from population
                  if (i == argc - 1 || atoi(argv[i + 1]) == 0)
                    {
                      fprintf(stderr, "%s: -d must be followed by a positive integer.", argv[0]);
                      free(board);
                      population_destroy(pop);
                      return 1;
                    }
                  // otherwise we have a valid input, get return array and free everything in it
                  strategy **removed = population_remove_last(pop, atoi(argv[i+1]));
                  for (size_t k = 0; k < atoi(argv[i+1]); k++)
                    {
                      strategy_destroy(removed[k]);
                    }
                  free(removed);
                  i++; //increment twice
                }
              else if (strcmp(argv[i], "-x") == 0)
                {
                  // this means that we are adding the crossover strategy to end of population
                  if (i == argc - 1 || atoi(argv[i + 1]) == 0 || atoi(argv[i + 1]) < 2)
                    {
                      fprintf(stderr, "%s: improper input for number of strategies to be crossed.", argv[0]);
                      free(board);
                      population_destroy(pop);
                      return 1;
                    }
                  // otherwise we have a valid input
                  int crossovers = atoi(argv[i + 1]);
                  for (int i = 0; i < crossovers - 1; i++)
                    {
                      strategy *strat1 = population_get(pop, i);
                      for (int j = i + 1; j < crossovers; j++)
                        {
                          strategy *strat2 = population_get(pop, j);
                          strategy **cross_strat = strategy_crossover(strat1, strat2);
                          population_add_strategy(pop, cross_strat[0]);
                          population_add_strategy(pop, cross_strat[1]);
                          free(cross_strat);
                        }
                    }
                  if (pop == NULL)
                    {
                      fprintf(stderr, "%s: Error with crossover function.", argv[0]);
                      free(board);
                      population_destroy(pop);
                      return 1;
                    }
            
                  i++; // increment twice
                }
              else // then some improper input has been entered
                {
                  fprintf(stderr, "USAGE: %s values... files... operations...\n", argv[0]);
                  free(board);
                  population_destroy(pop);
                  return 1;
                }
            }
        }
      i++;
    }
  if (board_done == false)
    {
      fprintf(stderr, "%s: Must appropriately enter board.\n", argv[0]);
      free(board);
      population_destroy(pop);
      return 1;
    }
  
  print_population(pop);
  free(board);
  population_destroy(pop);

  return 0;
}

strategy *strategy_readin(FILE *in, size_t num_locations)
  {
    // initialize num_units and calculate based on the first strategy
    size_t num_units = 0;
    double weight;

    // Read in the first distribution to get num_units
    if (fscanf(in, "%lf", &weight) < 1)
      {
        fprintf(stderr, "Problem with weight\n");
        return NULL;
      }
    int *distro = malloc(sizeof(int) * num_locations);
    if (distro == NULL)
        {
          fprintf(stderr, "Malloc error\n");
          return NULL;
        }
    for (int i = 0; i < num_locations; i++)
        {
          fscanf(in, " %d", &distro[i]);
        }
    num_units = sum_array(distro, num_locations);
    strategy *s = strategy_create(num_locations, num_units); // initialize strategy
    strategy_add_distribution(s, distro, weight); // add first distribution to strategy
    free(distro);

    if (getc(in) != '\n')
      {
        fprintf(stderr, "Improper strategy formatting");
        return NULL;
      }
    
    // now we are on a new line, starting with the next strategy
    while (fscanf(in, "%lf", &weight) != EOF)
      {
      
        // first initiate a distribution array
        int *distro = malloc(sizeof(int) *num_locations);
        if (distro == NULL)
          {
            fprintf(stderr, "Malloc error");
            return NULL;
          }
        size_t check_units = 0;

        for (int i = 0; i < num_locations; i++)
          {
            // account for possibility that one strategy is too few; quit gracefully
            if (fscanf(in, " %d", &distro[i]) < 1)
              {
                fprintf(stderr, "Problem with fscanf!\n");
                free(distro);
                return NULL;
              }
            check_units += distro[i];
          }
        
        // once we're done reading in we check that size is ok
        if (num_units != check_units) // this means that the units are inconsistent
          {
            fprintf(stderr, "Unit mismatch!\n");
            free(distro);
            return NULL;
          }

        // now we add the completed distribution to the strategy 
        strategy_add_distribution(s, distro, weight);
        free(distro);
      }

    return s;
  }

int sum_array(int *arr, size_t num_locations)
{
  int sum = 0;
  for (int i = 0; i < num_locations; i++)
    {
      // add up elements of the array
      sum += arr[i];
    }
  return sum;
}