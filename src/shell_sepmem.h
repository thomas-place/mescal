/*********************************************/
/* Procèdures de séparation pour l'affichage */
/*********************************************/

#ifndef SHELL_SEPAR_H
#define SHELL_SEPAR_H

#include "macros_alloc.h"
#include "reg2aut.h"
#include "shell_keywords.h"
#include "shell_sclass.h"
#include "shell_languages.h"
#include "class_group.h"
#include "class_starfree.h"
#include "class_concath.h"
#include "class_temporal.h"
#include "class_deth.h"

#define BUFFERSIZE 200

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_TEST    "\x1b[48;5;68m\x1b[38;5;226m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/******************************************/
/* Commandes pour les tests de Membership */
/******************************************/

// Procédure principale
bool shell_membership_test(com_parameters*);

// Sous-procédures pour chaque opérateur
bool shell_sfc_mem(com_keyword, morphism*, object_type);
bool shell_tlc_mem(com_keyword, morphism*, object_type);
bool shell_tlctwo_mem(com_keyword, morphism*, object_type);
bool shell_flc_mem(com_keyword, morphism*, object_type);
bool shell_flctwo_mem(com_keyword, morphism*, object_type);
bool shell_plc_mem(com_keyword, morphism*, object_type);
bool shell_plctwo_mem(com_keyword, morphism*, object_type);
bool shell_pol_mem(com_keyword, morphism*, object_type);
bool shell_bpol_mem(com_keyword, morphism*, object_type);
bool shell_ubpol_mem(com_keyword, morphism*, object_type);
bool shell_poltwo_mem(com_keyword, morphism*, object_type);
bool shell_ubpoltwo_mem(com_keyword, morphism*, object_type);

// Affiche une outline des tests de membership pour les hiérarchies de concaténation
bool shell_concat_summary(com_parameters*);

// Hiérarchies déterministes 
bool shell_neghiera(com_parameters*);
bool shell_fphiera(com_parameters*);

/*********************************/
/* Generic separation procedures */
/*********************************/

// Procédure principale
bool shell_separation_test(com_parameters*);

/******************/
/* Pol Separation */
/******************/

bool shell_pol_sep(com_keyword, p_nfa, p_nfa, bool details);

#endif
