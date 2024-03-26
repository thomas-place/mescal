/******************************************/
/* Fonctions sur les langages sans-étoile */
/******************************************/

#ifndef STARFREE_H
#define STARFREE_H

#include <stdbool.h>
#include <stdlib.h>
#include "nfa.h"
#include "monoid.h"
#include "monoid_green.h"

/**************/
/* Membership */
/**************/

// Vérifie si un monoide est apériodique.
// En cas de réponse négative, un contre-exemple sera affiché dans la sortie prise en entrée (si non-NULL)
bool is_aperiodic_mono(p_cayley, p_green, FILE*);

// version sous-monoide
bool is_aperiodic_submono(p_green_sub, char*, FILE*);

// Vérifie si un DFA est counter-free
// En cas de réponse négative, un compteur sera affiché dans la sortie prise en entrée (si non-NULL)
bool is_counterfree_dfa(p_nfa, FILE*);

#endif
