#include "shell_languages.h"

#include "error.h"
#include "interrupt.h"
#include "nfa.h"
#include "regexp.h"
#include "shell_errors.h"
#include "printing.h"

object* objects = NULL;
int nb_objects = 0;
int nb_objemax = 0;

char* object_types_names[DUMMY] = { "empty object", "regexp", "automaton", "automaton", "morphism", "recdef" };

void init_objects_array(void) {
    CALLOC(objects, 1024);
    nb_objects = 0;
    nb_objemax = 1024;
}

void grow_objects_array(void) {
    if (nb_objects >= nb_objemax) {
        nb_objemax *= 2;
        REALLOC(objects, nb_objemax);
    }
}

/************************/
/* Création/Suppression */
/************************/

ob_prefixname* create_prefixname_full(const char* name) {
    if (!name || strlen(name) == 0) {
        fprintf(stderr, "Error: tried to create a prefix with an empty name.\n");
        return NULL;
    }
    ob_prefixname* prefix;
    MALLOC(prefix, 1);
    prefix->name = strdup(name);
    prefix->count = 1;
    prefix->digits = 0; // Full name has no suffix
    return prefix;
}

ob_prefixname* create_prefixname(const char* name, uint count) {
    if (!name || strlen(name) == 0) {
        fprintf(stderr, "Error: tried to create a prefix with an empty name.\n");
        return NULL;
    }
    if (count == 0) {
        fprintf(stderr, "Error: tried to create a prefix with count 0.\n");
        return NULL;
    }
    ob_prefixname* prefix;
    MALLOC(prefix, 1);
    prefix->name = strdup(name);
    prefix->count = count;
    prefix->digits = (uchar)get_uint_length(count);
    return prefix;
}

void remove_instance_prefixname(ob_prefixname* prefixname) {
    if (!prefixname) {
        return;
    }
    if (prefixname->count > 0) {
        prefixname->count--;
    }

    if (prefixname->count == 0 || prefixname->digits == 0) {
        free(prefixname->name);
        free(prefixname);
    }
}

int object_init(const char* name) {
    grow_objects_array();
    int i = nb_objects++;
    if (name) {
        objects[i].prefix = create_prefixname_full(name);
        objects[i].number = UINT_MAX;
    }
    else {
        objects[i].prefix = NULL;
        objects[i].number = UINT_MAX;
    }
    objects[i].type = EMPTYOBJ;
    objects[i].parent = -1;
    for (uint j = 0; j < OD_SIZE; j++) {
        objects[i].depend[j] = -1;
    }
    return i;
}



void object_swap(int i, int j) {
    if (i < 0 || i >= nb_objects || j < 0 || j >= nb_objects) {
        fprintf(stderr, "Error: tried to swap an invalid object.\n");
        return;
    }

    if (i == j) {
        return;
    }

    // An array to store the objects connected to the two that are going to be swapped.
    int tab[2 * (OD_SIZE)+2];

    // The parents of the two objects.
    tab[0] = objects[i].parent;
    int s = 1;
    if (tab[0] != objects[j].parent) {
        tab[s++] = objects[j].parent;
    }

    // The dependencies of the two objects.
    for (uchar h = 0; h < OD_SIZE; h++) {
        tab[s++] = objects[i].depend[h];
        tab[s++] = objects[j].depend[h];
    }

    for (uchar g = 0; g < s; g++) {
        int k = tab[g];
        if (k == -1) {
            continue;
        }
        if (objects[k].parent == i) {
            objects[k].parent = j;
        }
        else if (objects[k].parent == j) {
            objects[k].parent = i;
        }
        for (uchar h = 0; h < OD_SIZE; h++) {
            if (objects[k].depend[h] == i) {
                objects[k].depend[h] = j;
            }
            else if (objects[k].depend[h] == j) {
                objects[k].depend[h] = i;
            }
        }
    }
    object temp = objects[i];
    objects[i] = objects[j];
    objects[j] = temp;
}

