#include "shell_keywords.h"
#include <ctype.h>

/************************/
/* Tableau des keywords */
/************************/

// Tableaux pour mémoriser l'association entre les keywords et les chaînes à
// utiliser dans le parser
#define KEYWORDS_MAX 512
uint keywords_count, strings_count;
com_keyword keywords_keyarray[KEYWORDS_MAX];
const char *keywords_strarray[KEYWORDS_MAX];
const char *all_strings[KEYWORDS_MAX];

// Ajout d'un keyword
void keywords_add_key(com_keyword thekey, const char *thename)
{
    keywords_keyarray[keywords_count] = thekey;
    keywords_strarray[keywords_count] = thename;
    keywords_count++;

    all_strings[strings_count++] = thename;
}

// Ajout de tous les keywords
void keywords_add_all_keys(void)
{
    // Interface
    all_strings[strings_count++] = "exit";
    all_strings[strings_count++] = "help";
    all_strings[strings_count++] = "automata";
    all_strings[strings_count++] = "languages";
    all_strings[strings_count++] = "morphisms";

    // Mots-clés
    // keywords_add_key(CM_COMPLEM, "comp");
    keywords_add_key(CM_COMPLEM, "complement");
    // keywords_add_key(CM_CONCAT, "concat");
    keywords_add_key(CM_CONCAT, "concatenation");
    // keywords_add_key(CM_DELETE, "del");
    keywords_add_key(CM_DELETE, "delete");
    // keywords_add_key(CM_DETERMIN, "det");
    // keywords_add_key(CM_DETERMIN, "deter");
    keywords_add_key(CM_DETERMIN, "determinization");
    // keywords_add_key(CM_ELIMEPS, "elimeps");
    keywords_add_key(CM_ELIMEPS, "elimepsilon");
    keywords_add_key(CM_FPHIERA, "fphiera");
    // keywords_add_key(CM_GKERNEL, "gker");
    keywords_add_key(CM_GKERNEL, "gkernel");
    keywords_add_key(CM_GREEN, "green");
    keywords_add_key(CM_IDEMS, "idempotents");
    // keywords_add_key(CM_IDEMS, "idems");
    keywords_add_key(CM_IMAGE, "image");
    keywords_add_key(CM_INFO, "info");
    // keywords_add_key(CM_INTERSEC, "inter");
    keywords_add_key(CM_INTERSEC, "intersection");
    // keywords_add_key(CM_KERNEL, "ker");
    keywords_add_key(CM_KERNEL, "kernel");
    keywords_add_key(CM_KLEENE, "kleene");
    // keywords_add_key(CM_LCAY, "lcay");
    keywords_add_key(CM_LCAY, "lcayley");
    keywords_add_key(CM_LINK, "link");
    keywords_add_key(CM_LOADSESSION, "loadsession");
    // keywords_add_key(CM_MEMB, "memb");
    keywords_add_key(CM_MEMB, "membership");
    keywords_add_key(CM_MINI, "minimal");
    // keywords_add_key(CM_MKERNEL, "mker");
    keywords_add_key(CM_MKERNEL, "mkernel");
    // keywords_add_key(CM_MULT, "mult");
    keywords_add_key(CM_MULT, "multiplication");
    keywords_add_key(CM_NEGHIERA, "neghiera");
    keywords_add_key(CM_OPEN, "open");
    // keywords_add_key(CM_ORBIT, "orb");
    keywords_add_key(CM_ORBIT, "orbit");
    keywords_add_key(CM_ORBITS, "orbits");
    // keywords_add_key(CM_ORBITS, "orbs");
    keywords_add_key(CM_ORDER, "order");
    // keywords_add_key(CM_RCAY, "rcay");
    keywords_add_key(CM_RCAY, "rcayley");
    keywords_add_key(CM_RDFA, "rdfa");
    keywords_add_key(CM_RECDEF, "recdef");
    keywords_add_key(CM_RNFA, "rnfa");
    keywords_add_key(CM_RUN, "run");
    keywords_add_key(CM_SAVE, "save");
    keywords_add_key(CM_SAVESESSION, "savesession");
    // keywords_add_key(CM_SEPAR, "separ");
    keywords_add_key(CM_SEPAR, "separation");
    keywords_add_key(CM_SORT, "sort");
    keywords_add_key(CM_SYNT, "syntactic");
    // keywords_add_key(CM_THOMSON, "thom");
    keywords_add_key(CM_THOMSON, "thomson");
    keywords_add_key(CM_TRIMNFA, "trim");
    keywords_add_key(CM_UNION, "union");

    // Props
    keywords_add_key(PR_COMM, "comm");

    // Classes
    keywords_add_key(CL_AT, "AT");
    keywords_add_key(CL_ATT, "ATT");
    keywords_add_key(CL_SF, "SF");
    keywords_add_key(CL_UL, "UL");
    keywords_add_key(CL_PPT, "PPT");
    keywords_add_key(CL_PT, "PT");
    keywords_add_key(CL_LT, "LT");
    keywords_add_key(CL_LTT, "LTT");
    keywords_add_key(CL_ST, "ST");
    keywords_add_key(CL_DD, "DD");
    keywords_add_key(CL_MOD, "MOD");
    keywords_add_key(CL_MODP, "MODP");
    keywords_add_key(CL_AMT, "AMT");
    keywords_add_key(CL_GR, "GR");

    // Opérateurs
    keywords_add_key(OP_POL, "POL");
    keywords_add_key(OP_POL2, "POL2");
    keywords_add_key(OP_BPOL, "BPOL");
    keywords_add_key(OP_BPOL2, "BPOL2");
    keywords_add_key(OP_UBPOL, "UBPOL");
    keywords_add_key(OP_UBPOL2, "UBPOL2");
    keywords_add_key(OP_UPOL, "UPOL");
    keywords_add_key(OP_LPOL, "LPOL");
    keywords_add_key(OP_RPOL, "RPOL");
    keywords_add_key(OP_MPOL, "MPOL");
    keywords_add_key(OP_SFC, "SFC");
    keywords_add_key(OP_TLC, "TL");
    keywords_add_key(OP_TLC2, "TL2");
    keywords_add_key(OP_FLC, "FL");
    keywords_add_key(OP_FLC2, "FL2");
    keywords_add_key(OP_PLC, "PL");
    keywords_add_key(OP_PLC2, "PL2");

    all_strings[keywords_count] = NULL;
}

