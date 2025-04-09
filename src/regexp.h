/**
 * @file regexp.h
 * @brief
 * Implementation of regular expressions.
 */
#ifndef REGEXP_H_
#define REGEXP_H_

 /*  ____                  _                                                _                  */
 /* |  _ \ ___  __ _ _   _| | __ _ _ __    _____  ___ __  _ __ ___  ___ ___(_) ___  _ __  ___  */
 /* | |_) / _ \/ _` | | | | |/ _` | '__|  / _ \ \/ / '_ \| '__/ _ \/ __/ __| |/ _ \| '_ \/ __| */
 /* |  _ <  __/ (_| | |_| | | (_| | |    |  __/>  <| |_) | | |  __/\__ \__ \ | (_) | | | \__ \ */
 /* |_| \_\___|\__, |\__,_|_|\__,_|_|     \___/_/\_\ .__/|_|  \___||___/___/_|\___/|_| |_|___/ */
 /*            |___/                               |_|                                         */

#include "alloc.h"
#include "error.h"
#include "nfa.h"
#include "tools.h"
#include "type_basic.h"
#include "type_dequeue.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>



/**
 * @brief
 * Global variable corresponding to the number of symbolic variables which are
 * currently allowed (set to zero if no symbolic variables are allowed).
 */
extern short symbolic_count;

/**
 * @brief
 * Global array assigning its name to each symbolic variable index (its size is
 * symbolic_count).
 */
extern char** symbolic_names;



/**
 * @brief
 * Computes the index of a symbolic variable name in the array symbolic_names.
 *
 * @return
 * The index of the symbolic variable name.
 */
short symbolic_index(char* //!< The symbolic variable name.
);



/**
 * @brief
 * Type used to represent a symbolic letter.
 */
typedef struct syletter {
    uchar lab; //!< The letter itself.
    uchar dec; //!< The decrementation index. 
} syletter;

/**
 * @brief
 * Type used to represent a symbolic variable.
 */
typedef struct syvariable {
    uchar ind; //!< The index of the symbolic variable.
    uchar dec; //!< The decrementation index. 
} syvariable;

/**
 * @brief
 * Displays a symbolic letter on a given stream: UTF8 version for the indices.
 *
 * @return
 * The length of the displayed letter.
 */
uint display_syletter_utf8(syletter //!< The symbolic letter.
    , FILE*                         //!< The stream.
);

/**
 * @brief
 * Displays a symbolic variable on a given stream: UTF8 version for the indices.
 *
 * @attention
 * The variable name is taken from the global array symbolic_names.
 *
 * @return
 * The length of the displayed variable.
 */
uint display_syvar_utf8(syvariable v, FILE* out);

/**
 * @brief
 * The operator available in an extended regular expression.
 */
typedef enum {
    EMPTY,      //!< Empty language.
    EPSILON,    //!< Empty word.
    CHAR,       //!< Single letter.
    SYCHAR,     //!< Symbolic letter.
    SYVAR,      //!< Symbolic variable.
    WORD,       //!< Single word.
    UNION,      //!< Union of two expressions.
    INTER,      //!< Intersection of two expressions.
    COMPLEMENT, //!< Complement of an expression.
    CONCAT,     //!< Concatenation of two expressions.
    STAR,       //!< Kleene star of an expression.
    PLUS,       //!< Kleene plus of an expression.
    NONE        //!< Used for simplifying the display.
} regelem;

/**
 * @brief
 * Type used to represent a single node in a regular expression.
 */
typedef struct regexp {
    regelem op;           //!< The operator.
    struct regexp* left;  //!< The left sub-expression (NULL if no left sub-expression).
    struct regexp* right; //!< The right sub-expression (NULL if no right sub-expression).
    union {
        letter letter;    //!< Letter (if this is a letter node).
        word* word;       //!< Word (if this is a word node).
        syletter sylet;   //!< Symbolic letter.
        syvariable syvar; //!< Symbolic variable.
    };
} regexp;

/**
 * @brief
 * Tests if a regular expression contains a symbolic node.
 *
 * @return A Boolean indicating whether the regular expression contains a symbolic node.
 */
bool reg_has_symbolic(regexp* //!< The regular expression.
);

