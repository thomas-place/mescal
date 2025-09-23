#include "shell_filtering.h"
#include "shell_commands.h"

/*************************************/
/*+ Extraction of a numerical value +*/
/*************************************/

uint shell_extract_sizesynt(int i) {
    if (objects[i].type == RECDEF) {
        return UINT_MAX;
    }
    int j = shell_compute_syntac(i, false);
    if (j == -1) {
        return UINT_MAX;
    }
    return objects[j].mor->obj->r_cayley->size_graph;
}

uint shell_extract_sizemini(int i) {
    if (objects[i].type == RECDEF) {
        return UINT_MAX;
    }
    int j = shell_compute_minimal(i);
    if (j == -1) {
        return UINT_MAX;
    }
    return objects[j].obj_dfa->trans->size_graph;
}

uint shell_extract_idemnum(int i) {
    if (objects[i].type == RECDEF) {
        return UINT_MAX;
    }
    int j = shell_compute_syntac(i, false);
    if (j == -1) {
        return UINT_MAX;
    }
    return objects[j].mor->obj->nb_idems;
}

static uint shell_extract_greennum(int i, green_relation r) {
    if (objects[i].type == RECDEF) {
        return UINT_MAX;
    }
    int j = shell_compute_syntac(i, false);
    if (j == -1) {
        return UINT_MAX;
    }
    parti* P = grel_to_parti(objects[j].mor->obj->rels, r);
    return P->size_par;
}


uint shell_extract_rnum(int i) {
    return shell_extract_greennum(i, R_GREEN);
}

uint shell_extract_lnum(int i) {
    return shell_extract_greennum(i, L_GREEN);
}

uint shell_extract_jnum(int i) {
    return shell_extract_greennum(i, J_GREEN);
}

uint shell_extract_hnum(int i) {
    return shell_extract_greennum(i, H_GREEN);
}

static uint shell_extract_greenmaxsize(int i, green_relation r) {
    if (objects[i].type == RECDEF) {
        return UINT_MAX;
    }
    int j = shell_compute_syntac(i, false);
    if (j == -1) {
        return UINT_MAX;
    }
    parti* P = grel_to_parti(objects[j].mor->obj->rels, r);
    uint maxsize = 0;
    for (uint h = 0; h < P->size_par; h++) {
        if (P->cl_size[h] > maxsize) {
            maxsize = P->cl_size[h];
        }
    }
    return maxsize;
}

uint shell_extract_rmaxsize(int i) {
    return shell_extract_greenmaxsize(i, R_GREEN);
}

uint shell_extract_lmaxsize(int i) {
    return shell_extract_greenmaxsize(i, L_GREEN);
}

uint shell_extract_jmaxsize(int i) {
    return shell_extract_greenmaxsize(i, J_GREEN);
}

uint shell_extract_hmaxsize(int i) {
    return shell_extract_greenmaxsize(i, H_GREEN);
}


/**********************************/
/*+ Management of stored objects +*/
/**********************************/

uint(*num_extractors[FILTER_NUMSIZE]) (int i) = {
    shell_extract_sizemini,  // FILTER_MINI
    shell_extract_sizesynt,  // FILTER_SYNT
    shell_extract_idemnum,   // FILTER_IDEMS
    shell_extract_rnum,      // FILTER_RNUM
    shell_extract_lnum,      // FILTER_LNUM
    shell_extract_jnum,      // FILTER_JNUM
    shell_extract_hnum,      // FILTER_HNUM
    shell_extract_rmaxsize,  // FILTER_RMAXSIZE
    shell_extract_lmaxsize,  // FILTER_LMAXSIZE
    shell_extract_jmaxsize,  // FILTER_JMAXSIZE
    shell_extract_hmaxsize   // FILTER_HMAXSIZE
};

static bool extract_bool_nosimplec(int i) {
    if (objects[i].type != DAUTOMATON) {
        return false;
    }
    return is_nosimple_counter_dfa(objects[i].obj_dfa, NULL);
}

