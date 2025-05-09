/***************************/
/* Implémentation des NFAs */
/***************************/

#include "nfa.h"
#include "error.h"
#include "printing.h"

/*************/
/* Alphabets */
/*************/

void nfa_fprint_letter_utf8(const nfa *A, uint a, FILE *out) {
    if (!A || !out || !A->alphabet) {
        return;
    }
    fprint_letter_utf8(A->alphabet[a], out);
}

void nfa_fprint_letter_gviz(const nfa *A, uint a, FILE *out, bool inv) {
    if (!A || !out || !A->alphabet) {
        return;
    }
    fprint_letter_gviz(A->alphabet[a], out, inv);
}

letter *nfa_duplicate_alpha(const nfa *A) {
    if (!A) {
        return NULL;
    }

    letter *alphabet;
    MALLOC(alphabet, A->trans->size_alpha);
    for (uint i = 0; i < A->trans->size_alpha; i++) {
        alphabet[i] = A->alphabet[i];
    }
    return alphabet;
}

uint nfa_letter_index(const nfa *A, letter l) {
    letter *p = bsearch(&l, A->alphabet, A->trans->size_alpha, sizeof(letter), compare_letters);
    if (p) {
        return p - A->alphabet;
    } else {
        return A->trans->size_alpha;
    }
}

/***************/
/* State names */
/***************/

void nfa_print_state(const nfa *A, uint q, FILE *out) {
    if (!A || !out) {
        return;
    }
    if (A->state_names) {
        // fprintf(out, "%d", q);
        fprintf(out, "%s", A->state_names[q]);
    } else {
        fprintf(out, "%d", q);
    }
}

