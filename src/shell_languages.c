#include "shell_languages.h"

#include "macros_error.h"
#include "shell_languages.h"
#include "monoid.h"
#include "nfa.h"
#include "regexp.h"

// #define MAX_LANGUAGES 2048
// language* languages[MAX_LANGUAGES];
// int nb_languages = 0;

#define MAX_OBJECTS 2048
object *objects[MAX_OBJECTS];
int nb_objects           = 0;

unsigned short minicount = 0;
unsigned short syntcount = 0;

/************************/
/* Création/Suppression */
/************************/

object *object_language_new_reg(char *name, regexp *theregexp)
{
    object *theobject;
    MALLOC(theobject, 1);
    theobject->name   = strdup(name);
    theobject->type   = LANGUAGE;
    theobject->parent = -1;
    language *lang;
    MALLOC(lang, 1);
    lang->type           = SPE_REG;
    lang->spe.reg        = theregexp;
    lang->minauto        = -1;
    lang->syntmor        = -1;
    theobject->theob.lan = lang;
    return theobject;
}

object *object_language_new_nfa(char *name, int i)
{
    if (objects[i]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: this is not an automata variable.\n");
        return NULL;
    }
    object *theobject;
    MALLOC(theobject, 1);
    theobject->name   = strdup(name);
    theobject->type   = LANGUAGE;
    theobject->parent = -1;
    language *lang;
    MALLOC(lang, 1);
    lang->type           = SPE_NFA;
    lang->spe.nfa        = i;
    lang->minauto        = -1;
    lang->syntmor        = -1;
    theobject->theob.lan = lang;
    return theobject;
}

object *object_morphism_new(char *name, cayley *M)
{
    object *theobject;
    MALLOC(theobject, 1);
    theobject->name   = strdup(name);
    theobject->type   = MORPHISM;
    theobject->parent = -1;
    morphism *mor;
    MALLOC(mor, 1);
    mor->cayley          = M;
    mor->green           = NULL;
    mor->ddorbs          = NULL;
    mor->mporbs          = NULL;
    mor->atorbs          = NULL;
    mor->bpmorbs         = NULL;
    mor->bpgorbs         = NULL;
    mor->bpddorbs        = NULL;
    mor->bpmporbs        = NULL;
    mor->mker            = NULL;
    mor->gker            = NULL;
    theobject->theob.mor = mor;
    return theobject;
}

object *object_automata_new(char *name, nfa *A)
{
    object *theobject;
    MALLOC(theobject, 1);
    theobject->name   = strdup(name);
    theobject->type   = AUTOMATA;
    theobject->parent = -1;
    automata *aut;
    MALLOC(aut, 1);
    aut->nfa             = A;
    theobject->theob.aut = aut;
    return theobject;
}

