#define _GNU_SOURCE

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "ldigraph.h"
#include "gmap.h"
#include "hash_key.h"
#include "heapsort.h"


struct vertex // keep track of how many times the node has been added to the graph
{
  size_t index; // order that the team appeared in stdin
  size_t out_degree; // number of wins for each team
  size_t in_degree; // number of losses for each team
  size_t total_degrees; // in degree plus out degree
  size_t num_edges;
  vertex **edges;
  int order;
  float ratio;
  int dfs_order;
};

struct ldigraph {
  size_t n;          // the number of vertices (teams)
  size_t capacity; // current vertex capacity of the graph
  vertex **teams; // points to all teams created as nodes 
  gmap **adj; // gmap for each index, keys are other indices (teams) and values are pointers to their corresponding nodes [A: {B: Node B, C}, B: {C, D}]
  gmap **wins;
  gmap *index_to_word;
};

struct ldi_search
{
  const ldigraph *g;
  size_t from;
  int *color;
  size_t *pred;
  vertex **visited;
  size_t visit_count;
};

enum {DFS_UNSEEN, DFS_ACTIVE, DFS_DONE};

#define LDIGRAPH_INITIAL_CAPACITY 10


void ldigraph_dfs_visit(const ldigraph* g, ldi_search *s, size_t from, bool *is_cycle);
ldi_search *ldigraph_dfs(const ldigraph *g, size_t from, bool *is_cycle);
ldi_search *ldi_search_create(const ldigraph *g, size_t from);
vertex *node_create(size_t size);
void dfs_recursive_sort(const ldigraph *g, size_t from, size_t *wrong_ways, ldi_search *s); // runs DFS from starting node;
int compare_degree_ratio(const void *i, const void *j);
int compare_out_degree(const void *i, const void *j);
int compare_out_degree_dfs(const void *i, const void *j);
size_t *topo_sort(ldigraph *g);
size_t *degree_sort(ldigraph *g, size_t *wrong_ways);
size_t *count_wrong_ways(ldigraph *g, size_t *wrong_ways);
vertex **ldigraph_teams(ldigraph *g);
size_t *dfs_count_wrong(ldigraph *g, ldi_search *s, size_t *wrong_ways); // returns an array of size_t's from s->visited
void ldigraph_set_dfs_order(ldigraph *g);
void dfs_sort_visit(const ldigraph *g, ldi_search *s, size_t from);
void remove_edges(vertex *team, vertex **answer, size_t *num_next);
void print_table(gmap *m);

ldigraph *ldigraph_create()
{
  
  ldigraph *g = malloc(sizeof(ldigraph));
  if (g != NULL)
    {
      g->n = 0;
      g->capacity = LDIGRAPH_INITIAL_CAPACITY;
      
      g->teams = malloc(sizeof(vertex *) * g->capacity);
      g->adj = malloc(sizeof(gmap *) * g->capacity);
      g->wins = malloc(sizeof(size_t *) * g->capacity);
      
      if (g->adj == NULL || g->teams == NULL || g->wins == NULL)
        {
          ldigraph_destroy(g);
          return NULL;
        }
    }
  return g;
}

void ldigraph_grow(ldigraph *g)
{
  if (g->n == g->capacity)
    {
      g->capacity *= 3;
      g->teams = realloc(g->teams, g->capacity * sizeof(vertex *));
      g->adj = realloc(g->adj, g->capacity * sizeof(gmap *));
      g->wins = realloc(g->wins, g->capacity * sizeof(gmap *));
    }
  
  // add new node to the graph
  g->teams[g->n] = node_create(g->n);
  g->wins[g->n] = gmap_create(int_duplicate, compare_int_keys, int_hash, free);
  g->adj[g->n++] = gmap_create(int_duplicate, compare_int_keys, int_hash, free);
}


void input_index(ldigraph *g, gmap *index)
{
  g->index_to_word = index;
}

