#ifndef __COOCCUR_H__
#define __COOCCUR_H__

#include <stdio.h>

struct cooccurrence_matrix;
typedef struct cooccurrence_matrix cooccurrence_matrix;

/**
 * Creates a cooccurrence matrix that counts cooccurrences of the
 * given keywords and is initialized to 0 for all entries.  The caller
 * retains ownership of the array of keywords and is responsible for
 * destroying the matrix.
 *
 * @param key an array of distinct non-NULL strings, non-NULL
 * @param n the size of that array
 * @return a pointer to a new cooccurrence matrix
 * destroying the matrix
 */
cooccurrence_matrix *cooccur_create(char *key[], size_t n);

/**
 * Updates the given cooccurrence matrix by incrementing the counts
 * for each pair of keywords in the given context.  The caller retains
 * ownership of the context.
 *
 * @param mat a pointer to a cooccurrence matrix, non-NULL
 * @param context an array of distinct non-NULL strings that are keywords
 * for that matrix, non-NULL
 * @param n the size of that array
 */
void cooccur_update(cooccurrence_matrix *mat, char **context, size_t n);

/**
 * Reads keywords from the given matrix from the current line of the
 * given stream.  A keyword is considered to have been read if it
 * appears on the stream before the next newline surrounded by
 * whitespace, the beginning of input (as seen by this function), or
 * EOF.  The newline is read and removed from the stream.
 *
 * @param mat a pointer to a cooccurrence matrix, non-NULL
 * @param stream a stream, non-NULL
 * @param n a pointer to where the size of
 * the returned array will be written
 * @return an array of unique non-NULL strings containing all the
 * keywords read; the caller is responsible for deallocating the array
 * and the strings it contains
 */
char **cooccur_read_context(cooccurrence_matrix *mat, FILE *stream, size_t *n);

/**
 * Returns the vector (row) for the given word in the given matrix.
 * Values in the returned array correspond to the keywords for the
 * given matrix in the order they were given in the array passed to
 * the call to cooccur_create that created the given matrix.  Each
 * value is the proportion of times that the given keyword appeared in
 * a call to cooccur_update for the given matrix that also contained
 * each other keyword.  If the given word is not a keyword for the
 * given matrix or the keyword has never appeared in a call to
 * cooccur_update for the given matrix, then the returned array
 * contains 0.0 in every entry.
 *
 * @param mat a pointer to a cooccurrence matrix, non-NULL
 * @param word a string, non-NULL
 * @return an array of doubles; it is the caller's responsibility
 * to free that array
 */
double *cooccur_get_vector(cooccurrence_matrix *mat, const char *word);

/**
 * Destroys the given matrix.
 * 
 * @param mat a pointer to a cooccurrence matrix, non-NULL
 */
void cooccur_destroy(cooccurrence_matrix *mat);

#endif