void object_free(object *theobject)
{
    if (theobject == NULL)
    {
        return;
    }
    // Libération du nom
    free(theobject->name);

    switch (theobject->type)
    {
    case AUTOMATA:
        if (theobject->theob.aut == NULL)
        {
            free(theobject);
            return;
        }
        delete_nfa(theobject->theob.aut->nfa);
        free(theobject->theob.aut);
        break;
    case MORPHISM:
        if (theobject->theob.mor == NULL)
        {
            free(theobject);
            return;
        }
        free_cayley(theobject->theob.mor->cayley);
        if (theobject->theob.mor->green)
        {
            delete_green(theobject->theob.mor->green);
        }
        if (theobject->theob.mor->ddorbs)
        {
            delete_orbits(theobject->theob.mor->ddorbs, true);
        }
        if (theobject->theob.mor->mporbs)
        {
            delete_orbits(theobject->theob.mor->mporbs, false);
        }
        if (theobject->theob.mor->atorbs)
        {
            delete_orbits(theobject->theob.mor->atorbs, true);
        }
        if (theobject->theob.mor->bpmorbs)
        {
            delete_orbits(theobject->theob.mor->bpmorbs, true);
        }
        if (theobject->theob.mor->bpgorbs)
        {
            delete_orbits(theobject->theob.mor->bpgorbs, true);
        }
        if (theobject->theob.mor->bpmporbs)
        {
            delete_orbits(theobject->theob.mor->bpmporbs, true);
        }
        if (theobject->theob.mor->bpddorbs)
        {
            delete_orbits(theobject->theob.mor->bpddorbs, true);
        }
        if (theobject->theob.mor->mker)
        {
            delete_green_sub(theobject->theob.mor->mker);
        }
        if (theobject->theob.mor->gker)
        {
            delete_green_sub(theobject->theob.mor->gker);
        }
        free(theobject->theob.mor);
        break;
    case LANGUAGE:
        if (theobject->theob.lan == NULL)
        {
            free(theobject);
            return;
        }
        // Si le langage est spécifié par une expression,
        // on la libère.
        if (theobject->theob.lan->type == SPE_REG)
        {
            if (theobject->theob.lan->spe.reg)
            {
                reg_free(theobject->theob.lan->spe.reg);
            }
        }
        // Si le langage est spécifié par un NFA, on enlève
        // le flag qui protège ce NFA.
        else if (theobject->theob.lan->type == SPE_NFA)
        {
            objects[theobject->theob.lan->spe.nfa]->parent = -1;
        }
        // Si le langage est associèe à un minimal, on enlève
        // le flag qui protège ce minimal.
        if (theobject->theob.lan->minauto != -1)
        {
            objects[theobject->theob.lan->minauto]->parent = -1;
        }
        // Si le langage est associèe à un morphism syntaxique
        // le flag qui le protège.
        if (theobject->theob.lan->syntmor != -1)
        {
            objects[theobject->theob.lan->syntmor]->parent = -1;
        }
        free(theobject->theob.lan);
        break;

    default:
        break;
    }
    free(theobject);
}

/*************************************/
/* Récupération/insertion d'un objet */
/*************************************/