/**
 * @brief
 * Tests if a regular expression is simple.
 *
 * @remark
 * A regular expression is simple if and only if it does not contain the operators
 * for intersection and complement.
 *
 * @return A Boolean indicating whether the regular expression is simple.
 */
bool reg_issimple(regexp* //!< The regular expression.
);

/**
 * @brief
 * Release of a regular expression.
 */
void reg_free(regexp* //!< The regular expression.
);

/**
 * @brief
 * Copy of a regular expression.
 *
 * @return
 * A copy of the input regular expression.
 */
regexp* reg_copy(regexp* //!< The regular expression.
);

/**
 * @brief
 * Computes a regular expression recognizing the empty language.
 *
 * @return
 * The regular expression
 */
regexp* reg_empty(void);

/**
 * @brief
 * Computes a regular expression recognizing the language {ε}.
 *
 * @return
 * The regular expression.
 */
regexp* reg_epsilon(void);

/**
 * @brief
 * Computes a regular expression recognizing the language {a} for an input letter a.
 *
 * @return
 * The regular expression.
 */
regexp* reg_letter(uchar //!< The letter.
);

/**
 * @brief
 * Computes a regular expression recognizing the language {a} for an input letter a. Extended version.
 *
 * @return
 * The regular expression.
 */
regexp* reg_letter_ext(letter //!< The letter.
);

/**
 * @brief
 * Computes a regular expression recognizing the language {a_n} for an input letter a, subscripted by n.
 *
 * @return
 * The regular expression.
 */

regexp* reg_letter_numbered(uchar c,    //!< The letter.
    uchar index //!< The number.
);

/**
 * @brief
 * Computes a regular expression corresponding to the symbolic letter a_{i+k}
 *
 * @return
 * The regular expression.
 */
regexp* reg_letter_symbolic(uchar c,     //!< The letter.
    uchar number //!< The number k.
);

regexp* reg_var_symbolic(char* s, uchar number);

/**
 * @brief
 * Combines two regular expressions with the union operator.
 *
 * @attention
 * The two input expressions are not copied.
 *
 * @return
 * The resulting regular expression.
 */
regexp* reg_union(regexp*, //!< The left expression.
    regexp*  //!< The right expression.
);

/**
 * @brief
 * Combines two regular expressions with the intersection operator.
 *
 * @attention
 * The two input expressions are not copied.
 *
 * @return
 * The resulting regular expression.
 */
regexp* reg_inter(regexp*, //!< The left expression.
    regexp*  //!< The right expression.
);

/**
 * @brief
 * Combines two regular expressions with the concatenation operator.
 *
 * @attention
 * The two input expressions are not copied.
 *
 * @return
 * The resulting regular expression.
 */
regexp* reg_concat(regexp*, //!< The left expression.
    regexp*  //!< The right expression.
);

/**
 * @brief
 * Applies the Kleene star operator to a regular expression.
 *
 * @attention
 * The input expression is not copied.
 *
 * @return
 * The resulting regular expression.
 */
regexp* reg_star(regexp* //!< The expression.
);

/**
 * @brief
 * Applies the Kleene plus operator to a regular expression.
 *
 * @attention
 * The input expression is not copied.
 *
 * @return
 * The resulting regular expression.
 */
regexp* reg_plus(regexp* //!< The expression.
);

/**
 * @brief
 * Applies the complement operator to a regular expression.
 *
 * @attention
 * The input expression is not copied.
 *
 * @return
 * The resulting regular expression.
 */
regexp* reg_complement(regexp* //!< The expression.
);

/**
 * @brief
 * Displays a regular expression on the standard output stream.
 */
void reg_print(regexp* //!< The expression.
);


/**
 * @brief
 * Computes information on the symbolic variables of a regular expression.
 *
 * @details
 * A list of symbolic variable names is given as input. The function checks whether all
 * names in the expressions are in this set. Furthermore, for each name, it checks if there
 * exists an occurence of this name with the index 0.
 *
 * @return
 * A Boolean indicating whether all names in the expression are in the input set.
 */
bool reg_symbolic_loops(regexp* //!< The regular expression.
    , ushort                    //!< The maximum decrement.
    , uchar                     //!< The number of names in the input set.
    , bool*                     //!< Used to return the names which occur with the index 0.
);




#endif
