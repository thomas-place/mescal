/*****************************/
/* Gestionnaire de commandes */
/*****************************/

#include "shell_commands.h"
#include "monoid_display.h"

sub_level optimization_level = LV_REG;

/******************************************************************************/
/* Récupération des automates désignés par une commande */
/******************************************************************************/

int com_get_object(int i, string_chain* thechain) {
    // Tant qu'il y a des maillons
    while (thechain != NULL && i != -1) {
        switch (string_to_keyword(thechain->string)) {
        case KY_MINI:
            i = shell_compute_minimal(i);
            break;
        case KY_SYNT:
            i = shell_compute_syntac(i, false);
            break;
        default:
            return -1;
            break;
        }
        // On prend le prochain maillon
        thechain = thechain->next;
    }
    return i;
}

/******************************/
/* Application d'une commande */
/******************************/

void com_setrec_command(char* varname, char* subname, char* par, com_command* thecom) {

    int i = object_get_from_name(varname);
    if (i == -1) {
        shell_error_unknownvar(varname);
        return;
    }
    if (objects[i]->type != RECDEF) {
        shell_error_notrecvar(varname);
        return;
    }

    uchar h = shell_rec_getnum(objects[i]->rec, subname);
    if (h >= objects[i]->rec->num) {
        shell_error_unknownrel(varname, subname);
        return;
    }
    if (strlen(par) == 1 && par[0] == 'i') {
        if (thecom->thetype != CMT_RAW) {
            shell_error_notrecdef(thecom);
            return;
        }
        symbolic_count = objects[i]->rec->num;
        symbolic_names = objects[i]->rec->names;
        regexp* myexp = parse_string_regexp(thecom->main->string);
        symbolic_count = 0;
        symbolic_names = NULL;
        if (myexp == NULL) {
            shell_error_notrecdef(thecom);
            return;
        }
        shell_rec_defadd(i, h, myexp);
        return;
    }

    char* end;
    ushort nb = strtol(par, &end, 10);
    if (*end != '\0') {
        shell_error_syntax();
        return;
    }
    if (nb >= objects[i]->rec->init) {
        shell_error_wrongrecindex(nb, varname);
        return;
    }

    bool save;
    int j = com_apply_command(thecom, NULL, MODE_DEFAULT, &save);
    if (j == -1 || objects[j]->type != REGEXP) {
        shell_error_notregexp(thecom);
        if (save && j != -1) {
            object_free(j);
        }
        return;
    }
    shell_rec_iniadd(i, h, nb, reg_copy(objects[j]->exp));

    if (save) {
        object_free(j);
    }
}

