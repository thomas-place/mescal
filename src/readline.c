#include "readline.h"

int history_size = DEFAULT_HISTORY_SIZE;

char *histfile;

void set_history_size(int size) { history_size = size; }

void initialize_readline(char *prefix) {
    // Readline
    rl_attempted_completion_function = keyword_name_completion;
    // rl_basic_word_break_characters   = " \t\n\"\\'`@$><;|&{(,.=";
    rl_basic_word_break_characters = " \t.=,(";

    char *str;
    str = strrchr(prefix, '/');
    if (str != NULL) {
        str++;
    } else {
        str = prefix;
    }
    CALLOC(histfile, 10 + strlen(str));
    histfile[0] = '.';
    strcpy(histfile + 1, str);
    strcpy(histfile + 1 + strlen(str), "_history");

    srand(time(NULL));

    char *a = strdup("\"C-r\": history-search-backward");
    rl_parse_and_bind(a);
    free(a);
    a = strdup("\"C-s\": history-search-forward");
    rl_parse_and_bind(a);
    free(a);

    using_history();
    stifle_history(history_size);

    if (read_history(histfile)) {
        fprintf(stderr, "No history file, creating new in %s.\n", histfile);
    } else {
        fprintf(stderr, "Using history file %s.\n", histfile);
    }
}

char *keyword_name_generator(const char *text, int state) {
    static int index, len;
    const char *name;

    if (!state) {
        index = 0;
        len = strlen(text);
    }

    while ((name = all_strings[index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

char **keyword_name_completion(const char *text, int start, int end) {
    int z = start;
    start = z;
    z = end;
    end = z;

    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, keyword_name_generator);
}