vertex *node_create(size_t size)
{
  vertex *new_node = malloc(sizeof(vertex));
  new_node->in_degree = 0;
  new_node->out_degree = 0;
  new_node->total_degrees = 0;
  new_node->order = -1;
  new_node->dfs_order = -1;
  new_node->index = size;
  new_node->edges = NULL; // to be changed later
  return new_node;
}

/**
 * Returns the number of vertices in the given graph.
 *
 * @param g a pointer to an directed graph, non-NULL
 * @return the number of vertices in that graph
 */
size_t ldigraph_size(const ldigraph *g)
{
  if (g != NULL)
    {
      return g->n;
    }
  else
    {
      return 0;
    }
}

/**
 * Adds a directed edge between the given pair of vertices to
 * the given directed graph.  The behavior is undefined if the edge
 * already exists.
 *
 * @param g a pointer to a directed graph, non-NULL
 * @param from the index of a vertex in the given graph
 * @param to the index of a vertex in the given graph, not equal to from
 */
void ldigraph_add_edge(ldigraph *g, size_t winner_index, size_t loser_index)
{
  if (g != NULL && winner_index >= 0 && loser_index >= 0 && winner_index != loser_index)
    {
      if(!gmap_contains_key(g->adj[winner_index], &loser_index) && !gmap_contains_key(g->adj[loser_index], &winner_index))
        {
          // this is a new game being played between the two teams
          size_t *value = malloc(sizeof(size_t));
          *value = 1;
          if (!gmap_put(g->adj[winner_index], &loser_index, g->teams[loser_index]) || !gmap_put(g->wins[winner_index], &loser_index, value)) // entering the node into the adj set
            {
                printf("Error with gmap put\n");
            }

          // increment values
          g->teams[winner_index]->out_degree++;
          g->teams[loser_index]->in_degree++;
          g->teams[winner_index]->total_degrees++;
          g->teams[loser_index]->total_degrees++;
        }
      
      else if (gmap_contains_key(g->adj[loser_index], &winner_index)) // loser has already beaten winner before
        {
            size_t *wins = gmap_get(g->wins[loser_index], &winner_index);
            (*wins)--; 
            // check to see if wins equals 0 now, if so remove that edge
            if (*wins == 0)
                {
                free(wins);
                gmap_remove(g->adj[loser_index], &winner_index); // remove from node adjacency set
                gmap_remove(g->wins[loser_index], &winner_index); // remove from frequency adj set
                g->teams[winner_index]->in_degree--;
                g->teams[loser_index]->out_degree--;
                g->teams[winner_index]->total_degrees--;
                g->teams[loser_index]->total_degrees--;
                }
        }
      
      else // this means that winner has already beaten loser before
        {
            size_t *wins = gmap_get(g->wins[winner_index], &loser_index);
            (*wins)++;
        }
    }
}

void ldigraph_set_dfs_order(ldigraph *g)
{
  for (size_t i = 0; i < g->n; i++)
    {
      g->teams[i]->dfs_order = i;
    }
}

vertex **ldigraph_teams(ldigraph *g)
{
  return g->teams;
}

bool ldigraph_is_cycle(const ldigraph *g) // runs DFS on a graph to determine if there exists a cycle
{
  bool *is_cycle = malloc(sizeof(bool)); // cycle boolean to keep track of throughout the DFS
  *is_cycle = false;
  ldi_search *s = ldigraph_dfs(g, 0, is_cycle);
  bool answer = *is_cycle;
  free(is_cycle);
  ldi_search_destroy(s);
  return answer;
}

ldi_search *ldigraph_dfs(const ldigraph *g, size_t from, bool *is_cycle)
{
  if (g == NULL || from < 0 || from >= g->n)
    {
      return NULL;
    }
  // TO DO: initialize search results and start recursion
  ldi_search *s = ldi_search_create(g, from);
  for (size_t i = from; i < g->n; i++)
    {
      if (s->color[i] == DFS_UNSEEN)
        {
          ldigraph_dfs_visit(g, s, from, is_cycle);
        }
    }
  
  return s;
}