int com_apply_command(com_command* thecom, char* varname, com_mode mode, bool* saved) {


    if (thecom == NULL || thecom->main == NULL) {
        shell_error_syntax();
        return -1;
    }
    if (varname && !check_varname(varname)) {
        shell_error_disallowedvar(varname);
        return -1;
    }

    // Intialisation du Booléen de retour
    if (saved) {
        switch (mode) {
        case MODE_PRINT:
            *saved = false;
            break;
        case MODE_DEFAULT:
        case MODE_COPY:
            *saved = true;
            break;
        default:
            break;
        }
    }

    // On cherche d'abord si la commande commence par un mot-clé
    com_keyword key = string_to_keyword(thecom->main->string);

    // Si ce n'est pas le cas
    if (key == KY_NULL) {

        // Dans ce cas, on ne doit pas avoir de paramètres.

        if (thecom->params) {
            shell_error_syntax();
            return -1;
        }
        // Premier cas: la commmande est un texte à traiter comme une regexp
        if (thecom->thetype == CMT_RAW) {

            regexp* myexp = parse_string_regexp(thecom->main->string);
            if (myexp == NULL) {
                shell_error_notregexp(thecom);
                return -1;
            }
            switch (mode) {
            case MODE_PRINT:
                reg_print(myexp);
                reg_free(myexp);
                return -1;
                break;
            case MODE_DEFAULT:
            case MODE_COPY:
                return object_add_regexp(varname, myexp);
                break;
            default:
                break;
            }
            return -1;
        }

        // Sinon, le premier maillon doit être une variable.
        if (!check_varname(thecom->main->string)) {
            shell_error_syntax();
            return -1;
        }

        // On récupère l'objet désigné par la commande
        int i = object_get_from_name(thecom->main->string);
        if (i == -1) {
            shell_error_unknownvar(thecom->main->string);
            return -1;
        }

        // There are now two cases, depending on the type of variables we are dealing with

        // If the variable stands for an expression, an automaton or a morphism.
        if (objects[i]->type != RECDEF) {
            int j = com_get_object(i, thecom->main->next);
            if (j < 0) {
                shell_error_dispatch(j);
                return j;
            }
            switch (mode) {

            case MODE_DEFAULT:
                if (saved) {
                    *saved = false;
                }
                return j;
                break;
            case MODE_PRINT:
                shell_view_object(objects[j], true);
                return -1;
                break;
            case MODE_COPY:
                return shell_copy_generic(j, varname);
                break;
            default:
                break;
            }
        }
        else {
            // Otherwise, the object is a recursive definition (recdef).

            // If the command consists of a single link, display the summary of the recdef.
            if (com_single(thecom)) {
                if (mode == MODE_PRINT) {
                    shell_view_object(objects[i], true);
                    return -1;
                }
                else {
                    shell_error_copyrec();
                    return -1;
                }
            }

            if (thecom->thetype != CMT_IND || !thecom->main->next || thecom->main->next->next) {
                shell_error_syntax();
                return -1;
            }

            uchar h = shell_rec_getnum(objects[i]->rec, thecom->main->next->string);

            if (h >= objects[i]->rec->num) {
                shell_error_unknownrel(thecom->main->string, thecom->main->next->string);
                return -1;
            }

            if (!objects[i]->rec->full) {
                shell_error_recnotok(thecom->main->string);
                return -1;
            }

            int j = object_add_regexp(varname, shell_rec_getexp(i, thecom->main->next->string, thecom->ind));
            if (j == -1) {
                return -1;
            }

            switch (mode) {

            case MODE_DEFAULT:
                *saved = true;
                return j;
                break;
            case MODE_PRINT:
                shell_view_object(objects[j], true);
                object_free(j);
                return -1;
                break;
            case MODE_COPY:
                return j;
                break;
            default:
                break;
            }
            return j;
        }

        shell_error_syntax();
        return -1;
    }




    // The first link must be the only one and must correspond to a keyword.
    if (thecom->main->next || key == KY_NULL) {
        shell_error_syntax();
        return -1;
    }

    // Commands that don't calculate objects (specific to print mode).
    if (mode == MODE_PRINT) {


        classes cl = command_to_class(thecom);
        if (cl != CL_END) {
            fprintf(stdout, "#### ");
            print_command(thecom, stdout);
            fprintf(stdout, " is equal to the class %s.\n", class_names[cl]);
            print_class_info(cl, stdout);
            return -1;
        }

        switch (key) {
        case KY_LATEX:
            return shell_latex_gen(thecom->params);
            break;
        case KY_SAVE:
            return shell_save_to_file(thecom->params);
            break;
        case KY_SAVESESSION:
            return shell_save_session(thecom->params);
            break;
        case KY_LOADSESSION:
            return shell_load_session(thecom->params);
            break;
        case KY_DELETE:
            return shell_delete(thecom->params);
            break;
        case KY_SEPAR:
            return shell_separation(thecom->params);
            break;
        case KY_RESET:
            return shell_reset(thecom->params);
            break;
        case KY_INVTRANS:
            return shell_invtrans(thecom->params);
            break;
        case KY_RCAY:
            return shell_view_rcayley(thecom->params);
            break;
        case KY_LCAY:
            return shell_view_lcayley(thecom->params);
            break;
        case KY_MULT:
            return shell_view_mormult(thecom->params);
            break;
        case KY_IDEMS:
            return shell_view_idems(thecom->params);
            break;
        case KY_ORDER:
            return shell_view_morder(thecom->params);
            break;
        case KY_RUN:
            return shell_view_nfa_run(thecom->params);
            break;
        case KY_IMAGE:
            return shell_view_mor_image(thecom->params);
            break;
        case KY_GKER:
            return shell_view_gkernel(thecom->params);
            break;
        case KY_MKER:
            return shell_view_mkernel(thecom->params);
            break;
        case KY_AKER:
            return shell_view_akernel(thecom->params);
            break;
        case KY_ORB:
            return shell_view_orbits(thecom->params);
            break;
        case KY_MEMB:
            return shell_membership(thecom->params);
            break;
        case KY_CHIERA:
            return shell_chiera_summary(thecom->params);
            break;
        case KY_NHIERA:
            return shell_neghiera(thecom->params);
            break;
        case KY_FPHIERA:
            return shell_fphiera(thecom->params);
            break;
        case KY_COUNTER:
            return shell_counterfree(thecom->params);
            break;
        case KY_PERMUT:
            return shell_permutation(thecom->params);
            break;
        case KY_EXSEARCH:
            return shell_exsearch(thecom->params);
            break;
        case KY_EXALL:
            return shell_exall(thecom->params);
            break;
        case KY_SORT:
            return shell_sort(thecom->params);
            break;
        case KY_LIST:
            return shell_filter_objects(thecom->params, DUMMY);
            break;
        case KY_CLEAR:
            return shell_delete_all(thecom->params);
            break;
        case KY_TOGGLE:
            return shell_toggle_optimization(thecom->params);
            break;
        case KY_AUTOMATA:
            return shell_filter_objects(thecom->params, AUTOMATON);
            break;
        case KY_MORPHISMS:
            return shell_filter_objects(thecom->params, MORPHISM);
            break;
        case KY_REGEXPS:
            return shell_filter_objects(thecom->params, REGEXP);
            break;
        case KY_RECDEFS:
            return shell_filter_objects(thecom->params, RECDEF);
            break;
        default:
            break;
        }
    }

    int k = -1;
    switch (key) {
    case KY_BMCK:
        k = shell_mccluskey_reg(varname, thecom->params);
        break;
    case KY_THOMPSON:
        k = shell_thompson_nfa(varname, thecom->params);
        break;
    case KY_DYCKTRANS:
        k = shell_dycktrans_nfa(varname, thecom->params);
        break;
    case KY_GLUSHKOV:
        k = shell_glushkov_nfa(varname, thecom->params);
        break;
    case KY_HOPCROFT:
        k = shell_hopcroft_nfa(varname, thecom->params);
        break;
    case KY_MIRROR:
        k = shell_mirror_nfa(varname, thecom->params);
        break;
    case KY_TRIMNFA:
        k = shell_trim_nfa(varname, thecom->params);
        break;
    case KY_KLEENE:
        k = shell_kleene_nfa(varname, thecom->params);
        break;
    case KY_ELIMEPS:
        k = shell_elimeps_nfa(varname, thecom->params);
        break;
    case KY_INTERSEC:
        k = shell_intersect_nfa(varname, thecom->params);
        break;
    case KY_UNION:
        k = shell_union_nfa(varname, thecom->params);
        break;
    case KY_BRZOZO:
        k = shell_brzozowski_nfa(varname, thecom->params);
        break;
    case KY_OPEN:
        k = shell_open_object(varname, thecom->params);
        break;
    case KY_RNFA:
        k = shell_random_nfa(varname, thecom->params);
        break;
    case KY_RDFA:
        k = shell_random_dfa(varname, thecom->params);
        break;
    case KY_RECINIT:
        k = shell_recursive_init(varname, thecom->params);
        break;

    case KY_SAVE:
    case KY_SAVESESSION:
    case KY_LOADSESSION:
    case KY_DELETE:
    case KY_SEPAR:
    case KY_RESET:
    case KY_INVTRANS:
    case KY_SORT:
    case KY_RCAY:
    case KY_LCAY:
    case KY_MULT:
    case KY_IDEMS:
    case KY_ORDER:
    case KY_RUN:
    case KY_IMAGE:
    case KY_GKER:
    case KY_MKER:
    case KY_ORB:
    case KY_MEMB:
    case KY_CHIERA:
    case KY_NHIERA:
    case KY_FPHIERA:
        shell_error_noreturn(thecom);
        return -1;
        break;
    default:
        shell_error_syntax();
        return -1;
        break;
    }

    if (k != -1 && mode == MODE_PRINT) {
        shell_view_object(objects[k], true);
        object_free(k);
        return -1;
    }
    return k;
}

/****************************/
/* Affichage d'informations */
/****************************/

static bool filter_syntactic(int i, int* vals) {
    if (!objects[i] || objects[i]->type == RECDEF) {
        return false;
    }
    int j = shell_compute_syntac(i, false);
    if (objects[j]->mor->obj->r_cayley->size_graph >= (uint)vals[0] && (vals[1] == -1 || objects[j]->mor->obj->r_cayley->size_graph <= (uint)vals[1])) {
        return true;
    }
    return false;
}

static void info_syntactic(int i) {
    int j = shell_compute_syntac(i, false);
    char buffer[64];
    sprintf(buffer, "Syntactic monoid: %d elements", objects[j]->mor->obj->r_cayley->size_graph);
    fprintf(stdout, "%-45s", buffer);
}

