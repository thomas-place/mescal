/********************************************/
/*   Structures pour stocker les machines   */
/* Ensuite spécialisées pour NFA,DFA,Cayley */
/********************************************/

#include "graphs.h"

/*********************/
/* Fonctions de test */
/*********************/

void graph_printing_test(p_graph G, FILE *out)
{
    p_stack theedges = graph_to_edges(G);

    for (uint i = 0; i < size_stack(theedges); i++)
    { // Boucle sur les états de départ
        fprintf(out, "\"%d\" -> \"%d\"\n", ((p_edge)read_stack(theedges, i))->in, ((p_edge)read_stack(theedges, i))->out);
    }
}

/*************************/
/* Fonctions de création */
/*************************/

// Intitialise un graphe sans arêtes
p_graph create_graph_noedges(uint size)
{
    // Création du graphe
    p_graph new;
    MALLOC(new, 1);

    // Définition du graph
    new->size = size;
    if (size != 0)
    {
        MALLOC(new->edges, size);
        for (uint q = 0; q < size; q++)
        {
            new->edges[q] = create_vertices();
        }
    }
    else
    {
        new->edges = NULL;
    }
    return new;
}

// Intitialise un graphe étiqueté sans arêtes
p_lgraph create_lgraph_noedges(uint size_graph, uint size_alpha)
{
    if (size_alpha == 0)
    {
        printf("Error, an alphabet should contain at least one symbol.\n Returned a NULL pointer.");
        return NULL;
    }
    // Création du graphe
    p_lgraph new;
    MALLOC(new, 1);

    // Définition du graphe
    new->size_alpha = size_alpha;
    new->size_graph = size_graph;
    if (size_graph != 0)
    {
        MALLOC(new->edges, size_graph);
        for (uint q = 0; q < size_graph; q++)
        {
            MALLOC(new->edges[q], size_alpha);
            for (uint a = 0; a < size_alpha; a++)
            {
                new->edges[q][a] = create_vertices();
            }
        }
    }
    else
    {
        new->edges = NULL;
    }
    return new;
}

// Intitialise un graphe déterministe complet étiqueté sans uniquement des self-loops
p_dgraph create_dgraph_noedges(uint size_graph, uint size_alpha)
{
    if (size_alpha == 0)
    {
        printf("Error, an alphabet should contain at least one symbol.\n Returned a NULL pointer.");
        return NULL;
    }
    // Création du graphe
    p_dgraph new;
    MALLOC(new, 1);

    // Définition du graphe
    new->size_alpha = size_alpha;
    new->size_graph = size_graph;
    if (size_graph != 0)
    {
        MALLOC(new->edges, size_graph);
        for (uint q = 0; q < size_graph; q++)
        {
            MALLOC(new->edges[q], size_alpha);
            for (uint a = 0; a < size_alpha; a++)
            {
                new->edges[q][a] = q;
            }
        }
    }
    else
    {
        new->edges = NULL;
    }
    return new;
}

/****************************/
/* Fonctions de suppression */
/****************************/

// Désallocation d'un graphe
void delete_graph(p_graph G)
{
    if (G == NULL || G->edges == NULL)
    {
        free(G);
        return;
    }
    for (uint q = 0; q < G->size; q++)
    {
        delete_vertices(G->edges[q]);
    }
    free(G->edges);
    free(G);
}

// Désallocation d'un graphe étiqueté
void delete_lgraph(p_lgraph G)
{
    if (G == NULL || G->edges == NULL)
    {
        free(G);
        return;
    }
    for (uint q = 0; q < G->size_graph; q++)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {
            delete_vertices(G->edges[q][a]);
        }
        free(G->edges[q]);
    }
    free(G->edges);
    free(G);
}

// Désallocation d'un graphe étiqueté déterministe complet
void delete_dgraph(p_dgraph G)
{
    if (G == NULL || G->edges == NULL)
    {
        free(G);
        return;
    }
    for (uint q = 0; q < G->size_graph; q++)
    {
        free(G->edges[q]);
    }
    free(G->edges);
    free(G);
}

/****************************/
/* Matrices d'accessibilité */
/****************************/

// Saturation d'une matrice, la diagonale est supposée contenir des true
void saturate_mat(bool **mat, uint size)
{
    unsigned char newmat[size][size];
    bool change = true;
    while (change)
    {
        for (uint i = 0; i < size; i++)
        {
            for (uint j = 0; j < size; j++)
            {
                newmat[i][j] = mat[i][0] && mat[0][j];
                for (uint k = 1; k < size; k++)
                {
                    newmat[i][j] = newmat[i][j] || (mat[i][k] && mat[k][j]);
                }
            }
        }
        change = false;
        for (uint i = 0; i < size; i++)
        {
            for (uint j = 0; j < size; j++)
            {
                if (newmat[i][j] != mat[i][j])
                {
                    mat[i][j] = newmat[i][j];
                    change = true;
                }
            }
        }
    }
}

