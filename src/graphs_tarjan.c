

#include "graphs_tarjan.h"
#include <limits.h>





// static void scc(graph* g,
//     uint v,        // sommet courant
//     int* index,    // tableau des indices
//     int* id,       // id courant (prochain indice à donner)
//     int* lowlink,  // tableau des lowlinks (min indice atteignable depuis
//     // v)
//     int* thestack, // pile de sommets (la scc courante est en haut de pile)
//     int* top,      // taille de la pile
//     bool* onStack, // tableau de booléens indiquant les sommets dansla pile
//     // ufind **result, // structure union-find pour stocker
//     // les scc
//     uint* ncount,  // Entier pour numéroter les partitions trouvées
//     uint* nums     // Tableau mémorisant le numéro de classe de chaque
//     // élément
// )
// {
//     index[v] = lowlink[v] = (*id)++;
//     thestack[(*top)++] = v;
//     onStack[v] = true;

//     uint w;

//     for (uint c = 0; c < size_dequeue(g->edges[v]); c++)
//     {
//         w = lefread_dequeue(g->edges[v], c);
//         if (index[w] == -1)  // w n'a pas encore été visité
//         {
//             scc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
//             lowlink[v] = min(lowlink[v], lowlink[w]);
//         }
//         else if (onStack[w]) // w est dans la pile
//         {
//             lowlink[v] = min(lowlink[v], index[w]);
//         }
//     }
//     // for (uint c = 0; c < g->edges[v]->size_array; c++)
//     // {
//     // w = g->edges[v]->array[c];
//     // if (index[w] == -1) // w n'a pas encore été visité
//     // {
//     // scc(g, w, index, id, lowlink, thestack, top, onStack, result);
//     // lowlink[v] = min(lowlink[v], lowlink[w]);
//     // }
//     // else if (onStack[w]) // w est dans la pile
//     // {
//     // lowlink[v] = min(lowlink[v], index[w]);
//     // }
//     // }

//     if (lowlink[v] == index[v]) // v est la racine de sa scc
//     {
//         nums[v] = *ncount;
//         // printf("Discovered a SCC starting at %d\n", v);
//         while (*top > 0)
//         {
//             w = thestack[--(*top)];
//             onStack[w] = false;
//             nums[w] = *ncount;
//             // union_ufind(v, w, *result);
//             // printf("Union: %d %d\n", v, w);

//             if (w == v)
//             {
//                 break;
//             }
//         }
//         (*ncount)++;
//     }
// }

// static void lscc(lgraph* g,
//     uint v,        // sommet courant
//     int* index,    // tableau des indices
//     int* id,       // id courant (prochain indice à donner)
//     int* lowlink,  // tableau des lowlinks (min indice atteignable depuis
//     // v)
//     int* thestack, // pile de sommets (la scc courante est en haut de
//     // pile)
//     int* top,      // taille de la pile
//     bool* onStack, // tableau de booléens indiquant les sommets dansla
//     // pile
//     uint* ncount,  // Entier pour numéroter les partitions trouvées
//     uint* nums     // Tableau mémorisant le numéro de classe de chaque
//     // élément
// )
// {
//     index[v] = lowlink[v] = (*id)++;
//     thestack[(*top)++] = v;
//     onStack[v] = true;

//     uint w;

//     // Parcours des transitions étiquetées par chaque lettre sortant du sommet v
//     for (uint c = 0; c < g->size_alpha; c++)
//     {
//         for (uint k = 0; k < size_dequeue(g->edges[v][c]); k++)
//         {
//             w = lefread_dequeue(g->edges[v][c], k); // k-eme voisins de v
//             // étiqueté par c
//             DEBUG("lscc(%d):\t c = %d, k = %d, w = %d", v, c, k, w);
//             if (index[w] == -1)                      // w n'a pas encore été
//                 // visité
//             {
//                 lscc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
//                 lowlink[v] = min(lowlink[v], lowlink[w]);
//             }
//             else if (onStack[w])                     // w est dans la pile
//             {
//                 lowlink[v] = min(lowlink[v], index[w]);
//             }
//         }
//     }

