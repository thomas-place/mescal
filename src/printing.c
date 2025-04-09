#include "printing.h"
#include "monoid_display.h"
#include <sys/utsname.h>

#define LINUX_VIEW_COMMAND "| convert - -resize 70% sixel:- | cat"
// #define OSX_VIEW_COMMAND "| ./imgcat -W auto"
#define OSX_VIEW_COMMAND "| ./imgcat -W auto"

static char* view_command(void) {

    struct utsname name;
    uname(&name);
    if (strcmp(name.sysname, "Darwin") == 0) {
        return OSX_VIEW_COMMAND;
    }
    else {
        return LINUX_VIEW_COMMAND;
    }
}

bool external_viewer = false;

/**********************/
/* Printing functions */
/**********************/

// Print d'un sommet, version noms contenus dans un deq
void list_print_state(void* p, FILE* out) {
    if (isempty_dequeue(p)) {
        fprintf(out, "E");
    }
    else {
        for (uint j = 0; j < size_dequeue(p) - 1; j++) {
            fprintf(out, "%d,", lefread_dequeue(p, j));
        }
        fprintf(out, "%d", lefread_dequeue(p, size_dequeue(p) - 1));
    }
}

/********************************************************/
/*+ Récupération d'une liste d'arêtes pour l'affichage +*/
/********************************************************/

static int multi_edges_comp(void* s, void* t) {
    if (((multi_edge*)s)->in < ((multi_edge*)t)->in) {
        return 1;
    }
    else if (((multi_edge*)s)->in > ((multi_edge*)t)->in) {
        return -1;
    }
    else {
        if (((multi_edge*)s)->out < ((multi_edge*)t)->out) {
            return 1;
        }
        else if (((multi_edge*)s)->out > ((multi_edge*)t)->out) {
            return -1;
        }
        else {
            return 0;
        }
    }
}

static void tree_to_stack(avlnode* tree, dequeue_gen* the_stack) {
    if (tree == NULL) {
        return;
    }
    else {
        tree_to_stack(tree->left, the_stack);
        rigins_dequeue_gen(tree->value, the_stack);
        tree_to_stack(tree->right, the_stack);
        free(tree);
    }
}

dequeue_gen* nfa_to_multi_edges(nfa* A) {
    if (!A) {
        return NULL;
    }

    lgraph* G = A->trans;
    graph* GEPS = A->trans_e;
    lgraph* GINV = A->trans_i;

    dequeue_gen* thestack = create_dequeue_gen();
    avlnode* thetree = NULL;
    for (uint q = 0; q < G->size_graph; q++) {
        if (GEPS != NULL) {
            for (uint i = 0; i < size_dequeue(GEPS->edges[q]); i++) {
                if (lefread_dequeue(GEPS->edges[q], i) != q) {
                    multi_edge* new;
                    MALLOC(new, 1);
                    new->in = q;
                    new->out = lefread_dequeue(GEPS->edges[q], i);
                    avlnode* old = avl_search(new, thetree, &multi_edges_comp);
                    if (old == NULL) {
                        new->lab = create_dequeue();
                        new->lab_i = create_dequeue();
                        new->eps = true;
                        thetree = avl_insert(new, thetree, &multi_edges_comp, NULL);
                    }
                    else {
                        ((multi_edge*)old->value)->eps = true;
                        free(new);
                    }
                }
            }
        }
        for (uint a = 0; a < G->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(G->edges[q][a]); i++) {
                multi_edge* new;
                MALLOC(new, 1);
                new->in = q;
                new->out = lefread_dequeue(G->edges[q][a], i);
                avlnode* old = avl_search(new, thetree, &multi_edges_comp);
                if (old == NULL) {
                    new->lab = create_dequeue();
                    new->lab_i = create_dequeue();
                    new->eps = false;
                    rigins_dequeue(a, new->lab);
                    thetree = avl_insert(new, thetree, &multi_edges_comp, NULL);
                }
                else {
                    rigins_dequeue(a, ((multi_edge*)old->value)->lab);
                    free(new);
                }
            }
        }
        if (GINV != NULL) {
            for (uint a = 0; a < GINV->size_alpha; a++) {
                for (uint i = 0; i < size_dequeue(GINV->edges[q][a]); i++) {
                    multi_edge* new;
                    MALLOC(new, 1);
                    new->in = q;
                    new->out = lefread_dequeue(GINV->edges[q][a], i);
                    avlnode* old = avl_search(new, thetree, &multi_edges_comp);
                    if (old == NULL) {
                        new->lab = create_dequeue();
                        new->lab_i = create_dequeue();
                        new->eps = false;
                        rigins_dequeue(a, new->lab_i);
                        thetree = avl_insert(new, thetree, &multi_edges_comp, NULL);
                    }
                    else {
                        rigins_dequeue(a, ((multi_edge*)old->value)->lab_i);
                        free(new);
                    }
                }
            }
        }
    }

    tree_to_stack(thetree, thestack);
    return thestack;
}