static bool extract_bool_nosmallc(int i) {
    if (objects[i].type != DAUTOMATON) {
        return false;
    }
    return is_nosmallcountertwo_dfa(objects[i].obj_dfa);
}

bool (*bool_extractors[FILTER_BOOLSIZE]) (int i) = {
    extract_bool_nosimplec, // FILTER_NOSIMC
    extract_bool_nosmallc,  // FILTER_NOSMALLCOUNTER
};

static void info_num(num_filter filter, uint value) {
    if (filter < 0 || filter >= FILTER_NUMSIZE) {
        fprintf(stderr, "Error: Invalid numerical filter.\n");
        return;
    }
    char buffer[64];
    switch (filter)
    {
    case FILTER_SYNT:
        sprintf(buffer, "Syntactic monoid: %d elements", value);
        break;
    case FILTER_MINI:
        sprintf(buffer, "Minimal automaton: %d states", value);
        break;
    case FILTER_IDEMS:
        sprintf(buffer, "Idempotents in the syntactic monoid: %d", value);
        break;
    case FILTER_RNUM:
        sprintf(buffer, "R-classes in the syntactic monoid: %d", value);
        break;
    case FILTER_LNUM:
        sprintf(buffer, "L-classes in the syntactic monoid: %d", value);
        break;
    case FILTER_JNUM:
        sprintf(buffer, "J-classes in the syntactic monoid: %d", value);
        break;
    case FILTER_HNUM:
        sprintf(buffer, "H-classes in the syntactic monoid: %d", value);
        break;
    case FILTER_RMAXSIZE:
        sprintf(buffer, "Maximal size of an R-class: %d", value);
        break;
    case FILTER_LMAXSIZE:
        sprintf(buffer, "Maximal size of an L-class: %d", value);
        break;
    case FILTER_JMAXSIZE:
        sprintf(buffer, "Maximal size of a J-class: %d", value);
        break;
    case FILTER_HMAXSIZE:
        sprintf(buffer, "Maximal size of an H-class: %d", value);
        break;
    default:
        fprintf(stderr, "Error: Invalid numerical filter.\n");
        break;
    }

    fprintf(stdout, "%-45s", buffer);
}

static void shell_neg_filter_error(const char* filter) {
    fprintf(stderr, "Error: The filter '%s' cannot be negated.\n", filter);
}

static void shell_nopar_filter_error(const char* filter) {
    fprintf(stderr, "Error: The filter '%s' does not take parameters.\n", filter);
}