void object_free_aux(object* theob) {
    if (!theob) {
        return;
    }

    remove_instance_prefixname(theob->prefix);
    //free(theob->name);

    switch (theob->type)
    {
    case REGEXP:
        reg_free(theob->exp);
        break;
    case NAUTOMATON:
        nfa_delete(theob->obj_nfa);
        break;
    case DAUTOMATON:
        dfa_delete(theob->obj_dfa);
        break;
    case MORPHISM:
        if (theob->mor) {
            delete_morphism(theob->mor->obj);
            for (int i = 0; i < KER_SIZE; i++) {
                delete_subsemi(theob->mor->kers[i]);
            }
            for (int i = 0; i < ORB_SIZE; i++) {
                delete_orbits(theob->mor->orbs[i]);
            }
        }
        free(theob->mor);
        break;
    case RECDEF:
        if (theob->rec) {
            for (uint h = 0; h < theob->rec->num; h++) {
                for (uint g = 0; g < theob->rec->init; g++) {
                    reg_free(theob->rec->regexps[h][g]);
                }
                free(theob->rec->regexps[h]);
                reg_free(theob->rec->def[h]);
            }
        }
        free(theob->rec);
    default:
        break;
    }
    theob->type = EMPTYOBJ; // Clear the pointer to the deleted object
}

void object_free(int i) {
    if (i < 0 || i >= nb_objects) {
        //fprintf(stderr, "%d Error: tried to delete an invalid object.\n", i);
        return;
    }



    object_swap(i, nb_objects - 1);
    i = nb_objects - 1;
    uchar count = 1;
    for (uint h = 0; h < OD_SIZE; h++) {
        if (objects[i].depend[h] != -1) {
            object_swap(objects[i].depend[h], i - count);
            count++;
        }
    }

    for (uint h = 0; h < count; h++) {
        object_free_aux(&objects[i - h]);
        objects[i - h].type = EMPTYOBJ;
        nb_objects--;
    }


}

void object_free_all(void) {
    for (int i = 0; i < nb_objects; i++) {
        object_free_aux(&objects[i]);
        objects[i].type = EMPTYOBJ;
    }
    nb_objects = 0;
}

void object_delete_prefix(const char* prefix) {
    if (!prefix || strlen(prefix) == 0) {
        return;
    }

    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].parent == -1 && strncmp(objects[i].prefix->name, prefix, strlen(prefix)) == 0) {
            for (uint h = 0; h < OD_SIZE; h++) {
                if (objects[i].depend[h] != -1) {
                    object_free_aux(&objects[objects[i].depend[h]]);
                }
            }
            object_free_aux(&objects[i]);
        }
    }

    int nu = 0;

    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].type == EMPTYOBJ) {
            continue; // Skip deleted objects
        }

        if (i == nu) {
            nu++;
            continue; // No need to move the object
        }

        int k = objects[i].parent;
        if (k != -1) {
            for (uchar h = 0; h < OD_SIZE; h++) {
                if (objects[k].depend[h] == i) {
                    objects[k].depend[h] = nu;
                }
            }
        }
        for (uchar h = 0; h < OD_SIZE; h++) {
            int j = objects[i].depend[h];
            if (j == -1) {
                continue; // Skip if no dependency
            }
            objects[j].parent = nu; // Update the parent of the dependent object
        }
        objects[nu] = objects[i]; // Move the object to the new position
        objects[i].type = EMPTYOBJ;
        nu++;

    }

    nb_objects = nu; // Update the number of objects
}


/************************/
/* Get/insert an object */
/************************/

char full_name_buffer[256];

const char* object_get_full_name(int i) {
    if (i < 0 || i >= nb_objects) {
        return NULL; // Invalid index
    }
    if (objects[i].prefix == NULL) {
        return NULL; // No prefix name
    }
    if (objects[i].number == UINT_MAX || objects[i].prefix->digits == 0) {
        return objects[i].prefix->name; // Full name without suffix
    }
    sprintf(full_name_buffer, "%s%0*u", objects[i].prefix->name, objects[i].prefix->digits, objects[i].number);
    return full_name_buffer;
}




