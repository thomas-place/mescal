/*************************/
/*+ Gestion de fichiers +*/
/*************************/

#include "files.h"

/************************/
/*+ Example Generation +*/
/************************/

int files_save_exall(const char* filename, exall_profile* profile) {
    if (!filename || !profile) {
        fprintf(stderr, "Invalid parameters for saving example generation data.\n");
        return -2;
    }

    // Initialize the JSON object.
    json_object* root = json_object_new_object();
    if (!root) {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return -2;
    }
    switch (profile->mode) {
    case EXAGEN_ALL:
        json_object_object_add(root, "TYPE", json_object_new_string("EXALL"));
        break;
    case EXAGEN_FPHIERA:
        json_object_object_add(root, "TYPE", json_object_new_string("EXFP"));
        break;
    case EXAGEN_DETHIERA:
        json_object_object_add(root, "TYPE", json_object_new_string("EXDET"));
        break;
    default:
        fprintf(stderr, "Error: unknown example generation mode.\n");
        json_object_put(root);
        return -2;
    }




    // Number of states in the automaton.
    json_object_object_add(root, "NB_STATES", json_object_new_int(profile->states));

    // Number of letters in the alphabet.
    json_object_object_add(root, "NB_LETTERS", json_object_new_int(profile->alpha));

    // Initialization of the counter for the examples tested.
    json_object_object_add(root, "EX_COUNT", json_object_new_uint64(profile->done));

    // Initialization of the finished flag.
    json_object_object_add(root, "EX_FINISHED", json_object_new_boolean(profile->finished));

    // Initialization for the list of indices of the examples found.
    json_object* ex_found = json_object_new_array_ext(profile->nb_found);
    json_object_object_add(root, "EX_FOUND", ex_found);
    for (uint64_t i = 0; i < profile->nb_found; i++) {
        json_object_array_add(ex_found, json_object_new_uint64(profile->found[i]));
    }

    if (profile->mode == EXAGEN_ALL) {
        // Forbidden classes.
        json_object* cl_out = json_object_new_array_ext(profile->nblow);
        for (uint i = 0; i < profile->nblow; i++) {
            if (profile->low[i] < CL_END) {
                json_object_array_add(cl_out, json_object_new_int(profile->low[i]));
            }
            else {
                CRITICAL("Unknown class (%d)", profile->low[i]);
                json_object_put(root);
                return -2;
            }
        }
        json_object_object_add(root, "CL_OUT", cl_out);

        // Mandatory classes.
        json_object* cl_in = json_object_new_array_ext(profile->nbhigh);
        for (uint i = 0; i < profile->nbhigh; i++) {
            if (profile->high[i] < CL_END) {
                json_object_array_add(cl_in, json_object_new_int(profile->high[i]));
            }
            else {
                CRITICAL("Unknown class (%d)", profile->high[i]);
                json_object_put(root);
                return -2;
            }
        }
        json_object_object_add(root, "CL_IN", cl_in);
    }
    else {
        json_object_object_add(root, "CL_BASIS", json_object_new_int(profile->low[0]));
        json_object_object_add(root, "CL_LEVEL", json_object_new_int(profile->nblow));
    }

    if (json_object_to_file(filename, root)) {
        fprintf(stderr, "Error: failed to save %s.\n", filename);
        json_object_put(root);
        return -2;
    }
    else {
        printf("%s saved.\n", filename);
    }

    // Libération du JSON
    json_object_put(root);

    return -1;
}

