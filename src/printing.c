#include "printing.h"
#include "monoid_display.h"
#include "type_dequeue.h"
#include <json-c/json.h>
#include <sys/utsname.h>

#define LINUX_VIEW_COMMAND "convert - -resize 70% sixel:- | cat"
// #define OSX_VIEW_COMMAND "| ./imgcat -W auto"
#define OSX_VIEW_COMMAND   "./imgcat -W auto"

static char *view_command(void) {
    struct utsname name;
    uname(&name);
    if (strcmp(name.sysname, "Darwin") == 0) {
        return OSX_VIEW_COMMAND;
    } else {
        return LINUX_VIEW_COMMAND;
    }
}

bool external_viewer = false;

/**********************/
/* Printing functions */
/**********************/

static int edge_sort_ends(const void *a, const void *b) {
    const edge_triple *ea = (const edge_triple *)a;
    const edge_triple *eb = (const edge_triple *)b;
    if (ea->from < eb->from) {
        return -1;
    }

    if (ea->from > eb->from) {
        return 1;
    }

    if (ea->to < eb->to) {
        return -1;
    }

    if (ea->to > eb->to) {
        return 1;
    }

    return ea->lab - eb->lab;
}

static int edge_sort_ends_pairs(const void *a, const void *b) {
    const edge_pair *ea = (const edge_pair *)a;
    const edge_pair *eb = (const edge_pair *)b;
    if (ea->from < eb->from) {
        return -1;
    }

    if (ea->from > eb->from) {
        return 1;
    }

    return ea->to - eb->to;
}

/********************************/
/* Print des arêtes d'un graphe */
/********************************/

void named_nfaedges_print(nfa *A, FILE *out) {
    edge_triple *trans = lgraph_to_edge_list(A->trans, false);
    qsort(trans, A->trans->size_edges, sizeof(edge_triple), edge_sort_ends);

    uint i = 0;
    while (i < A->trans->size_edges) {
        uint q = trans[i].from;
        uint r = trans[i].to;
        fprintf(out, "%d -> %d [label = <", q, r);
        while (i < A->trans->size_edges && trans[i].from == q &&
               trans[i].to == r) {
            uint a = trans[i].lab;
            if (a < A->nb_letters) {
                fprint_letter_gviz(A->alphabet[a], out, false);
            } else if (A->inverse && a < (A->nb_letters << 1)) {
                fprint_letter_gviz(A->alphabet[a - A->nb_letters], out, true);
            } else if (A->epsilon && a == A->trans->size_alpha - 1) {
                fprintf(out, "ε");
            } else {
                fprintf(out, "?");
            }
            i++;
            if (i < A->trans->size_edges && trans[i].from == q &&
                trans[i].to == r) {
                fprintf(out, ",");
            }
        }
        fprintf(out, ">]\n");
    }
    free(trans);
}

void named_dfaedges_print(dfa *A, FILE *out) {
    edge_triple *trans = dgraph_to_edge_list(A->trans, false);
    qsort(trans, A->trans->size_edges, sizeof(edge_triple), edge_sort_ends);

    uint i = 0;
    while (i < A->trans->size_edges) {
        uint q = trans[i].from;
        uint r = trans[i].to;
        fprintf(out, "%d -> %d [label = <", q, r);
        while (i < A->trans->size_edges && trans[i].from == q &&
               trans[i].to == r) {
            uint a = trans[i].lab;
            if (a < A->trans->size_alpha) {
                fprint_letter_gviz(A->alphabet[a], out, false);
            } else {
                fprintf(out, "?");
            }
            i++;
            if (i < A->trans->size_edges && trans[i].from == q &&
                trans[i].to == r) {
                fprintf(out, ",");
            }
        }
        fprintf(out, ">]\n");
    }
    free(trans);
}

void named_moredges_print(morphism *M, bool left, FILE *out) {
    dgraph *g          = left ? M->l_cayley : M->r_cayley;
    edge_triple *trans = dgraph_to_edge_list(g, false);
    qsort(trans, g->size_edges, sizeof(edge_triple), edge_sort_ends);

    uint i = 0;

    while (i < g->size_edges) { // Boucle sur les états de départ

        fprintf(out, "%d -> %d [label = <", trans[i].from, trans[i].to);
        uint q = trans[i].from;
        uint r = trans[i].to;

        while (i < g->size_edges && trans[i].from == q && trans[i].to == r) {
            uint a = trans[i].lab;
            fprint_letter_gviz(M->alphabet[a], out, false);
            i++;
            if (i < g->size_edges && trans[i].from == q && trans[i].to == r) {
                fprintf(out, ",");
            }
        }

        fprintf(out, ">]\n");
    }
    free(trans);
}

