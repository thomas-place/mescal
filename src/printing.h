#ifndef PRINTING_H
#define PRINTING_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "type_vertices.h"
#include "nfa.h"
#include "monoid.h"
#include "macros_alloc.h"
#include "type_basic.h"

/**********************/
/* Printing functions */
/**********************/

// Print d'un sommet, version noms contenus dans un deq
void list_print_state(void* p, FILE* out);

// Print d'un sommet, version cayley graph
void cayley_print_state(void* p, FILE* out);

/********************************/
/* Print des arêtes d'un graphe */
/********************************/

void simple_gedges_print(p_graph G, FILE* out);
void simple_lgedges_print(p_stack theedges, FILE* out);
void named_lgedges_print(p_stack theedges, p_nfa A, FILE* out);

void named_gedges_print(p_graph G, void** names, void (*f)(void*, FILE*), FILE* out);
void named_dgedges_print(p_dgraph G, void** names, void (*f)(void*, FILE*), FILE* out);

/******************/
/* Print d'un NFA */
/******************/

void nfa_print(p_nfa A, FILE* out);

// void simple_nfa_print(p_nfa A, FILE *out);

// void named_nfa_print(p_nfa A, void (*f)(void *, FILE *), FILE *out);

/***************************/
/* Print d'un Cayley graph */
/***************************/

// Print du graphe droit
void cayley_print(p_cayley, FILE* out);

// Print du graphe gauche
void cayley_left_print(p_cayley, FILE* out);


/**************************/
/* Affichage sur le shell */
/**************************/

// Affichage d'un NFA
void view_nfa(nfa* nfa);

// Affichage du Cayley droit
void view_cayley(cayley* cayley);

// Affichage du Cayley gauche
void view_left_cayley(cayley* cayley);

// // Print d'un sommet, version noms contenus dans un deq
// void deq_print_state(p_prenfa A, int i, FILE *out);

// // Print d'un sommet, version cayley graph
// void cayley_print_state(p_prenfa A, int i, FILE *out);

// // Affichage d'un prenfa
// void prenfa_print(p_prenfa A, FILE *out, void (*f)(p_prenfa, int, FILE *));

// // Affichage d'un nfa
// void nfa_print(p_nfa A, FILE *out, void (*f)(p_prenfa, int, FILE *));

#endif