int object_get_from_name(const char* name) {
    if (!name) {
        return -1;
    }
    for (int i = 0; i < nb_objects; i++) {
        if (objects[i].parent == -1 && objects[i].prefix && strcmp(object_get_full_name(i), name) == 0) {
            return i;
        }
    }
    return -1;
}

int object_delete_from_name(const char* name) {
    int i = object_get_from_name(name);
    if (i == -1 || objects[i].parent != -1) {
        return -1;
    }
    object_free(i);
    return i;
}



int object_add_regexp(const char* name, regexp* theregexp) {
    if (theregexp == NULL) {
        shell_error_null();
        return -1;
    }

    if (name != NULL) {
        int i = object_get_from_name(name);
        if (i != -1) {
            object_free(i);
        }
    }

    grow_objects_array();
    int i = object_init(name);
    objects[i].type = REGEXP;
    objects[i].exp = theregexp;
    return i;
}

int object_add_automaton_nfa(const char* name, nfa* A) {
    if (A == NULL) {
        shell_error_null();
        return -1;
    }

    if (name != NULL) {
        int i = object_get_from_name(name);
        if (i != -1) {
            object_free(i);
        }
    }

    grow_objects_array();

    int i = object_init(name);
    objects[i].type = NAUTOMATON;
    objects[i].obj_nfa = A;
    return i;
}

int object_add_automaton_dfa(const char* name, dfa* A) {
    if (A == NULL) {
        shell_error_null();
        return -1;
    }

    if (name != NULL) {
        int i = object_get_from_name(name);
        if (i != -1) {
            object_free(i);
        }
    }

    grow_objects_array();
    int i = object_init(name);
    objects[i].type = DAUTOMATON;
    objects[i].obj_dfa = A;
    return i;
}


void object_add_automaton_dfa_family(const char* pname, dfa** array, uint count) {
    if (count == 0) {
        return;
    }
    if (!array || !pname) {
        shell_error_null();
        return;
    }
    // Deletes all objects with the given prefix.
    object_delete_prefix(pname);
    ob_prefixname* prefixname = create_prefixname(pname, count);

    for (uint k = 0; k < count; k++) {
        grow_objects_array();
        int i = nb_objects++;
        objects[i].prefix = prefixname;
        objects[i].number = k;
        objects[i].type = DAUTOMATON;
        objects[i].parent = -1;
        for (uint j = 0; j < OD_SIZE; j++) {
            objects[i].depend[j] = -1;
        }
        objects[i].obj_dfa = array[k];
    }
}

int object_add_morphism(char* name, morphism* M) {
    if (M == NULL) {
        shell_error_null();
        return -1;
    }

    if (name != NULL) {
        int i = object_get_from_name(name);
        if (i != -1) {
            object_free(i);
        }
    }

    grow_objects_array();
    int i = object_init(name);
    objects[i].type = MORPHISM;
    MALLOC(objects[i].mor, 1);
    objects[i].mor->obj = M;

    for (int j = 0; j < KER_SIZE; j++) {
        objects[i].mor->kers[j] = NULL;
    }
    for (int j = 0; j < ORB_SIZE; j++) {
        objects[i].mor->orbs[j] = NULL;
    }

    return i;
}

int shell_copy_generic(int i, char* newname) {
    if (i == -1) {
        return -1;
    }
    switch (objects[i].type) {
    case DAUTOMATON:
        return object_add_automaton_dfa(newname, dfa_copy(objects[i].obj_dfa));
        break;
    case NAUTOMATON:
        return object_add_automaton_nfa(newname, nfa_copy(objects[i].obj_nfa));
        break;
    case REGEXP:
        return object_add_regexp(newname, reg_copy(objects[i].exp));
    default:
        return -1;
        break;
    }
}

/***********************************************/
/* Computing information on an existing object */
/***********************************************/

