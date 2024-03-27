/************************/
/* Algorithme de Tarjan */
/************************/

#include "graphs_tarjan.h"

/*******************************/
/* Manipulation des partitions */
/*******************************/

// Fonction de désallocation
// Fonction de désallocation
void delete_parti(p_parti p)
{
    if (p == NULL)
    {
        return;
    }
    for (uint c = 0; c < p->size_par; c++)
    {
        delete_vertices(p->cl[c]);
    }
    free(p->cl);
    free(p->numcl);
    free(p);
}

// Conversion en Union-Find
p_ufind parti_to_ufind(p_parti p)
{
    p_ufind uf = create_ufind(p->size_set);
    for (uint c = 0; c < p->size_par; c++)
    {
        for (uint i = 1; i < size_vertices(p->cl[c]); i++)
        {
            union_ufind(lefread_vertices(p->cl[c], 0), lefread_vertices(p->cl[c], i), uf);
        }
    }
    return uf;
}

// Construction depuis Union-Find
p_parti ufind_to_parti(p_ufind uf)
{
    // Initialisation de la partition
    p_parti new;
    MALLOC(new, 1);
    new->size_par = uf->size_par;
    new->size_set = uf->size_set;
    MALLOC(new->cl, new->size_par);
    for (uint c = 0; c < new->size_par; c++)
    {
        new->cl[c] = create_vertices();
    }
    MALLOC(new->numcl, new->size_set);

    // Tableau qui mémorise les éléments déjà traités
    bool doelems[new->size_set];
    for (uint e = 0; e < uf->size_set; e++)
    {
        doelems[e] = false;
    }

    // Prochain numéro de classe
    uint num_class = 0;

    // Calcul des classes
    for (uint e = 0; e < uf->size_set; e++)
    {
        uint r = find_ufind(e, uf);
        if (doelems[r] == false)
        {
            new->numcl[r] = num_class;
            num_class++;
            doelems[r]    = true;
        }

        uint c = new->numcl[r];
        new->numcl[e] = c;
        rigins_vertices(e, new->cl[c]);
    }
    return new;
}

p_parti ufind_to_parti_refined(p_ufind uf, p_parti P)
{
    // Initialisation de la partition
    p_parti new;
    MALLOC(new, 1);
    new->size_par = uf->size_par;
    new->size_set = uf->size_set;
    MALLOC(new->cl, new->size_par);
    for (uint c = 0; c < new->size_par; c++)
    {
        new->cl[c] = create_vertices();
    }
    MALLOC(new->numcl, new->size_set);

    // Tableau qui mémorise les éléments déjà traités
    bool doelems[new->size_set];
    for (uint e = 0; e < uf->size_set; e++)
    {
        doelems[e] = false;
    }

    // Prochain numéro de classe
    uint num_class = 0;

    // Calcul des classes
    for (uint d = 0; d < P->size_par; d++)
    {
        for (uint i = 0; i < size_vertices(P->cl[d]); i++)
        {
            uint e = lefread_vertices(P->cl[d], i);

            uint r = find_ufind(e, uf);
            if (doelems[r] == false)
            {
                new->numcl[r] = num_class;
                num_class++;
                doelems[r]    = true;
            }

            uint c = new->numcl[r];
            new->numcl[e] = c;
            rigins_vertices(e, new->cl[c]);
        }
    }

    return new;
}

// Teste si la partition est triviale (les classes sont toutes des singletons)
bool istrivial_parti(p_parti p)
{
    for (uint c = 0; c < p->size_par; c++)
    {
        if (size_vertices(p->cl[c]) != 1)
        {
            return false;
        }
    }
    return true;
}

/*******************************************/
/* Algorithme de Tarjan: version classique */
/*******************************************/

