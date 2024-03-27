/*****************************/
/* Gestionnaire de commandes */
/*****************************/

#include "shell_commands.h"

/******************************************************************************/
/* Récupération des automates et des sous-monoides désignés par une commande */
/******************************************************************************/

p_genob shell_genob_from_command(com_command *thecom)
{
    // Si la commande est mal formée
    if (thecom == NULL || thecom->main == NULL)
    {
        return NULL;
    }

    // On récupère l'objet correspondant au premier maillon
    int i = object_get_from_name(thecom->main->string);
    if (i == -1)
    {
        return NULL;
    }

    // L'objet généralisé qu'on va retourner
    p_genob new = NULL;

    // La chaîne complète
    string_chain *thechain = thecom->main;

    // Tant qu'il y a des maillons
    while (thechain != NULL)
    {
        // On prend le prochain maillon
        thechain = thechain->next;
        // On regarde le type de l'objet
        switch (objects[i]->type)
        {
        case LANGUAGE: {
            // Si il n'y pas de maillon suivant et pas de paramètre
            // La commande désigne simplement le langage
            if (thechain == NULL && thecom->params == NULL)
            {
                MALLOC(new, 1);
                new->type  = OG_LANG;
                new->theob = objects[i]->theob.lan;
                return new;
            }

            // Sinon la commande continue pour désigner soit l'automate minimal,
            // soit
            // le morphisme syntactique du langage
            com_keyword key = key_from_string_chain(thechain);
            switch (key)
            {
            case CM_MINI:
                i = shell_compute_minimal(i);
                break;
            case CM_SYNT:
                i = shell_compute_syntac(i);
                break;
            default:
                return NULL;
                break;
            }
        }
        break;
        case AUTOMATA: {
            // Pour l'instant seul le cas d'un automate simple est géré
            if (thechain == NULL && thecom->params == NULL)
            {
                MALLOC(new, 1);
                new->type  = OG_AUTO;
                new->theob = objects[i]->theob.aut->nfa;
                return new;
            }
            else
            {
                return NULL;
            }
        }
        case MORPHISM: {
            morphism *themor = objects[i]->theob.mor;
            // Si il n'y pas de maillon suivant et pas de paramètres
            // La commande désigne simplement le morphisme
            if (thechain == NULL && thecom->params == NULL)
            {
                MALLOC(new, 1);
                new->type  = OG_MORP;
                new->theob = objects[i]->theob.mor->cayley;
                return new;
            }

            // Sinon, il doit reste un unique maillon désignant le paramètre du
            // morphisme qui nous intéresse
            if (thechain->next != NULL)
            {
                return NULL;
            }
            com_keyword key = key_from_string_chain(thechain);
            switch (key)
            {
            case CM_MKERNEL: {
                shell_compute_mker(themor);

                MALLOC(new, 1);
                new->type  = OG_SUBM;
                new->theob = themor->mker;
                return new;
                break;
            }
            case CM_GKERNEL: {
                shell_compute_gker(themor);

                MALLOC(new, 1);
                new->type  = OG_SUBM;
                new->theob = themor->gker;
                return new;
                break;
            }
            case CM_KERNEL: {
                if (com_nbparams(thecom->params) != 1 || !com_single(thecom->params->param))
                {
                    shell_arguments_error();
                    return NULL;
                }
                com_keyword class = key_from_string_chain_single(thecom->params->param->main);
                if (class == CL_MOD)
                {
                    shell_compute_mker(themor);

                    MALLOC(new, 1);
                    new->type  = OG_SUBM;
                    new->theob = themor->mker;
                    return new;
                    break;
                }
                if (class == CL_GR)
                {
                    shell_compute_gker(themor);

                    MALLOC(new, 1);
                    new->type  = OG_SUBM;
                    new->theob = themor->gker;
                    return new;
                    break;
                }
                break;
            }
            case CM_ORBIT: {
                int n = com_nbparams(thecom->params);
                if (n < 2 || n > 3)
                {
                    shell_arguments_error();
                    return NULL;
                }
                com_command *theidem = com_getparam(thecom->params, n - 1);

                // Récupération du numéro de l'idempotent dont on doit tester
                // l'orbite.
                if (!com_single(theidem))
                {
                    shell_arguments_error();
                    return NULL;
                }
                uint j;
                char *word = theidem->main->string;
                uint s;
                if (!cayley_elem_from_string(themor->cayley, word, &s))
                {
                    fprintf(stdout,
                            "Error: this word does not correspond to an element of the monoid.\n");
                    return NULL;
                }
                if (!cayley_num_idem(themor->cayley, s, &j))
                {
                    fprintf(stdout, "Error: this is not an idempotent element.\n");
                    return NULL;
                }

                com_keyword arg1 = key_from_string_chain_single(com_getparam(thecom->params,
                                                                             0)->main);

                if (n == 2)
                {
                    switch (arg1)
                    {
                    case CL_DD:
                        shell_compute_ddorbs(themor);

                        MALLOC(new, 1);
                        new->type  = OG_SUBM;
                        new->theob = themor->ddorbs->orbits[j];
                        return new;
                        break;
                    case CL_MODP:
                        shell_compute_mporbs(themor);

                        MALLOC(new, 1);
                        new->type  = OG_SUBM;
                        new->theob = themor->mporbs->orbits[j];
                        return new;
                        break;
                    case CL_AT:
                        shell_compute_atorbs(themor);
                        MALLOC(new, 1);
                        new->type  = OG_SUBM;
                        new->theob = themor->atorbs->orbits[j];
                        return new;
                        break;
                    default:
                        return NULL;
                        break;
                    }
                }
                else if (n == 3)
                {
                    com_keyword arg2 = key_from_string_chain_single(com_getparam(thecom->params,
                                                                                 1)->main);
                    if (arg1 != OP_TLC && arg1 != OP_BPOL)
                    {
                        return NULL;
                    }
                    switch (arg2)
                    {
                    case CL_ST:
                        shell_compute_atorbs(themor);

                        MALLOC(new, 1);
                        new->type  = OG_SUBM;
                        new->theob = themor->atorbs->orbits[j];
                        return new;
                        break;
                    case CL_DD:
                        shell_compute_bpddorbs(themor);

                        MALLOC(new, 1);
                        new->type  = OG_SUBM;
                        new->theob = shell_get_bpddorbs(themor)->orbits[j];
                        return new;
                        break;
                    case CL_MOD:
                        shell_compute_bpmorbs(themor);

                        MALLOC(new, 1);
                        new->type  = OG_SUBM;
                        new->theob = shell_get_bpmorbs(themor)->orbits[j];
                        return new;
                        break;
                    case CL_MODP:
                        shell_compute_bpmporbs(themor);
                        MALLOC(new, 1);
                        new->type  = OG_SUBM;
                        new->theob = shell_get_bpmporbs(themor)->orbits[j];
                        return new;
                        break;
                    case CL_GR:
                        shell_compute_bpgorbs(themor);

                        MALLOC(new, 1);
                        new->type  = OG_SUBM;
                        new->theob = themor->bpgorbs->orbits[j];
                        return new;
                        break;
                    default:
                        break;
                    }
                    break;
                }
                break;
            }
            break;
            case OG_ORBS: {
                int n = com_nbparams(thecom->params);
                if (n < 1 || n > 2)
                {
                    shell_arguments_error();
                    return NULL;
                }
                com_keyword arg1 = key_from_string_chain_single(thecom->params->param->main);
                if (n == 1)
                {
                    switch (arg1)
                    {
                    case CL_DD:
                        MALLOC(new, 1);
                        new->type  = OG_ORBS;
                        new->theob = themor->ddorbs;
                        return new;
                        break;
                    case CL_MODP:

                        MALLOC(new, 1);
                        new->type  = OG_ORBS;
                        new->theob = themor->mporbs;
                        return new;
                        break;
                    case CL_AT:

                        MALLOC(new, 1);
                        new->type  = OG_ORBS;
                        new->theob = themor->atorbs;
                        return new;
                        break;
                    default:
                        return NULL;
                        break;
                    }
                }
                else if (n == 2)
                {
                    com_keyword arg2 =
                        key_from_string_chain_single(thecom->params->next->param->main);
                    switch (arg1)
                    {
                    case OP_BPOL:
                    case OP_TLC:
                        switch (arg2)
                        {
                        case CL_ST:
                            MALLOC(new, 1);
                            new->type  = OG_ORBS;
                            new->theob = themor->atorbs;
                            return new;
                            break;
                        case CL_DD:
                            MALLOC(new, 1);
                            new->type  = OG_ORBS;
                            new->theob = shell_get_bpddorbs(themor);
                            return new;
                            break;
                        case CL_MOD:
                            MALLOC(new, 1);
                            new->type  = OG_ORBS;
                            new->theob = shell_get_bpmorbs(themor);
                            return new;
                            break;
                        case CL_MODP:

                            MALLOC(new, 1);
                            new->type  = OG_ORBS;
                            new->theob = shell_get_bpmporbs(themor);
                            return new;
                            break;
                        case CL_GR:

                            MALLOC(new, 1);
                            new->type  = OG_ORBS;
                            new->theob = themor->bpgorbs;
                            return new;
                            break;
                        default:
                            return NULL;
                            break;
                        }
                        break;

                    default:
                        return NULL;
                        break;
                    }
                }
            }

            break;
            default:
                return NULL;
                break;
            }
        }
        break;

        default:
            break;
        }
    }
    return NULL;
}

