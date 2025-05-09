/**
 * @file shell_keywords.h
 * @brief
 * Keywords and commands used in the shell.
 */

#ifndef SHELL_KEYWORDS_H
#define SHELL_KEYWORDS_H

#include "shell_languages.h"
#include "type_basic.h"
#include <stdbool.h>
#include <string.h>

 /**
  * @brief
  * The keywords.
  */
typedef enum {

    // Interface
    KY_INTERFACE,

    // General commands
    KY_SAVESESSION,
    KY_LOADSESSION,
    KY_OPEN,
    KY_SAVE,
    KY_DELETE,
    KY_CLEAR,
    KY_SORT,
    KY_INFO,
    KY_EXSEARCH,
    KY_EXALL,
    KY_NEXALL,
    KY_FPEXALL,
    KY_INSIDE,
    KY_OUTSIDE,
    KY_LIST,
    KY_AUTOMATA,
    KY_MORPHISMS,
    KY_GROUPS,
    KY_REGEXPS,
    KY_RECDEFS,
    KY_TOGGLE,
    KY_LATEX,

    KY_FILTER,
    KY_FMSIZE,

    // Automata commands
    KY_MINI,
    KY_THOMPSON,
    KY_GLUSHKOV,
    KY_HOPCROFT,
    KY_BMCK,
    KY_INTERSEC,
    KY_ELIMEPS,
    KY_COMPLEM,
    KY_BRZOZO,
    KY_KLEENE,
    KY_MIRROR,
    KY_CONCAT,
    KY_RESET,
    KY_TRIMNFA,
    KY_UNION,
    KY_RUN,
    KY_RNFA,
    KY_RDFA,
    KY_INVTRANS,
    KY_DYCKTRANS,
    KY_RECDEF,
    KY_PERMUT,
    KY_COUNTER,

    // Morphisms commands
    KY_SYNT,
    KY_RCAY,
    KY_LCAY,
    KY_MULT,
    KY_IDEMS,
    KY_ORDER,
    KY_IMAGE,
    KY_GKER,
    KY_MKER,
    KY_AKER,
    KY_ORB,

    // Recursive
    KY_RECINIT,

    // Tests
    KY_SEPAR,
    KY_MEMB,
    KY_CHIERA,
    KY_NHIERA,
    KY_FPHIERA,

    // Classes
    KY_HTGEN,
    KY_AT,
    KY_ATT,
    KY_SF,
    KY_UL,
    KY_TLX,
    KY_PPT,
    KY_PT,
    KY_LT,
    KY_LTT,
    KY_ST,
    KY_DD,
    KY_MOD,
    KY_MODP,
    KY_AMT,
    KY_AMTP,
    KY_GR,
    KY_GRP,
    KY_REG,
    KY_KNASTAT,

    // Operators

    KY_POL,
    KY_POL2,
    KY_BPOL,
    KY_BPOL2,
    KY_UBPOL,
    KY_UBPOL2,
    KY_UPOL,
    KY_LPOL,
    KY_RPOL,
    KY_MPOL,
    KY_TLC,
    KY_TLC2,
    KY_FLC,
    KY_FLC2,
    KY_PLC,
    KY_PLC2,
    KY_JORB,

    // Dummy keyword
    KY_NULL
} com_keyword;

/**
 * @brief
 * The two kinds of commands.
 */
typedef enum {
    CMT_RAW, //!< Raw text command (no parameters, no chain)
    CMT_IND, //!< Indexed command (no parameters, with chain).
    CMT_KEY, //!< Standard command (with parameters, with chain).
} com_type;

/**
 * @brief
 * Chain of strings in a command.
 */
typedef struct string_chain {
    char* string;              //!< The string.
    struct string_chain* next; //!< The next link in the chain.
} string_chain;

/**
 * @brief
 * Parameters of a command.
 */
typedef struct com_parameters {
    uchar count;                 //!< Number of parameters.
    struct com_command* param;   //!< The first parameter.
    struct com_parameters* next; //!< The next parameters.
} com_parameters;

/**
 * @brief
 * A command.
 */
typedef struct com_command {
    com_type thetype;       //!< The type of the command.
    string_chain* main;     //!< The string chain of the command.
    uint ind;               //!< The index of the command (when it is of indexed type).
    com_parameters* params; //!< The parameters of the command.
} com_command;

/*************************/
/* Array of all keywords */
/*************************/

/**
 * @brief
 * Initialization of a single keyword.
 */
void keywords_add_key(com_keyword //!< The keyword.
    ,
    const char* //!< The string corresponding to the keyword.
);

/**
 * @brief
 * Initialization of all the keywords.
 */
void keywords_add_all_keys(void);

