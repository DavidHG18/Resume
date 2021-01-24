#ifndef __DECISION_TREE__
#define __DECISION_TREE__

#include <string.h>
#include <stdbool.h>

typedef struct _decision_tree decision_tree;

typedef struct _dt_iterator dt_iterator;

/**
 * Creates a decision tree with the given class (guess) in the root.
 * Returns NULL if there was a memory alloction error.  It is the caller's
 * responsibility to eventually destroy the returned tree if it is not
 * NULL.  The caller retains ownership of the string passed in.
 *
 * @param s a pointer to a string, non-NULL
 * @return a pointer to a decision tree, or NULL
 */
decision_tree *dt_create(const char *s);

/**
 * Creates and returns an iterator positioned at the root of the given
 * tree.  Returns NULL if there was a memory allocation error.  It is
 * the caller's responsibility to eventually destroy the returned iterator
 * if it is not NULL.
 *
 * @param t a pointer to a decision tree, non-NULL
 */
dt_iterator *dt_iterator_create(decision_tree *t);

/**
 * Determines if the given iterator is positioned at a leaf.
 *
 * @param i a pointer to an iterator, non-NULL
 * @return true if and only if i is at a leaf
 */
bool dt_iterator_at_leaf(const dt_iterator *i);

/**
 * Moves the given iterator so it is positioned at the left child of the
 * node it is currently positioned at.
 *
 * @param i a pointer to an iterator positioned at a non-left, non-NULL
 */
void dt_iterator_move_left(dt_iterator *i);

/**
 * Moves the given iterator so it is positioned at the right child of the
 * node it is currently positioned at.
 *
 * @param i a pointer to an iterator positioned at a non-left, non-NULL
 */
void dt_iterator_move_right(dt_iterator *i);

/**
 * Returns the test (question) at the current position of the
 * given iterator.  The tree retains ownership of the returned string.
 *
 * @param i a pointer to an iterator positioned at a non-leaf, non-NULL
 * @return a pointer to a string
 */
const char *dt_iterator_get_test(const dt_iterator *i);

/**
 * Returns the class (guess) at the current position of the given iterator.
 * The tree retains ownership of the returned string.
 *
 * @param i a pointer to an iterator positioned at a leaf, non-NULL
 * @return a pointer to a string
 */
const char *dt_iterator_get_class(const dt_iterator *i);

/**
 * Modifies the tree this iterator is in by adding the given test (question)
 * at its current position, with the class at that position as the
 * right (no) branch and the new class as the left (yes) branch.
 * The caller retains ownership of the strings passed in as the test
 * and new class.  The return value indicates whether the operation
 * was successful (there were no memory allocation errors).
 * The iterator is positioned at the new test node if the tree was
 * successfully modified and stays at its current position otherwise.
 * The future behavior of any other iterators that were positioned at
 * the old leaf is undefined.
 *
 * @param i a pointer to an iterator positioned at a leaf, non-NULL
 * @param test a pointer to a string, non-NULL
 * @param class a pointer to a string, non-NULL
 * @return true if and only if the operation was successful
 */
bool dt_iterator_add_branch_left(dt_iterator *i, const char *test, const char *class);

/**
 * Modifies the tree this iterator is in by adding the given test (question)
 * at its current position, with the class at that position as the
 * left (yes) branch and the new class as the right (no) branch.
 * The caller retains ownership of the strings passed in as the test
 * and new class.  The return value indicates whether the operation
 * was successful (there were no memory allocation errors).
 * The iterator is positioned at the new test node if the tree was
 * successfully modified and stays at its current position otherwise.
 * The future behavior of any other iterators that were positioned at
 * the old leaf is undefined.
 *
 * @param i a pointer to an iterator positioned at a leaf, non-NULL
 * @param test a pointer to a string, non-NULL
 * @param class a pointer to a string, non-NULL
 * @return true if and only if the operation was successful
 */
bool dt_iterator_add_branch_right(dt_iterator *i, const char *test, const char *class);

/**
 * Destroys the given iterator, releasing its resources.
 *
 * @param i a pointer to an iterator, non-NULL
 */
void dt_iterator_destroy(dt_iterator *i);

/**
 * Destroys the given tree, releasing its resources.  Any remaining
 * iterators in the tree become invalid.
 *
 * @param i a pointer to an iterator, non-NULL
 */
void dt_destroy(decision_tree *t);


#endif
