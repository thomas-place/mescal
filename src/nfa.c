/***************************/
/* Implémentation des NFAs */
/***************************/

#include "nfa.h"
#include "error.h"
#include "printing.h"




/***************/
/* State names */
/***************/

void nfa_print_state(const nfa* A, uint q, FILE* out) {
    if (!A || !out) {
        return;
    }
    if (A->state_names) {
        // fprintf(out, "%d", q);
        fprintf(out, "%s", A->state_names[q]);
    }
    else {
        fprintf(out, "%d", q);
    }
}

void dfa_print_state(const dfa* A, uint q, FILE* out) {
    if (!A || !out || q >= A->trans->size_graph) {
        return;
    }
    if (A->state_names) {
        fprintf(out, "%s", A->state_names[q]);
    }
    else {
        fprintf(out, "%d", q);
    }
}

void nfa_reset_state_names(nfa* A) {
    if (!A || !A->state_names) {
        return;
    }

    for (uint i = 0; i < A->trans->size_graph; i++) {
        // printf("%d %s\n", A->trans->size_graph, A->state_names[i]);
        free(A->state_names[i]);
    }
    free(A->state_names);
    A->state_names = NULL;
}

void dfa_reset_state_names(dfa* A) {
    if (!A || !A->state_names) {
        return;
    }

    for (uint i = 0; i < A->trans->size_graph; i++) {
        free(A->state_names[i]);
    }
    free(A->state_names);
    A->state_names = NULL;
}

char** copy_all_names(char** names, uint size) {
    if (!names) {
        return NULL;
    }
    char** new_names;
    MALLOC(new_names, size);
    for (uint q = 0; q < size; q++) {
        new_names[q] = strdup(names[q]);
    }
    return new_names;
}



/**********************/
/*+ Copy and release +*/
/**********************/


void nfa_delete(nfa* A) {
    if (A == NULL) {
        return;
    }

    // Reseting the names first (the graph is needed)
    nfa_reset_state_names(A);

    delete_lgraph(A->trans);

    delete_dequeue(A->initials);

    delete_dequeue(A->finals);

    free(A->alphabet);

    if (A->trans_e) {
        delete_graph(A->trans_e);
    }

    if (A->trans_i) {
        delete_lgraph(A->trans_i);
    }

    free(A);
}

void dfa_delete(dfa* A) {
    if (!A) {
        return;
    }
    free(A->alphabet);
    free(A->finals);
    dfa_reset_state_names(A);
    delete_dgraph(A->trans);
    free(A);
}


nfa* nfa_copy(nfa* A) {
    if (!A) {
        return NULL;
    }
    nfa* B;
    CALLOC(B, 1);

    // Copie du graphe des transitions.
    B->trans = copy_lgraph(A->trans);

    if (A->trans_e) {
        B->trans_e = copy_graph(A->trans_e);
    }

    if (A->trans_i) {
        B->trans_i = copy_lgraph(A->trans_i);
    }

    // Initiaux
    B->initials = create_dequeue();
    copy_dequeue_right(B->initials, A->initials, 0);

    // Finaux
    B->finals = create_dequeue();
    copy_dequeue_right(B->finals, A->finals, 0);

    // Noms des lettres
    B->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);

    // Noms des états
    B->state_names = copy_all_names(A->state_names, A->trans->size_graph);

    return B;
}

dfa* dfa_copy(dfa* A) {
    if (!A) {
        return NULL;
    }
    //dfa* B = dfa_init(A->trans->size_graph, A->trans->size_alpha, A->nb_finals, A->alphabet);
    dfa* B;
    CALLOC(B, 1);
    B->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    B->trans = copy_dgraph(A->trans);
    B->initial = A->initial;
    B->nb_finals = A->nb_finals;
    if (B->nb_finals == 0) {
        B->finals = NULL;
    }
    else {
        MALLOC(B->finals, A->nb_finals);
        for (uint i = 0; i < A->nb_finals; i++) {
            B->finals[i] = A->finals[i];
        }
    }

    B->state_names = copy_all_names(A->state_names, A->trans->size_graph);
    return B;
}




static letter* merge_alphabet(letter* alpha1, uint size1, letter* alpha2, uint size2, uint* newsize) {
    uint i = 0, j = 0, c = 0;
    while (i < size1 && j < size2) {
        if (compare_letters(&alpha1[i], &alpha2[j]) < 0) {
            i++;
        }
        else if (compare_letters(&alpha1[i], &alpha2[j]) > 0) {
            j++;
        }
        else {
            i++;
            j++;
        }
        c++;
    }
    *newsize = c + (size1 - i) + (size2 - j);

    letter* newalpha;
    MALLOC(newalpha, *newsize);

    uint k = 0;
    i = 0;
    j = 0;
    while (i < size1 || j < size2) {
        if (i < size1 && j < size2) {
            if (compare_letters(&alpha1[i], &alpha2[j]) < 0) {
                newalpha[k] = alpha1[i];
                i++;
            }
            else if (compare_letters(&alpha1[i], &alpha2[j]) == 0) {
                newalpha[k] = alpha1[i];
                i++;
                j++;
            }
            else {
                newalpha[k] = alpha2[j];
                j++;
            }
        }
        else if (i < size1) {
            newalpha[k] = alpha1[i];
            i++;
        }
        else {
            newalpha[k] = alpha2[j];
            j++;
        }
        k++;
    }

    return newalpha;
}


