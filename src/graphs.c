/********************************************/
/*   Structures pour stocker les machines   */
/* Ensuite spécialisées pour NFA,DFA,morphism */
/********************************************/

#include "graphs.h"
#include "printing.h"
#include <limits.h>

/*****************************/
/*+ Création et suppression +*/
/*****************************/

graph *create_graph_noedges(uint size)
{
    // Création du graphe
    graph *new;
    CALLOC(new, 1);

    // Définition du graph
    new->size = size;
    new->size_edges = 0;
    if (size != 0)
    {
        CALLOC(new->intervals, size + 1);
    }
    return new;
}

void delete_graph(graph *G)
{
    if (!G)
    {
        return;
    }
    free(G->intervals);
    free(G->storage);
    free(G);
}

graph *copy_graph(graph *g)
{
    if (!g)
    {
        return NULL;
    }
    graph *new;
    CALLOC(new, 1);
    new->size = g->size;
    new->size_edges = g->size_edges;
    MALLOC(new->intervals, g->size + 1);
    MEMCPY(new->intervals, g->intervals, g->size + 1);
    if (g->size_edges != 0)
    {
        MALLOC(new->storage, g->size_edges);
        MEMCPY(new->storage, g->storage, g->size_edges);
    }
    return new;
}

lgraph *create_lgraph_noedges(uint size_graph, uint size_alpha)
{
    // Création du graphe
    lgraph *new;
    CALLOC(new, 1);

    // Définition du graphe
    new->size_alpha = size_alpha;
    new->size_graph = size_graph;
    new->size_edges = 0;
    CALLOC(new->intervals, size_graph * size_alpha + 1);
    return new;
}

void delete_lgraph(lgraph *G)
{
    if (G == NULL)
    {
        return;
    }
    free(G->intervals);
    free(G->storage);
    free(G);
}

lgraph *copy_lgraph(lgraph *g)
{
    if (!g)
    {
        return NULL;
    }
    lgraph *new;
    CALLOC(new, 1);
    new->size_alpha = g->size_alpha;
    new->size_graph = g->size_graph;
    new->size_edges = g->size_edges;
    MALLOC(new->intervals, g->size_graph * g->size_alpha + 1);
    MEMCPY(new->intervals, g->intervals, g->size_graph * g->size_alpha + 1);
    if (g->size_edges != 0)
    {
        MALLOC(new->storage, g->size_edges);
        MEMCPY(new->storage, g->storage, g->size_edges);
    }
    return new;
}

dgraph *create_dgraph_noedges(uint size_graph, uint size_alpha)
{

    // Création du graphe
    dgraph *new;
    CALLOC(new, 1);

    // Définition du graphe
    new->size_alpha = size_alpha;
    new->size_graph = size_graph;
    new->size_edges = 0; // No edges
    if (size_graph != 0 && size_alpha != 0)
    {
        MALLOC(new->edges, size_graph);
        MALLOC(new->storage, size_graph * size_alpha);
        for (uint q = 0; q < size_graph; q++)
        {
            new->edges[q] = new->storage + q * size_alpha;
            for (uint a = 0; a < size_alpha; a++)
            {
                new->edges[q][a] = UINT_MAX; // No transition
            }
        }
    }
    return new;
}

void delete_dgraph(dgraph *G)
{
    if (G == NULL)
    {
        return;
    }
    free(G->storage);
    free(G->edges);
    free(G);
}

dgraph *copy_dgraph(dgraph *g)
{
    if (!g)
    {
        return NULL;
    }
    dgraph *new = create_dgraph_noedges(g->size_graph, g->size_alpha);
    if (g->size_graph != 0 && g->size_alpha != 0)
    {
        new->size_edges = g->size_edges;
        MEMCPY(new->storage, g->storage, g->size_graph * g->size_alpha);
    }
    return new;
}

/*******************/
/* Basic functions */
/*******************/

lgraph *dgraph_to_lgraph(dgraph *g)
{

    if (!g)
    {
        return NULL;
    }
    lgraph *new;
    CALLOC(new, 1);
    new->size_graph = g->size_graph;
    new->size_alpha = g->size_alpha;
    new->size_edges = g->size_edges;
    MALLOC(new->intervals, g->size_graph * g->size_alpha + 1);
    MALLOC(new->storage, g->size_edges);

    new->intervals[0] = 0;

    for (uint q = 0; q < g->size_graph; q++)
    {
        for (uint a = 0; a < g->size_alpha; a++)
        {
            uint ind = q * g->size_alpha + a;
            if (g->edges[q][a] != UINT_MAX)
            {
                new->intervals[ind + 1] = new->intervals[ind] + 1;
                new->storage[new->intervals[ind]] = g->edges[q][a];
            }
            else
            {
                new->intervals[ind + 1] = new->intervals[ind];
            }
        }
    }
    return new;
}

