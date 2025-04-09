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
        fprintf(stderr, "#### Timeout after %d seconds.\n#### You can change the timeout value with 'timeout = <seconds>.'\n", timeout_value);
        break;
    default:
        fprintf(stderr, "#### Unknown error.\n");
        break;
    }
}

void shell_error_syntax(void) { fprintf(stderr, "Syntax error in command.\n"); }

void shell_error_full(void) { fprintf(stderr, "Error: the array of objects is full.\n"); }

void shell_error_unknownvar(const char *varname) { fprintf(stderr, "Error: Unknown variable \"%s\".\n", varname); }

void shell_error_disallowedvar(const char *varname) { fprintf(stderr, "Error: The variable name \"%s\" is disallowed.\n", varname); }

void shell_error_unknownrel(const char *varname, const char *relname) { fprintf(stderr, "Error: The relation \"%s\" is not part of the recursive definition \"%s\".\n", relname, varname); }

void shell_error_notrecdef(com_command *thecom) {
    fprintf(stderr, "Error: The command \"");
    print_command(thecom, stderr);
    fprintf(stderr, "\" does not return a symbolic regular expression.\n");
}

void shell_error_notregexp(com_command *thecom) {
    fprintf(stderr, "Error: The command \"");
    print_command(thecom, stderr);
    fprintf(stderr, "\" does not return a regular expression.\n");
}

void shell_error_wrongrecindex(uint i, const char *varname) { fprintf(stderr, "Error: The index \"%d\" is not an initial one for the the recursive definition \"%s\".\n", i, varname); }

void shell_error_copyrec(void) { fprintf(stderr, "Error: Cannot a copy a recursive definition."); }

void shell_error_notrecvar(const char *varname) { fprintf(stderr, "Error: \"%s\" is not a recursive definition variable.\n", varname); }
void shell_error_recnotok(const char *varname) { fprintf(stderr, "The recursive definition \"%s\" is not well-specified.\n", varname); }

void shell_error_noreturn(com_command *thecom) {
    fprintf(stderr, "Error: The command \"");
    print_command(thecom, stderr);
    fprintf(stderr, "\" does not return an object.\n");
}

void shell_error_leastparams(const char *cmname, uchar nb) { fprintf(stderr, "Error: The command \"%s\" takes at least %d parameters.\n", cmname, nb); }

void shell_error_nbparams(const char *cmname, uchar nb) { fprintf(stderr, "Error: The command \"%s\" takes %d parameters.\n", cmname, nb); }

void shell_error_rangeparams(const char *cmname, uchar low, uchar high) { fprintf(stderr, "Error: The command \"%s\" takes between %d and %d parameters.\n", cmname, low, high); }

