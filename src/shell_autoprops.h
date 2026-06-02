/**
 * @file shell_morprops.h
 * @brief Shell functions testing properties of automata.
 *
 * @attention
 * All functions must be called on indices of the objects array that refer to automata.ACCESSX_MAX_TABLESIZE
 * This is not checked in the functions.
 */

#include "alloc.h"
#include "nfa_props.h"
#include "shell_languages.h"

bool shell_autoprop_letterind(int j,      //!< Index of the automaton in the objects array.
                              int *error, //!< Error code (NULL if no error is desired).
                              FILE *out   //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_commut(int j,      //!< Index of the automaton in the objects array.
                           int *error, //!< Error code (NULL if no error is desired).
                           FILE *out   //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_permut(int j,      //!< Index of the automaton in the objects array.
                           int *error, //!< Error code (NULL if no error is desired).
                           FILE *out   //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_permutplus(int j,      //!< Index of the automaton in the objects array.
                               int *error, //!< Error code (NULL if no error is desired).
                               FILE *out   //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_sfgp(int j,      //!< Index of the automaton in the objects array.
                         basis mode, //!< The mode of the DA pattern to test.
                         int *error, //!< Error code (NULL if no error is desired).
                         FILE *out   //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_rtrivialgp(int j,      //!< Index of the automaton in the objects array.
                               basis mode, //!< The mode of the DA pattern to test.
                               int *error, //!< Error code (NULL if no error is desired).
                               FILE *out   //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_dagp(int j,      //!< Index of the automaton in the objects array.
                         basis mode, //!< The mode of the DA pattern to test.
                         int *error, //!< Error code (NULL if no error is desired).
                         FILE *out   //!< Output stream (NULL is no output is desired).
);

// bool shell_autoprop_dagp_opti(int j,      //!< Index of the automaton in the objects array.
//                               basis mode, //!< The mode of the DA pattern to test.
//                               int *error, //!< Error code (NULL if no error is desired).
//                               FILE *out   //!< Output stream (NULL is no output is desired).
// );

bool shell_autoprop_nosimplecounter(int j, int *error, FILE *out);