int shell_retrieve_filters(com_parameters* pars, filters_info* info) {
    // Initialization of the filters info structure
    for (uint i = 0; i < FILTER_BOOLSIZE; i++) {
        info->used_boolean[i] = false;
        info->sign_boolean[i] = true;
    }
    for (uint i = 0; i < FILTER_NUMSIZE; i++) {
        info->used_numerical[i] = false;
        info->values[i][0] = 0; // Default value for lower bound
        info->values[i][1] = -1; // Default value for upper bound
    }
    info->nblow = 0;
    info->nbhigh = 0;

    par_type numtypes[2] = { PAR_INTEGER, PAR_INTEGER };
    int n = com_nbparams(pars);

    for (int i = 0; i < n;i++) {
        // We retrieve the i-th parameter
        com_command* param = com_getparam(pars, i);
        // Check if the parameter is a single keyword with or without parameters
        if (!com_single_par(param)) {
            shell_error_syntax();
            return -2;
        }
        // We retrieve the keyword
        com_keyword key = string_to_keyword(param->main->string);

        // Negation detection.
        bool not;
        if (key == KY_NOT) {
            not = false;
            if (com_nbparams(param->params) != 1) {
                shell_error_syntax();
                return -2;
            }
            param = com_getparam(param->params, 0);
            // Check if the parameter is a single keyword with or without parameters
            if (!com_single_par(param)) {
                shell_error_syntax();
                return -2;
            }
            // We retrieve the keyword
            key = string_to_keyword(param->main->string);
        }
        else {
            not = true;
        }


        switch (key)
        {
        case KY_SYNT:
            info->used_numerical[FILTER_SYNT] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_SYNT], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_MINI:
            info->used_numerical[FILTER_MINI] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_MINI], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_IDEMS:
            info->used_numerical[FILTER_IDEMS] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_IDEMS], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_RNUM:
            info->used_numerical[FILTER_RNUM] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_RNUM], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_LNUM:
            info->used_numerical[FILTER_LNUM] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_LNUM], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_JNUM:
            info->used_numerical[FILTER_JNUM] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_JNUM], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_HNUM:
            info->used_numerical[FILTER_HNUM] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_HNUM], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_RMAXSIZE:
            info->used_numerical[FILTER_RMAXSIZE] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_RMAXSIZE], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_LMAXSIZE:
            info->used_numerical[FILTER_LMAXSIZE] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_LMAXSIZE], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_JMAXSIZE:
            info->used_numerical[FILTER_JMAXSIZE] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_JMAXSIZE], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;
        case KY_HMAXSIZE:
            info->used_numerical[FILTER_HMAXSIZE] = true;
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (param_retrieve(param->params, 2, 2, numtypes, NULL, NULL, info->values[FILTER_HMAXSIZE], NULL, NULL, param->main->string) == -2) {
                shell_error_syntax();
                return -2;
            }

            break;

        case KY_OUTSIDE:
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (info->nblow + com_nbparams(param->params) >= FILTER_MAXCLASSES) {
                fprintf(stderr, "Error: Too many forbidden classes in the filter (maximum: %d).\n", FILTER_MAXCLASSES);
                return -2;
            }
            for (int h = info->nblow; h < info->nblow + com_nbparams(param->params); h++) {
                info->low[h] = command_to_class(com_getparam(param->params, h));
                if (info->low[h] == CL_END || class_membership[info->low[h]] == NULL) {
                    fprintf(stdout, "Error : Membership is unsupported for the class %s.\n", class_names[info->low[h]]);
                    return -2;
                }
            }
            info->nblow += com_nbparams(param->params);
            break;
        case KY_INSIDE:
            if (!not) {
                shell_neg_filter_error(param->main->string);
                return -2;
            }
            if (info->nbhigh + com_nbparams(param->params) >= FILTER_MAXCLASSES) {
                fprintf(stderr, "Error: Too many mandatory classes in the filter (maximum: %d).\n", FILTER_MAXCLASSES);
                return -2;
            }
            for (int h = info->nbhigh; h < info->nbhigh + com_nbparams(param->params); h++) {
                info->high[h] = command_to_class(com_getparam(param->params, h));
                if (info->high[h] == CL_END || class_membership[info->high[h]] == NULL) {
                    fprintf(stdout, "Error : Membership is unsupported for the class %s.\n", class_names[info->high[h]]);
                    return -2;
                }
            }
            info->nbhigh += com_nbparams(param->params);
            break;
        case KY_NOSIMC:
            info->used_boolean[FILTER_NOSIMC] = true;
            info->sign_boolean[FILTER_NOSIMC] = not;
            if (com_nbparams(param->params) > 0) {
                shell_nopar_filter_error(param->main->string);
                return -2;
            }
            break;
        case KY_NOSMALLCOUNTER:
            info->used_boolean[FILTER_NOSMALLCOUNTER] = true;
            info->sign_boolean[FILTER_NOSMALLCOUNTER] = not;
            if (com_nbparams(param->params) > 0) {
                shell_nopar_filter_error(param->main->string);
                return -2;
            }
            break;
        default:
            break;
        }

    }



    info->order = false;
    for (int i = 0; i < info->nblow; i++) {
        if (shell_membership_needs_order(info->low[i])) {
            info->order = true;
            break;
        }
    }
    if (!info->order) {
        for (int i = 0; i < info->nbhigh; i++) {
            if (shell_membership_needs_order(info->high[i])) {
                info->order = true;
                break;
            }
        }
    }

    return -1; // Success

}


