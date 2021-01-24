#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gmap.h"
#include "gmap_test_functions.h"
#include "string_key.h"

void test_initial_size(size_t size, int on);
void test_get();
void test_get_time(size_t n, int on);
void test_size(size_t n);
void test_contains(size_t n);
void test_put_copies_key();
void test_put_does_not_copy_value();
void test_put_multiple_times();
void test_large_map(size_t n, size_t (*hash)(const void *));
void test_for_each(size_t n);
void test_put_time(size_t n, int on);
void test_for_each_time(size_t n, int on);
void test_uses_hash(size_t n);
void test_other_types();

size_t printing_hash_string(const void *s);

gmap *make_map(const char *prefix, size_t n, int value);
void add_keys(gmap *m, char * const *keys, size_t n, int value);
void add_keys_with_values(gmap *m, char * const *keys, size_t n, int *values);

void gmap_unit_free_value(const void *key, void *value, void *arg);

#define SMALL_TEST_SIZE 4
#define MEDIUM_TEST_SIZE 10000
#define LARGE_TEST_SIZE 1000000
#define VERY_LARGE_TEST_SIZE 10000000

int main(int argc, char **argv)
{
  int test = 0;
  size_t n = 0;
  int on = 0;

  if (argc > 1)
    {
      test = atoi(argv[1]);
    }
  if (argc > 2)
    {
      // test size and on/off
      if (atoi(argv[2]) < 0)
	{
	  fprintf(stderr, "%s: test size must be positive\n", argv[0]);
	  return 1;
	}
      n = atoi(argv[2]);
      on = atoi(argv[3]) == 1;
    }

  switch (test)
    {
    case 1:
      test_initial_size(0, on);
      break;

    case 2:
      test_uses_hash(SMALL_TEST_SIZE);
      break;
      
    case 3:
      test_get();
      break;

    case 4:
      test_size(SMALL_TEST_SIZE);
      break;

    case 5:
      test_contains(SMALL_TEST_SIZE);
      break;
      
    case 6:
      test_put_copies_key();
      break;

    case 7:
      test_put_does_not_copy_value();
      break;

    case 8:
      test_put_multiple_times();
      break;
      
    case 9:
      test_large_map(LARGE_TEST_SIZE, java_hash_string);
      break;

    case 10:
      test_for_each(SMALL_TEST_SIZE);
      break;
      
    case 11:
      test_other_types();
      break;
      
    case 12:
      test_put_time(n, on);
      break;

    case 13:
      test_get_time(n, on);
      break;

    case 14:
      test_for_each_time(n, on);
      break;

    default:
      fprintf(stderr, "USAGE: %s test-number\n", argv[0]);
    }
}

gmap *make_map(const char *prefix, size_t n, int value)
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);

  char **keys = make_words(prefix, n);
  add_keys(m, keys, n, value);
  free_words(keys, n);
  
  return m;
}

void add_keys(gmap *m, char * const *keys, size_t n, int value)
{
    for (int i = 0; i < n; i++)
    {
      int *temp = malloc(sizeof(int));
      *temp = value;
      gmap_put(m, keys[i], temp);
    }
}

void add_keys_with_values(gmap *m, char * const *keys, size_t n, int *values)
{
  for (size_t i = 0; i < n; i++)
    {
      gmap_put(m, keys[i], values + i);
    }
}

void test_get()
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);

  int twenty = 20;
  gmap_put(m, "Twenty", &twenty);

  if (*((int *)gmap_get(m, "Twenty")) == 20)
    {
      PRINT_PASSED;
    }
  else
    {
      PRINT_FAILED;
    }
  
  gmap_destroy(m);
}

void test_get_time(size_t n, int on)
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);
  char **keys = make_random_words(10, n);
  add_keys(m, keys, n, 1);

  if (on == 1)
    {
      for (int i = 0; i < n; i++)
	{
	  if (*((int *)gmap_get(m, keys[i])) != 1)
	    {
	      printf("FAILED -- retrieved value is incorrect\n");

	      free_values(m, keys, n);
	      gmap_destroy(m);
	      free_words(keys, n);
	      return;
	    }
	}
    }
  
  free_values(m, keys, n);
  gmap_destroy(m);
  free_words(keys, n);
}

void test_initial_size(size_t n, int on)
{
  int *dummy = malloc(sizeof(int));
  free(dummy);
  
  if (on)
    {
      gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);
      gmap_destroy(m);
    }
}

