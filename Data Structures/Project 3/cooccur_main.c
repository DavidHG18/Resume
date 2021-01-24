#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cooccur.h"
#include "gmap.h"




void matrix_output(cooccurrence_matrix *mat, char **word_array, size_t n); // output all vectors in a matrix
void arrays_free(char **array, size_t n); // frees all words in an array

int main(int argc, char **argv)
{
    // read in command line arguments
    if (argc < 2)
        {
            fprintf(stderr, "ERROR need to enter words for the cooccurrence matrix");
            return 1;
        }
    // initialize string array (to be freed after calling the function)
    char **word_array = malloc(sizeof(char *) * (argc - 1));
    if (word_array == NULL)
        {
            fprintf(stderr, "ERROR with allocating space for word array");
            return 1;
        }
    for (size_t i = 1; i < argc; i++) // read words into the matrix
        {
            // invalid input of words
            word_array[i-1] = argv[i];
        }
    
    // initialize cooccurrence matrix
    cooccurrence_matrix *mat = cooccur_create(word_array, argc - 1);
    if (!mat)
        {
            fprintf(stderr, "ERROR creating cooccurrence matrix");
            free(word_array);
            return 1;
        }
   

    // read input from stdin then update cooccurrence matrix until the stream has ended
    char c;
    while ((c = fgetc(stdin)) != EOF)
        {
            ungetc(c, stdin);
            size_t n = 0;
            char **context = cooccur_read_context(mat, stdin, &n);
            if (!context)
                {
                    fprintf(stderr, "ERROR issue in main with read_context");
                    free(word_array);
                    cooccur_destroy(mat);
                    return 1;
                }
            /*
            for (size_t i = 0; i < n; i++)
                {
                    fprintf(stdout, "%s\n", context[i]);
                }
            fprintf(stdout, "\n");
            */
            cooccur_update(mat, context, n);
            arrays_free(context, n);
        }

    // output the vectors
    matrix_output(mat, word_array, argc - 1);

    // free word array and matrix
    free(word_array);
    cooccur_destroy(mat);

    return 0;

}


void matrix_output(cooccurrence_matrix *mat, char **word_array, size_t n) // output all vectors in a matrix
{
    for (size_t i = 0; i < n; i++)
        {
            fprintf(stdout, "%s: [", word_array[i]);
            double *vector = cooccur_get_vector(mat, word_array[i]);
            if (!vector)
                {
                    fprintf(stderr, "ERROR with cooccur_get_vector");
                    return;
                }
            for (size_t j = 0; j < n - 1; j++)
                {
                    fprintf(stdout, "%lf, ", vector[j]);
                }
            fprintf(stdout, "%lf]\n", vector[n-1]);
            free(vector);
        }
}

void arrays_free(char **array, size_t n)
{
    for (size_t i = 0; i < n; i++)
        {
            free(array[i]);
        }
    free(array);
}