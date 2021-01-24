#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "ldigraph.h"
#include "gmap.h"
#include "hash_key.h"
#include "heapsort.h"

#define LDIGRAPH_INITIAL_CAPACITY 4


/**
 * UVA --> UNC, FSU, Wake
 * UNC --> FSU, Wake
 * Duke --> UNC
 * Wake --> FSU
 * FSU --> Duke
*/

typedef struct _master // master struct
{
    gmap *word_map; // "UVA" --> 0
    gmap *index_to_word; // 0 --> "UVA"
    size_t teams;
    size_t capacity;
    ldigraph *g; 
}master;


bool read_game(master *m); // reads a line from stdin and inserts edge into a graph
void master_destroy(master *m); // destroys master struct
void index_print(size_t *output_indices, master *m);
void arrays_free(void *array, size_t n);
void free_value(const void *key, void *value, void *arg);

int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "-topo") != 0 && strcmp(argv[1], "-degree") != 0 && strcmp(argv[1], "-dfs") != 0)) // if not second argument or not equal to desired CLAs
        {
            fprintf(stderr, "USAGE: ./Rank -topo | -degree | -dfs");
            return 1;
        }
    

    master *m = malloc(sizeof(master));


    if (m == NULL)
        {
            return 1;
        }

    // initialize ldigraph, num teams, and word/index maps
    m->word_map = gmap_create(duplicate, compare_string_keys, string_hash, free);
    m->index_to_word = gmap_create(int_duplicate, compare_int_keys, int_hash, free);
    m->teams = 0;
    m->capacity = LDIGRAPH_INITIAL_CAPACITY;
    m->g = ldigraph_create();

    if (m->word_map == NULL || m->index_to_word == NULL || m->g == NULL)
        {
            master_destroy(m);
            return 1;
        }

    // step 1: establish winning frequencies
    char c;
    while ((c = fgetc(stdin)) != EOF)
        {
            // first time reading game. this is where we check for errors and determine size of the graph
            ungetc(c, stdin);
            if (!read_game(m))
                {
                    fprintf(stderr, "ERROR reading game\n");
                    master_destroy(m);
                    return 1;
                }
        }
    
    // second time reading game. if we get here, we know it is valid input.
   

    // set all current ratios on graphs
    ratio_edge_set(m->g);

    // step 2: determine what we are being asked to do
    if (m->teams == 0)
        {
            master_destroy(m);
            return 0;
        }
    
    if (strcmp(argv[1], "-topo") == 0)
    {
        if (ldigraph_is_cycle(m->g))
            {
                master_destroy(m);
                return 0;
            }
        input_index(m->g, m->index_to_word);
        printf("%d\n", 0);
        size_t vertices = 0;
        vertex **new_first_zero_location;
        vertex **first_zeroes = malloc(sizeof(vertex *) * m->teams);
        bool *first = malloc(sizeof(bool));
        *first = true;
        size_t *num_zeroes = calloc(1, sizeof(size_t)); // both initialized to 0 at start
        size_t *num_next = calloc(1, sizeof(size_t)); // both initialized to 0 at the start
        while (vertices < m->teams)
            {
                vertex **next_zeroes = ldigraph_find_zeroes(m->g, first_zeroes, num_zeroes, num_next, first, &new_first_zero_location); // prints out sorted zeroes and returns list of next zeroes
                if (*first == true)
                    {
                        *first = false;
                        first_zeroes = new_first_zero_location;
                    }
                free(first_zeroes);
                vertices += (*num_zeroes);
                first_zeroes = next_zeroes;
                (*num_zeroes) = (*num_next);
                (*num_next) = 0;
            }
        free(num_zeroes);
        free(num_next);
        free(first);

    }
    //size_t *output_indices;
    else if (strcmp(argv[1], "-dfs") == 0)
        {
            size_t min_wrong = m->teams * m->teams; // determine which ordering minimizes wrong way edges
            size_t *best_order = malloc(sizeof(size_t) * m->teams);
            big_sort_dfs(m->g); // sorts entire array as well as all of the edges so we don't have to do it again
            ldigraph_set_dfs_order(m->g); // determine new order of each team now that we've sorted
            for (size_t i = 0; i < m->teams; i++)
                {
                    size_t *wrong_ways = calloc(1, sizeof(size_t)); // initializing number of wrong ways
                    ldi_search *search_order = dfs_sort(m->g, i); // returns the dfs sort run from that starting point (returns an ldi_search)
                    //size_t *visited = ldi_search_visited(search_order, &m->teams);
                    size_t *order = dfs_count_wrong(m->g, search_order, wrong_ways); // turns it into a size_t array with wrong ways counted
                    ldi_search_destroy(search_order);
                    if (*wrong_ways < min_wrong) // if this search was more efficient than best so far
                        {
                            free(best_order);
                            min_wrong = *wrong_ways;
                            best_order = order;
                        }
                    else // we move on
                        {
                            free(order);
                        }
                    free(wrong_ways);
                }
            printf("%ld\n", min_wrong);
            index_print(best_order, m);
            free(best_order);
        }
    else  // degree sort  
        {
            size_t *wrong_ways = calloc(1, sizeof(size_t));
            size_t *output_indices = degree_sort(m->g, wrong_ways);
            printf("%ld\n", *wrong_ways);
            free(wrong_ways);
            index_print(output_indices, m);
            free(output_indices);
        }
    
    master_destroy(m);

    return 0;
}



/**
void arrays_free(void **array, size_t n)
{
    for (size_t i = 0; i < n; i++)
        {
            free(array[i]);
        }
    free(array);
}
*/

