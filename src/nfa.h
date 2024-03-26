/***************************/
/* Implémentation des NFAs */
/***************************/

#ifndef NFA_H
#define NFA_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "type_basic.h"
#include "graphs.h"
#include "type_vertices.h"
#include "type_stack.h"
#include "type_binheap.h"
#include "type_abr.h"
#include "type_unionfind.h"
#include "graphs_tarjan.h"
#include "graphs_transclos.h"

/* Types de noms pour les états */

typedef enum
{
    NONAME,
    NUMBER,
    PAIR,
    TUPLE,
    SET
} state_name;

/* Le type */

typedef struct
{
    /* Champs obligatoires */

    // Graphe des transitions classiques
    p_lgraph trans;

    // Liste triée des états initiaux
    p_vertices initials;

    // Liste triée des états finaux
    p_vertices finals;

    /* Champs optionels (NULL si non-utilisés) */

    // Graphe des transitions inverses
    p_lgraph itrans;

    // Graphe des transitions epsilon
    p_graph etrans;

    /* Noms des états (NULL si non-utilisé) */

    // Type de nom utilisé
    state_name ntype;

    // Tableau des noms.
    void* names;

} nfa;



typedef nfa* p_nfa;

/***************************************************************/
/* Affichage des noms si ce sont des pointeurs sur des entiers */
/***************************************************************/

// Affiche le nom du ième état d'un NFA
void nfa_print_name(p_nfa A, uint i, FILE* out);

// Initialise la table des noms suivant le type choisit dans le NFA
void nfa_init_names(p_nfa A);

// Copie le nom du jème état du NFA B dans le ième état du NFA A
// Les tables doivent avoit été initialisées et les types de noms
// doivent être compatibles entre A et B
void nfa_copy_name(p_nfa A, uint i, p_nfa B, uint j);

// Efface les noms des états d'un NFA
// (si ces états ont des noms)
void nfa_delete_names(p_nfa A);

/*********************************/
/* Initialisation et suppression */
/*********************************/

// Initialise (si besoin) le graphe des transitions epsilon
void init_epstrans(p_nfa A);

// Crée un NFA qui reconnait le langage vide
p_nfa create_emptylang(uint size_alpha);

// Créé un NFA qui reconnait le singleton {epsilon}
p_nfa create_sing_epsilon(uint size_alpha);

// Créé un NFA qui reconnait le langage A^* de tous les mots
p_nfa create_freemonolang(uint size_alpha);

// Créé un NFA qui reconnait le langage A^+ de tous les mots non-vides
p_nfa create_freesemilang(uint size_alpha);

// Création d'un automate qui reconnait un singleton dont le mot contient une seule lettre
p_nfa create_sing_letter(uint size_alpha, uint letter);

// Création d'un automate qui reconnait un singleton
p_nfa create_sing(uint size_alpha, uint* word, uint length);

// Désallocation d'un NFA.
void delete_nfa(p_nfa A);

// Recopie du NFA B sur le NFA A (les anciens éléments de A sont supprimés, B est libéré)
void overwrite_nfa(p_nfa A, p_nfa B);

// Génération d'un NFA aléatoire
p_nfa nfa_random(uint size_alpha, uint min_size, uint max_size);

// Génération d'un DFA aléatoire
p_nfa dfa_random(uint size_alpha, uint min_size, uint max_size);

/***********************************/
/* Opérations simples sur les NFAs */
/***********************************/

// Copie d'un NFA
p_nfa nfa_copy(p_nfa A);

// Union disjointe d'un nombre arbitraire de NFAs
p_nfa nfa_union_gen(uint n, ...);

// Union disjointe de deux nfas
p_nfa nfa_union(p_nfa A, p_nfa B);

// Concaténation d'un nombre arbitraire de NFAs
p_nfa nfa_concat_gen(uint n, ...);

// Concaténation de deux nfas
p_nfa nfa_concat(p_nfa A, p_nfa B);

// Etoile de Kleene d'un nfa
p_nfa nfa_star(p_nfa A);

// Etoile de Kleene stricte (+) d'un nfa
p_nfa nfa_star_plus(p_nfa A);

// Élimination des états non-accessibles
p_nfa nfa_trim(p_nfa A);

// Élimination des états non-accessibles (modifie le NFA originel)
void nfa_trim_mod(p_nfa A);

// Élimination des transitions epsilon
p_nfa nfa_elimeps(p_nfa A);

// Élimination des transitions epsilon (modifie le NFA originel)
void nfa_elimeps_mod(p_nfa A);

// Miroir
p_nfa nfa_mirror(p_nfa A);

/***********************/
/* Information sur nfa */
/***********************/

// Calcule le nombre de transitions dans un automate
int nfa_nb_trans(p_nfa A);

// Teste si un nfa est déterministe
bool nfa_is_det(p_nfa A);

// Teste si un nfa est complet
bool nfa_is_comp(p_nfa A);

// Teste si un nfa reconnait le langage vide
bool nfa_is_empty(p_nfa A);

// Teste si un mot est accepté par un NFA
// Le NFA doit être sans transitions epsilon et inverses
bool nfa_accepts(p_nfa A, char* word);

// Calcule les états qui sont atteints par un mot dans un NFA.
p_vertices nfa_compute_runs(p_nfa A, char* word);


// Retourne l'état atteint à partir de s en lisant le mot w dans le DFA A
// (le déterminisme n'est pas vérifier)
uint dfa_function(p_nfa A, uint s, p_vertices w);

// Teste si un DFA est commutatif
bool dfa_is_comm(p_nfa, FILE*);


/***********************/
/* Partition d'un NFA  */
/***********************/

// Fusion des états d'un NFA selon une partition prise en entrée
// Les nouveaux états sont les classes
// On a une transition (c,a,c') si les classes c,c' contiennent des états q,q'
// tels que l'automate d'origine avait une transition (q,a,q')
// De la même façon, une classe est c est initiale (resp. finale) si elle contient
// un état initial (resp. final).
p_nfa nfa_merge_states(p_nfa, p_parti);

// Récupération du graphe étiqueté obtenu en ne gardant que
// les transitions internes à une partition prise en entrée
// (typiquement utilisé pour la partition en SCCs)
p_lgraph nfa_get_lab_parti(p_nfa A, p_parti P);

// Génération d'un nfa à partir d'un lgraph et d'un unique état initial
// Seuls les états accessibles sont conservés. Si l'automate n'est pas
// complet, celui-ci est complété en ajoutant un état supplémentaire.
p_nfa lgraph_to_nfa(p_lgraph P, uint ini);

#endif
