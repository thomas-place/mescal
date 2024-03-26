/****************/
/* Type monoide */
/****************/

#ifndef MONOID
#define MONOID

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "tools.h"
#include "type_basic.h"
#include "type_stack.h"
#include "type_binheap.h"
#include "type_dequeue.h"
#include "type_abr.h"
#include "graphs.h"
#include "nfa.h"

// 0 nous servira d'élément neutre
#define ONE 0

// Représentation d'un morphisme dans une monoide sous la forme d'un graph de Cayley
typedef struct {
    /* Champs obligatoires (toujours calculé en même temps que le cayley graph) */

    // Le graphe (Right cayley graph)
    p_dgraph trans;

    // Liste des idempotents (triée par ordre croissant)
    p_vertices idem_list;

    // Tableau de Booléens qui marque les idempotents
    bool* idem_array;

    // La liste des éléments de l'ensemble acceptant
    p_vertices accept_list;

    // Tableau de Booléens qui marque les élements acceptant
    bool* accept_array;

    // Les noms des éléments
    p_vertices* names;

    // Éventuelle lettre neutre (0 si il n'y a pas de telle lettre)
    char neutlet;

    /* Champs optionnels (NULL si pas encore calculés) */

    // Le graph symmétrique (Left cayley graph)
    p_dgraph leftgraph;

    // La table de multiplication
    uint** mult;

    // L'ordre (NULL si pas calculé)
    // Le tableau est de la taille du monoide
    // order[q] contient la liste des éléments plus grands que q
    p_vertices* order;
} cayley;

typedef cayley* p_cayley;

// Taille maximale d'un nom (pour l'affichage)
#define NAMELEN 5000

/*****************************************/
/* Fonctions génériques pour l'affichage */
/*****************************************/

// Retourne une chaîne de caractères correspondant au nom d'un élément
char* cayley_get_name(p_cayley M, uint elem);

// // Retourne une chaîne de caractères correspondant au nom d'un élément (factorisation des puissances)
// char* cayley_get_name_short(p_cayley M, uint elem);

// Affichage du nom de l'élément q (retourne la longueur utilisée)
uint cayley_print_name(p_cayley M, uint elem, FILE* out);

// Affichage du nom de l'élément q, version alignée
// On utilise exactement size caractère en ajoutant des espaces (size doit être suffisament grand)
void cayley_print_name_aligned(p_cayley M, uint elem, uint size, FILE* out);

// Récupération de la longueur maximale d'un nom parmi un sous-ensemble (si le deuxième argument est NULL,
// la fonction s'applique à tout les éléments du monoide)
uint cayley_max_name_size(p_cayley M, p_vertices sub);

// Affichage d'un sous-ensemble d'éléments dans un cayley graph
void print_sub_cayley(p_cayley, p_vertices, FILE*);

// Affichage d'un sous-ensemble d'éléments dans un cayley graph dans une boite partielle
// L'alignement est automatique
void print_sub_cayley_aligned(p_cayley, p_vertices, uint length, uint padding, FILE*);

// Affichage d'un sous-ensemble d'éléments dans un cayley graph dans une boite complète
// avec un titre (utilise la fonction précédente)
void print_sub_cayley_titled(p_cayley, p_vertices, uint length, char* name, FILE*);

// Affichage des images de toutes les lettres
void cayley_print_morphism(p_cayley, FILE*);






/****************/
/* Construction */
/****************/

// Ne fonctionne qu'avec un DFA complet.
p_cayley dfa_to_right_cayley(p_nfa A);

// Conversion d'un cayley en DFA
p_nfa cayley_to_dfa(p_cayley);

// Conversion d'un cayley gauche en DFA
p_nfa left_cayley_to_dfa(p_cayley);

// Suppression d'un cayley
void free_cayley(p_cayley);

/************************************************/
/* Calculs des informations sur un Cayley graph */
/************************************************/

// Calcule le graph de Cayley gauche
void compute_left_cayley(p_cayley);

// Calcule la table de multiplication d'un Cayley graph
void compute_mult(p_cayley);

// Calcule le mirroir du graphe de Cayley droit
p_lgraph cayley_rmirror(p_cayley);

// Calcule le mirroir du graphe de Cayley gauche
p_lgraph cayley_lmirror(p_cayley);

// Calcule le l'ordre syntaxique complet (quadratique)
void compute_syntac_order(p_cayley);

// Affichage des idempotents
void cayley_print_idems(p_cayley M, FILE* out);

// Affichage de l'ordre syntaxique
void cayley_print_order(p_cayley M, FILE* out);


// Print de la table de multiplication
void cayley_mult_print(p_cayley M, FILE* out);

/************************************/
/* Opérations sur les Cayley graphs */
/************************************/

// Multiplication de deux élément d'un monoide représenté par un Cayley graph
// Plus efficace si la table de multiplication a déjà été calculée
uint cayley_mult(p_cayley thegraph, uint s, uint t);

// Multiplication de n éléments
uint cayley_mult_gen(p_cayley thegraph, uint n, ...);

// Calcul de la puissance omega d'un élément dans un monoide représenté par un Cayley graph
// Ne fonctionne que avec un RIGHT Cayley graph
uint cayley_omega(p_cayley thegraph, uint s);

//
bool cayley_elem_from_string(p_cayley M, char* w, uint* res);

// Image d'un mot passé sous ma forme d'une chaîne de caractère
void cayley_print_image(p_cayley, char* w, FILE* out);

//
bool cayley_num_idem(p_cayley M, uint s, uint* res);

/**********************************/
/* Tests de propriétés classiques */
/**********************************/

// Idempotence
bool is_idem_mono(p_cayley, FILE* out);

// Commutativité
bool is_comm_mono(p_cayley, FILE* out);

#endif