void gedges_print(graph *g, FILE *out) {
    edge_pair *trans = graph_to_edge_list(g, false);
    qsort(trans, g->size_edges, sizeof(edge_pair), edge_sort_ends_pairs);

    uint i = 0;

    while (i < g->size_edges) { // Boucle sur les états de départ

        fprintf(out, "%d -> %d\n", trans[i].from, trans[i].to);
        i++;
    }
    free(trans);
}

void dgedges_print(dgraph *g, FILE *out) {
    edge_triple *trans = dgraph_to_edge_list(g, false);
    qsort(trans, g->size_edges, sizeof(edge_triple), edge_sort_ends);

    uint i = 0;

    while (i < g->size_edges) { // Boucle sur les états de départ

        fprintf(out, "%d -> %d [label = <", trans[i].from, trans[i].to);
        uint q = trans[i].from;
        uint r = trans[i].to;

        while (i < g->size_edges && trans[i].from == q && trans[i].to == r) {
            uint a = trans[i].lab;
            fprintf(out, "%c", a + 'a');
            i++;
            if (i < g->size_edges && trans[i].from == q && trans[i].to == r) {
                fprintf(out, ",");
            }
        }

        fprintf(out, ">]\n");
    }
    free(trans);
}

void lgedges_print(lgraph *g, FILE *out) {
    edge_triple *trans = lgraph_to_edge_list(g, false);
    qsort(trans, g->size_edges, sizeof(edge_triple), edge_sort_ends);

    uint i = 0;

    while (i < g->size_edges) { // Boucle sur les états de départ

        fprintf(out, "%d -> %d [label = <", trans[i].from, trans[i].to);
        uint q = trans[i].from;
        uint r = trans[i].to;

        while (i < g->size_edges && trans[i].from == q && trans[i].to == r) {
            uint a = trans[i].lab;
            fprintf(out, "%c", a + 'a');
            i++;
            if (i < g->size_edges && trans[i].from == q && trans[i].to == r) {
                fprintf(out, ",");
            }
        }

        fprintf(out, ">]\n");
    }
    free(trans);
}

/******************/
/* Print d'un NFA */
/******************/

void nfa_print(nfa *A, FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"\n");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n\n");
    fprintf(out, "splines=true;\n\n");

    uint i = 0;
    uint f = 0;

    for (uint k = 0; k < A->trans->size_graph; k++) {
        fprintf(out, "%d [style=solid", k);

        if (A->state_names) {
            fprintf(out, ",label=\"%s\"", A->state_names[k]);
        }
        if ((i < A->nb_initials && A->initials[i] == k) &&
            (f < A->nb_finals && A->finals[f] == k)) {
            fprintf(out, ",fillcolor=\"blue:green\",style=filled,shape = "
                         "doublecircle];\n");
            i++;
            f++;
            continue;
        }

        if (i < A->nb_initials && A->initials[i] == k) {
            fprintf(
                out,
                ",fillcolor=\"blue:green\",style=filled,shape = circle];\n");
            i++;
            continue;
        }

        if (f < A->nb_finals && A->finals[f] == k) {
            fprintf(out, ",shape = doublecircle];\n");
            f++;
            continue;
        }
        fprintf(out, ",shape = circle];\n");
    }

    // Calcul de l'ensemble de transitions

    named_nfaedges_print(A, out);
    fprintf(out, "}\n");
}

void dfa_print(dfa *A, FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"\n");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n\n");
    fprintf(out, "splines=true;\n\n");

    uint f = 0;

    for (uint k = 0; k < A->trans->size_graph; k++) {
        fprintf(out, "%d [style=solid", k);

        if (A->state_names) {
            fprintf(out, ",label=\"%s\"", A->state_names[k]);
        }
        if ((A->initial == k) && (f < A->nb_finals && A->finals[f] == k)) {
            fprintf(out, ",fillcolor=\"blue:green\",style=filled,shape = "
                         "doublecircle];\n");
            f++;
            continue;
        }

        if (A->initial == k) {
            fprintf(
                out,
                ",fillcolor=\"blue:green\",style=filled,shape = circle];\n");
            continue;
        }

        if (f < A->nb_finals && A->finals[f] == k) {
            fprintf(out, ",shape = doublecircle];\n");
            f++;
            continue;
        }
        fprintf(out, ",shape = circle];\n");
    }

    named_dfaedges_print(A, out);

    fprintf(out, "}\n");
}

void graph_print(graph *g, FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"\n");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n\n");
    fprintf(out, "splines=true;\n\n");

    for (uint k = 0; k < g->size; k++) {
        fprintf(out, "%d [style=solid,shape = circle];\n", k);
    }

    gedges_print(g, out);

    fprintf(out, "}\n");
}

void dgraph_print(dgraph *g, FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"\n");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n\n");
    fprintf(out, "splines=true;\n\n");

    for (uint k = 0; k < g->size_graph; k++) {
        fprintf(out, "%d [style=solid,shape = circle];\n", k);
    }

    dgedges_print(g, out);

    fprintf(out, "}\n");
}

