/***********************************/
/* Keywords utilisés dans le shell */
/***********************************/

#ifndef SHELL_KEYWORDS_H
#define SHELL_KEYWORDS_H

#include <stdbool.h>
#include <string.h>
#include "types.h"
#include "shell_languages.h"

// Liste des keywords possible
typedef enum {
    // Commands
    CM_MINI,
    CM_SYNT,
    CM_GREEN,
    CM_RCAY,
    CM_LCAY,
    CM_IDEMS,
    CM_ORDER,
    CM_MULT,
    CM_KERNEL,
    CM_MKERNEL,
    CM_GKERNEL,
    CM_ORBIT,
    CM_ORBITS,
    CM_MEMB,
    CM_SEPAR,
    CM_CONCAT,
    CM_NEGHIERA,
    CM_FPHIERA,
    CM_INFO,
    CM_THOMSON,
    CM_INTERSEC,
    CM_DETERMIN,
    CM_COMPLEM,
    CM_KLEENE,
    CM_ELIMEPS,
    CM_TRIMNFA,
    CM_LINK,
    CM_UNION,
    CM_OPEN,
    CM_SAVE,
    CM_SAVESESSION,
    CM_LOADSESSION,
    CM_IMAGE,
    CM_RUN,
    CM_DELETE,
    CM_SORT,
    CM_RECDEF,
    CM_RNFA,
    CM_RDFA,

    // Props
    PR_COMM,

    // Classes
    CL_AT,
    CL_ATT,
    CL_SF,
    CL_UL,
    CL_TL,
    CL_TLX,
    CL_PPT,
    CL_PT,
    CL_LT,
    CL_LTT,
    CL_ST,
    CL_DD,
    CL_MOD,
    CL_MODP,
    CL_AMT,
    CL_GR,

    // Operators

    OP_POL,
    OP_POL2,
    OP_BPOL,
    OP_BPOL2,
    OP_UBPOL,
    OP_UBPOL2,
    OP_UPOL,
    OP_LPOL,
    OP_RPOL,
    OP_MPOL,
    OP_SFC,
    OP_TLC,
    OP_TLC2,
    OP_FLC,
    OP_FLC2,
    OP_PLC,
    OP_PLC2,

    // Dummy keyword
    KY_NULL
} com_keyword;

typedef enum {
    CMT_RAW,
    CMT_KEY,
} com_type;

typedef struct string_chain {
    char* string;
    struct string_chain* next;
} string_chain;

typedef struct com_parameters {
    uchar count;
    struct com_command* param;
    struct com_parameters* next;
} com_parameters;

typedef struct com_command {
    com_type thetype;
    string_chain* main;
    struct com_parameters* params;
} com_command;


/************************/
/* Tableau des keywords */
/************************/

// Ajout d'un keyword
void keywords_add_key(com_keyword, const char*);

// Ajout de tous les keywords
void keywords_add_all_keys(void);


/********************************/
/* Messages d'érreur génériques */
/********************************/

// Commande invalide
void shell_command_error(void);

// Arguments invalides
void shell_arguments_error(void);

// Variable inconnue
void shell_variable_error(void);

/**************************************/
/* Conversion d'une chaîne en element */
/**************************************/

// Retourne le keyword associé à une chaine
com_keyword string_to_keyword(char* s);

// Fonction qui teste si un nom de variable est autorisé
// Est-ce qu'on évite les keywords ?
// Est-ce que ce n'est pas une variable protégée?
bool check_varname(char* s);

/*********************************/
/* Informations sur une commande */
/*********************************/

// Teste si une commande est constituée d'un maillon correspondant à une chaîne brute
bool com_israw(com_command*);

// Teste si une commande est constituée d'un maillon unique SANS paramètres
bool com_single(com_command*);

// Teste si une commande est constituée d'un maillon unique avec ou sans paramètres
bool com_single_par(com_command*);

// Récupération du nombre de paramètres
int com_nbparams(com_parameters* thepar);

// Retourne le tableau des paramètres d'une commande
com_command** com_getparamarray(com_parameters* thepar);


// Récupération du nième paramètre d'une commande (qui est lui-même une commande)
com_command* com_getparam(com_parameters* thepar, int n);

// Retourne l'index de l'objet désigné par une chaîne (-1 si il n'y en a pas)
int index_from_string_chain(string_chain*);

// Retourne le mot-clé associé au premier maillon d'une chaîne (KY_NULL si il n'y en a pas)
com_keyword key_from_string_chain(string_chain*);

// Retourne le mot-clé associé à une chaîne d'un seul maillon (KY_NULL si il n'y en a pas ou si il y a plus d'un maillon)
com_keyword key_from_string_chain_single(string_chain*);

/*******************************/
/* Récupération d'une commande */
/*******************************/

// Récupération à partir du parser
string_chain* com_make_string_chain(char*, string_chain*);
com_parameters* com_make_parameters(com_command*, com_parameters*);
com_command* com_make_command(char*, com_command*);
com_command* com_init_command(char*, com_parameters*);
com_command* com_init_rawcommand(char*);


// Libération
void com_free_string_chain(string_chain*);
void com_free_command(com_command*);
void com_free_parameters(com_parameters*);

// Analyse


#endif 