static bool params_syntactic(com_parameters* pars, int* vals) {
    if (com_nbparams(pars) == 0) {
        vals[0] = 0;
        vals[1] = -1;
        return true;
    }
    char* end;
    vals[0] = strtol(com_getparam(pars, 0)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_SYNT), 1);
        return false;
    }
    if (com_nbparams(pars) == 1) {
        vals[1] = -1;
        return true;
    }
    vals[1] = strtol(com_getparam(pars, 1)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_SYNT), 2);
        return false;
    }
    return true;
}

static bool filter_minimal(int i, int* vals) {
    if (!objects[i] || objects[i]->type == RECDEF) {
        return false;
    }
    int j = shell_compute_minimal(i);
    if (objects[j]->nfa->trans->size_graph >= (uint)vals[0] && (vals[1] == -1 || objects[j]->nfa->trans->size_graph <= (uint)vals[1])) {
        return true;
    }
    return false;
}

static void info_minimal(int i) {
    int j = shell_compute_minimal(i);
    char buffer[64];
    sprintf(buffer, "Minimal automaton: %d states", objects[j]->nfa->trans->size_graph);
    fprintf(stdout, "%-45s", buffer);
}

static bool params_minimal(com_parameters* pars, int* vals) {
    if (com_nbparams(pars) == 0) {
        vals[0] = 0;
        vals[1] = -1;
        return true;
    }
    char* end;
    vals[0] = strtol(com_getparam(pars, 0)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_MINI), 1);
        return false;
    }
    if (com_nbparams(pars) == 1) {
        vals[1] = -1;
        return true;
    }
    vals[1] = strtol(com_getparam(pars, 1)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_MINI), 2);
        return false;
    }
    return true;
}

static bool filter_idems(int i, int* vals) {
    if (!objects[i] || objects[i]->type == RECDEF) {
        return false;
    }
    int j = shell_compute_syntac(i, false);
    morphism* M = objects[j]->mor->obj;
    if (size_dequeue(M->idem_list) >= (uint)vals[0] && (vals[1] == -1 || size_dequeue(M->idem_list) <= (uint)vals[1])) {
        return true;
    }
    return false;
}

static void info_idems(int i) {
    int j = shell_compute_syntac(i, false);
    char buffer[64];
    morphism* M = objects[j]->mor->obj;
    sprintf(buffer, "Idempotents in the syntactic monoid: %d", size_dequeue(M->idem_list));
    fprintf(stdout, "%-45s", buffer);
}

static bool params_idems(com_parameters* pars, int* vals) {
    if (com_nbparams(pars) == 0) {
        vals[0] = 0;
        vals[1] = -1;
        return true;
    }
    char* end;
    vals[0] = strtol(com_getparam(pars, 0)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_IDEMS), 1);
        return false;
    }
    if (com_nbparams(pars) == 1) {
        vals[1] = -1;
        return true;
    }
    vals[1] = strtol(com_getparam(pars, 1)->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_IDEMS), 2);
        return false;
    }
    return true;
}

int shell_filter_objects(com_parameters* pars, ob_type type) {
    int n = com_nbparams(pars);
    if (type == RECDEF && n > 0) {
        fprintf(stderr, "Error: This command does not take any parameters.\n");
        return -1;
    }

    bool (*filters[n + 1])(int, int*);
    void (*infos[n + 1])(int);
    int vals[n + 1][8];

    for (int i = 0; i < n; i++) {
        if (!com_single_par(com_getparam(pars, i))) {
            shell_error_syntax();
            return -1;
        }
        com_keyword key = string_to_keyword(com_getparam(pars, i)->main->string);
        switch (key) {
        case KY_SYNT:
            filters[i] = filter_syntactic;
            infos[i] = info_syntactic;
            if (!params_syntactic(com_getparam(pars, i)->params, vals[i])) {
                return -1;
            }
            break;
        case KY_MINI:
            filters[i] = filter_minimal;
            infos[i] = info_minimal;
            if (!params_minimal(com_getparam(pars, i)->params, vals[i])) {
                return -1;
            }
            break;
        case KY_IDEMS:
            filters[i] = filter_idems;
            infos[i] = info_idems;
            if (!params_idems(com_getparam(pars, i)->params, vals[i])) {
                return -1;
            }
            break;
        default:
            shell_error_syntax();
            return -1;
            break;
        }
    }

    // Computes the number of user objects
    uint count = 0;
    for (int i = 0; i < nb_objects; i++) {
        bool ok = true;
        for (int h = 0; h < n; h++) {
            if (!filters[h](i, vals[h])) {
                ok = false;
                break;
            }
        }
        if (objects[i] && objects[i]->name && ok) {
            count++;
        }
    }

    if (n == 0) {
        switch (type) {
        case REGEXP:
            printf("#### There are %d regular expressions(s) in memory:\n\n", count);
            break;
        case AUTOMATON:
            printf("#### There are %d automata in memory:\n\n", count);
            break;
        case MORPHISM:
            printf("#### There are %d morphism(s) in memory:\n\n", count);
            break;
        case RECDEF:
            printf("#### There are %d recursive definition(s) in memory:\n\n", count);
            break;
        case DUMMY:
            printf("#### There are %d object(s) in memory:\n\n", count);
            break;
        default:
            break;
        }
    }
    else {
        switch (type) {
        case REGEXP:
            printf("#### There are %d regular expressions(s) in memory with the specified properties:\n\n", count);
            break;
        case AUTOMATON:
            printf("#### There are %d automata in memory with the specified properties:\n\n", count);
            break;
        case MORPHISM:
            printf("#### There are %d morphism(s) in memory with the specified properties:\n\n", count);
            break;
        case RECDEF:
            printf("#### There are %d recursive definition(s) in memory with the specified properties:\n\n", count);
            break;
        case DUMMY:
            printf("#### There are %d object(s) in memory with the specified properties:\n\n", count);
            break;
        default:
            break;
        }
    }
    if (count > 0) {
        count = 1;
    }
    char buffer[128];
    int line[5];
    uchar found = 0;
    for (int i = 0; i < nb_objects; i++) {
        bool ok = objects[i] && objects[i]->name;
        if (ok) {
            for (int h = 0; h < n; h++) {
                if (!filters[h](i, vals[h])) {
                    ok = false;
                    break;
                }
            }
        }
        if (ok) {
            line[found] = i;
            found++;
            if (type == DUMMY) {
                sprintf(buffer, "#### %d (%s): %s", count, object_types_names[objects[i]->type], objects[i]->name);
            }
            else {
                sprintf(buffer, "#### %d: %s", count, objects[i]->name);
            }
            fprintf(stdout, "%-45s", buffer);
            count++;
        }

        if (found == 5) {
            fprintf(stdout, "\n");
            found = 0;
            for (int k = 0; k < n; k++) {
                for (int j = 0; j < 5; j++) {
                    if (infos[k]) {
                        infos[k](line[j]);
                    }
                }
                fprintf(stdout, "\n");
            }
            fprintf(stdout, "\n");
        }
    }

    if (found != 0) {

        fprintf(stdout, "\n");
        for (int k = 0; k < n; k++) {
            for (int j = 0; j < found; j++) {
                if (infos[k]) {
                    infos[k](line[j]);
                }
            }
            fprintf(stdout, "\n");
        }
        fprintf(stdout, "\n");
    }

    return -1;
}

