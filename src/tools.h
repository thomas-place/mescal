#ifndef TOOLS_H_
#define TOOLS_H_

#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "macros_alloc.h"
#include "macros_error.h"
#include "type_basic.h"

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
void print_title_box_shift(uint length, uint shiftl, uint shiftr, bool closed, FILE* out, uint nlines, ...);
void print_dtitle_box(uint length, bool closed, FILE* out, uint nlines, ...);

void print_line_box(uint length, FILE* out, char* s);
void print_dline_box(uint length, FILE* out, char* s);

void print_booltab_alph(bool* alpha_array, uint alph_size, FILE* out);

void append_power(uint n, char* name);
void print_power(uint n, FILE* out);
void print_facto_word(char* word, FILE* out);


#endif // TOOLS_H_
