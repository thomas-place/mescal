#ifndef PRINTING_H
#define PRINTING_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "tools.h"
#include "monoid.h"
#include "type_dequeue.h"
#include "nfa.h"
#include "alloc.h"
#include "type_basic.h"

/**********************/
/* Printing functions */
/**********************/

// Print d'un sommet, version noms contenus dans un deq
void list_print_state(void* p, FILE* out);

/********************************************************/
/*+ Récupération d'une liste d'arêtes pour l'affichage +*/
/********************************************************/


/**
 * @brief
 * Type utilisé pour représenter une multi-arête pour l'affichage.
 *
 * @remark
 * Une multi-arête est une arête qui peut avoir plusieurs étiquettes. On se sert de
 * ce type pour l'affichage : quand deux sommets sont reliés par plusieurs arêtes, on
 * simplifie la présentation en ne dessinant qu'une seule arête qui porte toutes les
 * étiquettes des vraies arêtes.
 */
typedef struct {
    uint in;                          //!< État de départ de l'arête.
    bool eps;                         //!< Epsilon transition
    dequeue* lab;                     //!< Liste triée des étiquettes.
    dequeue* lab_i;                   //!< Liste triée des étiquettes inverses.
    uint out;                         //!< État de sortie de l'arête
} multi_edge;

/**
 * @brief
 * Calcul de la liste des mumti-arêtes d'un automate.
 *
 * @return
 * La liste des multi-arêtes stockée dans une pile.
 */
dequeue_gen* nfa_to_multi_edges(nfa* //!< Le NFA.
);

// Quatre modes:
// 0: only an lgraph with classic labels
// 1: an additional graph (eps trans)
// 2: an additional lgraph(inv trans)
// 3: an additional lgraph(inv trans) + an additional graph (eps trans)
// Les transitions epsilon self-loops ne sont pas ajoutées
//dequeue_gen* dgraph_to_multi_edges(dgraph*);

dequeue_gen* dgraph_to_multi_edges(dgraph*);

/********************************/
/* Print des arêtes d'un graphe */
/********************************/

void named_lgedges_print(dequeue_gen* theedges, nfa* A, FILE* out);

void named_dedges_print(dequeue_gen* theedges, morphism* M, FILE* out);
/******************/
/* Print d'un NFA */
/******************/

void nfa_print(nfa* A, FILE* out);

// Print du graphe droit
void cayley_print(morphism* mor, FILE* out);

// Print du graphe gauche
void cayley_left_print(morphism* mor, FILE* out);

/**************************/
/* Affichage sur le shell */
/**************************/

// Affichage d'un NFA
void view_nfa(nfa* nfa);

// Affichage du Cayley droit
void view_cayley(morphism* mor);

// Affichage du Cayley gauche
void view_left_cayley(morphism* mor);

#endif
