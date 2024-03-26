/********************************************/
/* Determinisation et Minimisation des NFAs */
/********************************************/

#ifndef NFA_DETERMI_H
#define NFA_DETERMI_H

#include "nfa.h"

/************************/
/* Procédure principale */
/************************/

// Procédure de déterminisation. Le Booléen names indique si les noms des nouveaux états
// (des ensembles d'états de l'ancien NFA) doivent être sauvegardés dans le DFA.
p_nfa nfa_determinize(p_nfa A, bool names);

/********************************************/
/* Procédures basées sur la determinisation */
/********************************************/

// Minimisation (Brzozowski)
p_nfa nfa_minimize(p_nfa A);

// Complementation
p_nfa nfa_complement(p_nfa A);

#endif