/******************************/
/* Calculs de nouveaux objets */
/******************************/

// Ajout d'un nouvel objet en copiant un objet déjà existant dans la table
int shell_copy_generic(int i, char* newname) {
    if (i == -1) {
        return -1;
    }
    switch (objects[i]->type) {
    case AUTOMATON:
        return object_add_automaton(newname, nfa_copy(objects[i]->nfa));
        break;
    case REGEXP:
        return object_add_regexp(newname, reg_copy(objects[i]->exp));
    default:
        return -1;
        break;
    }
}

// Calcule un nouveau NFA avec la méthode de Glushkov à partir d'un langage.
int shell_glushkov_nfa(char* varname, com_parameters* pars) {

    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_GLUSHKOV), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != REGEXP) {
        shell_error_exppar(keywordtostring(KY_GLUSHKOV), 1);
        return -1;
    }
    nfa* automaton = reg_glushkov(objects[i]->exp);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

// Calcule le miroir d'un nfa à partir d'un langage.
int shell_mirror_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_MIRROR), 1);
        return -1;
    }

    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1 || objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_MIRROR), 1);
        return -1;
    }
    nfa* automaton = nfa_mirror(objects[i]->nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

int shell_hopcroft_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_HOPCROFT), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_HOPCROFT), 1);
        return -1;
    }
    nfa* automaton = nfa_hopcroft(objects[i]->nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

int shell_mccluskey_reg(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_BMCK), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_BMCK), 1);
        return -1;
    }
    DEBUG("Computing a regular expression from an automaton");
    regexp* exp = nfa_mccluskey(objects[i]->nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_regexp(varname, exp);
}

// Calcule un nouveau NFA avec la méthode de thompson à partir d'un langage.
int shell_thompson_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_BMCK), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != REGEXP) {
        shell_error_exppar(keywordtostring(KY_THOMPSON), 1);
        return -1;
    }
    nfa* automaton = reg_thompson(objects[i]->exp);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

// Union de deux NFAs
int shell_union_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 2) {
        shell_error_nbparams(keywordtostring(KY_UNION), 2);
        return -1;
    }
    bool saved1;
    bool saved2;
    int i1 = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved1);
    int i2 = com_apply_command(pars->next->param, NULL, MODE_DEFAULT, &saved2);
    if (i1 == -1 || i2 == -1) {
        return -1;
    }
    if (objects[i1]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_UNION), 1);
        return -1;
    }
    if (objects[i2]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_UNION), 2);
        return -1;
    }
    nfa* automaton = nfa_union(objects[i1]->nfa, objects[i2]->nfa);
    if (saved1) {
        object_free(i1);
    }
    if (saved2) {
        object_free(i2);
    }
    return object_add_automaton(varname, automaton);
}

// Concatène deux NFAs
int shell_concat_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 2) {
        shell_error_nbparams(keywordtostring(KY_CONCAT), 2);
        return -1;
    }
    bool saved1;
    bool saved2;
    int i1 = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved1);
    int i2 = com_apply_command(pars->next->param, NULL, MODE_DEFAULT, &saved2);
    if (i1 == -1 || i2 == -1) {
        return -1;
    }
    if (objects[i1]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_CONCAT), 1);
        return -1;
    }
    if (objects[i2]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_CONCAT), 2);
        return -1;
    }
    nfa* automaton = nfa_concat(objects[i1]->nfa, objects[i2]->nfa);
    if (saved1) {
        object_free(i1);
    }
    if (saved2) {
        object_free(i2);
    }
    return object_add_automaton(varname, automaton);
}

// Étoile de Kleene sur un NFA
int shell_kleene_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_KLEENE), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_KLEENE), 1);
        return -1;
    }
    nfa* automaton = nfa_star(objects[i]->nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

// Calcule un nouveau NFA en éliminant les transitions epsilon
int shell_elimeps_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_ELIMEPS), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_ELIMEPS), 1);
        return -1;
    }
    nfa* automaton = nfa_elimeps(objects[i]->nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

// Calcule un nouveau NFA en supprimant tous les états non-accessibles ou non
// co-accessibles
int shell_trim_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_TRIMNFA), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_TRIMNFA), 1);
        return -1;
    }

    nfa* automaton = nfa_trim(objects[i]->nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

// Calcule un nouveau NFA en réalisant l'intersection de deux NFAs existants
int shell_intersect_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 2) {
        shell_error_nbparams(keywordtostring(KY_INTERSEC), 2);
        return -1;
    }
    bool saved1;
    bool saved2;
    int i1 = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved1);
    int i2 = com_apply_command(pars->next->param, NULL, MODE_DEFAULT, &saved2);
    if (i1 == -1 || i2 == -1) {
        return -1;
    }
    if (objects[i1]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_INTERSEC), 1);
        return -1;
    }
    if (objects[i2]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_INTERSEC), 2);
        return -1;
    }
    nfa* automaton = nfa_intersect(objects[i1]->nfa, objects[i2]->nfa, true);
    if (saved1) {
        object_free(i1);
    }
    if (saved2) {
        object_free(i2);
    }
    return object_add_automaton(varname, automaton);
}

// Computes a minimal automaton with the Brzozowski method
int shell_brzozowski_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_BRZOZO), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_BRZOZO), 1);
        return -1;
    }
    nfa* automaton = nfa_brzozowski(objects[i]->nfa);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

// Extension of a NFA by adding Dyck transitions
int shell_dycktrans_nfa(char* varname, com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_DYCKTRANS), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_DYCKTRANS), 1);
        return -1;
    }
    parti* SCCS = nfa_inv_ext(objects[i]->nfa);
    parti* FOLD = nfa_stal_fold(objects[i]->nfa, SCCS);
    delete_parti(SCCS);
    nfa* automaton = nfa_dyck_ext(objects[i]->nfa, FOLD);
    delete_parti(FOLD);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton(varname, automaton);
}