void lgraph_print(lgraph *g, FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"\n");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n\n");
    fprintf(out, "splines=true;\n\n");

    for (uint k = 0; k < g->size_graph; k++) {
        fprintf(out, "%d [style=solid,shape = circle];\n", k);
    }

    lgedges_print(g, out);

    fprintf(out, "}\n");
}

void cayley_print(morphism *M, bool left, FILE *out) {
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

        if (f < M->nb_accept && M->accept_list[f] == k) {
            fprintf(out, ">,shape = doublecircle];\n");
            f++;
            continue;
        }
        fprintf(out, ">,shape = circle];\n");
    }

    named_moredges_print(M, left, out);

    fprintf(out, "}\n");
}

void facto_forest_print(morphism *M, facto_forest *forest, FILE *out) {
    fprintf(out, "graph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "ordering=out;\n");
    fprintf(out, "rankdir=TD;\n");

    // Root node
    // fprintf(out, "subgraph cluster_%d\n", nbc++);
    // fprintf(out, "{\n");
    fprintf(out, "%d [style=solid,label=<", forest->root);
    mor_print_name_gviz(M, forest->nodes[forest->root].elem, out);
    switch (forest->nodes[forest->root].type) {
    case FACTO_LEAF:
        fprintf(out, ">,shape = square];\n");
        break;
    case FACTO_BINARY:
        fprintf(out, ">,shape = circle];\n");
        break;
    case FACTO_IDEM:
        fprintf(out, ">,shape = diamond];\n");
        break;
    default:
        fprintf(out, ">,shape = circle];\n");
        break;
    }
    // fprintf(out, "}\n");

    dequeue *q = create_dequeue();
    rigins_dequeue((uint)forest->root, q);
    while (!isempty_dequeue(q)) {
        int n = (int)lefpull_dequeue(q);
        if (forest->nodes[n].type == FACTO_LEAF) {
            continue;
        }
        // fprintf(out, "subgraph cluster_%d\n", nbc++);
        // fprintf(out, "{\n");
        // fprintf(out, "rank = same\n");
        for (int i = 0; i < forest->nodes[n].nb_children; i++) {
            uint child =
                (uint)forest->childrens[forest->nodes[n].st_children + i];
            fprintf(out, "%d [style=solid,label=<", child);
            mor_print_name_gviz(M, forest->nodes[child].elem, out);
            switch (forest->nodes[child].type) {
            case FACTO_LEAF:
                fprintf(out, ">,shape = square];\n");
                break;
            case FACTO_BINARY:
                fprintf(out, ">,shape = circle];\n");
                break;
            case FACTO_IDEM:
                fprintf(out, ">,shape = diamond];\n");
                break;
            default:
                fprintf(out, ">,shape = circle];\n");
                break;
            }
            fprintf(out, "%d -- %d [style=dashed];\n", n, child);
            rigins_dequeue((uint)child, q);
        }
        // fprintf(out, "}\n");
        // for (int i = 0; i < forest->nodes[n].nb_children; i++)
        // {
        //     fprintf(out, "%d -> %d [style=dashed];\n", n,
        //     forest->childrens[forest->nodes[n].st_children + i]); if (i > 0)
        //     {
        //         fprintf(out, "%d -> %d [style=invis];\n",
        //         forest->childrens[forest->nodes[n].st_children + i - 1],
        //         forest->childrens[forest->nodes[n].st_children + i]);
        //     }
        // }
    }
    delete_dequeue(q);
    fprintf(out, "}\n");
}

// subgraph cluster_0
// {
//     style = filled;
//     color = lightgrey;
//     node[style = filled, color = white];
//     a0->a1->a2->a3;
//     label = "process #1";
// }

// // Print des sommets
// for (int k = 0; k < forest->nb_nodes; k++)
// {
//     fprintf(out, "%d [style=solid,label=<", k);
//     mor_print_name_gviz(M, forest->nodes[k].elem, out);
//     fprintf(out, ">,shape = circle];\n");
// }

// for (int k = 0; k < forest->nb_nodes; k++)
// { // Boucle sur les états de départ
//     for (int i = 0; i < forest->nodes[k].nb_children; i++)
//     {
//         fprintf(out, "%d -> %d [style=dashed];\n", k,
//         forest->childrens[forest->nodes[k].st_children + i]); if (i > 0)
//         {
//             fprintf(out, "%d -> %d [style=invis];\n",
//             forest->childrens[forest->nodes[k].st_children + i - 1],
//             forest->childrens[forest->nodes[k].st_children + i]);
//         }
//     }
// }

// fprintf(out, "}\n");
// }

void mor_order_print(morphism *M, graph *G, FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n");

    // Print des sommets
    for (uint k = 0; k < M->r_cayley->size_graph; k++) {
        fprintf(out, "%d [style=solid,label=<", k);
        mor_print_name_gviz(M, k, out);
        fprintf(out, ">,shape = circle];\n");
    }
    gedges_print(G, out);
    fprintf(out, "}\n");
}

void dfa_order_print(dfa *A, graph *G, FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n");

    // Print des sommets
    for (uint k = 0; k < A->trans->size_graph; k++) {
        fprintf(out, "%d [style=solid,label=<", k);
        dfa_print_state(A, k, out);
        fprintf(out, ">,shape = circle];\n");
    }
    gedges_print(G, out);
    fprintf(out, "}\n");
}

/**************/
/*+ Patterns +*/
/**************/

static void pattern_word_print_gviz(dfa *A, uint *theword, FILE *out) {
    uint i = 0;
    if (theword[i] == UINT_MAX) {
        fprintf(out, "ε");
        return;
    }

    while (theword[i] != UINT_MAX) {
        uint a = theword[i];
        fprint_letter_gviz(A->alphabet[a], out, false);
        i++;
    }
}

static void pattern_word_print_utf8(dfa *A, uint *theword, FILE *out) {
    uint i = 0;
    if (theword[i] == UINT_MAX) {
        fprintf(out, "ε");
        return;
    }

    while (theword[i] != UINT_MAX) {
        uint a = theword[i];
        fprint_letter_utf8(A->alphabet[a], out);
        i++;
    }
}

static void pattern_states_print(dfa *A, uint *states, uint nb_states,
                                 FILE *out) {
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n");
    fprintf(out, "nodesep=0.5;\n");
    for (uint k = 0; k < nb_states; k++) {
        if (A->state_names) {
            fprintf(out,
                    "%d "
                    "[style=\"dashed,filled\",color=darkorange,fillcolor="
                    "\"orange:white\",shape = "
                    "circle,label=\"%s\",xlabel=<x<SUB>%d</SUB>>];\n",
                    k, A->state_names[states[k]], k);
        } else {
            fprintf(out,
                    "%d "
                    "[style=\"dashed,filled\",color=darkorange,fillcolor="
                    "\"orange:white\",shape = "
                    "circle,label=\"%d\",xlabel=<x<SUB>%d</SUB>>];\n",
                    k, states[k], k);
        }
    }
}

static void pattern_edge_print(dfa *A, uint from, uint to, pattern_vartype type,
                               uint *theword, char var, FILE *out) {
    switch (type) {
    case PAT_SING:
        fprintf(out, "%d -> %d [label = <{", from, to);
        pattern_word_print_gviz(A, theword, out);
        fprintf(out, "}>,style=solid];\n");
        break;
    case PAT_WORD:
        fprintf(out, "%d -> %d [label = <", from, to);
        pattern_word_print_gviz(A, theword, out);
        fprintf(out, ">,style=solid];\n");
        break;
    case PAT_LANG:
        fprintf(out, "%d -> %d [label = <%c>,style=solid];\n", from, to, var);
        break;
    default:
        break;
    }
}

void sfc_pattern_print(dfa *A, uint *states, uint nb_states, uint *theword,
                       char var, FILE *out) {
    if (!states || !out) {
        return;
    }

    pattern_states_print(A, states, nb_states, out);

    if (theword) {
        for (uint k = 0; k < nb_states; k++) {
            pattern_edge_print(A, k, (k + 1) % nb_states, PAT_WORD, theword,
                               var, out);
        }
    } else {
        for (uint k = 0; k < nb_states; k++) {
            pattern_edge_print(A, k, (k + 1) % nb_states, PAT_LANG, NULL, var,
                               out);
        }
    }
    fprintf(out, "}\n");
}

void view_sfc_pattern(dfa *A, uint *states, uint nb_states, uint *the_word,
                      char var) {
    // Printing the state variable assignements in the pattern.
    fprintf(stdout, "#### The pattern equation fails for n = %d",
            nb_states - 1);
    for (uint i = 0; i < nb_states; i++) {
        if (i == nb_states - 1 && !the_word) {
            fprintf(stdout, " and ");
        } else {
            fprintf(stdout, ", ");
        }
        fprintf(stdout, "x");
        fprint_subsc_utf8(i, stdout);
        fprintf(stdout, " = ");
        dfa_print_state(A, states[i], stdout);
    }

    if (the_word) {
        fprintf(stdout, " and ");
        fprintf(stdout, "%c = ", var);
        pattern_word_print_utf8(A, the_word, stdout);
    }
    fprintf(stdout, ".\n");

    char tmp_filename[] = "/tmp/pat-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }
    sfc_pattern_print(A, states, nb_states, the_word, var, f_tmp);
    // nfa_print(thenfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void dd_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 4, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 2, 3, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);

    fprintf(out, "}\n");
}

void gr_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 3, out);

    pattern_edge_print(A, 1, 0, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 2, 0, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);

    fprintf(out, "}\n");
}

