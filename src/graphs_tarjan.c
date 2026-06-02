

#include "graphs_tarjan.h"
#include "printing.h"
#include <limits.h>
#include <time.h>

parti *tarjan(graph *g)
{
    // Array storing the class number of each vertex.
    uint *numcl;
    MALLOC(numcl, g->size);

    // Future size of the partition.
    uint size_par = 0;

    // Array storing the visiting order of the vertices
    uint *index;
    CALLOC(index, g->size);

    // Current index
    uint id = 1;

    // Array storing the lowlink of each vertex
    uint *lowlink;
    CALLOC(lowlink, g->size);

    // Stack of SCCs
    uint *SCCstack;
    CALLOC(SCCstack, g->size);

    // Top of the stack
    uint top = 0;

    // Flags indicating whether a vertex is in the stack of SCCs
    bool *onStack;
    CALLOC(onStack, g->size);

    // Stack for the DFS
    dequeue *thestack = create_dequeue();

    // Number of outgoing edges already handled in the DFS for each vertex
    uint *iout;
    CALLOC(iout, g->size);

    // For each vertex in the graph
    for (uint q = 0; q < g->size; q++)
    {
        // If the vertex has already been visited, skip it
        if (index[q] > 0)
        {
            continue;
        }

        // Initialize the index and lowlink of the vertex and push it onto the stack of SSCs
        index[q] = id++;
        lowlink[q] = index[q];
        SCCstack[top++] = q;
        onStack[q] = true;

        // Push the vertex onto the stack for the DFS
        rigins_dequeue(q, thestack);

        // The DFS
        while (!isempty_dequeue(thestack))
        {
            // Get the next vertex to process (without popping it from the stack yet, only done after all its edges have been processed).
            uint v = rigread_dequeue(thestack, 0);

            // If there are still edges to process for the vertex, we process the next one
            uint start = g->intervals[v];
            uint end = g->intervals[v + 1];
            if (start + iout[v] < end)
            {
                uint w = g->storage[start + iout[v]];
                if (index[w] == 0)
                {
                    index[w] = id++;
                    lowlink[w] = index[w];
                    SCCstack[top++] = w;
                    onStack[w] = true;
                    rigins_dequeue(w, thestack);
                }
                iout[v]++;
            }
            else
            {
                // If all edges have been processed, we pop the vertex from the stack
                rigpull_dequeue(thestack);

                // We update the lowlink of the vertex
                for (uint i = start; i < end; i++)
                {
                    uint w = g->storage[i];
                    if (onStack[w])
                    {
                        lowlink[v] = min(lowlink[v], lowlink[w]);
                    }
                }

                // If we have found a SCC, we store it in the result by popping vertices from the stack of SSCs
                if (lowlink[v] == index[v])
                {
                    while (top > 0)
                    {
                        uint w = SCCstack[--top];
                        onStack[w] = false;
                        numcl[w] = size_par;
                        if (w == v)
                        {
                            break;
                        }
                    }
                    size_par++;
                }
            }
        }
    }

    free(index);
    free(lowlink);
    free(SCCstack);
    free(onStack);

    free(iout);
    delete_dequeue(thestack);

    for (uint i = 0; i < g->size; i++)
    {
        numcl[i] = size_par - 1 - numcl[i];
    }
    return create_parti(g->size, size_par, numcl);
}

