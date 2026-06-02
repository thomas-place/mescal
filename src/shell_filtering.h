/**
 * @file shell_filtering.h
 * @brief Header file for objects filtering in the shell.
 */

#ifndef SHELL_FILTERING_H
#define SHELL_FILTERING_H

#include "shell_commands.h"

/*************************************/
/*+ Extraction of a numerical value +*/
/*************************************/

/**
 * @brief
 * Extracts the size of the syntactic monoid of an object.
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The size of the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_sizesynt(int i //!< the index of the object.
);

/**
 * @brief
 * Extracts the size of the minimal automaton of an object.
 *
 * @remark
 * The minimal automaton of the object is computed if it has not already been computed.
 *
 * @return
 * The size of the minimal automaton of the object or UINT_MAX if the object is a recursive definition or if the minimal automaton could not be computed.
 */
uint shell_extract_sizemini(int i //!< the index of the object.
);

/**
 * @brief
 * Extracts the number of idempotents in the syntactic monoid of an object.
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The number of idempotents in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_idemnum(int i //!< the index of the object.
);

/**
 * @brief
 * Extarcts the number of R-classes in the syntactic monoid of an object.
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The number of R-classes in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_rnum(int i //!< the index of the object.
);

/**
 * @brief
 * Extarcts the number of L-classes in the syntactic monoid of an object.
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The number of L-classes in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_lnum(int i //!< the index of the object.
);

/**
 * @brief
 * Extarcts the number of J-classes in the syntactic monoid of an object.
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The number of J-classes in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_jnum(int i //!< the index of the object.
);

/**
 * @brief
 * Extarcts the number of H-classes in the syntactic monoid of an object.
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The number of H-classes in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_hnum(int i //!< the index of the object.
);

/**
 * @brief
 * Extracts the maximal size of an R-class in the syntactic monoid of an object
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The maximal size of an R-class in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_rmaxsize(int i //!< the index of the object.
);

/**
 * @brief
 * Extracts the maximal size of an L-class in the syntactic monoid of an object
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The maximal size of an L-class in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_lmaxsize(int i //!< the index of the object.
);

/**
 * @brief
 * Extracts the maximal size of a J-class in the syntactic monoid of an object
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The maximal size of a J-class in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_jmaxsize(int i //!< the index of the object.
);

/**
 * @brief
 * Extracts the maximal size of an H-class in the syntactic monoid of an object
 *
 * @remark
 * The syntactic morphism of the object is computed if it has not already been computed.
 *
 * @return
 * The maximal size of an H-class in the syntactic monoid of the object or UINT_MAX if the object is a recursive definition or if the syntactic morphism could not be computed.
 */
uint shell_extract_hmaxsize(int i //!< the index of the object.
);

/**********************************/
/*+ Management of stored objects +*/
/**********************************/

typedef enum
{
    FILTER_MINI,     //!< Filter by the size of the minimal automaton.
    FILTER_SYNT,     //!< Filter by the size of the syntactic monoid.
    FILTER_IDEMS,    //!< Filter by the number of idempotents in the syntactic monoid.
    FILTER_RNUM,     //!< Filter by the number of R-classes in the syntactic monoid.
    FILTER_LNUM,     //!< Filter by the number of L-classes in the syntactic monoid.
    FILTER_JNUM,     //!< Filter by the number of J-classes in the syntactic monoid.
    FILTER_HNUM,     //!< Filter by the number of H-classes in the syntactic monoid.
    FILTER_RMAXSIZE, //!< Filter by the maximal size of an R-class in the syntactic monoid.
    FILTER_LMAXSIZE, //!< Filter by the maximal size of an L-class in the syntactic monoid.
    FILTER_JMAXSIZE, //!< Filter by the maximal size of a J-class in the syntactic monoid.
    FILTER_HMAXSIZE, //!< Filter by the maximal size of an H-class in the syntactic monoid.
    FILTER_NUMSIZE
} num_filter;

extern uint (*num_extractors[FILTER_NUMSIZE])(int i); //!< Array of pointers to functions that extract numerical values from objects.

typedef enum
{
    FILTER_NOSIMC,         //!< No simple counter.
    FILTER_NOSMALLCOUNTER, //!< No small counter.
    FILTER_NOSINK,         //!< No sink state.
    FILTER_NOSELFLOOPS,    //!< No self-loops.
    FILTER_BOOLSIZE
} bool_filter;

extern bool (*bool_extractors[FILTER_BOOLSIZE])(int i); //!< Array of pointers to functions that extract boolean values from objects.

