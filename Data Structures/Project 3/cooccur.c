#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "gmap.h"
#include "cooccur.h"
#include "string_key.h"


// Strategy to deal with seg faults
/**
 * EXIT STATEMENTS
 * exit(0);
*/
/**
 * QUESTIONS:
 * 2) Segmentation faults with GmapUnit
 * 3) Recursive functions with tree structs not working properly?
 * 4) Rotation functions
 * */

struct cooccurrence_matrix // will include two gmaps
{
  // will have n rows and n columns of doubles initialized to zero
  double **data;
  size_t size;
  size_t max_length;
  char **words;
  gmap *word_map;
  // mapping keys to where they are indexed in the data array

}; 


cooccurrence_matrix *cooccur_create(char *key[], size_t n)
{
  cooccurrence_matrix *result = malloc(sizeof(cooccurrence_matrix));
  if (result == NULL)
        {
          fprintf(stderr, "ERROR creating matrix");
          return NULL;
        }
  result->size = n;
  result->data = malloc(sizeof(double *) * n);
  result->max_length = 0;
  result->word_map = gmap_create(duplicate, compare_keys, hash29, free);
  result->words = malloc(sizeof(char *) * n);
  if (result->word_map == NULL || result->data == NULL)
        {
          fprintf(stderr, "ERROR creating gmap");
          return NULL;
        }

  // iterate through all keys and put them into the map with values initialized at 0
  for (int i = 0; i < n; i++)
    {
      double *row = calloc(n, sizeof(double));
      int *index = malloc(sizeof(int));
      if (index == NULL || row == NULL)
        {
          free(index);
          free(row);
          fprintf(stderr, "ERROR creating array or copy of string");
          return NULL;
        }
      
      result->data[i] = row;
      *index = i;
      // update max length in case this string is the longest yet in the matrix
      if (strlen(key[i]) > result->max_length)
        {
          result->max_length = strlen(key[i]);
        }
      gmap_put(result->word_map, key[i], index); // mapping keys to indices in the data array
      result->words[i] = malloc(sizeof(char) * (strlen(key[i]) + 1));
      if (result->words[i] == NULL)
        {
          fprintf(stderr, "ERROR in gmap_create");
          return NULL;
        }
      strcpy(result->words[i], key[i]);
    }

  return result;
}

void cooccur_update(cooccurrence_matrix *mat, char **context, size_t n)
{

  if (!mat || !context)
    {
      fprintf(stderr, "ERROR with either cooccurrence matrix or context provided");
      free(mat);
      free(context);
      return;
    }
  for (size_t i = 0; i < n; i++)
    {
      // make copies of keys
      char *word1 = context[i];
      if (gmap_contains_key(mat->word_map, word1))
        {
          int *index1 = gmap_get(mat->word_map, word1);
          mat->data[*index1][*index1]+= 2;
          for (size_t j = 0; j < n; j++)
          {
            char *word2 = context[j];
            if (gmap_contains_key(mat->word_map, word2))
              {
                // grab index from word map
                int *index2 = gmap_get(mat->word_map, word2);
                if (!index1 || !index2)
                  {
                    fprintf(stderr, "ERROR with gmap_get function in cooccur update");
                    return;
                  }
                if (*index1 == *index2)
                  {
                    continue;
                  }

                // increment corresponding values of the matrix
                mat->data[*index1][*index2]++;
                mat->data[*index2][*index1]++;              
              }
          }
        }
    }
}

