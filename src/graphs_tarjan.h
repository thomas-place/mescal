/************************/
/* Algorithme de Tarjan */
/************************/

#ifndef TARJAN_H_
#define TARJAN_H_

#include "type_vertices.h"
#include "type_unionfind.h"
#include "type_stack.h"
#include "graphs.h"
#include <stdbool.h>

/*********************************/
/* Représentation d'un partition */
/*********************************/

typedef struct
{
    uint size_set;  // Taille de l'ensemble partitionné
    uint size_par;  // Taille de la partition
    uint *numcl;    // Tableau de taille size_set, stocke les numéros (entre 0 et size_par-1) des classes de chaque élément
    p_vertices *cl; // Tableau de taille size_par, la liste des éléments de la classe c est à l'indice c
} parti;

typedef parti *p_parti;

/*******************************/
/* Manipulation des partitions */
/*******************************/

// Fonction de désallocation
void delete_parti(p_parti p);

// Conversion en Union-Find
p_ufind parti_to_ufind(p_parti p);

// Construction depuis Union-Find
p_parti ufind_to_parti(p_ufind);
p_parti ufind_to_parti_refined(p_ufind, p_parti);

// Teste si la partition est triviale (les classes sont toutes des singletons)
bool istrivial_parti(p_parti p);

/*******************************************/
/* Algorithme de Tarjan: version classique */
/*******************************************/

void scc(p_graph g, uint v, int *index, int *id, int *lowlink, int *stack, int *top, bool *onStack, uint *ncount, uint *nums);
void lscc(p_lgraph g, uint v, int *index, int *id, int *lowlink, int *stack, int *top, bool *onStack, uint *ncount, uint *nums);
void dscc(p_dgraph g, uint v, int *index, int *id, int *lowlink, int *stack, int *top, bool *onStack, uint *ncount, uint *nums);

// Les sccs sont ordonnées selon un tri topologique du DAG des sccs
p_parti tarjan(p_graph g);
p_parti ltarjan(p_lgraph g);
p_parti dtarjan(p_dgraph g);

/***************************************************************/
/* Algorithme de Tarjan: version restreinte à un sous-alphabet */
/***************************************************************/

void lscc_alph(p_lgraph g, uint v, int *index, int *id, int *lowlink, int *stack, int *top, bool *onStack, uint *ncount, uint *nums, bool *alph);
void dscc_alph(p_dgraph g, uint v, int *index, int *id, int *lowlink, int *stack, int *top, bool *onStack, uint *ncount, uint *nums, bool *alph);

// Les sccs sont ordonnées selon un tri topologique du DAG des sccs
p_parti ltarjan_alph(p_lgraph g, bool *alph);
p_parti dtarjan_alph(p_dgraph g, bool *alph);

#endif // TARJAN_H_