nfa* nfa_copy_exalpha(nfa* A, letter* alpha, uint size) {
    if (!A) {
        return NULL;
    }
    nfa* B;
    CALLOC(B, 1);

    uint alpha_size;
    B->alphabet = merge_alphabet(A->alphabet, A->trans->size_alpha, alpha, size, &alpha_size);


    // Copie du graphe des transitions.
    B->trans = create_lgraph_noedges(A->trans->size_graph, alpha_size);
    uint num = 0;
    for (uint a = 0; a < B->trans->size_alpha; a++) {
        if (num < A->trans->size_alpha && compare_letters(&A->alphabet[num], &B->alphabet[a]) == 0) {
            for (uint q = 0; q < B->trans->size_graph; q++) {
                copy_dequeue_right(B->trans->edges[q][a], A->trans->edges[q][num], 0);
            }
            num++;
        }
    }

    if (A->trans_e) {
        B->trans_e = create_graph_noedges(A->trans_e->size);
        for (uint q = 0; q < B->trans_e->size; q++) {
            copy_dequeue_right(B->trans_e->edges[q], A->trans_e->edges[q], 0);
        }
    }

    if (A->trans_i) {
        B->trans_i = create_lgraph_noedges(A->trans_i->size_graph, alpha_size);
        for (uint a = 0; a < B->trans_i->size_alpha; a++) {
            if (num < A->trans_i->size_alpha && compare_letters(&A->alphabet[num], &B->alphabet[a]) == 0) {
                for (uint q = 0; q < B->trans_i->size_graph; q++) {
                    copy_dequeue_right(B->trans_i->edges[q][a], A->trans_i->edges[q][num], 0);
                }
                num++;
            }
        }

    }

    // Initiaux
    B->initials = create_dequeue();
    copy_dequeue_right(B->initials, A->initials, 0);

    // Finals
    B->finals = create_dequeue();
    copy_dequeue_right(B->finals, A->finals, 0);

    // Copies des noms d'états
    B->state_names = copy_all_names(A->state_names, A->trans->size_graph);
    return B;
}

dfa* dfa_copy_exalpha(dfa* A, letter* alpha, uint size) {
    if (!A) {
        return NULL;
    }

    dfa* C;
    CALLOC(C, 1);


    uint alphasize;
    C->alphabet = merge_alphabet(A->alphabet, A->trans->size_alpha, alpha, size, &alphasize);

    uint statesize;
    if (alphasize == A->trans->size_alpha) {
        statesize = A->trans->size_graph;
    }
    else {
        statesize = A->trans->size_graph + 1;
    }

    C->trans = create_dgraph_noedges(statesize, alphasize);
    C->nb_finals = A->nb_finals;
    if (C->nb_finals == 0) {
        C->finals = NULL;
    }
    else {
        MALLOC(C->finals, C->nb_finals);
    }
    C->initial = A->initial;
    for (uint i = 0; i < C->nb_finals; i++) {
        C->finals[i] = A->finals[i];
    }

    uint num = 0;
    for (uint a = 0; a < C->trans->size_alpha; a++) {
        if (num < A->trans->size_alpha && compare_letters(&A->alphabet[num], &C->alphabet[a]) == 0) {
            for (uint q = 0; q < A->trans->size_graph; q++) {
                C->trans->edges[q][a] = A->trans->edges[q][num];
            }
            num++;
        }
        else {
            for (uint q = 0; q < A->trans->size_graph; q++) {
                C->trans->edges[q][a] = A->trans->size_graph; // Sink state.
            }
        }
    }

    if (alphasize != A->trans->size_alpha) {
        for (uint a = 0; a < C->trans->size_alpha; a++) {
            C->trans->edges[A->trans->size_graph][a] = A->trans->size_graph;
        }
    }

    C->state_names = copy_all_names(A->state_names, A->trans->size_graph);

    return C;
}


void nfa_overwrite(nfa* A, nfa* B) {
    if (A == NULL || B == NULL) {
        return;
    }

    delete_lgraph(A->trans);
    A->trans = B->trans;
    if (A->trans_e) {
        delete_graph(A->trans_e);
        A->trans_e = B->trans_e;
    }

    if (A->trans_i) {
        delete_lgraph(A->trans_i);
        A->trans_i = B->trans_i;
    }

    delete_dequeue(A->initials);
    A->initials = B->initials;
    delete_dequeue(A->finals);
    A->finals = B->finals;
    free(A->alphabet);
    A->alphabet = B->alphabet;
    nfa_reset_state_names(A);
    A->state_names = B->state_names;


    free(B);
}

/**************************************************************/
/*+ Computation of basic NFAs (used in Thompson's algorithm) +*/
/**************************************************************/


nfa* create_emptylang(void) {
    nfa* A;
    CALLOC(A, 1);
    A->initials = create_dequeue();
    A->finals = create_dequeue();
    A->trans = create_lgraph_noedges(0, 0);
    return A;
}

nfa* create_sing_epsilon(void) {
    nfa* A;
    CALLOC(A, 1);
    A->initials = create_dequeue();
    A->finals = create_dequeue();
    A->trans = create_lgraph_noedges(1, 0);
    lefins_dequeue(0, A->initials);
    lefins_dequeue(0, A->finals);
    return A;
}

nfa* create_sing_letter(letter the_letter) {
    nfa* A;
    CALLOC(A, 1);
    A->initials = create_dequeue();
    A->finals = create_dequeue();
    A->trans = create_lgraph_noedges(2, 1);
    lefins_dequeue(0, A->initials);
    lefins_dequeue(1, A->finals);
    lefins_dequeue(1, A->trans->edges[0][0]);
    MALLOC(A->alphabet, 1);
    A->alphabet[0] = the_letter;
    return A;
}

nfa* create_sing_word(word* the_word) {
    nfa* A;
    CALLOC(A, 1);
    A->initials = create_dequeue();
    A->finals = create_dequeue();
    lefins_dequeue(0, A->initials);
    lefins_dequeue(size_word(the_word), A->finals);
    uint alpha_size;
    A->alphabet = get_alphabet_word(the_word, &alpha_size);
    A->trans = create_lgraph_noedges(size_word(the_word) + 1, alpha_size);
    for (uint i = 0; i < size_word(the_word); i++) {
        letter l = lefread_word(the_word, i);
        letter* p = bsearch(&l, A->alphabet, alpha_size, sizeof(letter), compare_letters);
        lefins_dequeue(i + 1, A->trans->edges[i][p - A->alphabet]);
    }
    return A;
}

/*******************************/
/*+ Simple operations on NFAs +*/
/*******************************/



