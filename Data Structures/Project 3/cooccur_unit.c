#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "gmap_test_functions.h"

#include "cooccur.h"

cooccurrence_matrix *make_matrix(const char *prefix, size_t size);
cooccurrence_matrix *make_matrix_keywords(char * const *keys, size_t size);
bool compare_string_arrays(char **a1, int sz1, char **a2, int sz2);

void test_create();
void test_update_all_keywords(size_t size);
void test_update_mixed_words(size_t size);
void test_read_all_keywords(size_t size, FILE *in);
void test_read_mixed_words(size_t size, FILE *in);
void test_get_returns_copy(size_t size);

void test_create_duplicate_keywords(size_t size);

int main(int argc, char **argv)
{
  int test = 0;
  size_t size = 1;
  
  if (argc > 1)
    {
      test = atoi(argv[1]);
    }

  if (argc > 2)
    {
      size = atoi(argv[2]);
      if (size < 1)
	{
	  fprintf(stderr, "USAGE: %s test-number matrix-size\n", argv[0]);
	}
    }

  switch (test)
    {
    case 1:
      test_create();
      break;
      
    case 2:
      test_update_all_keywords(size);
      break;
      
    case 3:
      test_update_mixed_words(size);
      break;
            
    case 4:
      test_read_all_keywords(size, stdin); // must write all keywords to stdin
      break;
      
    case 5:
      test_read_mixed_words(size, stdin); // must write all keywords to stdin with non-keywords mixed in
      break;
      
    case 6:
      test_get_returns_copy(size);
      break;
      
    default:
      fprintf(stderr, "USAGE: %s test-number [matrix-size]\n", argv[0]);
    }
}

cooccurrence_matrix *make_matrix(const char *prefix, size_t size)
{
  char **keys = make_words(prefix, size);
  cooccurrence_matrix *m = cooccur_create(keys, size);
  free_words(keys, size);
  
  return m;
}

cooccurrence_matrix *make_matrix_keywords(char * const *keys, size_t size)
{
  char **copy = copy_words(keys, size);
  cooccurrence_matrix *m = cooccur_create(copy, size);
  free_words(copy, size);
  
  return m;
}

void test_create()
{
  char *keys[] = { "word0", "word1", "word2" };
  cooccurrence_matrix *m = cooccur_create(keys, 3);

  cooccur_destroy(m);
  PRINT_PASSED;
}

/**
 * Tests that a given double is equal to a given quotient with some
 * room for the imprecision of floating point numbers.
 */
bool test_ratio(double x, int num, int denom)
{
  return (int)(x * denom + 0.000001) == num;
}

void test_update_all_keywords(size_t size)
{
  cooccurrence_matrix *m = make_matrix("word", size);
  char **keys = make_words("word", size);

  // update with "word0" then "word0 word1", "word0 word1 word2" ...
  for (size_t i = 1; i <= size; i++)
    { 
      // make context from scratch each time in case students did something untoward to it
      char **context = make_words("word", i);
      cooccur_update(m, context, i);
      free_words(context, i);
    }

  // test first keyword (1, n-1/n, ..., 1/n)
  double *vec0 = cooccur_get_vector(m, keys[0]);
  for (size_t i = 0; i < size; i++)
    {
      if (!test_ratio(vec0[i], size - i, size))
	{
	  printf("FAILED -- ratio is %lf\n", vec0[i]);

	  free_words(keys, size);
	  free(vec0);
	  cooccur_destroy(m);
	  return;
	}
    }
  free(vec0);

  // test last keyword (all 1.0)
  double *vec_last = cooccur_get_vector(m, keys[size - 1]);
  for (size_t i = 0; i < size; i++)
    {
      if (!test_ratio(vec_last[i], 1, 1))
	{
	  printf("FAILED -- ratio is %lf\n", vec_last[i]);
	
	  free_words(keys, size);
	  free(vec_last);
	  cooccur_destroy(m);
	  return;
	}
    }
  free(vec_last);
  
  free_words(keys, size);
  cooccur_destroy(m);
  
  PRINT_PASSED;
}