void grp1_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 5, out);

    pattern_edge_print(A, 1, 0, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 2, 0, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 3, 1, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 4, 2, pattern->types[2], pattern->words[2],
                       pattern->vars[2], out);

    fprintf(out, "}\n");
}

void grp2_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 5, out);

    pattern_edge_print(A, 1, 0, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 2, 0, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 1, 3, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 2, 4, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);

    fprintf(out, "}\n");
}

void com_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 5, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 3, 4, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 0, 3, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 1, 2, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);

    for (int i = 0; i < 5; i++) {
        pattern_edge_print(A, i, i, pattern->types[2], pattern->words[2],
                           pattern->vars[2], out);
    }

    fprintf(out, "}\n");
}

void lttcom_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 7, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 5, 6, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 0, 4, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 2, 3, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 1, 2, pattern->types[2], pattern->words[2],
                       pattern->vars[2], out);
    pattern_edge_print(A, 4, 5, pattern->types[2], pattern->words[2],
                       pattern->vars[2], out);

    pattern_edge_print(A, 0, 0, pattern->types[3], pattern->words[3],
                       pattern->vars[3], out);
    pattern_edge_print(A, 2, 2, pattern->types[3], pattern->words[3],
                       pattern->vars[3], out);
    pattern_edge_print(A, 5, 5, pattern->types[3], pattern->words[3],
                       pattern->vars[3], out);

    pattern_edge_print(A, 1, 1, pattern->types[4], pattern->words[4],
                       pattern->vars[4], out);
    pattern_edge_print(A, 3, 3, pattern->types[4], pattern->words[4],
                       pattern->vars[4], out);
    pattern_edge_print(A, 4, 4, pattern->types[4], pattern->words[4],
                       pattern->vars[4], out);
    pattern_edge_print(A, 6, 6, pattern->types[4], pattern->words[4],
                       pattern->vars[4], out);

    fprintf(out, "}\n");
}