void big_sort_dfs(ldigraph *g)
{
  // first sort the big array
  heapsort(g->n, sizeof(vertex *), g->teams, compare_out_degree_dfs);

  // then sort each of the edges
  for (size_t i = 0; i < g->n; i++)
    {
      heapsort(g->teams[i]->num_edges, sizeof(vertex *), g->teams[i]->edges, compare_out_degree_dfs);
    }
}

ldi_search *dfs_sort(const ldigraph *g, size_t from) // ./Rank -dfs
{
  if (g == NULL || from < 0 || from >= g->n)
    {
      return NULL;
    }
  // TO DO: initialize search results
  ldi_search *s = ldi_search_create(g, from);
  bool first = true;
  size_t index;
  size_t last = 0;

  // heapsort the entire array initially so I know how to find unvisited nodes

  while (s->visit_count < g->n) // call this function until I run out of remaining nodes
    {
      if (first == true) 
        {
          first = false;
          index = from;
        }
      else 
        {
          index = last; 
          while (s->color[index] != DFS_UNSEEN)
            {
              index++;
            }
          last = index + 1;
        }
      dfs_sort_visit(g, s, index);
    }

  return s;
}

void dfs_sort_visit(const ldigraph *g, ldi_search *s, size_t from)
{
  s->color[from] = DFS_ACTIVE;
  s->visited[s->visit_count++] = g->teams[from];
  if (s->visit_count == g->n)
    {
      s->color[from] = DFS_DONE;
      return;
    }
  vertex *team = g->teams[from];
  vertex **next_nodes = team->edges; // ALREADY SORTED
  size_t next_size = team->num_edges;
  for (size_t i = 0; i < next_size; i++)
    {
      if (s->color[next_nodes[i]->dfs_order] == DFS_UNSEEN)
        {
          dfs_sort_visit(g, s, next_nodes[i]->dfs_order);
        }
    }
  // make sure to mark s as done when you finish
  s->color[from] = DFS_DONE;
}

size_t *dfs_count_wrong(ldigraph *g, ldi_search *s, size_t *wrong_ways) // returns an array of size_t's from s->visited
{
  size_t *answer = malloc(sizeof(size_t) * g->n);
  for (size_t i = 0; i < g->n; i++)
    {
      vertex *team = s->visited[i];
      team->order = i; // setting order for each team node
      vertex **edges = team->edges; // FIX THIS IN OH
      for (size_t j = 0; j < team->num_edges; j++)
        {
          if (edges[j]->order >= 0 && team->order > edges[j]->order) // checking if they are ordered and if ordered earlier
            {
              (*wrong_ways)++;
            }
        }
      answer[i] = s->visited[i]->index;
    }

  return answer;
}

void ldigraph_dfs_visit(const ldigraph *g, ldi_search *s, size_t from, bool *is_cycle)
{
  s->color[g->teams[from]->index] = DFS_ACTIVE;
  s->visited[s->visit_count++] = g->teams[from];
  
  vertex **next_nodes = g->teams[g->teams[from]->index]->edges;
  size_t next_size = g->teams[g->teams[from]->index]->num_edges;
  for (size_t i = 0; i < next_size; i++)
    {
      if (s->color[next_nodes[i]->index] == DFS_ACTIVE)
        {
          *is_cycle = true;
        }
      else if (s->color[next_nodes[i]->index] == DFS_UNSEEN)
        {
          ldigraph_dfs_visit(g, s, next_nodes[i]->index, is_cycle);
        }
    }
  // make sure to mark s as done when you finish
  s->color[from] = DFS_DONE;
}