#define FILTER_MAXCLASSES 64 //!< Maximum number of classes for the filters.

typedef struct
{
    bool used_boolean[FILTER_BOOLSIZE];  //!< The boolean filters used.
    bool sign_boolean[FILTER_BOOLSIZE];  //!< The sign of the boolean filters used (true for positive, false for negative).
    bool used_numerical[FILTER_NUMSIZE]; //!< The numerical filters used.
    int values[FILTER_NUMSIZE][2];       //!< The values for the numerical filters, where values[i][0] is the lower bound and values[i][1] is the upper bound.
    uchar nblow;                         //!< The number of classes to exclude in the filter.
    uchar nbhigh;                        //!< The number of classes to include in the filter.
    classes low[FILTER_MAXCLASSES];      //!< The classes to exclude in the filter.
    classes high[FILTER_MAXCLASSES];     //!< The classes to include in the filter.
    bool order;                          //!< Is the ordering on monoids required for the classes tests.
} filters_info;

/**
 * @brief
 * Retrieves the filters from the command parameters.
 *
 * @return
 * -1 in case of success, -2 in case of error.
 */
int shell_retrieve_filters(com_parameters *pars, //!< The parameters of the command.
                           filters_info *info    //!< The structure to fill with the filters information.
);

/**
 * @brief
 *  Displays of all objects of a given type in memory.
 *
 * @return
 * -1 in case of success, -2 in case of error.
 */
int shell_filter_objects(com_parameters *pars, //!< The parameters of the command.
                         ob_type               //!< The type of the objects to display.
);

/***********/
/* Sorting */
/***********/

/**
 * @brief
 * Default comparison function for objects. Sorts according to the
 * variable name.
 *
 * @return
 * A negative value if the first object should be placed before the second one,
 * a positive value if the second object should be placed before the first one,
 * and 0 if the two objects are equal.
 */
int object_compare(int, //!< The index of the first object.
                   int  //!< The index of the second object.
);

/**
 * @brief
 * Pointer to the function that extracts a numerical value from an object for the object_compare_numerical function.
 *
 * @remark
 * Initially set to NULL.
 */
extern uint (*extractor)(int);

/**
 * @brief
 * Comparison function for objects. Sorts according to the numerical value given by the extractor function.
 *
 * @remark
 * Recursive definition objects are placed at the end of the list (they are not considered by extarctor functions).
 *
 * @return
 * A negative value if the first object should be placed before the second one,
 * a positive value if the second object should be placed before the first one,
 * and 0 if the two objects are equal.
 */
int object_compare_numerical(int i1, int i2);

/**
 * @brief
 * Sorts the array of objects according to the comparison function passed as input.
 *
 * @remark
 * Uses a heap sort algorithm.
 */
void object_sort_array(int (*comp)(int, int) //!< The comparison function.
);

/**
 * @brief
 * Sorts the objects array according to a comparison function given as input.
 */
int shell_sort(com_parameters *pars);

/**************/
/*+ Deletion +*/
/**************/

/**
 * @brief
 * Deletes all object that do not satify the filters
 *
 * @remark
 * -1 in case of success, -2 in case of error.
 */
int shell_filter_delete(com_parameters *pars, ob_type type);

/**
 * @brief
 * Deletes an object.
 *
 * @remark
 * -1 in case of success, -2 in case of error.
 */
int shell_delete(com_parameters *pars, //!< The parameters of the command.
                 const char *str       //!< The string used to call the command.
);

/**
 * @brief
 * Deletes all objects in memory.
 *
 * @remark
 * -1 in case of success, -2 in case of error.
 */
int shell_delete_all(com_parameters *pars, //!< The parameters of the command.
                     const char *str       //!< The string used to call the command.
);

/***************************/
/*+ Recursive definitions +*/
/***************************/

/**
 * @brief
 * Initializes a recursive definition.
 *
 * @remark
 * Index of the new object in case of success, -2 in case of error.
 */
int shell_recursive_init(char *varname,        //!< The variable name for the new object.
                         com_parameters *pars, //!< The parameters of the command.
                         const char *str       //!< string used to call the command.
);

/**
 * @brief
 * Deletes the state names in an NFA.
 *
 * @return
 * -1 in case of success, -2 in case of error.
 */
int shell_reset(com_parameters *pars, //!< The parameters of the command.
                const char *str       //!< string used to call the command.
);

#endif // SHELL_FILTERING_H
