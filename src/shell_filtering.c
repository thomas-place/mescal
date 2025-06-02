#include "shell_filtering.h"


/**********************************/
/*+ Management of stored objects +*/
/**********************************/

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
    if (objects[j]->aut->obj_nfa->trans->size_graph >= (uint)vals[0] && (vals[1] == -1 || objects[j]->aut->obj_nfa->trans->size_graph <= (uint)vals[1])) {
        return true;
    }
    return false;
}

static void info_minimal(int i) {
    int j = shell_compute_minimal(i);
    char buffer[64];
    sprintf(buffer, "Minimal automaton: %d states", objects[j]->aut->obj_nfa->trans->size_graph);
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
    if (M->nb_idems >= (uint)vals[0] && (vals[1] == -1 || M->nb_idems <= (uint)vals[1])) {
        return true;
    }
    return false;
}

static void info_idems(int i) {
    int j = shell_compute_syntac(i, false);
    char buffer[64];
    morphism* M = objects[j]->mor->obj;
    sprintf(buffer, "Idempotents in the syntactic monoid: %d", M->nb_idems);
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

int shell_delete(com_parameters* pars, const char* str) {
    int i;
    bool saved = false;
    par_type types[] = { PAR_OBJECTV };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    object_free(i);
    return -1;
}

int shell_delete_all(com_parameters* pars, const char* str) {
    if (param_retrieve(pars, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, str) == -2) {
        usage_generic(NULL, 0, PAR_NONE, str);
        return -2;
    }
    object_free_all();
    return -1;
}

static void usage_recursive_init(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<nb_max>, <var1>, ...)\n", str);
    fprintf(stderr, "####         <nb_max> is an integer (maximal index that the user must explicitely define).\n");
    fprintf(stderr, "####         <var1>, ... are the (sub)variable names used for the relations in the recusrive definition.\n");
    fprintf(stderr, "#### Return : a recursive definition.\n");
}
int shell_recursive_init(char* varname, com_parameters* pars, const char* str) {
    if (error_nbpars(str, pars, 2, -1)) {
        usage_recursive_init(str);
        return -2;
    }

    int nb;
    if (com_integer_par(pars, 0, str, &nb) == -2) {
        usage_recursive_init(str);
        return -2;
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
    objects[i]->rec->num = com_nbparams(pars) - 1;
    objects[i]->rec->init = nb + 1;
    objects[i]->rec->full = false;

    for (uchar j = 0; j < objects[i]->rec->num; j++) {
        pars = pars->next;
        objects[i]->rec->names[j] = strdup(pars->param->main->string);
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

int shell_reset(com_parameters* pars, const char* str) {
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATONV };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    nfa_reset_state_names(objects[i]->aut->obj_nfa);
    return -1;
}
