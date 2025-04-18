#include "printing.h"
#include "monoid_display.h"
#include <sys/utsname.h>
#include <json-c/json.h>

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
                fprint_letter_gviz(M->alphabet[lefread_dequeue(oneedge->lab, j)], out, false);
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


/********************/
/* Latex generation */
/********************/


enum {
    TIKZ_CAYN,
    TIKZ_CAYT,
    TIKZ_AUTN,
    TIKZ_AUTT,
};


#define TIKZ_SIZE 4
char tikz_types[4][40];
char* tikz_types_names[4] = {
    "CAYLEY_NODE",
    "CAYLEY_TRANS",
    "AUTOMATON_NODE",
    "AUTOMATON_TRANS",
};


void latex_init(void) {
    json_object* root = json_object_from_file("tikz_params.json");
    if (!root) {
        for (int i = 0; i < TIKZ_SIZE; i++) {
            tikz_types[i][0] = '\0';
        }
        return;
    }

    for (int i = 0; i < TIKZ_SIZE; i++) {
        json_object* obj;
        if (!json_object_object_get_ex(root, tikz_types_names[i], &obj) || json_object_get_type(obj) != json_type_string) {
            tikz_types[i][0] = '\0';
            return;
        }
        const char* str = json_object_get_string(obj);
        if (strlen(str) >= 40) {
            tikz_types[i][0] = '\0';
            return;
        }
        strcpy(tikz_types[i], str);
    }

    json_object_put(root);
}


static void latex_print_mono_elem(morphism* M, uint q, FILE* out) {
    dequeue* name = mor_name(M, q);
    if (isempty_dequeue(name)) {
        fprintf(out, "1");
        delete_dequeue(name);
        return;
    }
    uint n = 1;
    fprint_letter_latex(M->alphabet[lefread_dequeue(name, 0)], out, false);
    for (uint i = 1; i < size_dequeue(name); i++) {
        if (lefread_dequeue(name, i) != lefread_dequeue(name, i - 1)) {
            if (n > 1) {
                fprintf(out, "^{%d}", n);
            }
            n = 1;
            fprint_letter_latex(M->alphabet[lefread_dequeue(name, i)], out, false);
        }
        else {
            n++;
        }
    }
    if (n > 1) {
        fprintf(out, "^{%d}", n);
    }
    delete_dequeue(name);
}


static void latex_print_aux(morphism* M, dgraph* G, FILE* out) {
    latex_init();
    fprintf(out, "\\begin{tikzpicture}\n");

    for (uint i = 0; i < G->size_graph; i++) {
        fprintf(out, "\\node[%s] (n%d) at (%d,0) {$", tikz_types[TIKZ_CAYN], i, 2 * i);
        latex_print_mono_elem(M, i, out);
        fprintf(out, "$};\n");
    }

    dequeue_gen* theedges = dgraph_to_multi_edges(M->r_cayley);
    while (!isempty_dequeue_gen(theedges)) {
        multi_edge* new = rigpull_dequeue_gen(theedges);

        fprintf(out, "\\draw[%s] (n%d) to ", tikz_types[TIKZ_CAYT], new->in);

        if (new->in == new->out) {
            fprintf(out, "[loop above] node[above] {$");
        }
        else if (new->in < new->out) {
            fprintf(out, "[out=0,in=180] node[above] {$");
        }
        else {
            fprintf(out, "[out=180,in=0] node[above] {$");
        }
        for (uint j = 0; j < size_dequeue(new->lab); j++) {
            if (j > 0) {
                fprintf(out, ",");
            }
            fprint_letter_latex(M->alphabet[lefread_dequeue(new->lab, j)], out, false);
        }
        fprintf(out, "$} (n%d);\n", new->out);
        delete_dequeue(new->lab);
        free(new);
    }
    delete_dequeue_gen(theedges);
    fprintf(out, "\\end{tikzpicture}\n");

}

void latex_print_cayley(morphism* M, FILE* out) {
    latex_print_aux(M, M->r_cayley, out);
}

void latex_print_lcayley(morphism* M, FILE* out) {
    latex_print_aux(M, M->l_cayley, out);
}


void latex_print_automaton(nfa* A, FILE* out) {
    latex_init();
    fprintf(out, "\\begin{tikzpicture}\n");

    for (uint i = 0; i < A->trans->size_graph; i++) {
        fprintf(out, "\\node[%s", tikz_types[TIKZ_AUTN]);
        if (mem_dequeue_sorted(i, A->initials, NULL)) {
            fprintf(out, ",initial below");
        }
        if (mem_dequeue_sorted(i, A->finals, NULL)) {
            fprintf(out, ",accepting below");
        }
        fprintf(out, "] (n%d) at (%d,0) {$%d$};\n", i, 2 * i, i);
    }

    dequeue_gen* theedges = nfa_to_multi_edges(A);
    while (!isempty_dequeue_gen(theedges)) {
        multi_edge* new = rigpull_dequeue_gen(theedges);

        fprintf(out, "\\draw[%s] (n%d) to ", tikz_types[TIKZ_CAYT], new->in);

        if (new->in == new->out) {
            fprintf(out, "[loop above] node[above] {$");
        }
        else if (new->in < new->out) {
            fprintf(out, "[out=0,in=180] node[above] {$");
        }
        else {
            fprintf(out, "[out=180,in=0] node[above] {$");
        }

        if (new->eps) {
            fprintf(out, "\\varepsilon");
            if (!isempty_dequeue(new->lab) || !isempty_dequeue(new->lab_i)) {
                fprintf(out, ",");
            }
        }
        for (uint j = 0; j < size_dequeue(new->lab); j++) {
            if (j > 0 || !isempty_dequeue(new->lab_i)) {
                fprintf(out, ",");
            }
            fprint_letter_latex(A->alphabet[lefread_dequeue(new->lab, j)], out, false);
        }
        for (uint j = 0; j < size_dequeue(new->lab_i); j++) {
            if (j > 0) {
                fprintf(out, ",");
            }
            fprint_letter_latex(A->alphabet[lefread_dequeue(new->lab, j)], out, true);
        }

        fprintf(out, "$} (n%d);\n", new->out);
        delete_dequeue(new->lab);
        free(new);
    }
    delete_dequeue_gen(theedges);
    fprintf(out, "\\end{tikzpicture}\n");

}