void test_size(size_t n)
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);

  if (gmap_size(m) != 0)
    {
      PRINT_FAILED;
      return;
    }
    
  char **keys = make_words("word", n);
  add_keys(m, keys, n, 0);

  if (gmap_size(m) != n)
    {
      printf("FAILED -- size is %lu after %lu puts\n", gmap_size(m), n);
      
      free_values(m, keys, n);
      gmap_destroy(m);
      free_words(keys, n);
      return;
    }

  gmap_put(m, "word0", gmap_get(m, "word0"));
  
  if (gmap_size(m) != n)
    {
      printf("FAILED -- size changed when putting duplicate key\n");

      free_values(m, keys, n);
      gmap_destroy(m);
      free_words(keys, n);
      return;
    }
  
  free_values(m, keys, n);
  gmap_destroy(m);
  free_words(keys, n);

  PRINT_PASSED;
}

void test_contains(size_t n)
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);
  char **keys = make_words("word", n);
  char **not_keys = make_words("worte", n);
  
  add_keys(m, keys, n, 0);

  for (int i = 0; i < n; i++)
    {
      if (!gmap_contains_key(m, keys[i]))
	{
	  printf("FAILED -- does not contain key %s\n", keys[i]);
	  
	  free_values(m, keys, n);
	  gmap_destroy(m);
	  free_words(keys, n);
	  free_words(not_keys, n);
	  return;
	}
      if (gmap_contains_key(m, not_keys[i]))
	{
	  printf("FAILED -- contains non-key %s\n", not_keys[i]);
	  
	  free_values(m, keys, n);
	  gmap_destroy(m);
	  free_words(keys, n);
	  free_words(not_keys, n);
	  return;
	}
    }
  
  free_values(m, keys, n);
  gmap_destroy(m);
  free_words(keys, n);
  free_words(not_keys, n);
  
  PRINT_PASSED;
}

void test_put_copies_key()
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);
  char *key = malloc(sizeof(char) * (strlen("Twenty") + 1));
  strcpy(key, "Twenty");

  int twenty = 20;
  gmap_put(m, key, &twenty);
  strcpy(key, "Plenty");

  if (!gmap_contains_key(m, "Twenty"))
    {
      printf("FAILED -- key in map lost\n");
      
      gmap_destroy(m);
      free(key);
      return;
    }
  else if (gmap_contains_key(m, "Plenty"))
    {
      printf("FAILED -- key in map changed\n");
      
      gmap_destroy(m);
      free(key);
      return;
    }
  else if (*((int *)gmap_get(m, "Twenty")) != 20)
    {
      printf("FAILED -- incorrect value for key\n");
      
      gmap_destroy(m);
      free(key);
      return;
    }
  
  gmap_destroy(m);
  free(key);
  PRINT_PASSED;
}

void test_put_does_not_copy_value()
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);

  int twenty = 20;
  gmap_put(m, "Twenty", &twenty);
  twenty++;

  if (*((int *)gmap_get(m, "Twenty")) != 21)
    {
      printf("FAILED -- value did not change\n");
      
      gmap_destroy(m);
      return;
    }

  gmap_destroy(m);
  PRINT_PASSED;
}

void test_put_multiple_times()
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);

  int *values = malloc(sizeof(int) * 20);
  for (int i = 0; i < 20; i++)
    {
      values[i] = i + 1;
      gmap_put(m, "Twenty", values + i);
    }

  if (*((int *)gmap_get(m, "Twenty")) != 20)
    {
      printf("FAILED -- value did not update after multiple puts\n");
      
      gmap_destroy(m);
      free(values);
      return;
    }

  gmap_destroy(m);
  free(values);

  PRINT_PASSED;
}
  
void test_large_map(size_t n, size_t (*hash)(const void *))
{
  gmap *m = gmap_create(duplicate, compare_keys, hash, free);
  char **keys = make_words("word", n);
  char **not_keys = make_words("wort", n);

  for (int i = 0; i < n; i++)
    {
      int *value = malloc(sizeof(int));
      *value = n - i;
      gmap_put(m, keys[i], value);
    }

  if (gmap_size(m) != n)
    {
      printf("FAILED -- size is %lu; should be %lu\n", gmap_size(m), n);
      
      free_values(m, keys, n);
      gmap_destroy(m);
      free_words(keys, n);
      free_words(not_keys, n);
      return;
    }

  for (size_t i = 0; i < n; i++)
    {
      if (!gmap_contains_key(m, keys[i]))
	{
	  printf("FAILED -- map missing key %s\n", keys[i]);
    int *value = gmap_get(m, keys[i]);
	  
	  free_values(m, keys, n);
	  gmap_destroy(m);
	  free_words(keys, n);
	  free_words(not_keys, n);
	  return;
	}
      else if(*((int *)gmap_get(m, keys[i])) != n - i)
	{
	  printf("FAILED -- value for key %s is not %lu\n", keys[i], n - i);
	  
	  free_values(m, keys, n);
	  gmap_destroy(m);
	  free_words(keys, n);
	  free_words(not_keys, n);
	  return;
	}
      else if(gmap_contains_key(m, not_keys[i]))
	{
	  printf("FAILED -- map contains non-key %s\n", not_keys[i]);
	  
	  free_values(m, keys, n);
	  gmap_destroy(m);
	  free_words(keys, n);
	  free_words(not_keys, n);
	  return;
	}
    }

  free_values(m, keys, n);
  gmap_destroy(m);
  free_words(keys, n);
  free_words(not_keys, n);
  
  PRINT_PASSED;
}