void index_print(size_t *output_indices, master *m)
{
    for (size_t i = 0; i < m->teams; i++)
        {
            fprintf(stdout, "%s\n", (char *)gmap_get(m->index_to_word, &output_indices[i]));
        }
}

bool read_game(master *m) // reads a line from stdin and inserts edge into a graph
{
    if (!m)
        {
            return false;
        }

    size_t capacity = 5;
    char *winner = malloc(sizeof(char) * capacity);
    char *loser = malloc(sizeof(char) * capacity);
    if (!winner || !loser)
        {
            return false;
        }
    size_t i = 0;

    char c;

    if ((c = fgetc(stdin)) != '"') // first character is a quote
        {
            fprintf(stderr, "Invalid input\n");
            free(winner);
            free(loser);
            return false;
        }

    if (isspace((c = fgetc(stdin))))
        {
            fprintf(stderr, "Invalid input (space)\n");
            free(winner);
            free(loser);
            return false;
        }
    else 
        {
            ungetc(c, stdin);
        }

    while ((c = fgetc(stdin)) != '"') // check later if it could be EOF
        {
            if (c == EOF)   
                {
                    fprintf(stderr, "Invalid input EOF\n");
                    free(winner);
                    free(loser);
                    return false;
                }
            winner[i++] = c;
            if (i == capacity)
                {
                    capacity *= 2;
                    winner = realloc(winner, capacity);
                }
        }
    
    // end of first word
    ungetc(c, stdin); // unget the quotes
    ungetc(c, stdin); // unget the last character

    if (isspace((c = fgetc(stdin))))
        {
            fprintf(stderr, "Invalid input space\n");
            free(winner);
            free(loser);
            return false;
        }
    c = fgetc(stdin); // read past quotes
    
    if ((c = fgetc(stdin)) != ',') // after reading in the first team
        {
            fprintf(stderr, "Invalid input\n");
            free(winner);
            free(loser);
            return false;
        }

    if ((c = fgetc(stdin)) != '"') // beginning of second team
        {
            fprintf(stderr, "Invalid input\n");
            free(winner);
            free(loser);
            return false;
        }
    // enter null character at end
    winner[i] = '\0'; // A
    
    i = 0;
    capacity = 5; // reset i and capacity for second team

    if (isspace((c = fgetc(stdin))))
        {
            fprintf(stderr, "Invalid input\n");
            free(winner);
            free(loser);
            return false;
        }
    else 
        {
            ungetc(c, stdin);
        }

    while ((c = fgetc(stdin)) != '"') // check later if it could be EOF
        {
            if (c == EOF)   
                {
                    fprintf(stderr, "Invalid input\n");
                    free(winner);
                    free(loser);
                    return false;
                }
            loser[i++] = c;
            if (i == capacity)
                {
                    capacity *= 2;
                    loser = realloc(loser, capacity);
                }
        }
    
    // end of first word
    ungetc(c, stdin); // unget the quotes
    ungetc(c, stdin); // unget the last character

    if (isspace((c = fgetc(stdin))))
        {
            fprintf(stderr, "Invalid input\n");
            free(winner);
            free(loser);
            return false;
        }
    c = fgetc(stdin); // read past quotes
    
    if ((c = fgetc(stdin)) != '\n' && c != EOF) // must be end of line after second team
        {
            fprintf(stderr, "Invalid input\n");
            free(winner);
            free(loser);
            return false;
        }
    loser[i] = '\0';
    
    if (strcmp(winner, loser) == 0) // winner and loser can't be the same
        {
            fprintf(stderr, "Invalid input, team names are the same\n");
            free(winner);
            free(loser);
            return false;
        }
    
    bool new_winner = false;
    bool new_loser = false;
    // check to see if the graph already has these teams; if not, add them in
    if (!gmap_contains_key(m->word_map, winner)) // if we have not seen winning team before // "A" --> &0
        {
            new_winner = true;
            size_t *winner_index = malloc(sizeof(size_t));
            if (!winner_index)
                {
                    return false;
                }
            *winner_index = m->teams++; // 0
            ldigraph_grow(m->g);
            
            if (!gmap_put(m->word_map, winner, winner_index)) // UVA --> 0
                {
                    return false;
                }
            gmap_put(m->index_to_word, winner_index, winner); // winner = UVA, winner_index = &0
        }
    
    if (!gmap_contains_key(m->word_map, loser)) // input losing team into word map, index map, and frequency map
        {
            new_loser = true;
            size_t *loser_index = malloc(sizeof(size_t));
            size_t loser_index_key = m->teams;
            if (!loser_index)
                {
                    return false;
                }
            *loser_index = m->teams++;
            ldigraph_grow(m->g);

            if (!gmap_put(m->word_map, loser, loser_index) || !gmap_put(m->index_to_word, &loser_index_key, loser)) // Duke --> 1
                {
                    return false;
                }
        }
    
    // update the frequency map
    size_t winner_index = *(size_t *)gmap_get(m->word_map, winner);
    size_t loser_index = *(size_t *)gmap_get(m->word_map, loser);
    ldigraph_add_edge(m->g, winner_index, loser_index);

    if (new_winner == false)
        {
            free(winner);
        }
    if (new_loser == false)
        {
            free(loser);
        }

   return true;
}

void master_destroy(master *m) // destroys master struct
{
    gmap_for_each(m->word_map, free_value, NULL); // frees the indices corresponding to each string
    gmap_for_each(m->index_to_word, free_value, NULL); // free the strings corresponding to each index
    gmap_destroy(m->word_map); // frees keys in word map
    gmap_destroy(m->index_to_word); // frees indices in index map
    ldigraph_destroy(m->g); // destroys graph and all contents
    free(m); // frees master pointer
}

void free_value(const void *key, void *value, void *arg)
{
  free(value);
}
