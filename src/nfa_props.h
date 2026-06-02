/**
 * @file monoid_props.h
 * @brief
 * Tests of properties on DFAs (mostly) related to languages classes.
 */

#ifndef NFA_PROPS_H
#define NFA_PROPS_H

#include "nfa.h"
#include "nfa_determi.h"
#include "nfa_intersec.h"
#include "nfa_props.h"

/*  _   _ _____ _        ____                            _   _            */
/* | \ | |  ___/ \   _  |  _ \ _ __ ___  _ __   ___ _ __| |_(_) ___  ___  */
/* |  \| | |_ / _ \ (_) | |_) | '__/ _ \| '_ \ / _ \ '__| __| |/ _ \/ __| */
/* | |\  |  _/ ___ \ _  |  __/| | | (_) | |_) |  __/ |  | |_| |  __/\__ \ */
/* |_| \_|_|/_/   \_(_) |_|   |_|  \___/| .__/ \___|_|   \__|_|\___||___/ */
/*                                      |_|                               */

/**************/
/*+ Messages +*/
/**************/

#define MAKE_PATTERN_INTRO(pattern, out)                                                                 \
    do                                                                                                   \
    {                                                                                                    \
        if (out)                                                                                         \
        {                                                                                                \
            fprintf(out, "#### Checking if this automaton satisfies the following pattern equation:\n"); \
            view_image(pattern);                                                                         \
        }                                                                                                \
    } while (0)

#define MAKE_PATTERN_INTRO_STRICT(pattern, out)                                                                               \
    do                                                                                                                        \
    {                                                                                                                         \
        if (out)                                                                                                              \
        {                                                                                                                     \
            fprintf(out, "#### Checking if this automaton satisfies the following pattern equation for strict matchings:\n"); \
            view_image(pattern);                                                                                              \
        }                                                                                                                     \
    } while (0)

#define CONCLUSION_POSITIVE(out)                                       \
    do                                                                 \
    {                                                                  \
        if (out)                                                       \
        {                                                              \
            fprintf(out, "#### The pattern equation is satisfied.\n"); \
        }                                                              \
    } while (0)

#define CONCLUSION_NEGATIVE(out)                                           \
    do                                                                     \
    {                                                                      \
        if (out)                                                           \
        {                                                                  \
            fprintf(out, "#### The pattern equation is not satisfied.\n"); \
        }                                                                  \
    } while (0)

// Star-free patterns
#define SF_PATTERN_ST " ./patterns/sf/sfst.png"
#define SF_PATTERN_MOD " ./patterns/sf/sfmod.png"
#define SF_PATTERN_AMT " ./patterns/sf/sfamt.png"
#define SF_PATTERN_GR " ./patterns/sf/sfgr.png"

// L-trivial patterns
#define LTRIV_PATTERN_ST " ./patterns/ltrivial/ltrivialst.png"
#define LTRIV_PATTERN_MOD " ./patterns/ltrivial/ltrivialmod.png"
#define LTRIV_PATTERN_AMT " ./patterns/ltrivial/ltrivialamt.png"
#define LTRIV_PATTERN_GR " ./patterns/ltrivial/ltrivialgr.png"
#define LTRIV_PATTERN_ST_OPTI " ./patterns/ltrivial/ltrivialstopti.png"
#define LTRIV_PATTERN_MOD_OPTI " ./patterns/ltrivial/ltrivialmodopti.png"
#define LTRIV_PATTERN_AMT_OPTI " ./patterns/ltrivial/ltrivialamtopti.png"
#define LTRIV_PATTERN_GR_OPTI " ./patterns/ltrivial/ltrivialgropi.png"
#define LTRIV_PATTERN_ST_PLUS " ./patterns/ltrivial/ltrivialstplus.png"
#define LTRIV_PATTERN_MOD_PLUS " ./patterns/ltrivial/ltrivialmodplus.png"
#define LTRIV_PATTERN_AMT_PLUS " ./patterns/ltrivial/ltrivialamtplus.png"
#define LTRIV_PATTERN_GR_PLUS " ./patterns/ltrivial/ltrivialgrplus.png"
#define LTRIV_PATTERN_BPST " ./patterns/ltrivial/ltrivialbpst.png"
#define LTRIV_PATTERN_BPMOD " ./patterns/ltrivial/ltrivialbpmod.png"
#define LTRIV_PATTERN_BPAMT " ./patterns/ltrivial/ltrivialbpamt.png"
#define LTRIV_PATTERN_BPGR " ./patterns/ltrivial/ltrivialbpgr.png"
#define LTRIV_PATTERN_BPST_PLUS " ./patterns/ltrivial/ltrivialbpstplus.png"
#define LTRIV_PATTERN_BPMOD_PLUS " ./patterns/ltrivial/ltrivialbpmodplus.png"
#define LTRIV_PATTERN_BPAMT_PLUS " ./patterns/ltrivial/ltrivialbpamtplus.png"
#define LTRIV_PATTERN_BPGR_PLUS " ./patterns/ltrivial/ltrivialbpgrplus.png"

