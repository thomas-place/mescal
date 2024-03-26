/************************/
/* Intersection of NFAs */
/************************/

#ifndef NFA_INTERSEC_H
#define NFA_INTERSEC_H

#include "nfa.h"

/************************************************************************************************/
/* Construction classique: calcul de l'automate produit en ne gardant que les états accessibles */
/************************************************************************************************/

// Intersection (le Booléen indique si on doit mémoriser les noms)
p_nfa nfa_intersect(p_nfa A1, p_nfa A2, bool names);

#endif
