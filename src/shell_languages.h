#ifndef LANGUAGES_H_
#define LANGUAGES_H_

#include "monoid.h"
#include "monoid_display.h"
#include "monoid_kernels.h"
#include "monoid_orbits.h"
#include "nfa.h"
#include "nfa_determi.h"
#include "printing.h"
#include "regexp.h"
#include "regexp_tonfa.h"
#include <stdlib.h>

/*
//  _____ _     _           _         _   _           _                       _
// |  _  | |   (_)         | |       | | | |         | |                     | |
// | | | | |__  _  ___  ___| |_ ___  | |_| |__   __ _| |_    ___ __ _ _ __   | |__   ___
// | | | | '_ \| |/ _ \/ __| __/ __| | __| '_ \ / _` | __|  / __/ _` | '_ \  | '_ \ / _ \
// \ \_/ / |_) | |  __/ (__| |_\__ \ | |_| | | | (_| | |_  | (_| (_| | | | | | |_) |  __/
//  \___/|_.__/| |\___|\___|\__|___/  \__|_| |_|\__,_|\__|  \___\__,_|_| |_| |_.__/ \___|
//            _/ |
//           |__/
//                        _             _       _           _    _
//                       (_)           | |     | |         | |  (_)
//  _ __ ___   __ _ _ __  _ _ __  _   _| | __ _| |_ ___  __| |   _ _ __
// | '_ ` _ \ / _` | '_ \| | '_ \| | | | |/ _` | __/ _ \/ _` |  | | '_ \
// | | | | | | (_| | | | | | |_) | |_| | | (_| | ||  __/ (_| |  | | | | |
// |_| |_| |_|\__,_|_| |_|_| .__/ \__,_|_|\__,_|\__\___|\__,_|  |_|_| |_|
//                         | |
//                         |_|
//  _   _                  _          _ _
// | | | |                | |        | | |
// | |_| |__   ___     ___| |__   ___| | |
// | __| '_ \ / _ \   / __| '_ \ / _ \ | |
// | |_| | | |  __/   \__ \ | | |  __/ | |
//  \__|_| |_|\___|   |___/_| |_|\___|_|_|
*/

/**
 * @brief
 * Structure used to store a variable name prefix.
 */
typedef struct
{
  char *name;   //!< The variable name prefix.
  uint count;   //!< The number of objects with this prefix.
  uchar digits; //!< The number of digits used for the suffix.
} ob_prefixname;

/**
 * @brief
 * Types of kernels available for computation.
 */
typedef enum
{
  KER_MOD, //!< MOD-kernel.
  KER_AMT, //!< AMT-kernel.
  KER_GR,  //!< GR-kernel.
  KER_SIZE,
} kernel_type;

/**
 * @brief
 * Types of orbits available for computation.
 */
typedef enum
{
  ORB_DD,     //!< DD-orbits.
  ORB_MODP,   //!< MOD⁺-orbits.
  ORB_AMTP,   //!< AMT⁺-orbits.
  ORB_GRP,    //!< GR⁺-orbits.
  ORB_PT,     //!< PT-orbits.
  ORB_BPMOD,  //!< BPol(MOD)-orbits.
  ORB_BPAMT,  //!< BPol(AMT)-orbits.
  ORB_BPGR,   //!< BPol(GR)-orbits.
  ORB_BPDD,   //!< BPol(DD)-orbits.
  ORB_BPMODP, //!< BPol(MOD⁺)-orbits.
  ORB_BPAMTP, //!< BPol(AMT⁺)-orbits.
  ORB_BPGRP,  //!< BPol(GR⁺)-orbits.
  ORB_SIZE,
} orbits_type;

/**
 * @brief
 * Structure used to store a morphism and its Green relations.
 */
typedef struct
{
  morphism *obj;           //!< The morphism.
  subsemi *kers[KER_SIZE]; //!< The kernels.
  orbits *orbs[ORB_SIZE];  //!< The orbits.
} ob_morphism;

#define RECMAX 8

/**
 * @brief
 * Type used to represent a recursive definition of regular expressions.
 */
typedef struct
{
  bool full;                //!< Boolean indicating if the recursive definition is fully specified.
  uchar num;                //!< Number of relations in the recursion.
  ushort init;              //!< The number of indices that are required to start the recursion.
  char *names[RECMAX];      //!< Array mapping each relation to its variable name.
  uchar evalseq[RECMAX];    //!< Sequence of evaluation.
  regexp **regexps[RECMAX]; //!< Array mapping each sequence to an array containing the indices of the computed elements in the objects array.
  regexp *def[RECMAX];      //!< Array mapping each sequence to its inductive definition.
} ob_recursion;

/**
 * @brief
 * The different dependencies an object can have.
 */
typedef enum
{
  OD_MINI, //!< Mininmal automaton.
  OD_SYNT, //!< Syntactic morphism.
  OD_SIZE,
} ob_depend;

/**
 * @brief
 * The different types of objects.
 */