/********************************/
/* Messages d'érreur génériques */
/********************************/

// Commande invalide
void shell_command_error(void)
{
    fprintf(stderr, "Error: this is not a valid command.\n");
}

// Arguments invalides
void shell_arguments_error(void)
{
    fprintf(stderr, "Error: these are not valid arguments.\n");
}

// Variable inconnue
void shell_variable_error(void)
{
    fprintf(stderr, "Error: unknown variable.\n");
}

/**************************************/
/* Conversion d'une chaîne en élement */
/**************************************/

static bool keywords_comp_chain(const char *s, const char *slow)
{
    uint len = strlen(s);
    if (strlen(s) != strlen(slow))
    {
        return false;
    }

    for (uint i = 0; i < len; i++)
    {
        if (tolower(s[i]) != tolower(slow[i]))
        {
            return false;
        }
    }
    // if (!(('a' <= s[i] && s[i] <= 'z') || ('0' <= s[i] && s[i] <= '9') ||
    // ('A' <= s[i] && s[i] <= 'Z')))
    // return false;
    // else if ((('a' <= s[i] && s[i] <= 'z') || ('0' <= s[i] && s[i] <= '9'))
    // && s[i] != slow[i])
    // return false;
    // else if (('A' <= s[i] && s[i] <= 'Z') && s[i] - 'A' + 'a' != slow[i])
    // return false;
    return true;
}

com_keyword string_to_keyword(char *s)
{
    for (uint i = 0; i < keywords_count; i++)
    {
        if (keywords_comp_chain(s, keywords_strarray[i]))
        {
            return keywords_keyarray[i];
        }
    }
    return KY_NULL;
}

// Fonction qui teste si un nom de variable est autorisé
// Est-ce qu'on évite les keywords ?
// Est-ce que la première lettre est bien une majuscule ?
// Est-ce que ce n'est pas une variable protégée?
bool check_varname(char *name)
{
    if (name == NULL || name[0] < 'A' || name[0] > 'Z')
    {
        fprintf(stderr, "Error: a variable name must start with an upper case letter.\n");
        return false;
    }
    for (uint i = 0; i < keywords_count; i++)
    {
        if (keywords_comp_chain(name, keywords_strarray[i]))
        {
            fprintf(stderr, "Error: this variable name is disallowed.\n");
            return false;
        }
    }
    int i = object_get_from_name(name);
    if (i != -1 && objects[i]->parent != -1)
    {
        fprintf(stderr, "Error: cannot overwrite a protected variable.\n");
        return false;
    }
    return true;
}

/*********************************/
/* Informations sur une commande */
/*********************************/

// Teste si une commande est constituée d'un maillon correspondant à une chaîne
// brute
bool com_israw(com_command *thecom)
{
    if (thecom == NULL || thecom->main == NULL || thecom->main->next != NULL ||
        thecom->params != NULL || thecom->thetype != CMT_RAW)
    {
        return false;
    }
    return true;
}

// Teste si une commande est constituée d'un maillon unique SANS paramètres
bool com_single(com_command *thecom)
{
    if (thecom == NULL || thecom->main == NULL || thecom->main->next != NULL ||
        thecom->params != NULL)
    {
        return false;
    }
    return true;
}