// Computes a random NFA
int shell_random_nfa(char* varname, com_parameters* params) {
    if (com_nbparams(params) != 3) {
        shell_error_nbparams(keywordtostring(KY_RNFA), 3);
        return -1;
    }
    com_command* arg1 = params->param;
    com_command* arg2 = params->next->param;
    com_command* arg3 = params->next->next->param;
    if (!com_single(arg1)) {
        shell_error_numpar(keywordtostring(KY_RNFA), 1);
        return -1;
    }

    if (!com_single(arg2)) {
        shell_error_numpar(keywordtostring(KY_RNFA), 2);
        return -1;
    }

    if (!com_single(arg3)) {
        shell_error_numpar(keywordtostring(KY_RNFA), 3);
        return -1;
    }

    char* end;
    int nb1 = strtol(arg1->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_RNFA), 1);
        return -1;
    }
    int nb2 = strtol(arg2->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_RNFA), 2);
        return -1;
    }
    int nb3 = strtol(arg3->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_RNFA), 3);
        return -1;
    }

    return object_add_automaton(varname, nfa_random(nb1, nb2, nb3));
}

int shell_random_dfa(char* varname, com_parameters* params) {
    if (com_nbparams(params) != 3) {
        shell_error_nbparams(keywordtostring(KY_RDFA), 3);
        return -1;
    }
    com_command* arg1 = params->param;
    com_command* arg2 = params->next->param;
    com_command* arg3 = params->next->next->param;
    if (!com_single(arg1)) {
        shell_error_numpar(keywordtostring(KY_RDFA), 1);
        return -1;
    }

    if (!com_single(arg2)) {
        shell_error_numpar(keywordtostring(KY_RDFA), 2);
        return -1;
    }

    if (!com_single(arg3)) {
        shell_error_numpar(keywordtostring(KY_RDFA), 3);
        return -1;
    }

    char* end;
    int nb1 = strtol(arg1->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_RDFA), 1);
        return -1;
    }
    int nb2 = strtol(arg2->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_RDFA), 2);
        return -1;
    }
    int nb3 = strtol(arg3->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_RDFA), 3);
        return -1;
    }

    return object_add_automaton(varname, dfa_random(nb1, nb2, nb3));
}

// Latex génération
int shell_latex_gen(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_LATEX), 1);
        return -1;
    }

    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }

    if (objects[i]->type == AUTOMATON) {
        latex_print_automaton(objects[i]->nfa, stdout);
        return -1;
    }

    if (objects[i]->type == MORPHISM) {
        latex_print_cayley(objects[i]->mor->obj, stdout);
        return -1;
    }
    if (saved) {
        object_free(i);
    }
    return -1;
}


int shell_recursive_init(char* varname, com_parameters* params) {
    if (com_nbparams(params) < 2) {
        shell_error_leastparams(keywordtostring(KY_RECDEF), 2);
        return -1;
    }

    com_command* arg = params->param;

    if (!com_single(arg)) {
        shell_error_numpar(keywordtostring(KY_RECDEF), 1);
        return -1;
    }

    char* end;
    ushort nb = strtol(arg->main->string, &end, 10);
    if (*end != '\0') {
        shell_error_numpar(keywordtostring(KY_RECDEF), 1);
        return -1;
    }

    int i = object_get_from_name(varname);
    if (i != -1) {
        object_free(i);
    }

    grow_objects_array();

    i = nb_objects++;

    if (objects[i]) {
        shell_error_full();
        return -1;
    }

    objects[i] = object_init(varname);
    objects[i]->type = RECDEF;
    MALLOC(objects[i]->rec, 1);
    objects[i]->rec->num = com_nbparams(params) - 1;
    objects[i]->rec->init = nb + 1;
    objects[i]->rec->full = false;

    for (uchar j = 0; j < objects[i]->rec->num; j++) {
        params = params->next;
        objects[i]->rec->names[j] = strdup(params->param->main->string);
        objects[i]->rec->def[j] = NULL;
        MALLOC(objects[i]->rec->regexps[j], nb);
        for (ushort x = 0; x < nb + 1; x++) {
            objects[i]->rec->regexps[j][x] = NULL;
        }
    }

    fprintf(stdout, "\n#### Initialization of the recursive definition %s.\n", varname);
    shell_rec_display(objects[i]->rec, stdout);

    return i;
}

// Save a single object to a file
int shell_save_to_file(com_parameters* pars) {
    if (com_nbparams(pars) != 2) {
        shell_error_nbparams(keywordtostring(KY_SAVE), 2);
        return -1;
    }

    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (!com_israw(pars->next->param)) {
        shell_error_filepar(keywordtostring(KY_SAVE), 2);
        return -1;
    }
    char* filename = pars->next->param->main->string;
    fprintf(stdout, "saving %s in the file: \"%s\".\n", pars->param->main->string, filename);
    files_save_object(objects[i], filename);
    if (saved) {
        object_free(i);
    }
    return -1;
}

// Save the current session in a file
int shell_save_session(com_parameters* params) {
    if (com_nbparams(params) != 1) {
        shell_error_nbparams(keywordtostring(KY_SAVESESSION), 1);
        return -1;
    }
    if (!com_israw(params->param)) {
        shell_error_filepar(keywordtostring(KY_SAVESESSION), 1);
        return -1;
    }
    char* filename = params->param->main->string;
    printf("saving the session in the file: \"%s\".\n", filename);
    files_save_session(filename);
    return -1;
}

// Loading a session from a file
int shell_load_session(com_parameters* params) {
    if (com_nbparams(params) != 1) {
        shell_error_nbparams(keywordtostring(KY_LOADSESSION), 1);
        return -1;
    }
    if (!com_israw(params->param)) {
        shell_error_filepar(keywordtostring(KY_LOADSESSION), 1);
        return -1;
    }
    char* filename = params->param->main->string;
    printf("loading the session saved in the file: \"%s\".\n", filename);
    files_load_session(filename);
    return -1;
}

// Deleting an object
int shell_delete(com_parameters* params) {
    if (com_nbparams(params) != 1) {
        shell_error_nbparams(keywordtostring(KY_DELETE), 1);
        return -1;
    }
    if (!com_single(params->param) || !check_varname(params->param->main->string)) {
        shell_error_varpar(keywordtostring(KY_DELETE), 1);
        return -1;
    }
    int i = object_get_from_name(params->param->main->string);
    if (i == -1 || !objects[i]) {
        shell_error_unknownvar(params->param->main->string);
        return -1;
    }
    object_free(i);
    return -1;
}

// Deletion of all objects
int shell_delete_all(com_parameters* params) {
    if (com_nbparams(params) != 0) {
        shell_error_nbparams(keywordtostring(KY_CLEAR), 0);
        return -1;
    }
    object_free_all();
    return -1;
}