static void unify_alphabets(void* I1, bool is_dfa_I1, void* I2, bool is_dfa_I2, nfa** A, nfa** B) {
    if (!I1 || !I2 || !A || !B) {
        return;
    }
    // We retrieve the alphabets.
    uint size_alpha1;
    uint size_alpha2;
    letter* alpha1 = NULL;
    letter* alpha2 = NULL;

    if (is_dfa_I1) {
        alpha1 = ((dfa*)I1)->alphabet;
        size_alpha1 = ((dfa*)I1)->trans->size_alpha;
    }
    else {
        alpha1 = ((nfa*)I1)->alphabet;
        size_alpha1 = ((nfa*)I1)->trans->size_alpha;
    }
    if (is_dfa_I2) {
        alpha2 = ((dfa*)I2)->alphabet;
        size_alpha2 = ((dfa*)I2)->trans->size_alpha;
    }
    else {
        alpha2 = ((nfa*)I2)->alphabet;
        size_alpha2 = ((nfa*)I2)->trans->size_alpha;
    }


    if (is_dfa_I1) {
        *A = dfa_to_nfa_exalpha((dfa*)I1, alpha2, size_alpha2);
    }
    else {
        *A = nfa_copy_exalpha((nfa*)I1, alpha2, size_alpha2);
    }
    if (is_dfa_I2) {
        *B = dfa_to_nfa_exalpha((dfa*)I2, alpha1, size_alpha1);
    }
    else {
        *B = nfa_copy_exalpha((nfa*)I2, alpha1, size_alpha1);
    }
}


nfa* nfa_union(void* I1, bool is_dfa_I1, void* I2, bool is_dfa_I2) {
    if (!I1 || !I2) {
        return NULL;
    }

    nfa* A = NULL;
    nfa* B = NULL;
    unify_alphabets(I1, is_dfa_I1, I2, is_dfa_I2, &A, &B);

    nfa* UNION;
    CALLOC(UNION, 1);
    UNION->initials = create_dequeue();
    UNION->finals = create_dequeue();


    uint lag[2];

    if (A->trans_e || B->trans_e) {
        if (!A->trans_e) {
            graph* Ae = create_graph_noedges(A->trans->size_graph);
            UNION->trans_e = merge_graphs(lag, 2, Ae, B->trans_e);
            delete_graph(Ae);
        }
        else if (!B->trans_e) {
            graph* Be = create_graph_noedges(B->trans->size_graph);
            UNION->trans_e = merge_graphs(lag, 2, A->trans_e, Be);
            delete_graph(Be);
        }
        else {
            UNION->trans_e = merge_graphs(lag, 2, A->trans_e, B->trans_e);
        }
    }
    if (A->trans_i || B->trans_i) {
        if (!A->trans_i) {
            lgraph* Ai = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
            UNION->trans_i = merge_lgraphs(lag, 2, Ai, B->trans_i);
            delete_lgraph(Ai);
        }
        else if (!B->trans_i) {
            lgraph* Bi = create_lgraph_noedges(B->trans->size_graph, B->trans->size_alpha);
            UNION->trans_i = merge_lgraphs(lag, 2, A->trans_i, Bi);
            delete_lgraph(Bi);
        }
        else {
            UNION->trans_i = merge_lgraphs(lag, 2, A->trans_i, B->trans_i);
        }
    }
    UNION->trans = merge_lgraphs(lag, 2, A->trans, B->trans);

    copy_dequeue_right(UNION->initials, A->initials, 0);
    copy_dequeue_right(UNION->initials, B->initials, A->trans->size_graph);
    copy_dequeue_right(UNION->finals, A->finals, 0);
    copy_dequeue_right(UNION->finals, B->finals, A->trans->size_graph);

    UNION->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    UNION->state_names = NULL;

    nfa_delete(A);
    nfa_delete(B);

    return UNION;


}

nfa* nfa_concat(void* I1, bool is_dfa_I1, void* I2, bool is_dfa_I2) {
    if (!I1 || !I2) {
        return NULL;
    }

    nfa* A = NULL;
    nfa* B = NULL;
    unify_alphabets(I1, is_dfa_I1, I2, is_dfa_I2, &A, &B);

    nfa* CONCAT;
    CALLOC(CONCAT, 1);
    CONCAT->initials = create_dequeue();
    CONCAT->finals = create_dequeue();

    uint lag[2];
    if (!A->trans_e && !B->trans_e) {
        CONCAT->trans_e = create_graph_noedges(A->trans->size_graph + B->trans->size_graph);
    }
    else if (!A->trans_e) {
        graph* Ae = create_graph_noedges(A->trans->size_graph);
        CONCAT->trans_e = merge_graphs(lag, 2, Ae, B->trans_e);
        delete_graph(Ae);
    }
    else if (!B->trans_e) {
        graph* Be = create_graph_noedges(B->trans->size_graph);
        CONCAT->trans_e = merge_graphs(lag, 2, A->trans_e, Be);
        delete_graph(Be);
    }
    else {
        CONCAT->trans_e = merge_graphs(lag, 2, A->trans_e, B->trans_e);
    }

    if (A->trans_i || B->trans_i) {
        if (!A->trans_i) {
            lgraph* Ai = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
            CONCAT->trans_i = merge_lgraphs(lag, 2, Ai, B->trans_i);
            delete_lgraph(Ai);
        }
        else if (!B->trans_i) {
            lgraph* Bi = create_lgraph_noedges(B->trans->size_graph, B->trans->size_alpha);
            CONCAT->trans_i = merge_lgraphs(lag, 2, A->trans_i, Bi);
            delete_lgraph(Bi);
        }
        else {
            CONCAT->trans_i = merge_lgraphs(lag, 2, A->trans_i, B->trans_i);
        }
    }
    CONCAT->trans = merge_lgraphs(lag, 2, A->trans, B->trans);

    for (uint i = 0; i < size_dequeue(A->finals); i++) {
        copy_dequeue_right(CONCAT->trans_e->edges[lefread_dequeue(A->finals, i)], B->initials, lag[1]);
    }

    // Création des listes d'états finaux et initiaux
    copy_dequeue_right(CONCAT->initials, A->initials, lag[0]);
    copy_dequeue_right(CONCAT->finals, B->finals, lag[1]);

    CONCAT->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    CONCAT->state_names = NULL;

    nfa_delete(A);
    nfa_delete(B);

    return CONCAT;
}