int shell_compute_minimal(int i) {
    if (i < 0 || i > nb_objects - 1 || objects[i].type > MORPHISM || objects[i].type < REGEXP) {
        fprintf(stderr, "Error: invalid object.\n");
        return -1;
    }

    if (objects[i].parent != -1) {
        return shell_compute_minimal(objects[i].parent);
    }

    if (objects[i].depend[OD_MINI] != -1) {
        return objects[i].depend[OD_MINI];
    }

    dfa* mini = NULL;
    switch (objects[i].type)
    {
    case REGEXP:
    {
        nfa* A = reg_thompson(objects[i].exp);
        mini = nfa_brzozowski(A);
        nfa_delete(A);
    }
    break;
    case NAUTOMATON:
        mini = nfa_brzozowski(objects[i].obj_nfa);
        break;
    case DAUTOMATON:
        //mini = dfa_brzozowski(objects[i].obj_dfa);
        mini = dfa_hopcroft(objects[i].obj_dfa);
        break;
    case MORPHISM:
    {
        dfa A;
        A.alphabet = objects[i].mor->obj->alphabet;
        A.finals = objects[i].mor->obj->accept_list;
        A.initial = ONE;
        A.nb_finals = objects[i].mor->obj->nb_accept;
        A.state_names = NULL;
        A.trans = objects[i].mor->obj->r_cayley;
        mini = dfa_hopcroft(&A);
    }
    break;
    default:
        break;
    }


    int j = object_add_automaton_dfa(NULL, mini);
    if (j == -1) {
        dfa_delete(mini);
        return j;
    }
    objects[i].depend[OD_MINI] = j;
    objects[j].parent = i;

    return j;
}

int shell_compute_syntac(int i, bool order) {
    if (i < 0 || i > nb_objects - 1 || objects[i].type > MORPHISM || objects[i].type < REGEXP) {
        fprintf(stderr, "Error: invalid object.\n");
        return INVALID_OBJECT;
    }

    if (objects[i].parent != -1) {
        //printf("Parent: %d\n", objects[i].parent);
        return shell_compute_syntac(objects[i].parent, order);
    }



    if (objects[i].depend[OD_SYNT] != -1) {
        int j = objects[i].depend[OD_SYNT];
        if (order && !objects[j].mor->obj->order) {
            object_free(j);
            objects[i].depend[OD_SYNT] = -1;
        }
        else {
            return objects[i].depend[OD_SYNT];
        }
    }

    // Start by calculating the object's minimal automaton (if not already done).
    int j = shell_compute_minimal(i);
    if (j < 0) {
        return j;
    }

    int error = 0;
    morphism* synt = dfa_to_morphism(objects[j].obj_dfa, order, &error, NULL);

    // if (interrupt_flag) {
    //     interrupt_flag = false;
    // }

    if (!synt) {
        // The morphism could not be built, return immediately the error code.
        return error;
    }


    int k = object_add_morphism(NULL, synt);
    if (k < 0) {
        delete_morphism(synt);
        return k;
    }
    objects[i].depend[OD_SYNT] = k;
    objects[k].parent = i;
    return k;
}

void shell_compute_mult(int i) {
    if (i < 0 || i > nb_objects - 1 || objects[i].type != MORPHISM) {
        fprintf(stderr, "Error: invalid object.\n");
        return;
    }
    mor_compute_mult(objects[i].mor->obj);
}

void shell_compute_order(int i) {
    if (i < 0 || i > nb_objects - 1 || objects[i].type != MORPHISM) {
        fprintf(stderr, "Error: invalid object.\n");
        return;
    }
    mor_compute_order(objects[i].mor->obj);
}