//     if (lowlink[v] == index[v])                      // v est la racine de sa
//         // scc
//     {
//         DEBUG("Discovered a SCC starting at %d", v);
//         nums[v] = *ncount;
//         while (*top > 0)
//         {
//             w = thestack[--(*top)];
//             onStack[w] = false;
//             nums[w] = *ncount;
//             if (w == v)
//             {
//                 break;
//             }
//         }
//         (*ncount)++;
//     }
// }

// static void dscc(dgraph* g,
//     uint v,        // sommet courant
//     int* index,    // tableau des indices
//     int* id,       // id courant (prochain indice à donner)
//     int* lowlink,  // tableau des lowlinks (min indice atteignable depuis
//     // v)
//     int* thestack, // pile de sommets (la scc courante est en haut de
//     // pile)
//     int* top,      // taille de la pile
//     bool* onStack, // tableau de booléens indiquant les sommets dansla
//     // pile
//     uint* ncount,  // Entier pour numéroter les partitions trouvées
//     uint* nums     // Tableau mémorisant le numéro de classe de chaque
//     // élément
// )
// {
//     index[v] = lowlink[v] = (*id)++;
//     thestack[(*top)++] = v;
//     onStack[v] = true;

//     uint w;

//     // Parcours des transitions étiquetées par chaque lettre sortant du sommet v
//     for (uint c = 0; c < g->size_alpha; c++)
//     {
//         w = g->edges[v][c];     // voisin de v étiqueté par c
//         if (index[w] == -1)     // w n'a pas encore été visité
//         {
//             dscc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
//             lowlink[v] = min(lowlink[v], lowlink[w]);
//         }
//         else if (onStack[w])    // w est dans la pile
//         {
//             lowlink[v] = min(lowlink[v], index[w]);
//         }
//     }

//     if (lowlink[v] == index[v]) // v est la racine de sa scc
//     {
//         DEBUG("Discovered a SCC starting at %d", v);
//         nums[v] = *ncount;
//         while (*top > 0)
//         {
//             w = thestack[--(*top)];
//             onStack[w] = false;
//             nums[w] = *ncount;
//             if (w == v)
//             {
//                 break;
//             }
//         }
//         (*ncount)++;
//     }
// }

// parti* tarjan(graph* g)
// {
//     int* index, * lowlink, * thestack;
//     MALLOC(index, g->size);
//     MALLOC(lowlink, g->size);
//     MALLOC(thestack, g->size);

//     for (uint v = 0; v < g->size; v++)
//     {
//         lowlink[v] = index[v] = -1;
//     }

//     bool* onStack;
//     CALLOC(onStack, g->size);

//     int* id;
//     CALLOC(id, 1);

//     int top = 0;

//     parti* result;
//     MALLOC(result, 1);
//     MALLOC(result->numcl, g->size);
//     result->size_par = 0;
//     result->size_set = g->size;

//     // ufind *result = create_ufind(g->size);

//     for (uint v = 0; v < g->size; v++)
//     {
//         if (index[v] == -1)
//         {
//             scc(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
//                 result->numcl);
//         }
//     }

//     // Création du tableau des classes
//     MALLOC(result->cl, result->size_par);
//     for (uint c = 0; c < result->size_par; c++)
//     {
//         result->cl[c] = create_dequeue();
//     }
//     for (uint v = 0; v < g->size; v++)
//     {
//         result->numcl[v] = result->size_par - 1 - result->numcl[v];
//         rigins_dequeue(v, result->cl[result->numcl[v]]);
//     }

//     free(index);
//     free(lowlink);
//     free(thestack);
//     free(onStack);
//     free(id);

//     return result;
// }