/**************/
/*+ Products +*/
/**************/

dgraph *dgraph_direct_product(dgraph *g1, dgraph *g2)
{
    if (g1->size_alpha != g2->size_alpha)
    {
        fprintf(stderr, "dgraph_direct_product: The two graphs must have the same alphabet size.\n");
        exit(EXIT_FAILURE);
    }
    dgraph *inter = create_dgraph_noedges(g1->size_graph * g2->size_graph, g1->size_alpha); // Create the resulting graph
    for (uint q = 0; q < g1->size_graph; q++)
    {
        for (uint r = 0; r < g2->size_graph; r++)
        {
            for (uint a = 0; a < g1->size_alpha; a++)
            {
                uint qa = g1->edges[q][a];
                uint ra = g2->edges[r][a];
                if (qa != UINT_MAX && ra != UINT_MAX)
                {
                    inter->edges[q * g2->size_graph + r][a] = qa * g2->size_graph + ra;
                    inter->size_edges++;
                }
            }
        }
    }

    return inter;
}

/***********/
/* Mirrors */
/***********/

graph *graph_mirror(graph *g)
{
    if (!g)
    {
        return NULL;
    }
    // Get the edge list of the inverse graph.
    edge_pair *elist = graph_to_edge_list(g, true);
    graph *new = edge_list_to_graph(elist, g->size_edges, g->size);
    free(elist);
    return new;
}

lgraph *lgraph_mirror(lgraph *g)
{
    if (!g)
    {
        return NULL;
    }
    // Get the edge list of the inverse graph.
    edge_triple *elist = lgraph_to_edge_list(g, true);
    lgraph *new = edge_list_to_lgraph(elist, g->size_edges, g->size_graph, g->size_alpha);
    free(elist);
    return new;
}

lgraph *dgraph_mirror(dgraph *g)
{
    if (!g)
    {
        return NULL;
    }
    // Get the edge list of the inverse graph.
    edge_triple *elist = dgraph_to_edge_list(g, true);
    lgraph *new = edge_list_to_lgraph(elist, g->size_edges, g->size_graph, g->size_alpha);
    free(elist);
    return new;
}

/*************/
/*+ Merging +*/
/*************/

graph *dgraph_merge(dgraph *g, parti *p)
{
    dyn_edge_pair *dea = create_dyn_edge_pair();
    for (uint q = 0; q < g->size_graph; q++)
    {
        for (uint a = 0; a < g->size_alpha; a++)
        {
            if (g->edges[q][a] != UINT_MAX)
            {
                dyn_edge_pair_add(dea, p->numcl[q], p->numcl[g->edges[q][a]]);
            }
        }
    }
    graph *ret = edge_list_to_graph(dea->array, dea->size, p->size_par);
    delete_dyn_edge_pair(dea);
    return ret;
}

/************/
/* Parcours */
/************/

void graph_search_update(graph_stype T, graph *G, dequeue *ini, bool *visited)
{
    uint (*f)(dequeue *);
    if (T == DFS)
    {
        f = rigpull_dequeue;
    }
    else
    {
        f = lefpull_dequeue;
    }

    while (!isempty_dequeue(ini))
    {
        uint q = f(ini);
        if (visited[q])
        {
            continue;
        }
        visited[q] = true;
        uint start = G->intervals[q];
        uint end = G->intervals[q + 1];
        for (uint j = start; j < end; j++)
        {
            rigins_dequeue(G->storage[j], ini);
        }
    }
}

dequeue *graph_search(graph_stype T, graph *G, dequeue *ini, bool *rest)
{
    // Tableau pour mémoriser les sommets visités
    bool *visited;
    CALLOC(visited, G->size);

    graph_search_update(T, G, ini, visited);

    // Construction de la liste des sommets accessibles
    dequeue *res = create_dequeue();
    for (uint q = 0; q < G->size; q++)
    {
        if (visited[q] && (!rest || rest[q]))
        {
            rigins_dequeue(q, res);
        }
    }

    free(visited);
    return res;
}

void lgraph_search_update(graph_stype T, lgraph *G, dequeue *ini, bool *alph, bool *visited)
{
    uint (*f)(dequeue *);
    if (T == DFS)
    {
        f = rigpull_dequeue;
    }
    else
    {
        f = lefpull_dequeue;
    }
    uint start, end;
    if (alph)
    {
        while (!isempty_dequeue(ini))
        {
            uint q = f(ini);
            if (visited[q])
            {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G->size_alpha; a++)
            {
                if (alph[a])
                {
                    start = G->intervals[q * G->size_alpha + a];
                    end = G->intervals[q * G->size_alpha + a + 1];
                    for (uint j = start; j < end; j++)
                    {
                        rigins_dequeue(G->storage[j], ini);

                        // rigins_dequeue(lefread_dequeue(G->edges[q][a], j), ini);
                    }
                }
            }
        }
    }
    else
    {
        while (!isempty_dequeue(ini))
        {
            uint q = f(ini);
            if (visited[q])
            {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G->size_alpha; a++)
            {
                start = G->intervals[q * G->size_alpha + a];
                end = G->intervals[q * G->size_alpha + a + 1];
                for (uint j = start; j < end; j++)
                {
                    rigins_dequeue(G->storage[j], ini);
                }
            }
        }
    }
}

