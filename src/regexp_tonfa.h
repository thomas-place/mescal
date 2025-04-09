/**
 * @file regexp_tonfa.h
 * @brief
 * Construction of a NFA from a regular expression. Thompson's et Glushkov's algorithms.
 */

#ifndef REGAUT_H_
#define REGAUT_H_

 /*  ____                  _                                                _                    */
 /* |  _ \ ___  __ _ _   _| | __ _ _ __    _____  ___ __  _ __ ___  ___ ___(_) ___  _ __  ___ _  */
 /* | |_) / _ \/ _` | | | | |/ _` | '__|  / _ \ \/ / '_ \| '__/ _ \/ __/ __| |/ _ \| '_ \/ __(_) */
 /* |  _ <  __/ (_| | |_| | | (_| | |    |  __/>  <| |_) | | |  __/\__ \__ \ | (_) | | | \__ \_  */
 /* |_| \_\___|\__, |\__,_|_|\__,_|_|  _  \___/_/\_\ .__/|_|  \___||___/___/_|\___/|_| |_|___(_) */
 /*   ___ ___  |___/_   _____ _ __ ___(_) ___  _ __|_|| |_ ___   | \ | |  ___/ \   ___           */
 /*  / __/ _ \| '_ \ \ / / _ \ '__/ __| |/ _ \| '_ \  | __/ _ \  |  \| | |_ / _ \ / __|          */
 /* | (_| (_) | | | \ V /  __/ |  \__ \ | (_) | | | | | || (_) | | |\  |  _/ ___ \\__ \          */
 /*  \___\___/|_| |_|\_/ \___|_|  |___/_|\___/|_| |_|  \__\___/  |_| \_|_|/_/   \_\___/          */

#include "error.h"
#include "nfa.h"
#include "nfa_intersec.h"
#include "nfa_minimization.h"
#include "regexp.h"
#include "type_boolarray.h"
#include <stdlib.h>

/**************************/
/*+ Glushkov's algorithm +*/
/**************************/

/**
 * @brief
 * Counts the number of letters in a regular expression.
 *
 * @return
 * The number of letters.
 */
uint reg_countletters(regexp* //!< The regular expression.
);

/**
 * @brief
 * Type utilized to represent the information computed by Glushkov's algorithm from
 * the input regular expression.
 *
 * @remark
 * In the array indexed by two-letter words, the word "ab" corresponds to the cell "a * size + b".
 */
typedef struct {
    uint size;      //!< Number of letters in the regular expression.
    bool epsilon;   //!< Boolean indicating whether the regular expression contains the empty word.
    barray* first;  //!< Array of Booleans indexed by the letters. Marks the letters which start a word rexcognized by the expression.
    barray* last;   //!< Array of Booleans indexed by the letters. Marks the letters which end a word recognized by the expression.
    barray* follow; //!< Array of Booleans indexed by two-letter words. Marks the two-letter words which are a factor of a word recognized by the expression.
} glushkov_info;

/**
 * @brief
 * Initialization of a glushkov_info for a sub-expression defining the
 * empty language.
 *
 * @return
 * The glushkov_info object.
 */
glushkov_info* reg_create_gk_emp(uint //!<  Total number of letters in the complete expression.
);

/**
 * @brief
 * Realease of a glushkov_info object.
 */
void reg_gk_delete(glushkov_info* //!< The left glushkov_info object.
);

/**
 * @brief
 * Full computation of the glushkov_info object from a regular expression.
 *
 * @details
 * The computation is recursive. The total number of letters in the complete
 * expression must be given as input.
 *
 * @remark
 * The letters are renamed dynamically. Each letter is renamed by an integer.
 * The third and fourth parameters are used for this renaming. The third one
 * is a pointer to a counter which stores the number of letters which have
 * already been renamed.  The fourth parameter is used to return the mapping
 * from the new names to the original letters in the expression.
 *
 * @return
 * The glushkov_info object.
 */
glushkov_info* gk_indexleaves(regexp*, //!< A sub-expression of the one considered by the Glushkov's algorithm.
    uint,                              //!< The total number of letters in the full regular expression.
    uint*,                             //!< A pointer to a counter which stores the number of letters which have already been renamed.
    letter*                            //!< Array indexed by the letter named computed by the function. Maps each name to the original letter in the expression.
);

/**
 * @brief
 * Glushkov's algorithm.
 *
 * @details
 * Computes a NFA from a regular expression.
 *
 * @remark
 * Glushkov's algorithm can only handle simple regular expressions (i.e. which
 * does not contain the intersection and complement operators). This property is
 * checked by the algorithm and a NULL pointer is returned if it is not satisfied.
 *
 * @return
 * A NFA recognizing the same language as the input regular expression.
 */
nfa* reg_glushkov(regexp* //!< A simple regular expression.
);

/**************************/
/*+ Thompson's algorithm +*/
/**************************/

/**
 * @brief
 * Thompson's algorithm.
 *
 * @details
 * Recursive computation of a NFA from a regular expression.
 *
 * @remark
 * Thompson's algorithm can handle arbitrary regular expressions.
 *
 * @return
 * A NFA recognizing the same language as the input regular expression.
 */
nfa* reg_thompson(regexp* //!< A regular expression.
);

#endif // REGAUT_H_
