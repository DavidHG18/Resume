#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "gmap.h"

#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef struct _node
{
  void *key;
  void *value;
  struct _node *left;
  struct _node *right;
  size_t left_height;
  size_t right_height;
} node;

struct gmap
{
  size_t capacity;
  size_t size;
  node **table;
  void *(*copy)(const void *);
  int (*compare)(const void *, const void *);
  size_t (*hash)(const void *);
  void (*free)(void *);
};

#define GMAP_INITIAL_CAPACITY 100

size_t gmap_compute_index(const void *key, size_t (*hash)(const void *), size_t size);
void gmap_embiggen(gmap *m, size_t n);
void gmap_table_add(node **table, node *n, int (*compare)(const void *, const void *), size_t (*hash)(const void *), size_t capacity);
node *gmap_table_find_key(node **table, const void *key, int (*compare)(const void *, const void *), size_t (*hash)(const void *), size_t capacity);
void recursive_add(node **bigger, node *n, int (*compare)(const void *, const void *), size_t (*hash)(const void *), size_t capacity); // function to complement embiggen; recursively adds nodes to new table
void recursive_destroy(node *n); // function to recursively destroy each node in the AVL trees of hash table
void recursive_apply(node *n, void (*f)(const void *, void *, void *), void *arg); // funciton to recursively apply a function to all nodes in an AVL tree
node *recursive_table_add(node *curr, node *n, int (*compare)(const void *, const void *));
node *double_rotate_right(node *n);
node *double_rotate_left(node *n);
node *rotate_right(node *n);
node *rotate_left(node *n);

gmap *gmap_create(void *(*cp)(const void *), int (*comp)(const void *, const void *), size_t (*h)(const void *s), void (*f)(void * k))
{
  gmap *result = malloc(sizeof(gmap));
  if (result != NULL)
    {
      result->size = 0;
      result->copy = cp;
      result->compare = comp;
      result->hash = h;
      result->free = f;
      result->table = malloc(sizeof(node *) * GMAP_INITIAL_CAPACITY);
      result->capacity = (result->table != NULL ? GMAP_INITIAL_CAPACITY : 0);
      for (size_t i = 0; i < result->capacity; i++)
        {
          result->table[i] = NULL;
        }
    }
  return result;
}

size_t gmap_size(const gmap *m)
{
  if (m == NULL)
    {
      return 0;
    }
  
  return m->size;
}

/**
 * Returns the node where the given key is located, or NULL if it is not present.
 * where it would go if it is not present.
 * 
 * @param table a table with at least one free slot
 * @param key a string, non-NULL
 * @param capacity the capacity of table
 * @param hash a hash function for strings
 * @return a pointer to the node containing key, or NULL
 */
node *gmap_table_find_key(node **table, const void *key, int (*compare)(const void *, const void *), size_t (*hash)(const void *), size_t capacity)
{
  // compute starting location for search from hash function
  size_t i = gmap_compute_index(key, hash, capacity);
  node *curr = table[i];

  // iterate through tree and find node
  while (curr != NULL && compare(key, curr->key) != 0)
    {
      if (compare(key, curr->key) < 0)
        {
          curr = curr->left;
        }
      else 
        {
          curr = curr->right;
        }
    }
  return curr;
}

bool gmap_put(gmap *m, const void *key, void *value)
{
  if (m == NULL || key == NULL)
    {
      return false;
    }

  node *n = gmap_table_find_key(m->table, key, m->compare, m->hash, m->capacity);
  if (n != NULL)
    {
      // key already present
      n->value = value;
      return false;
    }
  else
    {
      // make a copy of the key
      void *copy = m->copy(key);
      
      if (copy != NULL)
        {
          // new key, value pair -- check capacity
          if (m->size >= m->capacity)
            {
              // grow
              gmap_embiggen(m, m->capacity * 2);
            }
              
          // add to table
          node *n = malloc(sizeof(node));
          if (n != NULL)
            {
              n->key = copy;
              n->value = value;
              gmap_table_add(m->table, n, m->compare, m->hash, m->capacity);
              m->size++;
              return true;
            }
          else
            {
              free(copy);
              return false;
            }
        }
      else
        {
          return false;
        }
    }
}

/**
 * Adds the given node containing a key and value into the appropriate chain the in the
 * given hash table.
 *
 * @param table a hash table
 * @param n a node containing a key and value
 * @param hash a hash function for keys
 * @param capacity the size of the hash table
 */
void gmap_table_add(node **table, node *n, int (*compare)(const void *, const void *), size_t (*hash)(const void *), size_t capacity)
{
  size_t i = gmap_compute_index(n->key, hash, capacity);

  // funciton to add element recursively
  // starts from the root and looks at whether it should add to left or right subtree, then calls on that subtree
  // after recursive call, checks balance and does rotation on that node

  if (table[i] == NULL)
    {
      // just directly initialize the node
      n->left = NULL;
      n->right = NULL;
      n->left_height = 0;
      n->right_height = 0;
      table[i] = n;
    }
  
  else 
    {
      // tree points to the balanced tree with the added node there 
      table[i] = recursive_table_add(table[i], n, compare);
    }
}