/******************************/
/* Application d'une commande */
/******************************/

static bool com_apply_command_rec(string_chain *thechain, com_parameters *pars, int i)
{
    if (i == -1)
    {
        shell_command_error();
        return false;
    }

    switch (objects[i]->type)
    {
    case LANGUAGE: {
        language *thelang = objects[i]->theob.lan;
        if (thechain == NULL && pars == NULL)
        {
            if (thelang->type == SPE_REG)
            {
                fprintf(stdout, "This language is specified by a regular expression:\n");
                reg_print(reg_simplify(thelang->spe.reg));
            }
            else if (thelang->type == SPE_NFA)
            {
                fprintf(stdout, "This language is specified by an automaton:\n");
                view_nfa(objects[thelang->spe.nfa]->theob.aut->nfa);
            }
            return true;
        }

        com_keyword key = key_from_string_chain(thechain);

        switch (key)
        {
        case CM_MINI:
            shell_compute_minimal(i);
            return com_apply_command_rec(thechain->next, pars, thelang->minauto);
            break;
        case CM_SYNT:
            shell_compute_syntac(i);
            return com_apply_command_rec(thechain->next, pars, thelang->syntmor);
            break;
        default:
            shell_command_error();
            return false;
            break;
        }
    }
    break;
    case AUTOMATA: {
        automata *theauto = objects[i]->theob.aut;
        if (thechain == NULL && pars == NULL)
        {
            print_title_box(100, true, stdout, 1, "Automaton.");
            view_nfa(theauto->nfa);
            return true;
        }
        com_keyword key = key_from_string_chain_single(thechain);
        switch (key)
        {
        case CM_RUN:
            shell_make_nfa_run(theauto, pars);
            break;

        default:
            shell_command_error();
            return false;
            break;
        }
    }
    break;
    case MORPHISM: {
        morphism *themor = objects[i]->theob.mor;
        if (thechain == NULL)
        {
            return shell_view_morphism(themor, pars);
        }
        com_keyword key = key_from_string_chain_single(thechain);
        switch (key)
        {
        case CM_RCAY:
            return shell_view_rcayley(themor, pars);
            break;
        case CM_LCAY:
            return shell_view_lcayley(themor, pars);
            break;
        case CM_MULT:
            return shell_view_mormult(themor, pars);
            break;
        case CM_IDEMS:
            return shell_view_idems(themor, pars);
            break;
        case CM_ORDER:
            return shell_view_morder(themor, pars);
            break;
        case CM_MKERNEL:
            return shell_view_mkernel(themor, pars);
            break;
        case CM_GKERNEL:
            return shell_view_gkernel(themor, pars);
            break;
        case CM_KERNEL:
            return shell_view_kernel(themor, pars);
            break;
        case CM_ORBIT:
            return shell_view_oneorbit(themor, pars);
            break;
        case CM_ORBITS:
            return shell_view_orbits(themor, pars);
            break;
        case CM_IMAGE:
            return shell_compute_image(themor, pars);
            break;
        default:
            shell_command_error();
            return false;
            break;
        }
    }
    break;
    default:
        shell_command_error();
        return false;
        break;
    }

    return false;
}