int sum;

void sum_keys_and_values(const void *key, void *value, void *p)
{
  int *sum = p;
  *sum += atoi(key);
  *sum += *((int *)value);
}

void test_for_each(size_t n)
{
  int sum = 0;
  gmap *m = make_map("", n, 2);
  gmap_for_each(m, sum_keys_and_values, &sum);
  
  if (sum != n * (n - 1) / 2 + 2 * n)
    {
      printf("FAILED -- sum of keys and values is %d\n", sum);
      
    }

  gmap_for_each(m, gmap_unit_free_value, NULL);

  gmap_destroy(m);
  PRINT_PASSED;
}

void test_put_time(size_t n, int on)
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);
  char **keys = make_random_words(10, n);
  int *values = calloc(n, sizeof(int));
  
  if (on == 1)
    {
      add_keys_with_values(m, keys, n, values);
    }

  free(values);
  gmap_destroy(m);
  free_words(keys, n);
}

void count_keys(const void *key, void *value, void *p)
{
  (*((int *)p))++;
}

void test_for_each_time(size_t n, int on)
{
  gmap *m = gmap_create(duplicate, compare_keys, java_hash_string, free);
  char **keys = make_random_words(10, n);
  int *values = calloc(n, sizeof(int));
  add_keys_with_values(m, keys, n, values);

  int sum = 0;

  if (on == 1)
    {
      gmap_for_each(m, count_keys, &sum);
      if (sum != n)
	{
	  printf("FAILED -- iterated over %d keys\n", sum);
	  
	  free(values);
	  free_words(keys, n);
	  gmap_destroy(m);
	  return;
	}
    }

  free(values);
  free_words(keys, n);
  gmap_destroy(m);
}

void gmap_unit_free_value(const void *key, void *value, void *arg)
{
  free(value);
}

void test_uses_hash(size_t n)
{
  gmap *m = gmap_create(duplicate, compare_keys, printing_hash_string, free);

  char **keys = make_words("word", n);
  int *values = calloc(n, sizeof(int));
  for (int i = 0; i < n; i++)
    {
      gmap_put(m, keys[i], values + i);
    }

  int *value = malloc(sizeof(int));
  *value = 42;
  printf("And now a message from your friendly local hash function: ");
  gmap_put(m, "magic", value);
  
  gmap_destroy(m);
  free_words(keys, n);
  free(values);
  free(value);
}

size_t printing_hash_string(const void *s)
{
  if (strcmp("magic", s) == 0)
    {
      printf("hello!\n");
    }

  return java_hash_string(s);
}


typedef struct _pair
{
  int x;
  int y;
} pair;

void *copy_pair(const void *ptr)
{
  const pair *p = ptr;

  pair *copy = malloc(sizeof(pair));
  *copy = *p;
  return copy;
}

int compare_pairs(const void *p1, const void *p2)
{
  const pair *pair1 = p1;
  const pair *pair2 = p2;

  if (pair1->x != pair2->x)
    {
      return pair1->x - pair2->x;
    }
  else
    {
      return pair1->y - pair2->y;
    }
}

size_t hash_pair(const void *p)
{
  const pair *pair = p;

  // this is not intended to be good
  return pair->x + pair->y;
}
  

void test_other_types()
{
  gmap *m = gmap_create(copy_pair, compare_pairs, hash_pair, free);

  pair key = {10, 23};
  pair not_key = {23, 10};
  char *value = "value";

  gmap_put(m, &key, value);

  if (!gmap_contains_key(m, &key))
    {
      printf("FAILED -- key is missing\n");
      gmap_destroy(m);
      return;
    }
  else if (gmap_contains_key(m, &not_key))
    {
      printf("FAILED -- map contains non-key\n");
      gmap_destroy(m);
      return;
    }
  else if (gmap_get(m, &key) != value)
    {
      printf("FAILED -- incorrect value\n");
      gmap_destroy(m);
      return;
    }

  gmap_destroy(m);
  PRINT_PASSED;
}