int shell_filter_objects(com_parameters* pars, ob_type type) {
    int n = com_nbparams(pars);
    if (type == RECDEF && n > 0) {
        fprintf(stderr, "Error: Cannot apply filters to recursive definitions.\n");
        return -2;
    }

    filters_info info;
    if (shell_retrieve_filters(pars, &info) == -2) {
        return -2; // Error in retrieving filters
    }


    // Computes the total number of objects of the given type (skips hidden objects: syntactic monoids, minimal automata, etc.)
    uint tcount = 0;
    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].parent != -1 || (type != DUMMY && objects[i].type != type)) {
            continue;
        }
        tcount++;
    }

    switch (type) {
    case REGEXP:
        printf("#### There are %d regular expressions(s) in memory:\n\n", tcount);
        break;
    case NAUTOMATON:
        printf("#### There are %d automata in memory:\n\n", tcount);
        break;
    case MORPHISM:
        printf("#### There are %d morphism(s) in memory:\n\n", tcount);
        break;
    case RECDEF:
        printf("#### There are %d recursive definition(s) in memory:\n\n", tcount);
        break;
    case DUMMY:
        printf("#### There are %d object(s) in memory:\n\n", tcount);
        break;
    default:
        break;
    }


    // Computes the number of objects satisfying the filters.
    uint count = 1;
    uint objects_vals[5][FILTER_NUMSIZE];
    char buffer[128];
    uchar f = 0;

    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].parent != -1 || (type != DUMMY && objects[i].type != type)) {
            continue;
        }
        bool skip = false;
        for (uint j = 0; j < FILTER_NUMSIZE; j++) {
            if (!info.used_numerical[j]) {
                continue; // Skip unused numerical filters.
            }
            objects_vals[f][j] = num_extractors[j](i); // Extract the numerical value for the object.
            if (objects_vals[f][j] == UINT_MAX || objects_vals[f][j] < ((uint)info.values[j][0]) || (info.values[j][1] != -1 && objects_vals[f][j] > ((uint)info.values[j][1]))) {
                skip = true;
                break; // No need to check further filters for this object.
            }
        }
        if (skip) {
            continue; // If the object is not selected by numerical filters, skip boolean filters.
        }

        for (uint j = 0; j < FILTER_BOOLSIZE; j++) {
            if (!info.used_boolean[j]) {
                continue; // Skip unused boolean filters.
            }
            if (info.sign_boolean[j]) {
                skip = !bool_extractors[j](i);
            }
            else {
                skip = !bool_extractors[j](i);
            }
        }
        if (skip) {
            continue; // If the object is not selected by numerical filters, skip boolean filters.
        }

        for (int h = 0; h < info.nbhigh; h++) {
            if (!class_membership[info.high[h]](i, MEMB_OPTIMAL, NULL)) {
                skip = true;
                break; // If the automaton does not belong to a required class, we stop checking.
            }
        }

        if (skip) {
            continue;
        }
        for (int h = 0; h < info.nblow; h++) {
            if (class_membership[info.low[h]](i, MEMB_OPTIMAL, NULL)) {
                skip = true;
                break; // If the automaton belongs to a forbidden class, we stop checking.
            }
        }

        if (skip) {
            continue;
        }

        f++;
        if (type == DUMMY) {
            sprintf(buffer, "#### %d (%s): %s", count, object_types_names[objects[i].type], object_get_full_name(i));
        }
        else {
            sprintf(buffer, "#### %d: %s", count, object_get_full_name(i));
        }
        fprintf(stdout, "%-45s", buffer);
        count++;
        if (f == 5) {
            fprintf(stdout, "\n");
            f = 0;
            for (int k = 0; k < FILTER_NUMSIZE; k++) {
                if (!info.used_numerical[k]) {
                    continue; // Skip unused numerical filters.
                }
                for (int j = 0; j < 5; j++) {
                    info_num((num_filter)k, objects_vals[j][k]);
                }
                fprintf(stdout, "\n");
            }
            fprintf(stdout, "\n");
        }


    }

    if (f != 0) {
        fprintf(stdout, "\n");
        for (int k = 0; k < FILTER_NUMSIZE; k++) {
            if (!info.used_numerical[k]) {
                continue; // Skip unused numerical filters.
            }
            for (int j = 0; j < f; j++) {
                info_num((num_filter)k, objects_vals[j][k]);
            }
            fprintf(stdout, "\n");
        }
        fprintf(stdout, "\n");
    }

    count--;

    if (count < tcount) {
        switch (type) {
        case REGEXP:
            printf("#### Of the %d regular expressions(s) in memory, %d satisfy the specified properties.\n\n", tcount, count);
            break;
        case NAUTOMATON:
            printf("#### Of the %d automata in memory, %d satisfy the specified properties.\n\n", tcount, count);
            break;
        case MORPHISM:
            printf("#### Of the %d morphism(s) in memory, %d satisfy the specified properties.\n\n", tcount, count);
            break;
        case RECDEF:
            printf("#### Of the %d recursive definition(s) in memory, %d satisfy the specified properties.\n\n", tcount, count);
            break;
        case DUMMY:
            printf("#### Of the %d object(s) in memory, %d satisfy the specified properties.\n\n", tcount, count);
            break;
        default:
            break;
        }
    }
    return -1;
}