// R-trivial patterns
#define RTRIV_PATTERN_ST " ./patterns/rtrivial/rtrivialst.png"
#define RTRIV_PATTERN_MOD " ./patterns/rtrivial/rtrivialmod.png"
#define RTRIV_PATTERN_AMT " ./patterns/rtrivial/rtrivialamt.png"
#define RTRIV_PATTERN_GR " ./patterns/rtrivial/rtrivialgr.png"
#define RTRIV_PATTERN_ST_PLUS " ./patterns/rtrivial/rtrivialstplus.png"
#define RTRIV_PATTERN_MOD_PLUS " ./patterns/rtrivial/rtrivialmodplus.png"
#define RTRIV_PATTERN_AMT_PLUS " ./patterns/rtrivial/rtrivialamtplus.png"
#define RTRIV_PATTERN_GR_PLUS " ./patterns/rtrivial/rtrivialgrplus.png"
#define RTRIV_PATTERN_BPST " ./patterns/rtrivial/rtrivialbpst.png"
#define RTRIV_PATTERN_BPMOD " ./patterns/rtrivial/rtrivialbpmod.png"
#define RTRIV_PATTERN_BPAMT " ./patterns/rtrivial/rtrivialbpamt.png"
#define RTRIV_PATTERN_BPGR " ./patterns/rtrivial/rtrivialbpgr.png"
#define RTRIV_PATTERN_BPST_PLUS " ./patterns/rtrivial/rtrivialbpstplus.png"
#define RTRIV_PATTERN_BPMOD_PLUS " ./patterns/rtrivial/rtrivialbpmodplus.png"
#define RTRIV_PATTERN_BPAMT_PLUS " ./patterns/rtrivial/rtrivialbpamtplus.png"
#define RTRIV_PATTERN_BPGR_PLUS " ./patterns/rtrivial/rtrivialbpgrplus.png"

// DA patterns
#define DA_PATTERN_ST " ./patterns/da/dast.png"
#define DA_PATTERN_MOD " ./patterns/da/damod.png"
#define DA_PATTERN_AMT " ./patterns/da/daamt.png"
#define DA_PATTERN_GR " ./patterns/da/dagr.png"
#define DA_PATTERN_ST_OPTI " ./patterns/da/dastopti.png"
#define DA_PATTERN_MOD_OPTI " ./patterns/da/damodopti.png"
#define DA_PATTERN_AMT_OPTI " ./patterns/da/daamtopti.png"
#define DA_PATTERN_GR_OPTI " ./patterns/da/dagropi.png"
#define DA_PATTERN_ST_PLUS " ./patterns/da/dastplus.png"
#define DA_PATTERN_MOD_PLUS " ./patterns/da/damodplus.png"
#define DA_PATTERN_AMT_PLUS " ./patterns/da/daamtplus.png"
#define DA_PATTERN_GR_PLUS " ./patterns/da/dagrplus.png"
#define DA_PATTERN_BPST " ./patterns/da/dabpst.png"
#define DA_PATTERN_BPMOD " ./patterns/da/dabpmod.png"
#define DA_PATTERN_BPAMT " ./patterns/da/dabpamt.png"
#define DA_PATTERN_BPGR " ./patterns/da/dabpgr.png"
#define DA_PATTERN_BPST_PLUS " ./patterns/da/dabpstplus.png"
#define DA_PATTERN_BPMOD_PLUS " ./patterns/da/dabpmodplus.png"
#define DA_PATTERN_BPAMT_PLUS " ./patterns/da/dabpamtplus.png"
#define DA_PATTERN_BPGR_PLUS " ./patterns/da/dabpgrplus.png"

