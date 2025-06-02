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

/**
 * @brief
 * Variable storing the optimization level used for kernels and orbits.
 */
extern sub_level optimization_level;

/**
 * @brief
 * Get the object linked to another object by a command chain.
 *
 * @return
 * The index of the linked object.
 */
int com_get_object(int i, //<! Index of the original object in the table.
    string_chain* chain //<! The command chain.
);

/**
 * @brief
 * Setup of a recursive relation.
 */
void com_setrec_command(char* varname, //!< Name of the variable representing the whole recursive definition.
    char* subname, //!< Name of the particular subvariable to set.
    char* param, //!< Name of the parameter to set.
    com_command* com //!< Regular expression to assign (possibly symbolic)
);

/**
 * @brief
 * Available modes for applying a command.
 *
 * @remark
 * Only impacts the commands that return an object.
 */
typedef enum {
    MODE_DEFAULT, //!< Default mode: stores a new object only if does not exist yet (no copy).
    MODE_COPY,    //!< Copy mode: creates a new even it already exists (makes a copy).
    MODE_PRINT,   //!< Display mode: no new object is created.
} com_mode;

/**
 * @brief
 * Application of a command.
 *
 * @return
 * If the command computes an object: the index of this object in the table.
 * If the command does not compute an object: -1.
 * If there was an error in evaluating the command: -2.
 */
int com_apply_command(com_command* com, //!< The command to apply.
    char* varname, //!< The name of the variable to store the result (NULL if no result to store).
    com_mode mode, //!< The mode to apply the command.
    bool* new //!< Used to indicate whether a new object has been created (NULL if not needed).
);


/******************************/
/*+ Parameter interpretation +*/
/******************************/


/**
 * @brief
 * Tries to retrieve a class from a parameter.
 *
 * @return
 * True if the parameter is indeed a class, false otherwise.
 */
bool param_getclass(com_parameters* pars, //!< The parameters list.
    int p,                                //!< The parameter index.
    const char* str,                      //!< The string used to call the command.
    classes* class                    //!< The class returned by pointer in case of success.
);

/**
 * @brief
 * Tries to retrieve raw text from a parameter.
 *
 * @return
 * True if the parameter is indeed raw text, false otherwise.
 */
bool param_getrawtext(com_parameters* pars, //!< The parameters list.
    int p,                                  //!< The parameter index.
    const char* str,                        //!< The string used to call the command.
    char** rawtext                          //!< The raw text returned by pointer in case of success (no MALLOC is done, pointeer to the char* in the command).
);


/**
 * @brief
 * Tries to retrieve an integer from a parameter.
 *
 * @return
 * True if the parameter is indeed an integer, false otherwise.
 */
bool param_getinteger(com_parameters* pars, //!< The parameters list.
    int p,                                  //!< The parameter index.
    const char* str,                        //!< The string used to call the command.
    int* integer                            //!< The integer returned by pointer in case of success.
);

typedef enum {
    PAR_CLASS,    //!< The parameter is a class.
    PAR_RAWTEXT,  //!< The parameter is raw text.
    PAR_INTEGER,  //!< The parameter is an integer.
    PAR_AUTOMATONV, //!< The parameter is an automaton variable (more restrictive than the next one).
    PAR_AUTOMATON, //!< The parameter is an automaton.
    PAR_REGEXP,   //!< The parameter is a regular expression.
    PAR_MORPHISM, //!< The parameter is a morphism.
    PAR_RECDEF,   //!< The parameter is a recursive definition.
    PAR_AUTOMOR, //!< The parameter is an automaton or a morphism.
    PAR_AUTOREG,  //!< The parameter is an automaton or a regular expression.
    PAR_REGMOR,  //!< The parameter is a regular expression or a morphism.
    PAR_REGAUTOMOR, //!< The parameter is a regular expression or an automaton or a morphism.
    PAR_OBJECTV,  //!< The parameter is an object variable (more restrictive than the next one).
    PAR_OBJECT,   //!< The parameter is an arbitrary object.
    PAR_NONE,     //!< The parameter is not used.
} par_type; //!< The type of the parameter.


/**
 * @brief
 * Checks the type of a retrieved object.
 *
 * @return
 * True if the type is correct, false otherwise.
 */
