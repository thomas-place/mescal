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
 * Computes the length of the decimal representation of an integer.
 *
 * @return
 * The length.
 */
uint num_length(uint //!< The integer.
);







char* multiple_strcat(char* s, ...);

void print_top_line(uint length, FILE* out);

void print_mid_line(uint length, FILE* out);

void print_bot_line(uint length, FILE* out);

void print_sep_line(uint length, FILE* out);

void print_dtop_line(uint length, FILE* out);

void print_dmid_line(uint length, FILE* out);

void print_dbot_line(uint length, FILE* out);

void print_spaces(uint number, FILE* out);

void print_char(uint number, char* c, FILE* out);

uint count_utf8_code_points(const char* s);

// Affichage d'un titre dans une boite.
// La taille minimale autorisée est 100 (le max de length et 100 est utilisé)
void print_title_box(uint length, bool closed, FILE* out, uint nlines, ...);

void print_title_box_shift(uint length, uint shiftl, uint shiftr, bool closed, FILE* out,
    uint nlines, ...);

void print_dtitle_box(uint length, bool closed, FILE* out, uint nlines, ...);

void print_line_box(uint length, FILE* out, char* s);

void print_dline_box(uint length, FILE* out, char* s);

void print_booltab_alph(bool* alpha_array, uint alph_size, FILE* out);

void append_power(uint n, char* name);

uint fprint_subsc_utf8(uint n, FILE* out);
uint fprint_power_utf8(uint n, FILE* out);
int sprint_subsc_utf8(uint n, char* out);
int sprint_power_utf8(uint n, char* out);

uint get_uint_length(uint n);

void print_facto_word(char* word, FILE* out);


typedef enum { RED, GREEN, YELLOW, BLUE, PURPLE, CYAN, WHITE } color;

void print_color(char* s, color col, FILE* out);


int dicho_search(uint*, uint, int);

#endif // TOOLS_H_
