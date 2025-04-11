#include "shell_keywords.h"
#include "shell_errors.h"
#include <ctype.h>

/************************/
/* Tableau des keywords */
/************************/

// Tableaux pour mémoriser l'association entre les keywords et les chaînes à
// utiliser dans le parser
#define KEYWORDS_MAX 512
uint keywords_count, strings_count;
com_keyword keywords_keyarray[KEYWORDS_MAX];
const char* keywords_strarray[KEYWORDS_MAX];
const char* all_strings[KEYWORDS_MAX];

// Ajout d'un keyword
void keywords_add_key(com_keyword thekey, const char* thename) {
    keywords_keyarray[keywords_count] = thekey;
    keywords_strarray[keywords_count] = thename;
    keywords_count++;

    all_strings[strings_count++] = thename;
}

// Ajout de tous les keywords
void keywords_add_all_keys(void) {
    // Interface
    keywords_add_key(KY_INTERFACE, "exit");
    keywords_add_key(KY_INTERFACE, "help");
    keywords_add_key(KY_INTERFACE, "classes");
    keywords_add_key(KY_INTERFACE, "jep");
    keywords_add_key(KY_INTERFACE, "timeout");
    keywords_add_key(KY_INTERFACE, "history");
    keywords_add_key(KY_INTERFACE, "limit");
    keywords_add_key(KY_TOGGLE, "toggleopti");

    // Commandes sauvegarde et chargement
    keywords_add_key(KY_LOADSESSION, "loadsession");
    keywords_add_key(KY_SAVESESSION, "savesession");
    keywords_add_key(KY_SAVE, "save");
    keywords_add_key(KY_OPEN, "open");
    keywords_add_key(KY_DELETE, "delete");
    keywords_add_key(KY_CLEAR, "clear");
    keywords_add_key(KY_SORT, "sort");
    keywords_add_key(KY_INFO, "info");
    keywords_add_key(KY_EXSEARCH, "exsearch");
    keywords_add_key(KY_EXALL, "exall");
    keywords_add_key(KY_INSIDE, "in");
    keywords_add_key(KY_OUTSIDE, "out");
    keywords_add_key(KY_LIST, "list");
    keywords_add_key(KY_AUTOMATA, "automata");
    keywords_add_key(KY_MORPHISMS, "morphisms");
    keywords_add_key(KY_GROUPS, "groups");
    keywords_add_key(KY_REGEXPS, "regexps");
    keywords_add_key(KY_RECDEFS, "recdefs");

    keywords_add_key(KY_FILTER, "filter");
    keywords_add_key(KY_FMSIZE, "sizesynt");

    // Commandes automates

    keywords_add_key(KY_COMPLEM, "complement");
    keywords_add_key(KY_INTERSEC, "intersection");
    keywords_add_key(KY_KLEENE, "kleene");
    keywords_add_key(KY_MINI, "minimal");
    keywords_add_key(KY_RUN, "run");
    keywords_add_key(KY_ELIMEPS, "elimepsilon");
    keywords_add_key(KY_THOMPSON, "thompson");
    keywords_add_key(KY_GLUSHKOV, "glushkov");
    keywords_add_key(KY_HOPCROFT, "hopcroft");
    keywords_add_key(KY_BRZOZO, "brzozowski");
    keywords_add_key(KY_BMCK, "mccluskey");
    keywords_add_key(KY_TRIMNFA, "trim");
    keywords_add_key(KY_CONCAT, "concatenation");
    keywords_add_key(KY_RESET, "resetnames");
    keywords_add_key(KY_UNION, "union");
    keywords_add_key(KY_MIRROR, "mirror");
    keywords_add_key(KY_INVTRANS, "iextension");
    keywords_add_key(KY_DYCKTRANS, "dyckextension");
    keywords_add_key(KY_PERMUT, "permutation");
    keywords_add_key(KY_COUNTER, "counterfree");
    keywords_add_key(KY_RNFA, "nfarandom");
    keywords_add_key(KY_RDFA, "dfarandom");

    // Commandes morphismes
    keywords_add_key(KY_SYNT, "syntactic");
    keywords_add_key(KY_IMAGE, "image");
    keywords_add_key(KY_LCAY, "lcayley");
    keywords_add_key(KY_RCAY, "rcayley");
    keywords_add_key(KY_ORDER, "order");
    keywords_add_key(KY_IDEMS, "idempotents");
    keywords_add_key(KY_MULT, "multiplication");
    keywords_add_key(KY_MKER, "mkernel");
    keywords_add_key(KY_GKER, "gkernel");
    keywords_add_key(KY_AKER, "akernel");
    keywords_add_key(KY_ORB, "orbit");

    keywords_add_key(KY_RECINIT, "initrecursion");

    // Tests
    keywords_add_key(KY_SEPAR, "separation");
    keywords_add_key(KY_MEMB, "membership");
    keywords_add_key(KY_CHIERA, "chierarchies");
    keywords_add_key(KY_NHIERA, "nhierarchies");
    keywords_add_key(KY_FPHIERA, "fphierarchies");

    // Classes
    keywords_add_key(KY_AT, "AT");
    keywords_add_key(KY_ATT, "ATT");
    keywords_add_key(KY_SF, "SF");
    keywords_add_key(KY_UL, "UL");
    keywords_add_key(KY_PPT, "PPT");
    keywords_add_key(KY_PT, "PT");
    keywords_add_key(KY_LT, "LT");
    keywords_add_key(KY_LTT, "LTT");
    keywords_add_key(KY_ST, "ST");
    keywords_add_key(KY_DD, "DD");
    keywords_add_key(KY_MOD, "MOD");
    keywords_add_key(KY_MODP, "MODP");
    keywords_add_key(KY_AMT, "AMT");
    keywords_add_key(KY_AMTP, "AMTP");
    keywords_add_key(KY_GR, "GR");
    keywords_add_key(KY_GRP, "GRP");
    keywords_add_key(KY_REG, "REG");
    keywords_add_key(KY_KNASTAT, "KNASTAT");

    // Opérateurs
    keywords_add_key(KY_POL, "POL");
    keywords_add_key(KY_POL2, "POL2");
    keywords_add_key(KY_BPOL, "BPOL");
    keywords_add_key(KY_BPOL2, "BPOL2");
    keywords_add_key(KY_UBPOL, "UBPOL");
    keywords_add_key(KY_UBPOL2, "UBPOL2");
    keywords_add_key(KY_UPOL, "UPOL");
    keywords_add_key(KY_LPOL, "LPOL");
    keywords_add_key(KY_RPOL, "RPOL");
    keywords_add_key(KY_MPOL, "MPOL");
    keywords_add_key(KY_TLC, "TL");
    keywords_add_key(KY_TLC2, "TL2");
    keywords_add_key(KY_FLC, "FL");
    keywords_add_key(KY_FLC2, "FL2");
    keywords_add_key(KY_PLC, "PL");
    keywords_add_key(KY_PLC2, "PL2");
    keywords_add_key(KY_JORB, "JORB");

    all_strings[keywords_count] = NULL;
}

