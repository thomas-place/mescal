/*****************************/
/* Fonctions sur les groupes */
/*****************************/

#ifndef GROUP_H
#define GROUP_H

#include <stdbool.h>
#include <stdlib.h>
#include "nfa.h"
#include "monoid.h"
#include "monoid_green.h"
#include "monoid_ideals.h"
#include "printing.h"
#include "graphs_tarjan.h"
#include "nfa_intersec.h"
#include "type_unionfind.h"
#include "type_dlist.h"
#include "shell_languages.h"

/**************/
/* Membership */
/**************/

// Langages de groupes

// Vérifie si un monoide est un groupe.
// Affiche un contre-exemple sur la sortie si celle-ci est non-NULL
bool is_group_mono(p_cayley, p_green, FILE*);

// Vérifie si un DFA est un automate de permutation
// Affiche un contre-exemple sur la sortie si celle-ci est non-NULL
bool is_permutation_dfa(p_nfa, FILE*);

// Langages modulo

// Vérifie si un morphisme envoie toutes les lettres sur le même élément
bool is_letterind_mono(p_cayley, FILE*);

// Vérifie si toutes les lettres ont la même action dans un DFA
bool is_letterind_dfa(p_nfa, FILE*);

// Langages triviaux

// Vérifie si un monoide est trivial
bool is_trivial_monoid(p_cayley, FILE*);

// Vérifie si un DFA complet est trivial
bool is_trivial_dfa(p_nfa, FILE*);

/*********************************************/
/* Construction des les transitions inverses */
/*********************************************/

// Construction des transitions inverses à l'intérieur des SCCs. Si
// il y a déjà des transitions inverses, elles seront supprimées et
// recalculées.
// Les transitions epsilon sont éliminées avant le calcul.
// La partition en SCCS utilisée pour le calcul est retournée (elle
// est souvent utile pour les calculs suivants).
p_parti nfa_inv_ext(p_nfa);

// Suppression des transitions inverses
void nfa_remove_inv(p_nfa);

/*******************************************/
/* Séparation pour les langages de groupes */
/*******************************************/

// Calcul de la partition obtenue en effectuant le folding à l'intérieur
// des SCCS. Les transitions inverses doivent déjà avoir été calculées et
// la partition prise en entrée doit être celle des SCCS.
p_parti nfa_stal_fold(p_nfa, p_parti);

// Calcul du NFA obtenu en effectuant le folding et en ajoutant ensuite
// des transitions espilon entre les paires d'états liées par un mot de
// Dyck.
// Les transitions inverses doivent déjà avoir été calculées et on doit
// fournir la partition correspondant au folding.
p_nfa nfa_dyck_ext(p_nfa, p_parti FOLD);

// Calcul du GR-kernel d'un morphisme
p_green_sub get_grp_kernel(p_cayley, p_green);

// Procédure de séparation complète
bool decid_grp_sep(p_nfa I1, p_nfa I2, bool details, FILE* out);

/***************************************/
/* Séparation pour les langages modulo */
/***************************************/

// Projection d'un NFA sur un alphabet à une lettre (on
// ramène la MOD-separation à la GR-separation).
p_nfa nfa_proj_unary(p_nfa);

// Calcul du MOD-kernel d'un morphisme
p_green_sub get_mod_kernel(p_cayley, p_green);

// Calcul d'une MODPlus-orbite d'un morphisme (le MOD-kernel doit être passé en entrée)
p_green_sub get_one_modporb(p_green_sub, uint);

// Calcul de toutes les  MODPlus-orbites d'un morphisme (le MOD-kernel doit être passé en entrée)
p_orbits get_all_modporbs(p_green_sub);

// Affiche toutes les MODPlus-orbites
void print_all_modporbs(p_orbits L, bool title, FILE* out);

// Procédure de séparation complète
bool decid_mod_sep(p_nfa I1, p_nfa I2, bool details, FILE* out);

/*****************************************/
/* Séparation pour les langages triviaux */
/*****************************************/

// Commence par éliminer les éventuelles transitions epsilon.
// Puis, double toute transition existante d'une transition epsilon,
// dans un nouveau NFA.
p_nfa nfa_add_eps(p_nfa A);

// Procédure de séparation complète
bool decid_st_sep(p_nfa I1, p_nfa I2, FILE* out);

#endif
