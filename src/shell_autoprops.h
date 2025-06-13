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
#include "nfa_props.h"

bool shell_autoprop_trivial(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


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

bool shell_autoprop_dapluspat(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_dagppat(int j, //!< Index of the automaton in the objects array.
    dfagp_mode mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_daplusgppat(int j, //!< Index of the automaton in the objects array.
    dfagp_mode mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_cyclet(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_cycletplus(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_cycletgp(int j, //!< Index of the automaton in the objects array.
    dfagp_mode mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_cycletplusgp(int j, //!< Index of the automaton in the objects array.
    dfagp_mode mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_cycletbp(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_cycletbpgp(int j, //!< Index of the automaton in the objects array.
    dfagp_mode mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_nosimplecounter(int j, char* name, int* error, FILE* out);


