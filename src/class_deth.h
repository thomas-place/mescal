/*****************************/
/* Deterministic hierarchies */
/*****************************/

#ifndef DETH_H
#define DETH_H

#include <stdbool.h>
#include <stdlib.h>
#include "class_group.h"
#include "class_temporal.h"
#include "nfa.h"
#include "nfa_intersec.h"
#include "monoid.h"
#include "monoid_green.h"
#include "monoid_ideals.h"

/*************************/
/* Calcul de congruences */
/*************************/

// Calcule de la plus petit congruence contenant une partition quelconque (donnée sous la forme d'un union-find)
void compute_leastcong(p_cayley M, p_ufind uf);

/****************************************/
/* Partitions générées par une équation */
/****************************************/

// Retourne l'équivalence obtenue en idientifiant les éléments qui devraient être égaux d'après l'équation de BPol(MOD)
// A combiner avec compute_leastcong pour avoir la congruence canonique de BPol(DD) sur le monoide
p_ufind iden_bpolmod_mono(p_cayley M, p_green G);

// Retourne l'équivalence obtenue en idientifiant les éléments qui devraient être égaux d'après l'équation de BPol(GR)
// A combiner avec compute_leastcong pour avoir la congruence canonique de BPol(DD) sur le monoide
p_ufind iden_blockg_mono(p_cayley M, p_green G);

// Retourne l'équivalence obtenue en idientifiant les éléments qui devraient être égaux d'après l'équation de Knast
// A combiner avec compute_leastcong pour avoir la congruence canonique de BPol(DD) sur le monoide
p_ufind iden_knast_mono(p_cayley M, p_green G);

// Retourne l'équivalence obtenue en idientifiant les éléments qui devraient être égaux d'après l'équation de BPol(MOD⁺)
// A combiner avec compute_leastcong pour avoir la congruence canonique de BPol(DD) sur le monoide
p_ufind iden_qknast_mono(p_cayley M, p_green G, p_green_sub mker);

// Retourne l'équivalence obtenue en identifiant qui devraient être égaux selon l'équation de MPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de MPol(C) sur le monoide
// La congruence C canonique est passée en entrée
// Ne fonctionne que si le morphisme est dans UPol(C)
p_ufind iden_mpolc_mono(p_cayley M, p_parti C);

// Retourne l'équivalence obtenue en identifiant qui devraient être égaux selon l'équation de LPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de LPol(C) sur le monoide
// La congruence C canonique est passée en entrée
// Ne fonctionne que si le morphisme est dans UPol(C)
p_ufind iden_lpolc_mono(p_cayley M, p_parti C);

// Retourne l'équivalence obtenue en identifiant qui devraient être égaux selon l'équation de RPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de RPol(C) sur le monoide
// La congruence C canonique est passée en entrée
// Ne fonctionne que si le morphisme est dans UPol(C)
p_ufind iden_rpolc_mono(p_cayley M, p_parti C);

/**********************************************************************************/
/* Calcul du niveau dans une hiérarchie de négations en logique temporelle unaire */
/**********************************************************************************/

// Calcul du niveau dans la hiéarchie MPol de base C: 1 = C, 2 = MPol(C),...
short hdet_mpol_level(p_cayley M, p_ufind C, FILE *out);

// Calcul du niveau dans les hiéarchies LPol/RPol de base C: 1 = RPol(C),LPol(C) 2 = RPol₂(C),LPol₂(C)...
void hdet_lrpol_level(p_cayley M, p_ufind C, FILE *out, short *minf, short *minp);

#endif
