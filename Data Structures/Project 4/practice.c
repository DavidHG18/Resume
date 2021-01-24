
bool frequency_to_graph(master *m);
void increase_frequency_map(master *m);
void frequency_add_game(master *m, size_t *winner_index, size_t *loser_index);
bool master_realloc(master *m);




void increase_frequency_map(master *m)
{
    if (m->teams == m->capacity)
        {
            if (master_realloc(m) == false)
                {
                    fprintf(stderr, "ERROR with reallocing master\n");
                    return;
                }
        }
    m->frequencies[m->teams++] = gmap_create(int_duplicate, compare_int_keys, int_hash, free);
}

void frequency_add_game(master *m, size_t *winner_index, size_t *loser_index) //[Team A: {Team B: 2}, Team B: {Team C:2, Team D:1}]
{
    if(!gmap_contains_key(m->frequencies[*winner_index], loser_index) && !gmap_contains_key(m->frequencies[*loser_index], winner_index))
        {
          // this is a new game being played between the two teams
          size_t *wins = malloc(sizeof(size_t));
          *wins = 1;
          if (!gmap_put(m->frequencies[*winner_index], loser_index, wins))
            {
                printf("Error with gmap put\n");
            } // entering into the gmap
        }
      
      else if (gmap_contains_key(m->frequencies[*loser_index], winner_index)) // loser has already beaten winner before
        {
            size_t *wins = (size_t *)gmap_get(m->frequencies[*loser_index], winner_index);
            (*wins)--;

            // check to see if wins equals 0 now, if so remove that edge
            if (*wins == 0)
                {
                gmap_remove(m->frequencies[*loser_index], winner_index); // if Duke has beaten UVA one time already, remove edge
                }
        }
      
      else // this means that loser has already beaten winner before 
        {
            if (gmap_get(m->frequencies[*winner_index], loser_index) == NULL)
            {
                printf("NULL\n");
            }
            size_t *wins = (size_t *)gmap_get(m->frequencies[*winner_index], loser_index);
            (*wins)++;
        }
}

bool master_realloc(master *m)
{
    m->capacity *= 2;
    m->frequencies = realloc(m->frequencies, sizeof(gmap *) * m->capacity);
    return (m->frequencies != NULL);
}

bool frequency_to_graph(master *m) // turns m->frequencies into an ldigraph [Team A: {Team B: 2}, Team B: {Team C:2, Team D:1}]
{
    m->g = ldigraph_create(m->teams); // create graph
    if (m->g == NULL)
        {
            return false;
        }
    for (size_t i = 0; i < m->teams; i++)
        {
            size_t **matchups = (size_t **)gmap_get_keys(m->frequencies[i], sizeof(size_t *)); // returns an array of the losing team indices [&1, &2]
            size_t size = gmap_size(m->frequencies[i]);
            for (size_t j = 0; j < size; j++)
                {
                    ldigraph_add_edge(m->g, i, *matchups[j]);
                    free(matchups[j]);
                }
            free(matchups);
        }
    return true;
}