bool com_apply_command(com_command *thecom)
{
    if (thecom == NULL || thecom->main == NULL)
    {
        shell_command_error();
        return false;
    }

    // On commence par regarder si le premier maillon de la commande correspond
    // à
    // une variable.
    int i = object_get_from_name(thecom->main->string);
    if (i != -1)
    {
        // Si c'est le cas, la commande est récursive
        return com_apply_command_rec(thecom->main->next, thecom->params, i);
    }

    // Sinon, le premier maillon doit être le seul et doit correspondre à un
    // keyword
    if (thecom->main->next)
    {
        shell_command_error();
        return false;
    }

    com_keyword key = string_to_keyword(thecom->main->string);
    switch (key)
    {
    case CM_MEMB:
        return shell_membership_test(thecom->params);
        break;
    case CM_CONCAT:
        return shell_concat_summary(thecom->params);
        break;
    case CM_NEGHIERA:
        return shell_neghiera(thecom->params);
        break;
    case CM_FPHIERA:
        return shell_fphiera(thecom->params);
        break;
    case CM_SEPAR:
        return shell_separation_test(thecom->params);
        break;
    case CM_SAVE:
        return shell_save_to_file(thecom->params);
        break;
    case CM_SAVESESSION:
        return shell_save_session(thecom->params);
        break;
    case CM_LOADSESSION:
        return shell_load_session(thecom->params);
        break;
    case CM_DELETE:
        return shell_delete(thecom->params);
        break;
    case CM_SORT:
        return shell_sort(thecom->params);
        break;
    case CM_RECDEF:
        return shell_recursion_lang(thecom->params);
        break;
    case PR_COMM:
        return shell_prop_comm(thecom->params);
        break;
    default:
        shell_command_error();
        return false;
        break;
    }
    shell_command_error();
    return false;
}

int com_apply_link_command(char *name, com_command *thecom)
{
    if (thecom == NULL || thecom->main == NULL)
    {
        shell_command_error();
        return -1;
    }
    if (!check_varname(name))
    {
        return -1;
    }

    // Premier cas: la commmande est juste un texte à traiter comme une regexp
    if (thecom->thetype == CMT_RAW)
    {
        regexp *myexp = parse_string_regexp(thecom->main->string);
        if (myexp == NULL)
        {
            return -1;
        }
        int i = object_add_language_reg(name, myexp);
        reg_print(reg_simplify(myexp));
        return i;
    }

    // Si le premier maillon est un nom de variable, alors il s'agit juste de
    // faire une copie
    int i = index_from_string_chain(thecom->main);
    if (i != -1 && thecom->params == NULL)
    {
        object_copy_generic(i, name);
        return true;
    }

    // On sait maintenant que le premier argument est un keyword
    // Dans ce cas il ne peut pas y avoir de nesting.
    com_keyword key = key_from_string_chain_single(thecom->main);
    switch (key)
    {
    case CM_THOMSON:
        return shell_thomson_nfa(name, thecom->params);
        break;
    case CM_ELIMEPS:
        return shell_elimeps_nfa(name, thecom->params);
        break;
    case CM_TRIMNFA:
        return shell_trim_nfa(name, thecom->params);
        break;
    case CM_KLEENE:
        return shell_kleene_nfa(name, thecom->params);
        break;
    case CM_INTERSEC:
        return shell_intersect_nfa(name, thecom->params);
        break;
    case CM_CONCAT:
        return shell_concat_nfa(name, thecom->params);
        break;
    case CM_UNION:
        return shell_union_nfa(name, thecom->params);
        break;
    case CM_DETERMIN:
        return shell_determinize_nfa(name, thecom->params);
        break;
    case CM_MINI:
        return shell_minimize_nfa(name, thecom->params);
        break;
    case CM_LINK:
        return shell_linktolang_nfa(name, thecom->params);
        break;
    case CM_OPEN:
        return shell_open_object(name, thecom->params);
        break;
    case CM_RNFA:
        return shell_random_nfa(name, thecom->params, false);
        break;
    case CM_RDFA:
        return shell_random_nfa(name, thecom->params, true);
        break;
    default:
        shell_command_error();
        return false;
        break;
    }
    shell_command_error();
    return false;
}

/******************************/
/* Calculs de nouveaux objets */
/******************************/

