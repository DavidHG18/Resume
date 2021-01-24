#ifndef __GMAP_TEST_FXNS__
#define __GMAP_TEST_FXNS__

#include <stdlib.h>
#include <stdio.h>

#include "gmap.h"

char **make_words(const char *prefix, size_t n);
char **make_words_select(char * const *words, size_t *indices, size_t n);
char **make_random_words(size_t len, size_t n);
char **make_words_concat(const char *prefix, const int *suffixes, size_t n);
char **copy_words(char * const *words, size_t n);
void free_words(char **arr, size_t n);
void free_values(gmap *m, char **arr, size_t n);
size_t java_hash_string(const void *p);

#define PRINT_PASSED fprintf(stdout, "PASSED\n")
#define PRINT_FAILED fprintf(stdout, "FAILED\n")


#endif