void idem_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 3, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 1, 2, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);

    for (int i = 0; i < 3; i++) {
        pattern_edge_print(A, i, i, pattern->types[1], pattern->words[1],
                           pattern->vars[1], out);
    }

    fprintf(out, "}\n");
}

void rtriv_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 2, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 1, 0, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);

    for (uint i = 0; i < 2; i++) {
        pattern_edge_print(A, i, i, pattern->types[2], pattern->words[2],
                           pattern->vars[2], out);
    }

    fprintf(out, "}\n");
}

void ltriv_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 5, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 2, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 3, 4, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 0, 3, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 1, 2, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 4, 3, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);

    for (uint i = 0; i < 5; i++) {
        pattern_edge_print(A, i, i, pattern->types[2], pattern->words[2],
                           pattern->vars[2], out);
    }

    fprintf(out, "}\n");
}

void ltriv_opti_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 3, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 0, 2, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 1, 1, pattern->types[2], pattern->words[2],
                       pattern->vars[2], out);
    pattern_edge_print(A, 2, 2, pattern->types[2], pattern->words[2],
                       pattern->vars[2], out);

    fprintf(out, "}\n");
}

void da_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 4, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 2, 3, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 1, 0, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 3, 2, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 0, 2, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);

    for (uint i = 0; i < 4; i++) {
        pattern_edge_print(A, i, i, pattern->types[2], pattern->words[2],
                           pattern->vars[2], out);
    }

    fprintf(out, "}\n");
}

void pol_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 2, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 0, 0, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 1, 1, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    fprintf(out, "}\n");
}

void polgr_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 2, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 1, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    fprintf(out, "}\n");
}

void knast_pattern_print(dfa *A, generic_pattern *pattern, FILE *out) {
    if (!pattern || !out) {
        return;
    }

    pattern_states_print(A, pattern->states, 6, out);

    pattern_edge_print(A, 0, 1, pattern->types[0], pattern->words[0],
                       pattern->vars[0], out);
    pattern_edge_print(A, 1, 0, pattern->types[1], pattern->words[1],
                       pattern->vars[1], out);
    pattern_edge_print(A, 0, 2, pattern->types[2], pattern->words[2],
                       pattern->vars[2], out);
    pattern_edge_print(A, 3, 2, pattern->types[2], pattern->words[2],
                       pattern->vars[2], out);
    pattern_edge_print(A, 4, 5, pattern->types[2], pattern->words[2],
                       pattern->vars[2], out);
    pattern_edge_print(A, 2, 3, pattern->types[3], pattern->words[3],
                       pattern->vars[3], out);
    pattern_edge_print(A, 1, 4, pattern->types[3], pattern->words[3],
                       pattern->vars[3], out);
    pattern_edge_print(A, 5, 4, pattern->types[3], pattern->words[3],
                       pattern->vars[3], out);

    pattern_edge_print(A, 3, 3, pattern->types[4], pattern->words[4],
                       pattern->vars[4], out);
    pattern_edge_print(A, 0, 0, pattern->types[4], pattern->words[4],
                       pattern->vars[4], out);
    pattern_edge_print(A, 4, 4, pattern->types[4], pattern->words[4],
                       pattern->vars[4], out);

    pattern_edge_print(A, 2, 2, pattern->types[5], pattern->words[5],
                       pattern->vars[5], out);
    pattern_edge_print(A, 1, 1, pattern->types[5], pattern->words[5],
                       pattern->vars[5], out);
    pattern_edge_print(A, 5, 5, pattern->types[5], pattern->words[5],
                       pattern->vars[5], out);

    fprintf(out, "}\n");
}