// Ajout d'un nouvel objet en copiant un objet déjà existant dans la table
int object_copy_generic(int i, char *newname)
{
    if (i == -1)
    {
        fprintf(stderr, "Error: this is not a valid object to copy.\n");
        return -1;
    }
    switch (objects[i]->type)
    {
    case AUTOMATA:
        return object_add_automata(newname, nfa_copy(objects[i]->theob.aut->nfa));
        break;
    case LANGUAGE:
        if (objects[i]->theob.lan->type == SPE_REG)
        {
            return object_add_language_reg(newname, reg_copy(objects[i]->theob.lan->spe.reg));
        }
        else
        {
            int j = object_add_automata(newname,
                                        nfa_copy(
                                            objects[objects[i]->theob.lan->spe.nfa]->theob.aut->nfa));
            return object_add_language_nfa(newname, j);
        }

    default:
        return -1;
        break;
    }
}

// Calcule un nouveau NFA avec la méthode de thomson à partir d'un langage.
int shell_thomson_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 1)
    {
        shell_arguments_error();
        return -1;
    }
    int i = index_from_string_chain(pars->param->main);
    if (i == -1)
    {
        shell_variable_error();
        return false;
    }

    if (i < 0 || objects[i]->type != LANGUAGE || objects[i]->theob.lan->type != SPE_REG)
    {
        fprintf(stderr,
                "Error: Thomson's algorithm can only be applied to a language specified by a regular expression.\n");
        return -1;
    }
    DEBUG("Computing an automaton from a language");
    return object_add_automata(varname, reg2nfa(objects[i]->theob.lan->spe.reg));
}

// Union de deux NFAs
int shell_union_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 2)
    {
        shell_arguments_error();
        return -1;
    }
    int i1 = index_from_string_chain(pars->param->main);
    int i2 = index_from_string_chain(pars->next->param->main);
    if (i1 == -1 || i2 == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i1 < 0 || i2 < 0 || objects[i1]->type != AUTOMATA || objects[i2]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: The union algorithm requires two automata as input.\n");
        return -1;
    }
    return object_add_automata(varname,
                               nfa_union(objects[i1]->theob.aut->nfa, objects[i2]->theob.aut->nfa));
}

// Concatène deux NFAs
int shell_concat_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 2)
    {
        shell_arguments_error();
        return -1;
    }
    int i1 = index_from_string_chain(pars->param->main);
    int i2 = index_from_string_chain(pars->next->param->main);
    if (i1 == -1 || i2 == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i1 < 0 || i2 < 0 || objects[i1]->type != AUTOMATA || objects[i2]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: The concatenation algorithm requires two automata as input.\n");
        return -1;
    }
    return object_add_automata(varname,
                               nfa_concat(objects[i1]->theob.aut->nfa,
                                          objects[i2]->theob.aut->nfa));
}

// Étoile de Kleene sur un NFA
int shell_kleene_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 1)
    {
        shell_arguments_error();
        return -1;
    }
    int i = index_from_string_chain(pars->param->main);
    if (i == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i < 0 || objects[i]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: The Kleene star algorithm can only be applied to an automaton.\n");
        return -1;
    }
    return object_add_automata(varname, nfa_star(objects[i]->theob.aut->nfa));
}

// Calcule un nouveau NFA en éliminant les transitions epsilon
int shell_elimeps_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 1)
    {
        shell_arguments_error();
        return -1;
    }
    int i = index_from_string_chain(pars->param->main);
    if (i == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i < 0 || objects[i]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: Can only eliminate the epsilon transitions in an automaton.\n");
        return -1;
    }
    return object_add_automata(varname, nfa_elimeps(objects[i]->theob.aut->nfa));
}

// Calcule un nouveau NFA en supprimant tous les états non-accessibles ou non
// co-accessibles
int shell_trim_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 1)
    {
        shell_arguments_error();
        return -1;
    }
    int i = index_from_string_chain(pars->param->main);
    if (i == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i < 0 || objects[i]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: Can only trim an automaton.\n");
        return -1;
    }
    return object_add_automata(varname, nfa_trim(objects[i]->theob.aut->nfa));
}

// Calcule un nouveau NFA en réalisant l'intersection de deux NFAs existants
int shell_intersect_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 2)
    {
        shell_arguments_error();
        return -1;
    }
    int i1 = index_from_string_chain(pars->param->main);
    int i2 = index_from_string_chain(pars->next->param->main);
    if (i1 == -1 || i2 == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i1 < 0 || i2 < 0 || objects[i1]->type != AUTOMATA || objects[i2]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: The intersection algorithm requires two automata as input.\n");
        return -1;
    }
    return object_add_automata(varname,
                               nfa_intersect(objects[i1]->theob.aut->nfa,
                                             objects[i2]->theob.aut->nfa,
                                             true));
}

// Calcule un nouveau NFA déterministe complet en déterminisant un NFA déjà
// existant
int shell_determinize_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 1)
    {
        shell_arguments_error();
        return -1;
    }
    int i = index_from_string_chain(pars->param->main);
    if (i == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i < 0 || objects[i]->type != AUTOMATA || objects[i]->theob.aut->nfa->etrans != NULL ||
        objects[i]->theob.aut->nfa->itrans != NULL)
    {
        fprintf(stderr,
                "Error: The subset construction can only be applied to an automaton without epsilon transitions.\n");
        return -1;
    }
    return object_add_automata(varname, nfa_determinize(objects[i]->theob.aut->nfa, true));
}

// Calcule un automate minimal à partir d'un NFA
int shell_minimize_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 1)
    {
        shell_arguments_error();
        return -1;
    }
    int i = index_from_string_chain(pars->param->main);
    if (i == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i < 0 || objects[i]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: the minimization algorithm can only be applied to an automaton.\n");
        return -1;
    }
    return object_add_automata(varname, nfa_minimize(objects[i]->theob.aut->nfa));
}