void scc(p_graph g,
         uint v,        // sommet courant
         int *index,    // tableau des indices
         int *id,       // id courant (prochain indice à donner)
         int *lowlink,  // tableau des lowlinks (min indice atteignable depuis
                        // v)
         int *thestack, // pile de sommets (la scc courante est en haut de pile)
         int *top,      // taille de la pile
         bool *onStack, // tableau de booléens indiquant les sommets dansla pile
                        // ufind **result, // structure union-find pour stocker
                        // les scc
         uint *ncount,  // Entier pour numéroter les partitions trouvées
         uint *nums     // Tableau mémorisant le numéro de classe de chaque
                        // élément
         )
{
    index[v]           = lowlink[v] = (*id)++;
    thestack[(*top)++] = v;
    onStack[v]         = true;

    uint w;

    for (uint c = 0; c < size_vertices(g->edges[v]); c++)
    {
        w = lefread_vertices(g->edges[v], c);
        if (index[w] == -1)  // w n'a pas encore été visité
        {
            scc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
            lowlink[v] = min(lowlink[v], lowlink[w]);
        }
        else if (onStack[w]) // w est dans la pile
        {
            lowlink[v] = min(lowlink[v], index[w]);
        }
    }
    // for (uint c = 0; c < g->edges[v]->size_array; c++)
    // {
    // w = g->edges[v]->array[c];
    // if (index[w] == -1) // w n'a pas encore été visité
    // {
    // scc(g, w, index, id, lowlink, thestack, top, onStack, result);
    // lowlink[v] = min(lowlink[v], lowlink[w]);
    // }
    // else if (onStack[w]) // w est dans la pile
    // {
    // lowlink[v] = min(lowlink[v], index[w]);
    // }
    // }

    if (lowlink[v] == index[v]) // v est la racine de sa scc
    {
        nums[v] = *ncount;
        // printf("Discovered a SCC starting at %d\n", v);
        while (*top > 0)
        {
            w          = thestack[--(*top)];
            onStack[w] = false;
            nums[w]    = *ncount;
            // union_ufind(v, w, *result);
            // printf("Union: %d %d\n", v, w);

            if (w == v)
            {
                break;
            }
        }
        (*ncount)++;
    }
}

void lscc(p_lgraph g,
          uint v,        // sommet courant
          int *index,    // tableau des indices
          int *id,       // id courant (prochain indice à donner)
          int *lowlink,  // tableau des lowlinks (min indice atteignable depuis
                         // v)
          int *thestack, // pile de sommets (la scc courante est en haut de
                         // pile)
          int *top,      // taille de la pile
          bool *onStack, // tableau de booléens indiquant les sommets dansla
                         // pile
          uint *ncount,  // Entier pour numéroter les partitions trouvées
          uint *nums     // Tableau mémorisant le numéro de classe de chaque
                         // élément
          )
{
    index[v]           = lowlink[v] = (*id)++;
    thestack[(*top)++] = v;
    onStack[v]         = true;

    uint w;

    // Parcours des transitions étiquetées par chaque lettre sortant du sommet v
    for (uint c = 0; c < g->size_alpha; c++)
    {
        for (uint k = 0; k < size_vertices(g->edges[v][c]); k++)
        {
            w = lefread_vertices(g->edges[v][c], k); // k-eme voisins de v
                                                     // étiqueté par c
            DEBUG("lscc(%d):\t c = %d, k = %d, w = %d", v, c, k, w);
            if (index[w] == -1)                      // w n'a pas encore été
                                                     // visité
            {
                lscc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
                lowlink[v] = min(lowlink[v], lowlink[w]);
            }
            else if (onStack[w])                     // w est dans la pile
            {
                lowlink[v] = min(lowlink[v], index[w]);
            }
        }
    }

    if (lowlink[v] == index[v])                      // v est la racine de sa
                                                     // scc
    {
        DEBUG("Discovered a SCC starting at %d", v);
        nums[v] = *ncount;
        while (*top > 0)
        {
            w          = thestack[--(*top)];
            onStack[w] = false;
            nums[w]    = *ncount;
            if (w == v)
            {
                break;
            }
        }
        (*ncount)++;
    }
}