dequeue *lgraph_search(graph_stype T, lgraph *G, dequeue *ini, bool *alph, bool *rest)
{

    // Tableau pour mémoriser les sommets visités
    bool *visited;
    CALLOC(visited, G->size_graph);

    lgraph_search_update(T, G, ini, alph, visited);
    dequeue *res = create_dequeue();
    for (uint q = 0; q < G->size_graph; q++)
    {
        if (visited[q] && (!rest || rest[q]))
        {
            rigins_dequeue(q, res);
        }
    }

    free(visited);

    return res;
}

void dgraph_search_update(graph_stype T, dgraph *G, dequeue *ini, bool *alph, bool *visited)
{
    uint (*f)(dequeue *);
    if (T == DFS)
    {
        f = rigpull_dequeue;
    }
    else
    {
        f = lefpull_dequeue;
    }
    if (alph)
    {
        while (!isempty_dequeue(ini))
        {
            uint q = f(ini);
            if (visited[q])
            {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G->size_alpha; a++)
            {
                if (alph[a] && G->edges[q][a] != UINT_MAX)
                {
                    rigins_dequeue(G->edges[q][a], ini);
                }
            }
        }
    }
    else
    {
        while (!isempty_dequeue(ini))
        {
            uint q = f(ini);
            if (visited[q])
            {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G->size_alpha; a++)
            {
                if (G->edges[q][a] != UINT_MAX)
                {
                    rigins_dequeue(G->edges[q][a], ini);
                }
            }
        }
    }
}

dequeue *dgraph_search(graph_stype T, dgraph *G, dequeue *ini, bool *alph, bool *rest)
{

    // Tableau pour mémoriser les sommets visités
    bool *visited;
    CALLOC(visited, G->size_graph);

    for (uint q = 0; q < G->size_graph; q++)
    {
        visited[q] = false;
    }

    dgraph_search_update(T, G, ini, alph, visited);
    dequeue *res = create_dequeue();
    for (uint q = 0; q < G->size_graph; q++)
    {
        if (visited[q] && (!rest || rest[q]))
        {
            rigins_dequeue(q, res);
        }
    }
    free(visited);

    return res;
}

void twin_dgraph_search_update(graph_stype T, dgraph *G1, dgraph *G2, dequeue *ini, bool *alph, bool *visited)
{
    if (G1->size_graph != G2->size_graph || G1->size_alpha != G2->size_alpha)
    {
        fprintf(stderr, "Error: the two graphs have different sizes.\n");
        return;
    }

    uint (*f)(dequeue *);
    if (T == DFS)
    {
        f = rigpull_dequeue;
    }
    else
    {
        f = lefpull_dequeue;
    }

    if (alph)
    {
        while (!isempty_dequeue(ini))
        {
            uint q = f(ini);
            if (visited[q])
            {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G1->size_alpha; a++)
            {
                if (alph[a] && G1->edges[q][a] != UINT_MAX && G2->edges[q][a] != UINT_MAX)
                {
                    rigins_dequeue(G1->edges[q][a], ini);
                    rigins_dequeue(G2->edges[q][a], ini);
                }
            }
        }
    }
    else
    {
        while (!isempty_dequeue(ini))
        {
            uint q = f(ini);
            if (visited[q])
            {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G1->size_alpha; a++)
            {
                if (G1->edges[q][a] != UINT_MAX && G2->edges[q][a] != UINT_MAX)
                {
                    rigins_dequeue(G1->edges[q][a], ini);
                    rigins_dequeue(G2->edges[q][a], ini);
                }
            }
        }
    }
}

dequeue *twin_dgraph_search(graph_stype T, dgraph *G1, dgraph *G2, dequeue *ini, bool *alph, bool *rest)
{
    if (G1->size_graph != G2->size_graph || G1->size_alpha != G2->size_alpha)
    {
        fprintf(stderr, "Error: the two graphs have different sizes.\n");
        return NULL;
    }

    // Tableau pour mémoriser les sommets visités
    bool *visited;
    CALLOC(visited, G1->size_graph);

    twin_dgraph_search_update(T, G1, G2, ini, alph, visited);
    dequeue *res = create_dequeue();
    for (uint q = 0; q < G1->size_graph; q++)
    {
        if (visited[q] && (!rest || rest[q]))
        {
            rigins_dequeue(q, res);
        }
    }

    free(visited);

    return res;
}