// Links an existing nfa to a nex language
int shell_linktolang_nfa(char *varname, com_parameters *pars)
{
    if (com_nbparams(pars) != 1)
    {
        shell_arguments_error();
        return -1;
    }
    int i = index_from_string_chain(pars->param->main);
    if (i == -1)
    {
        shell_variable_error();
        return false;
    }
    if (i < 0 || objects[i]->type != AUTOMATA)
    {
        fprintf(stderr, "Error: the argument should be an automata variable.\n");
        return -1;
    }
    return object_add_language_nfa(varname, i);
}

// Calcul d'une fammille de langages définie récursivement
bool shell_recursion_lang(com_parameters *params)
{
    int nb_params = com_nbparams(params);
    if (nb_params < 3 || nb_params % 2 != 1)
    {
        shell_arguments_error();
        return false;
    }
    int lcount           = (nb_params - 1) / 2;
    com_command **partab = com_getparamarray(params);

    // Calcul de la table des indices des langages impliqués dans le calcul
    int indtab[lcount];
    for (int i = 0; i < lcount; i++)
    {
        if (!com_single(partab[i]))
        {
            shell_arguments_error();
            return false;
        }
        indtab[i] = object_get_from_name(partab[i]->main->string);
        if (indtab[i] == -1)
        {
            shell_variable_error();
            return false;
        }
    }
    // Calcul de la table des chaînes représentant les expressions utilisées
    // dans
    // la récursion
    char *expstab[lcount];
    for (int i = 0; i < lcount; i++)
    {
        int j = i + lcount;
        if (!com_israw(partab[j]))
        {
            shell_arguments_error();
            return false;
        }
        expstab[i] = partab[j]->main->string;
    }

    // Récupération du nombre de récursions demandées
    if (!com_single(partab[nb_params - 1]))
    {
        shell_arguments_error();
        return false;
    }
    char *end;
    int nrec = strtol(partab[nb_params - 1]->main->string, &end, 10);
    if (*end != '\0')
    {
        shell_arguments_error();
        return false;
    }

    // Calcul de la table des noms des langages utilisés dans le calcul
    // Création des copies initiales
    char *namestab[lcount];
    for (int i = 0; i < lcount; i++)
    {
        namestab[i] = strdup(partab[i]->main->string);
        char *tempname;
        MALLOC(tempname, 20);
        sprintf(tempname, "%s%03d", namestab[i], 0);
        object_copy_generic(indtab[i], tempname);
    }

    for (int j = 0; j <= nrec; j++)
    {
        for (int i = 0; i < lcount; i++)
        {
            regexp *myexp = parse_string_regexp(expstab[i]);
            if (myexp == NULL)
            {
                return false;
            }
            object_add_language_reg(namestab[i], myexp);
            char *tempname;
            MALLOC(tempname, 20);
            sprintf(tempname, "%s%03d", namestab[i], j);
            object_copy_generic(indtab[i], tempname);
        }
    }

    free(partab);

    return true;
}

// Sauvegarde d'un objet
bool shell_save_to_file(com_parameters *params)
{
    if (com_nbparams(params) != 2)
    {
        shell_arguments_error();
        return false;
    }
    com_command **pararray = com_getparamarray(params);
    int i                  = index_from_string_chain(pararray[0]->main);
    if (i == -1)
    {
        shell_variable_error();
        return false;
    }
    if (!com_israw(pararray[1]))
    {
        fprintf(stderr, "Error: cannot parse the filename\n");
        return false;
    }
    char *filename = pararray[1]->main->string;
    printf("saving %s in the file: \"%s\".\n", params->param->main->string, filename);
    files_save_object(objects[i], filename);
    free(pararray);
    return true;
}

// Sauvegarde d'une session complète
bool shell_save_session(com_parameters *params)
{
    if (com_nbparams(params) != 1)
    {
        shell_arguments_error();
        return false;
    }
    if (!com_israw(params->param))
    {
        fprintf(stderr, "Error: cannot parse the filename\n");
        return false;
    }
    char *filename = params->param->main->string;
    printf("saving the session in the file: \"%s\".\n", filename);
    files_save_session(filename);
    return false;
}

// Chargement d'une session à partir d'un fichier
bool shell_load_session(com_parameters *params)
{
    if (com_nbparams(params) != 1)
    {
        shell_arguments_error();
        return false;
    }
    if (!com_israw(params->param))
    {
        fprintf(stderr, "Error: cannot parse the filename\n");
        return false;
    }
    char *filename = params->param->main->string;
    printf("loading the session saved in the file: \"%s\".\n", filename);
    files_load_session(filename);
    return false;
}

// Suppression d'un objet
int shell_delete(com_parameters *params)
{
    if (com_nbparams(params) != 1 || !com_single(params->param))
    {
        shell_arguments_error();
        return false;
    }
    return object_delete_from_name(params->param->main->string);
}

// Tri des objets
bool shell_sort(com_parameters *params)
{
    if (params != NULL)
    {
        shell_arguments_error();
        return false;
    }
    object_sort_array();
    return true;
}

// Ouverture d'un objet
bool shell_open_object(char *varname, com_parameters *params)
{
    if (com_nbparams(params) != 1 || params->param->thetype != CMT_RAW)
    {
        shell_arguments_error();
        return false;
    }
    printf("reading the file: %s\n", params->param->main->string);
    files_read_object(params->param->main->string, varname);
    return 1;
}