int object_get_from_name(char *name)
{
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i] && strcmp(objects[i]->name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int object_delete_from_name(char *name)
{
    int i = object_get_from_name(name);
    if (i == -1)
    {
        fprintf(stderr, "Error: unknown variable.\n");
        return -1;
    }
    else if (objects[i] && objects[i]->parent != -1)
    {
        fprintf(stderr, "Error: this is a protected variable.\n");
        return -1;
    }
    else
    {
        object_free(objects[i]);
        nb_objects--;
        if (nb_objects > 0)
        {
            objects[i] = objects[nb_objects];
        }
        return i;
    }
}

int object_add_language_reg(char *name, regexp *theregexp)
{
    if (theregexp == NULL)
    {
        return -1;
    }

    if (nb_objects == MAX_OBJECTS)
    {
        ERROR("Error: too many objects are already stored in memory.\n");
        return -1;
    }

    int i = object_get_from_name(name);

    if (i != -1 && objects[i]->parent == -1)
    {
        // Si il y a déjà un objet à ce nom et qu'il n'est pas protégé
        DEBUG("Warning: an object with name %s already exists\n", name);
        object_free(objects[i]);
        objects[i] = object_language_new_reg(name, theregexp);
        return i;
    }
    else if (i != -1 && objects[i]->parent != -1)
    {
        fprintf(stderr, "Error: cannot assign a new object to a protected variable.\n");
        reg_free(theregexp);
        return -1;
    }
    else
    {
        objects[nb_objects++] = object_language_new_reg(name, theregexp);
        return nb_objects - 1;
    }
}

int object_add_language_nfa(char *name, int j)
{
    if (j == -1 || objects[j]->type != AUTOMATA)
    {
        return -1;
    }

    if (nb_objects == MAX_OBJECTS)
    {
        ERROR("Error: too many objects are already stored in memory.\n");
        return -1;
    }
    int i = object_get_from_name(name);

    if (i != -1 && objects[i]->parent == -1)
    {
        // Si il y a déjà un objet à ce nom et qu'il n'est pas protégé
        DEBUG("Warning: an object with name %s already exists\n", name);
        object_free(objects[i]);
        objects[i]         = object_language_new_nfa(name, j);
        objects[j]->parent = i;
        return i;
    }
    else if (i != -1 && objects[i]->parent != -1)
    {
        fprintf(stderr, "Error: cannot assign a new object to a protected variable.\n");
        return -1;
    }
    else
    {
        objects[nb_objects++] = object_language_new_nfa(name, j);
        objects[j]->parent    = nb_objects - 1;
        return nb_objects - 1;
    }
}

int object_add_morphism(char *name, cayley *M)
{
    if (M == NULL)
    {
        return -1;
    }

    if (nb_objects == MAX_OBJECTS)
    {
        ERROR("Error: too many objects are already stored in memory.\n");
        return -1;
    }

    int i = object_get_from_name(name);

    // Si il y a déjà un objet à ce nom
    if (i != -1 && objects[i]->parent == -1)
    {
        DEBUG("Warning: an object with name %s already exists\n", name);
        object_free(objects[i]);
        objects[i] = object_morphism_new(name, M);
        return i;
    }
    else if (i != -1 && objects[i]->parent != -1)
    {
        fprintf(stderr, "Error: cannot assign a new object to a protected variable.\n");
        free_cayley(M);
        return -1;
    }
    else
    {
        objects[nb_objects++] = object_morphism_new(name, M);
        return nb_objects - 1;
    }
}

int object_add_automata(char *name, nfa *A)
{
    if (A == NULL)
    {
        return -1;
    }

    if (nb_objects == MAX_OBJECTS)
    {
        ERROR("Error: too many objects are already stored in memory.\n");
        return -1;
    }

    int i = object_get_from_name(name);

    // Si il y a déjà un objet à ce nom
    if (i != -1 && objects[i]->parent == -1)
    {
        DEBUG("Warning: an object with name %s already exists\n", name);
        object_free(objects[i]);
        objects[i] = object_automata_new(name, A);
        return i;
    }
    else if (i != -1 && objects[i]->parent != -1)
    {
        fprintf(stderr, "Error: cannot assign a new object to a protected variable.\n");
        delete_nfa(A);
        return -1;
    }
    else
    {
        objects[nb_objects++] = object_automata_new(name, A);
        return nb_objects - 1;
    }
}

static int object_compare(const void *pob1, const void *pob2)
{
    object *ob1 = *(object * const *)pob1;
    object *ob2 = *(object * const *)pob2;
    if (ob1 == NULL)
    {
        if (ob2 == NULL)
        {
            return 0;
        }
        return -1;
    }
    if (ob2 == NULL)
    {
        return 1;
    }
    return strcmp(ob1->name, ob2->name);
}

void object_sort_array(void)
{
    qsort(objects, nb_objects, sizeof(object *), &object_compare);
}

/****************************/
/* Affichage d'informations */
/****************************/

void object_print_langs(void)
{
    // Calcul du nombre de langages
    ushort count = 0;
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i]->type == LANGUAGE)
        {
            count++;
        }
    }

    printf("#### There are %d language(s) in memory:\n\n", count);

    if (count > 0)
    {
        count = 1;
    }
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i]->type == LANGUAGE)
        {
            printf("#### %d: %s\n", count, objects[i]->name);
            if (objects[i]->theob.lan->type == SPE_REG)
            {
                printf("     Specified by a regular expression\n");
                printf("     Alphabet size: %d\n", objects[i]->theob.lan->spe.reg->size_alph);
                printf("     Regexp: ");
                reg_print(objects[i]->theob.lan->spe.reg);
            }
            else if (objects[i]->theob.lan->type == SPE_NFA)
            {
                printf("     Specified by an automaton.\n");
            }
            printf("\n");
            count++;
        }
    }
}