ldi_search *ldi_search_create(const ldigraph *g, size_t from)
{
  if (g != NULL && from >= 0 && from < g->n)
    {
      ldi_search *s = malloc(sizeof(ldi_search));
      
      if (s != NULL)
        {
          s->g = g;
          s->from = from;
          s->color = malloc(sizeof(int) * g->n);
          s->visited = malloc(sizeof(vertex *) * g->n);
          s->visit_count = 0;
          s->pred = malloc(sizeof(size_t) * g->n);

          if (s->color != NULL && s->visited != NULL && s->pred != NULL)
            {
              for (size_t i = 0; i < g->n; i++)
                {
                  s->color[i] = DFS_UNSEEN;
                  s->pred[i] = g->n;
                }
            }
          else
            {
              free(s->pred);
              free(s->visited);
              free(s->color);
              free(s);
              return NULL;
            }
        }

      return s;
    }
  else
    {
      return NULL;
    }
}

size_t *ldi_search_visited(const ldi_search *s, size_t *len)
{
  if (s == NULL && len != NULL)
    {
      return NULL;
    }
  
  size_t *visited = malloc(sizeof(size_t) * s->visit_count);
  if (visited != NULL)
    {
      *len = s->visit_count;
      memcpy(visited, s->visited, sizeof(size_t) * s->visit_count);
    }
  
  return visited;
}

void ldi_search_destroy(ldi_search *s)
{
  if (s != NULL)
    {
      free(s->color);
      free(s->visited);
      free(s->pred);
      free(s);
    }
}

void ratio_edge_set(ldigraph *g) // changes values in adj set from frequencies to nodes, sets ratios
{
  for (size_t i = 0; i < g->n; i++)
    {
      g->teams[i]->num_edges = gmap_size(g->adj[i]);
      g->teams[i]->edges = (vertex **)gmap_get_values(g->adj[i], sizeof(vertex **));

      if (g->teams[i]->total_degrees == 0)
        {
          g->teams[i]->ratio = -1;
        }
      else 
        {
          g->teams[i]->ratio = ((float)g->teams[i]->out_degree / (float)g->teams[i]->total_degrees);
        }
    }
}

vertex **ldigraph_find_zeroes(ldigraph *g, vertex **first_zeroes, size_t *num_zeroes, size_t *num_next, bool *first, vertex ***new_first_zero_location) // finds all vertices with 0 in degree in an ldigraph
{
  vertex **answer = malloc(sizeof(vertex *) * g->n); // return array which will contain the next set of zeroes
  
  if ((*first) == true)
    {
      // this is the first time, we have to iterate through and find all of the zeroes then output them
      for (size_t i = 0; i < g->n; i++)
        {
          if (g->teams[i]->in_degree == 0)
            {
              first_zeroes[(*num_zeroes)++] = g->teams[i];
            }
        }
        *new_first_zero_location = realloc(first_zeroes, sizeof(vertex *) *(*num_zeroes)); //['A', 'B', 'C', N, N, N, N] --> ['A', 'B', 'C']
        vertex **temp = *new_first_zero_location;
        if (*new_first_zero_location == NULL)
          {
            fprintf(stderr, "ERROR with first zeroes\n");
          }
        heapsort(*num_zeroes, sizeof(vertex *), *new_first_zero_location, compare_out_degree);
        for (size_t i = 0; i < (*num_zeroes); i++)
          {
            remove_edges((*new_first_zero_location)[i], answer, num_next);
            printf("%s\n", (char *)gmap_get(g->index_to_word, &temp[i]->index));
          }
    }
  else // we already have an array of next zeroes to be printed, so we just remove their edges and fill answer array
    {
      heapsort(*num_zeroes, sizeof(vertex *), first_zeroes, compare_degree_ratio);
      for (size_t i = 0; i < *num_zeroes; i++)
        {
          remove_edges(first_zeroes[i], answer, num_next);
          printf("%s\n", (char *)gmap_get(g->index_to_word, &first_zeroes[i]->index));
        }
    }

  if (*num_next != 0)
    {
      answer = realloc(answer, (*num_next) * sizeof(vertex *));
    }
  else // if none left return null
    {
      free(answer);
      answer = NULL;
    }

  return answer;
}