nfa* nfa_star(nfa* A) {

    // Création de l'automate
    nfa* STAR;
    CALLOC(STAR, 1);
    STAR->initials = create_dequeue();
    STAR->finals = create_dequeue();

    uint lag[2];
    graph* oeps = create_graph_noedges(1);
    if (A->trans_e) {
        STAR->trans_e = merge_graphs(lag, 2, oeps, A->trans_e);
    }
    else {
        graph* geps = create_graph_noedges(A->trans->size_graph);
        STAR->trans_e = merge_graphs(lag, 2, oeps, geps);
        delete_graph(geps);
    }
    delete_graph(oeps);

    // Noeud intermédiaire pour relier le NFA
    lgraph* one = create_lgraph_noedges(1, A->trans->size_alpha);

    if (A->trans_i) {
        STAR->trans_i = merge_lgraphs(lag, 2, one, A->trans_i);
    }
    else {
        lgraph* ginv = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
        STAR->trans_i = merge_lgraphs(lag, 2, one, ginv);
        delete_lgraph(ginv);
    }

    STAR->trans = merge_lgraphs(lag, 2, one, A->trans);
    delete_lgraph(one);

    rigins_dequeue(0, STAR->initials);
    rigins_dequeue(0, STAR->finals);

    copy_dequeue_right(STAR->trans_e->edges[0], A->initials, lag[1]);

    for (uint i = 0; i < size_dequeue(A->finals); i++) {
        lefins_dequeue(0, STAR->trans_e->edges[lefread_dequeue(A->finals, i) + lag[1]]);
    }

    STAR->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    STAR->state_names = NULL;
    return STAR;
}


nfa* dfa_star(dfa* A) {

    // Création de l'automate
    nfa* STAR;
    CALLOC(STAR, 1);
    STAR->initials = create_dequeue();
    STAR->finals = create_dequeue();

    STAR->trans_e = create_graph_noedges(A->trans->size_graph + 1);
    rigins_dequeue(A->initial + 1, STAR->trans_e->edges[0]);
    for (uint i = 0; i < A->nb_finals; i++) {
        rigins_dequeue(0, STAR->trans_e->edges[A->finals[i] + 1]);
    }
    rigins_dequeue(0, STAR->initials);
    rigins_dequeue(0, STAR->finals);

    STAR->trans = create_lgraph_noedges(A->trans->size_graph + 1, A->trans->size_alpha);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            rigins_dequeue(A->trans->edges[q][a] + 1, STAR->trans->edges[q + 1][a]);
        }
    }



    MALLOC(STAR->alphabet, A->trans->size_alpha);
    for (uint i = 0; i < A->trans->size_alpha; i++) {
        STAR->alphabet[i] = A->alphabet[i];
    }
    STAR->state_names = NULL;
    return STAR;
}

nfa* nfa_plus(nfa* A) {

    // Création de l'automate
    nfa* PLUS;
    CALLOC(PLUS, 1);
    PLUS->initials = create_dequeue();
    PLUS->finals = create_dequeue();

    uint lag[2];
    graph* oeps = create_graph_noedges(1);
    if (A->trans_e) {
        PLUS->trans_e = merge_graphs(lag, 2, oeps, A->trans_e);
    }
    else {
        graph* geps = create_graph_noedges(A->trans->size_graph);
        PLUS->trans_e = merge_graphs(lag, 2, oeps, geps);
        delete_graph(geps);
    }
    delete_graph(oeps);

    lgraph* one = create_lgraph_noedges(1, A->trans->size_alpha);

    if (A->trans_i) {
        PLUS->trans_i = merge_lgraphs(lag, 2, one, A->trans_i);
    }
    else {
        lgraph* ginv = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
        PLUS->trans_i = merge_lgraphs(lag, 2, one, ginv);
        delete_lgraph(ginv);
    }

    PLUS->trans = merge_lgraphs(lag, 2, one, A->trans);
    delete_lgraph(one);

    rigins_dequeue(0, PLUS->initials);
    copy_dequeue_right(PLUS->finals, A->finals, lag[1]);

    copy_dequeue_right(PLUS->trans_e->edges[0], A->initials, lag[1]);

    for (uint i = 0; i < size_dequeue(A->finals); i++) {
        lefins_dequeue(0, PLUS->trans_e->edges[lefread_dequeue(A->finals, i) + lag[1]]);
    }

    PLUS->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    PLUS->state_names = NULL;
    return PLUS;
}

// Miroir
nfa* nfa_mirror(nfa* A) {
    // Création du miroir
    nfa* themirror;
    CALLOC(themirror, 1);
    themirror->initials = create_dequeue();
    themirror->finals = create_dequeue();

    themirror->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    themirror->state_names = NULL;

    // Création des états initiaux et finaux (swap de l'automate d'origine)
    copy_dequeue_right(themirror->initials, A->finals, 0);
    copy_dequeue_right(themirror->finals, A->initials, 0);

    // Création du graph des transitions
    themirror->trans = lgraph_mirror(A->trans);
    themirror->trans_e = graph_mirror(A->trans_e);
    themirror->trans_i = lgraph_mirror(A->trans_i);

    return themirror;
}



nfa* dfa_mirror(dfa* A) {
    // Creation of the mirror
    nfa* themirror;
    CALLOC(themirror, 1);
    themirror->initials = create_dequeue();
    themirror->finals = create_dequeue();

    MALLOC(themirror->alphabet, A->trans->size_alpha);
    for (uint i = 0; i < A->trans->size_alpha; i++) {
        themirror->alphabet[i] = A->alphabet[i];
    }
    themirror->state_names = copy_all_names(A->state_names, A->trans->size_graph);

    // Initial and final states
    rigins_dequeue(A->initial, themirror->finals);
    for (uint i = 0; i < A->nb_finals; i++) {
        rigins_dequeue(A->finals[i], themirror->initials);
    }
    // Création du graph des transitions
    themirror->trans = dgraph_mirror(A->trans);


    return themirror;
}