parti *ltarjan(lgraph *g, bool *alph)
{
    // Array storing the class number of each vertex.
    uint *numcl;
    MALLOC(numcl, g->size_graph);

    // Future size of the partition.
    uint size_par = 0;

    // Array storing the visiting order of the vertices
    uint *index;
    CALLOC(index, g->size_graph);

    // Current index
    uint id = 1;

    // Array storing the lowlink of each vertex
    uint *lowlink;
    CALLOC(lowlink, g->size_graph);

    // Stack of SCCs
    uint *SCCstack;
    CALLOC(SCCstack, g->size_graph);

    // Top of the stack
    uint top = 0;

    // Flags indicating whether a vertex is in the stack of SCCs
    bool *onStack;
    CALLOC(onStack, g->size_graph);

    // Stack for the DFS
    dequeue *thestack = create_dequeue();

    // Number of letters already handled in the DFS for each vertex
    uint *aout;
    CALLOC(aout, g->size_graph);

    // Number of outgoing edges already handled for the current letter in the DFS for each vertex
    uint *iout;
    CALLOC(iout, g->size_graph);

    // For each vertex in the graph
    for (uint q = 0; q < g->size_graph; q++)
    {
        // If the vertex has already been visited, skip it
        if (index[q] > 0)
        {
            continue;
        }

        // Initialize the index and lowlink of the vertex and push it onto the stack of SSCs
        index[q] = id++;
        lowlink[q] = index[q];
        SCCstack[top++] = q;
        onStack[q] = true;

        // Push the vertex onto the stack for the DFS
        rigins_dequeue(q, thestack);

        // The DFS
        while (!isempty_dequeue(thestack))
        {
            // Get the next vertex to process (without popping it from the stack yet)
            uint v = rigread_dequeue(thestack, 0);

            // If the alphabet is restricted, we skip the letters that are not in the alphabet
            if (alph)
            {
                while (aout[v] < g->size_alpha && !alph[aout[v]])
                {
                    aout[v]++;
                }
            }

            if (aout[v] < g->size_alpha)
            {
                uint start = LGSTART(g, v, aout[v]);
                uint end = LGEND(g, v, aout[v]);

                // If there are still edges to process for the vertex and the current label, we process the next one
                if (start + iout[v] < end)
                {
                    uint w = g->storage[start + iout[v]];
                    if (index[w] == 0)
                    {
                        index[w] = id++;
                        lowlink[w] = index[w];
                        SCCstack[top++] = w;
                        onStack[w] = true;
                        rigins_dequeue(w, thestack);
                    }
                    iout[v]++;
                    continue;
                } // If there are still labels to process, we move to the next label
                else if (aout[v] < g->size_alpha - 1)
                {
                    aout[v]++;
                    iout[v] = 0;
                    continue;
                }
            }
            // If all edges have been processed, we pop the vertex from the stack
            rigpull_dequeue(thestack);

            // We update the lowlink of the vertex
            for (uint a = 0; a < g->size_alpha; a++)
            {
                if (alph && !alph[a])
                {
                    continue;
                }
                uint start = LGSTART(g, v, a);
                uint end = LGEND(g, v, a);

                for (uint i = start; i < end; i++)
                {
                    uint w = g->storage[i];
                    if (onStack[w])
                    {
                        lowlink[v] = min(lowlink[v], lowlink[w]);
                    }
                }
            }

            // If we have found a SCC, we store it in the result by popping vertices from the stack of SSCs
            if (lowlink[v] == index[v])
            {
                while (top > 0)
                {
                    uint w = SCCstack[--top];
                    onStack[w] = false;
                    numcl[w] = size_par;
                    if (w == v)
                    {
                        break;
                    }
                }
                size_par++;
            }
        }
    }

    free(index);
    free(lowlink);
    free(SCCstack);
    free(onStack);

    free(aout);
    free(iout);
    delete_dequeue(thestack);

    for (uint i = 0; i < g->size_graph; i++)
    {
        numcl[i] = size_par - 1 - numcl[i];
    }

    return create_parti(g->size_graph, size_par, numcl);
}

