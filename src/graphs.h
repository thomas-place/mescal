/********************************************/
/*   Structures pour stocker les machines   */
/* Ensuite spécialisées pour NFA,DFA,Cayley */
/********************************************/

#ifndef MACHINES_H
#define MACHINES_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "type_basic.h"
#include "type_vertices.h"
#include "type_stack.h"
#include "type_binheap.h"
#include "type_abr.h"

/* Graphs orientés non-étiquetés */
typedef struct
{
    // Nombre de sommets
    uint size;

    // Tableau de taille "size"
    // Edge[s] contient la liste des sommets adajacents à s
    p_vertices *edges;

} graph;

/* Graph orientés étiquetés par un alphabet */
typedef struct
{
    // Nombre de sommets
    uint size_graph;

    // Taille de l'alphabet
    uint size_alpha;

    // Tableau de tableaux taille "size_graph * size_alpha"
    // Edge[s][a] contient la liste des sommets reliés à s par la lettre a
    p_vertices **edges;

} lgraph;

/* Graph orientés déterministes complets étiquetés par un alphabet */
typedef struct
{
    // Nombre de sommets
    uint size_graph;

    // Taille de l'alphabet
    uint size_alpha;

    // Tableau de tableaux taille "size_graph * size_alpha"
    // Edge[s][a] contient l'unique sommet relié à s par la lettre a
    uint **edges;

} dgraph;

typedef graph *p_graph;
typedef lgraph *p_lgraph;
typedef dgraph *p_dgraph;

/*********************/
/* Fonctions de test */
/*********************/

void graph_printing_test(p_graph G, FILE *out);

/*************************/
/* Fonctions de création */
/*************************/

// Intitialise un graphe sans arêtes
p_graph create_graph_noedges(uint size);

// Intitialise un graphe étiqueté sans arêtes
p_lgraph create_lgraph_noedges(uint size_graph, uint size_alpha);

// Intitialise un graphe déterministe complet étiqueté sans uniquement des self-loops
p_dgraph create_dgraph_noedges(uint size_graph, uint size_alpha);

/****************************/
/* Fonctions de suppression */
/****************************/

// Désallocation d'un graphe
void delete_graph(p_graph G);

// Désallocation d'un graphe étiqueté
void delete_lgraph(p_lgraph G);

// Désallocation d'un graphe étiqueté déterministe complet
void delete_dgraph(p_dgraph G);

/****************************/
/* Matrices d'accessibilité */
/****************************/

// Saturation d'une matrice, la diagonale est supposée contenir des true
void saturate_mat(bool **mat, uint size);

// Récupération de la matrice des arêtes d'un graphe
bool **graph_to_mat(p_graph);

// Récupération de la matrice d'accessibilité d'un
// graphe.
bool **graph_to_accmat(p_graph);

// Récupération de la matrice des arêtes étiquetée
// d'un graphe étiqueté.  Indicage: [q][a][r] pour
// l'arête (q,a,r)
bool ***lgraph_to_lmat(p_lgraph);

// Récupération de la matrice d'accessibilité d'un
// graphe étiqueté.
bool **lgraph_to_accmat(p_lgraph);

// Fonctions de libération
void free_mat(bool **, uint size_graph);
void free_lmat(bool ***, uint size_graph, uint size_alpha);

/************/
/* Parcours */
/************/

// Parcours DFS: retourne la liste triée des sommets accessibles depuis un sommet dans ini
// La lise ini est vidée (elle est utilisée comme pile)
// Le tableau "alph" contient un sous-alphabet (on peut l'utiliser pour restreindre les transitions autorisées)
// Si alph = NULL, toutes les lettres sont autorisées
// Le tableau "rest" permet de restreindre les sommets retournés
p_vertices graph_dfs(p_graph G, p_vertices ini, bool *rest);
p_vertices lgraph_dfs(p_lgraph G, p_vertices ini, bool *alph, bool *rest);
p_vertices dgraph_dfs(p_dgraph G, p_vertices ini, bool *alph, bool *rest);