void test_update_mixed_words(size_t size)
{
  cooccurrence_matrix *m = make_matrix("word", size);
  char **keys = make_words("word", size);
  
  // update with "word0" then "word0 word1", "word0 word1 word2" ... "word0 ... wordsize"
  // (note "wordsize" is not a keyword)
  for (size_t i = 1; i <= size + 1; i++)
    {
      // make context from scratch each time in case students did something untoward to it
      char **context = make_words("word", i);
      cooccur_update(m, context, i);
      free_words(context, i);
    }
  // test first keyword (1, n-1/n, ..., 1/n)
  double *vec0 = cooccur_get_vector(m, keys[0]);
  for (int i = 0; i < size; i++)
    {
      if (!test_ratio(vec0[i], size - i, size))
	{
	  printf("FAILED -- ratio is %lf\n", vec0[i]);

	  free(vec0);
	  free_words(keys, size);
	  cooccur_destroy(m);
	  return;
	}
    }
  free(vec0);

  // test last keyword (all 1.0)
  double *vec_last = cooccur_get_vector(m, keys[size - 1]);
  for (size_t i = 0; i < size; i++)
    {
      if (!test_ratio(vec_last[i], 1, 1))
	{
	  printf("FAILED -- ratio is %lf\n", vec_last[i]);

	  free(vec_last);
	  free_words(keys, size);
	  cooccur_destroy(m);
	  return;
	}
    }

  free(vec_last);
  free_words(keys, size);
  cooccur_destroy(m);
  
  PRINT_PASSED;
}

void test_read_all_keywords(size_t size, FILE *in)
{
  char **words = make_words("word", size);
  cooccurrence_matrix *m = make_matrix_keywords(words, size);

  // select all keywords
  size_t *select = malloc(sizeof(size_t) * size);
  for (int i = 0; i < size; i++)
    {
      select[i] = i;
    }

  // make file containing "word0 word1..."
  char **context_correct = make_words_select(words, select, size);

  // make file containing context
  //FILE *in = make_file("temp_context", words, select, size);
  free_words(words, size);
  free(select);

  // read context from file
  size_t context_size = 0;
  char **context_read = cooccur_read_context(m, in, &context_size);
  //fclose(in);

  // check result
  if (!compare_string_arrays(context_correct, size, context_read, context_size))
    {
      PRINT_FAILED;
      free_words(context_correct, size);
      free_words(context_read, context_size);
      cooccur_destroy(m);
      return;
    }

  free_words(context_correct, size);
  free_words(context_read, context_size);
  cooccur_destroy(m);
  PRINT_PASSED;
}

void test_read_mixed_words(size_t size, FILE *in)
{
  char **words = make_words("word", size * 2); // make extra words for non-keywords
  cooccurrence_matrix *m = make_matrix_keywords(words, size);

  // select "word0 wordsize+1 word1 ..."
  size_t *select = malloc(sizeof(size_t) * size * 2);
  for (size_t i = 0; i < size * 2; i++)
    {
      if (i % 2 == 0)
	{
	  select[i] = i / 2;
	}
      else
	{
	  select[i] = size + i;
	}
    }

  // make context "word0 wordsize+1 word1..."
  char **context_correct = copy_words(words, size);

  // make file with that context
  //FILE *in = make_file("temp_context", words, select, size);
  free(select);
  free_words(words, size * 2);

  // read from file
  size_t context_size = 0;
  char **context_read = cooccur_read_context(m, in, &context_size);
  //fclose(in);

  // check result
  if (!compare_string_arrays(context_correct, size, context_read, context_size))
    {
      PRINT_FAILED;
      free_words(context_correct, size);
      free_words(context_read, context_size);
      cooccur_destroy(m);
      return;
    }

  free_words(context_correct, size);
  free_words(context_read, context_size);
  cooccur_destroy(m);

  PRINT_PASSED;
}


void test_get_returns_copy(size_t size)
{
  char **keys = make_words("word", size);
  cooccurrence_matrix *m = make_matrix_keywords(keys, size);

  // do an update to avoid potential divide by zero
  char **context = copy_words(keys, 1);
  cooccur_update(m, context, 1);
  free_words(context, 1);
  
  // get vector, write to it, get it again and make sure still correct
  double *vec0 = cooccur_get_vector(m, keys[0]);
  vec0[0] = 0.0;
  double *vec1 = cooccur_get_vector(m, keys[0]);
  if (vec1[0] != 1.0)
    {
      printf("FAILED -- cooccur_get_vector aliases array\n");
      free(vec0);
      free(vec1);
      free_words(keys, size);
      cooccur_destroy(m);
      return;
    }

  free(vec0);
  free(vec1);
  free_words(keys, size);
  cooccur_destroy(m);
  
  PRINT_PASSED;
}

int compare_strings(const void *p1, const void *p2)
{
  const char * const *s1 = p1;
  const char * const *s2 = p2;

  return strcmp(*s1, *s2);
}

bool compare_string_arrays(char **a1, int sz1, char **a2, int sz2)
{
  if (sz1 != sz2)
    {
      return false;
    }
  else
    {
      qsort(a1, sz1, sizeof(char *), compare_strings);
      qsort(a2, sz2, sizeof(char *), compare_strings);

      for (int i = 0; i < sz1; i++)
	{
	  if (strcmp(a1[i], a2[i]) != 0)
	    {
	      return false;
	    }
	}

      return true;
    }
}
