#include "shell_test.h"
#include "nfa.h"
#include "nfa_determi.h"
#include "shell_languages.h"
#include "shell_sclass.h"
#include "tools.h"
#include "files.h"
#include "parser.h"
#include "parse_regexp.h"
#include "nfa_enum.h"
#include "flint/fmpz.h"
#include "flint/fmpz_mat.h"
#include <stdio.h>

#define DIMEN 3
#define ALPHASIZE 6

void test(void) {

    char testc[60];
    int n = sprint_power_utf8(3457, testc);
    printf("testc: %s\n size: %d\n", testc, n);

    regexp* myexp = parse_string_regexp("(ab)*");
    nfa* aut = reg_glushkov(myexp);
    nfa* aut2 = nfa_brzozowski(aut);
    morphism* M = dfa_to_morphism(aut2, NULL, NULL);
    latex_print_cayley(M, stdout);

    /*
    long i, j;
    fmpz_mat_t A;
    fmpz_mat_t B;
    fmpz_mat_init(A, DIMEN, ALPHASIZE);
    fmpz_mat_init(B, DIMEN, ALPHASIZE);

    for (i = 0; i < DIMEN; i++) {
        for (j = 0; j < ALPHASIZE; j++) {
            fmpz_set_ui(fmpz_mat_entry(A, i, j), rand() % 50);
        }
    }
    fmpz_mat_hnf(B, A);
    flint_printf("A = \n");
    fmpz_mat_print_pretty(A);
    flint_printf("\n");
    flint_printf("H = \n");
    fmpz_mat_print_pretty(B);
    flint_printf("\n");
    fmpz_mat_clear(A);
    fmpz_mat_clear(B);
*/

/*
nfa_enum* E = nfa_enum_init(9, 2);
ulong count = 0;
while (nfa_enum_next(E)) {
    count++;
    if (count % 100000 == 0) {
        printf("count: %lu\n", count);
    }
}

printf("final: %lu\n", count);
*/

//    object_sort_array_advanced(object_compare_mono);
    /*  for (uint i = 0; i < CL_END;i++) {
         class_infos[i](stdout);
     } */
     //regexp* myexp = parse_string_regexp("(a(a(ab)*b)*b)*");
     //reg_print(myexp);
     //nfa* aut = reg_glushkov(myexp);
     //view_nfa(aut);
     //INFO("Test done\n");
     // const char* filename = "automata.json";

     // print_title_box(10, true, stdout, 2, "I've done this when I was 3 y. old.",
     // "B.J.");
     // nfa* Labc = create_sing_epsilon(3);
     // nfa* Lbca = create_sing_epsilon(3);
     // nfa* Lcab = create_sing_epsilon(3);
     // nfa* Lacb = create_sing_epsilon(3);
     // nfa* Lcba = create_sing_epsilon(3);
     // nfa* Lbac = create_sing_epsilon(3);
     // regexp* myexp = parse_string_regexp("(ab)*");
     // reg_print(myexp);

     // uint ab[] = { 0,1 };
     // nfa* W = create_sing(3, ab, 2);
     // nfa* A = nfa_star(W);
     // nfa* B = nfa_brzozowski(A);
     // view_nfa(B);
     // files_save_auto(B, "auto1.json");
     // nfa* C = files_read_auto("auto.json");
     // view_nfa(C);
     // files_save_auto(C, "auto2.json");
     // nfa* D = nfa_brzozowski(C);

     // nfa* ap = nfa_star_plus(a);
     // nfa* as = nfa_star(a);

     // nfa* bp = nfa_star_plus(b);
     // nfa* bs = nfa_star(b);
     // nfa* c = create_sing_letter(3, 2);
     // nfa* cp = nfa_star_plus(c);
     // nfa* cs = nfa_star(c);

     // for (uint i = 0; i < 6; i++) {
     // nfa* Labc1 = nfa_concat_gen(
     // 3, cs, Lbac, nfa_star(nfa_concat_gen(6, ap, Lcba, bp, Lacb, cp, Lbac)));
     // nfa* Lbca1 = nfa_concat_gen(
     // 3, as, Lcba, nfa_star(nfa_concat_gen(6, bp, Lacb, cp, Lbac, ap, Lcba)));
     // nfa* Lcab1 = nfa_concat_gen(
     // 3, bs, Lacb, nfa_star(nfa_concat_gen(6, cp, Lbac, ap, Lcba, bp, Lacb)));
     // nfa* Lacb1 = nfa_concat_gen(
     // 3, bs, Lcab, nfa_star(nfa_concat_gen(6, ap, Lbca, cp, Labc, bp, Lcab)));
     // nfa* Lcba1 = nfa_concat_gen(
     // 3, as, Lbca, nfa_star(nfa_concat_gen(6, cp, Labc, bp, Lcab, ap, Lbca)));
     // nfa* Lbac1 = nfa_concat_gen(
     // 3, cs, Labc, nfa_star(nfa_concat_gen(6, bp, Lcab, ap, Lbca, cp, Labc)));

     // Labc = Labc1;
     // Lbca = Lbca1;
     // Lcab = Lcab1;
     // Lacb = Lacb1;
     // Lcba = Lcba1;
     // Lbac = Lbac1;
     // }
     // nfa* mini = nfa_brzozowski(Labc);

     // view_nfa(mini);

     // nfa* I1 = nfa_concat(Lb, C2);
     // nfa_elimeps_mod(I1);
     // view_nfa(I1, false);

     // nfa* Las = nfa_star_plus(La);
     // nfa* I2 = nfa_concat(Las, Lb);
     // nfa_elimeps_mod(I2);
     // view_nfa(I2, false);

     //// Construction groupes
     // nfa_grdequeueext(I1);
     // nfa_grdequeueext(I2);

     //// Élimination des transitions epsilon pour simplifier le calcul de
     // l'intersection nfa_elimeps_mod(I1); nfa_elimeps_mod(I2);

     // nfa* INTER = nfa_intersect(I1, I2, true);

     //// if (nfa_grdequeuesepar(I1, I2))
     //// {
     ////   print_title_box(100, true, stdout, 1, "They are separable");
     //// }
     //// else
     //// {
     ////   print_title_box(100, true, stdout, 1, "They are NOT separable");
     //// }

     //// nfa_grdequeueext(I1);
     //// nfa_grdequeueext(I2);
     //// view_nfa(I1, false);
     //// view_nfa(I2, false);

     //// nfa* INTER = nfa_intersect(I1, I2, true);
     // view_nfa(INTER, true);

     // nfa* C3 = nfa_brzozowski(C2);

     // nfa* PLUS = nfa_star_plus(C1);
     // nfa* PPLUS = nfa_determinize(PLUS, true);
     // view_nfa(PPLUS, true);
     //// named_nfa_print(PPLUS, &list_print_state, stdout);

     //// printf("test\n");
     // nfa* D1 = nfa_concat_gen(3, La, C2, Lb);
     // nfa* D2 = nfa_star(D1);

     // nfa* Lc = create_sing_letter(3, 2);

     // nfa* E = nfa_concat_gen(3, D2, Lc, D2);

     // nfa* F = nfa_brzozowski(E);
     // nfa* G = nfa_brzozowski(E);

     // view_nfa(F, false);
     // nfa_inv_ext(F);
     // view_nfa(F, false);

     // nfa_grdequeueext(F);
     // view_nfa(F, false);

     // nfa_mod_ext(G);
     // view_nfa(G, false);
}