int files_read_exall(const char* filename, exall_profile* profile) {
    // Read the JSON file.
    json_object* root = json_object_from_file(filename);
    if (!root) {
        fprintf(stderr, "Failed to read the file.\n");
        return -2;
    }

    // Check the type of the object.
    json_object* obj_type;
    if (!json_object_object_get_ex(root, "TYPE", &obj_type) || json_object_get_type(obj_type) != json_type_string) {
        fprintf(stderr, "Error: cannot read the type of the object.\n");
        json_object_put(root);
        return -2;
    }
    if (strcmp(json_object_get_string(obj_type), "EXALL") == 0) {
        profile->mode = EXAGEN_ALL;
    }
    else if (strcmp(json_object_get_string(obj_type), "EXFP") == 0) {
        profile->mode = EXAGEN_FPHIERA;
    }
    else if (strcmp(json_object_get_string(obj_type), "EXDET") == 0) {
        profile->mode = EXAGEN_DETHIERA;
    }
    else if (strcmp(json_object_get_string(obj_type), "EXALL") != 0) {
        fprintf(stderr, "Error: the file is not an example generation file.\n");
        json_object_put(root);
        return -2;
    }

    // Read the number of states.
    json_object* obj_nb_states;
    if (!json_object_object_get_ex(root, "NB_STATES", &obj_nb_states) || json_object_get_type(obj_nb_states) != json_type_int) {
        fprintf(stderr, "Error: cannot read the number of states.\n");
        json_object_put(root);
        return -2;
    }
    profile->states = json_object_get_int(obj_nb_states);

    // Read the number of letters.
    json_object* obj_nb_letters;
    if (!json_object_object_get_ex(root, "NB_LETTERS", &obj_nb_letters) || json_object_get_type(obj_nb_letters) != json_type_int) {
        fprintf(stderr, "Error: cannot read the number of letters.\n");
        json_object_put(root);
        return -2;
    }
    profile->alpha = json_object_get_int(obj_nb_letters);

    // Read the finished flag.
    json_object* obj_finished;
    if (!json_object_object_get_ex(root, "EX_FINISHED", &obj_finished) || json_object_get_type(obj_finished) != json_type_boolean) {
        fprintf(stderr, "Error: cannot read the finished flag.\n");
        json_object_put(root);
        return -2;
    }
    profile->finished = json_object_get_boolean(obj_finished);

    // Read the classes for the all generation mode.
    if (profile->mode == EXAGEN_ALL) {

        // Read the forbidden classes.
        json_object* obj_cl_out;
        if (!json_object_object_get_ex(root, "CL_OUT", &obj_cl_out) || json_object_get_type(obj_cl_out) != json_type_array) {
            fprintf(stderr, "Error: cannot read the forbidden classes.\n");
            json_object_put(root);
            return -2;
        }
        profile->nblow = json_object_array_length(obj_cl_out);
        if (profile->nblow > 16) {
            fprintf(stderr, "Error: too many forbidden classes (%d).\n", profile->nblow);
            json_object_put(root);
            return -2;
        }
        for (uint i = 0; i < profile->nblow; i++) {
            json_object* obj_class = json_object_array_get_idx(obj_cl_out, i);
            if (json_object_get_type(obj_class) != json_type_int) {
                fprintf(stderr, "Error: forbidden class %d is not a valid class number.\n", i);
                json_object_put(root);
                return -2;
            }
            int class_index = json_object_get_int(obj_class);
            if (class_index < 0 || class_index >= CL_END) {
                fprintf(stderr, "Error: forbidden class %d is not a valid class number.\n", i);
                json_object_put(root);
                return -2;
            }
            profile->low[i] = class_index;  // Get the index of the class.
        }

        // Read the mandatory classes.
        json_object* obj_cl_in;
        if (!json_object_object_get_ex(root, "CL_IN", &obj_cl_in) || json_object_get_type(obj_cl_in) != json_type_array) {
            fprintf(stderr, "Error: cannot read the mandatory classes.\n");
            json_object_put(root);
            return -2;
        }
        profile->nbhigh = json_object_array_length(obj_cl_in);
        if (profile->nbhigh > 16) {
            fprintf(stderr, "Error: too many mandatory classes (%d).\n", profile->nbhigh);
            json_object_put(root);
            return -2;
        }
        for (uint i = 0; i < profile->nbhigh; i++) {
            json_object* obj_class = json_object_array_get_idx(obj_cl_in, i);
            if (json_object_get_type(obj_class) != json_type_int) {
                fprintf(stderr, "Error: mandatory class %d is not a valid class number.\n", i);
                json_object_put(root);
                return -2;
            }
            int class_index = json_object_get_int(obj_class);
            if (class_index < 0 || class_index >= CL_END) {
                fprintf(stderr, "Error: mandatory class %d is not a valid class number.\n", i);
                json_object_put(root);
                return -2;
            }
            profile->high[i] = class_index;  // Get the index of the class.
        }
    }
    else {
        json_object* obj_basis;
        if (!json_object_object_get_ex(root, "CL_BASIS", &obj_basis) || json_object_get_type(obj_basis) != json_type_int) {
            fprintf(stderr, "Error: cannot read the base class.\n");
            json_object_put(root);
            return -2;
        }
        profile->low[0] = json_object_get_int(obj_basis);

        json_object* obj_level;
        if (!json_object_object_get_ex(root, "CL_LEVEL", &obj_level) || json_object_get_type(obj_level) != json_type_int) {
            fprintf(stderr, "Error: cannot read the level.\n");
            json_object_put(root);
            return -2;
        }
        profile->nblow = json_object_get_int(obj_level);
    }

    // Read the number of examples tested.
    json_object* obj_ex_count;
    if (!json_object_object_get_ex(root, "EX_COUNT", &obj_ex_count) || json_object_get_type(obj_ex_count) != json_type_int) {
        fprintf(stderr, "Error: cannot read the number of examples tested.\n");
        json_object_put(root);
        return -2;
    }
    profile->done = json_object_get_uint64(obj_ex_count);

    // Read the list of examples found.
    json_object* obj_ex_found;
    if (!json_object_object_get_ex(root, "EX_FOUND", &obj_ex_found) || json_object_get_type(obj_ex_found) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of examples found.\n");
        json_object_put(root);
        return -2;
    }
    uint64_t nb_found_temp = json_object_array_length(obj_ex_found);
    profile->nb_found = nb_found_temp;
    profile->size_found = 1;
    while (nb_found_temp > 0) {
        profile->size_found <<= 1;  // Double the size of the found array.
        nb_found_temp >>= 1;  // Divide by 2.
    }


    MALLOC(profile->found, profile->size_found);
    for (uint i = 0; i < profile->nb_found; i++) {
        json_object* obj_index = json_object_array_get_idx(obj_ex_found, i);
        if (json_object_get_type(obj_index) != json_type_int) {
            fprintf(stderr, "Error: example index %d is not an integer.\n", i);
            json_object_put(root);
            free(profile->found);
            profile->found = NULL;
            return -2;
        }
        profile->found[i] = json_object_get_uint64(obj_index);
    }





    json_object_put(root);
    return -1;

}