char **cooccur_read_context(cooccurrence_matrix *mat, FILE *stream, size_t *n)
{
  if (!mat || !stream)
    {
      fprintf(stderr, "ERROR can't find matrix or stream in cooccur_read_context");
      return NULL;
    }
  // initialize string matrix to size of array (setting values equal to zero first so we can tell where end of array is)
  char **answer = malloc(sizeof(char *) * mat->size);
  if (answer == NULL)
    {
      fprintf(stderr, "ERROR allocating memory for return array in cooccur_read_context");
      return NULL;
    }
  
  // allocate all of answer's rows into a buffer
  for (size_t i = 0; i < mat->size; i++)
    {
      answer[i] = calloc(mat->max_length + 1, sizeof(char));
      if (answer[i] == NULL)
        {
          for (size_t j = 0; j < i; j++)
            {
              free(answer[j]);
            }
          cooccur_destroy(mat);
          fprintf(stderr, "ERROR allocating memory for return array in cooccur_read_context");
          return NULL;
        }
    }
  
  char c;
  size_t index = 0;
  size_t counter = 0;
  bool dead = false;
  bool contains = false;
  
  // start reading through stream until you get to the end of the line
  while (fscanf(stream, "%c", &c) > 0 && c != '\n' && index < mat->size)
    {
      if (!isspace(c) && dead == true)
        {
          continue;
        }
      // if we have a space or the count exceeds max count, then we need to check if the word is in the word_map
      else if (isspace(c))
        {
          dead = false;
          answer[index][counter] = '\0'; // end string before comparison

          if (gmap_contains_key(mat->word_map, answer[index]))
            {
              contains = false;
              // first check to see if word is already in the answer array
              for (size_t j = 0; j < index; j++)
                {
                  if (strcmp(answer[j], answer[index]) == 0)
                    {
                      contains = true;
                    }
                }
              
              // if not already in the array, add and continue
              if (contains == false)
                {
                  index++;
                }
              else 
                {
                  free(answer[index]);
                  answer[index] = calloc(mat->max_length + 1, sizeof(char));
                  if (answer[index] == NULL)
                    {
                      for (size_t i = 0; i < index; i++)
                        {
                          free(answer[i]);
                        }
                    }
                }
            }
          counter = 0;
        }
      else if (counter >= mat->max_length)
        {
          // we have exceeded the max word length and do not have an end of string; go into dead state and reset counter
          free(answer[index]);
          answer[index] = calloc(mat->max_length + 1, sizeof(char));
          if (answer[index] == NULL)
            {
              for (size_t i = 0; i < index; i++)
                {
                  free(answer[i]);
                }
            }
          dead = true;
          counter = 0;
        }
      else // in this case we are validly reading a character, so we just add it to the current place in answer
        {
          answer[index][counter] = c;
          counter++;
        }
    }


  // post while loop: first check last string, then ree all indices beyond what has been entered
if (index < mat->size && dead == false && gmap_contains_key(mat->word_map, answer[index]))
  {
    contains = false;
    // first check to see if word is already in the answer array
    for (size_t j = 0; j < index; j++)
      {
        if (strcmp(answer[j], answer[index]) == 0)
          {
            contains = true;
          }
      }
    
    // if already in the array, then free the index and try again
    if (contains == false)
      {
        index++;
      }
  }

// free all remaining indices
  for (size_t i = index; i < mat->size; i++)
    {
      free(answer[i]);

    }

// set n to the current index level and return the answer array
*n = index;
return answer;
}

double *cooccur_get_vector(cooccurrence_matrix *mat, const char *word)
{

  if (!mat || !word)
    {
      fprintf(stderr, "ERROR finding matrix or word");
      return NULL;
    }
  
  double *return_array = calloc(mat->size, sizeof(double));

  if (!return_array)
    {
      cooccur_destroy(mat);
      fprintf(stderr, "ERROR key not found in map");
      return NULL;
    }

  if (!gmap_contains_key(mat->word_map, word))
    {
      return return_array;
    }
  
  // grab the index from gmap and the row corresponding to that index
  int *index = gmap_get(mat->word_map, word);
  double *word_row = mat->data[*index];

  if (word_row[*index] == 0)
    {
      return return_array;
    }
  
  // update return array to hold all proportions
  for (size_t i = 0; i < mat->size; i++)
    {
      return_array[i] = word_row[i] / word_row[*index];
    }
  
  return return_array;
}

void cooccur_destroy(cooccurrence_matrix *mat)
{
  for (size_t i = 0; i < mat->size; i++)
    {
      free(mat->data[i]);
      int *value = gmap_get(mat->word_map, mat->words[i]);
      //printf("%s%d\n", mat->words[i], *value);
      free(value);
      free(mat->words[i]);
    }
  free(mat->words);
  free(mat->data);
  gmap_destroy(mat->word_map);
  free(mat);
}