bool param_checkobjtype(int i, //!< Index in the object table.
    par_type type,                //!< The expected type of the object.
    int p,                 //!< The parameter index.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Retrieves the parameters of a command.
 *
 * @remark
 * The parameters are stored in the com_parameters structure.
 *
 * @return
 * -1 in case of success, -2 in case of error.
 */
int param_retrieve(com_parameters* pars, //!< The parameters list.
    int n,                               //!< The expected number of parameters to retrieve.
    int f,                               //!< The number of optional parameters (at least n - f parameters in total).
    par_type* types,                     //!< The types of the parameters (array of size n).
    classes* classes,                    //!< Used to return the evaluated classes.
    char** rawtext,                      //!< Used to return the evaluated raw text.
    int* integers,                       //!< Used to return the evaluated integers.
    int* objs,                           //!< Used to return the evaluated objects.
    bool* saved,                         //!< Used to indicate whether new objects have been created.
    const char* str                      //!< The string used to call the command.
);

/**
 * @brief
 * Generic usage function.
 */
void usage_generic(par_type* types,      //!< The types of the parameters.
    int n,                               //!< The number of parameters.
    par_type typeret,                    //!< Return type of the command.
    const char* str                      //!< The string used to call the command.
);



/**
 * @brief
 * Converts a parameter to an integer.
 *
 * @remark
 * It is not checked whether i is indeed a valid parameter index.
 *
 * @return
 * Returns -1 if the parameter is an integer, -2 is the conversion fails.
 */
int com_integer_par(com_parameters* pars, //!< The parameters of the command.
    int p, //!< The parameter to convert.
    const char* str, //!< The string used to call the command.
    int* res //!< The integer to store the result.
);





/********************************/
/*+ Computation of new objects +*/
/********************************/



/**
 * @brief
 * Computes a regular expression from an NFA using the Brzozowski-McCluskey algorithm.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created regular expression.
 */
int shell_mccluskey_reg(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command. 
);

/**
 * @brief
 * Computes an NFA  from a regular expression using the Thompson algorithm.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_thompson_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command. 
);

/**
 * @brief
 * Computes an NFA  from a regular expression using the Glushkov algorithm.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_glushkov_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command. 
);

/**
 * @brief
 * Computes the mirror of an NFA.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_mirror_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command. 
);

/**
 * @brief
 * Union of two NFAs.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_union_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Intersection of two NFAs.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_intersect_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Concatenation of two NFAs.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_concat_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Kleene star of a NFA.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_kleene_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Eliminates espilon transitions in a NFA (returns a new NFA).
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_elimeps_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Computes a new nfa by eliminating all useless states (not reachable or not co-reachable).
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_trim_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);


/**
 * @brief
 * Computes a dfa by determinizing an automaton with the subset construction.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @remark
 * If the input is already a DFA, it is copied.
 *
 * @return
 * The index of the created DFA.
 */
int shell_determinize_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);


/**
 * @brief
 * Commplements an automaton (NFA or DFA).
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created DFA.
 */
int shell_complement_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);


/**
 * @brief
 * Computes the minimal DFA of a NFA  using Hopcroft's algorithm.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_hopcroft_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Computes the minimal DFA of a NFA using Brzozowski's algorithm.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_brzozowski_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Computes a copy of NFA and adds the inverse transitions to it.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_invtrans(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Computes the Dyck extension of a NFA.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_dycktrans_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Computes a random NFA.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_random_nfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);

/**
 * @brief
 * Computes a random DFA.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the created NFA.
 */
int shell_random_dfa(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command.
);






/*******************/
/*+ File handling +*/
/*******************/

/**
 * @brief
 * Opens an object.
 *
 * @return
 * The index of the object in the table, or -2 on failure.
 */
int shell_open_object(char* varname,   //!< The variable name for storing the object
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Saves an object in a file.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_save_to_file(com_parameters* pars,//!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Loads a session from a file.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_load_session(com_parameters* pars,//!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Saves a session in a file.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_save_session(com_parameters* pars,//!< The parameters of the command.
    const char* str //!< The string used to call the command.
);


/*************/
/*+ Display +*/
/*************/




/**
 * @brief
 * Displays tikz code representing an object.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_latex_gen(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the right Cayley graph of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_rcayley(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the left Cayley graph of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_lcayley(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the multiplication table of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_mormult(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the syntactic order of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_morder(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the idempotents of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_idems(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Toggles the optimization level for kernels and orbits.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_toggle_optimization(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the MOD-kernel of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_mkernel(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the AMT-kernel of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_akernel(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the GR-kernel of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_gkernel(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the orbits of a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_orbits(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays a run in an NFA.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_nfa_run(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Displays the image of a word in a morphism.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_view_mor_image(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);


int shell_jep(void);


/*******************/
/*+ Property test +*/
/*******************/

/**
 * @brief
 * Tests if a DFA is counterfree.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_counterfree(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Tests if a DFA satisfies the DA pattern equation.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_autoda(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Tests if a DFA is a permutation automata.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_permutation(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);


/**
 * @brief
 * Tests if a DFA is cycle trivial.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_cycletrivial(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);


/**
 * @brief
 * Tests if a DFA is commutative.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_commutative(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Tests if a DFA is letter uniform.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_letteruniform(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);


/**
 * @brief
 * Generic membership function.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_membership(com_parameters* pars, //!< Command parameters (should include the class and the input language).
    const char* str //!< The command name.
);


/**
 * @brief
 * Generic separation function.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_separation(com_parameters* pars, //!< Command parameters (should include the class and the input language).
    const char* str //!< The command name.
);


/**
 * @brief
 * Summary of all available tests for concatenation hierarchies.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_print_chiera(com_parameters* pars, //!< Command parameters (should include the input language).
    const char* str //!< The command name.
);


/**
 * @brief
 * Computes the level of a language inside a negation hierarchy.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_print_neghiera(com_parameters* pars, //!< Command parameters (should include the base class and the input language).
    const char* str //!< The command name.
);

/**
 * @brief
 * Computes the level of a language inside a future/past hierarchy.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_print_fphiera(com_parameters* pars, //!< Command parameters (should include the base class and the input language).
    const char* str //!< The command name.
);


/*************************/
/*+ Examples generators +*/
/*************************/

/**
 * @brief
 * Browses all DFAs of a given alphabet size and number of states and checks membership conditions.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_browse_dfas(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Browses all DFAs of a given alphabet size and number of states and checks membership in a level of a negation hierarchy.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_browse_dfas_neg(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Browses all DFAs of a given alphabet size and number of states and checks membership in a level of a negation hierarchy.
 *
 * @return
 * -1 on success, -2 on failure.
 */
int shell_browse_dfas_fp(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);


#endif
