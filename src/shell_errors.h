/**
 * @file shell_erros.h
 * @brief
 * Error messages in the shell.
 */
#ifndef ERRORS_H_
#define ERRORS_H_

#include "shell_keywords.h"
#include "tools.h"
#include <limits.h>
#include <stdio.h>

enum shell_errors {
    INTERRUPTION = INT_MIN,
    COMPUTATION_FAILURE,
    SYNTAX_ERROR,
    INVALID_OBJECT,
    MEMORY_LIMIT,
    TIMEOUT_OCCURRED,
};

/**
 * @brief
 * Dispatches the error.
 */
void shell_error_dispatch(int err);

/**
 * @brief
 * Generic syntax error.
 */
void shell_error_syntax(void);

/**
 * @brief
 * Objects table full.
 */
void shell_error_full(void);

/**
 * @brief
 * Unknown variable.
 */
void shell_error_unknownvar(const char * //!< The variable name.
);

/**
 * @brief
 * Disallowed variable.
 */
void shell_error_disallowedvar(const char * //!< The variable name.
);

/**
 * @brief
 * Unknown relation in a recursive definition.
 */
void shell_error_unknownrel(const char * //!< The variable name.
                            ,
                            const char * //!< The relation name.
);

/**
 * @brief
 * Not a symbolic regular expression.
 */
void shell_error_notrecdef(com_command * //!< The command.
);

/**
 * @brief
 * Not a regular expression
 */
void shell_error_notregexp(com_command * //!< The command.
);

/**
 * @brief
 * Used a wrong index in the definition of a recursive relation.
 */
void shell_error_wrongrecindex(uint //!< The index used.
                               ,
                               const char * //!< The variable name.
);

/**
 * @brief
 * Tried to copy a recursive definition.
 */
void shell_error_copyrec(void);

/**
 * @brief
 * Not a recursive definition variable.
 */
void shell_error_notrecvar(const char * //!< The variable name.
);

/**
 * @brief
 * Error in the specification.
 */
void shell_error_recnotok(const char * //!< The variable name.
);

/**
 * @brief
 * The command does not return anything
 */
void shell_error_noreturn(com_command * //!< The command
);

/**
 * @brief
 * The number of parameters is wrong
 */
void shell_error_leastparams(const char * //!< The command name
                             ,
                             uchar //!< The expected number of parameters
);

/**
 * @brief
 * The number of parameters is wrong
 */
void shell_error_nbparams(const char * //!< The command name
                          ,
                          uchar //!< The expected number of parameters
);

/**
 * @brief
 * The number of parameters is wrong
 */
void shell_error_rangeparams(const char * //!< The command name
                             ,
                             uchar //!< The low number of parameters
                             ,
                             uchar //!< The high number of parameters
);

/**
 * @brief
 * Parameter should be a regexp
 */
void shell_error_exppar(const char * //!< The command name
                        ,
                        uchar //!< The parameter
);

/**
 * @brief
 * Parameter should be an automaton
 */
void shell_error_autopar(const char * //!< The command name
                         ,
                         uchar //!< The parameter
);

/**
 * @brief
 * Parameter should be a morphism
 */
void shell_error_morpar(const char * //!< The command name
                        ,
                        uchar //!< The parameter
);

/**
 * @brief
 * Parameter should be a number
 */
void shell_error_numpar(const char * //!< The command name
                        ,
                        uchar //!< The parameter
);

/**
 * @brief
 * Parameter should be a number
 */
void shell_error_filepar(const char * //!< The command name
                         ,
                         uchar //!< The parameter
);

/**
 * @brief
 * Parameter should be a number
 */
void shell_error_wordpar(const char * //!< The command name
                         ,
                         uchar //!< The parameter
);

/**
 * @brief
 * Parameter should be a variable
 */
void shell_error_varpar(const char * //!< The command name
                        ,
                        uchar //!< The parameter
);

/**
 * @brief
 * Parameter should be a class
 */
void shell_error_classpar(const char * //!< The command name
                          ,
                          uchar //!< The parameter
);

/**
 * @brief
 * Parameter is invalid.
 */
void shell_error_invalidpar(const char * //!< The command name
                            ,
                            com_command * //!< The parameter
);

/**
 * @brief
 * Class not supported
 */
void shell_error_classnsup(com_command * //!< The class.
                           ,
                           const char * //!< The command name.
);

/**
 * @brief
 * NULL
 */
void shell_error_null(void);

#endif