void view_pattern(dfa *A, generic_pattern *pattern,
                  void (*pattern_print)(dfa *, generic_pattern *, FILE *)) {
    // Counting the number of variables assignements to display.
    uint countvar   = 0;
    uint countstate = 0;
    for (uint i = 0; i < PATTERN_MAXSIZE && pattern->states[i] != UINT_MAX;
         i++) {
        if (pattern->types[i] == PAT_WORD || pattern->types[i] == PAT_SING) {
            countvar++;
        }
        countstate++;
    }

    // Printing the state variable assignements in the pattern.
    fprintf(stdout, "#### The pattern equation fails for");
    for (uint i = 0; i < PATTERN_MAXSIZE && pattern->states[i] != UINT_MAX;
         i++) {
        if (i > 0) {
            if (i == countstate - 1 && countvar == 0) {
                fprintf(stdout, " and ");
            } else {
                fprintf(stdout, ", ");
            }
        } else {
            fprintf(stdout, " ");
        }
        fprintf(stdout, "x");
        fprint_subsc_utf8(i, stdout);
        fprintf(stdout, " = ");
        dfa_print_state(A, pattern->states[i], stdout);
    }

    // Printing the variable assignements in the pattern.
    uint j = 0;
    for (uint i = 0; i < PATTERN_MAXSIZE && j < countvar; i++) {
        // If the variable does not have a printable assignement, we skip
        if ((pattern->types[i] != PAT_WORD && pattern->types[i] != PAT_SING) ||
            !pattern->words[i]) {
            continue;
        }

        // Printing the separator.
        fprintf(stdout, j == countvar - 1 ? " and " : ", ");
        j++;
        fprintf(stdout, "%c = ", pattern->vars[i]);
        if (pattern->types[i] == PAT_SING) {
            fprintf(stdout, "{");
            pattern_word_print_utf8(A, pattern->words[i], stdout);
            fprintf(stdout, "}");
        } else {
            pattern_word_print_utf8(A, pattern->words[i], stdout);
        }
    }
    fprintf(stdout, ".\n");

    if (!pattern_print) {
        return;
    }

    char tmp_filename[] = "/tmp/pat-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }
    pattern_print(A, pattern, f_tmp);
    // nfa_print(thenfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

/**************************/
/* Affichage sur le shell */
/**************************/

// Affichage d'un NFA
void view_nfa(nfa *thenfa) {
    char tmp_filename[] = "/tmp/nfa-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    nfa_print(thenfa, f_tmp);
    // nfa_print(thenfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void view_dfa(dfa *thedfa) {
    char tmp_filename[] = "/tmp/dfa-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    dfa_print(thedfa, f_tmp);
    // dfa_print(thedfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void view_graph(graph *g) {
    char tmp_filename[] = "/tmp/graph-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    graph_print(g, f_tmp);
    // dfa_print(thedfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void view_mor_order(morphism *M, graph *g) {
    char tmp_filename[] = "/tmp/graph-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    mor_order_print(M, g, f_tmp);
    // dfa_print(thedfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void view_dfa_order(dfa *A, graph *g) {
    char tmp_filename[] = "/tmp/graph-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    dfa_order_print(A, g, f_tmp);
    // dfa_print(thedfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void view_dgraph(dgraph *g) {
    char tmp_filename[] = "/tmp/dgraph-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    dgraph_print(g, f_tmp);
    // dfa_print(thedfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void view_lgraph(lgraph *g) {
    char tmp_filename[] = "/tmp/lgraph-XXXXXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "pdf");

    FILE *f_tmp = fdopen(d, "w");

    if (!f_tmp) {
        CRITICAL("Unable to open temporary file");
    }

    lgraph_print(g, f_tmp);
    // dfa_print(thedfa, stdout);
    fclose(f_tmp);

    char *command;

    if (!external_viewer) {
        command = multiple_strcat("dot -Tpng -Gsize=20,25 ", tmp_filename, "| ",
                                  view_command(), NULL);
        // fprintf(stderr, "%s\n", command);
    } else {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ",
                                  png_filename, "&& open ", png_filename, NULL);
    }
    TRACE("%s", command);
    system(command);

    free(command);
}