// POL patterns
#define POL_PATTERN_ST " ./patterns/pol/polst.png"
#define POL_PATTERN_MOD " ./patterns/pol/polmod.png"
// #define POL_PATTERN_AMT " ./patterns/pol/polamt.png"
#define POL_PATTERN_GR " ./patterns/pol/polgr.png"
#define POL_PATTERN_GR_SPE " ./patterns/pol/polgrspe.png"
#define POL_PATTERN_ST_PLUS " ./patterns/pol/polstplus.png"
#define POL_PATTERN_MOD_PLUS " ./patterns/pol/polmodplus.png"
// #define POL_PATTERN_AMT_PLUS " ./patterns/pol/polamtplus.png"
#define POL_PATTERN_GR_PLUS " ./patterns/pol/polgrplus.png"
#define POL2_PATTERN_ST " ./patterns/pol/pol2st.png"
#define POL2_PATTERN_MOD " ./patterns/pol/pol2mod.png"
#define POL2_PATTERN_AMT " ./patterns/pol/pol2amt.png"
#define POL2_PATTERN_GR " ./patterns/pol/pol2gr.png"
#define POL2_PATTERN_ST_PLUS " ./patterns/pol/pol2stplus.png"
#define POL2_PATTERN_MOD_PLUS " ./patterns/pol/pol2modplus.png"
#define POL2_PATTERN_AMT_PLUS " ./patterns/pol/pol2amtplus.png"
#define POL2_PATTERN_GR_PLUS " ./patterns/pol/pol2grplus.png"

// CoPOL patterns
#define COPOL_PATTERN_ST " ./patterns/pol/copolst.png"
#define COPOL_PATTERN_MOD " ./patterns/pol/copolmod.png"
// #define COPOL_PATTERN_AMT " ./patterns/pol/copolamt.png"
#define COPOL_PATTERN_GR " ./patterns/pol/copolgr.png"
#define COPOL_PATTERN_GR_SPE " ./patterns/pol/copolgrspe.png"
#define COPOL_PATTERN_ST_PLUS " ./patterns/pol/copolstplus.png"
#define COPOL_PATTERN_MOD_PLUS " ./patterns/pol/copolmodplus.png"
// #define COPOL_PATTERN_AMT_PLUS " ./patterns/pol/copolamtplus.png"
#define COPOL_PATTERN_GR_PLUS " ./patterns/pol/copolgrplus.png"
#define COPOL2_PATTERN_ST " ./patterns/pol/copol2st.png"
#define COPOL2_PATTERN_MOD " ./patterns/pol/copol2mod.png"
#define COPOL2_PATTERN_AMT " ./patterns/pol/copol2amt.png"
#define COPOL2_PATTERN_GR " ./patterns/pol/copol2gr.png"
#define COPOL2_PATTERN_ST_PLUS " ./patterns/pol/copol2stplus.png"
#define COPOL2_PATTERN_MOD_PLUS " ./patterns/pol/copol2modplus.png"
#define COPOL2_PATTERN_AMT_PLUS " ./patterns/pol/copol2amtplus.png"
#define COPOL2_PATTERN_GR_PLUS " ./patterns/pol/copol2grplus.png"