dgraph *dgraph_paths(dgraph *G, uint start)
{
    bool *visited;
    CALLOC(visited, G->size_graph);
    dgraph *res = create_dgraph_noedges(G->size_graph, G->size_alpha);

    dequeue *thequeue = create_dequeue();
    rigins_dequeue(start, thequeue);
    visited[start] = true;
    while (!isempty_dequeue(thequeue))
    {
        uint q = lefpull_dequeue(thequeue);
        for (uint a = 0; a < G->size_alpha; a++)
        {
            uint r = G->edges[q][a];
            if (r != UINT_MAX && !visited[r])
            {
                rigins_dequeue(r, thequeue);
                visited[r] = true;
                res->edges[r][a] = q; // On enregistre le chemin
                res->size_edges++;
            }
        }
    }
    delete_dequeue(thequeue);
    free(visited);
    return res;
}

/*****************************************************************/
/*+ Récupération des sommets adjacents à un ensemble de sommets +*/
/*****************************************************************/

void lgraph_reachable(lgraph *G, uint *in, uint *out, uint a)
{
    uint size_uint = (G->size_graph + sizeof(uint) - 1) / sizeof(uint);
    for (uint i = 0; i < size_uint; i++)
    {
        out[i] = 0;
    }

    for (uint q = 0; q < G->size_graph; q++)
    {
        uint n = q / sizeof(uint);
        uint l = q % sizeof(uint);
        if (in[n] & (1 << l))
        {
            uint start = G->intervals[q * G->size_alpha + a];
            uint end = G->intervals[q * G->size_alpha + a + 1];
            for (uint j = start; j < end; j++)
            {
                uint to = G->storage[j];
                uint m = to / sizeof(uint);
                uint k = to % sizeof(uint);
                out[m] |= (1 << k);
            }
        }
    }
}

void lgraph_reachable_eps(lgraph *G, uint *in, uint *out, uint a, uint ieps)
{
    uint size_uint = (G->size_graph + sizeof(uint) - 1) / sizeof(uint);
    for (uint i = 0; i < size_uint; i++)
    {
        out[i] = 0;
    }

    bool *visited;
    CALLOC(visited, G->size_graph);
    dequeue *thestack = create_dequeue();

    for (uint q = 0; q < G->size_graph; q++)
    {
        uint n = q / sizeof(uint);
        uint l = q % sizeof(uint);
        if (!(in[n] & (1 << l)))
        {
            continue;
        }
        rigins_dequeue(q, thestack);
        while (!isempty_dequeue(thestack))
        {
            uint r = rigpull_dequeue(thestack);
            if (visited[r])
            {
                continue;
            }
            visited[r] = true;
            uint start = LGSTART(G, r, a);
            uint end = LGEND(G, r, a);
            for (uint j = start; j < end; j++)
            {
                uint to = G->storage[j];
                uint m = to / sizeof(uint);
                uint k = to % sizeof(uint);
                out[m] |= (1 << k);
            }

            start = LGSTART(G, r, ieps);
            end = LGEND(G, r, ieps);
            for (uint j = start; j < end; j++)
            {
                rigins_dequeue(G->storage[j], thestack);
            }
        }
    }
    delete_dequeue(thestack);
    free(visited);
}

/**********************************/
/*+ Paths computation in dgraphs +*/
/**********************************/