void object_print_autos(void)
{
    // Calcul du nombre d'automates'
    ushort count = 0;
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i]->type == AUTOMATA)
        {
            count++;
        }
    }

    printf("#### There are %d automata in memory:\n\n", count);

    if (count > 0)
    {
        count = 1;
    }
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i]->type == AUTOMATA)
        {
            printf("#### %d: %s\n", count, objects[i]->name);
            if (objects[i]->parent != -1 && objects[objects[i]->parent]->type == LANGUAGE)
            {
                printf("     Protected: tied to the language %s.\n",
                       objects[objects[i]->parent]->name);
            }
            printf("\n");
            count++;
        }
    }
}

void object_print_morps(void)
{
    // Calcul du nombre de morphismes
    ushort count = 0;
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i]->type == MORPHISM)
        {
            count++;
        }
    }

    printf("#### There are %d morphism(s) in memory:\n\n", count);

    if (count > 0)
    {
        count = 1;
    }
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i]->type == MORPHISM)
        {
            printf("#### %d: %s\n", count, objects[i]->name);
            if (objects[i]->parent != -1 && objects[objects[i]->parent]->type == LANGUAGE)
            {
                printf("     Protected: tied to the language %s.\n",
                       objects[objects[i]->parent]->name);
            }
            printf("\n");
            count++;
        }
    }
}

/************************************************/
/* Calculs d'informations sur un objet existant */
/************************************************/

// Calcul du DFA minimal d'un langage
int shell_compute_minimal(int i)
{
    if (objects[i]->type != LANGUAGE)
    {
        fprintf(stderr, "Should be a language.\n");
        return -1;
    }
    language *lang = objects[i]->theob.lan;
    if (lang->minauto == -1)
    {
        DEBUG("Computing the minimal automaton of a language");
        p_nfa A;
        if (lang->type == SPE_REG)
        {
            A = reg2nfa(lang->spe.reg);
        }
        else           // if (lang->type == SPE_NFA) {
        {
            A = nfa_minimize(objects[lang->spe.nfa]->theob.aut->nfa);
        }
        char *newname;
        MALLOC(newname, 20);
        sprintf(newname, "SYSMINI%04d", minicount++);
        lang->minauto                  = object_add_automata(newname, nfa_minimize(A));
        // On protège le NFA minimal qu'on a créé
        objects[lang->minauto]->parent = i;
        delete_nfa(A);
    }
    return lang->minauto;
}

// Calcul du morphisme syntaxique d'un langage
int shell_compute_syntac(int i)
{
    if (objects[i]->type != LANGUAGE)
    {
        fprintf(stderr, "Should be a language.\n");
        return -1;
    }
    language *lang = objects[i]->theob.lan;
    // On commence par calculer l'automate minimal du langage (si ce n'est pas
    // déjà fait)
    automata *mini = objects[shell_compute_minimal(i)]->theob.aut;
    if (lang->syntmor == -1)
    {
        DEBUG("Computing the syntactic morphism of a language");
        char *newname;
        MALLOC(newname, 20);
        sprintf(newname, "SYSSYNT%04d", syntcount++);
        // On protège le morphisme syntaxique qu'on a créé
        lang->syntmor                  = object_add_morphism(newname,
                                                             dfa_to_right_cayley(mini->nfa));
        objects[lang->syntmor]->parent = i;
    }
    return lang->syntmor;
}

// Calcul de la table de multiplication d'un morphism
void shell_compute_mult(morphism *morp)
{
    compute_mult(morp->cayley);
}

// Calcul des relations de green d'un morphisme
void shell_compute_green(morphism *themor)
{
    if (themor->green == NULL)
    {
        themor->green = cayley_green_compute(themor->cayley);
    }
}

// Calcul de l'ordre syntactique d'un morphisme
void shell_compute_order(morphism *mor)
{
    if (mor->cayley->order == NULL)
    {
        compute_syntac_order(mor->cayley);
    }
}

// Calcul du MOD-kernel d'un morphisme
void shell_compute_mker(morphism *themor)
{
    if (themor->mker == NULL)
    {
        // On calcule d'abord les relations de Green
        shell_compute_green(themor);
        themor->mker = get_mod_kernel(themor->cayley, themor->green);
    }
}