void view_cayley(morphism *thecayley, bool left) {
    char tmp_filename[] = "/tmp/cay-XXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    // char png_filename[1 + strlen(tmp_filename)];

    // strcpy(png_filename, tmp_filename);
    // strcpy(png_filename + strlen(tmp_filename) - 3, "png");

    FILE *f_tmp         = fdopen(d, "w");

    cayley_print(thecayley, left, f_tmp);

    fclose(f_tmp);

    char *command =
        multiple_strcat("dot -Tpng ", tmp_filename, "| ", view_command(), NULL);
    // " -o ", png_filename,
    // " && open ", png_filename, NULL);
    system(command);
    free(command);
}

void view_facto_forest(morphism *mor, facto_forest *forest) {
    char tmp_filename[] = "/tmp/facto_forest-XXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    // char png_filename[1 + strlen(tmp_filename)];

    // strcpy(png_filename, tmp_filename);
    // strcpy(png_filename + strlen(tmp_filename) - 3, "png");

    FILE *f_tmp         = fdopen(d, "w");

    facto_forest_print(mor, forest, f_tmp);

    fclose(f_tmp);

    char *command =
        multiple_strcat("dot -Tpng ", tmp_filename, "| ", view_command(), NULL);
    // " -o ", png_filename,
    // " && open ", png_filename, NULL);
    system(command);
    free(command);
}

void view_image(const char *filename) {
    char *command =
        multiple_strcat("cat ", filename, "| ", view_command(), NULL);
    // char *command = multiple_strcat(view_command(), filename, NULL);
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
char *tikz_types_names[4] = {
    "CAYLEY_NODE",
    "CAYLEY_TRANS",
    "AUTOMATON_NODE",
    "AUTOMATON_TRANS",
};

void latex_init(void) {
    json_object *root = json_object_from_file("tikz_params.json");
    if (!root) {
        for (int i = 0; i < TIKZ_SIZE; i++) {
            tikz_types[i][0] = '\0';
        }
        return;
    }

    for (int i = 0; i < TIKZ_SIZE; i++) {
        json_object *obj;
        if (!json_object_object_get_ex(root, tikz_types_names[i], &obj) ||
            json_object_get_type(obj) != json_type_string) {
            tikz_types[i][0] = '\0';
            return;
        }
        const char *str = json_object_get_string(obj);
        if (strlen(str) >= 40) {
            tikz_types[i][0] = '\0';
            return;
        }
        strcpy(tikz_types[i], str);
    }

    json_object_put(root);
}

static void latex_print_mono_elem(morphism *M, uint q, FILE *out) {
    dequeue *name = mor_name(M, q);
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
            fprint_letter_latex(M->alphabet[lefread_dequeue(name, i)], out,
                                false);
        } else {
            n++;
        }
    }
    if (n > 1) {
        fprintf(out, "^{%d}", n);
    }
    delete_dequeue(name);
}

static void latex_print_expanded_mono_elem(morphism *M, uint q, FILE *out) {
    dequeue *name = mor_name(M, q);
    if (isempty_dequeue(name)) {
        fprintf(out, "1");
        delete_dequeue(name);
        return;
    }

    for (uint i = 0; i < size_dequeue(name); i++) {
        fprint_letter_latex(M->alphabet[lefread_dequeue(name, i)], out, false);
    }
    delete_dequeue(name);
}

static void latex_print_aux(morphism *M, dgraph *G, FILE *out) {
    latex_init();
    fprintf(out, "\\begin{tikzpicture}\n");

    for (uint i = 0; i < G->size_graph; i++) {
        fprintf(out, "\\node[%s] (", tikz_types[TIKZ_CAYN]);
        latex_print_expanded_mono_elem(M, i, out);
        fprintf(out, ") at (%d,0) {$", 2 * i);
        latex_print_mono_elem(M, i, out);
        fprintf(out, "$};\n");
    }

    edge_triple *trans = dgraph_to_edge_list(G, false);
    qsort(trans, G->size_edges, sizeof(edge_triple), edge_sort_ends);

    uint i = 0;

    while (i < G->size_edges) { // Boucle sur les états de départ
        uint s = trans[i].from;
        uint t = trans[i].to;

        fprintf(out, "\\draw[%s](", tikz_types[TIKZ_CAYT]);
        latex_print_expanded_mono_elem(M, s, out);
        fprintf(out, ") to ");

        if (s == t) {
            fprintf(out, "[loop above] node[above] {$");
        } else if (s < t) {
            fprintf(out, "[bend left=15] node[above] {$");
        } else {
            fprintf(out, "[bend left=15] node[below] {$");
        }

        while (i < G->size_edges && trans[i].from == s && trans[i].to == t) {
            uint a = trans[i].lab;
            fprint_letter_latex(M->alphabet[a], out, false);
            i++;
            if (i < G->size_edges && trans[i].from == s && trans[i].to == t) {
                fprintf(out, ",");
            }
        }
        fprintf(out, "$} (");
        latex_print_expanded_mono_elem(M, t, out);
        fprintf(out, ");\n");
    }
    free(trans);
    fprintf(out, "\\end{tikzpicture}\n");
}