bool dgraph_exists_path(dgraph *g, uint s, uint e, bool strict, uint **theword)
{
    // Quick check for the trivial case
    if (!strict && s == e)
    {
        if (theword)
        {
            MALLOC(*theword, 1);      // Allocate memory for the path
            (*theword)[0] = UINT_MAX; // Set the path to empty
        }
        return true; // If the starting states are the same as the ending states, we found an intersection path
    }

    // The alphabet
    uint thealph = g->size_alpha;

    bool *visited;
    CALLOC(visited, g->size_graph); // Allocate memory for the visited states
    uint *pred_state;
    CALLOC(pred_state, g->size_graph); // Allocate memory for the predecessor states
    uint *pred_symbol;
    CALLOC(pred_symbol, g->size_graph); // Allocate memory for the predecessor symbols

    // Queue containing the elements to be processed.
    dequeue *thequeue = create_dequeue();

    // Create the starting element
    visited[s] = true;           // Mark the starting state as visited
    rigins_dequeue(s, thequeue); // Add the starting state to the queue.

    // BFS
    while (!isempty_dequeue(thequeue))
    {
        uint q = lefpull_dequeue(thequeue); // Retrieve the state to be processed.

        // Handling transitions
        for (uint a = 0; a < thealph; a++)
        {
            if (g->edges[q][a] == UINT_MAX)
            {
                continue; // If there are no transitions for this symbol, skip it
            }
            if (g->edges[q][a] == e)
            {
                delete_dequeue(thequeue); // If we reach the final states, we can stop the search.
                if (theword)              // If we need to store the word labeling the path.
                {
                    uint r = q;
                    uint k = 1;
                    while (r != s)
                    {
                        k++;               // Count the number of transitions in the path
                        r = pred_state[r]; // Backtrack to find the previous pair
                    }
                    r = q;
                    MALLOC(*theword, k + 1); // Allocate memory for the path
                    (*theword)[k] = UINT_MAX;
                    k--;
                    (*theword)[k] = a; // Store the last transition in the path
                    while (r != s)
                    {                                   // Backtrack to find the path
                        k--;                            // Decrement the index for the path
                        (*theword)[k] = pred_symbol[r]; // Store the transition leading to this pair
                        r = pred_state[r];              // Backtrack to the previous pair
                    }
                }
                free(visited);     // Free the memory allocated for the visited states
                free(pred_state);  // Free the memory allocated for the predecessor states
                free(pred_symbol); // Free the memory allocated for the predecessor symbols
                return true;       // We found an intersection path
            }

            if (!visited[g->edges[q][a]])
            {                                             // If the state has not been visited yet
                visited[g->edges[q][a]] = true;           // Mark it as visited
                rigins_dequeue(g->edges[q][a], thequeue); // Add the state to the queue for future processing.
                pred_state[g->edges[q][a]] = q;           // Store the predecessor state
                pred_symbol[g->edges[q][a]] = a;          // Store the symbol leading to this state
            }
        }
    }

    // DFS completed. We can delete the queue and the hash table.
    delete_dequeue(thequeue);
    free(visited);     // Free the memory allocated for the visited states
    free(pred_state);  // Free the memory allocated for the predecessor states
    free(pred_symbol); // Free the memory allocated for the predecessor symbols

    return false; // No intersection path found
}

bool dgraph_exists_path_alpha(dgraph *g, uint s, uint e, bool strict, bool *alpha, uint **theword)
{
    // Quick check for the trivial case
    if (!strict && s == e)
    {
        if (theword)
        {
            MALLOC(*theword, 1);      // Allocate memory for the path
            (*theword)[0] = UINT_MAX; // Set the path to empty
        }
        return true; // If the starting states are the same as the ending states, we found an intersection path
    }

    // The alphabet
    uint thealph = g->size_alpha;

    bool *visited;
    CALLOC(visited, g->size_graph); // Allocate memory for the visited states
    uint *pred_state;
    CALLOC(pred_state, g->size_graph); // Allocate memory for the predecessor states
    uint *pred_symbol;
    CALLOC(pred_symbol, g->size_graph); // Allocate memory for the predecessor symbols

    // Queue containing the elements to be processed.
    dequeue *thequeue = create_dequeue();

    // Create the starting element
    visited[s] = true;           // Mark the starting state as visited
    rigins_dequeue(s, thequeue); // Add the pair to the queue.

    // BFS
    while (!isempty_dequeue(thequeue))
    {
        uint q = lefpull_dequeue(thequeue); // Retrieve the state to be processed.

        // Handling transitions
        for (uint a = 0; a < thealph; a++)
        {
            if ((alpha && !alpha[a]) || g->edges[q][a] == UINT_MAX)
            {
                continue;
            }
            if (g->edges[q][a] == e)
            {
                delete_dequeue(thequeue); // If we reach the final states, we can stop the search
                if (theword)              // If we need to store the word labeling the path.
                {
                    uint r = q;
                    uint k = 1;
                    while (r != s)
                    {
                        k++;               // Count the number of transitions in the path
                        r = pred_state[r]; // Backtrack to find the previous pair
                    }
                    r = q;
                    MALLOC(*theword, k + 1); // Allocate memory for the path
                    (*theword)[k] = UINT_MAX;
                    k--;
                    (*theword)[k] = a; // Store the last transition in the path
                    while (r != s)
                    {                                   // Backtrack to find the path
                        k--;                            // Decrement the index for the path
                        (*theword)[k] = pred_symbol[r]; // Store the transition leading to this pair
                        r = pred_state[r];              // Backtrack to the previous pair
                    }
                }
                free(visited);     // Free the memory allocated for the visited states
                free(pred_state);  // Free the memory allocated for the predecessor states
                free(pred_symbol); // Free the memory allocated for the predecessor symbols
                return true;       // We found an intersection path
            }

            if (!visited[g->edges[q][a]])
            {                                             // If the state has not been visited yet
                visited[g->edges[q][a]] = true;           // Mark it as visited
                rigins_dequeue(g->edges[q][a], thequeue); // Add the state to the queue for future processing.
                pred_state[g->edges[q][a]] = q;           // Store the predecessor state
                pred_symbol[g->edges[q][a]] = a;          // Store the symbol leading to this state
            }
        }
    }

    // DFS completed. We can delete the queue and the hash table.
    delete_dequeue(thequeue);
    free(visited);     // Free the memory allocated for the visited states
    free(pred_state);  // Free the memory allocated for the predecessor states
    free(pred_symbol); // Free the memory allocated for the predecessor symbols

    return false; // No intersection path found
}

