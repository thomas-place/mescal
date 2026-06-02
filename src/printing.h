#ifndef PRINTING_H
#define PRINTING_H

#include "alloc.h"
#include "monoid.h"
#include "nfa.h"
#include "nfa_props.h"
#include "tools.h"
#include "type_basic.h"
#include "type_dequeue.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/***********************/
/* Graphviz generation */
/***********************/

void named_nfaedges_print(nfa *A, FILE *out);

void named_dfaedges_print(dfa *A, FILE *out);

void named_moredges_print(morphism *M, bool left, FILE *out);

void gedges_print(graph *g, FILE *out);

void dgedges_print(dgraph *g, FILE *out);

void lgedges_print(lgraph *g, FILE *out);

void nfa_print(nfa *A, FILE *out);

void dfa_print(dfa *A, FILE *out);

void graph_print(graph *g, FILE *out);

void dgraph_print(dgraph *g, FILE *out);

void lgraph_print(lgraph *g, FILE *out);

void cayley_print(morphism *mor, bool left, FILE *out);

void facto_forest_print(morphism *mor, facto_forest *forest, FILE *out);

void mor_order_print(morphism *M, graph *G, FILE *out);

void dfa_order_print(dfa *A, graph *G, FILE *out);

/**************/
/*+ Patterns +*/
/**************/

void sfc_pattern_print(dfa *A, uint *states, uint nb_states, uint *word, char var, FILE *out);

void view_sfc_pattern(dfa *A, uint *states, uint nb_states, uint *word, char var);

void dd_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void gr_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void grp1_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void grp2_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void com_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void lttcom_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void idem_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void rtriv_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void ltriv_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void ltriv_opti_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void da_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void pol_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void polgr_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void knast_pattern_print(dfa *A, generic_pattern *pattern, FILE *out);

void view_pattern(dfa *A, generic_pattern *pattern, void (*pattern_print)(dfa *, generic_pattern *, FILE *));

/****************/
/* Shell output */
/****************/

void view_nfa(nfa *nfa);

void view_dfa(dfa *dfa);

void view_graph(graph *g);

void view_mor_order(morphism *M, graph *g);

void view_dfa_order(dfa *A, graph *g);

void view_dgraph(dgraph *g);

void view_lgraph(lgraph *g);

void view_cayley(morphism *mor, bool left);

void view_facto_forest(morphism *mor, facto_forest *forest);

void view_image(const char *filename);

/********************/
/* Latex generation */
/********************/

void latex_init(void);

void latex_print_nfa(nfa *A, FILE *out);

void latex_print_dfa(dfa *A, FILE *out);

void latex_print_cayley(morphism *M, FILE *out);

void latex_print_lcayley(morphism *M, FILE *out);

#endif