void nfa_reset_state_names(nfa *A) {
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

char *nfa_copy_one_name(const nfa *A, uint q) {
    if (!A || !A->state_names) {
        return NULL;
    }
    char *new;
    MALLOC(new, strlen(A->state_names[q]) + 1);
    strcpy(new, A->state_names[q]);
    return new;
}

char **nfa_copy_all_names(const nfa *A) {
    if (!A || !A->state_names) {
        return NULL;
    }
    char **names;
    MALLOC(names, A->trans->size_graph);
    for (uint q = 0; q < A->trans->size_graph; q++) {
        names[q] = nfa_copy_one_name(A, q);
    }
    return names;
}

/*************/
/* Ancestors */
/*************/

uint *nfa_copy_ancestors(const nfa *A) {
    if (!A || !A->ancestors) {
        return NULL;
    }
    uint *ancestors;
    MALLOC(ancestors, A->trans->size_graph);
    for (uint i = 0; i < A->trans->size_graph; i++) {
        ancestors[i] = A->ancestors[i];
    }
    return ancestors;
}

/******************************/
/* Initialization and release */
/******************************/

nfa *nfa_init(void) {
    nfa *A;
    MALLOC(A, 1);

    A->initials = create_dequeue();
    A->finals = create_dequeue();

    A->trans = NULL;
    A->trans_e = NULL;
    A->trans_i = NULL;

    A->alphabet = NULL;
    A->state_names = NULL;

    A->ancestors = NULL;
    return A;
}

nfa *create_emptylang(void) {
    nfa *A = nfa_init();

    // Graphe des transitions (vide)
    A->trans = create_lgraph_noedges(0, 0);

    return A;
}

nfa *create_sing_epsilon(void) {
    nfa *A = nfa_init();

    A->trans = create_lgraph_noedges(1, 0);
    lefins_dequeue(0, A->initials);
    lefins_dequeue(0, A->finals);

    return A;
}

nfa *create_sing_letter(letter the_letter) {
    nfa *A = nfa_init();

    // Graphe des transitions (un seul état)
    A->trans = create_lgraph_noedges(2, 1);
    lefins_dequeue(0, A->initials);
    lefins_dequeue(1, A->finals);
    lefins_dequeue(1, A->trans->edges[0][0]);

    // Alphabet
    MALLOC(A->alphabet, 1);
    A->alphabet[0] = the_letter;

    return A;
}

nfa *create_sing_word(word *the_word) {
    nfa *A = nfa_init();

    // Graphe des transitions (un seul état)

    lefins_dequeue(0, A->initials);
    lefins_dequeue(size_word(the_word), A->finals);
    uint alpha_size;

    A->alphabet = get_alphabet_word(the_word, &alpha_size);

    A->trans = create_lgraph_noedges(size_word(the_word) + 1, alpha_size);

    for (uint i = 0; i < size_word(the_word); i++) {
        letter l = lefread_word(the_word, i);
        letter *p = bsearch(&l, A->alphabet, alpha_size, sizeof(letter), compare_letters);
        lefins_dequeue(i + 1, A->trans->edges[i][p - A->alphabet]);
    }
    return A;
}

void delete_nfa(nfa *A) {
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

    if (A->ancestors) {
        free(A->ancestors);
    }

    free(A);
}

void overwrite_nfa(nfa *A, nfa *B) {
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

    A->ancestors = B->ancestors;

    free(B);
}

/***********************************/
/* Opérations simples sur les NFAs */
/***********************************/

nfa *nfa_copy(nfa *A) {
    if (!A) {
        return NULL;
    }
    nfa *B = nfa_init();

    // Copie du graphe des transitions.
    B->trans = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
    for (uint q = 0; q < B->trans->size_graph; q++) {
        for (uint a = 0; a < B->trans->size_alpha; a++) {
            copy_dequeue_right(B->trans->edges[q][a], A->trans->edges[q][a], 0);
        }
    }

    if (A->trans_e) {
        B->trans_e = create_graph_noedges(A->trans_e->size);
        for (uint q = 0; q < B->trans_e->size; q++) {
            copy_dequeue_right(B->trans_e->edges[q], A->trans_e->edges[q], 0);
        }
    }

    if (A->trans_i) {
        B->trans_i = create_lgraph_noedges(A->trans_i->size_graph, A->trans_i->size_alpha);
        for (uint q = 0; q < B->trans_i->size_graph; q++) {
            for (uint a = 0; a < B->trans_i->size_alpha; a++) {
                copy_dequeue_right(B->trans_i->edges[q][a], A->trans_i->edges[q][a], 0);
            }
        }
    }

    // Initiaux
    copy_dequeue_right(B->initials, A->initials, 0);
    // Finaux
    copy_dequeue_right(B->finals, A->finals, 0);

    // Noms des lettres
    B->alphabet = nfa_duplicate_alpha(A);

    // Noms des états
    B->state_names = nfa_copy_all_names(A);

    B->ancestors = nfa_copy_ancestors(A);

    return B;
}

nfa *nfa_copy_exalpha(nfa *A, letter *alpha, uint size) {
    if (!A) {
        return NULL;
    }
    nfa *B = nfa_init();

    // Taille du nouvel alphabet
    uchar i = 0;
    uchar j = 0;
    uchar c = 0;
    while (i < A->trans->size_alpha && j < size) {
        if (compare_letters(&A->alphabet[i], &alpha[j]) < 0) {
            i++;
        } else if (compare_letters(&A->alphabet[i], &alpha[j]) > 0) {
            j++;
        } else {
            i++;
            j++;
        }
        c++;
    }
    if (i < A->trans->size_alpha) {
        c += A->trans->size_alpha - i;
    } else if (j < size) {
        c += size - j;
    }

    // Calcul du nouvel alphabet
    MALLOC(B->alphabet, c);

    uint k = 0;
    i = 0;
    j = 0;
    while (i < A->trans->size_alpha || j < size) {
        if (i < A->trans->size_alpha && j < size) {
            if (compare_letters(&A->alphabet[i], &alpha[j]) < 0) {
                B->alphabet[k] = A->alphabet[i];
                i++;
            } else if (compare_letters(&A->alphabet[i], &alpha[j]) == 0) {
                B->alphabet[k] = A->alphabet[i];
                i++;
                j++;
            } else {
                B->alphabet[k] = alpha[j];
                j++;
            }
        } else if (i < A->trans->size_alpha) {
            B->alphabet[k] = A->alphabet[i];
            i++;
        } else {
            B->alphabet[k] = alpha[j];
            j++;
        }
        k++;
    }

    // Copie du graphe des transitions.
    B->trans = create_lgraph_noedges(A->trans->size_graph, c);
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
    } else {
        B->trans_e = NULL;
    }

    if (A->trans_i) {
        B->trans_i = create_lgraph_noedges(A->trans_i->size_graph, c);
        for (uint a = 0; a < B->trans_i->size_alpha; a++) {
            if (num < A->trans_i->size_alpha && compare_letters(&A->alphabet[num], &B->alphabet[a]) == 0) {
                for (uint q = 0; q < B->trans_i->size_graph; q++) {
                    copy_dequeue_right(B->trans_i->edges[q][a], A->trans_i->edges[q][num], 0);
                }
                num++;
            }
        }

    } else {
        B->trans_i = NULL;
    }

    // Initiaux
    copy_dequeue_right(B->initials, A->initials, 0);
    // Finals
    copy_dequeue_right(B->finals, A->finals, 0);

    // Copies des noms d'états
    B->state_names = nfa_copy_all_names(A);

    B->ancestors = nfa_copy_ancestors(A);

    return B;
}