uint dgraph_exists_path_to_letter(dgraph *g, uint s, uint b, bool *alpha, uint **theword)
{

    // Quick check for the trivial case when a b-transition exists from s
    if (g->edges[s][b] != UINT_MAX)
    {
        if (theword)
        {
            MALLOC(*theword, 1);      // Allocate memory for the path
            (*theword)[0] = UINT_MAX; // Set the path to empty
        }
        return s; // If the starting states are the same as the ending states, we found an intersection path
    }

    // The alphabet
    uint thealph = g->size_alpha;

    bool *visited;
    CALLOC(visited, g->size_graph); // Allocate memory for the visited states
    uint *pred_state;
    CALLOC(pred_state, g->size_graph); // Allocate memory for the predecessor states
    uint *pred_symbol;
    CALLOC(pred_symbol, g->size_graph); // Allocate memory for the predecessor symbols

    // Queue containing the elements to be processed.
    dequeue *thequeue = create_dequeue();

    // Create the starting element
    visited[s] = true;           // Mark the starting state as visited
    rigins_dequeue(s, thequeue); // Add the pair to the queue.

    // BFS
    while (!isempty_dequeue(thequeue))
    {
        uint q = lefpull_dequeue(thequeue); // Retrieve the state to be processed.

        // Handling transitions
        for (uint a = 0; a < thealph; a++)
        {
            if (alpha && !alpha[a])
            {
                continue; // If the symbol is not in the alphabet, skip it
            }

            if (g->edges[q][a] == UINT_MAX)
            {
                continue; // If there are no transitions for this symbol, skip it
            }
            if (g->edges[g->edges[q][a]][b] != UINT_MAX)
            {
                delete_dequeue(thequeue); // If we reach the final states, we can stop the search
                if (theword)
                {
                    uint r = q;
                    uint k = 1;
                    while (r != s)
                    {
                        k++;               // Count the number of transitions in the path
                        r = pred_state[r]; // Backtrack to find the previous pair
                    }
                    r = q;
                    MALLOC(*theword, k + 1); // Allocate memory for the path
                    (*theword)[k] = UINT_MAX;
                    k--;
                    (*theword)[k] = a; // Store the last transition in the path
                    while (r != s)
                    {                                   // Backtrack to find the path
                        k--;                            // Decrement the index for the path
                        (*theword)[k] = pred_symbol[r]; // Store the transition leading to this pair
                        r = pred_state[r];              // Backtrack to the previous pair
                    }
                }
                free(visited);         // Free the memory allocated for the visited states
                free(pred_state);      // Free the memory allocated for the predecessor states
                free(pred_symbol);     // Free the memory allocated for the predecessor symbols
                return g->edges[q][a]; // We found a path ending at this state.
            }

            if (!visited[g->edges[q][a]])
            {                                             // If the state has not been visited yet
                visited[g->edges[q][a]] = true;           // Mark it as visited
                rigins_dequeue(g->edges[q][a], thequeue); // Add the state to the queue for future processing.
                pred_state[g->edges[q][a]] = q;           // Store the predecessor state
                pred_symbol[g->edges[q][a]] = a;          // Store the symbol leading to this state
            }
        }
    }

    // DFS completed. We can delete the queue and the hash table.
    delete_dequeue(thequeue);
    free(visited);     // Free the memory allocated for the visited states
    free(pred_state);  // Free the memory allocated for the predecessor states
    free(pred_symbol); // Free the memory allocated for the predecessor symbols

    return UINT_MAX; // No intersection path found
}