void remove_edges(vertex *team, vertex **answer, size_t *num_next)
{
  for (size_t i = 0; i < team->num_edges; i++)
    {
      team->edges[i]->in_degree -= 1;
      if (team->edges[i]->in_degree == 0)
        {
          answer[(*num_next)++] = team->edges[i];
        }
    }
}

size_t *degree_sort(ldigraph *g, size_t *wrong_ways) // sorts graph based on degrees
{
  heapsort(g->n, sizeof(vertex *), g->teams, compare_degree_ratio);
  
  return count_wrong_ways(g, wrong_ways);
}

size_t *count_wrong_ways(ldigraph *g, size_t *wrong_ways) // takes an ordered graph and counts wrong way edges
{
  size_t *answer = malloc(sizeof(size_t) * g->n);
  for (size_t i = 0; i < g->n; i++)
    {
      g->teams[i]->order = i; // setting order for each team node // [0, 1, 4, 3, 2]
      vertex **edges = g->teams[i]->edges; // getting all adjacent teams
      for (size_t j = 0; j < g->teams[i]->num_edges; j++)
        {
          if (edges[j]->order >= 0 && g->teams[i]->order > edges[j]->order) // checking if they are ordered and if ordered earlier
            {
              (*wrong_ways)++;
            }
        }
      answer[i] = g->teams[i]->index;
    }
  return answer;
}
    
int compare_out_degree(const void *i, const void *j)
{
  const vertex *q1 = *(vertex **)i;
  const vertex *q2 = *(vertex **)j;
  if (q1->out_degree == q2->out_degree) // if the out degrees are the same, earlier indices are preferred
    {
      return q1->index - q2->index;
    }
  else 
    {
      return q2->out_degree - q1->out_degree;
    }
}

int compare_out_degree_dfs(const void *i, const void *j)
{
  const vertex *q1 = *(vertex **)i;
  const vertex *q2 = *(vertex **)j;
  if (q1->out_degree == q2->out_degree && q1->in_degree == q2->in_degree) // if the out degrees are the same, earlier indices are preferred
    {
      return q1->index - q2->index; // want a lower index
    }
  else if (q1->out_degree == q2->out_degree) // want a lower in degree
    {
      return q1->in_degree - q2->in_degree;
    }
  else 
    {
      return q2->out_degree - q1->out_degree; // want a higher out degree
    }
}

int compare_degree_ratio(const void *i, const void *j)
{
  const vertex *q1 = *(vertex **)i;
  const vertex *q2 = *(vertex **)j;
  

  if (q1->ratio == q2->ratio) // if the ratios are the same, then we pass to the out_degree function
    {
      return compare_out_degree(i, j);
    }
  else if (q1->ratio > q2->ratio)
    {
      return -1;
    }
  else 
    {
      return 1;
    }
}

void free_value_ldigraph(const void *key, void *value, void *arg)
{
  free(value);
}


void ldigraph_destroy(ldigraph *g)
{
  if (g != NULL)
    {
      for (size_t i = 0; i < g->n; i++)
        {
          if (g->teams[i]->edges != NULL)
            {
              free(g->teams[i]->edges);
            }
          free(g->teams[i]);
          gmap_for_each(g->wins[i], free_value_ldigraph, NULL);
          gmap_destroy(g->adj[i]);
          gmap_destroy(g->wins[i]);
        }
      free(g->adj);
      free(g->wins);
      free(g->teams);
      free(g);
    }
}

void print_keys(const void *key, void *value, void *arg)
{
  fprintf(stdout, "key: %ld, value: %ld\n", *(size_t *)key, *(size_t *)value);
}

void print_table(gmap *m)
{
  fprintf(stdout, "%ld\n", gmap_size(m));
  gmap_for_each(m, print_keys, NULL);
}