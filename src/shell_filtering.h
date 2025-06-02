/**
 * @file shell_filtering.h
 * @brief Header file for objects filtering in the shell.
 */


#ifndef SHELL_FILTERING_H
#define SHELL_FILTERING_H


#include "shell_commands.h"


 /**********************************/
 /*+ Management of stored objects +*/
 /**********************************/

 /**
  * @brief
  *  Displays of all objects of a given type in memory.
  */
int shell_filter_objects(com_parameters* pars, //!< The parameters of the command.
    ob_type //!< The type of the objects to display.
);

/**
 * @brief
 * Sorts the objects array according to a comparison function given as input.
 */
int shell_sort(com_parameters* pars);


/**
 * @brief
 * Deletes an object.
 *
 * @remark
 * -1 in case of success, -2 in case of error.
 */
int shell_delete(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);

/**
 * @brief
 * Deletes all objects in memory.
 *
 * @remark
 * -1 in case of success, -2 in case of error.
 */
int shell_delete_all(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< The string used to call the command.
);


/**
 * @brief
 * Initializes a recursive definition.
 *
 * @remark
 * Index of the new object in case of success, -2 in case of error.
 */
int shell_recursive_init(char* varname, //!< The variable name for the new object.
    com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command. 
);

/**
 * @brief
 * Deletes the state names in an NFA.
 *
 * @return
 * -1 in case of success, -2 in case of error.
 */
int shell_reset(com_parameters* pars, //!< The parameters of the command.
    const char* str //!< string used to call the command. 
);






#endif // SHELL_FILTERING_H