// Calcul d'un NFA aléatoire
bool shell_random_nfa(char *varname, com_parameters *params, bool det)
{
    if (com_nbparams(params) != 3)
    {
        shell_arguments_error();
        return false;
    }
    com_command *arg1 = params->param;
    com_command *arg2 = params->next->param;
    com_command *arg3 = params->next->next->param;
    if (!com_single(arg1) || !com_single(arg2) || !com_single(arg3))
    {
        shell_arguments_error();
        return false;
    }

    char *end;
    int nb1 = strtol(arg1->main->string, &end, 10);
    if (*end != '\0')
    {
        shell_arguments_error();
        return false;
    }
    int nb2 = strtol(arg2->main->string, &end, 10);
    if (*end != '\0')
    {
        shell_arguments_error();
        return false;
    }
    int nb3 = strtol(arg3->main->string, &end, 10);
    if (*end != '\0')
    {
        shell_arguments_error();
        return false;
    }

    if (det)
    {
        return object_add_automata(varname, dfa_random(nb1, nb2, nb3));
    }
    else
    {
        return object_add_automata(varname, nfa_random(nb1, nb2, nb3));
    }
}

/********************************************************************/
/* Affichage - fonctions appellées par le gestionnaire de commandes */
/********************************************************************/

bool shell_view_morphism(morphism *themor, com_parameters *pars)
{
    if (pars)
    {
        shell_arguments_error();
        return false;
    }
    shell_compute_green(themor);
    print_title_box(100, true, stdout, 1, "The morphism.");
    view_morphism(themor->cayley, themor->green);
    return true;
}

bool shell_view_rcayley(morphism *themor, com_parameters *pars)
{
    if (pars)
    {
        shell_arguments_error();
        return false;
    }
    print_title_box(100, true, stdout, 1, "Right Cayley graph of the morphism.");
    view_cayley(themor->cayley);
    return true;
}

bool shell_view_lcayley(morphism *themor, com_parameters *pars)
{
    if (pars)
    {
        shell_arguments_error();
        return false;
    }
    compute_left_cayley(themor->cayley);
    print_title_box(100, true, stdout, 1, "Left Cayley graph of the morphism.");
    view_left_cayley(themor->cayley);
    return true;
}

bool shell_view_mormult(morphism *themor, com_parameters *pars)
{
    if (pars)
    {
        shell_arguments_error();
        return false;
    }
    print_title_box(100, true, stdout, 1, "Multiplication of the morphism.");
    shell_compute_mult(themor);
    cayley_mult_print(themor->cayley, stdout);
    return true;
}

bool shell_view_morder(morphism *themor, com_parameters *pars)
{
    if (pars)
    {
        shell_arguments_error();
        return false;
    }
    print_title_box(100, true, stdout, 1, "Ordering of the morphism.");
    shell_compute_order(themor);
    cayley_print_order(themor->cayley, stdout);
    return true;
}

bool shell_view_idems(morphism *themor, com_parameters *pars)
{
    if (pars)
    {
        shell_arguments_error();
        return false;
    }
    cayley_print_idems(themor->cayley, stdout);
    return true;
}

bool shell_view_mkernel(morphism *morp, com_parameters *pars)
{
    if (pars)
    {
        shell_arguments_error();
        return false;
    }
    shell_compute_mker(morp);
    print_title_box(100, true, stdout, 1, "MOD-kernel of the morphism.");
    print_full_green_sub(morp->mker, false, stdout);
    return true;
}

bool shell_view_gkernel(morphism *morp, com_parameters *pars)
{
    if (pars)
    {
        shell_arguments_error();
        return false;
    }
    shell_compute_gker(morp);
    print_title_box(100, true, stdout, 1, "GR-kernel of the morphism.");
    print_full_green_sub(morp->gker, false, stdout);
    return true;
}

bool shell_view_kernel(morphism *themor, com_parameters *pars)
{
    if (com_nbparams(pars) != 1 || !com_single(pars->param))
    {
        shell_arguments_error();
        return false;
    }
    com_keyword class = key_from_string_chain_single(pars->param->main);

    switch (class)
    {
    case CL_GR:
        shell_compute_gker(themor);
        print_title_box(100, true, stdout, 1, "GR-kernel of the morphism.");
        print_full_green_sub(themor->gker, false, stdout);
        return true;
        break;
    case CL_MOD:
        shell_compute_mker(themor);
        print_title_box(100, true, stdout, 1, "MOD-kernel of the morphism.");
        print_full_green_sub(themor->mker, false, stdout);
        return true;
        break;
    default:
        fprintf(stderr, "Error: kernels are not supported for this class.\n");
        return false;
        break;
    }
    shell_arguments_error();
    return false;
}