parti* tarjan(graph* g)
{

    // Initialization of the partition
    parti* result;
    MALLOC(result, 1);
    MALLOC(result->numcl, g->size);
    result->size_par = 0;
    result->size_set = g->size;

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
            // Get the next vertex to process (without popping it from the stack yet)
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
                    //printf("Pushed %d onto the stack for index %d\n", w, index[w]);
                }
                // else if (onStack[w])
                // {
                //     lowlink[v] = min(lowlink[v], index[w]);
                // }
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

                // printf("Popped %d from the stack index: %d, lowlink: %d\n", v, index[v], lowlink[v]);

                // If we have found a SCC, we store it in the result by popping vertices from the stack of SSCs
                if (lowlink[v] == index[v])
                {
                    //printf("Discovered a SCC starting at %d\n", v);
                    while (top > 0)
                    {
                        uint w = SCCstack[--top];
                        onStack[w] = false;
                        result->numcl[w] = result->size_par;
                        if (w == v)
                        {
                            break;
                        }
                    }
                    result->size_par++;
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

    // Creating the array of classes
    MALLOC(result->cl, result->size_par);
    for (uint c = 0; c < result->size_par; c++)
    {
        result->cl[c] = create_dequeue();
    }
    for (uint v = 0; v < g->size; v++)
    {
        result->numcl[v] = result->size_par - 1 - result->numcl[v];
        rigins_dequeue(v, result->cl[result->numcl[v]]);
    }
    return result;
}






// parti* ltarjan(lgraph* g)
// {
//     int* index, * lowlink, * thestack;
//     MALLOC(index, g->size_graph);
//     MALLOC(lowlink, g->size_graph);
//     MALLOC(thestack, g->size_graph);


//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         lowlink[v] = index[v] = -1;
//     }

//     bool* onStack;
//     CALLOC(onStack, g->size_graph);

//     int* id;
//     CALLOC(id, 1);

//     int top = 0;

//     parti* result;
//     MALLOC(result, 1);
//     MALLOC(result->numcl, g->size_graph);
//     result->size_par = 0;
//     result->size_set = g->size_graph;


//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         if (index[v] == -1)
//         {
//             lscc(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
//                 result->numcl);
//         }
//     }


//     // Création du tableau des classes
//     MALLOC(result->cl, result->size_par);
//     for (uint c = 0; c < result->size_par; c++)
//     {
//         result->cl[c] = create_dequeue();
//     }
//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         result->numcl[v] = result->size_par - 1 - result->numcl[v];
//         rigins_dequeue(v, result->cl[result->numcl[v]]);
//     }


//     free(index);
//     free(lowlink);
//     free(thestack);
//     free(onStack);
//     free(id);


//     return result;
// }







parti* ltarjan(lgraph* g)
{
    graph* ug = lgraph_to_graph(g);
    parti* result = tarjan(ug);
    delete_graph(ug);
    return result;
}



// parti* dtarjan(dgraph* g)
// {
//     int* index, * lowlink, * thestack;
//     MALLOC(index, g->size_graph);
//     MALLOC(lowlink, g->size_graph);
//     MALLOC(thestack, g->size_graph);

//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         lowlink[v] = index[v] = -1;
//     }

//     bool* onStack;
//     CALLOC(onStack, g->size_graph);

//     int* id;
//     CALLOC(id, 1);

//     int top = 0;

//     parti* result;
//     MALLOC(result, 1);
//     MALLOC(result->numcl, g->size_graph);
//     result->size_par = 0;
//     result->size_set = g->size_graph;

//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         if (index[v] == -1)
//         {
//             dscc(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
//                 result->numcl);
//         }
//     }

//     // Création du tableau des classes
//     MALLOC(result->cl, result->size_par);
//     for (uint c = 0; c < result->size_par; c++)
//     {
//         result->cl[c] = create_dequeue();
//     }
//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         result->numcl[v] = result->size_par - 1 - result->numcl[v];
//         rigins_dequeue(v, result->cl[result->numcl[v]]);
//     }

//     free(index);
//     free(lowlink);
//     free(thestack);
//     free(onStack);
//     free(id);

//     return result;
// }


