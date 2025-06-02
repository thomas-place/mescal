/***********************/
/* Gestion de fichiers */
/***********************/

#include "files.h"

static void reg_to_string_aux(regexp* r, char* buffer) {
    TRACE("reg_to_string_aux");

    if (r == NULL) {
        return;
    }
    char mess[10];
    switch (r->op) {
    case CHAR:
        if (r->letter.num == -1) {
            sprintf(mess, "%c", r->letter.lab);
        }
        else {
            sprintf(mess, "%c[%d]", r->letter.lab, r->letter.num);
        }
        strcat(buffer, mess);
        break;
    case WORD:
        for (uint i = 0; i < size_word(r->word); i++) {
            if (lefread_word(r->word, i).num == -1) {
                sprintf(mess, "%c", lefread_word(r->word, i).lab);
            }
            else {
                sprintf(mess, "%c[%d]", lefread_word(r->word, i).lab, lefread_word(r->word, i).num);
            }
            strcat(buffer, mess);
        }
        break;
    case EMPTY:
        strcat(buffer, "\\0");
        break;
    case EPSILON:
        strcat(buffer, "\\1");
        break;
    case UNION:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, " + ");
        reg_to_string_aux(r->right, buffer);
        strcat(buffer, ")");
        break;
    case INTER:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, " & ");
        reg_to_string_aux(r->right, buffer);
        strcat(buffer, ")");
        break;
    case CONCAT:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, " ");
        reg_to_string_aux(r->right, buffer);
        strcat(buffer, ")");
        break;
    case STAR:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, ")*");
        break;
    case PLUS:
        strcat(buffer, "(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, ")#");
        break;
    case COMPLEMENT:
        strcat(buffer, "!(");
        reg_to_string_aux(r->left, buffer);
        strcat(buffer, ")");
        break;
    default:
        CRITICAL("Unknown regexp operator (%d)", r->op);
        break;
    }
}

void reg_to_string(regexp* r, char* buffer) {
    TRACE("reg_to_string");

    if (!r) {
        return;
    }
    reg_to_string_aux(r, buffer);
}

unsigned short autocount = 0;

/***********************************************/
/* Manipulation de structures JSON - Automates */
/***********************************************/

json_object* files_nfa_to_json(nfa* A) {
    // Création de l'objet json
    json_object* root = json_object_new_object();
    if (!root) {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return NULL;
    }

    json_object_object_add(root, "TYPE", json_object_new_string("AUTO"));

    // Taille de l'automate (nombre d'états)
    json_object_object_add(root, "N_SIZE", json_object_new_int(A->trans->size_graph));

    // Alphabet
    json_object* alphabet = json_object_new_array_ext(A->trans->size_alpha);

    char buffer[2];
    for (uint i = 0; i < A->trans->size_alpha; i++) {
        sprintf(buffer, "%c", A->alphabet[i].lab);
        if (A->alphabet[i].num < 0) {
            json_object_array_add(alphabet, json_object_new_string(buffer));
        }
        else {
            json_object* the_letter = json_object_new_array_ext(2);
            json_object_array_add(the_letter, json_object_new_string(buffer));
            json_object_array_add(the_letter, json_object_new_int(A->alphabet[i].num));
            json_object_array_add(alphabet, the_letter);
        }
    }
    json_object_object_add(root, "L_ALPH", alphabet);

    // Liste des transitions (on commence par compter le nombre de transitions)
    uint num_trans = 0;
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            num_trans = num_trans + size_dequeue(A->trans->edges[q][a]);
        }
    }

    json_object* trans = json_object_new_array_ext(num_trans);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(A->trans->edges[q][a]); i++) {
                json_object* onetrans = json_object_new_array_ext(3);
                json_object_array_add(onetrans, json_object_new_int(q));
                json_object_array_add(onetrans, json_object_new_int(a));
                json_object_array_add(onetrans, json_object_new_int(lefread_dequeue(A->trans->edges[q][a], i)));
                json_object_array_add(trans, onetrans);
            }
        }
    }
    json_object_object_add(root, "L_TRAN", trans);

    // Liste des états initiaux
    json_object* initials = json_object_new_array_ext(size_dequeue(A->initials));
    for (uint i = 0; i < size_dequeue(A->initials); i++) {
        json_object_array_add(initials, json_object_new_int(lefread_dequeue(A->initials, i)));
    }

    json_object_object_add(root, "L_INIT", initials);

    // Liste des états finaux
    json_object* finals = json_object_new_array_ext(size_dequeue(A->finals));
    for (uint i = 0; i < size_dequeue(A->finals); i++) {
        json_object_array_add(finals, json_object_new_int(lefread_dequeue(A->finals, i)));
    }
    json_object_object_add(root, "L_FINA", finals);

    return root;
}