subsemi* shell_compute_ker(int i, kernel_type type, sub_level level) {
    if (i < 0 || i > nb_objects - 1 || objects[i].type != MORPHISM) {
        fprintf(stderr, "Error: invalid object.\n");
        return NULL;
    }


    // If part of the kernel has already been computed and we now want more, we delete the old kernel.
    if (objects[i].mor->kers[type] && objects[i].mor->kers[type]->level < level) {
        delete_subsemi(objects[i].mor->kers[type]);
        objects[i].mor->kers[type] = NULL;
    }

    if (!objects[i].mor->kers[type]) {
        switch (type)
        {
        case KER_MOD:
            objects[i].mor->kers[type] = get_kernel(objects[i].mor->obj, level, BA_MOD);
            break;
        case KER_AMT:
            objects[i].mor->kers[type] = get_kernel(objects[i].mor->obj, level, BA_AMT);
            break;
        case KER_GR:
            objects[i].mor->kers[type] = get_kernel(objects[i].mor->obj, level, BA_GR);
            break;
        default:
            break;
        }
    }
    return objects[i].mor->kers[type];
}

orbits* shell_compute_orbits(int i, orbits_type type, sub_level level) {
    if (i < 0 || i > nb_objects - 1 || objects[i].type != MORPHISM) {
        fprintf(stderr, "Error: invalid object.\n");
        return NULL;
    }

    // If part of the orbits have already been computed and we now want more, we delete the old orbits.
    if (objects[i].mor->orbs[type] && objects[i].mor->orbs[type]->level < level) {
        delete_orbits(objects[i].mor->orbs[type]);
        objects[i].mor->orbs[type] = NULL;
    }

    if (!objects[i].mor->orbs[type]) {
        switch (type)
        {
        case ORB_DD:
            objects[i].mor->orbs[type] = compute_ddorbits(objects[i].mor->obj);
            break;
        case ORB_MODP:
            objects[i].mor->orbs[type] = compute_gplusorbits(shell_compute_ker(i, KER_MOD, level));
            break;
        case ORB_AMTP:
            objects[i].mor->orbs[type] = compute_gplusorbits(shell_compute_ker(i, KER_AMT, level));
            break;
        case ORB_GRP:
            objects[i].mor->orbs[type] = compute_gplusorbits(shell_compute_ker(i, KER_GR, level));
            break;
        case ORB_PT:
            objects[i].mor->orbs[type] = compute_ptorbits(objects[i].mor->obj, level);
            break;
        case ORB_BPMOD:
            objects[i].mor->orbs[type] = compute_bpgorbits(objects[i].mor->obj, level, BA_MOD);
            break;
        case ORB_BPAMT:
            objects[i].mor->orbs[type] = compute_bpgorbits(objects[i].mor->obj, level, BA_AMT);
            break;
        case ORB_BPGR:
            objects[i].mor->orbs[type] = compute_bpgorbits(objects[i].mor->obj, level, BA_GR);
            break;
        case ORB_BPDD:
            objects[i].mor->orbs[type] = compute_bpgplusorbits(objects[i].mor->obj, level, BA_ST);
            break;
        case ORB_BPMODP:
            objects[i].mor->orbs[type] = compute_bpgplusorbits(objects[i].mor->obj, level, BA_MOD);
            break;
        case ORB_BPAMTP:
            objects[i].mor->orbs[type] = compute_bpgplusorbits(objects[i].mor->obj, level, BA_AMT);
            break;
        case ORB_BPGRP:
            objects[i].mor->orbs[type] = compute_bpgplusorbits(objects[i].mor->obj, level, BA_GR);
            break;
        default:
            break;
        }
    }

    return objects[i].mor->orbs[type];
}


/*************/
/* Recursion */
/*************/

uchar shell_rec_getnum(ob_recursion* obj, char* name) {
    uchar j = 0;
    while (j < obj->num && strcmp(name, obj->names[j]) != 0) {
        j++;
    }
    return j;
}

static bool shell_check_defined(ob_recursion* obj) {
    for (uint j = 0; j < obj->num; j++) {
        if (!obj->def[j]) {
            return false;
        }
        for (ushort x = 0; x < obj->init; x++) {
            if (!obj->regexps[j][x]) {
                return false;
            }
        }
    }
    return true;
}

