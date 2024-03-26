/**************************************************/
/* Fonctions sur les hiérarchies de concatenation */
/**************************************************/

#ifndef CONCATHIERA_H
#define CONCATHIERA_H

#include <stdbool.h>
#include <stdlib.h>
#include "class_group.h"
#include "nfa.h"
#include "nfa_intersec.h"
#include "monoid.h"
#include "printing.h"
#include "class_at.h"

/********************/
/* Membership - Pol */
/********************/

// Equation 1 ≤ s (Pol(ST))
bool is_jsat_mono(p_cayley, FILE*);

// Equation 1 ≤ e (Pol(GR))
bool is_ejsat_mono(p_cayley, FILE*);

// Equation 1 ≤ s sur un sous-monoide (utilisé pour Pol(MOD) sur le MOD-kernel)
bool is_jsat_submono(p_green_sub, char* name, FILE*);

// Equation 1 ≤ s pour un ensemble de C-orbites
// (la chaîne de caractère sert à afficher le nom de la classe C)
bool is_jsat_orbmono(p_orbits L, char* class, FILE* out);

/*************************/
/* Membership - UPolBPol */
/*************************/

// Membership in UPol(BPol(C)) (les C-orbites du monoide doivent être passées en entrée)
bool is_upbp_mono(p_orbits L, char* class, FILE* out);

/*********************/
/* Membership - BPol */
/*********************/

// J-triviality (PT)
bool is_jtrivial_mono(p_cayley, p_green, FILE* out);
bool is_jtrivial_submono(p_cayley M, p_green_sub S, FILE* out);

// Block-group
bool is_blockg_mono(p_cayley, p_green, FILE* out);

// Mod
bool is_bpolmod_mono(p_cayley, p_green, FILE* out);

// Knast
bool is_knast_mono(p_cayley, p_green, FILE* out);

// Knast on the MOD-kernel
bool is_qknast_mono(p_green_sub mker, FILE* out);

// Procedure for BPol(AT)
bool is_knast_at_mono(p_cayley M, p_green G, FILE* out);

// bool is_piecewise_dfa(p_nfa, bool);

/***********************/
/* Pol(G) - Separation */
/***********************/

// Pol(ST)-separation of A from B
bool decid_polst_sep(p_nfa A, p_nfa B, bool details, FILE* out);

// Pol(MOD)-separation of A from B
bool decid_polmod_sep(p_nfa A, p_nfa B, bool details, FILE* out);

// Pol(GR)-separation of A from B
bool decid_polgr_sep(p_nfa A, p_nfa B, bool details, FILE* out);

/*******************/
/* BPol(G)-orbites */
/*******************/

// Algorithmes de calcul des BPol(G) et BPol(G⁺)-orbites
// Les deux procédures sont basées sur le fait que ce sont
// aussi les co-Pol(G) et co-Pol(G⁺)-orbites

// Calcul des BPol(G)-orbites à partir du G-kernel
p_orbits get_bpgorbs(p_cayley M, p_green G, p_green_sub ker);

// Calcul des BPol(G⁺)-orbites à partir des G⁺-orbites
// (on ajoute aussi le G-kernel pour le cas où 1 a un antécédent non-vide:
// il suffit alors de calculer les BPol(G)-orbites).
p_orbits get_bpgporbs(p_cayley M, p_green G, p_orbits orbs);

/***********************/
/* Pol(G) - Separation */
/***********************/

// BPol(GR)-separation of A from B
bool decid_bpolgr_sep(p_nfa A, p_nfa B, bool details, FILE* out);

#endif