// Élimination des états non-accessibles et non-co-accessibles
// Le NFA produit n'est pas nécessairement complet
nfa* nfa_trim(nfa* A) {
    if (!A) {
        return NULL;
    }

    barray* reachable = create_barray(A->trans->size_graph);
    dequeue* queue = create_dequeue();
    copy_dequeue_right(queue, A->initials, 0);

    while (!isempty_dequeue(queue)) {
        uint q = lefpull_dequeue(queue);
        if (!getval_barray(reachable, q)) {
            settrue_barray(reachable, q);
            for (uint a = 0; a < A->trans->size_alpha; a++) {
                copy_dequeue_right(queue, A->trans->edges[q][a], 0);
            }
            if (A->trans_e) {
                copy_dequeue_right(queue, A->trans_e->edges[q], 0);
            }

            if (A->trans_i) {
                for (uint a = 0; a < A->trans->size_alpha; a++) {
                    copy_dequeue_right(queue, A->trans_i->edges[q][a], 0);
                }
            }
        }
    }
    delete_dequeue(queue);

    nfa* AM = nfa_mirror(A);

    barray* coreachable = create_barray(A->trans->size_graph);
    queue = create_dequeue();
    copy_dequeue_right(queue, AM->initials, 0);

    while (!isempty_dequeue(queue)) {
        uint q = lefpull_dequeue(queue);
        if (!getval_barray(coreachable, q)) {
            settrue_barray(coreachable, q);
            for (uint a = 0; a < AM->trans->size_alpha; a++) {
                copy_dequeue_right(queue, AM->trans->edges[q][a], 0);
            }
            if (AM->trans_e) {
                copy_dequeue_right(queue, AM->trans_e->edges[q], 0);
            }

            if (AM->trans_i) {
                for (uint a = 0; a < AM->trans->size_alpha; a++) {
                    copy_dequeue_right(queue, A->trans_i->edges[q][a], 0);
                }
            }
        }
    }
    delete_dequeue(queue);

    barray* keeped = and_barray(reachable, coreachable);
    delete_barray(reachable);
    delete_barray(coreachable);

    dequeue* statelist = create_dequeue();
    uint* newnums;
    MALLOC(newnums, A->trans->size_graph);
    for (uint q = 0; q < getsize_barray(keeped); q++) {
        if (getval_barray(keeped, q)) {
            newnums[q] = size_dequeue(statelist);
            rigins_dequeue(q, statelist);
        }
    }

    nfa* TRIM;
    CALLOC(TRIM, 1);
    TRIM->initials = create_dequeue();
    TRIM->finals = create_dequeue();

    TRIM->trans = create_lgraph_noedges(size_dequeue(statelist), A->trans->size_alpha);

    for (uint r = 0; r < TRIM->trans->size_graph; r++) {
        uint q = lefread_dequeue(statelist, r);
        for (uint a = 0; a < TRIM->trans->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(A->trans->edges[q][a]); i++) {
                if (getval_barray(keeped, lefread_dequeue(A->trans->edges[q][a], i))) {
                    rigins_dequeue(newnums[lefread_dequeue(A->trans->edges[q][a], i)], TRIM->trans->edges[r][a]);
                }
            }
        }
    }

    if (A->trans_e) {
        TRIM->trans_e = create_graph_noedges(size_dequeue(statelist));
        for (uint r = 0; r < TRIM->trans_e->size; r++) {
            uint q = lefread_dequeue(statelist, r);
            for (uint i = 0; i < size_dequeue(A->trans_e->edges[q]); i++) {
                if (getval_barray(keeped, lefread_dequeue(A->trans_e->edges[q], i))) {
                    rigins_dequeue(newnums[lefread_dequeue(A->trans_e->edges[q], i)], TRIM->trans_e->edges[r]);
                }
            }
        }
    }

    if (A->trans_i) {
        TRIM->trans_i = create_lgraph_noedges(size_dequeue(statelist), A->trans_i->size_alpha);
        for (uint r = 0; r < TRIM->trans_i->size_graph; r++) {
            uint q = lefread_dequeue(statelist, r);
            for (uint a = 0; a < TRIM->trans_i->size_alpha; a++) {
                for (uint i = 0; i < size_dequeue(A->trans_i->edges[q][a]); i++) {
                    if (getval_barray(keeped, lefread_dequeue(A->trans_i->edges[q][a], i))) {
                        rigins_dequeue(newnums[lefread_dequeue(A->trans_i->edges[q][a], i)], TRIM->trans_i->edges[r][a]);
                    }
                }
            }
        }
    }

    for (uint i = 0; i < size_dequeue(A->initials); i++) {
        rigins_dequeue(newnums[lefread_dequeue(A->initials, i)], TRIM->initials);
    }

    for (uint i = 0; i < size_dequeue(A->finals); i++) {
        if (getval_barray(keeped, lefread_dequeue(A->finals, i))) {
            rigins_dequeue(newnums[lefread_dequeue(A->finals, i)], TRIM->finals);
        }
    }

    TRIM->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);

    // Gestion des noms
    if (A->state_names) {
        MALLOC(TRIM->state_names, TRIM->trans->size_graph);
        for (uint q = 0; q < A->trans->size_graph; q++) {
            TRIM->state_names[newnums[q]] = strdup(A->state_names[q]);
        }
    }
    else {
        TRIM->state_names = NULL;
    }

    delete_dequeue(statelist);
    delete_barray(keeped);
    free(newnums);
    nfa_delete(AM);

    return TRIM;
}



dfa* dfa_trim(dfa* A) {
    if (!A) {
        return NULL;
    }

    uint* map;
    MALLOC(map, A->trans->size_graph);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        map[q] = UINT_MAX; // Marque les états non visités
    }
    dequeue* stack = create_dequeue();
    rigins_dequeue(A->initial, stack);
    uint nb_visted = 0;

    while (!isempty_dequeue(stack)) {
        uint q = rigpull_dequeue(stack);
        if (map[q] != UINT_MAX) {
            continue;
        }
        map[q] = nb_visted++;
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            rigins_dequeue(A->trans->edges[q][a], stack);
        }
    }

    delete_dequeue(stack);

    uint nb_finals = 0;
    for (uint i = 0; i < A->nb_finals; i++) {
        if (map[A->finals[i]] != UINT_MAX) {
            nb_finals++;
        }
    }

    dfa* TRIM;
    CALLOC(TRIM, 1);
    TRIM->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    TRIM->nb_finals = nb_finals;
    if (TRIM->nb_finals == 0) {
        TRIM->finals = NULL;
    }
    else {
        MALLOC(TRIM->finals, TRIM->nb_finals);
    }
    TRIM->trans = create_dgraph_noedges(nb_visted, A->trans->size_alpha);


    TRIM->initial = map[A->initial];
    for (uint q = 0; q < A->trans->size_graph; q++) {
        if (map[q] == UINT_MAX) {
            continue;
        }

        for (uint a = 0; a < A->trans->size_alpha; a++) {
            TRIM->trans->edges[map[q]][a] = map[A->trans->edges[q][a]];
        }
    }
    uint i = 0;
    for (uint j = 0; j < A->nb_finals; j++) {
        if (map[A->finals[j]] != UINT_MAX) {
            TRIM->finals[i++] = map[A->finals[j]];
        }
    }
    free(map);
    return TRIM;
}