/*************************/
/*+ Regular expressions +*/
/*************************/

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

    // Number of states in the automaton.
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

    // List of transitions (we start by counting the number of transitions)
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

    // List of epsilon transitions (if any)
    if (A->trans_e) {
        uint num_etrans = 0;
        for (uint q = 0; q < A->trans_e->size; q++) {
            num_trans = num_trans + size_dequeue(A->trans_e->edges[q]);
        }


        json_object* etrans = json_object_new_array_ext(num_etrans);
        for (uint q = 0; q < A->trans_e->size; q++) {
            for (uint i = 0; i < size_dequeue(A->trans_e->edges[q]); i++) {
                json_object* onetrans = json_object_new_array_ext(2);
                json_object_array_add(onetrans, json_object_new_int(q));
                json_object_array_add(onetrans, json_object_new_int(lefread_dequeue(A->trans_e->edges[q], i)));
                json_object_array_add(etrans, onetrans);
            }
        }
        json_object_object_add(root, "L_ETRAN", etrans);
    }


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

// nfa* files_json_to_nfa(json_object* root) {
//     // Récupération de la taille de l'automate
//     json_object* jsize_graph;
//     if (!json_object_object_get_ex(root, "N_SIZE", &jsize_graph) || json_object_get_type(jsize_graph) != json_type_int) {
//         fprintf(stderr, "Error: cannot read the size of the automaton.\n");
//         return NULL;
//     }
//     uint size_graph = json_object_get_int(jsize_graph);

//     // Récupération de l'alphabet
//     json_object* jalph;
//     if (!json_object_object_get_ex(root, "L_ALPH", &jalph) || json_object_get_type(jalph) != json_type_array) {
//         fprintf(stderr, "Error: cannot read the alphabet.\n");
//         return NULL;
//     }
//     uint size_alpha = json_object_array_length(jalph);

//     // Création du NFA
//     nfa* A = nfa_init();
//     A->trans = create_lgraph_noedges(size_graph, size_alpha);
//     A->state_names = NULL;

//     MALLOC(A->alphabet, size_alpha);