bool shell_check_recursion(int i) {
    if (objects[i].type != RECDEF) {
        return false;
    }
    if (!shell_check_defined(objects[i].rec)) {
        return false;
    }
    ob_recursion* obj = objects[i].rec;

    // Checking that there are no loops in the recursion scheme.
    bool trans[obj->num][obj->num];
    for (uint j = 0; j < obj->num; j++) {
        for (uint h = 0; h < obj->num; h++) {
            trans[j][h] = false;
        }
        if (!reg_symbolic_loops(obj->def[j], obj->init, obj->num, trans[j])) {
            return false;
        }
        if (trans[j][j]) {
            return false;
        }
    }

    graph* G = create_graph_noedges(obj->num);
    for (uint j = 0; j < obj->num; j++) {
        for (uint h = 0; h < obj->num; h++) {
            if (trans[j][h]) {
                rigins_dequeue(h, G->edges[j]);
            }
        }
    }

    parti* part = tarjan(G);
    delete_graph(G);
    if (part->size_par != part->size_set) {
        delete_parti(part);
        return false;
    }
    for (uint j = 0; j < obj->num; j++) {
        obj->evalseq[j] = part->numcl[obj->num - 1 - j];
    }

    delete_parti(part);

    return true;
}

int shell_rec_defadd(int i, uchar j, regexp* exp) {
    if (objects[i].type != RECDEF) {
        return -1;
    }
    if (j >= objects[i].rec->num) {
        return -1;
    }
    // Release of the previous recursion
    reg_free(objects[i].rec->def[j]);
    objects[i].rec->def[j] = exp;
    objects[i].rec->full = shell_check_recursion(i);
    return i;
}

int shell_rec_iniadd(int i, uchar j, ushort ind, regexp* exp) {
    if (objects[i].type != RECDEF) {
        return -1;
    }
    if (ind >= objects[i].rec->init) {
        return -1;
    }

    if (j >= objects[i].rec->num) {
        return -1;
    }

    if (!objects[i].rec->regexps[j][ind]) {
        reg_free(objects[i].rec->regexps[j][ind]);
    }
    objects[i].rec->regexps[j][ind] = exp;
    objects[i].rec->full = shell_check_recursion(i);
    return i;
}

static regexp* shell_makerec(regexp* exp, ushort ind, regexp** new, dequeue_gen* old) {
    if (!exp) {
        return NULL;
    }
    switch (exp->op) {
    case EPSILON:
    case EMPTY:
    case WORD:
    case CHAR:
        return reg_copy(exp);
        break;
    case SYCHAR:
        return reg_letter_numbered(exp->sylet.lab, ind - exp->sylet.dec);
        break;
    case SYVAR:
        if (exp->syvar.dec == 0) {
            return reg_copy(new[exp->syvar.ind]);
        }
        else {
            regexp** row = rigread_dequeue_gen(old, exp->syvar.dec - 1);
            return reg_copy(row[exp->syvar.ind]);
        }
        break;
    case CONCAT:
        return reg_concat(shell_makerec(exp->left, ind, new, old), shell_makerec(exp->right, ind, new, old));
        break;
    case UNION:
        return reg_union(shell_makerec(exp->left, ind, new, old), shell_makerec(exp->right, ind, new, old));
        break;
    case STAR:
        return reg_star(shell_makerec(exp->left, ind, new, old));
        break;
    case PLUS:
        return reg_plus(shell_makerec(exp->left, ind, new, old));
        break;
    default:
        return NULL;
        break;
    }
    return NULL;
}