void dscc(p_dgraph g,
          uint v,        // sommet courant
          int *index,    // tableau des indices
          int *id,       // id courant (prochain indice à donner)
          int *lowlink,  // tableau des lowlinks (min indice atteignable depuis
                         // v)
          int *thestack, // pile de sommets (la scc courante est en haut de
                         // pile)
          int *top,      // taille de la pile
          bool *onStack, // tableau de booléens indiquant les sommets dansla
                         // pile
          uint *ncount,  // Entier pour numéroter les partitions trouvées
          uint *nums     // Tableau mémorisant le numéro de classe de chaque
                         // élément
          )
{
    index[v]           = lowlink[v] = (*id)++;
    thestack[(*top)++] = v;
    onStack[v]         = true;

    uint w;

    // Parcours des transitions étiquetées par chaque lettre sortant du sommet v
    for (uint c = 0; c < g->size_alpha; c++)
    {
        w = g->edges[v][c];     // voisin de v étiqueté par c
        if (index[w] == -1)     // w n'a pas encore été visité
        {
            dscc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
            lowlink[v] = min(lowlink[v], lowlink[w]);
        }
        else if (onStack[w])    // w est dans la pile
        {
            lowlink[v] = min(lowlink[v], index[w]);
        }
    }

    if (lowlink[v] == index[v]) // v est la racine de sa scc
    {
        DEBUG("Discovered a SCC starting at %d", v);
        nums[v] = *ncount;
        while (*top > 0)
        {
            w          = thestack[--(*top)];
            onStack[w] = false;
            nums[w]    = *ncount;
            if (w == v)
            {
                break;
            }
        }
        (*ncount)++;
    }
}

p_parti tarjan(p_graph g)
{
    int *index, *lowlink, *thestack;
    MALLOC(index, g->size);
    MALLOC(lowlink, g->size);
    MALLOC(thestack, g->size);

    for (uint v = 0; v < g->size; v++)
    {
        lowlink[v] = index[v] = -1;
    }

    bool *onStack;
    CALLOC(onStack, g->size);

    int *id;
    CALLOC(id, 1);

    int top = 0;

    p_parti result;
    MALLOC(result, 1);
    MALLOC(result->numcl, g->size);
    result->size_par = 0;
    result->size_set = g->size;

    // ufind *result = create_ufind(g->size);

    for (uint v = 0; v < g->size; v++)
    {
        if (index[v] == -1)
        {
            scc(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
                result->numcl);
        }
    }

    // Création du tableau des classes
    MALLOC(result->cl, result->size_par);
    for (uint c = 0; c < result->size_par; c++)
    {
        result->cl[c] = create_vertices();
    }
    for (uint v = 0; v < g->size; v++)
    {
        result->numcl[v] = result->size_par - 1 - result->numcl[v];
        rigins_vertices(v, result->cl[result->numcl[v]]);
    }

    free(index);
    free(lowlink);
    free(thestack);
    free(onStack);
    free(id);

    return result;
}

p_parti ltarjan(p_lgraph g)
{
    int *index, *lowlink, *thestack;
    MALLOC(index, g->size_graph);
    MALLOC(lowlink, g->size_graph);
    MALLOC(thestack, g->size_graph);

    for (uint v = 0; v < g->size_graph; v++)
    {
        lowlink[v] = index[v] = -1;
    }

    bool *onStack;
    CALLOC(onStack, g->size_graph);

    int *id;
    CALLOC(id, 1);

    int top = 0;

    p_parti result;
    MALLOC(result, 1);
    MALLOC(result->numcl, g->size_graph);
    result->size_par = 0;
    result->size_set = g->size_graph;

    for (uint v = 0; v < g->size_graph; v++)
    {
        if (index[v] == -1)
        {
            lscc(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
                 result->numcl);
        }
    }

    // Création du tableau des classes
    MALLOC(result->cl, result->size_par);
    for (uint c = 0; c < result->size_par; c++)
    {
        result->cl[c] = create_vertices();
    }
    for (uint v = 0; v < g->size_graph; v++)
    {
        result->numcl[v] = result->size_par - 1 - result->numcl[v];
        rigins_vertices(v, result->cl[result->numcl[v]]);
    }

    free(index);
    free(lowlink);
    free(thestack);
    free(onStack);
    free(id);

    return result;
}

