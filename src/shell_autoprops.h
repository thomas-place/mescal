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

bool shell_autoprop_ddtrivial(int j, //!< Index of the automaton in the objects array.
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

bool shell_autoprop_alphaidem(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_ddidem(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_ddcomm(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_lttcomm(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_permut(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_permutplus(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_cfreegp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_polgp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);



bool shell_autoprop_polgpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_pol2gp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_pol2gpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);




bool shell_autoprop_rtrivialgp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_rtrivialgpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_rtrivialbpgp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_rtrivialbpgpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_ltrivialgp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_ltrivialgp_opti(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    int* error //!< Error code (NULL if no error is desired).
);

bool shell_autoprop_ltrivialgpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_ltrivialgpplus_opti(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    int* error //!< Error code (NULL if no error is desired).
);

bool shell_autoprop_ltrivialbpgp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_ltrivialbpgp_opti(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    int* error //!< Error code (NULL if no error is desired).
);

bool shell_autoprop_ltrivialbpgpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_dagp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_dagpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_dabpgp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_dabpgpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_bpolmod(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_bpolamt(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_bpolamtplus(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_bpolgrplus(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_knastgpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_piecewise(int j, //!< Index of the automaton in the objects array.
    int* error //!< Error code (NULL if no error is desired).
);

bool shell_autoprop_knastat(int j, //!< Index of the automaton in the objects array.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);


bool shell_autoprop_upolgpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_upolbp2gp(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_upolbp2gpplus(int j, //!< Index of the automaton in the objects array.
    basis mode, //!< The mode of the DA pattern to test.
    char* name, //!< Name of the object (for the display)
    int* error, //!< Error code (NULL if no error is desired).
    FILE* out //!< Output stream (NULL is no output is desired).
);

bool shell_autoprop_nosimplecounter(int j, char* name, int* error, FILE* out);