//     // Récupération des noms de lettres.
//     for (uint i = 0; i < size_alpha; i++) {
//         json_object* oneletter = json_object_array_get_idx(jalph, i);
//         if (json_object_get_type(oneletter) == json_type_string) {
//             const char* let = json_object_get_string(oneletter);
//             if (let[1] == '\0') {
//                 A->alphabet[i].lab = let[0];
//                 A->alphabet[i].num = -1;
//             }
//             else {
//                 fprintf(stderr, "Error: cannot read the alphabet.\n");
//                 nfa_delete(A);
//                 return NULL;
//             }
//         }
//         else if (json_object_get_type(oneletter) == json_type_array && json_object_array_length(oneletter) == 2) {
//             json_object* lab = json_object_array_get_idx(oneletter, 0);
//             json_object* num = json_object_array_get_idx(oneletter, 1);
//             if (json_object_get_type(lab) != json_type_string || json_object_get_type(num) != json_type_int) {
//                 fprintf(stderr, "Error: cannot read the alphabet.\n");
//                 nfa_delete(A);
//                 return NULL;
//             }
//             const char* let = json_object_get_string(lab);
//             int numi = json_object_get_int(num);
//             if (let[1] == '\0') {
//                 A->alphabet[i].lab = let[0];
//                 A->alphabet[i].num = numi;
//             }
//             else {
//                 fprintf(stderr, "Error: cannot read the alphabet.\n");
//                 nfa_delete(A);
//                 return NULL;
//             }
//         }
//         else {
//             fprintf(stderr, "Error: cannot read the alphabet.\n");
//             nfa_delete(A);
//             return NULL;
//         }
//     }

//     // Récupération du graphe des transitions
//     json_object* jtrans;
//     if (!json_object_object_get_ex(root, "L_TRAN", &jtrans) || json_object_get_type(jtrans) != json_type_array) {
//         fprintf(stderr, "Error: cannot read the list of transitions.\n");
//         nfa_delete(A);
//         return NULL;
//     }
//     uint len = json_object_array_length(jtrans);

//     for (uint i = 0; i < len; i++) {
//         json_object* onetrans = json_object_array_get_idx(jtrans, i);
//         if (json_object_get_type(onetrans) != json_type_array || json_object_array_length(onetrans) != 3) {
//             fprintf(stderr, "Error: cannot read the list of transitions.\n");
//             nfa_delete(A);
//             return NULL;
//         }
//         json_object* jq = json_object_array_get_idx(onetrans, 0);
//         json_object* ja = json_object_array_get_idx(onetrans, 1);
//         json_object* jr = json_object_array_get_idx(onetrans, 2);
//         if (json_object_get_type(jq) != json_type_int || json_object_get_type(jr) != json_type_int) {
//             fprintf(stderr, "Error: cannot read the list of transitions.\n");
//             nfa_delete(A);
//             return NULL;
//         }
//         uint q = json_object_get_int(jq);
//         uint r = json_object_get_int(jr);
//         uint a = 0;

//         switch (json_object_get_type(ja)) {
//         case json_type_int:
//             a = json_object_get_int(ja);
//             break;
//         case json_type_string:
//             if (json_object_get_string(ja)[1] != '\0') {
//                 fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
//                 nfa_delete(A);
//                 return NULL;
//             }
//             else {
//                 const char* sa = json_object_get_string(ja);
//                 uint j = 0;
//                 while (j < size_alpha) {
//                     if (sa[0] == A->alphabet[j].lab && A->alphabet[j].num == -1) {
//                         a = j;
//                         break;
//                     }
//                     j++;
//                 }
//                 if (j == size_alpha) {
//                     fprintf(stderr, "Error: unknown letter used in the transitions.\n");
//                     nfa_delete(A);
//                     return NULL;
//                 }
//             }
//             break;
//         case json_type_array:
//             if (json_object_array_length(ja) != 2) {
//                 fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
//                 nfa_delete(A);
//                 return NULL;
//             }
//             else {
//                 json_object* lab = json_object_array_get_idx(ja, 0);
//                 json_object* num = json_object_array_get_idx(ja, 1);
//                 if (json_object_get_type(lab) != json_type_string || json_object_get_string(lab)[1] != '\0' || json_object_get_type(num) != json_type_int) {
//                     fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
//                     nfa_delete(A);
//                     return NULL;
//                 }
//                 const char* sa = json_object_get_string(lab);
//                 int numi = json_object_get_int(num);
//                 uint j = 0;
//                 while (j < size_alpha) {
//                     if (sa[0] == A->alphabet[j].lab && A->alphabet[j].num == numi) {
//                         a = j;
//                         break;
//                     }
//                     j++;
//                 }
//                 if (j == size_alpha) {
//                     fprintf(stderr, "Error: unknown letter used in the transitions.\n");
//                     nfa_delete(A);
//                     return NULL;
//                 }
//             }
//             break;
//         default:
//             break;
//         }

