/**
 * @file words.h
 * @brief
 * Implementation of letters and words.
 */

#ifndef WORDS_H_
#define WORDS_H_

 /*  _         _   _                                  _  __        __            _      */
 /* | |    ___| |_| |_ ___ _ __ ___    __ _ _ __   __| | \ \      / /__  _ __ __| |___  */
 /* | |   / _ \ __| __/ _ \ '__/ __|  / _` | '_ \ / _` |  \ \ /\ / / _ \| '__/ _` / __| */
 /* | |__|  __/ |_| ||  __/ |  \__ \ | (_| | | | | (_| |   \ V  V / (_) | | | (_| \__ \ */
 /* |_____\___|\__|\__\___|_|  |___/  \__,_|_| |_|\__,_|    \_/\_/ \___/|_|  \__,_|___/ */

#include "alloc.h"
#include "tools.h"
#include "type_basic.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief
 * The type used to represent a letter.
 *
 * @details
 * A letter is given by an ASCII character and a number which is displayed
 * as an subscript.
 *
 * @remark
 * If no subscript is desired, the associated parameter should be set to -1.
 */
typedef struct {
    uchar lab; //!< The ASCII character.
    short num; //!< The subscript (must be either -1 or positive).
} letter;

/**
 * @brief
 * Computes the length of a letter when displayed in UTF8, including the subscript.
 *
 * @return
 * The length of the letter including its subscript.
 */
uint length_letter_utf8(letter //!< The letter.
);

/**
 * @brief
 * Displays a letter on a given stream: UTF8 version for the subscripts.
 *
 * @return
 * The on-screen length of the displayed letter.
 */
uint fprint_letter_utf8(letter, //!< The letter.
    FILE* //!< The stream.
);

/**
 * @brief
 * Prints a letter in a given char array: UTF8 version for the subscripts.
 *
 * @return
 * The length of the displayed letter in bytes (generally larger than the on-screen length if there are UTF8 characters).
 */
int sprint_letter_utf8(letter, //!< The letter.
    char* //!< The string (must be large enough).
);






/**
 * @brief
 * Displays a letter on a given stream: graphviz version for the subscript.
 *
 * @remark
 * Includes an option for displaying an inverse power "-1" on the letter.
 * This is used for the inverse transitions in NFAs.
 */
void fprint_letter_gviz(letter, //!< The letter.
    FILE*, //!< The stream.
    bool //!< True if an inverse power has to be displayed, false otherwise.
);


/**
 * @brief
 * Displays a letter on a given stream: latex version for the subscripts.
 */
void fprint_letter_latex(letter, //!< The letter.
    FILE*, //!< The stream.
    bool //!< True if an inverse power has to be displayed, false otherwise.
);

/**
 * @brief
 * Comparison of two letters for sorting algorithms.
 *
 * @return
 * The comparison.
 */
int compare_letters(const void*, //!< The first letter.
    const void* //!< The second letter.
);

/**
 * @brief
 * Creates a copy of an array of letters.
 *
 * @return
 * A copy of the array of letters.
 */
letter* duplicate_alphabet(const letter*, //!< The array of letters.
    uint              //!< The size of the array of letters.
);

/**
 * @brief
 * The type used to represent a word.
 *
 * @details
 * A word is implemented as a dequeue of letters.
 */
typedef struct {
    letter* array;   //!< Array of letters.
    uint size_array; //!< Size of the array of letters.
    uint left;       //!< Index of the leftmost letter (when nonempty).
    uint right;      //!< Index following the righmost letter (when nonempty).
    bool empty;      //!< Boolean indicating whether the word is empty.
} word;

/**
 * @brief
 * Creation of an empty word.
 *
 * @return
 * An empty word.
 */
word* create_empty_word(void);

/**
 * @brief
 * Release of a word.
 */
void delete_word(word* //!< The word.
);

/**
 * @brief
 * Computes the length of a word.
 *
 * @return
 * The length of the word.
 */
uint size_word(const word* //!< The word.
);

/**
 * @brief
 * Tests whether a word is empty.
 *
 * @return.
 * A Boolean indicating whether the word is empty.
 */
bool isempty_word(const word* //!< The word.
);

/**
 * @brief
 * Concatenates a letter to the left of a word.
 */
void lefcon_word(letter, //!< The letter.
    word*  //!< The word.
);

/**
 * @brief
 * Concatenates a letter to the right of a word.
 */
void rigcon_word(letter, //!< The letter.
    word*  //!< The word.
);

/**
 * @brief
 * Reads a letter inside a word without removing it: left-right version.
 *
 * @remark
 * The letters are indexed from left to right (the leftmost one has index 0).
 *
 * @return
 * The letter.
 */
letter lefread_word(const word*, //!< The word.
    uint          //!< The index.
);

/**
 * @brief
 * Reads a letter inside a word without removing it: right-left version.
 *
 * @remark
 * The letters are indexed from right to left (the righmost one has index 0).
 *
 * @return
 * The letter.
 */
letter rigread_word(const word*, //!< The word.
    uint          //!< The index.
);

/**
 * @brief
 * Removes the leftmost letter of a word.
 *
 * @return
 * The removed letter.
 */
letter lefpull_word(word* //!< The word.
);

/**
 * @brief
 * Removes the rightmost letter of a word.
 *
 * @return
 * The removed letter.
 */
letter rigpull_word(word* //!< The word.
);

/**
 * @brief
 * Concatenates two words into the left one.
 */
void concatenate_word(word*, //!< The left word (modified).
    const word* //!< The right word (not modified).
);

/**
 * @brief
 * Dislpays a word on a given stream (UTF8 is used for the subscripts of letters).
 */
void display_word(const word*, //!< The word.
    FILE* //!< The stream.
);

/**
 * @brief
 * Computes the set of letters occuring inside a word.
 *
 * @return
 * An array containing all letters that occur in the word, without repetition.
 * The length of this array is returned using the second parameter.
 */
letter* get_alphabet_word(const word*, //!< The word.
    uint* //!< A pointer used to return the length of the computed array.
);

#endif
