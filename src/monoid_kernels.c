#include "monoid_kernels.h"
#include "monoid_display.h"
#include "time.h"


/********************/
/*+ AMT-separation +*/
/********************/






nfa* morphism_to_dnfa_kernel(morphism* M) {
    green* G = M->rels;
    nfa* A;
    CALLOC(A, 1);
    A->initials = create_dequeue();
    A->finals = create_dequeue();
    A->alphabet = mor_duplicate_alpha(M);
    A->trans = create_lgraph_noedges(M->r_cayley->size_graph, M->r_cayley->size_alpha);
    for (uint q = 0; q < M->r_cayley->size_graph; q++) {
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            if (M->rels->RCL->numcl[q] == M->rels->RCL->numcl[M->r_cayley->edges[q][a]] || !G->regular_array[M->r_cayley->edges[q][a]]) {
                rigins_dequeue(M->r_cayley->edges[q][a], A->trans->edges[q][a]);
            }
        }
    }
    return A;
}

static nfa* morphism_to_dfa_aux(morphism* M, dgraph* C, parti* P) {
    green* G = M->rels;
    nfa* A;
    CALLOC(A, 1);
    A->initials = create_dequeue();
    A->finals = create_dequeue();
    A->alphabet = mor_duplicate_alpha(M);
    A->trans = create_lgraph_noedges(C->size_graph, C->size_alpha);
    for (uint q = 0; q < C->size_graph; q++) {
        if (G->regular_array[q]) {
            for (uint a = 0; a < C->size_alpha; a++) {
                if (P->numcl[q] == P->numcl[C->edges[q][a]]) {
                    rigins_dequeue(C->edges[q][a], A->trans->edges[q][a]);
                }
            }
        }
    }
    return A;
}

nfa* morphism_to_dfa_rcl(morphism* M) { return morphism_to_dfa_aux(M, M->r_cayley, M->rels->RCL); }

nfa* morphism_to_dfa_lcl(morphism* M) { return morphism_to_dfa_aux(M, M->l_cayley, M->rels->LCL); }

static parti* folding_to_green(parti* F, uint* jord, subsemi* S) {
    uint* numcl;
    MALLOC(numcl, S->size);
    uint size_par = 0;
    for (uint i = 0; i < S->size;i++) {
        numcl[i] = UINT_MAX;
    }


    for (uint i = 0; i < S->size; i++) {

        // We take the i-th element in the order given by the J-classes/
        if (numcl[jord[i]] == UINT_MAX) {
            // The number of the corresponding element in the original monoid.
            uint ms = S->sub_to_mono[jord[i]];

            // Construction of the R-class of jord[i] (it is the FOLD-class of ms).
            uint c = F->numcl[ms];
            for (uint j = 0; j < F->cl_size[c]; j++) {
                uint r = S->mono_to_sub[F->cl_elems[c][j]];
                numcl[r] = size_par;
            }
            size_par++;
        }
    }

    return create_parti(S->size, size_par, numcl);
}