/****************************/
/* Generic display commands */
/****************************/

void print_string_chain(const string_chain* thechain, FILE* out) {
    if (!thechain) {
        fprintf(out, "NULL");
        return;
    }

    fprintf(out, "%s", thechain->string);
    thechain = thechain->next;
    while (thechain) {
        fprintf(out, ".%s", thechain->string);
        thechain = thechain->next;
    }
}

void print_command(com_command* thecom, FILE* out) {
    if (!thecom) {
        fprintf(out, "NULL");
        return;
    }
    com_parameters* pars;
    switch (thecom->thetype) {
    case CMT_RAW:
        fprintf(out, "\"%s\"", thecom->main->string);
        return;
        break;
    case CMT_IND:
        print_string_chain(thecom->main, out);
        fprintf(out, "[%d]", thecom->ind);
        return;
    case CMT_KEY:
        print_string_chain(thecom->main, out);
        if (com_nbparams(thecom->params) == 0) {
            return;
        }
        pars = thecom->params;
        fprintf(out, "(");
        print_command(pars->param, out);
        for (int i = 1; i < com_nbparams(thecom->params); i++) {
            pars = pars->next;
            fprintf(out, ",");
            print_command(pars->param, out);
        }
        fprintf(out, ")");
        return;
        break;

    default:
        break;
    }
}

/**************************************/
/* Conversion d'une chaîne en élement */
/**************************************/

static bool keywords_comp_chain(const char* s, const char* slow) {
    uint len = strlen(s);
    if (strlen(s) != strlen(slow)) {
        return false;
    }

    for (uint i = 0; i < len; i++) {
        if (tolower(s[i]) != tolower(slow[i])) {
            return false;
        }
    }

    return true;
}

const char* keywordtostring(com_keyword key) {
    for (uint i = 0; i < keywords_count; i++) {
        if (keywords_keyarray[i] == key) {
            return keywords_strarray[i];
        }
    }
    return NULL;
}

com_keyword string_to_keyword(const char* s) {
    for (uint i = 0; i < keywords_count; i++) {
        if (keywords_comp_chain(s, keywords_strarray[i])) {
            return keywords_keyarray[i];
        }
    }
    return KY_NULL;
}

bool check_varname(const char* name) {
    if (name == NULL || name[0] < 'A' || name[0] > 'Z') {
        return false;
    }
    for (uint i = 0; i < keywords_count; i++) {
        if (keywords_comp_chain(name, keywords_strarray[i])) {
            return false;
        }
    }
    return true;
}

/*********************************/
/* Informations sur une commande */
/*********************************/

