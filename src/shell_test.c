#include "shell_test.h"
#include "class_at.h"
#include "class_group.h"
#include "graphs_tarjan.h"
#include "monoid_green.h"
#include "nfa.h"
#include "nfa_determi.h"
#include "shell_languages.h"
#include "tools.h"
#include "files.h"
#include "parser.h"
#include "parse_regexp.h"
#include <stdio.h>

void test(void) {
  // const char* filename = "automata.json";



  //   print_title_box(10, true, stdout, 2, "I've done this when I was 3 y. old.",
//                   "B.J.");
//   p_nfa Labc = create_sing_epsilon(3);
//   p_nfa Lbca = create_sing_epsilon(3);
//   p_nfa Lcab = create_sing_epsilon(3);
//   p_nfa Lacb = create_sing_epsilon(3);
//   p_nfa Lcba = create_sing_epsilon(3);
//   p_nfa Lbac = create_sing_epsilon(3);
  // regexp* myexp = parse_string_regexp("(ab)*");
  // reg_print(myexp);

  // uint ab[] = { 0,1 };
  // p_nfa W = create_sing(3, ab, 2);
  // p_nfa A = nfa_star(W);
  // p_nfa B = nfa_minimize(A);
  // view_nfa(B);
  // files_save_auto(B, "auto1.json");
  // p_nfa C = files_read_auto("auto.json");
  // view_nfa(C);
  // files_save_auto(C, "auto2.json");
  // p_nfa D = nfa_minimize(C);
  // p_cayley CAY = dfa_to_right_cayley(D);
  // p_green G = cayley_green_compute(CAY);
  // print_full_green(CAY, G, false, stdout);

  //   p_nfa ap = nfa_star_plus(a);
//   p_nfa as = nfa_star(a);

//   p_nfa bp = nfa_star_plus(b);
//   p_nfa bs = nfa_star(b);
//   p_nfa c = create_sing_letter(3, 2);
//   p_nfa cp = nfa_star_plus(c);
//   p_nfa cs = nfa_star(c);

//   for (uint i = 0; i < 6; i++) {
//     p_nfa Labc1 = nfa_concat_gen(
//         3, cs, Lbac, nfa_star(nfa_concat_gen(6, ap, Lcba, bp, Lacb, cp, Lbac)));
//     p_nfa Lbca1 = nfa_concat_gen(
//         3, as, Lcba, nfa_star(nfa_concat_gen(6, bp, Lacb, cp, Lbac, ap, Lcba)));
//     p_nfa Lcab1 = nfa_concat_gen(
//         3, bs, Lacb, nfa_star(nfa_concat_gen(6, cp, Lbac, ap, Lcba, bp, Lacb)));
//     p_nfa Lacb1 = nfa_concat_gen(
//         3, bs, Lcab, nfa_star(nfa_concat_gen(6, ap, Lbca, cp, Labc, bp, Lcab)));
//     p_nfa Lcba1 = nfa_concat_gen(
//         3, as, Lbca, nfa_star(nfa_concat_gen(6, cp, Labc, bp, Lcab, ap, Lbca)));
//     p_nfa Lbac1 = nfa_concat_gen(
//         3, cs, Labc, nfa_star(nfa_concat_gen(6, bp, Lcab, ap, Lbca, cp, Labc)));

//     Labc = Labc1;
//     Lbca = Lbca1;
//     Lcab = Lcab1;
//     Lacb = Lacb1;
//     Lcba = Lcba1;
//     Lbac = Lbac1;
//   }
//   p_nfa mini = nfa_minimize(Labc);

//   view_nfa(mini);

  // p_green thegreen = cayley_green_compute(CAY);

  // fprintf(stdout, "R-classes:\n");
  // print_part_cayley(CAY, thegreen->RCL, stdout);
  // fprintf(stdout, "L-classes:\n");
  // print_part_cayley(CAY, thegreen->HCL, stdout);
  // fprintf(stdout, "J-classes:\n");
  // print_part_cayley(CAY, thegreen->JCL, stdout);
  // fprintf(stdout, "H-classes:\n");
  // print_part_cayley(CAY, thegreen->HCL, stdout);

  // for (uint i = 0; i < thegreen->JCL->size_par; i++)
  // {
  //   print_jclass_cayley(CAY, thegreen, lefread_vertices(thegreen->JCL->cl[i],
  //   0), stdout);
  // }

  // print_jclass_cayley(CAY, thegreen, 1, stdout);
  // print_jclass_cayley(CAY, thegreen, 20, stdout);

  // p_nfa I1 = nfa_concat(Lb, C2);
  // nfa_elimeps_mod(I1);
  // view_nfa(I1, false);

  // p_nfa Las = nfa_star_plus(La);
  // p_nfa I2 = nfa_concat(Las, Lb);
  // nfa_elimeps_mod(I2);
  // view_nfa(I2, false);

  // // Construction groupes
  // nfa_grp_ext(I1);
  // nfa_grp_ext(I2);

  // // Élimination des transitions epsilon pour simplifier le calcul de
  // l'intersection nfa_elimeps_mod(I1); nfa_elimeps_mod(I2);

  // p_nfa INTER = nfa_intersect(I1, I2, true);

  // // if (nfa_grp_separ(I1, I2))
  // // {
  // //   print_title_box(100, true, stdout, 1, "They are separable");
  // // }
  // // else
  // // {
  // //   print_title_box(100, true, stdout, 1, "They are NOT separable");
  // // }

  // // nfa_grp_ext(I1);
  // // nfa_grp_ext(I2);
  // // view_nfa(I1, false);
  // // view_nfa(I2, false);

  // // p_nfa INTER = nfa_intersect(I1, I2, true);
  // view_nfa(INTER, true);

  // p_nfa C3 = nfa_minimize(C2);

  // p_cayley CAY = dfa_to_right_cayley(C3);

  // view_cayley(CAY);
  // cayley_print_order(CAY, stdout);

  // p_nfa PLUS = nfa_star_plus(C1);
  // p_nfa PPLUS = nfa_determinize(PLUS, true);
  // view_nfa(PPLUS, true);
  // // named_nfa_print(PPLUS, &list_print_state, stdout);

  // // printf("test\n");
  // p_nfa D1 = nfa_concat_gen(3, La, C2, Lb);
  // p_nfa D2 = nfa_star(D1);

  // p_nfa Lc = create_sing_letter(3, 2);

  // p_nfa E = nfa_concat_gen(3, D2, Lc, D2);

  // p_nfa F = nfa_minimize(E);
  // p_nfa G = nfa_minimize(E);

  // view_nfa(F, false);
  // nfa_inv_ext(F);
  // view_nfa(F, false);

  // nfa_grp_ext(F);
  // view_nfa(F, false);

  // nfa_mod_ext(G);
  // view_nfa(G, false);

  // p_cayley MONO = dfa_to_right_cayley(F);
  // view_cayley(MONO);

  // p_vertices *test = at_table_cayley(MONO);
  // print_at_table(MONO, test, stdout);

  // p_vertices gker = get_group_kernel(MONO);
  // print_sub_cayley_titled(MONO, gker, 100, "G-kernel", stdout);

  // p_vertices modker = get_modulo_kernel(MONO);
  // print_sub_cayley_titled(MONO, modker, 100, "MOD-kernel", stdout);

  // cayley_print_order(MONO, stdout);
}