bool dgraph_getloop_alpha(dgraph *g, parti *sccs, bool *alph, uint s, uint **theword)
{
    uint *inv_sccs = parti_compute_inv(sccs);
    dgraph *local = dgraph_extract(g, sccs, inv_sccs, sccs->numcl[s]);
    uint *words[g->size_alpha];

    uint t = inv_sccs[s];
    uint length = 0;
    uint i = 0;

    // We seek a path including all letters in the alphabet
    for (uint a = 0; a < g->size_alpha; a++)
    {
        words[a] = NULL;
        if (!alph[a])
        {
            continue; // We only need letters in the given alphabet.
        }
        // printf("Letter %u\n", a);
        t = dgraph_exists_path_to_letter(local, t, a, alph, &words[a]);
        if (t == UINT_MAX)
        {
            free(local);
            for (uint c = 0; c <= a; c++)
            {
                free(words[c]);
            }
            return false; // No path found for this letter
        }
        // printf("Found path for letter %u: \n", a);
        t = local->edges[t][a];
        i = 0;
        while (words[a][i] != UINT_MAX)
        {
            i++;
        }
        length += (i + 1); // +1 for the letter itself
    }

    // We go back to the starting state
    uint *end;
    if (!dgraph_exists_path(local, t, inv_sccs[s], false, &end))
    {
        free(local);
        for (uint a = 0; a < g->size_alpha; a++)
        {
            free(words[a]);
        }
        return false; // No path found to the starting state
    }
    i = 0;
    while (end[i] != UINT_MAX)
    {
        i++;
    }
    length += i;

    uint *word;
    MALLOC(word, length + 1);
    word[length] = UINT_MAX; // End of the word
    uint j = 0;
    for (uint a = 0; a < g->size_alpha; a++)
    {
        if (!alph[a])
        {
            continue;
        }
        i = 0;
        while (words[a][i] != UINT_MAX)
        {
            word[j] = words[a][i];
            i++;
            j++;
        }
        word[j] = a; // Add the letter
        j++;
        free(words[a]);
    }
    i = 0;
    while (end[i] != UINT_MAX)
    {
        word[j] = end[i];
        i++;
        j++;
    }
    free(end);
    free(inv_sccs);

    delete_dgraph(local);
    if (theword != NULL)
    {
        *theword = word;
    }
    else
    {
        free(word);
    }
    return true;
}

/****************************/
/*+ Edge lists computation +*/
/****************************/

int edge_pair_sort(const void *e1, const void *e2)
{
    const edge_pair *et1 = (const edge_pair *)e1;
    const edge_pair *et2 = (const edge_pair *)e2;
    if (et1->from < et2->from)
    {
        return -1;
    }
    else if (et1->from > et2->from)
    {
        return 1;
    }
    return et1->to - et2->to;
}

edge_pair *graph_to_edge_list(graph *g, bool inv)
{
    if (!g || g->size_edges == 0)
    {
        return NULL;
    }
    edge_pair *elist;
    MALLOC(elist, g->size_edges);
    uint num = 0;
    for (uint q = 0; q < g->size; q++)
    {
        uint start = g->intervals[q];
        uint end = g->intervals[q + 1];
        for (uint j = start; j < end; j++)
        {
            elist[num].from = inv ? g->storage[j] : q;
            elist[num].to = inv ? q : g->storage[j];
            num++;
        }
    }
    return elist;
}

graph *edge_list_to_graph(edge_pair *elist, uint elist_size, uint num_vertices)
{
    if (elist_size == 0)
    {
        return create_graph_noedges(num_vertices);
    }

    // Creation of the graph
    graph *g;
    CALLOC(g, 1);
    g->size = num_vertices;
    CALLOC(g->intervals, num_vertices + 1);
    CALLOC(g->storage, elist_size);

    // We first sort the edge list by (from, lab, to)
    qsort(elist, elist_size, sizeof(edge_pair), edge_pair_sort);

    uint i = 0;     // Index in the edge list
    uint count = 0; // Number of edges added to the graph so far (can be less than i due to duplicates)
    for (uint q = 0; q < num_vertices; q++)
    {
        g->intervals[q] = count;
        if (i >= elist_size || elist[i].from != q)
        {
            continue;
        }
        g->storage[count] = elist[i].to;
        count++;
        i++;
        while (i < elist_size && elist[i].from == q)
        {
            if (elist[i].to == elist[i - 1].to)
            {
                i++;
                continue; // To avoid duplicates
            }
            g->storage[count] = elist[i].to;
            count++;
            i++;
        }
    }

    g->intervals[num_vertices] = count;
    g->size_edges = count;
    if (count < elist_size)
    {
        REALLOC(g->storage, count);
    }
    return g;
}

dyn_edge_pair *create_dyn_edge_pair(void)
{
    dyn_edge_pair *dep;
    MALLOC(dep, 1);
    dep->size = 0;
    dep->capacity = 16;
    MALLOC(dep->array, dep->capacity);
    return dep;
}
void delete_dyn_edge_pair(dyn_edge_pair *dep)
{
    if (!dep)
    {
        return;
    }
    free(dep->array);
    free(dep);
}
void dyn_edge_pair_add(dyn_edge_pair *dep, uint from, uint to)
{
    if (dep->size == dep->capacity)
    {
        dep->capacity <<= 1;
        REALLOC(dep->array, dep->capacity);
    }
    dep->array[dep->size].from = from;
    dep->array[dep->size].to = to;
    dep->size++;
}