// Erases the names of the states of an automaton
int shell_reset(com_parameters* params) {
    if (com_nbparams(params) != 1) {
        shell_error_nbparams(keywordtostring(KY_RESET), 1);
        return -1;
    }
    if (!com_single(params->param) || !check_varname(params->param->main->string)) {
        shell_error_varpar(keywordtostring(KY_RESET), 1);
        return -1;
    }
    int i = object_get_from_name(params->param->main->string);
    if (i == -1 || !objects[i]) {
        shell_error_unknownvar(params->param->main->string);
        return -1;
    }

    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_RESET), 1);
        return -1;
    }
    nfa_reset_state_names(objects[i]->nfa);
    return -1;
}

// Calcul des transitions inverses dans un NFA
int shell_invtrans(com_parameters* params) {
    if (com_nbparams(params) != 1) {
        shell_error_nbparams(keywordtostring(KY_INVTRANS), 1);
        return -1;
    }
    if (!com_single(params->param) || !check_varname(params->param->main->string)) {
        shell_error_varpar(keywordtostring(KY_INVTRANS), 1);
        return -1;
    }
    int i = object_get_from_name(params->param->main->string);
    if (i == -1 || !objects[i]) {
        shell_error_unknownvar(params->param->main->string);
        return -1;
    }

    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_INVTRANS), 1);
        return false;
    }
    parti* par = nfa_inv_ext(objects[i]->nfa);
    delete_parti(par);
    return true;
}

// Ouverture d'un objet
bool shell_open_object(char* varname, com_parameters* params) {
    if (com_nbparams(params) != 1) {
        shell_error_nbparams(keywordtostring(KY_OPEN), 1);
        return -1;
    }
    if (params->param->thetype != CMT_RAW) {
        shell_error_filepar(keywordtostring(KY_OPEN), 1);
        return false;
    }
    files_read_object(params->param->main->string, varname);
    return true;
}

int shell_toggle_optimization(com_parameters* params) {
    if (com_nbparams(params) != 0) {
        shell_error_nbparams(keywordtostring(KY_TOGGLE), 0);
        return -1;
    }
    if (optimization_level == LV_FULL) {
        optimization_level = LV_REG;
        fprintf(stdout, "#### Optimization mode for kernels and orbits enabled.\n");
    }
    else {
        optimization_level = LV_FULL;
        fprintf(stdout, "#### Optimization mode for kernels and orbits disabled.\n");
    }
    return -1;
}

int shell_counterfree(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_COUNTER), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_COUNTER), 1);
        return -1;
    }

    int error = 0;

    is_counterfree_dfa(objects[i]->nfa, &error, stdout);
    if (error < 0) {
        fprintf(stdout, "#### Error received while testing counter-freeness\n");
    }
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_permutation(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_PERMUT), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_PERMUT), 1);
        return -1;
    }
    is_permutation_dfa(objects[i]->nfa, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

/********************************************************************/
/* Affichage - fonctions appellées par le gestionnaire de commandes */
/********************************************************************/

bool shell_view_object(object* ob, bool title) {
    if (!ob) {
        return false;
    }
    switch (ob->type) {
    case REGEXP:
        if (title) {
            print_title_box(100, true, stdout, 1, "Regular expression");
        }
        reg_print(ob->exp);
        return true;
        break;
    case AUTOMATON:
        if (title) {
            print_title_box(100, true, stdout, 1, "Automaton");
        }
        view_nfa(ob->nfa);
        return true;
        break;
    case MORPHISM:
        if (title) {
            print_title_box(100, true, stdout, 1, "Morphism");
        }
        view_morphism(ob->mor->obj, stdout);
        /*  {
             dequeue* test = compute_strict_minidems(ob->mor->obj, ob->mor->green);
             for (uint i = 0; i < size_dequeue(test);i++) {
                 mor_fprint_name_utf8(ob->mor->obj, lefread_dequeue(test, i), stdout);
                 printf("\n");
             }
         } */
        return true;
        break;
    case RECDEF:
        if (title) {
            print_title_box(100, true, stdout, 1, "Recursive definition of regular expressions");
            shell_rec_display(ob->rec, stdout);
        }
        return true;
        break;

    default:
        return false;
        break;
    }
}

int shell_sort(com_parameters* pars) {
    if (com_nbparams(pars) > 1) {
        shell_error_rangeparams(keywordtostring(KY_SORT), 0, 1);
        return -1;
    }

    if (com_nbparams(pars) == 0) {
        object_sort_array(object_compare);
        return -1;
    }

    if (!com_single(pars->param)) {
        shell_error_invalidpar(keywordtostring(KY_SORT), pars->param);
        return -1;
    }

    switch (string_to_keyword(pars->param->main->string)) {
    case KY_MINI:
        object_sort_array(object_compare_mini);
        break;
    case KY_SYNT:
        object_sort_array(object_compare_synt);
        break;
    default:
        shell_error_invalidpar(keywordtostring(KY_SORT), pars->param);
        break;
    }
    return -1;
}

int shell_view_rcayley(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_RCAY), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_RCAY), 1);
        return -1;
    }
    print_title_box(100, true, stdout, 1, "Right Cayley graph of the morphism");
    view_cayley(objects[i]->mor->obj);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_lcayley(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_LCAY), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_LCAY), 1);
        return -1;
    }
    print_title_box(100, true, stdout, 1, "Left Cayley graph of the morphism");
    view_left_cayley(objects[i]->mor->obj);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_mormult(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_MULT), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_MULT), 1);
        return -1;
    }
    mor_compute_mult(objects[i]->mor->obj);
    print_title_box(100, true, stdout, 1, "Multiplication table of the morphism");
    mor_mult_print(objects[i]->mor->obj, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_morder(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_ORDER), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_ORDER), 1);
        return -1;
    }
    mor_compute_order(objects[i]->mor->obj);
    print_title_box(100, true, stdout, 1, "Ordering of the morphism.");
    mor_print_order(objects[i]->mor->obj, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_idems(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_IDEMS), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_IDEMS), 1);
        return -1;
    }
    mor_print_idems(objects[i]->mor->obj, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_mkernel(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_MKER), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_MKER), 1);
        return -1;
    }
    shell_view_object(objects[i], true);

    print_title_box(100, true, stdout, 1, "MOD-kernel of the morphism.");
    print_full_subsemi(shell_compute_ker(i, KER_MOD, optimization_level), stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_akernel(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_AKER), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_AKER), 1);
        return -1;
    }
    shell_view_object(objects[i], true);

    print_title_box(100, true, stdout, 1, "AMT-kernel of the morphism.");
    //shell_compute_ker(i, KER_AMT, optimization_level);
    print_full_subsemi(shell_compute_ker(i, KER_AMT, optimization_level), stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_gkernel(com_parameters* pars) {
    if (com_nbparams(pars) != 1) {
        shell_error_nbparams(keywordtostring(KY_GKER), 1);
        return -1;
    }
    bool saved;
    int i = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_GKER), 1);
        return -1;
    }

    shell_view_object(objects[i], true);

    print_title_box(100, true, stdout, 1, "GR-kernel of the morphism.");
    print_full_subsemi(shell_compute_ker(i, KER_GR, optimization_level), stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}