// Union disjointe de deux nfas
nfa *nfa_union(nfa *A, nfa *B) {
    if (!A || !B) {
        return NULL;
    }

    nfa *UNION = nfa_init();

    // On étend les alphabets des automates

    A = nfa_copy_exalpha(A, B->alphabet, B->trans->size_alpha);
    B = nfa_copy_exalpha(B, A->alphabet, A->trans->size_alpha);

    uint lag[2];

    if (A->trans_e || B->trans_e) {
        if (!A->trans_e) {
            graph *Ae = create_graph_selfloops(A->trans->size_graph);
            UNION->trans_e = merge_graphs(lag, 2, Ae, B->trans_e);
            delete_graph(Ae);
        } else if (!B->trans_e) {
            graph *Be = create_graph_selfloops(B->trans->size_graph);
            UNION->trans_e = merge_graphs(lag, 2, A->trans_e, Be);
            delete_graph(Be);
        } else {
            UNION->trans_e = merge_graphs(lag, 2, A->trans_e, B->trans_e);
        }
    }
    if (A->trans_i || B->trans_i) {
        if (!A->trans_i) {
            lgraph *Ai = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
            UNION->trans_i = merge_lgraphs(lag, 2, Ai, B->trans_i);
            delete_lgraph(Ai);
        } else if (!B->trans_i) {
            lgraph *Bi = create_lgraph_noedges(B->trans->size_graph, B->trans->size_alpha);
            UNION->trans_i = merge_lgraphs(lag, 2, A->trans_i, Bi);
            delete_lgraph(Bi);
        } else {
            UNION->trans_i = merge_lgraphs(lag, 2, A->trans_i, B->trans_i);
        }
    }
    UNION->trans = merge_lgraphs(lag, 2, A->trans, B->trans);

    copy_dequeue_right(UNION->initials, A->initials, 0);
    copy_dequeue_right(UNION->initials, B->initials, A->trans->size_graph);
    copy_dequeue_right(UNION->finals, A->finals, 0);
    copy_dequeue_right(UNION->finals, B->finals, A->trans->size_graph);

    UNION->alphabet = nfa_duplicate_alpha(A);
    UNION->state_names = NULL;

    delete_nfa(A);
    delete_nfa(B);

    return UNION;
}