// Élimination des états non-accessibles (modifie le NFA originel)
void nfa_trim_mod(nfa* A) {
    nfa* B = nfa_trim(A);
    nfa_overwrite(A, B);
}

// Élimination des transitions epsilon
nfa* nfa_elimeps(nfa* A) {
    if (A->trans_e == NULL) {
        // Si il n'y a pas de transitions epsilon
        return nfa_copy(A);
    }

    // Calcul du graphe des SCCs des transitions epsilon
    parti* sccs = tarjan(A->trans_e);

    // On fusionne les composantes fortement connexes de sccs de transitions
    // epsilon
    // Par construction, les nouveaux états du DAG (des trans eps) sont ordonnés
    // selon un tri topologique
    nfa* B = nfa_merge_states(A, sccs);

    delete_parti(sccs);

    // Clôture transitive des transitions epsilon dans B
    for (uint q = B->trans_e->size; q > 0; q--) {
        for (uint j = 0; j < size_dequeue(B->trans_e->edges[q - 1]); j++) {
            merge_sorted_dequeue(B->trans_e->edges[q - 1], B->trans_e->edges[lefread_dequeue(B->trans_e->edges[q - 1], j)]);
        }
    }

    // Enregistrement des nouveaux états finaux
    bool tempfin[B->trans->size_graph]; // Tableau temporaire pour marquer les
    // états finaux
    for (uint q = 0; q < B->trans->size_graph; q++) {
        tempfin[q] = false;
    }
    for (uint i = 0; i < size_dequeue(B->finals); i++) {
        tempfin[lefread_dequeue(B->finals, i)] = true;
    }
    for (uint q = 0; q < B->trans->size_graph; q++) {
        uint i = 0;
        while (!tempfin[q] && i < size_dequeue(B->trans_e->edges[q])) {
            if (tempfin[lefread_dequeue(B->trans_e->edges[q], i)]) {
                tempfin[q] = true;
            }
            i++;
        }
    }
    makeempty_dequeue(B->finals); // On vide l'ancienne liste des états finaux
    for (uint q = 0; q < B->trans->size_graph; q++) {
        if (tempfin[q]) {
            rigins_dequeue(q, B->finals);
        }
    }

    // Nouvelles transitions
    for (uint q = 0; q < B->trans->size_graph; q++) {
        for (uint i = 0; i < size_dequeue(B->trans_e->edges[q]); i++) {
            if (q != lefread_dequeue(B->trans_e->edges[q], i)) {
                // Transition classiques
                for (uint a = 0; a < B->trans->size_alpha; a++) {
                    merge_sorted_dequeue(B->trans->edges[q][a], B->trans->edges[lefread_dequeue(B->trans_e->edges[q], i)][a]);
                }

                // Transition inverses
                if (B->trans_i != NULL) {
                    for (uint a = 0; a < B->trans_i->size_alpha; a++) {
                        merge_sorted_dequeue(B->trans_i->edges[q][a], B->trans_i->edges[lefread_dequeue(B->trans_e->edges[q], i)][a]);
                    }
                }
            }
        }
    }
    // Libération des epsilons transitions
    delete_graph(B->trans_e);
    B->trans_e = NULL;

    // nfa_trim_mod(B);
    return B;
}

void nfa_elimeps_mod(nfa* A) {
    nfa* B = nfa_elimeps(A);
    nfa_overwrite(A, B);
}

/*****************************/
/* Generation of random NFAs */
/*****************************/


nfa* nfa_random(uint size_alpha, uint min_size, uint max_size) {
    min_size = max(1, min_size);
    uint sizea = max(1, size_alpha);

    uint size = min_size + (rand() % (1 + max_size - min_size));

    nfa* A;
    CALLOC(A, 1);
    A->initials = create_dequeue();
    A->finals = create_dequeue();

    //!< At least one initial state.
    rigins_dequeue(0, A->initials);

    MALLOC(A->alphabet, sizea);
    for (uint i = 0; i < sizea; i++) {
        A->alphabet[i].lab = i + 'a';
        A->alphabet[i].num = -1;
    }

    A->trans = create_lgraph_noedges(size, sizea);
    for (uint q = 0; q < size; q++) {
        for (uint a = 0; a < sizea; a++) {
            if (rand() % 8 == 0) {
                rigins_dequeue(q, A->trans->edges[q][a]);
            }

            uint count = 0;
            for (uint r = q + 1; r < size; r++) {
                if (rand() % (2 + count++) == 0) {
                    rigins_dequeue(r, A->trans->edges[q][a]);
                }
            }
            for (uint r = 0; r < q; r++) {
                if (rand() % (2 + count++) == 0) {
                    rigins_dequeue(r, A->trans->edges[q][a]);
                }
            }
        }
    }

    if (!(rand() % 4)) {
        rigins_dequeue(0, A->finals);
    }
    for (uint i = 1; i < size; i++) {
        if (!(rand() % 4)) {
            rigins_dequeue(i, A->finals);
        }
        if (!(rand() % 10)) {
            rigins_dequeue(i, A->initials);
        }
    }
    if (isempty_dequeue(A->finals)) {
        rigins_dequeue(size - 1, A->finals);
    }

    return A;
}