typedef enum
{
  EMPTYOBJ,   //!< Empty object.
  REGEXP,     //!< Regular expression.
  NAUTOMATON, //!< Nondeterministic finite automaton.
  DAUTOMATON, //!< Deterministic finite automaton.
  MORPHISM,   //!< Morphism.
  RECDEF,     //!< Recursive definition of regular expressions.
  DUMMY,      //!< Empty object (used to handle errors).
} ob_type;

extern char *object_types_names[DUMMY];

#define NAME_MAXSIZE 128 //!< Maximum size of the variable name.

/**
 * @brief
 * Type used to represent an object.
 */
typedef struct
{
  // char name[NAME_MAXSIZE];   //!< Variable name (NULL if the object is a dependency).
  ob_prefixname *prefix; //<! Prefix of the variable name (Index in the prefixnames array). Equal to -1 if the object is a dependency (noname). Full name is prefix + number.
  uint number;           //!< Number of object for this variable name (added as a suffix to the name). UINT_MAX if not used.

  ob_type type; //!< Type of the object.
  union
  {
    regexp *exp;       //!< Case of a regular expression.
    nfa *obj_nfa;      //!< Case of a nondeterministic finite automaton.
    dfa *obj_dfa;      //!< Case of a deterministic finite automaton.
    ob_morphism *mor;  //!< Case of a morphism.
    ob_recursion *rec; //!< Case of a recursive definition of regular expressions.
  };
  int depend[OD_SIZE]; //!< Array of dependencies (-1 if no dependency).
  int parent;          //!< Index of the parent object for dependencies (-1 if no parent).
} object;

/**
 * @brief
 * The array of objects.
 */
extern object *objects;

/**
 * @brief
 * The number of objects in the array.
 */
extern int nb_objects;

/**
 * @brief
 * Initilization of the array of objects.
 */
void init_objects_array(void);

/**
 * @brief
 * Double the size of the array of objects.
 */
void grow_objects_array(void);

/************************/
/* Creation/Deletion */
/************************/

/**
 * @brief
 * Initialization of a new prefix name used as a full name (no number suffix).
 *
 * @return
 * The created prefix name.
 */
ob_prefixname *create_prefixname_full(const char *name //!< The variable name prefix (duplicated).
);

/**
 * @brief
 * Initialization of a new prefix name.
 *
 * @return
 * The created prefix name.
 */
ob_prefixname *create_prefixname(const char *name, //!< The variable name prefix (duplicated).
                                 uint count        //!< The number of objects with this prefix.
);

/**
 * @brief
 * Removes one instance of a prefix name by decrementing the count. If the count reaches 0, the prefix name is deleted.
 */
void remove_instance_prefixname(ob_prefixname *prefixname //!< The prefix name to delete.
);

/**
 * @brief
 * Initialization of a new object in the table. Does not define its type.
 *
 * @return
 * The index of the created object.
 */
int object_init(const char *name //!< The variable name of the object.
);

/**
 * @brief
 * Swap two objects in the table.
 */
void object_swap(int, //!< The index of the first object.
                 int  //!< The index of the second object.
);

/**
 * @brief
 * Auxiliary function to free an object.
 */
void object_free_aux(object * //!< The object to free.
);

/**
 * @brief
 * Release of an object.
 */
void object_free(int //!< Index of the object.
);

/**
 * @brief
 * Release of all objects.
 */
void object_free_all(void);

/**
 * @brief
 * Deletes all objects with a given prefix.
 *
 * @remark
 * The prefix is the first part of the variable name.
 */
void object_delete_prefix(const char * //!< The prefix of the variable names to delete.
);

/************************/
/* Get/insert an object */
/************************/

/**
 * @brief
 * Gets the full name of an object.
 *
 * @remark
 * The full name is written in a buffer (no allocation is done).
 *
 * @return
 * The full name of the object (prefix + number) or NULL if the object has no prefix.
 */
const char *object_get_full_name(int i //!< The index of the object.
);

/**
 * @brief
 * Gets the index of an object from its variable name.
 *
 * @return
 * The index of the object or -1 if the object was not found.
 */
int object_get_from_name(const char * //!< The variable name.
);

/**
 * @brief
 * Deletes an object from its variable name.
 *
 * @return
 * The index of the object that was deleted or -1 if the object was not found.
 */
int object_delete_from_name(const char * //!< The variable name.
);

/**
 * @brief
 * Adds a new object of type regular expression.
 *
 * @attention
 * The name passed as input is duplicated. The regular expression
 * passed as input is NOT duplicated.
 *
 * @return
 * The index of the created object.
 */
int object_add_regexp(const char *, //!< The desired variable name (NULL if no name for system objects).
                      regexp *      //!< The regular expression.
);

/**
 * @brief
 * Adds a new object of type automaton (NFA structure).
 *
 * @attention
 * The name passed as input is duplicated. The automaton passed
 * as input is NOT duplicated.
 *
 * @return
 * The index of the created object.
 */
int object_add_automaton_nfa(const char *, //!< The desired variable name (NULL if no name for system objects).
                             nfa *         //!< The automaton.
);

/**
 * @brief
 * Adds a new object of type automaton (DFA structure).
 *
 * @attention
 * The name passed as input is duplicated. The automaton passed
 * as input is NOT duplicated.
 *
 * @return
 * The index of the created object.
 */