void gmap_embiggen(gmap *m, size_t n)
{
  size_t bigger_capacity = n;
  node **bigger = calloc(bigger_capacity, sizeof(node *));
  if (bigger != NULL)
    {
      // would be better to do this without creating new nodes
      for (size_t i = 0; i < m->capacity; i++)
        {
          node *curr = m->table[i];

          // use post-order traversal to add all of the node's children and the node to the new table
          recursive_add(bigger, curr, m->compare, m->hash, bigger_capacity);
        }
      free(m->table);
      m->table = bigger;
      m->capacity = bigger_capacity;
    }
}

size_t gmap_compute_index(const void *key, size_t (*hash)(const void *), size_t size)
{
  return (hash(key) % size + size) % size;
}

bool gmap_contains_key(const gmap *m, const void *key)
{
  if (m == NULL || key == NULL)
    {
      return false;
    }

  return gmap_table_find_key(m->table, key, m->compare, m->hash, m->capacity) != NULL;
}

void *gmap_get(gmap *m, const void *key)
{
  if (m == NULL || key == NULL)
    {
      return NULL;
    }
  
  node *n = gmap_table_find_key(m->table, key, m->compare, m->hash, m->capacity);
  if (n != NULL)
    {
      return n->value;
    }
  else
    {
      return NULL;
    }
}

void gmap_for_each(gmap *m, void (*f)(const void *, void *, void *), void *arg)
{
  if (m == NULL || f == NULL)
    {
      return;
    }

  // recursively apply function to all nodes of tree
  for (size_t i = 0; i < m->capacity; i++)
    {
      node *curr = m->table[i];
      recursive_apply(curr, f, arg);
    }
}  

void gmap_destroy(gmap *m)
{
  if (m == NULL)
    {
      return;
    }

  //gmap_validate(m);
  for (size_t i = 0; i < m->capacity; i++)
    {
      node *curr = m->table[i];
      recursive_destroy(curr);
	  }

  // TO DO: fix memory leak from Ex. 7
  free(m->table);
  free(m);
}

void recursive_add(node **bigger, node *n, int (*compare)(const void *, const void *), size_t (*hash)(const void *), size_t capacity) // function to complement embiggen; recursively adds nodes to new table
{
  if (n != NULL)
    {
      recursive_add(bigger, n->left, compare, hash, capacity);
      recursive_add(bigger, n->right, compare, hash, capacity);
      gmap_table_add(bigger, n, compare, hash, capacity);
    }
}

void recursive_apply(node *n, void (*f)(const void *, void *, void *), void *arg)
{
  if (n != NULL)
    {
      recursive_apply(n->left, f, arg);
      recursive_apply(n->right, f, arg);
      f(n->key, n->value, arg);
    }
}

void recursive_destroy(node *n) // function to recursively destroy each node in the AVL trees of hash table
{
  if (n != NULL)
    {
      recursive_destroy(n->left);
      recursive_destroy(n->right);
      free(n->key);
      free(n);
    }
}


node *double_rotate_right(node *n) // function to double rotate an AVL tree
{
  n->right = rotate_left(n->right);
  return rotate_right(n);
}

node *double_rotate_left(node *n) // function to double rotate an AVL tree
{
  n->left = rotate_right(n->left);
  return rotate_left(n);
}

node *rotate_right(node *n)
{
  node *child = n->right;
  n->right = child->left;
  child->left = n;
  n->right_height = child->left_height;
  child->left_height++;

  return child;
}

node *rotate_left(node *n)
{
  node *child = n->left;
  n->left = child->right;
  child->right = n;
  n->left_height = child->right_height;
  child->right_height++;

  return child;
}



node *recursive_table_add(node *curr, node *n, int (*compare)(const void *, const void *)) // funciton to recursively add a node into a tree
{
  if (curr == NULL)
    {
      n->left_height = 0;
      n->right_height = 0;
      n->left = NULL;
      n->right = NULL;
      return n;
    }
  else if (compare(n->key, curr->key) < 0)
    {
      curr->left = recursive_table_add(curr->left, n, compare);
      curr->left_height = 1 + max(curr->left->left_height, curr->left->right_height);
      if (curr->left_height - curr->right_height > 1)
        {
          if (curr->left->left_height < curr->left->right_height)
            {
              curr = double_rotate_left(curr);
            }
          else 
            {
              curr = rotate_left(curr);
            }
        }
    }
  else 
    {
      curr->right = recursive_table_add(curr->right, n, compare);
      curr->right_height = 1 + max(curr->right->left_height, curr->right->right_height);
      if (curr->right_height - curr->left_height > 1)
        {
          if (curr->right->right_height < curr->right->left_height)
            {
              curr = double_rotate_right(curr);
            }
          else 
            {
              curr = rotate_right(curr);
            }
        }
    }
  return curr;
}