parti *dtarjan(dgraph *g, bool *alph, bool ismor)
{
#ifdef DEBUG_TARJAN
    printf("Starting Tarjan's algorithm (dtarjan) on a graph with %u vertices and %u labels.\n", g->size_graph, g->size_alpha);
#endif

    uint *numcl;
    MALLOC(numcl, g->size_graph);
    uint size_par = 0;

    // Array storing the visiting order of the vertices
    uint *index;
    CALLOC(index, g->size_graph);

    // Current index
    uint id = 1;

    // Array storing the lowlink of each vertex
    uint *lowlink;
    CALLOC(lowlink, g->size_graph);

    // Stack of SCCs
    uint *SCCstack;
    CALLOC(SCCstack, g->size_graph);

    // Top of the stack
    uint top = 0;

    // Flags indicating whether a vertex is in the stack of SCCs
    bool *onStack;
    CALLOC(onStack, g->size_graph);

    // Stack for the DFS
    dequeue *thestack = create_dequeue();

    // Number of labels already handled in the DFS for each vertex
    uint *aout;
    CALLOC(aout, g->size_graph);

    uint end;
    if (ismor)
    {
        end = 1;
    }
    else
    {
        end = g->size_graph;
    }

#ifdef DEBUG_TARJAN
    printf("Starting the loop. end: %u.\n", end);
#endif

    // For each vertex in the graph
    for (uint q = 0; q < end; q++)
    {
        // If the vertex has already been visited, skip it
        if (index[q] > 0)
        {
            continue;
        }

        // Initialize the index and lowlink of the vertex and push it onto the stack of SSCs
        index[q] = id++;
        lowlink[q] = index[q];
        SCCstack[top++] = q;
        onStack[q] = true;

        // Push the vertex onto the stack for the DFS
        rigins_dequeue(q, thestack);

        // The DFS
        while (!isempty_dequeue(thestack))
        {
            // Get the next vertex to process (without popping it from the stack yet)
            uint v = rigread_dequeue(thestack, 0);

            // If the alphabet is restricted, we skip the letters that are not in the alphabet
            if (alph)
            {
                while (aout[v] < g->size_alpha && !alph[aout[v]])
                {
                    aout[v]++;
                }
            }

            // If there are still edges to process for the vertex, we process the next one
            if (aout[v] < g->size_alpha)
            {
                uint w = g->edges[v][aout[v]];
                if (w != UINT_MAX && index[w] == 0)
                {
                    index[w] = id++;
                    lowlink[w] = index[w];
                    SCCstack[top++] = w;
                    onStack[w] = true;
                    rigins_dequeue(w, thestack);
                }
                aout[v]++;
            }
            else
            {
                // If all edges have been processed, we pop the vertex from the stack
                rigpull_dequeue(thestack);

                // We update the lowlink of the vertex
                for (uint a = 0; a < g->size_alpha; a++)
                {
                    if (alph && !alph[a])
                    {
                        continue;
                    }

                    if (g->edges[v][a] != UINT_MAX && onStack[g->edges[v][a]])
                    {
                        lowlink[v] = min(lowlink[v], lowlink[g->edges[v][a]]);
                    }
                }
                // If we have found a SCC, we store it in the result by popping vertices from the stack of SSCs
                if (lowlink[v] == index[v])
                {
                    while (top > 0)
                    {
                        uint w = SCCstack[--top];
                        onStack[w] = false;
                        numcl[w] = size_par;
                        if (w == v)
                        {
                            break;
                        }
                    }
                    size_par++;
                }
            }
        }
    }

    free(index);
    free(lowlink);
    free(SCCstack);
    free(onStack);

    free(aout);
    delete_dequeue(thestack);

    for (uint i = 0; i < g->size_graph; i++)
    {
        numcl[i] = size_par - 1 - numcl[i];
    }

#ifdef DEBUG_TARJAN
    printf("End of Tarjan's algorithm.\n");
#endif

    return create_parti(g->size_graph, size_par, numcl);
}