int object_add_automaton_dfa(const char *name, //!< The desired variable name (NULL if no name for system objects).
                             dfa *A            //!< The automaton.
);

void object_add_automaton_dfa_family(const char *pname, dfa **array, uint count);

/**
 * @brief
 * Adds a new object of type morphism.
 *
 * @attention
 * The name passed as input is duplicated. The morphism and the Green
 * relations passed as input are NOT duplicated.
 *
 * @return
 * The index of the created object.
 */
int object_add_morphism(char *,    //!< The desired variable name (NULL if no name for system objects).
                        morphism * //!< The morphism.
);

/**
 * @brief
 * Copy of an object.
 *
 * @remark
 * If an object with the same name already exists, it is deleted.
 *
 * @return
 * The index of the copy.
 */
int shell_copy_generic(int i,        //!< The index of the object to copy.
                       char *newname //!< The variable name for the new object.
);

/***********************************************/
/* Computing information on an existing object */
/***********************************************/

/**
 * @brief
 * Computes the minimal automaton of an object (if it has not already been computed).
 *
 * @return
 * The index of the minimal automaton in the object array.
 */
int shell_compute_minimal(int //!< The index of the object for which the minimal automaton needs to be computed.
);

/**
 * @brief
 * Computes the syntactic morphism of an object (if it has not already been computed).
 *
 * @remark
 * The function checks if the size of the computed syntactic morphism exceeds ths limit given in interruption.h.
 * If the limit is positive and is passed, the morphism is not computed and the function returns -1.
 *
 * @return
 * The index of the syntactic morphism in the object array.
 */
int shell_compute_syntac(int //!< The index of the object for which the syntactic morphism needs to be computed.
);

/**
 * @brief
 * Computes the multiplication table of a morphism (if it has not already been computed).
 *
 * @remark
 * The object has to be a morphism.
 */
void shell_compute_mult(int //!< The index of the morphism in the object array.
);

/**
 * @brief
 * Computes the syntactic order of a morphism (if it has not already been computed).
 *
 * @remark
 * The object has to be a morphism. Moreover, it is assumed to be a syntactic morphism
 * (this is not checked).
 */
void shell_compute_order(int //!< The index of the morphism in the object array.
);

/**
 * @brief
 * Computes and returns a kernel of a morphism.
 *
 * @remark
 * The object has to be a morphism.
 *
 * @return
 * The kernel.
 */
subsemi *shell_compute_ker(int,         //!< The index of the morphism in the object array.
                           kernel_type, //!< The type of kernel to compute.
                           sub_level    //!< Desired computation level.
);

/**
 * @brief
 * Computes and returns an orbits set of a morphism.
 *
 * @remark
 * The object has to be a morphism.
 *
 * @return
 * The orbits set.
 */
orbits *shell_compute_orbits(int,         //!< The index of the morphism.
                             orbits_type, //!< The type of orbits to compute.
                             sub_level    //!< Desired computation level.
);

/*************/
/* Recursion */
/*************/

/**
 * @brief
 * Computes the index of a relation in a recursion from
 * its name.
 *
 * @return
 * The index of the relation or the number of relations or
 * the total nimber of relations if the relation was not found.
 */
uchar shell_rec_getnum(ob_recursion *, //!< The recursion.
                       char *          //!< The name of the relation.
);

/**
 * @brief
 * Checks if a recursion is well-defined.
 *
 * @return
 * True if the recursion is well-defined, false otherwise.
 */
bool shell_check_recursion(int //!< The index of the recursion.
);

/**
 * @brief
 * Defines a relation in a recursion.
 *
 * @return
 * The index of the recursion.
 */
int shell_rec_defadd(int,     //!< The index of the recursion.
                     uchar,   //!< The name of the relation.
                     regexp * //!< The inductive definition of the relation.
);

/**
 * @brief
 * Defines an initial case for a relation in a recursion.
 *
 * @return
 * The index of the recursion.
 */
int shell_rec_iniadd(int,     //!< The index of the recursion.
                     uchar,   //!< The name of the relation.
                     ushort,  //!< The index of the initial case.
                     regexp * //!< The initial case.
);

/**
 * @brief
 * Computes the regular expressions of a recursion for a given index.
 *
 * @return
 * An array of regular expressions containing an expression for each relation.
 */
regexp **shell_rec_compute(ob_recursion *, //!< The recursion.
                           ushort          //!< The index.
);

/**
 * @brief
 * Gets the regular expression of a relation in a recursion for a given index.
 *
 * @return
 * The regular expression.
 */
regexp *shell_rec_getexp(int,    //!< The index of the recursion.
                         char *, //!< The name of the relation.
                         ushort  //!< The index.
);

/**
 * @brief
 * Displays a recursion on a given stream.
 */
void shell_rec_display(ob_recursion *, //!< The recursion.
                       FILE *          //!< The stream.
);

/***********/
/* Display */
/***********/

/**
 * @brief
 * Displays an object
 */
void shell_view_object(object *ob, //!< The object.
                       bool title  //!< Boolean indicating if a title should be displayed.
);

#endif // LANGUAGES_H_
