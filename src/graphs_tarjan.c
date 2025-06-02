

#include "graphs_tarjan.h"
#include <limits.h>
#include <time.h>

parti* tarjan(graph* g)
{
    // Array storing the class number of each vertex.
    uint* numcl;
    MALLOC(numcl, g->size);

    // Future size of the partition.
    uint size_par = 0;

    // Array storing the visiting order of the vertices
    uint* index;
    CALLOC(index, g->size);

    // Current index
    uint id = 1;

    // Array storing the lowlink of each vertex
    uint* lowlink;
    CALLOC(lowlink, g->size);

    // Stack of SCCs
    uint* SCCstack;
    CALLOC(SCCstack, g->size);

    // Top of the stack
    uint top = 0;

    // Flags indicating whether a vertex is in the stack of SCCs
    bool* onStack;
    CALLOC(onStack, g->size);


    // Stack for the DFS
    dequeue* thestack = create_dequeue();

    // Number of outgoing edges already handled in the DFS for each vertex
    uint* iout;
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
        while (!isempty_dequeue(thestack)) {
            // Get the next vertex to process (without popping it from the stack yet, only done after all its edges have been processed).
            uint v = rigread_dequeue(thestack, 0);

            // If there are still edges to process for the vertex, we process the next one
            if (iout[v] < size_dequeue(g->edges[v]))
            {
                uint w = lefread_dequeue(g->edges[v], iout[v]);
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
                for (uint i = 0; i < size_dequeue(g->edges[v]); i++)
                {
                    uint w = lefread_dequeue(g->edges[v], i);
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


parti* ltarjan(lgraph* g, bool* alph)
{
    // Array storing the class number of each vertex.
    uint* numcl;
    MALLOC(numcl, g->size_graph);

    // Future size of the partition.
    uint size_par = 0;

    // Array storing the visiting order of the vertices
    uint* index;
    CALLOC(index, g->size_graph);

    // Current index
    uint id = 1;

    // Array storing the lowlink of each vertex
    uint* lowlink;
    CALLOC(lowlink, g->size_graph);

    // Stack of SCCs
    uint* SCCstack;
    CALLOC(SCCstack, g->size_graph);

    // Top of the stack
    uint top = 0;

    // Flags indicating whether a vertex is in the stack of SCCs
    bool* onStack;
    CALLOC(onStack, g->size_graph);


    // Stack for the DFS
    dequeue* thestack = create_dequeue();

    // Number of letters already handled in the DFS for each vertex
    uint* aout;
    CALLOC(aout, g->size_graph);

    // Number of outgoing edges already handled for the current letter in the DFS for each vertex
    uint* iout;
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
        while (!isempty_dequeue(thestack)) {
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



            // If there are still edges to process for the vertex and the current label, we process the next one
            if (aout[v] < g->size_alpha && iout[v] < size_dequeue(g->edges[v][aout[v]]))
            {
                uint w = lefread_dequeue(g->edges[v][aout[v]], iout[v]);
                if (index[w] == 0)
                {
                    index[w] = id++;
                    lowlink[w] = index[w];
                    SCCstack[top++] = w;
                    onStack[w] = true;
                    rigins_dequeue(w, thestack);
                }
                iout[v]++;
            } // If there are still labels to process, we move to the next label
            else if (aout[v] < g->size_alpha - 1)
            {
                aout[v]++;
                iout[v] = 0;
            }
            else
            {
                // If all edges have been processed, we pop the vertex from the stack
                rigpull_dequeue(thestack);

                // We update the lowlink of the vertex
                for (uint a = 0; a < g->size_alpha; a++)
                {
                    for (uint i = 0; i < size_dequeue(g->edges[v][a]); i++)
                    {
                        uint w = lefread_dequeue(g->edges[v][a], i);
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


parti* dtarjan(dgraph* g, bool* alph, bool ismor)
{
#ifdef DEBUG_TARJAN
    printf("Starting Tarjan's algorithm (dtarjan) on a graph with %u vertices and %u labels.\n", g->size_graph, g->size_alpha);
#endif

    uint* numcl;
    MALLOC(numcl, g->size_graph);
    uint size_par = 0;

    // Array storing the visiting order of the vertices
    uint* index;
    CALLOC(index, g->size_graph);

    // Current index
    uint id = 1;

    // Array storing the lowlink of each vertex
    uint* lowlink;
    CALLOC(lowlink, g->size_graph);

    // Stack of SCCs
    uint* SCCstack;
    CALLOC(SCCstack, g->size_graph);

    // Top of the stack
    uint top = 0;

    // Flags indicating whether a vertex is in the stack of SCCs
    bool* onStack;
    CALLOC(onStack, g->size_graph);


    // Stack for the DFS
    dequeue* thestack = create_dequeue();

    // Number of labels already handled in the DFS for each vertex
    uint* aout;
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
        while (!isempty_dequeue(thestack)) {
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
                if (index[w] == 0)
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
                    if (onStack[g->edges[v][a]])
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




parti* dualdtarjan(dgraph* g1, dgraph* g2, bool* alph, bool ismor)
{
    if (g1->size_graph != g2->size_graph || g1->size_alpha != g2->size_alpha)
    {
        fprintf(stderr, "Error in dualdtarjan: The two graphs must have the same size and number of labels.\n");
        exit(EXIT_FAILURE);
    }


    uint* numcl;
    MALLOC(numcl, g1->size_graph);
    uint size_par = 0;

    // Array storing the visiting order of the vertices
    uint* index;
    CALLOC(index, g1->size_graph);

    // Current index
    uint id = 1;

    // Array storing the lowlink of each vertex
    uint* lowlink;
    CALLOC(lowlink, g1->size_graph);

    // Stack of SCCs
    uint* SCCstack;
    CALLOC(SCCstack, g1->size_graph);

    // Top of the stack
    uint top = 0;

    // Flags indicating whether a vertex is in the stack of SCCs
    bool* onStack;
    CALLOC(onStack, g1->size_graph);


    // Stack for the DFS
    dequeue* thestack = create_dequeue();

    // Number of labels already handled in the DFS for each vertex
    uint* aout1;
    CALLOC(aout1, g1->size_graph);
    uint* aout2;
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
        while (!isempty_dequeue(thestack)) {
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
                    if (!alph || alph[a]) {
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
        //printf("numcl[%u] = %u\n", i, numcl[i]);
        numcl[i] = size_par - 1 - numcl[i];
    }
    return create_parti(g1->size_graph, size_par, numcl);
}



