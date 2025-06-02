/**
 * @file shell_morprops.h
 * @brief Shell functions testing properties of automata.
 *
 * @attention
 * All functions must be called on indices of the objects array that refer to automata.ACCESSX_MAX_TABLESIZE
 * This is not checked in the functions.
 */


#include "alloc.h"
#include "shell_languages.h"


bool shell_autoprop_letterind(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_commut(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_permut(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_cfree(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_dapat(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_cycletrivial(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);