// BPol patterns
#define BPOL_PATTERN_MOD " ./patterns/bpol/bpolmod.png"
#define BPOL_PATTERN_AMT " ./patterns/bpol/bpolamt.png"
#define BPOL_PATTERN_ST_PLUS " ./patterns/bpol/bpolstplus.png"
#define BPOL_PATTERN_MOD_PLUS " ./patterns/bpol/bpolmodplus.png"
#define BPOL_PATTERN_AMT_PLUS " ./patterns/bpol/bpolamtplus.png"
#define BPOL_PATTERN_GR_PLUS " ./patterns/bpol/bpolgrplus.png"
#define BPOL2_PATTERN_ST " ./patterns/bpol/bpol2st.png"
#define KNAST_PATTERN_MOD_PLUS " ./patterns/bpol/knastmodplus.png"
#define KNAST_PATTERN_AMT_PLUS " ./patterns/bpol/knastamtplus.png"
#define KNAST_PATTERN_GR_PLUS " ./patterns/bpol/knastgrplus.png"

// UPPol patterns
#define UPOL_PATTERN_ST_PLUS " ./patterns/upol/upolstplus.png"
#define UPOL_PATTERN_MOD_PLUS " ./patterns/upol/upolmodplus.png"
#define UPOL_PATTERN_AMT_PLUS " ./patterns/upol/upolamtplus.png"
#define UPOL_PATTERN_GR_PLUS " ./patterns/upol/upolgrplus.png"
#define UPOL3_PATTERN_ST " ./patterns/upol/upol3st.png"
#define UPOL3_PATTERN_MOD " ./patterns/upol/upol3mod.png"
#define UPOL3_PATTERN_AMT " ./patterns/upol/upol3amt.png"
#define UPOL3_PATTERN_GR " ./patterns/upol/upol3gr.png"
#define UPOL3_PATTERN_ST_PLUS " ./patterns/upol/upol3stplus.png"
#define UPOL3_PATTERN_MOD_PLUS " ./patterns/upol/upol3modplus.png"
#define UPOL3_PATTERN_AMT_PLUS " ./patterns/upol/upol3amtplus.png"
#define UPOL3_PATTERN_GR_PLUS " ./patterns/upol/upol3grplus.png"

// Leftover patterns
#define DD_PATTERN " ./patterns/vrac/dd.png"
#define GR_PATTERN " ./patterns/vrac/gr.png"
#define GRPLUS1_PATTERN " ./patterns/vrac/grplus1.png"
#define GRPLUS2_PATTERN " ./patterns/vrac/grplus2.png"
#define COMM_PATTERN " ./patterns/vrac/commutative.png"
#define IDEM_PATTERN " ./patterns/vrac/idempotent.png"
#define LTIDEM_PATTERN " ./patterns/vrac/ltidem.png"
#define LTCOMM_PATTERN " ./patterns/vrac/ltcommutative.png"
#define LTTCOMM_PATTERN " ./patterns/vrac/lttcommutative.png"

typedef enum
{
    PAT_WORD,
    PAT_SING,
    PAT_LANG,
    PAT_NONE,
} pattern_vartype;

#define PATTERN_MAXSIZE 10

typedef struct
{
    uint states[PATTERN_MAXSIZE];
    uint *words[PATTERN_MAXSIZE];
    char vars[PATTERN_MAXSIZE];
    pattern_vartype types[PATTERN_MAXSIZE];
} generic_pattern;

generic_pattern *make_generic_pattern(void);

void free_generic_pattern(generic_pattern *pattern);

uint *make_singleton_word(uint letter);
uint *make_epsilon_word(void);
uint *copy_word(uint *word);

/***********/
/* Trivial */
/***********/

/**
 * @brief
 * Tests if a complete DFA is trivial (i.e., has a single state).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA is trivial.
 */