nfa *nfa_concat(nfa *A, nfa *B) {
    if (!A || !B) {
        return NULL;
    }

    nfa *CONCAT = nfa_init();
    // On étend les alphabets des automates
    A = nfa_copy_exalpha(A, B->alphabet, B->trans->size_alpha);
    B = nfa_copy_exalpha(B, A->alphabet, A->trans->size_alpha);

    uint lag[2];
    if (!A->trans_e && !B->trans_e) {
        CONCAT->trans_e = create_graph_selfloops(A->trans->size_graph + B->trans->size_graph);
    } else if (!A->trans_e) {
        graph *Ae = create_graph_selfloops(A->trans->size_graph);
        CONCAT->trans_e = merge_graphs(lag, 2, Ae, B->trans_e);
        delete_graph(Ae);
    } else if (!B->trans_e) {
        graph *Be = create_graph_selfloops(B->trans->size_graph);
        CONCAT->trans_e = merge_graphs(lag, 2, A->trans_e, Be);
        delete_graph(Be);
    } else {
        CONCAT->trans_e = merge_graphs(lag, 2, A->trans_e, B->trans_e);
    }

    if (A->trans_i || B->trans_i) {
        if (!A->trans_i) {
            lgraph *Ai = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
            CONCAT->trans_i = merge_lgraphs(lag, 2, Ai, B->trans_i);
            delete_lgraph(Ai);
        } else if (!B->trans_i) {
            lgraph *Bi = create_lgraph_noedges(B->trans->size_graph, B->trans->size_alpha);
            CONCAT->trans_i = merge_lgraphs(lag, 2, A->trans_i, Bi);
            delete_lgraph(Bi);
        } else {
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

    CONCAT->alphabet = nfa_duplicate_alpha(A);
    CONCAT->state_names = NULL;

    delete_nfa(A);
    delete_nfa(B);

    return CONCAT;
}

nfa *nfa_star(nfa *A) {

    // Création de l'automate
    nfa *STAR = nfa_init();

    uint lag[2];
    graph *oeps = create_graph_selfloops(1);
    if (A->trans_e) {
        STAR->trans_e = merge_graphs(lag, 2, oeps, A->trans_e);
    } else {
        graph *geps = create_graph_selfloops(A->trans->size_graph);
        STAR->trans_e = merge_graphs(lag, 2, oeps, geps);
        delete_graph(geps);
    }
    delete_graph(oeps);

    // Noeud intermédiaire pour relier le NFA
    lgraph *one = create_lgraph_noedges(1, A->trans->size_alpha);

    if (A->trans_i) {
        STAR->trans_i = merge_lgraphs(lag, 2, one, A->trans_i);
    } else {
        lgraph *ginv = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
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

    STAR->alphabet = nfa_duplicate_alpha(A);
    STAR->state_names = NULL;
    return STAR;
}

nfa *nfa_plus(nfa *A) {

    // Création de l'automate
    nfa *PLUS = nfa_init();

    uint lag[2];
    graph *oeps = create_graph_selfloops(1);
    if (A->trans_e) {
        PLUS->trans_e = merge_graphs(lag, 2, oeps, A->trans_e);
    } else {
        graph *geps = create_graph_selfloops(A->trans->size_graph);
        PLUS->trans_e = merge_graphs(lag, 2, oeps, geps);
        delete_graph(geps);
    }
    delete_graph(oeps);

    lgraph *one = create_lgraph_noedges(1, A->trans->size_alpha);

    if (A->trans_i) {
        PLUS->trans_i = merge_lgraphs(lag, 2, one, A->trans_i);
    } else {
        lgraph *ginv = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
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

    PLUS->alphabet = nfa_duplicate_alpha(A);
    PLUS->state_names = NULL;
    return PLUS;
}

// Miroir
nfa *nfa_mirror(nfa *A) {
    // Création du miroir
    nfa *themirror = nfa_init();
    themirror->alphabet = nfa_duplicate_alpha(A);
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

// Élimination des états non-accessibles et non-co-accessibles
// Le NFA produit n'est pas nécessairement complet
nfa *nfa_trim(nfa *A) {
    if (!A) {
        return NULL;
    }

    barray *reachable = create_barray(A->trans->size_graph);
    dequeue *queue = create_dequeue();
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

    nfa *AM = nfa_mirror(A);

    barray *coreachable = create_barray(A->trans->size_graph);
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

    barray *keeped = and_barray(reachable, coreachable);
    delete_barray(reachable);
    delete_barray(coreachable);

    dequeue *statelist = create_dequeue();
    uint *newnums;
    MALLOC(newnums, A->trans->size_graph);
    for (uint q = 0; q < getsize_barray(keeped); q++) {
        if (getval_barray(keeped, q)) {
            newnums[q] = size_dequeue(statelist);
            rigins_dequeue(q, statelist);
        }
    }

    nfa *TRIM = nfa_init();

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

    TRIM->alphabet = nfa_duplicate_alpha(A);

    // Gestion des noms
    if (A->state_names) {
        MALLOC(TRIM->state_names, TRIM->trans->size_graph);
        for (uint q = 0; q < A->trans->size_graph; q++) {
            TRIM->state_names[newnums[q]] = nfa_copy_one_name(A, q);
        }
    } else {
        TRIM->state_names = NULL;
    }

    delete_dequeue(statelist);
    delete_barray(keeped);
    free(newnums);
    delete_nfa(AM);

    return TRIM;
}

// Élimination des états non-accessibles (modifie le NFA originel)
void nfa_trim_mod(nfa *A) {
    nfa *B = nfa_trim(A);
    overwrite_nfa(A, B);
}

// Élimination des transitions epsilon
nfa *nfa_elimeps(nfa *A) {
    if (A->trans_e == NULL) {
        // Si il n'y a pas de transitions epsilon
        return nfa_copy(A);
    }

    // Calcul du graphe des SCCs des transitions epsilon
    parti *sccs = tarjan(A->trans_e);

    // On fusionne les composantes fortement connexes de sccs de transitions
    // epsilon
    // Par construction, les nouveaux états du DAG (des trans eps) sont ordonnés
    // selon un tri topologique
    nfa *B = nfa_merge_states(A, sccs);

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

void nfa_elimeps_mod(nfa *A) {
    nfa *B = nfa_elimeps(A);
    overwrite_nfa(A, B);
}

/*****************************/
/* Generation of random NFAs */
/*****************************/

// Génération d'un NFA aléatoire
nfa *nfa_random(uint size_alpha, uint min_size, uint max_size) {
    min_size = max(1, min_size);
    uint sizea = max(1, size_alpha);

    uint size = min_size + (rand() % (1 + max_size - min_size));

    nfa *A = nfa_init();

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

// Génération d'un DFA aléatoire
nfa *dfa_random(uint size_alpha, uint min_size, uint max_size) {
    min_size = max(1, min_size);
    uint sizea = max(1, size_alpha);

    uint size = min_size + (rand() % (1 + max_size - min_size));

    nfa *A = nfa_init();

    // One initial state.
    rigins_dequeue(0, A->initials);

    MALLOC(A->alphabet, sizea);
    for (uint i = 0; i < sizea; i++) {
        A->alphabet[i].lab = i + 'a';
        A->alphabet[i].num = -1;
    }
    int cf = 0;
    for (int i = size - 1; i >= 0; i--) {
        if (!(rand() % (2 + cf++))) {
            lefins_dequeue(i, A->finals);
        }
    }
    if (isempty_dequeue(A->finals)) {
        rigins_dequeue(size - 1, A->finals);
    }
    A->trans = create_lgraph_noedges(size, sizea);
    for (uint q = 0; q < size; q++) {
        for (uint a = 0; a < sizea; a++) {
            uint count = 0;
            uint r = (q + 1) % size;
            ;
            while (count == 0) {
                if (r >= q && rand() % 2 == 0) {
                    rigins_dequeue(r, A->trans->edges[q][a]);
                    count = 1;
                } else if (rand() % 8 == 0) {
                    rigins_dequeue(r, A->trans->edges[q][a]);
                    count = 1;
                } else {
                    r = (r + 1) % size;
                }
            }
        }
    }
    return A;
}

/***********************/
/* Information sur nfa */
/***********************/

int nfa_nb_trans(nfa *A) {
    uint nb = 0;
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; q < A->trans->size_alpha; a++) {
            nb = nb + size_dequeue(A->trans->edges[q][a]);
        }
    }
    return nb;
}

bool nfa_is_det(nfa *A) {
    if (size_dequeue(A->initials) > 1) {
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

bool nfa_is_comp(nfa *A) {
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

bool nfa_is_empty(nfa *A) {
    nfa *B = nfa_trim(A);
    bool res = isempty_dequeue(B->finals);
    delete_nfa(B);
    return res;
}

bool nfa_accepts(nfa *A, word *w) {
    if (isempty_dequeue(A->initials) || isempty_dequeue(A->finals)) {
        return false;
    }
    dequeue *reached = nfa_compute_runs(A, w);
    bool res = intersec_dequeue(reached, A->finals);
    delete_dequeue(reached);
    return res;
}

// Calcule les états qui sont atteints par un mot dans un NFA.
dequeue *nfa_compute_runs(nfa *A, word *w) {
    dequeue *states = create_dequeue();

    if (isempty_dequeue(A->initials)) {
        return states;
    }

    copy_dequeue_right(states, A->initials, 0);

    // Lecture du mot
    for (uint i = 0; i < size_word(w); i++) {
        uint letter_index = nfa_letter_index(A, lefread_word(w, i));
        if (letter_index == A->trans->size_alpha) {
            delete_dequeue(states);
            return NULL;
        }
        dequeue *temp = lgraph_reachable(A->trans, states, letter_index);
        delete_dequeue(states);
        states = temp;
        if (isempty_dequeue(states)) {
            return states;
        }
    }

    return states;
}

/************************/
/* Partitions of states */
/************************/

nfa *nfa_merge_states(nfa *A, parti *P) {
    // Création de l'automate fusionné
    nfa *B = nfa_init();
    B->alphabet = nfa_duplicate_alpha(A);
    // Création des listes d'états initiaux et finaux
    uint_avlnode *initree = NULL;
    for (uint i = 0; i < size_dequeue(A->initials); i++) {
        initree = uint_avl_insert(P->numcl[lefread_dequeue(A->initials, i)], initree);
    }
    uint_avl_to_dequeue(initree, B->initials);

    uint_avlnode *fintree = NULL;
    for (uint i = 0; i < size_dequeue(A->finals); i++) {
        fintree = uint_avl_insert(P->numcl[lefread_dequeue(A->finals, i)], fintree);
    }
    uint_avl_to_dequeue(fintree, B->finals);

    // Création du graphe des transition classiques
    B->trans = create_lgraph_noedges(P->size_par, A->trans->size_alpha);
    for (uint c = 0; c < P->size_par; c++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            uint_avlnode *thetree = NULL;
            for (uint i = 0; i < size_dequeue(P->cl[c]); i++) {
                for (uint j = 0; j < size_dequeue(A->trans->edges[lefread_dequeue(P->cl[c], i)][a]); j++) {
                    thetree = uint_avl_insert(P->numcl[lefread_dequeue(A->trans->edges[lefread_dequeue(P->cl[c], i)][a], j)], thetree);
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
                uint_avlnode *thetree = NULL;
                for (uint i = 0; i < size_dequeue(P->cl[c]); i++) {
                    for (uint j = 0; j < size_dequeue(A->trans_i->edges[lefread_dequeue(P->cl[c], i)][a]); j++) {
                        thetree = uint_avl_insert(P->numcl[lefread_dequeue(A->trans_i->edges[lefread_dequeue(P->cl[c], i)][a], j)], thetree);
                    }
                }
                uint_avl_to_dequeue(thetree, B->trans_i->edges[c][a]);
            }
        }
    } else {
        B->trans_i = NULL;
    }

    // Création du graphe des transitions epsilon (si besoin)
    if (A->trans_e != NULL) {
        B->trans_e = create_graph_noedges(P->size_par);
        for (uint c = 0; c < P->size_par; c++) {
            uint_avlnode *thetree = NULL;
            for (uint i = 0; i < size_dequeue(P->cl[c]); i++) {
                for (uint j = 0; j < size_dequeue(A->trans_e->edges[lefread_dequeue(P->cl[c], i)]); j++) {
                    thetree = uint_avl_insert(P->numcl[lefread_dequeue(A->trans_e->edges[lefread_dequeue(P->cl[c], i)], j)], thetree);
                }
            }
            uint_avl_to_dequeue(thetree, B->trans_e->edges[c]);
        }
    } else {
        B->trans_e = NULL;
    }
    B->state_names = NULL;

    MALLOC(B->state_names, P->size_par);
    char buffer[64];

    for (uint q = 0; q < P->size_par; q++) {
        if (A->state_names) {
            B->state_names[q] = strdup(A->state_names[lefread_dequeue(P->cl[q], 0)]);
        } else {
            sprintf(buffer, "%d", lefread_dequeue(P->cl[q], 0));
            B->state_names[q] = strdup(buffer);
        }
    }

    return B;
}

// Récupération du graphe étiqueté obtenu en ne gardant que
// les transitions internes à une partition prise en entrée
// (typiquement utilisé pour la partition en SCCs)
lgraph *nfa_get_lab_parti(nfa *A, parti *P) {
    // Calcul du graphe
    lgraph *G = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
    for (uint q = 0; q < G->size_graph; q++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(A->trans->edges[q][a]); i++) {
                if (P->numcl[q] == P->numcl[lefread_dequeue(A->trans->edges[q][a], i)]) {
                    rigins_dequeue(lefread_dequeue(A->trans->edges[q][a], i), G->edges[q][a]);
                }
            }
        }
    }
    return G;
}