p_parti dtarjan(p_dgraph g)
{
    int *index, *lowlink, *thestack;
    MALLOC(index, g->size_graph);
    MALLOC(lowlink, g->size_graph);
    MALLOC(thestack, g->size_graph);

    for (uint v = 0; v < g->size_graph; v++)
    {
        lowlink[v] = index[v] = -1;
    }

    bool *onStack;
    CALLOC(onStack, g->size_graph);

    int *id;
    CALLOC(id, 1);

    int top = 0;

    p_parti result;
    MALLOC(result, 1);
    MALLOC(result->numcl, g->size_graph);
    result->size_par = 0;
    result->size_set = g->size_graph;

    for (uint v = 0; v < g->size_graph; v++)
    {
        if (index[v] == -1)
        {
            dscc(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
                 result->numcl);
        }
    }

    // Création du tableau des classes
    MALLOC(result->cl, result->size_par);
    for (uint c = 0; c < result->size_par; c++)
    {
        result->cl[c] = create_vertices();
    }
    for (uint v = 0; v < g->size_graph; v++)
    {
        result->numcl[v] = result->size_par - 1 - result->numcl[v];
        rigins_vertices(v, result->cl[result->numcl[v]]);
    }

    free(index);
    free(lowlink);
    free(thestack);
    free(onStack);
    free(id);

    return result;
}

/***************************************************************/
/* Algorithme de Tarjan: version restreinte à un sous-alphabet */
/***************************************************************/

void lscc_alph(p_lgraph g,
               uint v,        // sommet courant
               int *index,    // tableau des indices
               int *id,       // id courant (prochain indice à donner)
               int *lowlink,  // tableau des lowlinks (min indice atteignable
                              // depuis v)
               int *thestack, // pile de sommets (la scc courante est en haut de
                              // pile)
               int *top,      // taille de la pile
               bool *onStack, // tableau de booléens indiquant les sommets
                              // dansla pile
               uint *ncount,  // Entier pour numéroter les partitions trouvées
               uint *nums,    // Tableau mémorisant le numéro de classe de
                              // chaque élément
               bool *alph)
{
    index[v]           = lowlink[v] = (*id)++;
    thestack[(*top)++] = v;
    onStack[v]         = true;

    uint w;

    // Parcours des transitions étiquetées par chaque lettre sortant du sommet v
    for (uint c = 0; c < g->size_alpha; c++)
    {
        if (alph[c])
        {
            for (uint k = 0; k < size_vertices(g->edges[v][c]); k++)
            {
                w = lefread_vertices(g->edges[v][c], k); // k-eme voisins de v
                                                         // étiqueté par c
                DEBUG("lscc(%d):\t c = %d, k = %d, w = %d", v, c, k, w);
                if (index[w] == -1)                      // w n'a pas encore été
                                                         // visité
                {
                    lscc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
                    lowlink[v] = min(lowlink[v], lowlink[w]);
                }
                else if (onStack[w])                     // w est dans la pile
                {
                    lowlink[v] = min(lowlink[v], index[w]);
                }
            }
        }
    }

    if (lowlink[v] == index[v])                          // v est la racine de
                                                         // sa scc
    {
        DEBUG("Discovered a SCC starting at %d", v);
        nums[v] = *ncount;
        while (*top > 0)
        {
            w          = thestack[--(*top)];
            onStack[w] = false;
            nums[w]    = *ncount;
            if (w == v)
            {
                break;
            }
        }
        (*ncount)++;
    }
}