/****************************/
/* Generic display commands */
/****************************/

/**
 * @brief
 * Display a string chain.
 */
void print_string_chain(const string_chain*, //!< The string chain.
    FILE*                //!< The output stream.
);

/**
 * @brief
 * Display a command.
 */
void print_command(com_command* //!< The command.
    ,
    FILE* //!< The output stream.
);

/************************************/
/* Converting a string to a keyword */
/************************************/

/**
 * @brief
 * Convert a keyword to the associated string.
 *
 * @return
 * The string corresponding to the keyword.
 */
const char* keywordtostring(com_keyword //!< The keyword.
);

/**
 * @brief
 * Convert a string to a keyword.
 *
 * @remark
 * If the string does not correspond to any keyword, the function
 * returns KY_NULL.
 *
 * @return
 * The keyword corresponding to the string.
 */
com_keyword string_to_keyword(const char* //!< The string.
);

/**
 * @brief
 * Check if a variable name is valid: it is not a keyword.
 *
 * @return
 * True if the variable name is valid, false otherwise.
 */
bool check_varname(const char* //!< The variable name.
);

/*****************************/
/* Informations on a command */
/*****************************/

/**
 * @brief
 * Check if a command is a class.
 *
 * @return
 * True if the command is a class, false otherwise.
 */
bool com_isclass(com_command* //!< The command.
);

/**
 * @brief
 * Check if a command is raw text.
 *
 * @return
 * True if the command is raw text, false otherwise.
 */
bool com_israw(const com_command* //!< The command.
);

/**
 * @brief
 * Check if a command is a single link without parameters.
 *
 * @return
 * True if the command is a single link without parameters, false otherwise.
 */
bool com_single(const com_command* //!< The command.
);

/**
 * @brief
 * Check if a command is a single link with or without parameters.
 *
 * @return
 * True if the command is a single link with or without parameters, false otherwise.
 */
bool com_single_par(const com_command* //!< The command.
);

/**
 * @brief
 * Computes the number of parameters of a command.
 *
 * @return
 * The number of parameters of the command.
 */
int com_nbparams(const com_parameters* //!< The parameters of the command.
);

/**
 * @brief
 * Gets the n-th parameter of a command.
 *
 * @return
 * The n-th parameter of the command.
 */
com_command* com_getparam(const com_parameters*, //!< The parameters of the command.
    int                     //!< The index of the parameter.
);

/**
 * @brief
 *  Get the keyword associated with the first link in a string chain.
 *
 * @return
 *  The keyword associated with the first link in a string chain.
 */
com_keyword key_from_string_chain(const string_chain* //!< The string chain.
);

/**
 * @brief
 *  Get the keyword associated with a single link string chain.
 *
 * @remark
 * If the chain is not a single link chain, the function returns KY_NULL.
 *
 * @return
 *  The keyword associated with the single link string chain.
 */
com_keyword key_from_string_chain_single(const string_chain* //!< The string chain.
);

/*****************************/
/* Construction of a command */
/*****************************/

/**
 * @brief
 * Adds another link to a string chain.
 *
 * @return
 * The new string chain.
 */
string_chain* com_make_string_chain(char*,        //!< The string.
    string_chain* //!< The original string chain.
);

/**
 * @brief
 * Adds a new parameter to a parameters chain.
 *
 * @return
 * The updated parameters.
 */
com_parameters* com_make_parameters(com_command*,   //!< The parameter.
    com_parameters* //!< The original parameters.
);

/**
 * @brief
 * Adds a new link to a command.
 *
 * @return
 * The updated command.
 */
com_command* com_make_command(char*,       //!< The string of the link.
    com_command* //!< The original command.
);

/**
 * @brief
 * Initializes a single link command.
 *
 * @return
 * The initialized command.
 */
com_command* com_init_command(char*,          //!< The string of the link.
    com_parameters* //!< The parameters of the command.
);

/**
 * @brief
 * Initializes a raw text command.
 *
 * @return
 * The initialized command.
 */
com_command* com_init_rawcommand(char* //!< The raw text.
);

/**
 * @brief
 * Initializes an indexed command.
 *
 * @return
 * The initialized command.
 */
com_command* com_init_indexing(char*, //!< The string of the link.
    char*  //!< The index of the command (must be a number).
);

/**
 * @brief
 * Frees a string chain.
 */
void com_free_string_chain(string_chain* //!< The string chain.
);

/**
 * @brief
 * Frees a command.
 */
void com_free_command(com_command* //!< The command.
);

/**
 * @brief
 * Frees the parameters of a command.
 */
void com_free_parameters(com_parameters* //!< The parameters.
);

#endif
