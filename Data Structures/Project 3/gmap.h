#ifndef __GMAP_H__
#define __GMAP_H__

#include <stdlib.h>
#include <stdbool.h>

struct gmap;
typedef struct gmap gmap;

/**
 * Creates an empty map that uses the given hash function.
 *
 * @param cp a function that take a pointer to a key and returns a pointer to a deep copy of that key
 * @param comp a pointer to a function that takes two keys and returns the result of comparing them,
 * with return value as for strcmp
 * @param h a pointer to a function that takes a pointer to a key and returns its hash code
 * @param f a pointer to a function that takes a pointer to a copy of a key make by cp and frees it
 * @return a pointer to the new map or NULL if it could not be created;
 * it is the caller's responsibility to destroy the map
 */
gmap *gmap_create(void *(*cp)(const void *), int (*comp)(const void *, const void *), size_t (*h)(const void *s), void (*f)(void *));

/**
 * Returns the number of (key, value) pairs in the given map.
 *
 * @param m a map, non-NULL
 * @return the size of m
 */
size_t gmap_size(const gmap *m);

/**
 * Adds a copy of the given key with value to this map.
 * If the key is already present then the old value is replaced.
 * The caller retains ownership of the value.  Note that if replacing the old value would
 * cause resource leaks, then the caller should retrieve the old value with get and release
 * its resources before calling put to replace it.
 *
 * @param m a map, non-NULL
 * @param key a pointer to a key, non-NULL
 * @param value a pointer to an integer
 * @return true if the put was successful, false otherwise
 */
bool gmap_put(gmap *m, const void *key, void *value);

/**
 * Determines if the given key is present in this map.
 *
 * @param m a map, non-NULL
 * @param key a pointer to a key, non-NULL
 * @return true if key is present in this map, false otherwise
 */
bool gmap_contains_key(const gmap *m, const void *key);

/**
 * Returns the value associated with the given key in this map.
 * If the key is not present in this map then the returned value is
 * NULL.  The value returned is the original value passed to gmap_put,
 * and it remains the responsibility of whatever called gmap_put to
 * release the value (no ownership transfer results from gmap_get).
 *
 * @param m a map, non-NULL
 * @param key a pointer to a key, non-NULL
 * @return the assocated value, or NULL if they key is not present
 */
void *gmap_get(gmap *m, const void *key);

/**
 * Calls the given function for each (key, value) pair in this map, passing
 * the extra argument as well.
 *
 * @param m a map, non-NULL
 * @param f a pointer to a function that takes a key, a value, and an
 * extra piece of information, non-NULL
 * @param arg a pointer
 */
void gmap_for_each(gmap *m, void (*f)(const void *, void *, void *), void *arg);

/**
 * Destroys the given map.
 *
 * @param m a map, non-NULL
 */
void gmap_destroy(gmap *m);

#endif