//         if (q < size_graph && r < size_graph && a < size_alpha) {
//             rigins_dequeue(r, A->trans->edges[q][a]);
//         }
//         else {
//             fprintf(stderr, "Error: incorrect letter or state used in the transitions.\n");
//             nfa_delete(A);
//             return NULL;
//         }
//     }

//     for (uint q = 0; q < size_graph; q++) {
//         for (uint a = 0; a < size_alpha; a++) {
//             sort_dequeue_norepeat(A->trans->edges[q][a]);
//         }
//     }

//     // Récupération de la liste des états initiaux
//     A->initials = create_dequeue();
//     json_object* jini;
//     if (!json_object_object_get_ex(root, "L_INIT", &jini) || json_object_get_type(jini) != json_type_array) {
//         fprintf(stderr, "Error: cannot read the list of initial states.\n");
//         nfa_delete(A);
//         return NULL;
//     }
//     len = json_object_array_length(jini);
//     for (uint i = 0; i < len; i++) {
//         json_object* jq = json_object_array_get_idx(jini, i);
//         if (json_object_get_type(jq) != json_type_int) {
//             fprintf(stderr, "Error: cannot read the list of final states.\n");
//             nfa_delete(A);
//             return NULL;
//         }
//         uint q = json_object_get_int(jq);
//         if (q < size_graph) {
//             rigins_dequeue(q, A->initials);
//         }
//         else {
//             fprintf(stderr, "Error: incorrect state in the list of initial states.\n");
//             nfa_delete(A);
//             return NULL;
//         }
//     }
//     sort_dequeue_norepeat(A->initials);

//     // Récupération de la liste des états finaux
//     A->finals = create_dequeue();
//     json_object* jfin;
//     if (!json_object_object_get_ex(root, "L_FINA", &jfin) || json_object_get_type(jfin) != json_type_array) {
//         fprintf(stderr, "Error: cannot read the list of final states.\n");
//         nfa_delete(A);
//         return NULL;
//     }
//     len = json_object_array_length(jfin);
//     for (uint i = 0; i < len; i++) {
//         json_object* jq = json_object_array_get_idx(jfin, i);
//         if (json_object_get_type(jq) != json_type_int) {
//             fprintf(stderr, "Error: cannot read the list of final states.\n");
//             nfa_delete(A);
//             return NULL;
//         }
//         uint q = json_object_get_int(jq);
//         if (q < size_graph) {
//             rigins_dequeue(q, A->finals);
//         }
//         else {
//             fprintf(stderr, "Error: incorrect state in the list of final states.\n");
//             nfa_delete(A);
//             return NULL;
//         }
//     }
//     sort_dequeue_norepeat(A->finals);

//     return A;
// }

typedef struct {
    uint in;
    uint lab;
    uint out;
} filetr;


static int filetr_compare(const void* t1, const void* t2) {
    const filetr* f1 = (const filetr*)t1;
    const filetr* f2 = (const filetr*)t2;

    if (f1->in < f2->in) {
        return -1;
    }
    else if (f1->in > f2->in) {
        return 1;
    }
    else if (f1->lab < f2->lab) {
        return -1;
    }
    else if (f1->lab > f2->lab) {
        return 1;
    }
    else if (f1->out < f2->out) {
        return -1;
    }
    else if (f1->out > f2->out) {
        return 1;
    }
    return 0;
}

static uint filetr_elimrepeats(filetr* trans, uint size) {
    uint d = 0;
    for (uint i = 1; i < size; i++) {
        if (trans[i].in == trans[i - 1].in && trans[i].lab == trans[i - 1].lab && trans[i].out == trans[i - 1].out) {
            d++;
        }
        trans[i - d] = trans[i];
    }
    return size - d;
}