bool shell_view_orbits(morphism *morp, com_parameters *pars)
{
    int n = com_nbparams(pars);
    if (n < 1 || n > 2)
    {
        shell_arguments_error();
        return false;
    }
    com_keyword arg1 = key_from_string_chain_single(pars->param->main);
    bool res         = false;
    if (n == 1)
    {
        switch (arg1)
        {
        case CL_DD:
            shell_compute_ddorbs(morp);
            print_title_box(100, true, stdout, 1, "DD-orbits of the morphism.");
            print_all_orbs(morp->ddorbs, "DD", stdout);
            res = true;
            break;
        case CL_MODP:
            shell_compute_mporbs(morp);
            print_title_box(100, true, stdout, 1, "MOD⁺-orbits of the morphism.");
            print_all_orbs(morp->mporbs, "MOD⁺", stdout);
            res = true;
            break;
        case CL_AT:
            shell_compute_atorbs(morp);
            print_title_box(100, true, stdout, 1, "AT-orbits of the morphism.");
            print_all_orbs(morp->atorbs, "AT", stdout);
            res = true;
            break;
        default:
            fprintf(stderr, "Error: orbits are not supported for this class.\n");
            res = false;
            break;
        }
    }
    else if (n == 2)
    {
        com_keyword arg2 = key_from_string_chain_single(pars->next->param->main);
        switch (arg1)
        {
        case OP_BPOL:
            switch (arg2)
            {
            case CL_ST:
                shell_compute_atorbs(morp);
                print_title_box(100, true, stdout, 1, "BPol(ST)-orbits of the morphism.");
                print_all_orbs(morp->atorbs, "BPol(ST)", stdout);
                res = true;
                break;
            case CL_DD:
                shell_compute_bpddorbs(morp);
                print_title_box(100, true, stdout, 1, "BPol(DD)-orbits of the morphism.");
                print_all_orbs(shell_get_bpddorbs(morp), "BPol(DD)", stdout);
                res = true;
                break;
            case CL_MOD:
                shell_compute_bpmorbs(morp);
                print_title_box(100, true, stdout, 1, "BPol(MOD)-orbits of the morphism.");
                print_all_orbs(shell_get_bpmorbs(morp), "BPol(MOD)", stdout);
                res = true;
                break;
            case CL_MODP:
                shell_compute_bpmporbs(morp);
                print_title_box(100, true, stdout, 1, "BPol(MOD⁺)-orbits of the morphism.");
                print_all_orbs(shell_get_bpmporbs(morp), "BPol(MOD⁺)", stdout);
                res = true;
                break;
            case CL_GR:
                shell_compute_bpgorbs(morp);
                print_title_box(100, true, stdout, 1, "BPol(GR)-orbits of the morphism.");
                print_all_orbs(morp->bpgorbs, "BPol(GR)", stdout);
                res = true;
                break;
            default:
                fprintf(stderr, "Error: orbits are not supported for this class.\n");
                res = false;
                break;
            }
            break;
        case OP_TLC:
            switch (arg2)
            {
            case CL_ST:
                shell_compute_atorbs(morp);
                print_title_box(100, true, stdout, 1, "TL(ST)-orbits of the morphism.");
                print_all_orbs(morp->atorbs, "TL(ST)", stdout);
                res = true;
                break;
            case CL_DD:
                shell_compute_bpddorbs(morp);
                print_title_box(100, true, stdout, 1, "TL(DD)-orbits of the morphism.");
                print_all_orbs(shell_get_bpddorbs(morp), "TL(DD)", stdout);
                res = true;
                break;
            case CL_MOD:
                shell_compute_bpmorbs(morp);
                print_title_box(100, true, stdout, 1, "TL(MOD)-orbits of the morphism.");
                print_all_orbs(shell_get_bpmorbs(morp), "TL(MOD)", stdout);
                res = true;
                break;
            case CL_MODP:
                shell_compute_bpmporbs(morp);
                print_title_box(100, true, stdout, 1, "TL(MOD⁺)-orbits of the morphism.");
                print_all_orbs(shell_get_bpmporbs(morp), "TL(MOD⁺)", stdout);
                res = true;
                break;
            case CL_GR:
                shell_compute_bpgorbs(morp);
                print_title_box(100, true, stdout, 1, "TL(GR)-orbits of the morphism.");
                print_all_orbs(morp->bpgorbs, "TL(GR)", stdout);
                res = true;
                break;
            default:
                fprintf(stderr, "Error: orbits are not supported for this class.\n");
                res = false;
                break;
            }
            break;
        default:
            fprintf(stderr, "Error: orbits are not supported for this operator.\n");
            res = false;
            break;
        }
    }
    return res;
}