// Teste si une commande est constituée d'un maillon unique avec ou sans
// paramètres
bool com_single_par(com_command *thecom)
{
    if (thecom == NULL || thecom->main == NULL || thecom->main->next != NULL)
    {
        return false;
    }
    return true;
}

// Récupération du nombre de paramètres d'une commande
int com_nbparams(com_parameters *thepar)
{
    int res = 0;
    while (thepar != NULL)
    {
        thepar = thepar->next;
        res++;
    }
    return res;
}

// Retourne le tableau des paramètres d'une commande
com_command **com_getparamarray(com_parameters *thepar)
{
    com_command **array = NULL;
    int n               = com_nbparams(thepar);
    if (n != 0)
    {
        MALLOC(array, n);
        for (int i = 0; i < n; i++)
        {
            array[i] = thepar->param;
            thepar   = thepar->next;
        }
    }
    return array;
}

// Récupération du nième paramètre d'une commande (qui est lui-même une
// commande)
com_command *com_getparam(com_parameters *thepar, int n)
{
    if (thepar == NULL)
    {
        return NULL;
    }
    if (thepar->count <= n)
    {
        return NULL;
    }
    while (n > 0)
    {
        thepar = thepar->next;
        n--;
    }
    return thepar->param;
}

// Retourne l'index de l'objet désigné par une chaîne (-1 si il n'y en a pas)
int index_from_string_chain(string_chain *thechain)
{
    if (thechain == NULL)
    {
        return -1;
    }
    int i = object_get_from_name(thechain->string);
    if (i == -1)
    {
        return -1;
    }
    string_chain *temp = thechain->next;
    while (temp)
    {
        com_keyword key = string_to_keyword(temp->string);

        switch (key)
        {
        case CM_MINI:
            if (objects[i]->type != LANGUAGE)
            {
                return -1;
            }
            shell_compute_minimal(i);
            i    = objects[i]->theob.lan->minauto;
            temp = temp->next;
            break;
        case CM_SYNT:
            if (objects[i]->type != LANGUAGE)
            {
                return -1;
            }
            shell_compute_syntac(i);
            i    = objects[i]->theob.lan->syntmor;
            temp = temp->next;
            break;

        default:
            return -1;
            break;
        }
    }
    return i;
}

// Retourne le mot-clé associé au premier maillon d'une chaîne (KY_NULL si il
// n'y en a pas)
com_keyword key_from_string_chain(string_chain *thechain)
{
    if (thechain == NULL)
    {
        return KY_NULL;
    }
    else
    {
        return string_to_keyword(thechain->string);
    }
}

// Retourne le mot-clé associé à une chaîne d'un seul maillon (KY_NULL si il n'y
// en a pas ou si il y a plus d'un maillon)
com_keyword key_from_string_chain_single(string_chain *thechain)
{
    if (thechain == NULL || thechain->next != NULL)
    {
        return KY_NULL;
    }
    else
    {
        return string_to_keyword(thechain->string);
    }
}

/*******************************/
/* Récupération d'une commande */
/*******************************/

string_chain *com_make_string_chain(char *s, string_chain *thechain)
{
    string_chain *new;
    MALLOC(new, 1);
    new->string = s;
    new->next   = thechain;
    return new;
}

com_parameters *com_make_parameters(com_command *theparam, com_parameters *params)
{
    com_parameters *new;
    MALLOC(new, 1);
    if (params)
    {
        new->count = params->count + 1;
    }
    else
    {
        new->count = 1;
    }
    new->next  = params;
    new->param = theparam;
    return new;
}

com_command *com_make_command(char *s, com_command *thecom)
{
    string_chain *new;
    MALLOC(new, 1);
    new->string  = s;
    new->next    = thecom->main;
    thecom->main = new;
    return thecom;
}

com_command *com_init_command(char *s, com_parameters *params)
{
    string_chain *new;
    MALLOC(new, 1);
    new->string = s;
    new->next   = NULL;
    com_command *thecom;
    MALLOC(thecom, 1);
    thecom->thetype = CMT_KEY;
    thecom->params  = params;
    thecom->main    = new;
    return thecom;
}

com_command *com_init_rawcommand(char *s)
{
    string_chain *new;
    MALLOC(new, 1);
    new->string = s;
    new->next   = NULL;
    com_command *thecom;
    MALLOC(thecom, 1);
    thecom->thetype = CMT_RAW;
    thecom->params  = NULL;
    thecom->main    = new;
    return thecom;
}

//
void com_free_string_chain(string_chain *thechain)
{
    if (thechain)
    {
        com_free_string_chain(thechain->next);
        free(thechain->string);
        free(thechain);
    }
}

void com_free_parameters(com_parameters *params)
{
    if (params)
    {
        com_free_parameters(params->next);
        com_free_command(params->param);
        free(params);
    }
}

void com_free_command(com_command *thecom)
{
    if (thecom)
    {
        com_free_string_chain(thecom->main);
        com_free_parameters(thecom->params);
        free(thecom);
    }
}

//