/***********/
/* Sorting */
/***********/

int object_compare(int i1, int i2) {
    if (i1 == -1 || i2 == -1) {
        return 0;
    }
    if (objects[i1].parent != -1) {
        if (objects[i2].parent != -1) {
            return 0;
        }
        return 1;
    }

    if (objects[i2].parent != -1) {
        return -1;
    }

    return strcmp(object_get_full_name(i1), object_get_full_name(i2));
}


uint(*extractor)(int) = NULL;

int object_compare_numerical(int i1, int i2) {
    if (i1 == -1 || i2 == -1) {
        return 0;
    }
    if (objects[i1].type == RECDEF && objects[i2].type == RECDEF) {
        return 0;
    }

    if (objects[i1].type == RECDEF) {
        return -1;
    }

    if (objects[i2].type == RECDEF) {
        return 1;
    }

    return ((int)extractor(i1)) - ((int)extractor(i2));
}



static void max_heapify(int i, int max, int (*comp)(int, int)) {
    if (i < 0 || i > max) {
        return;
    }
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    while ((l <= max && comp(l, i) > 0) || (r <= max && comp(r, i) > 0)) {
        if (r <= max && comp(l, r) < 0) {
            object_swap(i, r);
            i = r;

        }
        else {
            object_swap(i, l);
            i = l;
        }
        l = 2 * i + 1;
        r = 2 * i + 2;
    }
}

void object_sort_array(int (*comp)(int, int)) {
    int end = nb_objects - 1;
    // Heapify
    for (int i = end; i >= 0; i--) {
        max_heapify(i, end, comp);
    }
    // Sort

    for (int max = end; max > 0; max--) {
        object_swap(0, max);
        max_heapify(0, max - 1, comp);
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
        extractor = shell_extract_sizemini;
        break;
    case KY_SYNT:
        extractor = shell_extract_sizesynt;
        break;
    case KY_IDEMS:
        extractor = shell_extract_idemnum;
        break;
    case KY_RNUM:
        extractor = shell_extract_rnum;
        break;
    case KY_LNUM:
        extractor = shell_extract_lnum;
        break;
    case KY_JNUM:
        extractor = shell_extract_jnum;
        break;
    case KY_HNUM:
        extractor = shell_extract_hnum;
        break;
    case KY_RMAXSIZE:
        extractor = shell_extract_rmaxsize;
        break;
    case KY_LMAXSIZE:
        extractor = shell_extract_lmaxsize;
        break;
    case KY_JMAXSIZE:
        extractor = shell_extract_jmaxsize;
        break;
    case KY_HMAXSIZE:
        extractor = shell_extract_hmaxsize;
        break;
    default:
        break;
    }
    if (!extractor) {
        shell_error_invalidpar(keywordtostring(KY_SORT), pars->param);
        return -1;
    }
    object_sort_array(object_compare_numerical);
    extractor = NULL; // Reset extractor to avoid confusion in future calls
    return -1;
}