json_object* files_dfa_to_json(dfa* A) {
    // Création de l'objet json
    json_object* root = json_object_new_object();
    if (!root) {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return NULL;
    }

    json_object_object_add(root, "TYPE", json_object_new_string("AUTO"));

    // Taille de l'automate (nombre d'états)
    json_object_object_add(root, "N_SIZE", json_object_new_int(A->trans->size_graph));

    // Alphabet
    json_object* alphabet = json_object_new_array_ext(A->trans->size_alpha);

    char buffer[2];
    for (uint i = 0; i < A->trans->size_alpha; i++) {
        sprintf(buffer, "%c", A->alphabet[i].lab);
        if (A->alphabet[i].num < 0) {
            json_object_array_add(alphabet, json_object_new_string(buffer));
        }
        else {
            json_object* the_letter = json_object_new_array_ext(2);
            json_object_array_add(the_letter, json_object_new_string(buffer));
            json_object_array_add(the_letter, json_object_new_int(A->alphabet[i].num));
            json_object_array_add(alphabet, the_letter);
        }
    }
    json_object_object_add(root, "L_ALPH", alphabet);

    // Liste des transitions (on commence par compter le nombre de transitions)
    uint num_trans = A->trans->size_graph * A->trans->size_alpha;


    json_object* trans = json_object_new_array_ext(num_trans);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            json_object* onetrans = json_object_new_array_ext(3);
            json_object_array_add(onetrans, json_object_new_int(q));
            json_object_array_add(onetrans, json_object_new_int(a));
            json_object_array_add(onetrans, json_object_new_int(A->trans->edges[q][a]));
            json_object_array_add(trans, onetrans);
        }
    }
    json_object_object_add(root, "L_TRAN", trans);

    // Liste des états initiaux
    json_object* initials = json_object_new_array_ext(1);
    json_object_array_add(initials, json_object_new_int(A->initial));

    json_object_object_add(root, "L_INIT", initials);

    // Liste des états finaux
    json_object* finals = json_object_new_array_ext(A->nb_finals);
    for (uint i = 0; i < A->nb_finals; i++) {
        json_object_array_add(finals, json_object_new_int(A->finals[i]));
    }
    json_object_object_add(root, "L_FINA", finals);

    return root;
}

json_object* files_regexp_to_json(regexp* exp) {
    json_object* root = json_object_new_object();
    if (!root) {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return NULL;
    }
    json_object_object_add(root, "TYPE", json_object_new_string("REXP"));
    char* newexp;
    MALLOC(newexp, EXPSIZE);
    newexp[0] = 0;
    reg_to_string(exp, newexp);
    json_object_object_add(root, "REXP", json_object_new_string(newexp));
    free(newexp);
    return root;
}

