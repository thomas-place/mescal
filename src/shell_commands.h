/*****************************/
/* Gestionnaire de commandes */
/*****************************/

#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H

#include "alloc.h"
#include "error.h"

#include "files.h"
#include "monoid_kernels.h"
#include "nfa_mccluskey.h"
#include "nfa_minimization.h"
#include "nfa_props.h"
#include "parse_regexp.h"
#include "printing.h"
#include "sep_group.h"
#include "shell_memb.h"
#include "shell_sclass.h"
#include "shell_separ.h"
#include "type_basic.h"
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern sub_level optimization_level;

/**************************************************************/
/* Récupération d'un objet généralisé à partir d'une commande */
/**************************************************************/

int com_get_object(int i, string_chain *thechain);

/******************************/
/* Application d'une commande */
/******************************/

void com_setrec_command(char *, char *, char *, com_command *);

typedef enum {
    MODE_DEFAULT, //!< Mode par défaut: enregistre un nouvel objet uniquement si
                  //!< il n'existait pas c'est nécessaire.
    MODE_PRINT,   //!< Mode d'affichage: n'enregistre pas de nouvel objet.
    MODE_COPY, //!< Mode copie: créé un nouvel objet même si il existe déjà (il
               //!< y a une copie dans ce cas).
} com_mode;

int com_apply_command(com_command *, char *, com_mode mode, bool *new);

/****************************/
/* Affichage d'informations */
/****************************/

/**
 * @brief
 *  Display of all objects of a given type in memory.
 */
int shell_filter_objects(com_parameters * //!< The parameters of the command.
                         ,
                         ob_type //!< The type of the objects to display.
);

/******************************/
/* Calculs de nouveaux objets */
/******************************/

// Ajout d'un nouvel objet en copiant un objet déjà existant dans la table
int shell_copy_generic(int, char *newname);

// Calcule une regexp à partir d'un NFA
int shell_mccluskey_reg(char *varname, com_parameters *);

// Calcule un nouveau NFA avec la méthode de thompson à partir d'un langage.
int shell_thompson_nfa(char *varname, com_parameters *);

// Calcule un nouveau NFA avec la méthode de Glushkov à partir d'un langage.
int shell_glushkov_nfa(char *varname, com_parameters *pars);

int shell_hopcroft_nfa(char *varname, com_parameters *pars);

// Calcule le miroir d'un nfa à partir d'un langage.
int shell_mirror_nfa(char *varname, com_parameters *pars);

// Concatène deux NFAs
int shell_concat_nfa(char *varname, com_parameters *);

// Union de deux NFAs
int shell_union_nfa(char *varname, com_parameters *);

// Étoile de Kleene sur un NFA
int shell_kleene_nfa(char *varname, com_parameters *pars);

// Calcule un nouveau NFA en éliminant les transitions epsilon
int shell_elimeps_nfa(char *varname, com_parameters *pars);

// Calcule un nouveau NFA en supprimant tous les états non-accessibles ou non
// co-accessibles
int shell_trim_nfa(char *varname, com_parameters *pars);

// Calcule un nouveau NFA en réalisant l'intersection de deux NFAs existants
int shell_intersect_nfa(char *varname, com_parameters *pars);

// Calcule un automate minimal à partir d'un NFA
int shell_brzozowski_nfa(char *varname, com_parameters *pars);

int shell_dycktrans_nfa(char *varname, com_parameters *pars);

// Calcul d'un NFA aléatoire
int shell_random_nfa(char *varname, com_parameters *);

// Calcul d'un DFA complet aléatoire
int shell_random_dfa(char *varname, com_parameters *);

// Latex generation
int shell_latex_gen(com_parameters *);

int shell_recursive_init(char *varname, com_parameters *params);

// Sauvegarde d'un objet
int shell_save_to_file(com_parameters *);

// Sauvegarde d'une session complète
int shell_save_session(com_parameters *);

// Chargement d'une session à partir d'un fichier
int shell_load_session(com_parameters *);

// Suppression d'un objet
int shell_delete(com_parameters *);

int shell_delete_all(com_parameters *);

// Effacement des noms d'un automate
int shell_reset(com_parameters *);

int shell_invtrans(com_parameters *);

/**
 * Open an object.
 * @return
 * -1 on success,
 * An integer <= -2 on failure.
 */
int shell_open_object(
    char *varname,   //!< The variable name for storing the object
    com_parameters * //!< The parameters, whose field param->main->string is the
                     //!< filename to open
);

int shell_toggle_optimization(com_parameters *);

// Teste is un NFA est un DFA complet et counterfree.
int shell_counterfree(com_parameters *);

// Teste is un NFA est un permitation DFA.
int shell_permutation(com_parameters *);

/********************************************************************/
/* Affichage - fonctions appellées par le gestionnaire de commandes */
/********************************************************************/

bool shell_view_object(object *, bool);

/**
 * @brief
 * Sorts the objects array according to a comparison function given as input.
 */
int shell_sort(com_parameters *);

int shell_view_rcayley(com_parameters *);
int shell_view_lcayley(com_parameters *);
int shell_view_mormult(com_parameters *);
int shell_view_morder(com_parameters *);
int shell_view_idems(com_parameters *);
int shell_view_mkernel(com_parameters *);
int shell_view_akernel(com_parameters *);
int shell_view_gkernel(com_parameters *);

int shell_view_orbits(com_parameters *);

int shell_view_nfa_run(com_parameters *pars);
int shell_view_mor_image(com_parameters *pars);

int shell_jep(void);

#endif