/**************/
/*+ Deletion +*/
/**************/

// static void shell_filter_del(int i) {
//     for (uint h = 0; h < OD_SIZE; h++) {
//         if (objects[i].depend[h] != -1) {
//             object_free_aux(&objects[objects[i].depend[h]]);
//         }
//     }
//     object_free_aux(&objects[i]);
// }


int shell_filter_delete(com_parameters* pars, ob_type type) {
    int n = com_nbparams(pars);
    if (type == RECDEF && n > 0) {
        fprintf(stderr, "Error: Cannot apply filters to recursive definitions.\n");
        return -2;
    }

    filters_info info;
    if (shell_retrieve_filters(pars, &info) == -2) {
        return -2; // Error in retrieving filters
    }

    for (int i = nb_objects - 1; i >= 0; i--) {
        if (i >= nb_objects) {
            continue;
        }

        if (objects[i].type == EMPTYOBJ || (type != DUMMY && objects[i].type != type) || objects[i].parent != -1) {
            continue;
        }
        bool skip = false;


        for (uint j = 0; j < FILTER_NUMSIZE; j++) {
            if (!info.used_numerical[j]) {
                continue; // Skip unused numerical filters.
            }
            uint val = num_extractors[j](i); // Extract the numerical value for the object.
            if (val == UINT_MAX || val < ((uint)info.values[j][0]) || (info.values[j][1] != -1 && val > ((uint)info.values[j][1]))) {
                skip = true;
                break; // No need to check further filters for this object.
            }
        }
        if (skip) {
            object_free(i);
            continue; // If the object is not selected by numerical filters, skip boolean filters.
        }

        for (uint j = 0; j < FILTER_BOOLSIZE; j++) {
            if (!info.used_boolean[j]) {
                continue; // Skip unused boolean filters.
            }
            if (info.sign_boolean[j]) {
                skip = !bool_extractors[j](i);
            }
            else {
                skip = !bool_extractors[j](i);
            }
        }
        if (skip) {
            object_free(i);
            continue; // If the object is not selected by numerical filters, skip boolean filters.
        }

        for (int h = 0; h < info.nbhigh; h++) {
            if (!class_membership[info.high[h]](i, MEMB_OPTIMAL, NULL)) {
                skip = true;
                break; // If the automaton does not belong to a required class, we stop checking.
            }
        }

        if (skip) {
            object_free(i);
            continue;
        }
        for (int h = 0; h < info.nblow; h++) {
            if (class_membership[info.low[h]](i, MEMB_OPTIMAL, NULL)) {
                skip = true;
                break; // If the automaton belongs to a forbidden class, we stop checking.
            }
        }

        if (skip) {
            object_free(i);
            continue;
        }
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

/***************************/
/*+ Recursive definitions +*/
/***************************/

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

    i = object_init(varname);
    objects[i].type = RECDEF;
    MALLOC(objects[i].rec, 1);
    objects[i].rec->num = com_nbparams(pars) - 1;
    objects[i].rec->init = nb + 1;
    objects[i].rec->full = false;

    for (uchar j = 0; j < objects[i].rec->num; j++) {
        pars = pars->next;
        objects[i].rec->names[j] = strdup(pars->param->main->string);
        objects[i].rec->def[j] = NULL;
        MALLOC(objects[i].rec->regexps[j], nb);
        for (ushort x = 0; x < nb + 1; x++) {
            objects[i].rec->regexps[j][x] = NULL;
        }
    }

    fprintf(stdout, "\n#### Initialization of the recursive definition %s.\n", varname);
    shell_rec_display(objects[i].rec, stdout);

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
    nfa_reset_state_names(objects[i].obj_nfa);
    return -1;
}