nfa* files_json_to_nfa(json_object* root) {
    // Récupération de la taille de l'automate
    json_object* jsize_graph;
    if (!json_object_object_get_ex(root, "N_SIZE", &jsize_graph) || json_object_get_type(jsize_graph) != json_type_int) {
        fprintf(stderr, "Error: cannot read the size of the automaton.\n");
        return NULL;
    }
    uint size_graph = json_object_get_int(jsize_graph);

    // Récupération de l'alphabet
    json_object* jalph;
    if (!json_object_object_get_ex(root, "L_ALPH", &jalph) || json_object_get_type(jalph) != json_type_array) {
        fprintf(stderr, "Error: cannot read the alphabet.\n");
        return NULL;
    }
    uint size_alpha = json_object_array_length(jalph);

    // Création du NFA
    nfa* A = nfa_init();
    A->trans = create_lgraph_noedges(size_graph, size_alpha);
    A->state_names = NULL;

    MALLOC(A->alphabet, size_alpha);

    // Récupération des noms de lettres.
    for (uint i = 0; i < size_alpha; i++) {
        json_object* oneletter = json_object_array_get_idx(jalph, i);
        if (json_object_get_type(oneletter) == json_type_string) {
            const char* let = json_object_get_string(oneletter);
            if (let[1] == '\0') {
                A->alphabet[i].lab = let[0];
                A->alphabet[i].num = -1;
            }
            else {
                fprintf(stderr, "Error: cannot read the alphabet.\n");
                nfa_delete(A);
                return NULL;
            }
        }
        else if (json_object_get_type(oneletter) == json_type_array && json_object_array_length(oneletter) == 2) {
            json_object* lab = json_object_array_get_idx(oneletter, 0);
            json_object* num = json_object_array_get_idx(oneletter, 1);
            if (json_object_get_type(lab) != json_type_string || json_object_get_type(num) != json_type_int) {
                fprintf(stderr, "Error: cannot read the alphabet.\n");
                nfa_delete(A);
                return NULL;
            }
            const char* let = json_object_get_string(lab);
            int numi = json_object_get_int(num);
            if (let[1] == '\0') {
                A->alphabet[i].lab = let[0];
                A->alphabet[i].num = numi;
            }
            else {
                fprintf(stderr, "Error: cannot read the alphabet.\n");
                nfa_delete(A);
                return NULL;
            }
        }
        else {
            fprintf(stderr, "Error: cannot read the alphabet.\n");
            nfa_delete(A);
            return NULL;
        }
    }

    // Récupération du graphe des transitions
    json_object* jtrans;
    if (!json_object_object_get_ex(root, "L_TRAN", &jtrans) || json_object_get_type(jtrans) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of transitions.\n");
        nfa_delete(A);
        return NULL;
    }
    uint len = json_object_array_length(jtrans);

    for (uint i = 0; i < len; i++) {
        json_object* onetrans = json_object_array_get_idx(jtrans, i);
        if (json_object_get_type(onetrans) != json_type_array || json_object_array_length(onetrans) != 3) {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            nfa_delete(A);
            return NULL;
        }
        json_object* jq = json_object_array_get_idx(onetrans, 0);
        json_object* ja = json_object_array_get_idx(onetrans, 1);
        json_object* jr = json_object_array_get_idx(onetrans, 2);
        if (json_object_get_type(jq) != json_type_int || json_object_get_type(jr) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            nfa_delete(A);
            return NULL;
        }
        uint q = json_object_get_int(jq);
        uint r = json_object_get_int(jr);
        uint a = 0;

        switch (json_object_get_type(ja)) {
        case json_type_int:
            a = json_object_get_int(ja);
            break;
        case json_type_string:
            if (json_object_get_string(ja)[1] != '\0') {
                fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
                nfa_delete(A);
                return NULL;
            }
            else {
                const char* sa = json_object_get_string(ja);
                uint j = 0;
                while (j < size_alpha) {
                    if (sa[0] == A->alphabet[j].lab && A->alphabet[j].num == -1) {
                        a = j;
                        break;
                    }
                    j++;
                }
                if (j == size_alpha) {
                    fprintf(stderr, "Error: unknown letter used in the transitions.\n");
                    nfa_delete(A);
                    return NULL;
                }
            }
            break;
        case json_type_array:
            if (json_object_array_length(ja) != 2) {
                fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
                nfa_delete(A);
                return NULL;
            }
            else {
                json_object* lab = json_object_array_get_idx(ja, 0);
                json_object* num = json_object_array_get_idx(ja, 1);
                if (json_object_get_type(lab) != json_type_string || json_object_get_string(lab)[1] != '\0' || json_object_get_type(num) != json_type_int) {
                    fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
                    nfa_delete(A);
                    return NULL;
                }
                const char* sa = json_object_get_string(lab);
                int numi = json_object_get_int(num);
                uint j = 0;
                while (j < size_alpha) {
                    if (sa[0] == A->alphabet[j].lab && A->alphabet[j].num == numi) {
                        a = j;
                        break;
                    }
                    j++;
                }
                if (j == size_alpha) {
                    fprintf(stderr, "Error: unknown letter used in the transitions.\n");
                    nfa_delete(A);
                    return NULL;
                }
            }
            break;
        default:
            break;
        }

        if (q < size_graph && r < size_graph && a < size_alpha) {
            rigins_dequeue(r, A->trans->edges[q][a]);
        }
        else {
            fprintf(stderr, "Error: incorrect letter or state used in the transitions.\n");
            nfa_delete(A);
            return NULL;
        }
    }

    for (uint q = 0; q < size_graph; q++) {
        for (uint a = 0; a < size_alpha; a++) {
            sort_dequeue_norepeat(A->trans->edges[q][a]);
        }
    }

    // Récupération de la liste des états initiaux
    A->initials = create_dequeue();
    json_object* jini;
    if (!json_object_object_get_ex(root, "L_INIT", &jini) || json_object_get_type(jini) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of initial states.\n");
        nfa_delete(A);
        return NULL;
    }
    len = json_object_array_length(jini);
    for (uint i = 0; i < len; i++) {
        json_object* jq = json_object_array_get_idx(jini, i);
        if (json_object_get_type(jq) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of final states.\n");
            nfa_delete(A);
            return NULL;
        }
        uint q = json_object_get_int(jq);
        if (q < size_graph) {
            rigins_dequeue(q, A->initials);
        }
        else {
            fprintf(stderr, "Error: incorrect state in the list of initial states.\n");
            nfa_delete(A);
            return NULL;
        }
    }
    sort_dequeue_norepeat(A->initials);

    // Récupération de la liste des états finaux
    A->finals = create_dequeue();
    json_object* jfin;
    if (!json_object_object_get_ex(root, "L_FINA", &jfin) || json_object_get_type(jfin) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of final states.\n");
        nfa_delete(A);
        return NULL;
    }
    len = json_object_array_length(jfin);
    for (uint i = 0; i < len; i++) {
        json_object* jq = json_object_array_get_idx(jfin, i);
        if (json_object_get_type(jq) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of final states.\n");
            nfa_delete(A);
            return NULL;
        }
        uint q = json_object_get_int(jq);
        if (q < size_graph) {
            rigins_dequeue(q, A->finals);
        }
        else {
            fprintf(stderr, "Error: incorrect state in the list of final states.\n");
            nfa_delete(A);
            return NULL;
        }
    }
    sort_dequeue_norepeat(A->finals);

    return A;
}