dequeue_gen* dgraph_to_multi_edges(dgraph* G) {
    dequeue_gen* thestack = create_dequeue_gen();
    avlnode* thetree = NULL;
    for (uint q = 0; q < G->size_graph; q++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            multi_edge* new;
            MALLOC(new, 1);
            new->in = q;
            // printf("test: %d\n", G->edges[q][a]);
            new->out = G->edges[q][a];
            avlnode* old = avl_search(new, thetree, &multi_edges_comp);
            if (old == NULL) {
                new->lab = create_dequeue();
                rigins_dequeue(a, new->lab);
                thetree = avl_insert(new, thetree, &multi_edges_comp, NULL);
            }
            else {
                rigins_dequeue(a, ((multi_edge*)old->value)->lab);
                free(new);
            }
        }
    }

    tree_to_stack(thetree, thestack);
    return thestack;
}

/********************************/
/* Print des arêtes d'un graphe */
/********************************/

void named_lgedges_print(dequeue_gen* theedges, nfa* A, FILE* out) {
    for (uint i = 0; i < size_dequeue_gen(theedges); i++) { // Boucle sur les états de départ

        multi_edge* oneedge = lefread_dequeue_gen(theedges, i);
        fprintf(out, "%d -> %d [label = <", oneedge->in, oneedge->out);

        if (oneedge->eps) {
            fprintf(out, "ε");
            if (!isempty_dequeue(oneedge->lab) || !isempty_dequeue(oneedge->lab_i)) {
                fprintf(out, ",");
            }
        }

        if (!isempty_dequeue(oneedge->lab)) {
            for (uint j = 0; j < size_dequeue(oneedge->lab); j++) {
                nfa_fprint_letter_gviz(A, lefread_dequeue(oneedge->lab, j), out, false);
                if (j < size_dequeue(oneedge->lab) - 1 || !isempty_dequeue(oneedge->lab_i)) {
                    fprintf(out, ",");
                }
            }
        }

        if (!isempty_dequeue(oneedge->lab_i)) {
            for (uint j = 0; j < size_dequeue(oneedge->lab_i); j++) {
                nfa_fprint_letter_gviz(A, lefread_dequeue(oneedge->lab_i, j), out, true);
                if (j < size_dequeue(oneedge->lab_i) - 1) {
                    fprintf(out, ",");
                }
            }
        }

        fprintf(out, ">]\n");
    }
}

void named_dedges_print(dequeue_gen* theedges, morphism* M, FILE* out) {
    for (uint i = 0; i < size_dequeue_gen(theedges); i++) { // Boucle sur les états de départ

        multi_edge* oneedge = lefread_dequeue_gen(theedges, i);
        fprintf(out, "%d -> %d [label = <", oneedge->in, oneedge->out);

        if (!isempty_dequeue(oneedge->lab)) {
            for (uint j = 0; j < size_dequeue(oneedge->lab); j++) {
                mor_print_name_gviz(M, M->r_cayley->edges[ONE][lefread_dequeue(oneedge->lab, j)], out);
                if (j < size_dequeue(oneedge->lab) - 1) {
                    fprintf(out, ",");
                }
            }
        }

        fprintf(out, ">]\n");
    }
}

/******************/
/* Print d'un NFA */
/******************/

void nfa_print(nfa* A, FILE* out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"\n");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n\n");

    uint i = 0;
    uint f = 0;

    for (uint k = 0; k < A->trans->size_graph; k++) {
        fprintf(out, "%d [style=solid", k);

        if (A->state_names) {
            fprintf(out, ",label=\"%s\"", A->state_names[k]);
        }
        if ((i < size_dequeue(A->initials) && lefread_dequeue(A->initials, i) == k) && (f < size_dequeue(A->finals) && lefread_dequeue(A->finals, f) == k)) {
            fprintf(out, ",fillcolor=\"blue:green\",style=filled,shape = doublecircle];\n");
            i++;
            f++;
            continue;
        }

        if (i < size_dequeue(A->initials) && lefread_dequeue(A->initials, i) == k) {
            fprintf(out, ",fillcolor=\"blue:green\",style=filled,shape = circle];\n");
            i++;
            continue;
        }

        if (f < size_dequeue(A->finals) && lefread_dequeue(A->finals, f) == k) {
            fprintf(out, ",shape = doublecircle];\n");
            f++;
            continue;
        }
        fprintf(out, ",shape = circle];\n");
    }

    dequeue_gen* theedges;

    // Calcul de l'ensemble de transitions
    theedges = nfa_to_multi_edges(A);

    named_lgedges_print(theedges, A, out);

    while (!isempty_dequeue_gen(theedges)) {
        multi_edge* new = rigpull_dequeue_gen(theedges);
        delete_dequeue(new->lab);
        free(new);
    }
    delete_dequeue_gen(theedges);

    fprintf(out, "}\n");
}