// Récupération de la matrice des arêtes d'un graphe
bool **graph_to_mat(p_graph G)
{
    // Initialisation de la matrice
    bool **mat;
    MALLOC(mat, G->size);
    for (uint q = 0; q < G->size; q++)
    {
        MALLOC(mat[q], G->size);
        // On initialise toutes les cases à false
        for (uint r = 0; r < G->size; r++)
        {
            mat[q][r] = false;
        }
        // On enregistre maintenant les arêtes
        for (uint i = 0; i < size_vertices(G->edges[q]); i++)
        {
            mat[q][lefread_vertices(G->edges[q], i)] = true;
        }
    }
    return mat;
}

// Récupération de la matrice d'accessibilité d'un
// graphe.
bool **graph_to_accmat(p_graph G)
{
    bool **mat = graph_to_mat(G);
    // Affectation de la diagonale
    for (uint q = 0; q < G->size; q++)
    {
        mat[q][q] = true;
    }
    saturate_mat(mat, G->size);
    return mat;
}

// Récupération de la matrice des arêtes étiquetée
// d'un graphe étiqueté.  Indicage: [q][a][r] pour
// l'arête (q,a,r)
bool ***lgraph_to_lmat(p_lgraph G)
{
    // Initialisation de la matrice
    bool ***themat;
    MALLOC(themat, G->size_graph);
    for (uint q = 0; q < G->size_graph; q++)
    {
        MALLOC(themat[q], G->size_alpha);
        for (uint a = 0; a < G->size_alpha; a++)
        {
            MALLOC(themat[q][a], G->size_graph);
            // On remplit d'abord toutes les cases avec false
            for (uint r = 0; r < G->size_graph; r++)
            {
                themat[q][a][r] = false;
            }
            // On enregistre ensuite les transitions (q,a,r) valides
            for (uint i = 0; i < size_vertices(G->edges[q][a]); i++)
            {
                themat[q][a][lefread_vertices(G->edges[q][a], i)] = true;
            }
        }
    }
    return themat;
}

// Récupération de la matrice d'accessibilité d'un
// graphe étiqueté.
bool **lgraph_to_accmat(p_lgraph G)
{
    // Initialisation de la matrice
    bool **themat;
    MALLOC(themat, G->size_graph);
    for (uint q = 0; q < G->size_graph; q++)
    {
        MALLOC(themat[q], G->size_alpha);
        // On remplit d'abord toutes les cases avec false
        for (uint r = 0; r < G->size_graph; r++)
        {
            themat[q][r] = false;
        }
        themat[q][q] = true; // La diagonale
        for (uint a = 0; a < G->size_alpha; a++)
        {
            // On enregistre ensuite les arêtes valides
            for (uint i = 0; i < size_vertices(G->edges[q][a]); i++)
            {
                themat[q][lefread_vertices(G->edges[q][a], i)] = true;
            }
        }
    }
    saturate_mat(themat, G->size_graph);
    return themat;
}

// Fonctions de libération
void free_mat(bool **themat, uint size_graph)
{
    for (uint q = 0; q < size_graph; q++)
    {
        free(themat[q]);
    }
    free(themat);
}

void free_lmat(bool ***themat, uint size_graph, uint size_alpha)
{
    for (uint q = 0; q < size_graph; q++)
    {
        for (uint a = 0; a < size_alpha; a++)
        {
            free(themat[q][a]);
        }
        free(themat[q]);
    }
    free(themat);
}

/************/
/* Parcours */
/************/

// Parcours DFS: retourne la liste triée des sommets accessibles depuis un sommet dans ini
// La lise ini est vidée (elle est utilisée comme pile)
// La liste "alph" contient un sous-alphabet (on peut l'utiliser pour restreindre les transitions autorisées)
// Si la liste alph est égale à NULL, toutes les lettres sont autorisées
p_vertices graph_dfs(p_graph G, p_vertices ini, bool *rest)
{
    // Tableau pour mémoriser les sommets visités
    bool visited[G->size];
    for (uint q = 0; q < G->size; q++)
    {
        visited[q] = false;
    }

    // On y mémorise les sommes de départ
    for (uint i = 0; i < size_vertices(ini); i++)
    {
        visited[lefread_vertices(ini, i)] = true;
    }

    // Parcours DFS
    while (!isempty_vertices(ini))
    {
        uint q = rigpull_vertices(ini);

        for (uint j = 0; j < size_vertices(G->edges[q]); j++)
        {
            if (!visited[lefread_vertices(G->edges[q], j)])
            {
                visited[lefread_vertices(G->edges[q], j)] = true;
                rigins_vertices(lefread_vertices(G->edges[q], j), ini);
            }
        }
    }

    // Construction de la liste des sommets accessibles
    p_vertices res = create_vertices();
    for (uint q = 0; q < G->size; q++)
    {
        if (visited[q] && (rest == NULL || rest[q]))
        {
            rigins_vertices(q, res);
        }
    }
    return res;
}