dfa* dfa_random(uint size_alpha, uint min_size, uint max_size) {
    min_size = max(1, min_size);
    uint sizea = max(1, size_alpha);

    uint size = min_size + (rand() % (1 + max_size - min_size));

    dfa* A;
    CALLOC(A, 1);
    MALLOC(A->alphabet, sizea);
    for (uint i = 0; i < sizea; i++) {
        A->alphabet[i].lab = i + 'a';
        A->alphabet[i].num = -1;
    }
    A->state_names = NULL;
    A->initial = 0;


    dequeue* finals = create_dequeue();
    int cf = 0;
    for (int i = size - 1; i >= 0; i--) {
        if (!(rand() % (2 + cf++))) {
            lefins_dequeue(i, finals);
        }
    }
    if (isempty_dequeue(finals)) {
        rigins_dequeue(size - 1, finals);
    }
    A->nb_finals = size_dequeue(finals);
    MALLOC(A->finals, A->nb_finals);
    for (uint i = 0; i < A->nb_finals; i++) {
        A->finals[i] = lefread_dequeue(finals, i);
    }
    delete_dequeue(finals);


    A->trans = create_dgraph_noedges(size, sizea);
    for (uint q = 0; q < size; q++) {
        for (uint a = 0; a < sizea; a++) {
            uint count = 0;
            uint r = (q + 1) % size;
            ;
            while (count == 0) {
                if (r >= q && rand() % 2 == 0) {
                    A->trans->edges[q][a] = r;
                    count = 1;
                }
                else if (rand() % 8 == 0) {
                    A->trans->edges[q][a] = r;
                    count = 1;
                }
                else {
                    r = (r + 1) % size;
                }
            }
        }
    }
    return A;
}



/*****************/
/*+ Information +*/
/*****************/

int nfa_nb_trans(nfa* A) {
    uint nb = 0;
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; q < A->trans->size_alpha; a++) {
            nb = nb + size_dequeue(A->trans->edges[q][a]);
        }
    }
    return nb;
}

bool nfa_is_det(nfa* A) {
    if (size_dequeue(A->initials) > 1 || A->trans_e || A->trans_i) {
        return false;
    }
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            if (size_dequeue(A->trans->edges[q][a]) > 1) {
                return false;
            }
        }
    }
    return true;
}

bool nfa_is_comp(nfa* A) {
    if (isempty_dequeue(A->initials)) {
        return false;
    }
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            if (isempty_dequeue(A->trans->edges[q][a])) {
                return false;
            }
        }
    }
    return true;
}

bool nfa_is_empty(nfa* A) {
    nfa* B = nfa_trim(A);
    bool res = isempty_dequeue(B->finals);
    nfa_delete(B);
    return res;
}

bool nfa_accepts(nfa* A, word* w) {
    if (isempty_dequeue(A->initials) || isempty_dequeue(A->finals)) {
        return false;
    }
    dequeue* reached = nfa_compute_runs(A, w);
    bool res = intersec_dequeue(reached, A->finals);
    delete_dequeue(reached);
    return res;
}

// Calcule les états qui sont atteints par un mot dans un NFA.
dequeue* nfa_compute_runs(nfa* A, word* w) {
    dequeue* states = create_dequeue();

    if (isempty_dequeue(A->initials)) {
        return states;
    }

    copy_dequeue_right(states, A->initials, 0);

    // Lecture du mot
    for (uint i = 0; i < size_word(w); i++) {
        uint ind = letter_index(lefread_word(w, i), A->alphabet, A->trans->size_alpha);
        if (ind >= A->trans->size_alpha) {
            delete_dequeue(states);
            return NULL;
        }
        dequeue* temp = lgraph_reachable(A->trans, states, ind);
        delete_dequeue(states);
        states = temp;
        if (isempty_dequeue(states)) {
            return states;
        }
    }

    return states;
}


bool dfa_exists_path(dfa* A, uint in, uint out, bool* alph) {
    if (in == out) {
        return true;
    }

    dequeue* stack = create_dequeue();
    rigins_dequeue(in, stack);
    bool* visited;
    CALLOC(visited, A->trans->size_graph);

    while (!isempty_dequeue(stack)) {
        uint q = rigpull_dequeue(stack);
        if (visited[q]) {
            continue;
        }
        if (q == out) {
            delete_dequeue(stack);
            free(visited);
            return true;
        }
        visited[q] = true;
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            if (alph[a]) {
                rigins_dequeue(A->trans->edges[q][a], stack);
            }
        }
    }

    delete_dequeue(stack);
    free(visited);
    return false;
}



// Calcule les états qui sont atteints par un mot dans un NFA.
uint dfa_compute_run(dfa* A, word* w) {

    uint q = A->initial;

    // Lecture du mot
    for (uint i = 0; i < size_word(w); i++) {
        letter l = lefread_word(w, i);
        uint letter_index = ((letter*)bsearch(&l, A->alphabet, A->trans->size_alpha, sizeof(letter), compare_letters)) - A->alphabet;
        if (letter_index == A->trans->size_alpha) {
            return UINT_MAX; // Invalid letter.
        }
        q = A->trans->edges[q][letter_index];
    }
    return q;
}


/************************/
/* Partitions of states */
/************************/