bool shell_view_oneorbit(morphism *morp, com_parameters *pars)
{
    int n = com_nbparams(pars);
    if (n < 2 || n > 3)
    {
        shell_arguments_error();
        return false;
    }
    com_command **pararray = com_getparamarray(pars);

    // Récupération du numéro de l'idempotent dont on doit afficher l'orbite.
    if (!com_single(pararray[n - 1]))
    {
        shell_arguments_error();
        return false;
    }
    uint i;
    char *word = pararray[n - 1]->main->string;
    uint s;
    if (!cayley_elem_from_string(morp->cayley, word, &s))
    {
        fprintf(stdout, "Error: this word does not correspond to an element of the monoid.\n");
        return false;
    }
    if (!cayley_num_idem(morp->cayley, s, &i))
    {
        fprintf(stdout, "Error: this is not an idempotent element.\n");
        return false;
    }
    char *idemname = cayley_get_name(morp->cayley, s);
    char message[150];

    // Affichage
    bool res         = false;

    com_keyword arg1 = key_from_string_chain_single(pararray[0]->main);

    if (n == 2)
    {
        switch (arg1)
        {
        case CL_DD:
            shell_compute_ddorbs(morp);
            sprintf(message, "DD-orbit of the idempotent %s.", idemname);
            print_title_box(100, true, stdout, 1, message);
            print_one_orb(morp->ddorbs, "DD", i, stdout);
            res = true;
            break;
        case CL_MODP:
            shell_compute_mporbs(morp);
            sprintf(message, "MOD⁺-orbit of the idempotent %s.", idemname);
            print_title_box(100, true, stdout, 1, message);
            print_one_orb(morp->mporbs, "MOD⁺", i, stdout);
            res = true;
            break;
        case CL_AT:
            shell_compute_atorbs(morp);
            sprintf(message, "AT-orbit of the idempotent %s.", idemname);
            print_title_box(100, true, stdout, 1, message);
            print_one_orb(morp->atorbs, "AT", i, stdout);
            res = true;
            break;
        default:
            fprintf(stderr, "Error: orbits are not supported for this class.\n");
            res = false;
            break;
        }
    }
    else if (n == 3)
    {
        com_keyword arg2 = key_from_string_chain_single(pararray[1]->main);
        free(pararray);
        switch (arg1)
        {
        case OP_BPOL:
            switch (arg2)
            {
            case CL_ST:
                shell_compute_atorbs(morp);
                sprintf(message, "BPol(ST)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(morp->atorbs, "BPol(ST)", i, stdout);
                res = true;
                break;
            case CL_DD:
                shell_compute_bpddorbs(morp);
                sprintf(message, "BPol(DD)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(shell_get_bpddorbs(morp), "BPol(DD)", i, stdout);
                res = true;
                break;
            case CL_MOD:
                shell_compute_bpmorbs(morp);
                sprintf(message, "BPol(MOD)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(shell_get_bpmorbs(morp), "BPol(MOD)", i, stdout);
                res = true;
                break;
            case CL_MODP:
                shell_compute_bpmporbs(morp);
                sprintf(message, "BPol(MOD⁺)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(shell_get_bpmporbs(morp), "BPol(MOD⁺)", i, stdout);
                res = true;
                break;
            case CL_GR:
                shell_compute_bpgorbs(morp);
                sprintf(message, "BPol(GR)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(morp->bpgorbs, "BPol(GR)", i, stdout);
                res = true;
                break;
            default:
                fprintf(stderr, "Error: orbits are not supported for this class.\n");
                res = false;
                break;
            }
            break;
        case OP_TLC:
            switch (arg2)
            {
            case CL_ST:
                shell_compute_atorbs(morp);
                sprintf(message, "TL(ST)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(morp->atorbs, "TL(ST)", i, stdout);
                res = true;
                break;
            case CL_DD:
                shell_compute_bpddorbs(morp);
                sprintf(message, "TL(DD)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(shell_get_bpddorbs(morp), "TL(DD)", i, stdout);
                res = true;
                break;
            case CL_MOD:
                shell_compute_bpmorbs(morp);
                sprintf(message, "TL(MOD)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(shell_get_bpmorbs(morp), "TL(MOD)", i, stdout);
                res = true;
                break;
            case CL_MODP:
                shell_compute_bpmporbs(morp);
                sprintf(message, "TL(MOD⁺)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(shell_get_bpmporbs(morp), "TL(MOD⁺)", i, stdout);
                res = true;
                break;
            case CL_GR:
                shell_compute_bpgorbs(morp);
                sprintf(message, "TL(GR)-orbit of the idempotent %s.", idemname);
                print_title_box(100, true, stdout, 1, message);
                print_one_orb(morp->bpgorbs, "TL(GR)", i, stdout);
                res = true;
                break;
            default:
                fprintf(stderr, "Error: orbits are not supported for this class.\n");
                res = false;
                break;
            }
            break;
        default:
            fprintf(stderr, "Error: orbits are not supported for this operator.\n");
            res = false;
            break;
        }
    }

    free(pararray);
    free(idemname);
    return res;
}

bool shell_compute_image(morphism *themor, com_parameters *pars)
{
    if (com_nbparams(pars) != 1 || !com_single(pars->param))
    {
        shell_arguments_error();
        return false;
    }
    cayley_print_image(themor->cayley, pars->param->main->string, stdout);
    return true;
}

bool shell_make_nfa_run(automata *aut, com_parameters *pars)
{
    if (com_nbparams(pars) != 1 || !com_single(pars->param))
    {
        shell_arguments_error();
        return false;
    }
    p_vertices states = nfa_compute_runs(aut->nfa, pars->param->main->string);
    if (states == NULL)
    {
        printf("Error: this is not a valid word for this automaton.\n");
        return false;
    }
    printf("Set of states reached: ");
    if (isempty_vertices(states))
    {
        printf("∅.\n");
    }
    else
    {
        printf("{");
        for (uint i = 0; i < size_vertices(states) - 1; i++)
        {
            nfa_print_name(aut->nfa, lefread_vertices(states, i), stdout);
            printf(",");
        }
        nfa_print_name(aut->nfa, lefread_vertices(states, size_vertices(states) - 1), stdout);
        printf("}.\n");
    }

    // On teste si on a atteint un état final
    uint i = 0;
    uint j = 0;
    while (i < size_vertices(states) && j < size_vertices(aut->nfa->finals))
    {
        if (lefread_vertices(states, i) == lefread_vertices(aut->nfa->finals, j))
        {
            printf("The word is accepted.\n");
            return true;
        }
        else if (lefread_vertices(states, i) < lefread_vertices(aut->nfa->finals, j))
        {
            i++;
        }
        else
        {
            j++;
        }
    }
    printf("The word is rejected.\n");
    return true;
}

/********************************************************/
/* Test de propriétés sur les automates et les monoides */
/********************************************************/

bool shell_prop_comm(com_parameters *pars)
{
    if (com_nbparams(pars) != 1)
    {
        shell_arguments_error();
        return false;
    }

    p_genob mypar = shell_genob_from_command(pars->param);
    if (mypar == NULL)
    {
        fprintf(stdout, "#### Error: wrong parameter.\n");
        return false;
    }
    bool result = false;
    switch (mypar->type)
    {
    case OG_AUTO:
        return dfa_is_comm(mypar->theob, stdout);
        break;
    case OG_MORP:
        return is_comm_mono(mypar->theob, stdout);
        break;
    case OG_SUBM:
        return is_comm_submono(mypar->theob, stdout);
        break;
    // case OG_ORBS:
    // {
    // p_orbits orbs = (p_orbits)mypar->theob;
    // if (orbs->oneonly) {

    // }
    // }
    // break;
    default:
        fprintf(stdout, "#### Error: comutativity can only be tested for automata and monoids.\n");
        break;
    }
    return result;
}