parti *dualdtarjan(dgraph *g1, dgraph *g2, bool *alph, bool ismor)
{
    if (g1->size_graph != g2->size_graph || g1->size_alpha != g2->size_alpha)
    {
        fprintf(stderr, "Error in dualdtarjan: The two graphs must have the same size and number of labels.\n");
        exit(EXIT_FAILURE);
    }

    uint *numcl;
    MALLOC(numcl, g1->size_graph);
    uint size_par = 0;

    // Array storing the visiting order of the vertices
    uint *index;
    CALLOC(index, g1->size_graph);

    // Current index
    uint id = 1;

    // Array storing the lowlink of each vertex
    uint *lowlink;
    CALLOC(lowlink, g1->size_graph);

    // Stack of SCCs
    uint *SCCstack;
    CALLOC(SCCstack, g1->size_graph);

    // Top of the stack
    uint top = 0;

    // Flags indicating whether a vertex is in the stack of SCCs
    bool *onStack;
    CALLOC(onStack, g1->size_graph);

    // Stack for the DFS
    dequeue *thestack = create_dequeue();

    // Number of labels already handled in the DFS for each vertex
    uint *aout1;
    CALLOC(aout1, g1->size_graph);
    uint *aout2;
    CALLOC(aout2, g2->size_graph);

    uint end;
    if (ismor)
    {
        end = 1;
    }
    else
    {
        end = g1->size_graph;
    }

    // For each vertex in the graph
    for (uint q = 0; q < end; q++)
    {
        // If the vertex has already been visited, skip it
        if (index[q] > 0)
        {
            continue;
        }

        // Initialize the index and lowlink of the vertex and push it onto the stack of SSCs
        index[q] = id++;
        lowlink[q] = index[q];
        SCCstack[top++] = q;
        onStack[q] = true;

        // Push the vertex onto the stack for the DFS
        rigins_dequeue(q, thestack);

        // The DFS
        while (!isempty_dequeue(thestack))
        {
            // Get the next vertex to process (without popping it from the stack yet)
            uint v = rigread_dequeue(thestack, 0);

            // If there are still edges to process for the vertex, we process the next one
            if (aout1[v] < g1->size_alpha)
            {
                if (!alph || alph[aout1[v]])
                {
                    uint w = g1->edges[v][aout1[v]];
                    if (index[w] == 0)
                    {
                        index[w] = id++;
                        lowlink[w] = index[w];
                        SCCstack[top++] = w;
                        onStack[w] = true;
                        rigins_dequeue(w, thestack);
                    }
                }
                aout1[v]++;
            }
            else if (aout2[v] < g2->size_alpha)
            {
                if (!alph || alph[aout2[v]])
                {
                    uint w = g2->edges[v][aout2[v]];
                    if (index[w] == 0)
                    {
                        index[w] = id++;
                        lowlink[w] = index[w];
                        SCCstack[top++] = w;
                        onStack[w] = true;
                        rigins_dequeue(w, thestack);
                    }
                }
                aout2[v]++;
            }
            else
            {
                // If all edges have been processed, we pop the vertex from the stack
                rigpull_dequeue(thestack);

                // We update the lowlink of the vertex
                for (uint a = 0; a < g1->size_alpha; a++)
                {
                    if (!alph || alph[a])
                    {
                        if (onStack[g1->edges[v][a]])
                        {
                            lowlink[v] = min(lowlink[v], lowlink[g1->edges[v][a]]);
                        }
                        if (onStack[g2->edges[v][a]])
                        {
                            lowlink[v] = min(lowlink[v], lowlink[g2->edges[v][a]]);
                        }
                    }
                }

                // If we have found a SCC, we store it in the result by popping vertices from the stack of SSCs
                if (lowlink[v] == index[v])
                {
                    while (top > 0)
                    {
                        uint w = SCCstack[--top];
                        onStack[w] = false;
                        numcl[w] = size_par;
                        if (w == v)
                        {
                            break;
                        }
                    }
                    size_par++;
                }
            }
        }
    }
    free(index);
    free(lowlink);
    free(SCCstack);
    free(onStack);

    free(aout1);
    free(aout2);
    delete_dequeue(thestack);

    for (uint i = 0; i < g1->size_graph; i++)
    {
        // printf("numcl[%u] = %u\n", i, numcl[i]);
        numcl[i] = size_par - 1 - numcl[i];
    }
    return create_parti(g1->size_graph, size_par, numcl);
}

/**************************/
/*+ Computations on SCCS +*/
/**************************/

dgraph *dgraph_extract(dgraph *G, parti *P, uint *inv, uint j)
{
    // Creation of the graph
    dgraph *res = create_dgraph_noedges(P->cl_size[j], G->size_alpha);
    // Pour chaque élément du morphisme
    for (uint i = 0; i < P->cl_size[j]; i++)
    {
        uint q = P->cl_elems[j][i];
        for (uint a = 0; a < G->size_alpha; a++)
        {
            uint r = G->edges[q][a];
            if (r != UINT_MAX && P->numcl[r] == j)
            {
                res->edges[i][a] = inv[r];
                res->size_edges++;
            }
            else
            {
                res->edges[i][a] = UINT_MAX;
            }
        }
    }
    return res;
}

dgraph *dgraph_to_unary(dgraph *G)
{
    dgraph *res = create_dgraph_noedges(G->size_graph, 1);
    for (uint q = 0; q < G->size_graph; q++)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {
            uint r = G->edges[q][a];
            if (r != UINT_MAX)
            {
                res->edges[q][0] = r;
                res->size_edges++;
                break;
            }
        }
    }
    return res;
}

bool dgraph_compute_alph_scc(dgraph *g, parti *sccs, uint scc, bool *alph)
{
    // Initialize the alphabet to false
    for (uint a = 0; a < g->size_alpha; a++)
    {
        alph[a] = false;
    }

    bool ret = false;

    // For each label
    for (uint a = 0; a < g->size_alpha; a++)
    {
        // For each vertex in the SCC
        for (uint i = 0; i < sccs->cl_size[scc]; i++)
        {
            uint v = sccs->cl_elems[scc][i];

            // If there is an edge labeled by a from v, set alph[a] to true
            if (g->edges[v][a] != UINT_MAX && sccs->numcl[g->edges[v][a]] == scc)
            {
                alph[a] = true;
                ret = true;
            }
        }
    }
    return ret;
}

