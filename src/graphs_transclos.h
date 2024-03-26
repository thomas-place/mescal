/*********************************/
/* Transitive closure procedures */
/*********************************/

#ifndef TRANSCLOS_H
#define TRANSCLOS_H

#include <stdbool.h>
#include "type_vertices.h"
#include "type_unionfind.h"
#include "type_stack.h"
#include "graphs.h"
#include "graphs_tarjan.h"
#include "type_binheap.h"

/**************************/
/* Fonctions sur les DAGs */
/**************************/

// Prend un graphe quelconque et la liste de ses SCCs en entrée (déjà calculée)
// Retourne le DAG obtenu en prenant chaque SCC comme sommet
p_graph compute_dag_of_sccs(p_graph, p_parti clist);

// Prend un DAG en entrée (ne termine pas sur un graphe quelconque )
// Calcule un tri topologique de ses sommets
p_vertices topo_sort_dag(p_graph);

// Version qui ne garde que les sommets accessible depuis un sommet désigné.
p_vertices topo_sort_dag_start(p_graph, uint);

// Calcule la clôture transitive d'un DAG
// Requiert une liste des sommets triée selon un tri topologique
p_graph compute_tclos_dag(p_graph G, p_vertices toposort);

/*****************************************/
/* Fonctions sur les graphes quelconques */
/*****************************************/

// Prend un graphe quelconque en entrée et l'étend en faisant sa clôture transitive
void make_tclos_graph(p_graph G);

#endif
