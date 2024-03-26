/*****************************************/
/* Fonctions spécifiques sur les classes */
/*      (affichage principalement)       */
/*****************************************/

#ifndef SHELL_SCLASS_H
#define SHELL_SCLASS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "macros_alloc.h"
#include "class_at.h"
#include "shell_languages.h"
#include "shell_keywords.h"

/*********************/
/* Affichage partagé */
/*********************/

void print_info_input(int i, FILE* out);

void print_start_comp(FILE* out, char* class, object_type thetype);

void print_conclusion_comp(FILE* out, bool res, char* namec, object_type thetype);

/*********************************************/
/* Affichage des informations sur une classe */
/*********************************************/

// Alphabet testable
void print_infoclass_at(FILE* out);

// Alphabet threshold testable
void print_infoclass_att(FILE* out);

// Locally testable
void print_infoclass_lt(FILE* out);

// Locally threshold testable
void print_infoclass_ltt(FILE* out);

// Star-free
void print_infoclass_sf(FILE* out);

// Group
void print_infoclass_gr(FILE* out);

// Alphabet modulo
void print_infoclass_amt(FILE* out);

// Modulo
void print_infoclass_mod(FILE* out);

// Trivial class
void print_infoclass_st(FILE* out);

// Piecewise testable
void print_infoclass_pt(FILE* out);

// Positive piecewise testable
void print_infoclass_ppt(FILE* out);

// Unambiguous
void print_infoclass_ul(FILE* out);


/***********************************************/
/* Affichage des informations sur un opérateur */
/***********************************************/


// Star-free closure
void print_infooper_sf(com_keyword c, FILE* out);

// Temporal closure
void print_infooper_tl(com_keyword c, FILE* out);

// Nested temporal closure
void print_infooper_tl2(com_keyword c, FILE* out);

// Future temporal closure
void print_infooper_fl(com_keyword c, FILE* out);

// Nested future temporal closure
void print_infooper_fl2(com_keyword c, FILE* out);

// Past temporal closure
void print_infooper_pl(com_keyword c, FILE* out);

// Nested past temporal closure
void print_infooper_pl2(com_keyword c, FILE* out);

// Polynomial closure
void print_infooper_pol(com_keyword c, FILE* out);

// Boolean polynomial closure
void print_infooper_bpol(com_keyword c, FILE* out);

// Unambiguous Boolean polynomial closure
void print_infooper_ubpol(com_keyword c, FILE* out);

// Nested polynomail closure 
void print_infooper_pol2(com_keyword c, FILE* out);

// Nested unambiguous Boolean polynomial closure
void print_infooper_ubpol2(com_keyword c, FILE* out);


// Negation hierarchies
void print_infooper_neghiera(com_keyword c, FILE* out);

// Future/past hierarchies
void print_infooper_fphiera(com_keyword c, FILE* out);

#endif
