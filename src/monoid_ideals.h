/*********************/
/* Ideals of monoids */
/*********************/

#ifndef IDEALS_H
#define IDEALS_H

#include <stdbool.h>
#include "monoid.h"
#include "monoid_green.h"

/**************************/
/* Computations of ideals */
/**************************/

// Calcul d'un idéal droit (la liste triée des éléments de l'ensemble sM)
p_vertices compute_r_ideal(p_cayley M, uint s, bool* rest);

// Calcul d'un idéal gauche (la liste triée des éléments de l'ensemble Ms)
p_vertices compute_l_ideal(p_cayley M, uint s, bool* rest);

// Calcul d'un idéal droit restreint par l'alphabet.
p_vertices compute_r_ideal_alph(p_cayley M, uint s, bool* alph, bool* rest);

// Calcul d'un idéal gauche restreint par l'alphabet.
p_vertices compute_l_ideal_alph(p_cayley M, uint s, bool* alph, bool* rest);

/***********************/
/* Gestion des orbites */
/***********************/

typedef struct
{
    // Un Booléen qui permet de marquer le cas où toutes les orbites sont des sous-semigroupes
    // de celle de 1 qui est égale à sub. Dans ce cas, on n'enregistre pas les orbites: orbits == NULL
    bool oneonly;

    // Le sous-monoide à l'intérieur duquel les orbites sont calculées
    p_green_sub sub;

    // Nombre d'idempotents
    uint nb_idems;

    // Tableau des orbites
    p_green_sub* orbits;

} orbits;

typedef orbits* p_orbits;

// Intialise un ensemble d'orbites
// Après initialisation, on est dans le cas où l'orbite de 1 est tout le sous-monoide
p_orbits init_orbits(p_green_sub S);

// Suppression (le Booléen indque si on doit aussi supprimer sub)
void delete_orbits(p_orbits L, bool delsub);

// Affichage d'un ensemble d'orbites
void print_all_orbs(p_orbits L, char* namecl, FILE* out);

void print_one_orb(p_orbits L, char* namecl, uint idem_num, FILE* out);

/**************/
/* DD-orbites */
/**************/

// Calcule la DD-orbite associée à l'idempotent e (monoide local)
p_green_sub compute_one_ddorb(p_cayley M, p_green G, uint e);

// Calcule toutes les DD-orbites associées
p_orbits compute_all_ddorbs(p_cayley M, p_green G);

// Affiche toutes les DD-orbites
void print_all_ddorbs(p_orbits L, bool title, FILE* out);

/********************************/
/* Liens entre deux idempotents */
/********************************/

// Pour deux idempotents e,f calcule tous le sous-ensemble eMf
// Marque simultanément les éléments dans eM et Mf si les pointeurs sont non-nuls
p_vertices compute_ef_links(p_cayley M, uint e, uint f);

#endif