static subsemi* shell_compute_orbit_aux(int i, uint e, orbits_type thetype) {
    switch (thetype)
    {
    case ORB_DD:
        return compute_one_ddorb(objects[i]->mor->obj, e);
        break;
    case ORB_MODP:
        return compute_one_gplusorb(shell_compute_ker(i, KER_MOD, optimization_level), e);
        break;
    case ORB_AMTP:
        return compute_one_gplusorb(shell_compute_ker(i, KER_AMT, optimization_level), e);
        break;
    case ORB_GRP:
        return compute_one_gplusorb(shell_compute_ker(i, KER_GR, optimization_level), e);
        break;
    case ORB_PT:
        return compute_one_ptorb(objects[i]->mor->obj, e);
        break;
    case ORB_BPMOD:
        return compute_one_bpgorb(shell_compute_ker(i, KER_MOD, LV_REG), e, optimization_level);
        break;
    case ORB_BPAMT:
        return compute_one_bpgorb(shell_compute_ker(i, KER_AMT, LV_REG), e, optimization_level);
        break;
    case ORB_BPGR:
        return compute_one_bpgorb(shell_compute_ker(i, KER_GR, LV_REG), e, optimization_level);
        break;
    case ORB_BPDD:
        return compute_one_bpgplusorb(shell_compute_orbits(i, ORB_DD, LV_REG), e, optimization_level);
        break;
    case ORB_BPMODP:
        return compute_one_bpgplusorb(shell_compute_orbits(i, ORB_MODP, LV_REG), e, optimization_level);
        break;
    case ORB_BPAMTP:
        return compute_one_bpgplusorb(shell_compute_orbits(i, ORB_AMTP, LV_REG), e, optimization_level);
        break;
    case ORB_BPGRP:
        return compute_one_bpgplusorb(shell_compute_orbits(i, ORB_GRP, LV_REG), e, optimization_level);
        break;
    default:
        break;
    }
    return NULL;
}

int shell_view_orbits(com_parameters* pars) {
    int n = com_nbparams(pars);
    if (n < 2 || n > 3) {
        shell_error_rangeparams(keywordtostring(KY_ORB), 2, 3);
        return -1;
    }

    orbits_type thetype;
    classes cl = command_to_class(com_getparam(pars, 0));

    if (cl == CL_END) {
        fprintf(stdout, "#### The class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, " is either unknown or unsupported.\n\n");
        return false;
    }

    char cl_name[32];

    switch (cl) {
    case CL_DD:
        sprintf(cl_name, "DD");
        thetype = ORB_DD;
        break;
    case CL_MODP:
        sprintf(cl_name, "MOD⁺");
        thetype = ORB_MODP;
        break;
    case CL_AMTP:
        sprintf(cl_name, "AMT⁺");
        thetype = ORB_AMTP;
        break;
    case CL_GRP:
        sprintf(cl_name, "GR⁺");
        thetype = ORB_GRP;
        break;
    case CL_AT:
        sprintf(cl_name, "AT");
        thetype = ORB_PT;
        break;
    case CL_ATT:
        sprintf(cl_name, "ATT");
        thetype = ORB_PT;
        break;
    case CL_UL:
        sprintf(cl_name, "UL");
        thetype = ORB_PT;
        break;
    case CL_FLST:
        sprintf(cl_name, "FL(ST)");
        thetype = ORB_PT;
        break;
    case CL_PLST:
        sprintf(cl_name, "PL(ST)");
        thetype = ORB_PT;
        break;
    case CL_PT:
        sprintf(cl_name, "PT");
        thetype = ORB_PT;
        break;
    case CL_LT:
        sprintf(cl_name, "LT");
        thetype = ORB_BPDD;
        break;
    case CL_LTT:
        sprintf(cl_name, "LTT");
        thetype = ORB_BPDD;
        break;
    case CL_TLDD:
        sprintf(cl_name, "TL(DD)");
        thetype = ORB_BPDD;
        break;
    case CL_FLDD:
        sprintf(cl_name, "FL(DD)");
        thetype = ORB_BPDD;
        break;
    case CL_PLDD:
        sprintf(cl_name, "PL(DD)");
        thetype = ORB_BPDD;
        break;
    case CL_BPOLDD:
        sprintf(cl_name, "BPol(DD)");
        thetype = ORB_BPDD;
        break;
    case CL_TLMOD:
        sprintf(cl_name, "TL(MOD)");
        thetype = ORB_BPMOD;
        break;
    case CL_FLMOD:
        sprintf(cl_name, "FL(MOD)");
        thetype = ORB_BPMOD;
        break;
    case CL_PLMOD:
        sprintf(cl_name, "PL(MOD)");
        thetype = ORB_BPMOD;
        break;
    case CL_BPOLMOD:
        sprintf(cl_name, "BPol(MOD)");
        thetype = ORB_BPMOD;
        break;
    case CL_TLAMT:
        sprintf(cl_name, "TL(AMT)");
        thetype = ORB_BPAMT;
        break;
    case CL_FLAMT:
        sprintf(cl_name, "FL(AMT)");
        thetype = ORB_BPAMT;
        break;
    case CL_PLAMT:
        sprintf(cl_name, "PL(AMT)");
        thetype = ORB_BPAMT;
        break;
    case CL_BPOLAMT:
        sprintf(cl_name, "BPol(AMT)");
        thetype = ORB_BPAMT;
        break;
    case CL_TLMODP:
        sprintf(cl_name, "TL(MOD⁺)");
        thetype = ORB_BPMODP;
        break;
    case CL_FLMODP:
        sprintf(cl_name, "FL(MOD⁺)");
        thetype = ORB_BPMODP;
        break;
    case CL_PLMODP:
        sprintf(cl_name, "PL(MOD⁺)");
        thetype = ORB_BPMODP;
        break;
    case CL_BPOLMODP:
        sprintf(cl_name, "BPol(MOD⁺)");
        thetype = ORB_BPMODP;
        break;
    case CL_TLAMTP:
        sprintf(cl_name, "TL(AMT⁺)");
        thetype = ORB_BPAMTP;
        break;
    case CL_FLAMTP:
        sprintf(cl_name, "FL(AMT⁺)");
        thetype = ORB_BPAMTP;
        break;
    case CL_PLAMTP:
        sprintf(cl_name, "PL(AMT⁺)");
        thetype = ORB_BPAMTP;
        break;
    case CL_BPOLAMTP:
        sprintf(cl_name, "BPol(AMT⁺)");
        thetype = ORB_BPAMTP;
        break;
    case CL_TLGR:
        sprintf(cl_name, "TL(GR)");
        thetype = ORB_BPGR;
        break;
    case CL_FLGR:
        sprintf(cl_name, "FL(GR)");
        thetype = ORB_BPGR;
        break;
    case CL_PLGR:
        sprintf(cl_name, "PL(GR)");
        thetype = ORB_BPGR;
        break;
    case CL_BPOLGR:
        sprintf(cl_name, "BPol(GR)");
        thetype = ORB_BPGR;
        break;
    case CL_TLGRP:
        sprintf(cl_name, "TL(GR⁺)");
        thetype = ORB_BPGRP;
        break;
    case CL_FLGRP:
        sprintf(cl_name, "FL(GR⁺)");
        thetype = ORB_BPGRP;
        break;
    case CL_PLGRP:
        sprintf(cl_name, "PL(GR⁺)");
        thetype = ORB_BPGRP;
        break;
    case CL_BPOLGRP:
        sprintf(cl_name, "BPol(GR⁺)");
        thetype = ORB_BPGRP;
        break;
    default:
        return -1;
        break;
    }

    // Récupération du morphisme.
    bool saved;
    int i = com_apply_command(pars->next->param, NULL, MODE_DEFAULT, &saved);
    if (i == -1) {
        return -1;
    }
    if (objects[i]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_ORB), 2);
        return false;
    }
    //shell_view_object(objects[i], true);

    char title[64];
    sprintf(title, "%s-orbits of the morphism", cl_name);
    print_title_box(100, true, stdout, 1, title);

    if (n == 2) {
        print_all_orbs(shell_compute_orbits(i, thetype, optimization_level), cl_name, stdout);
    }
    else {
        regexp* myexp = parse_string_regexp(pars->next->next->param->main->string);
        if (myexp->op != WORD) {
            fprintf(stderr, "Error: Parameter 3 of the command \"%s\" has to be an idempotent of the monoid.\n", keywordtostring(KY_ORB));
            return -1;
        }
        uint e = mor_compute_image(objects[i]->mor->obj, myexp->word);
        reg_free(myexp);
        if (e >= objects[i]->mor->obj->r_cayley->size_graph || !objects[i]->mor->obj->idem_array[e]) {
            fprintf(stderr, "Error: Parameter 3 of the command \"%s\" has to be an idempotent of the monoid.\n", keywordtostring(KY_ORB));
            return false;
        }
        subsemi* theorbit = shell_compute_orbit_aux(i, e, thetype);

        fprintf(stdout, "#### %s-orbit of the idempotent ", cl_name);
        mor_fprint_name_utf8(objects[i]->mor->obj, e, stdout);
        printf(".\n");
        print_full_subsemi(theorbit, stdout);

        delete_subsemi(theorbit);




        //print_one_orb(f(i, ORB_ELEM, j), j, stdout);
    }

    if (saved) {
        object_free(i);
    }
    return true;
}

