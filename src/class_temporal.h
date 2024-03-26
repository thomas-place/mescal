/*********************************************/
/* Fonctions sur les classes associées à UTL */
/*********************************************/

#ifndef CLASS_TEMPORAL_H
#define CLASS_TEMPORAL_H

#include <stdbool.h>
#include <stdlib.h>
#include "nfa.h"
#include "monoid.h"
#include "monoid_green.h"
#include "monoid_ideals.h"

/******/
/* TL */
/******/

// DA
bool is_da_mono(p_cayley, p_green, FILE*);
bool is_da_submono(p_green_sub, char* name, FILE*);
bool is_da_orbmono(p_orbits, char* class, FILE*);
bool is_da_orbmono_opti(p_orbits, char* class, FILE*);

/*********/
/* FL-PL */
/*********/

// L-triviality
bool is_ltrivial_mono(p_cayley, p_green, FILE* out);
bool is_ltrivial_submono(p_green_sub, char* name, FILE* out);
bool is_ltrivial_orbmono(p_orbits, char* class, FILE* out);
bool is_ltrivial_orbmono_opti(p_orbits, char* class, FILE* out);

// R-triviality
bool is_rtrivial_mono(p_cayley, p_green, FILE* out);
bool is_rtrivial_submono(p_green_sub, char*, FILE* out);
bool is_rtrivial_orbmono(p_orbits, char* class, FILE* out);
bool is_rtrivial_orbmono_opti(p_orbits, char* class, FILE* out);

#endif