p_vertices lgraph_dfs(p_lgraph G, p_vertices ini, bool *alph, bool *rest)
{
    {
        // Tableau pour mémoriser les sommets visités
        bool visited[G->size_graph];
        for (uint q = 0; q < G->size_graph; q++)
        {
            visited[q] = false;
        }

        // On y mémorise les sommes de départ
        for (uint i = 0; i < size_vertices(ini); i++)
        {
            visited[lefread_vertices(ini, i)] = true;
        }

        // Parcours DFS
        while (!isempty_vertices(ini))
        {
            uint q = rigpull_vertices(ini);

            if (alph)
            {
                for (uint a = 0; a < G->size_alpha; a++)
                {
                    for (uint j = 0; j < size_vertices(G->edges[q][a]); j++)
                    {

                        if (alph[a] && !visited[lefread_vertices(G->edges[q][a], j)])
                        {
                            visited[lefread_vertices(G->edges[q][a], j)] = true;
                            rigins_vertices(lefread_vertices(G->edges[q][a], j), ini);
                        }
                    }
                }
            }
            else
            {
                for (uint a = 0; a < G->size_alpha; a++)
                {
                    for (uint j = 0; j < size_vertices(G->edges[q][a]); j++)
                    {

                        if (!visited[lefread_vertices(G->edges[q][a], j)])
                        {
                            visited[lefread_vertices(G->edges[q][a], j)] = true;
                            rigins_vertices(lefread_vertices(G->edges[q][a], j), ini);
                        }
                    }
                }
            }
        }

        // Construction de la liste des sommets accessibles
        p_vertices res = create_vertices();
        for (uint q = 0; q < G->size_graph; q++)
        {
            if (visited[q] && (rest == NULL || rest[q]))
            {
                rigins_vertices(q, res);
            }
        }
        return res;
    }
}

p_vertices dgraph_dfs(p_dgraph G, p_vertices ini, bool *alph, bool *rest)
{
    // Tableau pour mémoriser les sommets visités
    bool visited[G->size_graph];
    for (uint q = 0; q < G->size_graph; q++)
    {
        visited[q] = false;
    }

    // On y mémorise les sommes de départ
    for (uint i = 0; i < size_vertices(ini); i++)
    {
        visited[lefread_vertices(ini, i)] = true;
    }

    // Parcours DFS
    while (!isempty_vertices(ini))
    {
        uint q = rigpull_vertices(ini);

        if (alph)
        {
            for (uint a = 0; a < G->size_alpha; a++)
            {
                if (alph[a] && !visited[G->edges[q][a]])
                {
                    visited[G->edges[q][a]] = true;
                    rigins_vertices(G->edges[q][a], ini);
                }
            }
        }
        else
        {
            for (uint a = 0; a < G->size_alpha; a++)
            {

                if (!visited[G->edges[q][a]])
                {
                    visited[G->edges[q][a]] = true;
                    rigins_vertices(G->edges[q][a], ini);
                }
            }
        }
    }

    // Construction de la liste des sommets accessibles
    p_vertices res = create_vertices();
    for (uint q = 0; q < G->size_graph; q++)
    {
        if (visited[q] && (rest == NULL || rest[q]))
        {
            rigins_vertices(q, res);
        }
    }
    return res;
}

