#ifndef READLINE_H_
#define READLINE_H_

#include "alloc.h"
#include "error.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_HISTORY_SIZE 1000

extern int history_size;

extern char *histfile;

extern const char *all_strings[];

void set_history_size(int size);

void initialize_readline(char *prefix);

char **keyword_name_completion(const char *text, int start, int end);

char *keyword_name_generator(const char *, int);

#endif // READLINE_H_