nfa* nfa_merge_states(nfa* A, parti* P) {
    // Création de l'automate fusionné
    nfa* B;
    CALLOC(B, 1);
    B->initials = create_dequeue();
    B->finals = create_dequeue();

    B->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    // Création des listes d'états initiaux et finaux
    uint_avlnode* initree = NULL;
    for (uint i = 0; i < size_dequeue(A->initials); i++) {
        initree = uint_avl_insert(P->numcl[lefread_dequeue(A->initials, i)], initree);
    }
    uint_avl_to_dequeue(initree, B->initials);

    uint_avlnode* fintree = NULL;
    for (uint i = 0; i < size_dequeue(A->finals); i++) {
        fintree = uint_avl_insert(P->numcl[lefread_dequeue(A->finals, i)], fintree);
    }
    uint_avl_to_dequeue(fintree, B->finals);

    // Création du graphe des transition classiques
    B->trans = create_lgraph_noedges(P->size_par, A->trans->size_alpha);
    for (uint c = 0; c < P->size_par; c++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            uint_avlnode* thetree = NULL;
            for (uint i = 0; i < P->cl_size[c]; i++) {
                for (uint j = 0; j < size_dequeue(A->trans->edges[P->cl_elems[c][i]][a]); j++) {
                    thetree = uint_avl_insert(P->numcl[lefread_dequeue(A->trans->edges[P->cl_elems[c][i]][a], j)], thetree);
                }
            }
            uint_avl_to_dequeue(thetree, B->trans->edges[c][a]);
        }
    }

    // Création du graphe des transition inverses (si besoin)
    if (A->trans_i != NULL) {
        B->trans_i = create_lgraph_noedges(P->size_par, A->trans_i->size_alpha);
        for (uint c = 0; c < P->size_par; c++) {
            for (uint a = 0; a < A->trans_i->size_alpha; a++) {
                uint_avlnode* thetree = NULL;
                for (uint i = 0; i < P->cl_size[c]; i++) {
                    for (uint j = 0; j < size_dequeue(A->trans_i->edges[P->cl_elems[c][i]][a]); j++) {
                        thetree = uint_avl_insert(P->numcl[lefread_dequeue(A->trans_i->edges[P->cl_elems[c][i]][a], j)], thetree);
                    }
                }
                uint_avl_to_dequeue(thetree, B->trans_i->edges[c][a]);
            }
        }
    }
    else {
        B->trans_i = NULL;
    }

    // Création du graphe des transitions epsilon (si besoin)
    if (A->trans_e != NULL) {
        B->trans_e = create_graph_noedges(P->size_par);
        for (uint c = 0; c < P->size_par; c++) {
            uint_avlnode* thetree = NULL;
            for (uint i = 0; i < P->cl_size[c]; i++) {
                for (uint j = 0; j < size_dequeue(A->trans_e->edges[P->cl_elems[c][i]]); j++) {
                    thetree = uint_avl_insert(P->numcl[lefread_dequeue(A->trans_e->edges[P->cl_elems[c][i]], j)], thetree);
                }
            }
            uint_avl_to_dequeue(thetree, B->trans_e->edges[c]);
        }
    }
    else {
        B->trans_e = NULL;
    }
    B->state_names = NULL;

    MALLOC(B->state_names, P->size_par);
    char buffer[64];

    for (uint q = 0; q < P->size_par; q++) {
        if (A->state_names) {
            B->state_names[q] = strdup(A->state_names[P->cl_elems[q][0]]);
        }
        else {
            sprintf(buffer, "%d", P->cl_elems[q][0]);
            B->state_names[q] = strdup(buffer);
        }
    }

    return B;
}




/****************/
/*+ Conversion +*/
/****************/



dfa* detnfa_to_dfa(nfa* A) {
    if (!nfa_is_det(A) || isempty_dequeue(A->initials)) {
        return NULL;
    }
    uint size = A->trans->size_graph;
    if (!nfa_is_comp(A)) {
        size++;
    }

    dfa* D;
    CALLOC(D, 1);
    D->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    D->initial = lefread_dequeue(A->initials, 0);
    D->nb_finals = size_dequeue(A->finals);
    MALLOC(D->finals, D->nb_finals);
    for (uint i = 0; i < D->nb_finals; i++) {
        D->finals[i] = lefread_dequeue(A->finals, i);
    }

    D->trans = create_dgraph_noedges(size, A->trans->size_alpha);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            if (size_dequeue(A->trans->edges[q][a]) > 0) {
                D->trans->edges[q][a] = lefread_dequeue(A->trans->edges[q][a], 0);
            }
            else {
                D->trans->edges[q][a] = A->trans->size_graph; // Sink state.
            }
        }
    }

    if (size > A->trans->size_graph) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            D->trans->edges[A->trans->size_graph][a] = A->trans->size_graph;
        }
    }

    return D;
}


nfa* dfa_to_nfa(dfa* A) {

    if (!A) {
        return NULL;
    }

    // Initialize the NFA
    nfa* N;
    CALLOC(N, 1);
    N->initials = create_dequeue();
    N->finals = create_dequeue();

    // Copy the alphabet
    MALLOC(N->alphabet, A->trans->size_alpha);
    for (uint i = 0; i < A->trans->size_alpha; i++) {
        N->alphabet[i] = A->alphabet[i];
    }

    // Initial state
    rigins_dequeue(A->initial, N->initials);

    // Final states
    for (uint i = 0; i < A->nb_finals; i++) {
        rigins_dequeue(A->finals[i], N->finals);
    }

    // Transitions
    N->trans = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);

    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            if (A->trans->edges[q][a] != UINT_MAX) {
                rigins_dequeue(A->trans->edges[q][a], N->trans->edges[q][a]);
            }
        }
    }

    // Gestion des noms d'états
    if (A->state_names) {
        N->state_names = copy_all_names(A->state_names, A->trans->size_graph);
    }

    return N;

}

nfa* dfa_to_nfa_exalpha(dfa* A, letter* alpha, uint size) {
    if (!A) {
        return NULL;
    }
    nfa* B;
    CALLOC(B, 1);
    B->initials = create_dequeue();
    B->finals = create_dequeue();

    uint alpha_size;

    B->alphabet = merge_alphabet(A->alphabet, A->trans->size_alpha, alpha, size, &alpha_size);

    // Copie du graphe des transitions.
    B->trans = create_lgraph_noedges(A->trans->size_graph, alpha_size);
    uint num = 0;
    for (uint a = 0; a < B->trans->size_alpha; a++) {
        if (num < A->trans->size_alpha && compare_letters(&A->alphabet[num], &B->alphabet[a]) == 0) {
            for (uint q = 0; q < B->trans->size_graph; q++) {
                rigins_dequeue(A->trans->edges[q][num], B->trans->edges[q][a]);
            }
            num++;
        }
    }


    // Initials
    rigins_dequeue(A->initial, B->initials);

    // Finals
    for (uint i = 0; i < A->nb_finals; i++) {
        rigins_dequeue(A->finals[i], B->finals);
    }

    // Copies des noms d'états
    B->state_names = copy_all_names(A->state_names, A->trans->size_graph);


    return B;
}














