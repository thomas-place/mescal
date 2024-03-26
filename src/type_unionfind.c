#include "type_unionfind.h"
#include <stdint.h>
#include <limits.h>

/*******************/
/* Type UNION-FIND */
/*******************/

// Crée un union-find de l'ensemble des éléments de 0 à size-1
// Initialement, la partition est composée de singletons
p_ufind create_ufind(uint size)
{
    if (size == 0)
    {
        printf("Warning, cannot create a union-find of size zero. Returned NULL.\n");
        return NULL;
    }
    p_ufind new;
    MALLOC(new, 1);
    new->size_set = size; // On partitionne les éléments de 0 à size-1
    new->size_par = size; // Initialement, chaque classe est un singleton, on a donc size classes

    // Calcul de la taille des tableau (plus petite puissance de deux supérieure à size)
    new->size_tab = 1;
    while (new->size_tab < new->size_set)
    {
        new->size_tab = new->size_tab << 1;
    }

    // Création des tableaux
    MALLOC(new->parent, new->size_tab);
    MALLOC(new->rank, new->size_tab);
    MALLOC(new->sizec, new->size_tab);
    for (uint i = 0; i < new->size_set; i++)
    {
        new->parent[i] = i;
        new->sizec[i]  = 1; // Chaque classe est un singleton
        new->rank[i]   = 0; // La profondeur de l'arbre est 0
    }
    // les éléments au delà de size_set ne servent pas
    return new;
}

// Suppression
void delete_ufind(p_ufind uf)
{
    if (uf == NULL)
    {
        return;
    }
    free(uf->parent);
    free(uf->rank);
    free(uf->sizec);
    free(uf);
}

// Récupération d'information
uint sizeset_ufind(p_ufind uf)
{
    return uf->size_set;
}

uint sizepar_ufind(p_ufind uf)
{
    return uf->size_par;
}

static void grow_ufind(p_ufind uf)
{
    uf->size_tab = 2 * uf->size_tab;
    REALLOC(uf->parent, uf->size_tab);
    REALLOC(uf->rank, uf->size_tab);
    REALLOC(uf->sizec, uf->size_tab);
}

// Agrandit la partition en ajoutant un élément
void makeset_ufind(p_ufind uf)
{
    if (uf->size_tab == uf->size_set)
    {
        grow_ufind(uf);
    }

    uf->parent[uf->size_set] = uf->size_set;
    uf->sizec[uf->size_set]  = 1; // La classe du nouvel élément est un singleton
    uf->rank[uf->size_set]   = 0; // L'arbre correspondant est de profondeur 0

    uf->size_set++;
    uf->size_par++;
}

// Recherche
uint find_ufind(uint i, p_ufind uf)
{
    if (uf->parent[i] != i)
    {
        uf->parent[i] = find_ufind(uf->parent[i], uf);
    }
    return uf->parent[i];
}

// Récupération de la taille de la classe
uint sizeclass_ufind(uint i, p_ufind uf)
{
    i = find_ufind(i, uf);
    return uf->sizec[i];
}

// Union
void union_ufind(uint i, uint j, p_ufind uf)
{
    i = find_ufind(i, uf);
    j = find_ufind(j, uf);
    if (i == j)
    {
        return;
    }
    if (uf->rank[i] < uf->rank[j])
    {
        uf->parent[i] = j;
        uf->sizec[j]  = uf->sizec[i] + uf->sizec[j];
    }
    else if (uf->rank[i] > uf->rank[j])
    {
        uf->parent[j] = i;
        uf->sizec[i]  = uf->sizec[i] + uf->sizec[j];
    }
    else
    {
        uf->parent[j] = i;
        uf->rank[i]++;
        uf->sizec[i] = uf->sizec[i] + uf->sizec[j];
    }
    uf->size_par--;
}

// Affichage
void print_ufind(p_ufind uf)
{
    CRITICAL("Union-find of %d elements, %d classes", uf->size_set, uf->size_par);
    for (uint i = 0; i < uf->size_set; i++)
    {
        printf("Element %d has parent %d and size %d\n", i, uf->parent[i], uf->sizec[i]);
    }

    // Classes et remplissage actuel
    p_vertices classes[uf->size_par];
    for (uint c = 0; c < uf->size_par; c++)
    {
        classes[c] = create_vertices();
    }

    // Tableau pour mémoriser la classe associée à chaque racine
    int root_class[uf->size_set];
    for (uint r = 0; r < uf->size_set; r++)
    {
        root_class[r] = -1;
    }

    // Prochain numéro de classe
    int num_class = 0;

    // Calcul des classes
    for (uint i = 0; i < uf->size_set; i++)
    {
        DEBUG("Processing %d", i);
        uint r = find_ufind(i, uf);
        DEBUG("Root is %d", r);
        DEBUG("Root class of %d is %d", r, root_class[r]);
        if (root_class[r] == -1)
        {
            DEBUG("Root %d has no class yet, assigning %d", r, num_class);
            root_class[r] = num_class;
            num_class++;
        }
        else
        {
            DEBUG("Root %d has already class %d", r, root_class[r]);
        }
        int c = root_class[r];
        rigins_vertices(i, classes[c]);
    }

    // Affichage des classes
    for (uint class = 0; class < uf->size_par; class++)
    {
        printf("Classe %d size %d: ", class, size_vertices(classes[class]));
        for (uint i = 0; i < size_vertices(classes[class]); i++)
        {
            printf("%d ", lefread_vertices(classes[class], i));
        }
        printf("\n");
    }
    printf("\n");

    // Free
    for (uint class = 0; class < uf->size_par; class++)
    {
        delete_vertices(classes[class]);
    }
}