int shell_view_nfa_run(com_parameters* pars) {
    if (com_nbparams(pars) != 2 || pars->next->param->thetype != CMT_RAW) {
        shell_error_nbparams(keywordtostring(KY_RUN), 2);
        return -1;
    }
    // Calcul de l'automate
    bool saved;
    int k = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (k == -1 || objects[k]->type != AUTOMATON) {
        shell_error_autopar(keywordtostring(KY_RUN), 1);
        return -1;
    }

    shell_view_object(objects[k], true);
    nfa* A = objects[k]->nfa;

    fprintf(stdout, "Running the word %s in this automaton.\n", pars->next->param->main->string);

    regexp* myexp = parse_string_regexp(pars->next->param->main->string);
    if (myexp->op != WORD) {
        shell_error_wordpar(keywordtostring(KY_RUN), 2);
        return -1;
    }

    dequeue* states = nfa_compute_runs(A, myexp->word);

    reg_free(myexp);

    if (states == NULL) {
        fprintf(stderr, "Error: This is not a valid word for this automaton.\n");
        return -1;
    }
    fprintf(stdout, "Set of states reached: ");
    if (isempty_dequeue(states)) {
        printf("∅.\n");
    }
    else {
        printf("{");
        for (uint i = 0; i < size_dequeue(states) - 1; i++) {
            nfa_print_state(A, lefread_dequeue(states, i), stdout);
            printf(",");
        }
        nfa_print_state(A, lefread_dequeue(states, size_dequeue(states) - 1), stdout);
        printf("}.\n");
    }

    // On teste si on a atteint un état final
    uint i = 0;
    uint j = 0;
    while (i < size_dequeue(states) && j < size_dequeue(A->finals)) {
        if (lefread_dequeue(states, i) == lefread_dequeue(A->finals, j)) {
            printf("The word is accepted.\n");
            return -1;
        }
        else if (lefread_dequeue(states, i) < lefread_dequeue(A->finals, j)) {
            i++;
        }
        else {
            j++;
        }
    }
    printf("The word is rejected.\n");
    if (saved) {
        object_free(k);
    }
    return -1;
}

int shell_view_mor_image(com_parameters* pars) {
    if (com_nbparams(pars) != 2 || pars->next->param->thetype != CMT_RAW) {
        shell_error_nbparams(keywordtostring(KY_IMAGE), 1);
        return -1;
    }
    // Calcul de l'automate
    bool saved;
    int k = com_apply_command(pars->param, NULL, MODE_DEFAULT, &saved);
    if (k == -1 || objects[k]->type != MORPHISM) {
        shell_error_morpar(keywordtostring(KY_IMAGE), 1);
        return -1;
    }

    shell_view_object(objects[k], true);
    morphism* M = objects[k]->mor->obj;

    fprintf(stdout, "Computing the image of the word %s by this morphism.\n", pars->next->param->main->string);

    regexp* myexp = parse_string_regexp(pars->next->param->main->string);
    if (myexp->op != WORD) {
        shell_error_wordpar(keywordtostring(KY_IMAGE), 2);
        return -1;
    }

    uint s = mor_compute_image(M, myexp->word);

    reg_free(myexp);
    if (s >= M->r_cayley->size_graph) {
        fprintf(stderr, "Error: This is not a valid word for this morphism.\n");
        return -1;
    }
    fprintf(stdout, "Image: ");
    mor_fprint_name_utf8(M, s, stdout);

    // On teste si ce mot est dans le langage.
    if (M->accept_array[s]) {
        fprintf(stdout, "\nThe word belongs to the recognized language.\n");
    }
    else {
        fprintf(stdout, "\nThe word does not belong to the recognized language.\n");
    }
    if (saved) {
        object_free(k);
    }
    return -1;
}

int shell_jep(void) {
    fprintf(stdout, "Please enter the number of states: ");
    getchar();
    return -1;
}