void cayley_print(morphism* M, FILE* out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n");

    uint f = 0;
    // Print des sommets
    for (uint k = 0; k < M->r_cayley->size_graph; k++) {
        fprintf(out, "%d [style=solid,label=<", k);
        mor_print_name_gviz(M, k, out);

        if (f < size_dequeue(M->accept_list) && lefread_dequeue(M->accept_list, f) == k) {
            fprintf(out, ">,shape = doublecircle];\n");
            f++;
            continue;
        }
        fprintf(out, ">,shape = circle];\n");
    }

    dequeue_gen* theedges = dgraph_to_multi_edges(M->r_cayley);

    named_dedges_print(theedges, M, out);
    while (!isempty_dequeue_gen(theedges)) {
        multi_edge* new = rigpull_dequeue_gen(theedges);
        delete_dequeue(new->lab);
        free(new);
    }
    delete_dequeue_gen(theedges);

    fprintf(out, "}\n");
}

void cayley_left_print(morphism* M, FILE* out) {


    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n");
    uint f = 0;
    // Print des sommets
    for (uint k = 0; k < M->l_cayley->size_graph; k++) {
        fprintf(out, "%d [style=solid,label=<", k);
        mor_print_name_gviz(M, k, out);

        if (f < size_dequeue(M->accept_list) && lefread_dequeue(M->accept_list, f) == k) {
            fprintf(out, ">,shape = doublecircle];\n");
            f++;
            continue;
        }
        fprintf(out, ">,shape = circle];\n");
    }

    dequeue_gen* theedges = dgraph_to_multi_edges(M->l_cayley);
    named_dedges_print(theedges, M, out);

    while (!isempty_dequeue_gen(theedges)) {
        multi_edge* new = rigpull_dequeue_gen(theedges);
        delete_dequeue(new->lab);
        free(new);
    }
    delete_dequeue_gen(theedges);

    fprintf(out, "}\n");
}

/**************************/
/* Affichage sur le shell */
/**************************/

// Affichage d'un NFA
void view_nfa(nfa* thenfa) {
    char tmp_filename[] = "/tmp/nfa-XXXXXX.dot";
    int d = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE* f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    nfa_print(thenfa, f_tmp);
    // nfa_print(thenfa, stdout);
    fclose(f_tmp);

    char* command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, view_command(), NULL);
        fprintf(stderr, "%s\n", command);
    }
    else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ", png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void view_cayley(morphism* thecayley) {
    char tmp_filename[] = "/tmp/cay-XXX.dot";
    int d = mkostemps(tmp_filename, 4, O_APPEND);
    // char png_filename[1 + strlen(tmp_filename)];

    // strcpy(png_filename, tmp_filename);
    // strcpy(png_filename + strlen(tmp_filename) - 3, "png");

    FILE* f_tmp = fdopen(d, "w");

    cayley_print(thecayley, f_tmp);

    fclose(f_tmp);

    char* command = multiple_strcat("dot -Tpng ", tmp_filename, view_command(), NULL);
    // " -o ", png_filename,
    // " && open ", png_filename, NULL);
    system(command);
    free(command);
}

void view_left_cayley(morphism* thecayley) {
    char tmp_filename[] = "/tmp/cay-XXX.dot";
    int d = mkostemps(tmp_filename, 4, O_APPEND);
    // char png_filename[1 + strlen(tmp_filename)];

    // strcpy(png_filename, tmp_filename);
    // strcpy(png_filename + strlen(tmp_filename) - 3, "png");

    FILE* f_tmp = fdopen(d, "w");

    cayley_left_print(thecayley, f_tmp);

    fclose(f_tmp);

    char* command = multiple_strcat("dot -Tpng ", tmp_filename, view_command(), NULL);

    // " -o ", png_filename, " && open ", png_filename,
    system(command);
    free(command);
}