bool com_isclass(com_command* thecom) {
    if (!thecom || thecom->thetype != CMT_KEY || thecom->main->next) {
        return false;
    }
    com_keyword key = key_from_string_chain_single(thecom->main);
    if (com_nbparams(thecom->params) == 0) {
        switch (key) {
        case KY_AT:
        case KY_ATT:
        case KY_SF:
        case KY_UL:
        case KY_TLC:
        case KY_TLX:
        case KY_PPT:
        case KY_PT:
        case KY_LT:
        case KY_LTT:
        case KY_ST:
        case KY_DD:
        case KY_MOD:
        case KY_MODP:
        case KY_AMT:
        case KY_GR:
        case KY_GRP:
            return true;
            break;
        default:
            return false;
            break;
        }
    }

    if (com_nbparams(thecom->params) == 1) {
        switch (key) {
        case KY_POL:
        case KY_POL2:
        case KY_BPOL:
        case KY_BPOL2:
        case KY_UBPOL:
        case KY_UBPOL2:
        case KY_UPOL:
        case KY_LPOL:
        case KY_RPOL:
        case KY_MPOL:
        case KY_TLC:
        case KY_TLC2:
        case KY_FLC:
        case KY_FLC2:
        case KY_PLC:
        case KY_PLC2:
            return com_isclass(thecom->params->param);
            break;
        default:
            return false;
            break;
        }
    }
    return false;
}

bool com_israw(const com_command* thecom) {
    if (thecom == NULL || thecom->main == NULL || thecom->main->next != NULL || thecom->params != NULL || thecom->thetype != CMT_RAW) {
        return false;
    }
    return true;
}

bool com_single(const com_command* thecom) {
    if (thecom == NULL || thecom->main == NULL || thecom->main->next != NULL || (thecom->params != NULL && thecom->params->count != 0)) {
        return false;
    }
    return true;
}

bool com_single_par(const com_command* thecom) {
    if (thecom == NULL || thecom->main == NULL || thecom->main->next != NULL) {
        return false;
    }
    return true;
}

int com_nbparams(const com_parameters* thepar) {
    int res = 0;
    while (thepar != NULL) {
        thepar = thepar->next;
        res++;
    }
    return res;
}

com_command* com_getparam(const com_parameters* thepar, int n) {
    if (thepar == NULL) {
        return NULL;
    }

    if (thepar->count <= n) {
        return NULL;
    }

    while (n > 0) {
        thepar = thepar->next;
        n--;
    }

    return thepar->param;
}

// Retourne le mot-clé associé au premier maillon d'une chaîne (KY_NULL si il
// n'y en a pas)
com_keyword key_from_string_chain(const string_chain* thechain) {
    if (thechain == NULL) {
        return KY_NULL;
    }
    else {
        return string_to_keyword(thechain->string);
    }
}

// Retourne le mot-clé associé à une chaîne d'un seul maillon (KY_NULL si il n'y
// en a pas ou si il y a plus d'un maillon)
com_keyword key_from_string_chain_single(const string_chain* thechain) {
    if (thechain == NULL || thechain->next != NULL) {
        return KY_NULL;
    }
    else {
        return string_to_keyword(thechain->string);
    }
}

/*******************************/
/* Récupération d'une commande */
/*******************************/

string_chain* com_make_string_chain(char* s, string_chain* thechain) {
    string_chain* new;
    MALLOC(new, 1);
    new->string = s;
    new->next = thechain;
    return new;
}

com_parameters* com_make_parameters(com_command* theparam, com_parameters* params) {
    com_parameters* new;
    MALLOC(new, 1);
    if (params) {
        new->count = params->count + 1;
    }
    else {
        new->count = 1;
    }
    new->next = params;
    new->param = theparam;
    return new;
}

com_command* com_make_command(char* s, com_command* thecom) {
    if (!thecom) {
        free(s);
        return NULL;
    }
    string_chain* new;
    MALLOC(new, 1);
    new->string = s;
    new->next = thecom->main;
    thecom->main = new;
    return thecom;
}

com_command* com_init_command(char* s, com_parameters* params) {
    string_chain* new;
    MALLOC(new, 1);
    new->string = s;
    new->next = NULL;
    com_command* thecom;
    MALLOC(thecom, 1);
    thecom->thetype = CMT_KEY;
    thecom->params = params;
    thecom->main = new;
    return thecom;
}

com_command* com_init_rawcommand(char* s) {
    string_chain* new;
    MALLOC(new, 1);
    new->string = s;
    new->next = NULL;
    com_command* thecom;
    MALLOC(thecom, 1);
    thecom->thetype = CMT_RAW;
    thecom->params = NULL;
    thecom->main = new;
    return thecom;
}

com_command* com_init_indexing(char* s, char* num) {
    char* end;
    uint ind = strtol(num, &end, 10);
    if (*end != '\0') {
        free(s);
        free(num);
        return NULL;
    }
    string_chain* new;
    MALLOC(new, 1);
    new->string = s;
    new->next = NULL;
    com_command* thecom;
    MALLOC(thecom, 1);
    thecom->main = new;
    thecom->thetype = CMT_IND;
    thecom->ind = ind;
    thecom->params = NULL;
    return thecom;
}

void com_free_string_chain(string_chain* thechain) {
    if (thechain) {
        com_free_string_chain(thechain->next);
        free(thechain->string);
        free(thechain);
    }
}

void com_free_parameters(com_parameters* params) {
    if (params) {
        com_free_parameters(params->next);
        com_free_command(params->param);
        free(params);
    }
}

void com_free_command(com_command* thecom) {
    if (thecom) {
        com_free_string_chain(thecom->main);
        com_free_parameters(thecom->params);
        free(thecom);
    }
}