subsemi* get_kernel(morphism* M, sub_level level, basis ba) {
    if (ba == BA_ST) {
        fprintf(stderr, "The ST-kernel is not implemented (this is useless: The ST-kernel is the whole monoid).\n");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG_KERNEL
    printf("Computing the kernel.\n");
    ulong thetime = time(NULL);
#endif




    // Initialization of the submonoid
    subsemi* ker = init_subsemi(M);
    CALLOC(ker->rels, 1);
    ker->neut = ONE;
    ker->size = 0;
    CALLOC(ker->mono_in_sub, M->r_cayley->size_graph);

    bool allreg = mor_all_regular(M);

    // If we only want to compute the regular elements
    if (level == LV_REG || allreg || ba == BA_AMT) {
        // Setting the computation level (if all elements are regular, we do compute everything).
        if (allreg) {
            ker->level = LV_FULL;
        }
        else {
            ker->level = LV_REG;
        }

        // Computing the elements of the kernel in ker->mono_in_sub
        if (ba == BA_AMT) {
            // Special algorithm for the AMT-kernel.
            compute_amt_kernel_regular(M, ker->mono_in_sub, &ker->size);
        }
        else
        {
            // MOD- and GR-kernels are handled via folding.
            parti* FOLD = dgraph_stal_fold(M->r_cayley, M->rels->RCL, ba);
            for (uint k = 0; k < M->nb_idems; k++) {
                uint e = M->idem_list[k];
                // The regular elements in the kernel are the ones folded with an idempotent.
                for (uint j = 0; j < FOLD->cl_size[FOLD->numcl[e]]; j++) {
                    uint s = FOLD->cl_elems[FOLD->numcl[e]][j];
                    //uint s = lefread_dequeue(FOLD->cl[FOLD->numcl[e]], j);
                    if (!ker->mono_in_sub[s]) {
                        ker->mono_in_sub[s] = true;
                        ker->size++;
                    }
                }
            }
            delete_parti(FOLD);
        }

        // Computation of the other tables of the submonoid
        compute_maps_subsemi(ker);
        // Computing the idempotent list
        compute_idems_subsemi(ker);

        // Computes the Green's relations R and L (restriction of those of the original monoid)
        CALLOC(ker->rels, 1);
        ker->rels->RCL = restrict_parti(M->rels->RCL, ker->size, ker->mono_in_sub, ker->mono_to_sub);
        ker->rels->LCL = restrict_parti(M->rels->LCL, ker->size, ker->mono_in_sub, ker->mono_to_sub);

        // Computes the J-classes
        // We first order the elements of the Kernel according to the J order of the original monoid
        // (useful to have the J-classes of the Kernel in a topological order)
        uint* jord = compute_jord_subsemi(ker);
        compute_jrel_subsemi(ker, jord);
        free(jord);

        // Computes the H-classes
        h_green_compute(ker->rels);

        // Groups and regular elements
        gr_green_compute(ker->idem_list, ker->nb_idems, ker->rels);

#ifdef DEBUG_KERNEL
        printf("Fully done. Time: %f\n", difftime(time(NULL), thetime));
#endif
        return ker;
    }


    // If the computation was not restricted to the regular elements, we have to compute the full kernel.
    // This can only happen for MOD and GR (AMT goes always to the regular mode).


    ker->level = LV_FULL;
    nfa* A = morphism_to_dnfa_kernel(M);
    if (ba == BA_MOD) {
        // If we want the MOD-kernel, we project the automaton on the unary alphabet.
        nfa* U = nfa_proj_unary(A);
        nfa_delete(A);
        A = U;
    }
    parti* SCCS = nfa_inv_ext(A);
    parti* FOLD = nfa_stal_fold(A, SCCS);
    delete_parti(SCCS);
    // We compute the Dyck extension of the automaton only necesary if the full kernel has to be computed).
    nfa* B = nfa_dyck_ext(A, FOLD);
    nfa_delete(A);

    for (uint k = 0; k < M->nb_idems; k++) {
        uint e = M->idem_list[k];
        // If we want all elements, there are the one reachable from an idempotent with an epsilon transition.
        for (uint i = 0; i < size_dequeue(B->trans_e->edges[FOLD->numcl[e]]); i++) {
            uint c = lefread_dequeue(B->trans_e->edges[FOLD->numcl[e]], i);
            for (uint j = 0; j < FOLD->cl_size[c]; j++) {
                uint s = FOLD->cl_elems[c][j];
                if (!ker->mono_in_sub[s]) {
                    ker->mono_in_sub[s] = true;
                    ker->size++;
                }
            }

        }
    }
    nfa_delete(B);

    // Computation of the other tables of the submonoid
    compute_maps_subsemi(ker);
    // Computing the idempotent list
    compute_idems_subsemi(ker);



    // Computing the Green relations of the Kernel
    // We first order the elements of the Kernel according to the J order of the original monoid
    // (useful to have the R,L,J-classes of the Kernel in a topological order)
    uint* jord = compute_jord_subsemi(ker);


    // Computing the R-classes (two elements are R-equivalent if they have been folded together).
    ker->rels->RCL = folding_to_green(FOLD, jord, ker);
    delete_parti(FOLD);

    // Computing the L-classes (two elements are R-equivalent if they have been folded together
    // in the left Cayley folding that we compute first).
    FOLD = dgraph_stal_fold(M->l_cayley, M->rels->LCL, ba);
    ker->rels->LCL = folding_to_green(FOLD, jord, ker);
    delete_parti(FOLD);


    // Computing the J-classes.
    compute_jrel_subsemi(ker, jord);
    free(jord);



    // Creation of the H-classes
    h_green_compute(ker->rels);



    // We still have to fill the lists of H-classes which are groups and of the
    // regular elements.
    gr_green_compute(ker->idem_list, ker->nb_idems, ker->rels);

#ifdef DEBUG_KERNEL
    printf("Fully done. Time: %f\n", difftime(time(NULL), thetime));
#endif
    return ker;




}