// Parcours BFS: retourne la liste triée des sommets accessibles depuis un sommet dans ini
// La lise ini est vidée (elle est utilisée comme file)
// La liste "alph" contient un sous-alphabet (on peut l'utiliser pour restreindre les transitions autorisées)
// Si la liste alph est égale à NULL, toutes les lettres sont autorisées
p_vertices graph_bfs(p_graph G, p_vertices ini, bool *rest)
{
    // Tableau pour mémoriser les sommets visités
    bool visited[G->size];
    for (uint q = 0; q < G->size; q++)
    {
        visited[q] = false;
    }

    // On y mémorise les sommes de départ
    for (uint i = 0; i < size_vertices(ini); i++)
    {
        visited[lefread_vertices(ini, i)] = true;
    }

    // Parcours DFS
    while (!isempty_vertices(ini))
    {
        uint q = rigpull_vertices(ini);

        for (uint j = 0; j < size_vertices(G->edges[q]); j++)
        {
            if (!visited[lefread_vertices(G->edges[q], j)])
            {
                visited[lefread_vertices(G->edges[q], j)] = true;
                lefins_vertices(lefread_vertices(G->edges[q], j), ini);
            }
        }
    }

    // Construction de la liste des sommets accessibles
    p_vertices res = create_vertices();
    for (uint q = 0; q < G->size; q++)
    {
        if (visited[q] && (rest == NULL || rest[q]))
        {
            rigins_vertices(q, res);
        }
    }
    return res;
}

p_vertices lgraph_bfs(p_lgraph G, p_vertices ini, bool *alph, bool *rest)
{
    {
        // Tableau pour mémoriser les sommets visités
        bool visited[G->size_graph];
        for (uint q = 0; q < G->size_graph; q++)
        {
            visited[q] = false;
        }

        // On y mémorise les sommes de départ
        for (uint i = 0; i < size_vertices(ini); i++)
        {
            visited[lefread_vertices(ini, i)] = true;
        }

        // Parcours DFS
        while (!isempty_vertices(ini))
        {
            uint q = rigpull_vertices(ini);

            if (alph)
            {
                for (uint a = 0; a < G->size_alpha; a++)
                {
                    for (uint j = 0; j < size_vertices(G->edges[q][a]); j++)
                    {

                        if (alph[a] && !visited[lefread_vertices(G->edges[q][a], j)])
                        {
                            visited[lefread_vertices(G->edges[q][a], j)] = true;
                            lefins_vertices(lefread_vertices(G->edges[q][a], j), ini);
                        }
                    }
                }
            }
            else
            {
                for (uint a = 0; a < G->size_alpha; a++)
                {
                    for (uint j = 0; j < size_vertices(G->edges[q][a]); j++)
                    {

                        if (!visited[lefread_vertices(G->edges[q][a], j)])
                        {
                            visited[lefread_vertices(G->edges[q][a], j)] = true;
                            lefins_vertices(lefread_vertices(G->edges[q][a], j), ini);
                        }
                    }
                }
            }
        }

        // Construction de la liste des sommets accessibles
        p_vertices res = create_vertices();
        for (uint q = 0; q < G->size_graph; q++)
        {
            if (visited[q] && (rest == NULL || rest[q]))
            {
                rigins_vertices(q, res);
            }
        }
        return res;
    }
}

p_vertices dgraph_bfs(p_dgraph G, p_vertices ini, bool *alph, bool *rest)
{
    // Tableau pour mémoriser les sommets visités
    bool visited[G->size_graph];
    for (uint q = 0; q < G->size_graph; q++)
    {
        visited[q] = false;
    }

    // On y mémorise les sommes de départ
    for (uint i = 0; i < size_vertices(ini); i++)
    {
        visited[lefread_vertices(ini, i)] = true;
    }

    // Parcours DFS
    while (!isempty_vertices(ini))
    {
        uint q = rigpull_vertices(ini);

        if (alph)
        {
            for (uint a = 0; a < G->size_alpha; a++)
            {
                if (alph[a] && !visited[G->edges[q][a]])
                {
                    visited[G->edges[q][a]] = true;
                    lefins_vertices(G->edges[q][a], ini);
                }
            }
        }
        else
        {
            for (uint a = 0; a < G->size_alpha; a++)
            {

                if (!visited[G->edges[q][a]])
                {
                    visited[G->edges[q][a]] = true;
                    lefins_vertices(G->edges[q][a], ini);
                }
            }
        }
    }

    // Construction de la liste des sommets accessibles
    p_vertices res = create_vertices();
    for (uint q = 0; q < G->size_graph; q++)
    {
        if (visited[q] && (rest == NULL || rest[q]))
        {
            rigins_vertices(q, res);
        }
    }
    return res;
}

/************************************/
/* Récupération des listes d'arêtes */
/************************************/

/* Version simple: arêtes indépendantes */

// Calcul
p_stack
graph_to_edges(p_graph G)
{
    p_stack thestack = create_stack();

    for (uint q = 0; q < G->size; q++)
    {
        for (uint i = 0; i < size_vertices(G->edges[q]); i++)
        {
            p_edge new;
            MALLOC(new, 1);
            new->in = q;
            new->out = lefread_vertices(G->edges[q], i);
            push(new, thestack);
        }
    }
    return thestack;
}