parti* dtarjan(dgraph* g)
{

    // Initialization of the partition
    parti* result;
    MALLOC(result, 1);
    MALLOC(result->numcl, g->size_graph);
    result->size_par = 0;
    result->size_set = g->size_graph;

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
                    //printf("Pushed %d onto the stack for index %d\n", w, index[w]);
                }
                // else if (onStack[w])
                // {
                //     lowlink[v] = min(lowlink[v], index[w]);
                // }
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

                // printf("Popped %d from the stack index: %d, lowlink: %d\n", v, index[v], lowlink[v]);

                // If we have found a SCC, we store it in the result by popping vertices from the stack of SSCs
                if (lowlink[v] == index[v])
                {



                    //printf("Discovered a SCC starting at %d\n", v);
                    while (top > 0)
                    {
                        uint w = SCCstack[--top];
                        onStack[w] = false;
                        result->numcl[w] = result->size_par;
                        if (w == v)
                        {
                            break;
                        }
                    }
                    result->size_par++;
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

    // Creating the array of classes
    MALLOC(result->cl, result->size_par);
    for (uint c = 0; c < result->size_par; c++)
    {
        result->cl[c] = create_dequeue();
    }
    for (uint v = 0; v < g->size_graph; v++)
    {
        result->numcl[v] = result->size_par - 1 - result->numcl[v];
        rigins_dequeue(v, result->cl[result->numcl[v]]);
    }
    return result;
}






/***************************************************************/
/* Algorithme de Tarjan: version restreinte à un sous-alphabet */
/***************************************************************/

// static void lscc_alph(lgraph* g,
//     uint v,        // sommet courant
//     int* index,    // tableau des indices
//     int* id,       // id courant (prochain indice à donner)
//     int* lowlink,  // tableau des lowlinks (min indice atteignable
//     // depuis v)
//     int* thestack, // pile de sommets (la scc courante est en haut de
//     // pile)
//     int* top,      // taille de la pile
//     bool* onStack, // tableau de booléens indiquant les sommets
//     // dansla pile
//     uint* ncount,  // Entier pour numéroter les partitions trouvées
//     uint* nums,    // Tableau mémorisant le numéro de classe de
//     // chaque élément
//     bool* alph)
// {
//     index[v] = lowlink[v] = (*id)++;
//     thestack[(*top)++] = v;
//     onStack[v] = true;

//     uint w;

//     // Parcours des transitions étiquetées par chaque lettre sortant du sommet v
//     for (uint c = 0; c < g->size_alpha; c++)
//     {
//         if (alph[c])
//         {
//             for (uint k = 0; k < size_dequeue(g->edges[v][c]); k++)
//             {
//                 w = lefread_dequeue(g->edges[v][c], k); // k-eme voisins de v
//                 // étiqueté par c
//                 DEBUG("lscc(%d):\t c = %d, k = %d, w = %d", v, c, k, w);
//                 if (index[w] == -1)                      // w n'a pas encore été
//                     // visité
//                 {
//                     lscc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
//                     lowlink[v] = min(lowlink[v], lowlink[w]);
//                 }
//                 else if (onStack[w])                     // w est dans la pile
//                 {
//                     lowlink[v] = min(lowlink[v], index[w]);
//                 }
//             }
//         }
//     }

//     if (lowlink[v] == index[v])                          // v est la racine de
//         // sa scc
//     {
//         DEBUG("Discovered a SCC starting at %d", v);
//         nums[v] = *ncount;
//         while (*top > 0)
//         {
//             w = thestack[--(*top)];
//             onStack[w] = false;
//             nums[w] = *ncount;
//             if (w == v)
//             {
//                 break;
//             }
//         }
//         (*ncount)++;
//     }
// }

// static void dscc_alph(dgraph* g,
//     uint v,        // sommet courant
//     int* index,    // tableau des indices
//     int* id,       // id courant (prochain indice à donner)
//     int* lowlink,  // tableau des lowlinks (min indice atteignable
//     // depuis v)
//     int* thestack, // pile de sommets (la scc courante est en haut de
//     // pile)
//     int* top,      // taille de la pile
//     bool* onStack, // tableau de booléens indiquant les sommets
//     // dansla pile
//     uint* ncount,  // Entier pour numéroter les partitions trouvées
//     uint* nums,    // Tableau mémorisant le numéro de classe de
//     // chaque élément
//     bool* alph)
// {
//     index[v] = lowlink[v] = (*id)++;
//     thestack[(*top)++] = v;
//     onStack[v] = true;

//     uint w;

