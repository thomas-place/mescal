/*************************/
/* Les classes at et wat */
/*************************/

#ifndef AT_H
#define AT_H

#include "nfa.h"
#include "monoid.h"
#include "monoid_ideals.h"
#include "type_basic.h"
#include "class_starfree.h"

/**************/
/* Membership */
/**************/

bool is_at_mono(p_cayley, FILE* out);
bool is_lt_mono(p_cayley, p_green, p_orbits, FILE* out);

bool is_att_mono(p_cayley, p_green, FILE* out);
bool is_ltt_mono(p_cayley, p_green, FILE* out);

/*************/
/* AT-orbits */
/*************/

// Calcule l'ensemble des lettres qui étiquettent des transitions de la SCC de e
// Retour sous la forme d'un tableau de Booléens
bool* compute_maxalph_scc(p_cayley M, p_parti SCCS, uint e);

// Calcule l'alphabet commun maximum des idempotents e et f
// Le retour se fait sous la forme d'un tableau de Booléens
// Si il n'y a pas d'alphabet commun, on retourne NULL
bool* compute_maxalph_com_scc(p_cayley M, p_parti SCCS, uint e, uint f);

// Calcule la AT-orbite associée à l'idempotent e
// On doit fournir la partition en SCCS du graphe de Cayley droit
p_green_sub compute_one_atorb(p_cayley M, p_green G, p_parti SCCS, uint e);

// Calcule toutes les AT-orbites associées
p_orbits compute_all_atorbs(p_cayley M, p_green G);

// Affiche toutes les orbites enregistrées
void print_all_atorbs(p_cayley M, p_orbits L, bool title, FILE* out);

/************/
/* Covering */
/************/

// Récupération des éléments intersectant chaque alphabet dans un tableau
p_vertices* at_table_cayley(p_cayley);

// Affichage du tableau
void print_at_table(p_cayley M, p_vertices* table, FILE* out);

#endif