p_stack lgraph_to_ledges(p_lgraph G)
{
    p_stack thestack = create_stack();

    for (uint q = 0; q < G->size_graph; q++)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {
            for (uint i = 0; i < size_vertices(G->edges[q][a]); i++)
            {
                p_ledge new;
                MALLOC(new, 1);
                new->in = q;
                new->lab = a;
                new->out = lefread_vertices(G->edges[q][a], i);
                push(new, thestack);
            }
        }
    }
    return thestack;
}

p_stack dgraph_to_ledges(p_dgraph G)
{
    p_stack thestack = create_stack();

    for (uint q = 0; q < G->size_graph; q++)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {

            p_ledge new;
            MALLOC(new, 1);
            new->in = q;
            new->lab = a;
            new->out = G->edges[q][a];
            push(new, thestack);
        }
    }
    return thestack;
}

// Fonctions de comparaison (pour les tris)
// -1: >, 0 : ==, 1: <
int edge_comp(void *e, void *f)
{
    if (((p_edge)e)->in < ((p_edge)f)->in)
    {
        return 1;
    }
    else if (((p_edge)e)->in > ((p_edge)f)->in)
    {
        return -1;
    }
    else
    {
        if (((p_edge)e)->out < ((p_edge)f)->out)
        {
            return 1;
        }
        else if (((p_edge)e)->out > ((p_edge)f)->out)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}
int edge_comp_rev(void *e, void *f)
{
    if (((p_edge)e)->out < ((p_edge)f)->out)
    {
        return 1;
    }
    else if (((p_edge)e)->out > ((p_edge)f)->out)
    {
        return -1;
    }
    else
    {
        if (((p_edge)e)->in < ((p_edge)f)->in)
        {
            return 1;
        }
        else if (((p_edge)e)->in > ((p_edge)f)->in)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}
int l_edge_comp(void *e, void *f)
{
    if (((p_ledge)e)->in < ((p_ledge)f)->in)
    {
        return 1;
    }
    else if (((p_ledge)e)->in > ((p_ledge)f)->in)
    {
        return -1;
    }
    else
    {
        if (((p_ledge)e)->lab < ((p_ledge)f)->lab)
        {
            return 1;
        }
        else if (((p_ledge)e)->lab > ((p_ledge)f)->lab)
        {
            return -1;
        }
        else
        {
            if (((p_ledge)e)->out < ((p_ledge)f)->out)
            {
                return 1;
            }
            else if (((p_ledge)e)->out > ((p_ledge)f)->out)
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}
int l_edge_comp_rev(void *e, void *f)
{
    if (((p_ledge)e)->out < ((p_ledge)f)->out)
    {
        return 1;
    }
    else if (((p_ledge)e)->out > ((p_ledge)f)->out)
    {
        return -1;
    }
    else
    {
        if (((p_ledge)e)->lab < ((p_ledge)f)->lab)
        {
            return 1;
        }
        else if (((p_ledge)e)->lab > ((p_ledge)f)->lab)
        {
            return -1;
        }
        else
        {
            if (((p_ledge)e)->in < ((p_ledge)f)->in)
            {
                return 1;
            }
            else if (((p_ledge)e)->in > ((p_ledge)f)->in)
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
    }
}

/* Version fusion: pour l'affichage */

static int multi_edges_comp(void *s, void *t)
{
    if (((p_multi_edge)s)->in < ((p_multi_edge)t)->in)
    {
        return 1;
    }
    else if (((p_multi_edge)s)->in > ((p_multi_edge)t)->in)
    {
        return -1;
    }
    else
    {
        if (((p_multi_edge)s)->out < ((p_multi_edge)t)->out)
        {
            return 1;
        }
        else if (((p_multi_edge)s)->out > ((p_multi_edge)t)->out)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

static void tree_to_stack(p_avlnode tree, p_stack stack)
{
    if (tree == NULL)
    {
        return;
    }
    else
    {
        tree_to_stack(tree->left, stack);
        push(tree->value, stack);
        tree_to_stack(tree->right, stack);
        free(tree);
    }
}
// 0: only an lgraph with classic labels
// 1: an additional graph (eps trans)
// 2: an additional lgraph(inv trans)
// 3: an additional lgraph(inv trans) + an additional graph (eps trans)
p_stack lgraph_to_multi_edges(uint n, ...)
{
    if (n > 3)
    {
        return NULL;
    }
    va_list list;
    va_start(list, n);
    p_lgraph G = va_arg(list, p_lgraph);
    p_graph GEPS = NULL;
    p_lgraph GINV = NULL;
    if (n == 1)
    {
        GEPS = va_arg(list, p_graph);
    }
    if (n == 2)
    {
        GINV = va_arg(list, p_lgraph);
    }
    if (n == 3)
    {
        GINV = va_arg(list, p_lgraph);
        GEPS = va_arg(list, p_graph);
    }

    p_stack thestack = create_stack();
    p_avlnode thetree = NULL;
    for (uint q = 0; q < G->size_graph; q++)
    {
        if (GEPS != NULL)
        {
            for (uint i = 0; i < size_vertices(GEPS->edges[q]); i++)
            {
                if (lefread_vertices(GEPS->edges[q], i) != q)
                {
                    p_multi_edge new;
                    MALLOC(new, 1);
                    new->in = q;
                    new->out = lefread_vertices(GEPS->edges[q], i);
                    p_avlnode old = avl_search(new, thetree, &multi_edges_comp);
                    if (old == NULL)
                    {

                        new->lab = create_vertices();
                        new->lab_i = create_vertices();
                        new->eps = true;
                        thetree = avl_insert(new, thetree, &multi_edges_comp);
                    }
                    else
                    {
                        ((p_multi_edge)old->value)->eps = true;
                        free(new);
                    }
                }
            }
        }
        for (uint a = 0; a < G->size_alpha; a++)
        {
            for (uint i = 0; i < size_vertices(G->edges[q][a]); i++)
            {
                p_multi_edge new;
                MALLOC(new, 1);
                new->in = q;
                new->out = lefread_vertices(G->edges[q][a], i);
                p_avlnode old = avl_search(new, thetree, &multi_edges_comp);
                if (old == NULL)
                {

                    new->lab = create_vertices();
                    new->lab_i = create_vertices();
                    new->eps = false;
                    rigins_vertices(a, new->lab);
                    thetree = avl_insert(new, thetree, &multi_edges_comp);
                }
                else
                {
                    rigins_vertices(a, ((p_multi_edge)old->value)->lab);
                    free(new);
                }
            }
        }
        if (GINV != NULL)
        {
            for (uint a = 0; a < GINV->size_alpha; a++)
            {
                for (uint i = 0; i < size_vertices(GINV->edges[q][a]); i++)
                {
                    p_multi_edge new;
                    MALLOC(new, 1);
                    new->in = q;
                    new->out = lefread_vertices(GINV->edges[q][a], i);
                    p_avlnode old = avl_search(new, thetree, &multi_edges_comp);
                    if (old == NULL)
                    {

                        new->lab = create_vertices();
                        new->lab_i = create_vertices();
                        new->eps = false;
                        rigins_vertices(a, new->lab_i);
                        thetree = avl_insert(new, thetree, &multi_edges_comp);
                    }
                    else
                    {
                        rigins_vertices(a, ((p_multi_edge)old->value)->lab_i);
                        free(new);
                    }
                }
            }
        }
    }

    tree_to_stack(thetree, thestack);
    return thestack;
}

p_stack dgraph_to_multi_edges(p_dgraph G)
{
    p_stack thestack = create_stack();
    p_avlnode thetree = NULL;
    for (uint q = 0; q < G->size_graph; q++)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {
            p_multi_edge new;
            MALLOC(new, 1);
            new->in = q;
            // printf("test: %d\n", G->edges[q][a]);
            new->out = G->edges[q][a];
            p_avlnode old = avl_search(new, thetree, &multi_edges_comp);
            if (old == NULL)
            {

                new->lab = create_vertices();
                rigins_vertices(a, new->lab);
                thetree = avl_insert(new, thetree, &multi_edges_comp);
            }
            else
            {
                rigins_vertices(a, ((p_multi_edge)old->value)->lab);
                free(new);
            }
        }
    }

    tree_to_stack(thetree, thestack);
    return thestack;
}

/*******************************/
/* Fusion de plusieurs graphes */
/*******************************/

/* Versions disjointes */

// Graphes classiques
// Le paramètre int* lag set à retourner (par pointeur)
// les indices où commencent les copies de chaque graph
p_graph merge_graphs(uint *lag, uint n, p_graph *list)
{
    // Variables qui contiendra la taille du nouveau graph
    uint size = 0;

    // Récupération des inputs
    for (uint i = 0; i < n; i++)
    {
        lag[i] = size;
        size = size + list[i]->size;
    }

    // Création du nouveau graph
    p_graph new = create_graph_noedges(size);
    // printf("New graph size: %d\n", size);
    for (uint i = 0; i < n; i++)
    {
        // Pour tout sommet du graphe i
        for (uint q = 0; q < list[i]->size; q++)
        {
            // On copie les sommets adjacents en décalant leurs numéros
            copy_vertices_right(new->edges[q + lag[i]], list[i]->edges[q], lag[i]);
        }
    }
    return new;
}

// Graphes étiquetés
// Le paramètre int* lag set à retourner (par pointeur)
// les indices où commencent les copies de chaque graph
p_lgraph merge_lgraphs(uint *lag, uint n, p_lgraph *list)
{
    // Variables qui contiendront les tailles du nouveau graph et de son alphabet
    uint size_graph = 0;
    uint size_alpha = 1;

    // Récupération des inputs
    for (uint i = 0; i < n; i++)
    {

        lag[i] = size_graph;
        size_graph = size_graph + list[i]->size_graph;
        size_alpha = max(size_alpha, list[i]->size_alpha);
    }

    // Création du nouveau graph
    p_lgraph new = create_lgraph_noedges(size_graph, size_alpha);
    for (uint i = 0; i < n; i++)
    {
        // Pour tout sommet du graphe i
        for (uint q = 0; q < list[i]->size_graph; q++)
        {
            // Pour toute lettre a
            for (uint a = 0; a < list[i]->size_alpha; a++)
            {
                // On copie les sommets adjacents en décalant leurs numéros
                copy_vertices_right(new->edges[q + lag[i]][a], list[i]->edges[q][a], lag[i]);
            }
        }
    }
    return new;
}

// Graphes étiqueté déterministe (le retour n'est plus déterministe)
// Le paramètre int* lag set à retourner (par pointeur)
// les indices où commencent les copies de chaque graph
p_lgraph merge_dgraphs(uint *lag, uint n, p_dgraph *list)
{
    // Variables qui contiendront les tailles du nouveau graph et de son alphabet
    uint size_graph = 0;
    uint size_alpha = 1;

    // Récupération des inputs
    for (uint i = 0; i < n; i++)
    {
        lag[i] = size_graph;
        size_graph = size_graph + list[i]->size_graph;
        size_alpha = max(size_alpha, list[i]->size_alpha);
    }

    // Création du nouveau graph
    p_lgraph new = create_lgraph_noedges(size_graph, size_alpha);
    for (uint i = 0; i < n; i++)
    {
        // Pour tout sommet du graphe i
        for (uint q = 0; q < list[i]->size_graph; q++)
        {
            // Pour toute lettre a
            for (uint a = 0; a < list[i]->size_alpha; a++)
            {
                // On copie les sommets adjacents en décalant leurs numéros
                rigins_vertices(list[i]->edges[q][a] + lag[i], new->edges[q + lag[i]][a]);
            }
        }
    }
    return new;
}

/* Fusion de plusieurs graphes ayant le même ensemble de sommets (partage des arêtes) */

// ng: nombre de graphes classiques dans la fusion
// nlg: nombre de graphes étiquetés dans la fusion
// ndg: nombre de graphes étiquetés déterministes dans la fusion
// n: le nombre total de paramètre (la somme des 3)
// Les graphes à fusionner
p_graph ldgraphs_to_graph(uint ng, uint nlg, uint ndg, uint n, ...)
{
    if (n == 0 || ng + nlg + ndg != n)
    { // Si il n'y a pas d'arguments ou qu'ils sont inconsistants
        return NULL;
    }
    // Récupération des inputs
    va_list list;
    va_start(list, n);
    uint size;
    p_graph G;

    // p_lgraph lginputs[nlg + 1];
    // p_dgraph dginputs[ndg + 1];
    if (ng != 0)
    {
        p_graph ginput;

        for (uint i = 0; i < ng; i++)
        {
            ginput = va_arg(list, p_graph);
            if (i == 0)
            {
                size = ginput->size;
                G = create_graph_noedges(size);
            }
            else if (ginput->size != size)
            {
                printf("Error, the graphs should all have the same size\n");
                delete_graph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++)
            {
                merge_sorted_vertices(G->edges[q], ginput->edges[q]);
            }
        }
    }
    if (nlg != 0)
    {
        // printf("test\n");
        p_lgraph lginput;
        for (uint i = 0; i < nlg; i++)
        {
            lginput = va_arg(list, p_lgraph);
            if (i == 0 && ng == 0)
            {
                size = lginput->size_graph;
                // printf("thesize %d\n", size);
                G = create_graph_noedges(size);
            }
            else if (lginput->size_graph != size)
            {
                printf("Error, the graphs should all have the same size\n");
                delete_graph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++)
            {
                for (uint a = 0; a < lginput->size_alpha; a++)
                {
                    merge_sorted_vertices(G->edges[q], lginput->edges[q][a]);
                }
            }
        }
    }

    if (ndg != 0)
    {
        p_dgraph dginput;
        for (uint i = 0; i < ndg; i++)
        {
            dginput = va_arg(list, p_dgraph);
            if (i == 0 && ng == 0 && nlg == 0)
            {
                size = dginput->size_graph;
                G = create_graph_noedges(size);
            }
            else if (dginput->size_graph != size)
            {
                printf("Error, the graphs should all have the same size\n");
                delete_graph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++)
            {
                for (uint a = 0; a < dginput->size_alpha; a++)
                {
                    insert_vertices(G->edges[q], dginput->edges[q][a]);
                }
            }
        }
    }

    return G;
}

// nlg: nombre de graphes étiquetés dans la fusion
// ndg: nombre de graphes étiquetés déterministes dans la fusion
// n: le nombre total de paramètre (la somme des 2)
// Les graphes à fusionner
p_lgraph ldgraphs_to_lgraph(uint nlg, uint ndg, uint n, ...)
{
    if (n == 0 || nlg + ndg != n)
    { // Si il n'y a pas d'arguments ou qu'ils sont inconsistants
        return NULL;
    }
    // Récupération des inputs
    va_list list;
    va_start(list, n);
    uint size;
    uint asize;
    p_lgraph G;

    if (nlg != 0)
    {
        // printf("test\n");
        p_lgraph lginput;
        for (uint i = 0; i < nlg; i++)
        {
            lginput = va_arg(list, p_lgraph);
            if (i == 0)
            {
                size = lginput->size_graph;
                asize = lginput->size_alpha;
                // printf("thesize %d\n", size);
                G = create_lgraph_noedges(size, asize);
            }
            else if (lginput->size_graph != size)
            {
                printf("Error, the graphs should all have the same size\n");
                delete_lgraph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++)
            {
                for (uint a = 0; a < lginput->size_alpha; a++)
                {
                    merge_sorted_vertices(G->edges[q][a], lginput->edges[q][a]);
                }
            }
        }
    }

    if (ndg != 0)
    {
        p_dgraph dginput;
        for (uint i = 0; i < ndg; i++)
        {
            dginput = va_arg(list, p_dgraph);
            if (i == 0 && nlg == 0)
            {
                size = dginput->size_graph;
                asize = dginput->size_alpha;
                G = create_lgraph_noedges(size, asize);
            }
            else if (dginput->size_graph != size)
            {
                printf("Error, the graphs should all have the same size\n");
                delete_lgraph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++)
            {
                for (uint a = 0; a < dginput->size_alpha; a++)
                {
                    insert_vertices(G->edges[q][a], dginput->edges[q][a]);
                }
            }
        }
    }

    return G;
}

/* Conversion d'un dgraph en lgraph */
p_lgraph dgraph_to_lgraph(p_dgraph DG)
{
    p_lgraph LG = create_lgraph_noedges(DG->size_graph, DG->size_alpha);
    for (uint q = 0; q < DG->size_graph; q++)
    {
        for (uint a = 0; a < DG->size_alpha; a++)
        {
            rigins_vertices(DG->edges[q][a], LG->edges[q][a]);
        }
    }
    return LG;
}

/**************************************************************************************/
/* Récupération de tous les sommets adjacents à un ensemble de sommets dans un lgraph */
/**************************************************************************************/

// Retourne tous les sommets adjacents à un sommet dans start par la lettre a
p_vertices lgraph_reachable(p_lgraph G, p_vertices start, uint a)
{
    p_vertices tomerge[size_vertices(start)];
    for (uint i = 0; i < size_vertices(start); i++)
    {
        tomerge[i] = G->edges[lefread_vertices(start, i)][a];
    }
    p_vertices end = create_vertices();
    merge_array_sorted_vertices(end, tomerge, size_vertices(start));
    return end;
}

// // Retourne tous les sommets adjacents à un sommet dans start par la lettre a
// p_vertices lgraph_reachable(p_lgraph G, p_vertices start, uint a)
// {
//     p_binheap heap = create_binheap(&fcmp_int);
//     for (uint i = 0; i < size_vertices(start); i++)
//     {
//         for (uint j = 0; j < size_vertices(G->edges[lefread_vertices(start, i)][a]); j++)
//         {
//             push_binheap(heap, lefread_vertices_ref(G->edges[lefread_vertices(start, i)][a], j));
//         }
//     }
//     p_vertices end = create_vertices();
//     while (!isempty_binheap(heap))
//     {
//         uint *p_q = popmin_binheap(heap);
//         if (isempty_vertices(end) || rigread_vertices(end, 0) != *p_q)
//         {
//             rigins_vertices(*p_q, end);
//         }
//     }
//     delete_binheap(heap);
//     return end;
// }