bool is_trivial_dfa(dfa *A,   //!< The DFA.
                    FILE *out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the DD pattern equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the DD pattern equation.
 */
bool is_ddtrivial_dfa(dfa *A,   //!< The DFA.
                      FILE *out //!< The stream.
);

/**
 * @brief
 * Tests if all letters have the same action in a complete DFA.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether all letters have the same action.
 */
bool is_letterind_dfa(dfa *A,   //!< The DFA.
                      FILE *out //!< The stream.
);

/*********/
/* Group */
/*********/

/**
 * @brief
 * Tests is a complete DFA is a permutation automata.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA is a permutation automata.
 */
bool is_permutation_dfa(dfa *A,   //!< The NFA.
                        FILE *out //!< The stream.
);

/**
 * @brief
 * Tests is an arbitrary DFA satisfies the GR⁺ pattern equations.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the GR⁺ pattern equation.
 */
bool is_permutationplus_dfa(dfa *A,   //!< The DFA.
                            FILE *out //!< The stream.
);

/*********/
/* Local */
/*********/

/**
 * @brief
 * Tests is a complete DFA is commutative.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA is commutative.
 */
bool is_comm_dfa(dfa *A,   //!< The DFA.
                 FILE *out //!< The stream.
);

/**
 * @brief
 * Tests is a complete commutative DFA is idempotent for the alphabet aa = a (used in AT).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The result is undefined if the DFA is not commutative.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA is idempotent for the alphabet.
 */
bool is_alphaidem_dfa(dfa *, //!< The DFA.
                      FILE * //!< The stream.
);

/**
 * @brief
 * Tests is a complete DFA satisfies the DD idempotent pattern equation (used in LT).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the DD idempotent pattern equation
 */
bool is_ddidem_dfa(dfa *A,   //!< The DFA.
                   FILE *out //!< The stream.
);

/**
 * @brief
 * Tests is a complete DFA satisfies the DD commutative pattern equation (used in LT).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the DD commutative pattern equation
 */
bool is_ddcomm_dfa(dfa *A,     //!< The DFA.
                   int *error, //!< The error code.
                   FILE *out   //!< The stream.
);

bool is_lt_opti_dfa(dfa *A);

/**
 * @brief
 * Tests is a complete DFA satisfies the LTT commutative pattern equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the LTT commutative pattern equation
 */
bool is_lttcomm_dfa(dfa *,      //!< The DFA.
                    int *error, //!< The error code.
                    FILE *      //!< The stream.
);

/*************************/
/* Counter-free variants */
/*************************/

typedef struct
{
    uint nb_states;
    uint *states;
    uint *word;
} sfc_pattern;

/**
 * @brief
 * Tests if a DFA is G-counterfree for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is used to display a counter if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism (unavoidable in this case, the problem is
 * PSPACE-complete).
 *
 * @return
 * A Boolean indicating whether the NFA is counterfree DFA.
 */
bool is_counterfreegp_dfa(dfa *A,     //!< The DFA.
                          basis mode, //!< The mode indicating which group prevariety to test.
                          int *error, //!< The error code.
                          FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA does not contain any counter whose size at most two.
 *
 * @remark
 * Used in filtering to look for more involved examples.
 *
 * @return
 * A Boolean indicating whether the DFA does not contain any counter of size at most two.
 */
bool is_nosmallcountertwo_dfa(dfa *A //!< The DFA.
);

/**
 * @brief
 * Tests if a complete DFA does not contain any simple counter.
 *
 * @details
 * A simple counter is a counter labeled by a single letter word.
 *
 * @remark
 * Used to speed up example searches in some cases, by allowing to skip many
 * non-star-free examples quickly.
 *
 * @return
 * A Boolean indicating whether the DFA does not contain any simple counter.
 */
bool is_nosimple_counter_dfa(dfa *A,    //!< The DFA.
                             int *error //!< The error code.
);

/****************/
/*+ R-trivial. +*/
/****************/

/**
 * @brief
 * Tests if a complete DFA is R-trivial for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A pointer to a generic pattern is taken as input. It is used to return a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the group prevarieties
 */
bool is_rtrivialgp_dfa_noprint(dfa *A,               //!< The DFA.
                               basis mode,           //!< The basis
                               generic_pattern *cexa //!< Pointer to a generic pattern used to return a counterexample (must be of size two)
);

/**
 * @brief
 * Tests if a complete DFA is R-trivial for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the group prevarieties
 */
bool is_rtrivialgp_dfa(dfa *A,     //!< The DFA.
                       basis mode, //!< The basis
                       FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @remark
 * A pointer to a generic pattern is taken as input. It is used to return a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_rtrivialgpplus_dfa_noprint(dfa *A,               //!< The DFA.
                                   basis mode,           //!< The basis
                                   generic_pattern *cexa //!< Pointer to a generic pattern used to return a counterexample (must be of size two)
);

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_rtrivialgpplus_dfa(dfa *A,     //!< The DFA.
                           basis mode, //!< The basis
                           FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the four prevarieties BPol(ST), BPol(MOD), BPol(AMT) or BPol(GR).
 *
 * @remark
 * A pointer to a generic pattern is taken as input. It is used to return a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_rtrivialbpgp_dfa_noprint(dfa *A,               //!< The DFA.
                                 basis mode,           //!< The basis
                                 generic_pattern *cexa //!< Pointer to a generic pattern used to return a counterexample (must be of size two)
);

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the four prevarieties BPol(ST), BPol(MOD), BPol(AMT) or BPol(GR).
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_rtrivialbpgp_dfa(dfa *A,     //!< The DFA.
                         basis mode, //!< The mode of the cycle triviality test.
                         FILE *out   //!< The stream.
);

bool is_rtrivialbpgpplus_dfa_noprint(dfa *A, basis mode, generic_pattern *cexa);

/**
 * @brief
 * Tests if a DFA is R-trivial for one of the four prevarieties BPol(DD), BPol(MOD⁺), BPol(AMT⁺) or BPol(GR⁺).
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_rtrivialbpgpplus_dfa(dfa *A,     //!< The DFA.
                             basis mode, //!< The mode of the cycle triviality test.
                             FILE *out   //!< The stream.
);

/****************/
/*+ L-trivial. +*/
/****************/

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is L-trivial for one of the group prevarieties
 */
bool is_ltrivialgp_dfa(dfa *A,     //!< The DFA.
                       basis mode, //!< The basis
                       FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is L-trivial for one of the group prevarieties
 */
bool is_ltrivialgp_opti_dfa(dfa *A,     //!< The DFA.
                            basis mode, //!< The basis
                            FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the group prevarieties ST, MOD, AMT or GR.
 * Optimized version that checks rtriviality of the mirror automton.
 *
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is L-trivial for one of the group prevarieties
 */
bool is_ltrivialgp_mirror_dfa(dfa *A,    //!< The DFA.
                              basis mode //!< The basis
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties ST, MOD, AMT or GR when
 * the DFA is already known to be R-trivial.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is L-trivial
 */
bool is_ltrivialgp_rt_dfa(dfa *A,     //!< The DFA.
                          basis mode, //!< The basis
                          FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialgpplus_dfa(dfa *A,     //!< The DFA.
                           basis mode, //!< The basis
                           FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 * Optimized version that checks rtriviality of the mirror automton.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialgpplus_mirror_dfa(dfa *A,    //!< The DFA.
                                  basis mode //!< The basis
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialbpgp_dfa(dfa *A,     //!< The DFA.
                         basis mode, //!< The basis
                         FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 * Optimized version that checks rtriviality of the mirror automton.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialbpgp_mirror_dfa(dfa *A,    //!< The DFA.
                                basis mode //!< The basis
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialbpgpplus_dfa(dfa *A,     //!< The DFA.
                             basis mode, //!< The basis
                             int *error, //!< The error code.
                             FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a DFA is L-trivial for one of the four prevarieties DD, MOD⁺, AMT⁺ or GR⁺.
 * Optimized version that checks rtriviality of the mirror automton.
 *
 * @remark
 * A stream is taken as input. It is used to display a counterexample if one is found.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the DFA is R-trivial for one of the prevarieties
 */
bool is_ltrivialbpgpplus_mirror_dfa(dfa *A,    //!< The DFA.
                                    basis mode //!< The basis
);

/*******/
/* Pol */
/*******/

/**
 * @brief
 * Tests if a complete DFA satisfies the Pol(G) pattern equation for one of the group prevarieties ST, MOD or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Does not work for the group prevariety AMT.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the Pol(G) pattern equation.
 */
bool is_polgp_dfa(dfa *A,     //!< The complete DFA.
                  bool copol, //!< Whether to test the copol instead of pol.
                  basis mode, //!< The mode indicating which group prevariety to test.
                  FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the Pol(G) pattern equation for one of the group prevarieties ST, MOD or GR.
 * Optimized version with no counterexample output.
 *
 * @attention
 * Does not work for the group prevariety AMT.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the Pol(G) pattern equation.
 */
bool is_polgp_opti_dfa(dfa *A,     //!< The complete DFA.
                       bool copol, //!< Whether to test the copol instead of pol.
                       basis mode, //!< The mode indicating which group prevariety to test.
                       FILE *out);

/**
 * @brief
 * Tests if a complete DFA satisfies the Pol(G⁺) pattern equation where G is ST, MOD or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Does not work for the group prevariety AMT.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the Pol(G⁺) pattern equation.
 */
bool is_polgpplus_dfa(dfa *A,     //!< The complete DFA.
                      bool copol, //!< Whether to test the copol instead of pol.
                      basis mode, //!< The mode indicating which group prevariety to test.
                      FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the Pol₂(G) pattern equation for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the Pol₂(G) pattern equation.
 */
bool is_pol2gp_dfa(dfa *A,     //!< The complete DFA.
                   bool copol, //!< Whether to test the copol instead of pol.
                   basis mode, //!< The mode indicating which group prevariety to test.
                   FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the Pol₂(G⁺) pattern equation for one of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the Pol₂(G⁺) pattern equation.
 */
bool is_pol2gpplus_dfa(dfa *A,     //!< The complete DFA.
                       bool copol, //!< Whether to test the copol instead of pol.
                       basis mode, //!< The mode indicating which group prevariety to test.
                       FILE *out   //!< The stream.
);

/********/
/*+ DA +*/
/********/

typedef struct
{
    uint states[4];
    uint *words[3];
    pattern_vartype types[3];
} da_pattern;

/**
 * @brief
 * Tests if a complete DFA satisfies the classic DA pattern equation for one of the group
 * prevarieties ST, MOD, AMT or GR.
 *
 * @details
 * This function considers the classical DA pattern corresponding to the equation
 * (xy)^ω y (xy)^ω = (xy)^ω. It is slower that the optimized variant is_dagp_dfa.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_dagp_dfa(dfa *A,     //!< The complete DFA.
                 basis mode, //!< The mode indicating which group prevariety to test.
                 FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted DA pattern equation for one
 * of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_dagp_opti_dfa(dfa *A,     //!< The complete DFA.
                      basis mode, //!< The mode indicating which group prevariety to test.
                      FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted LDA pattern equation for one
 * of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_dagpplus_dfa(dfa *A,     //!< The complete DFA.
                     basis mode, //!< The mode indicating which group prevariety to test.
                     FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted LDA pattern equation for one
 * of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_dagpplus_opti_dfa(dfa *A,     //!< The complete DFA.
                          basis mode, //!< The mode indicating which group prevariety to test.
                          FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted BpolDA pattern equation for one
 * of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_dabpgp_dfa(dfa *A,     //!< The complete DFA.
                   basis mode, //!< The mode indicating which group prevariety to test.
                   FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted BpolDA pattern equation for one
 * of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_dabpgp_opti_dfa(dfa *A,     //!< The complete DFA.
                        basis mode, //!< The mode indicating which group prevariety to test.
                        FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted BpolplusDA pattern equation for one
 * of the group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_dabpgpplus_dfa(dfa *A,     //!< The complete DFA.
                       basis mode, //!< The mode indicating which group prevariety to test.
                       int *error, //!< The error code.
                       FILE *out   //!< The stream.
);

/**********/
/*+ BPol +*/
/**********/

/**
 * @brief
 * Tests if a complete DFA satisfies the PT equation.
 *
 * @remark
 * No display (use ltrivial + rtrivial for a counterexample if needed).
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the PT equation.
 */
bool is_piecewise_dfa(dfa *A //!< The DFA.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(MOD) equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(MOD) equation.
 */
bool is_bpolmod_dfa(dfa *A,   //!< The complete DFA.
                    FILE *out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(AMT) equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(AMT) equation.
 */
bool is_bpolamt_dfa(dfa *A,   //!< The complete DFA.
                    FILE *out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(DD) equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(DD) equation.
 */
bool is_bpoldd_dfa(dfa *A,   //!< The complete DFA.
                   FILE *out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the BPol(AMT⁺) equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the BPol(AMT⁺) equation.
 */
bool is_bpolamtplus_dfa(dfa *A,   //!< The complete DFA.
                        FILE *out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies theBPol(MOD⁺) or BPol(GR⁺) equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the complete DFA satisfies the equation.
 */
bool is_bpolgpplus_dfa(dfa *A,   //!< The complete DFA.
                       bool grp, //!< Whether to test the GR⁺ equation instead of the MOD⁺ one.
                       FILE *out //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted Knast(G⁺) pattern equation for one of the
 * four group prevarieties ST, MOD, AMT or GR.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the DFA satisfies the equation.
 */
bool is_knastgpplus_dfa(dfa *A,     //!< The complete DFA.
                        basis mode, //!< The mode indicating which group prevariety to test.
                        FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted Knast(AT) pattern equation.
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_knastat_dfa(dfa *A,   //!< The complete DFA.
                    FILE *out //!< The stream.
);

/**********/
/*+ UPol +*/
/**********/

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted UPol pattern equation for one
 * of the group prevarieties DD, MOD⁺, AMT⁺ or GR⁺. Classic version with counterexample
 * output (slower than the optimized version).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_upolgpplus_dfa(dfa *A,     //!< The complete DFA.
                       basis mode, //!< The mode indicating which group prevariety to test.
                       FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted UPol pattern equation for one
 * of the group prevarieties DD, MOD⁺, AMT⁺ or GR⁺. Optimized version with no counterexample output (faster than the classic version).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_upolgpplus_opti_dfa(dfa *A,    //!< The complete DFA.
                            basis mode //!< The mode indicating which group prevariety to test.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted UPol₂ pattern equation for one
 * of the group prevarieties ST, MOD, AMT or GR. Classic version with counterexample output (slower than the optimized version).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_upolbp2gp_dfa(dfa *A,     //!< The complete DFA.
                      basis mode, //!< The mode indicating which group prevariety to test.
                      FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted UPol₂ pattern equation for one
 * of the group prevarieties ST, MOD, AMT or GR. Optimized version with no counterexample output (faster than the classic version).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * The input DFA must be complete (this is not checked).
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_upolbp2gp_opti_dfa(dfa *A,    //!< The complete DFA.
                           basis mode //!< The mode indicating which group prevariety to test.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted UPol₂ pattern equation for one
 * of the group prevarieties DD, MOD⁺, AMT⁺ or GR⁺. Classic version with counterexample output (slower than the optimized version).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @attention
 * Inefficient: computes the transition morphism.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_upolbp2gpplus_dfa(dfa *A,     //!< The complete DFA.
                          basis mode, //!< The mode indicating which group prevariety to test.
                          int *error, //!< The error code.
                          FILE *out   //!< The stream.
);

/**
 * @brief
 * Tests if a complete DFA satisfies the restricted UPol₂ pattern equation for one
 * of the group prevarieties DD, MOD⁺, AMT⁺ or GR⁺. Optimized version with no counterexample output (faster than the classic version but still quite slow).
 *
 * @remark
 * A stream is taken as input. It is utilized to display the results of the computations.
 * If this is not desired, a NULL pointer should be given.
 *
 * @return
 * A Boolean indicating whether the NFA satisfies the equation.
 */
bool is_upolbp2gpplus_opti_dfa(dfa *A, basis mode);

/************************************************/
/*+ Additional ad-hoc properties for filtering +*/
/************************************************/

/**
 * @brief
 * Tests if a DFA (complete or not) has no sink state
 *
 * @return
 * A Boolean indicating whether the DFA has no sink state.
 */
bool is_nosink_dfa(dfa *A //!< The DFA.
);

/**
 * @brief
 * Tests if a DFA (complete or not) has no self-loops
 *
 * @return
 * A Boolean indicating whether the DFA has no self-loops.
 */
bool is_noselfloops_dfa(dfa *A //!< The DFA.
);

#endif
