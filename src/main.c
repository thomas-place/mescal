#include "main.h"
#include "regexp.h"
#include "nfa.h"
#include "monoid.h"
#include "printing.h"
#include "macros_alloc.h"
#include "tools.h"
#include "string.h"
#include "time.h"
#include "shell_keywords.h"

#include <readline/readline.h>
#include <readline/history.h>

#define HISTFILE "./.maga_history"

extern const char* keywords_strarray[];
extern uint keywords_count;

char** keyword_name_completion(const char*, int, int);

char* keyword_name_generator(const char*, int);

char** keyword_name_completion(const char* text, int start, int end)
{
    int z = start;
    start = z;
    z = end;
    end = z;

    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, keyword_name_generator);
}

extern const char* all_strings[];

char* keyword_name_generator(const char* text, int state)
{
    static int index, len;
    const char* name;

    if (!state)
    {
        index = 0;
        len = strlen(text);
    }

    while ((name = all_strings[index++]))
    {
        if (strncmp(name, text, len) == 0)
        {
            return strdup(name);
        }
    }

    return NULL;
}

int main(int argc, char* argv[])
{
    assert(argc > 0);

    rl_attempted_completion_function = keyword_name_completion;
    rl_basic_word_break_characters = " \t\n\"\\'`@$><;|&{(,.=";

    char* histfile;
    char* str;
    str = strrchr(argv[0], '/');
    if (str != NULL)
    {
        str++;
    }
    else
    {
        str = argv[0];
    }
    CALLOC(histfile, 10 + strlen(str));
    histfile[0] = '.';
    strcpy(histfile + 1, str);
    strcpy(histfile + 1 + strlen(str), "_history");

    srand(time(NULL));

    using_history();

    int err = read_history(histfile);
    if (err)
    {
        DEBUG("No history file, %d.\n", err);
    }

    keywords_add_all_keys();

    print_title_box_shift(10, 1, 1, true, stdout, 2, "🌮 Holà compañeros! 🌮", "🌵 Welcome to the MeSCal program! 🌵");

    shell_parse();

    fprintf(stdout, "Bye!\n");
    write_history(histfile);
    exit(0);
}