// Parcours BFS: retourne la liste triée des sommets accessibles depuis un sommet dans ini
// La lise ini est vidée (elle est utilisée comme file)
// Le tableau "alph" contient un sous-alphabet (on peut l'utiliser pour restreindre les transitions autorisées)
// Si alph = NULL, toutes les lettres sont autorisées
// Le tableau "rest" permet de restreindre les sommets retournés
p_vertices graph_bfs(p_graph G, p_vertices ini, bool *rest);
p_vertices lgraph_bfs(p_lgraph G, p_vertices ini, bool *alph, bool *rest);
p_vertices dgraph_bfs(p_dgraph G, p_vertices ini, bool *alph, bool *rest);

/************************************/
/* Récupération des listes d'arêtes */
/************************************/

/* Version simple: arêtes indépendantes */

/* Une arête non étiquetée */
typedef struct
{
    uint in;
    uint out;
} edge;

/* Une arête-étiquetée */
typedef struct
{
    uint in;
    uint lab;
    uint out;
} ledge;

typedef edge *p_edge;
typedef ledge *p_ledge;

// Calcul
p_stack graph_to_edges(p_graph);
p_stack lgraph_to_ledges(p_lgraph);
p_stack dgraph_to_ledges(p_dgraph);

// Fonctions de comparaison (pour les tris)
// -1: >, 0 : ==, 1: <
int edge_comp(void *, void *);
int edge_comp_rev(void *, void *);
int l_edge_comp(void *, void *);
int l_edge_comp_rev(void *, void *);

/* Version fusion: pour l'affichage */

/* Une fusion d'arêtes étiquetées (pour l'affichage) */
typedef struct
{
    uint in;
    p_vertices lab;   // étiquettes standard
    p_vertices lab_i; // étiquettes inverses
    bool eps;         // epsilon
    uint out;
} multi_edge;

typedef multi_edge *p_multi_edge;

// Quatre modes:
// 0: only an lgraph with classic labels
// 1: an additional graph (eps trans)
// 2: an additional lgraph(inv trans)
// 3: an additional lgraph(inv trans) + an additional graph (eps trans)
// Les transitions epsilon self-loops ne sont pas ajoutées
p_stack lgraph_to_multi_edges(uint n, ...);
p_stack dgraph_to_multi_edges(p_dgraph);

/*******************************/
/* Fusion de plusieurs graphes */
/*******************************/

/* Versions disjointes (de nouveaux sommets sont créées) */

// Le paramètre int* lag set à retourner (par pointeur)
// les indices où commencent les copies de chaque graph
p_graph merge_graphs(uint *lag, uint n, p_graph *);

// Le paramètre int* lag set à retourner (par pointeur)
// les indices où commencent les copies de chaque graph
p_lgraph merge_lgraphs(uint *lag, uint n, p_lgraph *);

// Graphes étiqueté déterministe (le retour n'est plus déterministe)
// Le paramètre int* lag set à retourner (par pointeur)
// les indices où commencent les copies de chaque graph
p_lgraph merge_dgraphs(uint *lag, uint n, p_dgraph *list);

/* Fusion de plusieurs graphes ayant le même ensemble de sommets (partage des arêtes) */

// ng: nombre de graphes classiques dans la fusion
// nlg: nombre de graphes étiquetés dans la fusion
// ndg: nombre de graphes étiquetés déterministes dans la fusion
// n: le nombre total de paramètre (la somme des 3)
// Les graphes à fusionner
p_graph ldgraphs_to_graph(uint ng, uint nlg, uint ndg, uint n, ...);

// nlg: nombre de graphes étiquetés dans la fusion
// ndg: nombre de graphes étiquetés déterministes dans la fusion
// n: le nombre total de paramètre (la somme des 2)
// Les graphes à fusionner
p_lgraph ldgraphs_to_lgraph(uint nlg, uint ndg, uint n, ...);

/* Conversion d'un dgraph en lgraph */
p_lgraph dgraph_to_lgraph(p_dgraph);

/**************************************************************************************/
/* Récupération de tous les sommets adjacents à un ensemble de sommets dans un lgraph */
/**************************************************************************************/

// Retourne tous les sommets adjacents à un sommet dans start par la lettre a
p_vertices lgraph_reachable(p_lgraph G, p_vertices start, uint a);

#endif