// Calcul du GR-kernel d'un morphisme
void shell_compute_gker(morphism *themor)
{
    if (themor->gker == NULL)
    {
        // On calcule d'abord les relations de Green
        shell_compute_green(themor);
        themor->gker = get_grp_kernel(themor->cayley, themor->green);
    }
}

// Calcul des DD-orbites
void shell_compute_ddorbs(morphism *mor)
{
    if (mor->ddorbs == NULL)
    {
        shell_compute_green(mor);
        mor->ddorbs = compute_all_ddorbs(mor->cayley, mor->green);
    }
}

// Calcul des MOD⁺-orbites
void shell_compute_mporbs(morphism *mor)
{
    if (mor->mporbs == NULL)
    {
        shell_compute_green(mor);
        shell_compute_mker(mor);
        mor->mporbs = get_all_modporbs(mor->mker);
    }
}

// Calcul des AT-orbites
void shell_compute_atorbs(morphism *mor)
{
    shell_compute_green(mor);

    if (mor->atorbs == NULL)
    {
        mor->atorbs = compute_all_atorbs(mor->cayley, mor->green);
    }
}

// Calcul des BPol(MOD)-orbites
void shell_compute_bpmorbs(morphism *mor)
{
    // Si le morphisme a une lettre neutre, alors les BPol(MOD)-orbites sont
    // exactement les AT-orbites (il suffit donc de calculer ces dernières)
    if (mor->cayley->neutlet)
    {
        shell_compute_atorbs(mor);
        return;
    }
    else if (mor->bpmorbs == NULL)
    {
        shell_compute_green(mor);
        shell_compute_mker(mor);
        mor->bpmorbs = get_bpgorbs(mor->cayley, mor->green, mor->mker);
    }
}

p_orbits shell_get_bpmorbs(morphism *mor)
{
    if (mor->bpmorbs)
    {
        return mor->bpmorbs;
    }
    else
    {
        return mor->atorbs;
    }
}

// Calcul des BPol(GR)-orbites
void shell_compute_bpgorbs(morphism *mor)
{
    if (mor->bpgorbs == NULL)
    {
        shell_compute_green(mor);
        shell_compute_gker(mor);
        mor->bpgorbs = get_bpgorbs(mor->cayley, mor->green, mor->gker);
    }
}

// Calcul des BPol(DD)-orbites
void shell_compute_bpddorbs(morphism *mor)
{
    shell_compute_green(mor);

    // Si le neutre a un antécédent non-vide, les BPol(DD)-orbites sont
    // exactement
    // les AT-orbites (il suffit donc de calculer ces dernières)
    if (cayley_neutstrict(mor->cayley, mor->green))
    {
        shell_compute_atorbs(mor);
        return;
    }
    else if (mor->bpddorbs == NULL)
    {
        shell_compute_ddorbs(mor);
        mor->bpddorbs = get_bpgporbs(mor->cayley, mor->green, mor->ddorbs);
    }
}

p_orbits shell_get_bpddorbs(morphism *mor)
{
    if (mor->bpddorbs)
    {
        return mor->bpddorbs;
    }
    else
    {
        return mor->atorbs;
    }
}

// Calcul des BPol(MOD⁺)-orbites
void shell_compute_bpmporbs(morphism *mor)
{
    shell_compute_green(mor);
    // Si le neutre a un antécédent non-vide, les BPol(MOD⁺)-orbites sont
    // exactement les BPol(MOD)-orbites (il suffit donc de calculer ces
    // dernières)
    if (cayley_neutstrict(mor->cayley, mor->green))
    {
        shell_compute_bpmorbs(mor);
        return;
    }
    else if (mor->bpmporbs == NULL)
    {
        shell_compute_mporbs(mor);
        mor->bpmporbs = get_bpgporbs(mor->cayley, mor->green, mor->mporbs);
    }
}

p_orbits shell_get_bpmporbs(morphism *mor)
{
    if (mor->bpmporbs)
    {
        return mor->bpmporbs;
    }
    else
    {
        return shell_get_bpmorbs(mor);
    }
}
