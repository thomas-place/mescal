/*****************************/
/* Gestionnaire de commandes */
/*****************************/

#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H

#include "macros_alloc.h"
#include "macros_error.h"

#include "types.h"
#include "printing.h"
#include "shell_keywords.h"
#include "shell_languages.h"
#include "shell_sepmem.h"
#include "parse_regexp.h"
#include "files.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**************************************************************/
/* Récupération d'un objet généralisé à partir d'une commande */
/**************************************************************/

typedef enum {
    OG_LANG,
    OG_AUTO,
    OG_MORP,
    OG_SUBM,
    OG_ORBS,
} genob_type;


typedef struct {
    void* theob;
    genob_type type;
} genob;

typedef genob* p_genob;


p_genob shell_genob_from_command(com_command* thecom);



/******************************/
/* Application d'une commande */
/******************************/

// Commandes sans affectation
bool com_apply_command(com_command*);

// Commandes avec affectation
int com_apply_link_command(char* name, com_command* thecom);


/******************************/
/* Calculs de nouveaux objets */
/******************************/

// Ajout d'un nouvel objet en copiant un objet déjà existant dans la table 
int object_copy_generic(int i, char* newname);

// Calcule un nouveau NFA avec la méthode de thomson à partir d'un langage.
int shell_thomson_nfa(char* varname, com_parameters*);

// Concatène deux NFAs
int shell_concat_nfa(char* varname, com_parameters*);

// Union de deux NFAs
int shell_union_nfa(char* varname, com_parameters*);

// Étoile de Kleene sur un NFA
int shell_kleene_nfa(char* varname, com_parameters* pars);

// Calcule un nouveau NFA en éliminant les transitions epsilon
int shell_elimeps_nfa(char* varname, com_parameters* pars);

// Calcule un nouveau NFA en supprimant tous les états non-accessibles ou non co-accessibles
int shell_trim_nfa(char* varname, com_parameters* pars);

// Calcule un nouveau NFA en réalisant l'intersection de deux NFAs existants
int shell_intersect_nfa(char* varname, com_parameters* pars);

// Calcule un nouveau NFA déterministe complet en déterminisant un NFA déjà existant
int shell_determinize_nfa(char* varname, com_parameters* pars);

// Calcule un automate minimal à partir d'un NFA
int shell_minimize_nfa(char* varname, com_parameters* pars);

// Links an existing nfa to a nex language
int shell_linktolang_nfa(char* varname, com_parameters* pars);

// Calcul d'une fammille de langages définie récursivement
bool shell_recursion_lang(com_parameters*);

// Sauvegarde d'un objet
bool shell_save_to_file(com_parameters*);

// Sauvegarde d'une session complète
bool shell_save_session(com_parameters*);

// Chargement d'une session à partir d'un fichier
bool shell_load_session(com_parameters*);

// Suppression d'un objet
int shell_delete(com_parameters*);

// Tri des objets
bool shell_sort(com_parameters*);

// Ouverture d'un objet
bool shell_open_object(char* varname, com_parameters*);

// Calcul d'un automate aléatoire (det=false: NFA det=true: DFA)
bool shell_random_nfa(char* varname, com_parameters*, bool det);

// Calcule un morphisme de transitions à partir d'un DFA complet
int shell_morphism_dfa(int i, char* newname);


/********************************************************************/
/* Affichage - fonctions appellées par le gestionnaire de commandes */
/********************************************************************/

bool shell_view_morphism(morphism*, com_parameters*);
bool shell_view_rcayley(morphism*, com_parameters*);
bool shell_view_lcayley(morphism*, com_parameters*);
bool shell_view_mormult(morphism*, com_parameters*);
bool shell_view_morder(morphism*, com_parameters*);
bool shell_view_idems(morphism*, com_parameters*);
bool shell_view_mkernel(morphism* morp, com_parameters* pars);
bool shell_view_gkernel(morphism* morp, com_parameters* pars);
bool shell_view_kernel(morphism* morp, com_parameters* pars);
bool shell_view_orbits(morphism* morp, com_parameters* pars);
bool shell_view_oneorbit(morphism* morp, com_parameters* pars);
bool shell_compute_image(morphism* mor, com_parameters* pars);
bool shell_make_nfa_run(automata* aut, com_parameters* pars);


/**********************/
/* Test de propriétés */
/**********************/

bool shell_prop_comm(com_parameters*);










#endif
