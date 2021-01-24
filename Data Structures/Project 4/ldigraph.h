#ifndef __LUGRAPH_H__
#define __LUGRAPH_H__

#include <stdlib.h>
#include <stdbool.h>

#include "heapsort.h"
#include "gmap.h"

typedef struct ldigraph ldigraph;
typedef struct ldi_search ldi_search;
typedef struct vertex vertex;


ldigraph *ldigraph_create();
size_t ldigraph_size(const ldigraph *g);
void ldigraph_add_edge(ldigraph *g, size_t winner_index, size_t loser_index);
size_t ldigraph_outdegree(const ldigraph *g, size_t v);
ldi_search *ldigraph_dfs(const ldigraph *g, size_t from, bool *is_cycle);
bool ldigraph_connected(const ldigraph *g, size_t from, size_t to);
void ldigraph_destroy(ldigraph *g);
bool ldigraph_is_cycle(const ldigraph *g);
ldi_search *dfs_sort(const ldigraph *g, size_t from);
void ratio_edge_set(ldigraph *g);
size_t *degree_sort(ldigraph *g, size_t *wrong_ways);
size_t *ldi_search_visited(const ldi_search *s, size_t *n);
void ldi_search_destroy(ldi_search *s);
size_t *topo_sort(ldigraph *g);
int compare_out_degree_dfs(const void *i, const void *j);
vertex **ldigraph_teams(ldigraph *g);
void ldigraph_grow(ldigraph *g);
size_t *count_wrong_ways(ldigraph *g, size_t *wrong_ways);
size_t *dfs_count_wrong(ldigraph *g, ldi_search *s, size_t *wrong_ways); // returns an array of size_t's from s->visited
void ldigraph_set_dfs_order(ldigraph *g);
void big_sort_dfs(ldigraph *g);
vertex **ldigraph_find_zeroes(ldigraph *g, vertex **first_zeroes, size_t *num_zeroes, size_t *num_next, bool *first, vertex ***new_first_zero_location);
void remove_edges(vertex *team, vertex **answer, size_t *num_next);
void input_index(ldigraph *g, gmap *index);

#endif