void dscc_alph(p_dgraph g,
               uint v,        // sommet courant
               int *index,    // tableau des indices
               int *id,       // id courant (prochain indice à donner)
               int *lowlink,  // tableau des lowlinks (min indice atteignable
                              // depuis v)
               int *thestack, // pile de sommets (la scc courante est en haut de
                              // pile)
               int *top,      // taille de la pile
               bool *onStack, // tableau de booléens indiquant les sommets
                              // dansla pile
               uint *ncount,  // Entier pour numéroter les partitions trouvées
               uint *nums,    // Tableau mémorisant le numéro de classe de
                              // chaque élément
               bool *alph)
{
    index[v]           = lowlink[v] = (*id)++;
    thestack[(*top)++] = v;
    onStack[v]         = true;

    uint w;

    // Parcours des transitions étiquetées par chaque lettre sortant du sommet v
    for (uint c = 0; c < g->size_alpha; c++)
    {
        if (alph[c])
        {
            w = g->edges[v][c];  // voisin de v étiqueté par c
            if (index[w] == -1)  // w n'a pas encore été visité
            {
                dscc(g, w, index, id, lowlink, thestack, top, onStack, ncount, nums);
                lowlink[v] = min(lowlink[v], lowlink[w]);
            }
            else if (onStack[w]) // w est dans la pile
            {
                lowlink[v] = min(lowlink[v], index[w]);
            }
        }
    }

    if (lowlink[v] == index[v])  // v est la racine de sa scc
    {
        DEBUG("Discovered a SCC starting at %d", v);
        nums[v] = *ncount;
        while (*top > 0)
        {
            w          = thestack[--(*top)];
            onStack[w] = false;
            nums[w]    = *ncount;
            if (w == v)
            {
                break;
            }
        }
        (*ncount)++;
    }
}

// Les sccs sont ordonnées selon un tri topologique du DAG des sccs
p_parti ltarjan_alph(p_lgraph g, bool *alph)
{
    int *index, *lowlink, *thestack;
    MALLOC(index, g->size_graph);
    MALLOC(lowlink, g->size_graph);
    MALLOC(thestack, g->size_graph);

    for (uint v = 0; v < g->size_graph; v++)
    {
        lowlink[v] = index[v] = -1;
    }

    bool *onStack;
    CALLOC(onStack, g->size_graph);

    int *id;
    CALLOC(id, 1);

    int top = 0;

    p_parti result;
    MALLOC(result, 1);
    MALLOC(result->numcl, g->size_graph);
    result->size_par = 0;
    result->size_set = g->size_graph;

    for (uint v = 0; v < g->size_graph; v++)
    {
        if (index[v] == -1)
        {
            lscc_alph(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
                      result->numcl, alph);
        }
    }

    // Création du tableau des classes
    MALLOC(result->cl, result->size_par);
    for (uint c = 0; c < result->size_par; c++)
    {
        result->cl[c] = create_vertices();
    }
    for (uint v = 0; v < g->size_graph; v++)
    {
        result->numcl[v] = result->size_par - 1 - result->numcl[v];
        rigins_vertices(v, result->cl[result->numcl[v]]);
    }

    free(index);
    free(lowlink);
    free(thestack);
    free(onStack);
    free(id);

    return result;
}

p_parti dtarjan_alph(p_dgraph g, bool *alph)
{
    int *index, *lowlink, *thestack;
    MALLOC(index, g->size_graph);
    MALLOC(lowlink, g->size_graph);
    MALLOC(thestack, g->size_graph);

    for (uint v = 0; v < g->size_graph; v++)
    {
        lowlink[v] = index[v] = -1;
    }

    bool *onStack;
    CALLOC(onStack, g->size_graph);

    int *id;
    CALLOC(id, 1);

    int top = 0;

    p_parti result;
    MALLOC(result, 1);
    MALLOC(result->numcl, g->size_graph);
    result->size_par = 0;
    result->size_set = g->size_graph;

    for (uint v = 0; v < g->size_graph; v++)
    {
        if (index[v] == -1)
        {
            dscc_alph(g, v, index, id, lowlink, thestack, &top, onStack, &result->size_par,
                      result->numcl, alph);
        }
    }

    // Création du tableau des classes
    MALLOC(result->cl, result->size_par);
    for (uint c = 0; c < result->size_par; c++)
    {
        result->cl[c] = create_vertices();
    }
    for (uint v = 0; v < g->size_graph; v++)
    {
        result->numcl[v] = result->size_par - 1 - result->numcl[v];
        rigins_vertices(v, result->cl[result->numcl[v]]);
    }

    free(index);
    free(lowlink);
    free(thestack);
    free(onStack);
    free(id);

    return result;
}
