/**
 * @file nfa_patterns.h
 * @brief Patterns equations for DFAs
 */

#ifndef NFA_PATTERNS_H
#define NFA_PATTERNS_H

#include "nfa.h"

void view_pattern_positive(FILE* out);

void view_pattern_example(dfa* A, uint nb_states, uint* states, uint nb_words, char* vars, uint** words, bool print_num, FILE* out);

void view_pattern_intro_default(FILE* out, const char* name);
void view_pattern_intro_strict(FILE* out, const char* name);
bool make_pattern_conclusion_default(bool res, FILE* out);

void view_pattern_dd(void);
void view_pattern_commutative(void);
void view_pattern_idem(void);
void view_pattern_ddidem(void);
void view_pattern_ddcomm(void);
void view_pattern_lttcomm(void);
void view_pattern_group(void);
void view_pattern_grplus(void);
void view_pattern_rtrivialgp(basis mode);
void view_pattern_rtrivialgpplus(basis mode);
void view_pattern_rtrivialbpgp(basis mode);
void view_pattern_rtrivialbpgpplus(basis mode);
void view_pattern_ltrivialgp(basis mode);
void view_pattern_ltrivialgpplus(basis mode);
void view_pattern_ltrivialbpgp(basis mode);
void view_pattern_ltrivialbpgpplus(basis mode);
void view_pattern_dagp(basis mode);
void view_pattern_dagpplus(basis mode);
void view_pattern_dabpgp(basis mode);
void view_pattern_dabpgpplus(basis mode);
void view_pattern_cfreegp(basis mode);
void view_pattern_polgp(basis mode);
void view_pattern_polgpplus(basis mode);
void view_pattern_pol2gp(basis mode);
void view_pattern_pol2gpplus(basis mode);
void view_pattern_bpolgp(basis mode);
void view_pattern_bpolgpplus(basis mode);
void view_pattern_knastgpplus(basis mode);


void view_pattern_knastat(void);
void view_pattern_upolgpplus(basis mode);
void view_pattern_upolbp2gp(basis mode);
void view_pattern_upolbp2gpplus(basis mode);




#endif // NFA_PATTERNS_H