regexp** shell_rec_compute(ob_recursion* obj, ushort ind) {
    if (!obj->full) {
        return NULL;
    }
    if (ind < obj->init) {
        regexp** ret;
        MALLOC(ret, obj->num);
        for (uchar i = 0; i < obj->num; i++) {
            ret[i] = reg_copy(obj->regexps[i][ind]);
        }
        return ret;
    }

    dequeue_gen* thequeue = create_dequeue_gen();
    for (ushort j = 0; j < obj->init; j++) {
        regexp** row;
        MALLOC(row, obj->num);
        for (uchar i = 0; i < obj->num; i++) {
            row[i] = reg_copy(obj->regexps[i][j]);
        }
        rigins_dequeue_gen(row, thequeue);
    }

    ushort c = obj->init;
    while (ind >= c) {
        regexp** nrow;
        MALLOC(nrow, obj->num);
        for (uchar h = 0; h < obj->num; h++) {
            uchar g = obj->evalseq[h];
            nrow[g] = shell_makerec(obj->def[g], c, nrow, thequeue);
        }
        rigins_dequeue_gen(nrow, thequeue);
        regexp** old = lefpull_dequeue_gen(thequeue);
        for (uchar h = 0; h < obj->num; h++) {
            reg_free(old[h]);
        }
        free(old);
        c++;
    }

    regexp** ret = rigpull_dequeue_gen(thequeue);
    while (!isempty_dequeue_gen(thequeue)) {
        regexp** old = lefpull_dequeue_gen(thequeue);
        for (uchar h = 0; h < obj->num; h++) {
            reg_free(old[h]);
        }
        free(old);
    }
    return ret;
}

regexp* shell_rec_getexp(int i, char* name, ushort ind) {
    if (objects[i].type != RECDEF) {
        return NULL;
    }

    if (!objects[i].rec->full) {
        return NULL;
    }

    uchar j = shell_rec_getnum(objects[i].rec, name);

    if (j >= objects[i].rec->num) {
        return NULL;
    }
    regexp** array = shell_rec_compute(objects[i].rec, ind);
    if (!array) {
        return NULL;
    }
    regexp* ret = array[j];
    for (uchar h = 0; h < objects[i].rec->num; h++) {
        if (h != j) {
            reg_free(array[h]);
        }
    }
    free(array);
    return ret;

    /*
    if (shell_rec_compute(i, ind + 1) == -1) {
        return NULL;
    }



    return objects[i].rec->regexps[j][ind];
 */
}

void shell_rec_display(ob_recursion* obj, FILE* out) {
    symbolic_count = obj->num;
    symbolic_names = obj->names;
    for (uint i = 0; i < obj->num; i++) {
        fprintf(out, "#### Recursion relation %sᵢ : ", obj->names[i]);

        if (obj->def[i]) {
            reg_print(obj->def[i]);
        }
        else {
            fprintf(out, "not yet defined.\n");
        }
        for (uint j = 0; j < obj->init; j++) {
            fprintf(out, "#### Initial case %s", obj->names[i]);
            fprint_subsc_utf8(j, out);
            fprintf(out, " : ");
            if (obj->regexps[i][j]) {
                reg_print(obj->regexps[i][j]);
            }
            else {
                fprintf(out, "not yet defined.\n");
            }
        }
        fprintf(out, "\n");
    }

    if (shell_check_defined(obj) && !obj->full) {
        fprintf(out, "#### Warning: Error found in the recursion scheme.\n\n");
    }

    symbolic_count = 0;
    symbolic_names = NULL;
}


/***********/
/* Display */
/***********/



void shell_view_object(object* ob, bool title) {
    if (!ob) {
        return;
    }
    switch (ob->type) {
    case REGEXP:
        if (title) {
            print_title_box(100, true, stdout, 1, "Regular expression");
        }
        reg_print(ob->exp);
        return;
        break;
    case NAUTOMATON:
        if (title) {
            print_title_box(100, true, stdout, 1, "Automaton");
        }
        view_nfa(ob->obj_nfa);
        return;
        break;
    case DAUTOMATON:
        if (title) {
            print_title_box(100, true, stdout, 1, "Automaton");
        }
        view_dfa(ob->obj_dfa);
        return;
        break;
    case MORPHISM:
        if (title) {
            print_title_box(100, true, stdout, 1, "Morphism");
        }
        view_morphism(ob->mor->obj, stdout);
        return;
        break;
    case RECDEF:
        if (title) {
            print_title_box(100, true, stdout, 1, "Recursive definition of regular expressions");
            shell_rec_display(ob->rec, stdout);
        }
        return;
        break;

    default:
        return;
        break;
    }
}
