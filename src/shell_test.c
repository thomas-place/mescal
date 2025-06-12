#include "shell_test.h"
#include "files.h"
#include "flint/fmpz.h"
#include "flint/fmpz_mat.h"
#include "nfa.h"
#include "nfa_determi.h"
#include "nfa_enum.h"
#include "parse_regexp.h"
#include "parser.h"
#include "shell_languages.h"
#include "shell_sclass.h"
#include "tools.h"
#include <stdio.h>

#define DIMEN 3
#define ALPHASIZE 6

extern uint slice_test, width_test;

void test(void) {

    shell_exall_dfatest();

    // dfa_enum* E = dfa_enum_init(7, 2);
    // ulong count = 0;
    // ulong size = 0;
    // ulong sizesfg = 0;
    // ulong countsfg = 0;
    // while (dfa_enum_next(E)) {
    //     count++;
    //     dfa* A = dfa_enum_to_dfa(E);
    //     dfa* MINI = dfa_hopcroft(A);
    //     dfa_delete(A);
    //     morphism* M = dfa_to_morphism(MINI, NULL, NULL);
    //     dfa_delete(MINI);
    //     size = max(size, M->r_cayley->size_graph);
    //     subsemi* S = get_grp_kernel(M, LV_REG);
    //     if (S->rels->HCL->size_par == S->rels->HCL->size_set) {
    //         sizesfg = max(sizesfg, S->original->r_cayley->size_graph);
    //         countsfg++;
    //     }
    //     delete_subsemi(S);
    //     delete_morphism(M);


    //     //size = max(size, dfa_to_morphism_size(MINI));
    //     if (count % 100000 == 0) {
    //         printf("Total count: %lu, Maximum size: %lu, SF(GR) count: %lu, Maximum size SF(GR): %lu\n", count, size, countsfg, sizesfg);
    //     }
    //     //    dfa_delete(MINI);
    // }
    // printf("Final total count: %lu\n", count);
    // printf("Final total size: %lu\n", size);
    // printf("Final SF(GR) count: %lu\n", countsfg);
    // printf("Final SF(GR) size: %lu\n", sizesfg);
    // dfa_enum_free(E);
#if 0
    // Calcule et sauvegarde les automates atteints toutes les 5x10^6 étapes
    nfa_enum* E = nfa_enum_init(7, 2);
    ulong count = 0;
    ulong size = 0;
    char filename[1000];
    char* prefix = "automata/hopcroft-";
    system("mkdir -p automata");
    while (nfa_enum_next(E)) {
        count++;
        nfa* A = nfa_enum_to_nfa(E);
        nfa* MINI = nfa_hopcroft(A);
        // ulong size_mini = dfa_to_morphism_opti(MINI);
        if (count % 100000 == 0) {
            printf("count: %lu\n", count);
        }

        if (count % 5000000 == 0) {
            strcpy(filename, prefix);
            sprintf(filename + strlen(prefix), "%lu", count);
            DEBUG("saving automaton in file: \"%s\".\n", filename);
            if (json_object_to_file(filename, files_auto_to_json(MINI))) {
                DEBUG("Error: failed to save %s.\n", filename);
            }
            else {
                DEBUG("%s saved.\n", filename);
            }
        }
        nfa_delete(MINI);
        nfa_delete(A);
    }
    printf("final count: %lu\n", count);
    printf("final size: %lu\n", size);
    nfa_enum_free(E);
#endif
#if 0
    uint slice = slice_test * width_test + 1;
    uint width = width_test;

    INFO("From %u to %u", slice, slice + width - 1);

    nfa_enum* E = nfa_enum_init(7, 2);

    ulong count = 0;
    ulong size = 0;
    system("mkdir -p automata");
    char filename[1000];
    char* prefix = "automata/hopcroft-";

    while (nfa_enum_next(E)) {
        count++;

        nfa* A = nfa_enum_to_nfa(E);
        if (count < slice) { // we do not treat these cases
            nfa_delete(A);
            continue;
        }
        if (count >= slice + width) {
            nfa_delete(A);
            break;
        }
        nfa* MINI = nfa_hopcroft(A);
        ulong size_mini = dfa_to_morphism_opti(MINI);

        if (size_mini > size) {
            size = size_mini;
            strcpy(filename, prefix);
            sprintf(filename + strlen(prefix), "%lu-%lu", count, size);
            DEBUG("saving automaton in file: \"%s\".\n", filename);
            if (json_object_to_file(filename, files_auto_to_json(MINI))) {
                DEBUG("Error: failed to save %s.\n", filename);
            }
            else {
                DEBUG("%s saved.\n", filename);
            }
        }
        nfa_delete(MINI);
        nfa_delete(A);

        if (count % 5000 == 0) {
            printf("count: %lu, size so far: %lu\n", count, size);
        }
    }
    printf("final count: %lu\n", count);
    printf("final size: %lu\n", size);
    nfa_enum_free(E);
#endif


}