int edge_triple_sort(const void *e1, const void *e2)
{
    const edge_triple *et1 = (const edge_triple *)e1;
    const edge_triple *et2 = (const edge_triple *)e2;
    if (et1->from < et2->from)
    {
        return -1;
    }
    else if (et1->from > et2->from)
    {
        return 1;
    }

    if (et1->lab < et2->lab)
    {
        return -1;
    }
    else if (et1->lab > et2->lab)
    {
        return 1;
    }

    return et1->to - et2->to;
}

edge_triple *dgraph_to_edge_list(dgraph *g, bool inv)
{
    if (!g || g->size_edges == 0)
    {
        return NULL;
    }

    edge_triple *elist;
    MALLOC(elist, g->size_edges);
    uint num = 0;
    for (uint q = 0; q < g->size_graph; q++)
    {
        for (uint a = 0; a < g->size_alpha; a++)
        {
            if (g->edges[q][a] == UINT_MAX)
            {
                continue;
            }
            elist[num].from = inv ? g->edges[q][a] : q;
            elist[num].lab = a;
            elist[num].to = inv ? q : g->edges[q][a];
            num++;
        }
    }

    return elist;
}

edge_triple *lgraph_to_edge_list(lgraph *g, bool inv)
{
    if (!g || g->size_edges == 0)
    {
        return NULL;
    }
    edge_triple *elist;
    MALLOC(elist, g->size_edges);
    uint idx = 0;
    uint num = 0;
    for (uint q = 0; q < g->size_graph; q++)
    {
        for (uint a = 0; a < g->size_alpha; a++)
        {
            uint start = g->intervals[idx];
            uint end = g->intervals[++idx];
            for (uint j = start; j < end; j++)
            {
                elist[num].from = inv ? g->storage[j] : q;
                elist[num].lab = a;
                elist[num].to = inv ? q : g->storage[j];
                num++;
            }
        }
    }
    return elist;
}

lgraph *edge_list_to_lgraph(edge_triple *elist, uint elist_size, uint num_vertices, uint num_labels)
{
    if (elist_size == 0)
    {
        return create_lgraph_noedges(num_vertices, num_labels);
    }

    lgraph *g;
    CALLOC(g, 1);
    g->size_graph = num_vertices;
    g->size_alpha = num_labels;
    CALLOC(g->intervals, num_vertices * num_labels + 1);
    CALLOC(g->storage, elist_size);

    // We first sort the edge list by (from, lab, to)
    qsort(elist, elist_size, sizeof(edge_triple), edge_triple_sort);

    // for (uint i = 0; i < elist_size; i++)
    // {
    //     printf("%d --%d--> %d\n", elist[i].from, elist[i].lab, elist[i].to);
    // }

    uint count = 0;
    uint j = 0;

    for (uint q = 0; q < num_vertices; q++)
    {
        for (uint a = 0; a < num_labels; a++)
        {
            g->intervals[q * num_labels + a] = count;

            if (j >= elist_size || elist[j].from != q || elist[j].lab != a)
            {
                continue;
            }

            g->storage[count++] = elist[j].to;
            // printf("Adding edge %d --%d--> %d\n", elist[j].from, elist[j].lab, elist[j].to);
            j++;
            while (j < elist_size && elist[j].from == q && elist[j].lab == a)
            {
                if (elist[j].to != elist[j - 1].to) // To avoid duplicates
                {
                    g->storage[count++] = elist[j].to;
                }
                j++;
            }
        }
    }

    g->intervals[num_vertices * num_labels] = count;
    g->size_edges = count;
    if (count < elist_size)
    {

        REALLOC(g->storage, count);
    }

    // for (uint i = 0; i < count; i++)
    // {
    //     printf("storage[%d] = %d\n", i, g->storage[i]);
    // }

    // for (uint i = 0; i <= num_vertices * num_labels; i++)
    // {
    //     printf("intervals[%d] = %d\n", i, g->intervals[i]);
    // }

    return g;
}

dyn_edge_triple *create_dyn_edge_triple(void)
{
    dyn_edge_triple *dea;
    MALLOC(dea, 1);
    dea->size = 0;
    dea->capacity = 16;
    MALLOC(dea->array, dea->capacity);
    return dea;
}
void delete_dyn_edge_triple(dyn_edge_triple *dea)
{
    if (!dea)
    {
        return;
    }
    free(dea->array);
    free(dea);
}
void dyn_edge_triple_add(dyn_edge_triple *dea, uint from, uint lab, uint to)
{
    if (dea->size == dea->capacity)
    {
        dea->capacity <<= 1;
        REALLOC(dea->array, dea->capacity);
    }
    dea->array[dea->size].from = from;
    dea->array[dea->size].lab = lab;
    dea->array[dea->size].to = to;
    dea->size++;
}