// Merges two alphabets into a new one and tests simultanoeously if these alphabets were equal
static bool merge_subalph(bool *m, bool *s1, bool *s2, uint sizea, int *count)
{
    *count = 0;
    bool res = true;
    for (uint a = 0; a < sizea; a++)
    {
        if (s1[a] != s2[a])
        {
            res = false;
        }
        else
        {
            (*count)++;
        }
        m[a] = s1[a] && s2[a];
    }
    return res;
}

static void restricted_loop_alph(dgraph *g, uint q, bool *allowed, bool *alph)
{

    // We compute the strongly connected components of the graph restricted to the allowed letters
    parti *sccs = dtarjan(g, allowed, false);

    // Initialize the alphabet to false
    for (uint a = 0; a < g->size_alpha; a++)
    {
        alph[a] = false;
    }

    // For each label
    for (uint a = 0; a < g->size_alpha; a++)
    {
        if (!allowed[a])
        {
            continue;
        }
        for (uint i = 0; i < sccs->cl_size[sccs->numcl[q]]; i++)
        {
            uint v = sccs->cl_elems[sccs->numcl[q]][i];
            // If there is an edge labeled by a from q, set alph[a] to true
            if (g->edges[v][a] != UINT_MAX && sccs->numcl[g->edges[v][a]] == sccs->numcl[q])
            {
                alph[a] = true;
            }
        }
    }
    delete_parti(sccs);
}

bool dgraph_common_alph_loop(dgraph *g, parti *sccs, uint *inv_sccs, uint q1, uint q2, bool *alph)
{

    // Compute the maximum loop alphabet for each state
    bool alph1[g->size_alpha];
    bool alph2[g->size_alpha];
    dgraph_compute_alph_scc(g, sccs, sccs->numcl[q1], alph1);
    dgraph_compute_alph_scc(g, sccs, sccs->numcl[q2], alph2);

    // We extract the sccs of the two states (useful for quickly recopmputing the sccs for restricted alphabets).
    dgraph *ge1 = dgraph_extract(g, sccs, inv_sccs, sccs->numcl[q1]);
    dgraph *ge2 = dgraph_extract(g, sccs, inv_sccs, sccs->numcl[q2]);
    uint r1 = inv_sccs[q1];
    uint r2 = inv_sccs[q2];

    int count;
    // While the two alphabets are not equal, we make the intersection of the two.
    while (!merge_subalph(alph, alph1, alph2, g->size_alpha, &count))
    {
        // We compute the maximum loop alphabet for each state over the restricted alphabet.
        restricted_loop_alph(ge1, r1, alph, alph1);
        restricted_loop_alph(ge2, r2, alph, alph2);
    }
    delete_dgraph(ge1);
    delete_dgraph(ge2);
    return (count > 0); // Return true if the alphabet is not empty, false otherwise.
}

void dgraph_discard_nonscc_edges(dgraph *g, parti *sccs)
{
    for (uint q = 0; q < g->size_graph; q++)
    {
        for (uint a = 0; a < g->size_alpha; a++)
        {
            uint r = g->edges[q][a];
            if (r != UINT_MAX && sccs->numcl[r] != sccs->numcl[q])
            {
                g->edges[q][a] = UINT_MAX;
            }
        }
    }
}

dgraph *dgraph_copy_discard_nonscc_edges(dgraph *g, parti *sccs)
{
    dgraph *res = create_dgraph_noedges(g->size_graph, g->size_alpha);

    for (uint i = 0; i < g->size_graph; i++)
    {
        for (uint a = 0; a < g->size_alpha; a++)
        {
            if (g->edges[i][a] != UINT_MAX && sccs->numcl[g->edges[i][a]] == sccs->numcl[i])
            {
                // If the edge goes to the same SCC, copy it
                res->edges[i][a] = g->edges[i][a];
                res->size_edges++;
            }
            else
            {
                // Otherwise, no transition
                res->edges[i][a] = UINT_MAX;
            }
        }
    }
    return res;
}

bool dgraph_ntrivial_loop(dgraph *g, parti *sccs, uint q)
{
    if (sccs->cl_size[sccs->numcl[q]] > 1)
    {
        return true;
    }
    for (uint a = 0; a < g->size_alpha; a++)
    {
        if (g->edges[q][a] == q)
        {
            return true;
        }
    }
    return false;
}

bool dgraph_ntrivial_scc(dgraph *g, parti *sccs, uint i)
{
    if (sccs->cl_size[i] > 1)
    {
        return true;
    }
    uint q = sccs->cl_elems[i][0];
    for (uint a = 0; a < g->size_alpha; a++)
    {
        if (g->edges[q][a] == q)
        {
            return true;
        }
    }
    return false;
}