void* files_json_to_automaton(json_object* root, bool* is_dfa) {
    // Retrieving the size of the automaton
    json_object* jsize_graph;
    if (!json_object_object_get_ex(root, "N_SIZE", &jsize_graph) || json_object_get_type(jsize_graph) != json_type_int) {
        fprintf(stderr, "Error: cannot read the size of the automaton.\n");
        return NULL;
    }
    uint size_graph = json_object_get_int(jsize_graph);

    // Retrieving the size of the alphabet
    json_object* jalph;
    if (!json_object_object_get_ex(root, "L_ALPH", &jalph) || json_object_get_type(jalph) != json_type_array) {
        fprintf(stderr, "Error: cannot read the alphabet.\n");
        return NULL;
    }
    uint size_alpha = json_object_array_length(jalph);



    // Retrieving the letter names.
    letter* alphabet = NULL;
    MALLOC(alphabet, size_alpha);

    for (uint i = 0; i < size_alpha; i++) {
        json_object* oneletter = json_object_array_get_idx(jalph, i);
        if (json_object_get_type(oneletter) == json_type_string) {
            const char* let = json_object_get_string(oneletter);
            if (let[1] == '\0') {
                alphabet[i].lab = let[0];
                alphabet[i].num = -1;
            }
            else {
                fprintf(stderr, "Error: cannot read the alphabet.\n");
                free(alphabet);
                return NULL;
            }
        }
        else if (json_object_get_type(oneletter) == json_type_array && json_object_array_length(oneletter) == 2) {
            json_object* lab = json_object_array_get_idx(oneletter, 0);
            json_object* num = json_object_array_get_idx(oneletter, 1);
            if (json_object_get_type(lab) != json_type_string || json_object_get_type(num) != json_type_int) {
                fprintf(stderr, "Error: cannot read the alphabet.\n");
                free(alphabet);
                return NULL;
            }
            const char* let = json_object_get_string(lab);
            int numi = json_object_get_int(num);
            if (let[1] == '\0') {
                alphabet[i].lab = let[0];
                alphabet[i].num = numi;
            }
            else {
                fprintf(stderr, "Error: cannot read the alphabet.\n");
                free(alphabet);
                return NULL;
            }
        }
        else {
            fprintf(stderr, "Error: cannot read the alphabet.\n");
            free(alphabet);
            return NULL;
        }
    }

    // Retrivieng the transitions
    json_object* jtrans;
    if (!json_object_object_get_ex(root, "L_TRAN", &jtrans) || json_object_get_type(jtrans) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of transitions.\n");
        free(alphabet);
        return NULL;
    }

    uint len = json_object_array_length(jtrans);
    filetr* trans;
    MALLOC(trans, len + 1);
    uint id = 0;

    for (uint i = 0; i < len; i++) {
        json_object* onetrans = json_object_array_get_idx(jtrans, i);
        if (json_object_get_type(onetrans) != json_type_array || json_object_array_length(onetrans) != 3) {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            free(alphabet);
            free(trans);
            return NULL;
        }
        json_object* jq = json_object_array_get_idx(onetrans, 0);
        json_object* ja = json_object_array_get_idx(onetrans, 1);
        json_object* jr = json_object_array_get_idx(onetrans, 2);
        if (json_object_get_type(jq) != json_type_int || json_object_get_type(jr) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            free(alphabet);
            free(trans);
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
                free(alphabet);
                free(trans);
                return NULL;
            }
            else {
                const char* sa = json_object_get_string(ja);
                uint j = 0;
                while (j < size_alpha) {
                    if (sa[0] == alphabet[j].lab && alphabet[j].num == -1) {
                        a = j;
                        break;
                    }
                    j++;
                }
                if (j == size_alpha) {
                    fprintf(stderr, "Error: unknown letter used in the transitions.\n");
                    free(alphabet);
                    free(trans);
                    return NULL;
                }
            }
            break;
        case json_type_array:
            if (json_object_array_length(ja) != 2) {
                fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
                free(alphabet);
                free(trans);
                return NULL;
            }
            else {
                json_object* lab = json_object_array_get_idx(ja, 0);
                json_object* num = json_object_array_get_idx(ja, 1);
                if (json_object_get_type(lab) != json_type_string || json_object_get_string(lab)[1] != '\0' || json_object_get_type(num) != json_type_int) {
                    fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
                    free(alphabet);
                    free(trans);
                    return NULL;
                }
                const char* sa = json_object_get_string(lab);
                int numi = json_object_get_int(num);
                uint j = 0;
                while (j < size_alpha) {
                    if (sa[0] == alphabet[j].lab && alphabet[j].num == numi) {
                        a = j;
                        break;
                    }
                    j++;
                }
                if (j == size_alpha) {
                    fprintf(stderr, "Error: unknown letter used in the transitions.\n");
                    free(alphabet);
                    free(trans);
                    return NULL;
                }
            }
            break;
        default:
            break;
        }

        if (q < size_graph && r < size_graph && a < size_alpha) {
            trans[id].in = q;
            trans[id].lab = a;
            trans[id].out = r;
            id++;
        }
        else {
            fprintf(stderr, "Error: incorrect letter or state used in the transitions.\n");
            free(alphabet);
            free(trans);
            return NULL;
        }
    }

    // Sorting the transitions and eliminating duplicates.
    qsort(trans, len, sizeof(filetr), filetr_compare);
    len = filetr_elimrepeats(trans, len);


    // Retrivieng the epsilon transitions (if any)
    json_object* jetrans;
    uint elen = 0;
    filetr* etrans = NULL;
    if (json_object_object_get_ex(root, "L_ETRAN", &jetrans)) {
        if (json_object_get_type(jetrans) != json_type_array) {
            fprintf(stderr, "Error: cannot read the list of epsilon transitions.\n");
            free(alphabet);
            free(trans);
            return NULL;
        }
        elen = json_object_array_length(jetrans);
        MALLOC(etrans, elen + 1);
        id = 0;

        for (uint i = 0; i < elen; i++) {
            json_object* onetrans = json_object_array_get_idx(jetrans, i);
            if (json_object_get_type(onetrans) != json_type_array || json_object_array_length(onetrans) != 2) {
                fprintf(stderr, "Error: cannot read the list of epsilon transitions.\n");
                free(alphabet);
                free(trans);
                free(etrans);
                return NULL;
            }
            json_object* jq = json_object_array_get_idx(onetrans, 0);
            json_object* jr = json_object_array_get_idx(onetrans, 1);
            if (json_object_get_type(jq) != json_type_int || json_object_get_type(jr) != json_type_int) {
                fprintf(stderr, "Error: cannot read the list of epsilon transitions.\n");
                free(alphabet);
                free(trans);
                free(etrans);
                return NULL;
            }
            uint q = json_object_get_int(jq);
            uint r = json_object_get_int(jr);

            if (q < size_graph && r < size_graph) {
                etrans[id].in = q;
                etrans[id].lab = 0; // Epsilon transitions have no label.
                etrans[id].out = r;
                id++;
            }
            else {
                fprintf(stderr, "Error: incorrect letter or state used in the epsilon transitions.\n");
                free(alphabet);
                free(etrans);
                free(trans);
                return NULL;
            }
        }

        // Sorting the transitions and eliminating duplicates.
        qsort(etrans, elen, sizeof(filetr), filetr_compare);
        elen = filetr_elimrepeats(etrans, elen);
    }


    // Récupération de la liste des états initiaux
    json_object* jini;
    if (!json_object_object_get_ex(root, "L_INIT", &jini) || json_object_get_type(jini) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of initial states.\n");
        free(alphabet);
        free(trans);
        free(etrans);
        return NULL;
    }
    uint inilen = json_object_array_length(jini);
    uint* initials = NULL;
    id = 0;
    MALLOC(initials, inilen + 1);
    for (uint i = 0; i < inilen; i++) {
        json_object* jq = json_object_array_get_idx(jini, i);
        if (json_object_get_type(jq) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of final states.\n");
            free(alphabet);
            free(trans);
            free(etrans);
            free(initials);
            return NULL;
        }
        uint q = json_object_get_int(jq);
        if (q < size_graph) {
            initials[id] = q;
            id++;
        }
        else {
            fprintf(stderr, "Error: incorrect state in the list of initial states.\n");
            free(alphabet);
            free(trans);
            free(etrans);
            free(initials);
            return NULL;
        }
    }
    inilen = array_sort_norepeat_uint(initials, inilen);

    // Récupération de la liste des états finaux
    json_object* jfin;
    if (!json_object_object_get_ex(root, "L_FINA", &jfin) || json_object_get_type(jfin) != json_type_array) {
        fprintf(stderr, "Error: cannot read the list of final states.\n");
        free(alphabet);
        free(trans);
        free(etrans);
        free(initials);
        return NULL;
    }
    uint finlen = json_object_array_length(jfin);
    uint* finals;
    MALLOC(finals, finlen + 1);
    id = 0;
    for (uint i = 0; i < finlen; i++) {
        json_object* jq = json_object_array_get_idx(jfin, i);
        if (json_object_get_type(jq) != json_type_int) {
            fprintf(stderr, "Error: cannot read the list of final states.\n");
            free(alphabet);
            free(trans);
            free(etrans);
            free(initials);
            return NULL;
        }
        uint q = json_object_get_int(jq);
        if (q < size_graph) {
            finals[id] = q;
            id++;
        }
        else {
            fprintf(stderr, "Error: incorrect state in the list of final states.\n");
            free(alphabet);
            free(trans);
            free(etrans);
            free(initials);
            free(finals);
            return NULL;
        }
    }
    finlen = array_sort_norepeat_uint(finals, finlen);

    // The automaton is a DFA if there is only one initial state and the number of transitions is equal to the size of the graph times the size of the alphabet.
    if (inilen == 1 && len == size_graph * size_alpha && elen == 0) {
        // This is a DFA.
        dfa* thedfa = NULL;
        CALLOC(thedfa, 1);
        thedfa->alphabet = alphabet;
        thedfa->initial = initials[0];
        free(initials);
        thedfa->nb_finals = finlen;
        MALLOC(thedfa->finals, finlen);
        for (uint i = 0; i < finlen; i++) {
            thedfa->finals[i] = finals[i];
        }
        free(finals);
        thedfa->trans = create_dgraph_noedges(size_graph, size_alpha);
        for (uint i = 0; i < len; i++) {
            thedfa->trans->storage[i] = trans[i].out;
        }
        free(trans);
        *is_dfa = true;
        return thedfa;
    }
    else {
        // This is an NFA.
        nfa* thenfa = NULL;
        CALLOC(thenfa, 1);
        thenfa->alphabet = alphabet;
        thenfa->initials = create_dequeue();
        for (uint i = 0; i < inilen; i++) {
            rigins_dequeue(initials[i], thenfa->initials);
        }
        free(initials);
        thenfa->finals = create_dequeue();
        for (uint i = 0; i < finlen; i++) {
            rigins_dequeue(finals[i], thenfa->finals);
        }
        free(finals);
        thenfa->trans = create_lgraph_noedges(size_graph, size_alpha);
        uint j = 0;
        for (uint q = 0; q < size_graph; q++) {
            for (uint a = 0; a < size_alpha; a++) {
                while (trans[j].in == q && trans[j].lab == a && j < len) {
                    rigins_dequeue(trans[j].out, thenfa->trans->edges[q][a]);
                    j++;
                }
            }
            if (j >= len) {
                break;
            }
        }
        free(trans);
        if (elen > 0) {
            thenfa->trans_e = create_graph_noedges(size_graph);
            j = 0;
            for (uint q = 0; q < size_graph; q++) {
                while (etrans[j].in == q && j < elen) {
                    rigins_dequeue(etrans[j].out, thenfa->trans_e->edges[q]);
                    j++;
                }
                if (j >= elen) {
                    break;
                }
            }
            free(etrans);
        }
        else {
            thenfa->trans_e = NULL;
        }

        *is_dfa = false;
        return thenfa;
    }
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
        bool is_dfa = false;
        void* A = files_json_to_automaton(root, &is_dfa);
        if (!A) {
            fprintf(stderr, "Error: cannot read the automaton.\n");
            return -2;
        }
        if (is_dfa) {
            dfa* thedfa = (dfa*)A;
            return object_add_automaton_dfa(varname, thedfa);
        }
        else {
            nfa* thenfa = (nfa*)A;
            return object_add_automaton_nfa(varname, thenfa);
        }

        //nfa* A = files_json_to_nfa(root);
        //return object_add_automaton_nfa(varname, A);;
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
