#include "shell_errors.h"
#include "interrupt.h"

void shell_error_dispatch(int err) {
    switch (err) {
    case INTERRUPTION:
        fprintf(stderr, "#### User interruption.\n");
        break;
    case COMPUTATION_FAILURE:
        fprintf(stderr, "#### Computation failure.\n");
        break;
    case SYNTAX_ERROR:
        shell_error_syntax();
        break;
    case INVALID_OBJECT:
        fprintf(stderr, "#### Invalid object.\n");
        break;
    case MEMORY_LIMIT:
        fprintf(stderr, "#### Memory limit reached.\n");
        break;
    case TIMEOUT_OCCURRED:
        fprintf(stderr, "#### Timeout after %d seconds.\n#### You can change the timeout value with 'timeout = <seconds> (0 to cancel timeout).'\n", timeout_value);
        break;
    default:
        fprintf(stderr, "#### Unknown error.\n");
        break;
    }
}

void shell_error_syntax(void) { fprintf(stderr, "Syntax error in command.\n"); }

void shell_error_full(void) { fprintf(stderr, "Error: the array of objects is full.\n"); }

void shell_error_unknownvar(const char* varname) { fprintf(stderr, "Error: Unknown variable \"%s\".\n", varname); }

void shell_error_disallowedvar(const char* varname) { fprintf(stderr, "Error: The variable name \"%s\" is disallowed.\n", varname); }

void shell_error_unknownrel(const char* varname, const char* relname) { fprintf(stderr, "Error: The relation \"%s\" is not part of the recursive definition \"%s\".\n", relname, varname); }

void shell_error_notrecdef(com_command* thecom) {
    fprintf(stderr, "Error: The command \"");
    print_command(thecom, stderr);
    fprintf(stderr, "\" does not return a symbolic regular expression.\n");
}

void shell_error_notregexp(com_command* thecom) {
    fprintf(stderr, "Error: The command \"");
    print_command(thecom, stderr);
    fprintf(stderr, "\" does not return a regular expression.\n");
}

void shell_error_wrongrecindex(uint i, const char* varname) { fprintf(stderr, "Error: The index \"%d\" is not an initial one for the the recursive definition \"%s\".\n", i, varname); }

void shell_error_copyrec(void) { fprintf(stderr, "Error: Cannot a copy a recursive definition."); }

void shell_error_notrecvar(const char* varname) { fprintf(stderr, "Error: \"%s\" is not a recursive definition variable.\n", varname); }
void shell_error_recnotok(const char* varname) { fprintf(stderr, "The recursive definition \"%s\" is not well-specified.\n", varname); }

void shell_error_noreturn(com_command* thecom) {
    fprintf(stderr, "Error: The command \"");
    print_command(thecom, stderr);
    fprintf(stderr, "\" does not return an object.\n");
}

void shell_error_leastparams(const char* cmname, uchar nb) { fprintf(stderr, "Error: The command \"%s\" takes at least %d parameters.\n", cmname, nb); }

void shell_error_nbparams(const char* cmname, uchar nb) { fprintf(stderr, "Error: The command \"%s\" takes %d parameters.\n", cmname, nb); }

void shell_error_rangeparams(const char* cmname, uchar low, uchar high) { fprintf(stderr, "Error: The command \"%s\" takes between %d and %d parameters.\n", cmname, low, high); }

void shell_error_exppar(const char* cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a regular expression.\n", nb, cmname); }

void shell_error_autopar(const char* cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be an automaton.\n", nb, cmname); }

void shell_error_morpar(const char* cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a morphism.\n", nb, cmname); }

void shell_error_numpar(const char* cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a number.\n", nb, cmname); }

void shell_error_filepar(const char* cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a filepath.\n", nb, cmname); }

void shell_error_varpar(const char* cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a variable.\n", nb, cmname); }

void shell_error_wordpar(const char* cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a word.\n", nb, cmname); }

void shell_error_classpar(const char* cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a class.\n", nb, cmname); }

void shell_error_invalidpar(const char* cmname, com_command* par) {
    fprintf(stderr, "Error: Parameter ");
    print_command(par, stderr);
    fprintf(stderr, " of the command \"%s\" is invalid.\n", cmname);
}

void shell_error_classnsup(com_command* thecom, const char* cmname) {
    fprintf(stderr, "Error: The class ");
    print_command(thecom, stderr);
    fprintf(stderr, "is not supported by the command \"%s\".\n", cmname);
}

void shell_error_null(void) { fprintf(stderr, "Error: Returned a NULL object.\n"); }



bool error_nbpars(const char* str, com_parameters* pars, short pmin, short pmax) {
    pmin = max(pmin, 0);
    int n = com_nbparams(pars);

    // If the command takes no parameters, we check if the number of parameters is 0.
    if (pmin == 0 && pmax == 0 && n > 1) {
        fprintf(stderr, "#### Error  : the command \"%s\" takes no parameters.\n", str);
        return true;
    }

    // If the command takes at least one parameter, we check if the number of parameters is correct.
    if (n < pmin || (pmax >= 0 && n > pmax)) {
        fprintf(stderr, "#### Error  : wrong number of parameters in the command \"%s\".\n", str);
        return true;
    }
    return false;
}

bool error_parobtype(const char* str, int p, int i, ob_type type) {
    if (i < 0 || objects[i]->type != type) {
        fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not ", p + 1, str);
        switch (type)
        {
        case REGEXP:
            fprintf(stderr, "a regular expression.\n");
            break;
        case AUTOMATON:
            fprintf(stderr, "an automaton.\n");
            break;
        case MORPHISM:
            fprintf(stderr, "a morphism.\n");
            break;
        case RECDEF:
            fprintf(stderr, "a recursive definition.\n");
            break;
        default:
            fprintf(stderr, "ERROR.\n");
            break;
        }
        return true;
    }
    return false;
}

void error_parinteger(const char* str, int p) {
    fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an integer.\n", p + 1, str);
}
void error_partextraw(const char* str, int p) {
    fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not raw text.\n", p + 1, str);
}

void error_parvar(const char* str, int p) {
    fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not a variable.\n", p + 1, str);
}

void error_parunkvar(const char* var) {
    fprintf(stderr, "#### Error  : the variable \"%s\" is unknown.\n", var);
}