void latex_print_cayley(morphism *M, FILE *out) {
    latex_print_aux(M, M->r_cayley, out);
}

void latex_print_lcayley(morphism *M, FILE *out) {
    latex_print_aux(M, M->l_cayley, out);
}

void latex_print_nfa(nfa *A, FILE *out) {
    latex_init();
    fprintf(out, "\\begin{tikzpicture}\n");

    for (uint i = 0; i < A->trans->size_graph; i++) {
        fprintf(out, "\\node[%s", tikz_types[TIKZ_AUTN]);
        if (bsearch(&i, A->initials, A->nb_initials, sizeof(uint),
                    &compare_uint)) {
            fprintf(out, ",initial below");
        }
        if (bsearch(&i, A->finals, A->nb_finals, sizeof(uint), &compare_uint)) {
            fprintf(out, ",accepting below");
        }
        fprintf(out, "] (n%d) at (%d,0) {$q_%d$};\n", i, 2 * i, i);
    }

    edge_triple *trans = lgraph_to_edge_list(A->trans, false);
    qsort(trans, A->trans->size_edges, sizeof(edge_triple), edge_sort_ends);

    uint i = 0;
    while (i < A->trans->size_edges) {
        fprintf(out, "\\draw[%s] (n%d) to ", tikz_types[TIKZ_CAYT],
                trans[i].from);

        if (trans[i].from == trans[i].to) {
            fprintf(out, "[loop above] node[above] {$");
        } else if (trans[i].from < trans[i].to) {
            fprintf(out, "[bend left=15] node[above] {$");
        } else {
            fprintf(out, "[bend left=15] node[below] {$");
        }

        uint q = trans[i].from;
        uint r = trans[i].to;

        while (i < A->trans->size_edges && trans[i].from == q &&
               trans[i].to == r) {
            uint a = trans[i].lab;
            if (a < A->nb_letters) {
                fprint_letter_latex(A->alphabet[a], out, false);
            } else if (A->inverse && a < (A->nb_letters << 1)) {
                fprint_letter_latex(A->alphabet[a - A->nb_letters], out, true);
            } else if (A->epsilon && a == A->trans->size_alpha - 1) {
                fprintf(out, "\\varepsilon");
            } else {
                fprintf(out, "?");
            }
            i++;
            if (i < A->trans->size_edges && trans[i].from == q &&
                trans[i].to == r) {
                fprintf(out, ",");
            }
        }

        fprintf(out, "$} (n%d);\n", r);
    }
    free(trans);
    fprintf(out, "\\end{tikzpicture}\n");
}

void latex_print_dfa(dfa *A, FILE *out) {
    latex_init();
    fprintf(out, "\\begin{tikzpicture}\n");

    for (uint i = 0; i < A->trans->size_graph; i++) {
        fprintf(out, "\\node[%s", tikz_types[TIKZ_AUTN]);
        if (A->initial == i) {
            fprintf(out, ",initial below");
        }
        if (bsearch(&i, A->finals, A->nb_finals, sizeof(uint), &compare_uint)) {
            fprintf(out, ",accepting below");
        }
        fprintf(out, "] (n%d) at (%d,0) {$q_%d$};\n", i, 2 * i, i);
    }
    edge_triple *trans = dgraph_to_edge_list(A->trans, false);
    qsort(trans, A->trans->size_edges, sizeof(edge_triple), edge_sort_ends);

    uint i = 0;
    while (i < A->trans->size_edges) {
        uint q = trans[i].from;
        uint r = trans[i].to;

        fprintf(out, "\\draw[%s] (n%d) to ", tikz_types[TIKZ_CAYT], q);

        if (q == r) {
            fprintf(out, "[loop above] node[above] {$");
        } else if (q < r) {
            fprintf(out, "[bend left=15] node[above] {$");
        } else {
            fprintf(out, "[bend left=15] node[below] {$");
        }

        while (i < A->trans->size_edges && trans[i].from == q &&
               trans[i].to == r) {
            uint a = trans[i].lab;
            fprint_letter_latex(A->alphabet[a], out, false);
            i++;
            if (i < A->trans->size_edges && trans[i].from == q &&
                trans[i].to == r) {
                fprintf(out, ",");
            }
        }
        fprintf(out, "$} (n%d);\n", r);
    }
    free(trans);
    fprintf(out, "\\end{tikzpicture}\n");
}