//     // Parcours des transitions étiquetées par chaque lettre sortant du sommet v
//     for (uint c = 0; c < g->size_alpha; c++)
//     {
//         if (alph[c])
//         {
//             w = g->edges[v][c];  // voisin de v étiqueté par c
//             if (index[w] == -1)  // w n'a pas encore été visité
//             {
//                 dscc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
//                 lowlink[v] = min(lowlink[v], lowlink[w]);
//             }
//             else if (onStack[w]) // w est dans la pile
//             {
//                 lowlink[v] = min(lowlink[v], index[w]);
//             }
//         }
//     }

//     if (lowlink[v] == index[v])  // v est la racine de sa scc
//     {
//         DEBUG("Discovered a SCC starting at %d", v);
//         nums[v] = *ncount;
//         while (*top > 0)
//         {
//             w = thestack[--(*top)];
//             onStack[w] = false;
//             nums[w] = *ncount;
//             if (w == v)
//             {
//                 break;
//             }
//         }
//         (*ncount)++;
//     }
// }

parti* ltarjan_alph(lgraph* lg, bool* alph) {
    graph* g = lgraph_to_graph_alpha(lg, alph);
    parti* result = tarjan(g);
    delete_graph(g);
    return result;
}

// // Les sccs sont ordonnées selon un tri topologique du DAG des sccs
// parti* ltarjan_alph(lgraph* g, bool* alph)
// {
//     int* index, * lowlink, * thestack;
//     MALLOC(index, g->size_graph);
//     MALLOC(lowlink, g->size_graph);
//     MALLOC(thestack, g->size_graph);

//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         lowlink[v] = index[v] = -1;
//     }

//     bool* onStack;
//     CALLOC(onStack, g->size_graph);

//     int* id;
//     CALLOC(id, 1);

//     int top = 0;

//     parti* result;
//     MALLOC(result, 1);
//     MALLOC(result->numcl, g->size_graph);
//     result->size_par = 0;
//     result->size_set = g->size_graph;

//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         if (index[v] == -1)
//         {
//             lscc_alph(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
//                 result->numcl, alph);
//         }
//     }

//     // Création du tableau des classes
//     MALLOC(result->cl, result->size_par);
//     for (uint c = 0; c < result->size_par; c++)
//     {
//         result->cl[c] = create_dequeue();
//     }
//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         result->numcl[v] = result->size_par - 1 - result->numcl[v];
//         rigins_dequeue(v, result->cl[result->numcl[v]]);
//     }

//     free(index);
//     free(lowlink);
//     free(thestack);
//     free(onStack);
//     free(id);

//     return result;
// }


parti* dtarjan_alph(dgraph* dg, bool* alph)
{
    graph* g = dgraph_to_graph_alpha(dg, alph);
    parti* result = tarjan(g);
    delete_graph(g);
    return result;
}

// parti* dtarjan_alph(dgraph* g, bool* alph)
// {
//     int* index, * lowlink, * thestack;
//     MALLOC(index, g->size_graph);
//     MALLOC(lowlink, g->size_graph);
//     MALLOC(thestack, g->size_graph);

//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         lowlink[v] = index[v] = -1;
//     }

//     bool* onStack;
//     CALLOC(onStack, g->size_graph);

//     int* id;
//     CALLOC(id, 1);

//     int top = 0;

//     parti* result;
//     MALLOC(result, 1);
//     MALLOC(result->numcl, g->size_graph);
//     result->size_par = 0;
//     result->size_set = g->size_graph;

//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         if (index[v] == -1)
//         {
//             dscc_alph(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
//                 result->numcl, alph);
//         }
//     }

//     // Création du tableau des classes
//     MALLOC(result->cl, result->size_par);
//     for (uint c = 0; c < result->size_par; c++)
//     {
//         result->cl[c] = create_dequeue();
//     }
//     for (uint v = 0; v < g->size_graph; v++)
//     {
//         result->numcl[v] = result->size_par - 1 - result->numcl[v];
//         rigins_dequeue(v, result->cl[result->numcl[v]]);
//     }

//     free(index);
//     free(lowlink);
//     free(thestack);
//     free(onStack);
//     free(id);

//     return result;
// }