/************************************/
/* Lecture/écriture dans un fichier */
/************************************/

void files_save_object(object* theobj, char* filename) {
    json_object* root = NULL;
    switch (theobj->type) {
    case REGEXP:
        root = files_regexp_to_json(theobj->exp);
        break;
    case AUTOMATON:
        if (theobj->aut->dfa) {
            root = files_dfa_to_json(theobj->aut->obj_dfa);
        }
        else {
            root = files_nfa_to_json(theobj->aut->obj_nfa);
        }
        break;
    default:
        return;
        break;
    }
    // Sauvegarde
    if (json_object_to_file(filename, root)) {
        fprintf(stderr, "Error: failed to save %s.\n", filename);
    }
    else {
        printf("%s saved.\n", filename);
    }

    // Libération du JSON
    json_object_put(root);
}

static int files_read_object_aux(json_object* root, char* varname) {
    json_object* obj_type;
    if (!json_object_object_get_ex(root, "TYPE", &obj_type) || json_object_get_type(obj_type) != json_type_string) {
        fprintf(stderr, "Error: cannot read the type of the object.\n");
        return -2;
    }
    const char* thetype = json_object_get_string(obj_type);
    if (strcmp(thetype, "AUTO") == 0) {
        fprintf(stdout, "Reading an automaton.\n");
        nfa* A = files_json_to_nfa(root);
        return object_add_automaton_nfa(varname, A);;
    }

    if (strcmp(thetype, "REXP") == 0) {
        fprintf(stdout, "Reading a regular expression.\n");
        json_object* theregexp;
        if (!json_object_object_get_ex(root, "REXP", &theregexp) || json_object_get_type(theregexp) != json_type_string) {
            fprintf(stderr, "Error: cannot read the regular expression.\n");
            return -2;
        }
        const char* exp = json_object_get_string(theregexp);
        regexp* myexp = parse_string_regexp(exp);
        if (myexp == NULL) {
            return -2;
        }
        return object_add_regexp(varname, myexp);;
    }
    return -2;
}