void shell_error_exppar(const char *cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a regular expression.\n", nb, cmname); }

void shell_error_autopar(const char *cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be an automaton.\n", nb, cmname); }

void shell_error_morpar(const char *cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a morphism.\n", nb, cmname); }

void shell_error_numpar(const char *cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a number.\n", nb, cmname); }

void shell_error_filepar(const char *cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a filepath.\n", nb, cmname); }

void shell_error_varpar(const char *cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a variable.\n", nb, cmname); }

void shell_error_wordpar(const char *cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a word.\n", nb, cmname); }

void shell_error_classpar(const char *cmname, uchar nb) { fprintf(stderr, "Error: Parameter %d of the command \"%s\" has to be a class.\n", nb, cmname); }

void shell_error_invalidpar(const char *cmname, com_command *par) {
    fprintf(stderr, "Error: Parameter ");
    print_command(par, stderr);
    fprintf(stderr, " of the command \"%s\" is invalid.\n", cmname);
}

void shell_error_classnsup(com_command *thecom, const char *cmname) {
    fprintf(stderr, "Error: The class ");
    print_command(thecom, stderr);
    fprintf(stderr, "is not supported by the command \"%s\".\n", cmname);
}

void shell_error_null(void) { fprintf(stderr, "Error: Returned a NULL object.\n"); }
/*
error_code error_found = ERR_NONE;
string_chain* error_source = NULL;
char* error_string = NULL;
const char* error_command = NULL;
uchar error_parnum = 0;


void command_error(void) {
    // Cas où il n'y a pas d'erreur.
    if (error_found == ERR_NONE) {
        error_source = NULL;
        return;
    }

    // Sinon, on lève une erreur.
    fprintf(stderr, "Error: ");
    switch (error_found)
    {
    case ERR_MALFORMED:
        fprintf(stderr, "Malformed command.\n");
        break;
    case ERR_UNKNOWN_VAR:
        fprintf(stderr, "Unknown variable \"%s\".\n", error_string);
        break;
    case ERR_DISALLOWED_VAR:
        fprintf(stderr, "This variable name is disallowed.\n");
        break;
    case ERR_UPPERCASE_VAR:
        fprintf(stderr, "A variable name must start with an upper case letter.\n");
        break;
    case ERR_PROTECTED_VAR:
        fprintf(stderr, "Cannot overwrite a protected variable.\n");
        break;
    case ERR_NOTREC_VAR:
        fprintf(stderr, "The variable \"%s\" should correspond to a recursive definition.\n", error_string);
        break;
    case ERR_RECDEF_SUB:
        fprintf(stderr, "No recursion relation specified for the recursive definition variable \"%s\".\n", error_string);
        break;
    case ERR_RECDEF_REC:
        fprintf(stderr, "\"%s\" is not a recursive relation.\n", error_string);
        break;
    case ERR_RECDEF_UNK:
        fprintf(stderr, "Unknown recursive relation \"%s\".\n", error_string);
        break;
    case ERR_RECDEF_MWE:
        fprintf(stderr, "The recursive relation is not well-defined.\n");
        break;
    case ERR_RECDEF_CPY:
        fprintf(stderr, "Cannot copy a recursive definition.\n");
        break;
    case ERR_RECDEF_REG:
        fprintf(stderr, "The command does does not return a regular expression.\n");
        break;
    case ERR_RECDEF_PAR:
        fprintf(stderr, "A single index is required as a parameter.\n");
        break;
    case ERR_RECDEF_INI:
        fprintf(stderr, "This index is not a valid initial one for this recursive relation.\n");
        break;
    case ERR_UNKNOWN_COMMAND: // Mauvaise commande.
        fprintf(stderr, "Unknown command \"");
        print_string_chain(error_source, stderr);
        fprintf(stderr, "\".\n");
        break;
    case ERR_UNKNOWN_OBJECT: // Impossible de récupérer un objet.
        fprintf(stderr, "Unknown object \"");
        print_string_chain(error_source, stderr);
        fprintf(stderr, "\".\n");
        break;
    case ERR_MEMB_CLASS: // Membership non-supporté pour cette classe.
        fprintf(stderr, "Membership is not supported for the class ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_MEMB_OPER: //!< Membership non-supporté pour cet opérateur.
        fprintf(stderr, "Membership is not supported for the operator ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_MEMB_INPUT: // Membership non-supporté pour cette classe input.
        fprintf(stderr, "Membership is not supported for the input class ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_SEP_CLASS: // Séparation non-supportée pour cette classe.
        fprintf(stderr, "Separation is not supported for the class ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_SEP_OPER: // Séparation non-supportée pour cet opérateur.
        fprintf(stderr, "Separation is not supported for the operator ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_SEP_INPUT: // Séparation non-supportée pour cette classe input.
        fprintf(stderr, "Separation is not supported for the input class ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_ORB_CLASS: // Orbites non-supportés pour cette classe.
        fprintf(stderr, "Orbits are not supported for the class ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_ORB_OPER: // Orbites non-supportés pour cet opérateur.
        fprintf(stderr, "Orbits are not supported for the operator ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_ORB_INPUT: // Orbites non-supportés pour cette classe input.
        fprintf(stderr, "Orbits are not supported for the input class ");
        print_string_chain(error_source, stderr);
        fprintf(stderr, ".\n");
        break;
    case ERR_EVAL_REGEXP:
        fprintf(stderr, "Could not evaluate the regular expression \"");
        print_string_chain(error_source, stderr);
        fprintf(stderr, "\".\n");
        break;
    case ERR_NULL_REGEXP:
        fprintf(stderr, "Tried to access a NULL regular expression.\n");
        break;
    case ERR_NULL_AUTO:
        fprintf(stderr, "Tried to access a NULL automaton.\n");
        break;
    case ERR_NULL_MOR:
        fprintf(stderr, "Tried to access a NULL morphism.\n");
        break;
    case ERR_MAX_OBJECTS:
        fprintf(stderr, "The array of objects is full.\n");
        break;
    case ERR_NORETURN:
        fprintf(stderr, "Cannot assign the variable %s. The command \"", error_string);
        print_string_chain(error_source, stderr);
        fprintf(stderr, "\" does not return an object.\n");
        break;
    case ERR_PAR_NUMBER:
        fprintf(stderr, "Wrong number of parameters for the command \"");
        print_string_chain(error_source, stderr);
        fprintf(stderr, "\".\n");
        break;
    case ERR_PAR_REGEXP: // Le paramètre doit être une expression régulière.
        fprintf(stderr, "Parameter %d of the command \"%s\" has to be a regular expression.\n", error_parnum, error_command);
        break;
    case ERR_PAR_AUTO: // Le paramètre doit être un automate.
        fprintf(stderr, "Parameter %d of the command \"%s\" has to be an automaton.\n", error_parnum, error_command);
        break;
    case ERR_PAR_MOR: // Le paramètre doit être un morphisme.
        fprintf(stderr, "Parameter %d of the command \"%s\" has to be a morphism.\n", error_parnum, error_command);
        break;
    case ERR_PAR_OBJECT: // Le paramètre doit être un objet.
        fprintf(stderr, "Parameter %d of the command \"%s\" has to be an object.\n", error_parnum, error_command);
        break;
    case ERR_PAR_VAR: // Le paramètre doit être une variable.
        fprintf(stderr, "Parameter %d of the command \"%s\" has to be a variable name.\n", error_parnum, error_command);
        break;
    break;case ERR_PAR_TEXT: // Le paramètre doit être un mot
        fprintf(stderr, "Parameter %d of the command \"%s\" has to be a word.\n", error_parnum, error_command);
        break;
    break;case ERR_PAR_NUM: // Le paramètre doit être un nombre
        fprintf(stderr, "Parameter %d of the command \"%s\" has to be a number.\n", error_parnum, error_command);
        break;
    break;case ERR_PAR_FILENAME: // Le paramètre doit être un nom de fichier.
        fprintf(stderr, "Parameter %d of the command \"%s\" has to be a filename.\n", error_parnum, error_command);
        break;
    default:
        break;
    }


    error_found = ERR_NONE;
    error_source = NULL;
    error_string = NULL;
    error_command = NULL;


} */
