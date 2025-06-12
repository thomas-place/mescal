/**
 * @file type_tools.h
 * @brief
 * Various functions used for the display.
 */

#ifndef TOOLS_H_
#define TOOLS_H_

 /*  ____  _           _               _              _      */
 /* |  _ \(_)___ _ __ | | __ _ _   _  | |_ ___   ___ | |___  */
 /* | | | | / __| '_ \| |/ _` | | | | | __/ _ \ / _ \| / __| */
 /* | |_| | \__ \ |_) | | (_| | |_| | | || (_) | (_) | \__ \ */
 /* |____/|_|___/ .__/|_|\__,_|\__, |  \__\___/ \___/|_|___/ */
 /*             |_|            |___/                         */

#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "alloc.h"
#include "error.h"
#include "type_basic.h"
#include "type_dequeue.h"


/**
 * @brief
 * Compares two unsigned integers.
 *
 * @return
 * A negative value if p1 < p2, 0 if p1 == p2, and a positive value if p1 > p2.
 */
int compare_uint(const void* p1, //!< The first integer.
    const void* p2 //!< The second integer.
);


bool mem_array_sorted(uint e, uint* array, uint size, uint* ind);

uint array_sort_norepeat_uint(uint* array, //!< The array to sort.
    uint size //!< The size of the array.
);

/**
 * @brief
 * Computes the length of the decimal representation of an integer.
 *
 * @return
 * The length.
 */
uint get_uint_length(uint n //!< The integer.
);


/**
 * @brief
 * Computes the length of the binary representation of an integer.
 *
 * @return
 * The length.
 */
uchar get_uint_lbinary(uint n//!< The integer.
);

/**
 * @brief
 * Concatenates multiple strings.
 *
 * @remark
 *  The last string must be NULL.
 *
 * @return
 * The concatenated string.
 */
char* multiple_strcat(char* s, //!< The first string.
    ... //!< The other strings (NULL at the end).
);

/************************/
/** Printing functions **/
/************************/

/**
 * @brief
 * Prints the top line of a box (with corners on both ends)
 */
void print_top_line(uint length, //!< The length of the line.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints the middle line of a box (with links for the lower and upper parts on both ends)
 */
void print_mid_line(uint length, //!< The length of the line.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints the bottom line of a box (with corners on both ends)
 */
void print_bot_line(uint length, //!< The length of the line.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints a separator line (one top line and one bottom line)
 */
void print_sep_line(uint length, //!< The length of the line.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints the top line of a double box (with corners on both ends)
 */
void print_dtop_line(uint length, //!< The length of the line.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints the middle line of a double box (with links for the lower and upper parts on both ends)
 */
void print_dmid_line(uint length, //!< The length of the line.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints the bottom line of a double box (with corners on both ends)
 */
void print_dbot_line(uint length, //!< The length of the line.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints spaces.
 */
void print_spaces(uint number, //!< The number of spaces.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints several copies of a string.
 */
void print_copies_string(uint number, //!< The number of copies.
    char* s, //!< The string to print.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Counts the on-screen length of a UTF-8 string.
 *
 * @return
 * The on-screen length of the string.
 */
uint count_utf8_code_points(const char* s //!< The string to count.
);

/**
 * @brief
 * Prints a title box.
 *
 * @remark
 * The minimum length is 100 (if a lower length is passed, it is set to 100).
 *
 * @remark
 * The title is centered in the box.
 */
void print_title_box(uint length, //!< The length of the box.
    bool closed, //!< If true, a bottom line is printed at the bottom. If false, a mid line is printed instead.
    FILE* out, //!< The output stream.
    uint nlines, //!< The number of lines to print.
    ... //!< The lines to print (nlines char*).
);

/**
 * @brief
 * Prints a double title box.
 *
 * @remark
 * The minimum length is 100 (if a lower length is passed, it is set to 100).
 *
 * @remark
 * The title is centered in the box.
 */
void print_dtitle_box(uint length, //!< The length of the box.
    bool closed, //!< If true, a bottom line is printed at the bottom. If false, a mid line is printed instead.
    FILE* out, //!< The output stream.
    uint nlines, //!< The number of lines to print.
    ... //!< The lines to print (nlines char*).
);

/**
 * @brief
 * Prints a single line in a box.
 *
 * @remark
 * The minimum length is 100 (if a lower length is passed, it is set to 100).
 */
void print_line_box(uint length, //!< The length of the box.
    FILE* out, //!< The output stream.
    char* s //!< The line to print.
);

/**
 * @brief
 * Prints a line in a double box.
 *
 * @remark
 * The minimum length is 100 (if a lower length is passed, it is set to 100).
 */
void print_dline_box(uint length, //!< The length of the box.
    FILE* out, //!< The output stream.
    char* s //!< The line to print.
);

/**
 * @brief
 * Prints a natural number as a subscript in UTF-8 on a stream.
 *
 * @return
 * The on-screen length of the number.
 */
uint fprint_subsc_utf8(uint n, //!< The number to print.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints a natural number as a power in UTF-8 on a stream.
 *
 * @return
 * The on-screen length of the number.
 */
uint fprint_power_utf8(uint n, //!< The number to print.
    FILE* out //!< The output stream.
);

/**
 * @brief
 * Prints a natural number as a subscript in UTF-8 on a string.
 *
 * @return
 * The length in bytes of the string (larger than the on-screen length).
 */
int sprint_subsc_utf8(uint n, //!< The number to print.
    char* out //!< The output string (must be large enough).
);

/**
 * @brief
 * Prints a natural number as a power in UTF-8 on a string.
 *
 * @return
 * The length in bytes of the string (larger than the on-screen length).
 */
int sprint_power_utf8(uint n, //!< The number to print.
    char* out //!< The output string (must be large enough).
);

/**
 * @brief
 * Colors
 */
typedef enum { RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE } color;


/**
 * @brief
 * Prints a string in color on a stream.
 */
void print_color(char* s, //!< The string to print.
    color col, //!< The color to use.
    FILE* out //!< The output stream.
);


#endif // TOOLS_H_