int files_read_object(char* filename, char* varname) {
    json_object* root = json_object_from_file(filename);
    if (!root) {
        fprintf(stderr, "Failed to read the file.\n");
        return -2;
    }
    int i = files_read_object_aux(root, varname);
    json_object_put(root);
    return i;
}

void files_save_session(char* filename) {
    // Création de l'objet json
    json_object* root = json_object_new_object();
    if (!root) {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return;
    }
    json_object_object_add(root, "TYPE", json_object_new_string("SESSION"));
    uint size = 0;
    for (int i = 0; i < nb_objects; i++) {
        {
            if (objects[i]->parent == -1) {
                size++;
            }
        }
    }

    json_object* thetable = json_object_new_array_ext(size);
    for (int i = 0; i < nb_objects; i++) {
        if (objects[i]->parent == -1) {
            json_object* oneobj = json_object_new_array_ext(2);
            json_object_array_add(oneobj, json_object_new_string(objects[i]->name));
            if (objects[i]->type == REGEXP) {
                json_object_array_add(oneobj, files_regexp_to_json(objects[i]->exp));
            }
            if (objects[i]->type == AUTOMATON) {
                if (objects[i]->aut->dfa) {
                    json_object_array_add(oneobj, files_dfa_to_json(objects[i]->aut->obj_dfa));
                }
                else {
                    json_object_array_add(oneobj, files_nfa_to_json(objects[i]->aut->obj_nfa));
                }
            }
            json_object_array_add(thetable, oneobj);
        }
    }
    json_object_object_add(root, "OBJS", thetable);
    // Sauvegarde
    if (json_object_to_file(filename, root)) {
        fprintf(stderr, "Error: failed to save %s.\n", filename);
    }
    else {
        printf("%s saved.\n", filename);
    }

    // Libération du JSON
    json_object_put(root);
}

void files_load_session(char* filename) {
    json_object* root = json_object_from_file(filename);
    if (!root) {
        fprintf(stderr, "Failed to read the file.\n");
        return;
    }
    // Récupération du type
    json_object* obj_type;
    if (!json_object_object_get_ex(root, "TYPE", &obj_type) || json_object_get_type(obj_type) != json_type_string) {
        fprintf(stderr, "Error: cannot read the type of the object.\n");
        return;
    }

    const char* thetype = json_object_get_string(obj_type);
    if (strcmp(thetype, "SESSION") != 0) {
        fprintf(stderr, "Error: not a session file.\n");
        return;
    }

    // Récupération des objets
    json_object* jobjs;
    if (!json_object_object_get_ex(root, "OBJS", &jobjs) || json_object_get_type(jobjs) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of objects.\n");
        return;
    }
    uint len = json_object_array_length(jobjs);

    for (uint i = 0; i < len; i++) {
        json_object* oneobj = json_object_array_get_idx(jobjs, i);
        if (json_object_get_type(oneobj) != json_type_array || json_object_array_length(oneobj) != 2) {
            fprintf(stderr, "Error: cannot read the list of objects.\n");
            return;
        }
        json_object* varname = json_object_array_get_idx(oneobj, 0);
        json_object* theobje = json_object_array_get_idx(oneobj, 1);

        const char* thename = json_object_get_string(varname);
        files_read_object_aux(theobje, strdup(thename));
    }

    json_object_put(root);
}
