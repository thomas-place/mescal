#include "monoid_kernels.h"
#include "flint/fmpz.h"
#include "flint/fmpz_mat.h"
#include "monoid_display.h"

nfa *morphism_to_dfa_kernel(morphism *M) {
    green *G = M->rels;
    nfa *A = nfa_init();
    A->alphabet = mor_duplicate_alpha(M);
    A->trans = create_lgraph_noedges(M->r_cayley->size_graph, M->r_cayley->size_alpha);
    for (uint q = 0; q < M->r_cayley->size_graph; q++) {
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            if (M->rels->RCL->numcl[q] == M->rels->RCL->numcl[M->r_cayley->edges[q][a]] || !G->regular_set[M->r_cayley->edges[q][a]]) {
                rigins_dequeue(M->r_cayley->edges[q][a], A->trans->edges[q][a]);
            }
        }
    }
    return A;
}

static nfa *morphism_to_dfa_aux(morphism *M, dgraph *C, parti *P) {
    green *G = M->rels;
    nfa *A = nfa_init();
    A->alphabet = mor_duplicate_alpha(M);
    A->trans = create_lgraph_noedges(C->size_graph, C->size_alpha);
    for (uint q = 0; q < C->size_graph; q++) {
        if (G->regular_set[q]) {
            for (uint a = 0; a < C->size_alpha; a++) {
                if (P->numcl[q] == P->numcl[C->edges[q][a]]) {
                    rigins_dequeue(C->edges[q][a], A->trans->edges[q][a]);
                }
            }
        }
    }
    return A;
}

nfa *morphism_to_dfa_rcl(morphism *M) { return morphism_to_dfa_aux(M, M->r_cayley, M->rels->RCL); }

nfa *morphism_to_dfa_lcl(morphism *M) { return morphism_to_dfa_aux(M, M->l_cayley, M->rels->LCL); }

static parti *folding_to_green(parti *F, uint *jord, subsemi *S) {
    parti *ret;
    MALLOC(ret, 1);
    ret->size_set = S->size;
    MALLOC(ret->numcl, S->size);
    bool *done;
    CALLOC(done, S->size);
    uint num = 0;
    for (uint i = 0; i < S->size; i++) {

        // We take the i-th element in the order given by the J-classes/
        if (!done[jord[i]]) {
            // The number of the corresponding element in the original monoid.
            uint ms = S->sub_to_mono[jord[i]];

            // Construction of the R-class of jord[i] (it is the FOLD-class of ms).
            uint c = F->numcl[ms];
            for (uint j = 0; j < size_dequeue(F->cl[c]); j++) {
                uint r = S->mono_to_sub[lefread_dequeue(F->cl[c], j)];
                done[r] = true;
                ret->numcl[r] = num;
            }
            num++;
        }
    }
    free(done);

    ret->size_par = num;
    MALLOC(ret->cl, num);

    for (uint i = 0; i < num; i++) {
        ret->cl[i] = create_dequeue();
    }
    for (uint s = 0; s < S->size; s++) {
        rigins_dequeue(s, ret->cl[ret->numcl[s]]);
    }
    return ret;
}

static subsemi *get_kernel(morphism *M, sub_level level, bool mod) {

    // Allocation of the submonoid
    subsemi *ker = init_subsemi(M);
    MALLOC(ker->rels, 1);
    ker->neut = ONE;

    // We use the separation algorithm
    nfa *A;
    if (level == LV_FULL) {
        ker->level = LV_FULL;
        A = morphism_to_dfa_kernel(M);
    } else {
        ker->level = LV_REG;
        A = morphism_to_dfa_rcl(M);
    }

    if (mod) {
        // If we want the MOD-kernel, we project the automaton on the unary alphabet.
        nfa *U = nfa_proj_unary(A);
        delete_nfa(A);
        A = U;
    }

    // We compute the SCCs of the automaton
    parti *SCCS = nfa_inv_ext(A);

    // We compute the folding of the automaton
    parti *FOLD = nfa_stal_fold(A, SCCS);
    delete_parti(SCCS);

    // We compute the Dyck extension of the automaton only necesary if the full kernel has to be computed).
    nfa *B = NULL;
    if (level == LV_FULL) {
        B = nfa_dyck_ext(A, FOLD);
    }
    delete_nfa(A);

    // Computation of the elements of the kernel
    ker->size = 0;
    CALLOC(ker->mono_in_sub, M->r_cayley->size_graph);

    for (uint k = 0; k < size_dequeue(M->idem_list); k++) {
        uint e = lefread_dequeue(M->idem_list, k);

        // If we only want the regular elements, these are the ones folded with an idempotentd
        if (level != LV_FULL) {
            for (uint j = 0; j < size_dequeue(FOLD->cl[FOLD->numcl[e]]); j++) {
                uint s = lefread_dequeue(FOLD->cl[FOLD->numcl[e]], j);
                if (!ker->mono_in_sub[s]) {
                    ker->mono_in_sub[s] = true;
                    ker->size++;
                }
            }
        } else { // If we want all elements, there are the one reachable from an idempotent with an epsilon transition.
            for (uint i = 0; i < size_dequeue(B->trans_e->edges[FOLD->numcl[e]]); i++) {
                uint c = lefread_dequeue(B->trans_e->edges[FOLD->numcl[e]], i);
                for (uint j = 0; j < size_dequeue(FOLD->cl[c]); j++) {
                    uint s = lefread_dequeue(FOLD->cl[c], j);
                    if (!ker->mono_in_sub[s]) {
                        ker->mono_in_sub[s] = true;
                        ker->size++;
                    }
                }
            }
        }
    }
    delete_nfa(B);

    // Computation of the other tables of the submonoid
    compute_maps_subsemi(ker);

    // Computing the idempotent list
    compute_idems_subsemi(ker);

    // Computing the Green relations of the Kernel

    // We first order the elements of the Kernel according to the J order of the original monoid
    // (useful to have the R,L,J-classes of the Kernel in a topological order)
    uint *jord = compute_jord_subsemi(ker);

    // Computing the R-classes (two elements are R-equivalent if they have been folded together).
    ker->rels->RCL = folding_to_green(FOLD, jord, ker);
    delete_parti(FOLD);

    // Computing the L-classes (two elements are R-equivalent if they have been folded together
    // in the left Cayley folding that we compute first).

    A = morphism_to_dfa_lcl(M);
    if (mod) {
        // If we want the MOD-kernel, we project the automaton on the unary alphabet.
        nfa *U = nfa_proj_unary(A);
        delete_nfa(A);
        A = U;
    }

    SCCS = nfa_inv_ext(A);
    FOLD = nfa_stal_fold(A, SCCS);
    delete_parti(SCCS);
    delete_nfa(A);

    ker->rels->LCL = folding_to_green(FOLD, jord, ker);
    delete_parti(FOLD);

    // Computing the J-classes.
    compute_jrel_subsemi(ker, jord);
    free(jord);

    // Creation of the H-classes
    h_green_compute(ker->rels);

    // We still have to fill the lists of H-classes which are groups and of the
    // regular elements.
    gr_green_compute(ker->idem_list, ker->rels);

    return ker;
}

subsemi *get_grp_kernel(morphism *M, sub_level level) { return get_kernel(M, level, false); }

subsemi *get_mod_kernel(morphism *M, sub_level level) { return get_kernel(M, level, true); }

/*****************/
/** AMT-kernels **/
/*****************/

// Solves the system of equations given by the HNF matrix MAT and the target vector.
static bool solve_system_amt(fmpz_mat_t MAT, int *target, uint nb_rows, uint nb_cols) {

    // This Boolean will be set to false if we find that s is not in the kernel.
    bool inside = true;

    uint row = 0;
    for (uint a = 0; a < nb_cols; a++) {
        int val;
        if (row >= nb_rows) {
            val = 0;
        } else {
            val = fmpz_get_si(fmpz_mat_entry(MAT, row, a));
        }

        if (val == 0 && target[a] != 0) {
            inside = false;
            break;
        }
        if (val == 0 && target[a] == 0) {
            continue;
        }
        if (target[a] % val != 0) {
            inside = false;
            break;
        }

        int q = target[a] / val;
        for (uint b = a + 1; b < nb_cols; b++) {
            target[b] -= q * fmpz_get_si(fmpz_mat_entry(MAT, row, b));
        }
        row++;
    }
    return inside;
}

num_span_trees *compute_num_span_trees(morphism *M, bool right) {
    num_span_trees *ret;
    MALLOC(ret, 1);
    if (right) {
        ret->cay = M->r_cayley;
        ret->P = M->rels->RCL;
    } else {
        ret->cay = M->l_cayley;
        ret->P = M->rels->LCL;
    }

    // Initialization of the trees.
    MALLOC(ret->span_trees, ret->cay->size_graph);
    for (uint i = 0; i < ret->cay->size_graph; i++) {
        CALLOC(ret->span_trees[i], ret->cay->size_alpha);
    }

    // Initialization of the dropped edges.
    MALLOC(ret->dropped, ret->P->size_par);
    for (uint i = 0; i < ret->P->size_par; i++) {
        if (M->rels->regular_set[lefread_dequeue(ret->P->cl[i], 0)]) {
            ret->dropped[i] = create_dequeue();
        } else {
            ret->dropped[i] = NULL;
        }
    }

    // Array memorizing the elements already treated.
    bool *visited;
    CALLOC(visited, ret->cay->size_graph);

    // For each idempotent
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        uint e = lefread_dequeue(M->idem_list, i);

        // If the element has already been treated (there was another previously treated idempotent its class), we skip it.
        if (visited[e]) {
            continue;
        }
        // We mark e as visited.
        visited[e] = true;

        // We compute the spanning tree of the R-class of e. The function returns the
        // edges that are not used in the spanning tree. The relevant information for
        // the spanning tree is stored in the array counts. The function also marks
        // the encountered idempotents as treated.

        // Queue for the breadth first search that we will do to build the spanning tree.
        dequeue *thequeue = create_dequeue();

        // We first insert the edges starting at e in the spanning tree inside our queue.
        // (we only consider the edges internal to the R-class of e).
        for (uint a = 0; a < ret->cay->size_alpha; a++) {
            if (ret->P->numcl[e] == ret->P->numcl[ret->cay->edges[e][a]]) {
                rigins_dequeue(e * ret->cay->size_alpha + a, thequeue);
            }
        }

        // While the queue is not empty
        while (!isempty_dequeue(thequeue)) {

            // We take the first edge (r,b,s) in the queue.
            uint p = lefpull_dequeue(thequeue);
            uint r = p / ret->cay->size_alpha;
            uint b = p % ret->cay->size_alpha;
            uint s = ret->cay->edges[r][b];

            // If s has already been treated, (r,b,s) is not in the spanning tree.
            // We put it in the queue of dropped edges.
            if (visited[s]) {
                rigins_dequeue(p, ret->dropped[ret->P->numcl[e]]);
                continue;
            }

            // If s has not been treated, it is part of the spanning tree. We update the
            // counts of the letters on the path from e to s and we insert the edges starting
            // at s in the queue.
            visited[s] = true;
            for (uint a = 0; a < ret->cay->size_alpha; a++) {
                ret->span_trees[s][a] = ret->span_trees[r][a];
                if (ret->P->numcl[s] == ret->P->numcl[ret->cay->edges[s][a]]) {
                    rigins_dequeue(s * ret->cay->size_alpha + a, thequeue);
                }
            }
            ret->span_trees[s][b]++;
        }
        // We do need our queue anymore, we can delete it.
        delete_dequeue(thequeue);
    }

    free(visited);

    return ret;
}

void delete_num_span_trees(num_span_trees *S) {
    if (!S) {
        return;
    }
    for (uint i = 0; i < S->cay->size_graph; i++) {
        free(S->span_trees[i]);
    }
    free(S->span_trees);
    for (uint i = 0; i < S->P->size_par; i++) {
        if (S->dropped[i]) {
            delete_dequeue(S->dropped[i]);
        }
    }
    free(S->dropped);
    free(S);
}

void compute_amt_kernel_regular(morphism *M, bool *mono_in_sub, uint *size) {

    num_span_trees *spans = compute_num_span_trees(M, true);

    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        uint e = lefread_dequeue(M->idem_list, i);
        // If the element has already been treated (there was another previously treated idempotent in its class), we skip it.
        if (mono_in_sub[e]) {
            continue;
        }

        // We compute the matrix corresponding to the cycle base of the R-class of e from the spanning tree and the list of dropped edges.
        uint size_base = size_dequeue(spans->dropped[M->rels->RCL->numcl[e]]);
        fmpz_mat_t MAT;
        fmpz_mat_init(MAT, size_base, M->r_cayley->size_alpha);

        for (uint j = 0; j < size_dequeue(spans->dropped[M->rels->RCL->numcl[e]]); j++) {

            // We take the dropped edge (r,b,s) from the queue.
            uint p = lefread_dequeue(spans->dropped[M->rels->RCL->numcl[e]], j);
            uint r = p / spans->cay->size_alpha;
            uint b = p % spans->cay->size_alpha;
            uint s = spans->cay->edges[r][b];

            // We fill the matrix row corresponding to the cycle closed by this dropped edge.
            for (uint a = 0; a < spans->cay->size_alpha; a++) {
                if (a != b) {
                    fmpz_set_si(fmpz_mat_entry(MAT, j, a), spans->span_trees[r][a] - spans->span_trees[s][a]);
                } else {
                    fmpz_set_si(fmpz_mat_entry(MAT, j, a), spans->span_trees[r][a] - spans->span_trees[s][a] + 1);
                }
            }
        }

        // We put the matrix in HNF.
        fmpz_mat_hnf(MAT, MAT);

        // It remains to solve the sytem of equations given by the HNF for each element of the R-class
        // of the idempotent e. This checks whether the element is in the kernel or not.
        for (uint k = 0; k < size_dequeue(M->rels->RCL->cl[M->rels->RCL->numcl[e]]); k++) {

            // We take the k-th element of the R-class of e.
            uint s = lefread_dequeue(M->rels->RCL->cl[M->rels->RCL->numcl[e]], k);

            // If s is an idempotent, we skip it: it is in the kernel.
            // We do not need to check it.
            if (M->idem_array[s]) {
                mono_in_sub[s] = true;
                (*size)++;
                continue;
            }

            // We now know that s is not an idempotent.
            // We check if it is in the kernel or not.
            if (solve_system_amt(MAT, spans->span_trees[s], size_base, M->r_cayley->size_alpha)) {
                mono_in_sub[s] = true;
                (*size)++;
            }
        }

        // We do not need the matrix anymore, we can delete it.
        fmpz_mat_clear(MAT);
    }

    delete_num_span_trees(spans);
}

void compute_amt_pairs_regular(num_span_trees *rspans, num_span_trees *lspans, uint e, uint f, dequeue *p1, dequeue *p2) {

    // We compute the matrix corresponding to the cycle base (R-class of e + L-class of f).
    uint size_ebase = size_dequeue(rspans->dropped[rspans->P->numcl[e]]);
    uint size_fbase = size_dequeue(lspans->dropped[lspans->P->numcl[f]]);
    fmpz_mat_t MAT;
    fmpz_mat_init(MAT, size_ebase + size_fbase, rspans->cay->size_alpha);

    for (uint j = 0; j < size_ebase; j++) {

        // We take the dropped edge (r,b,s) from the queue.
        uint p = lefread_dequeue(rspans->dropped[rspans->P->numcl[e]], j);
        uint r = p / rspans->cay->size_alpha;
        uint b = p % rspans->cay->size_alpha;
        uint s = rspans->cay->edges[r][b];

        // We fill the matrix row corresponding to the cycle closed by this dropped edge.
        for (uint a = 0; a < rspans->cay->size_alpha; a++) {
            if (a != b) {
                fmpz_set_si(fmpz_mat_entry(MAT, j, a), rspans->span_trees[r][a] - rspans->span_trees[s][a]);
            } else {
                fmpz_set_si(fmpz_mat_entry(MAT, j, a), rspans->span_trees[r][a] - rspans->span_trees[s][a] + 1);
            }
        }
    }
    for (uint j = 0; j < size_fbase; j++) {

        // We take the dropped edge (r,b,s) from the queue.
        uint p = lefread_dequeue(lspans->dropped[lspans->P->numcl[f]], j);
        uint r = p / lspans->cay->size_alpha;
        uint b = p % lspans->cay->size_alpha;
        uint s = lspans->cay->edges[r][b];

        // We fill the matrix row corresponding to the cycle closed by this dropped edge.
        for (uint a = 0; a < lspans->cay->size_alpha; a++) {
            if (a != b) {
                fmpz_set_si(fmpz_mat_entry(MAT, j + size_ebase, a), lspans->span_trees[r][a] - lspans->span_trees[s][a]);
            } else {
                fmpz_set_si(fmpz_mat_entry(MAT, j + size_ebase, a), lspans->span_trees[r][a] - lspans->span_trees[s][a] + 1);
            }
        }
    }

    // We put the matrix in HNF.
    fmpz_mat_hnf(MAT, MAT);

    for (uint i = 0; i < size_dequeue(rspans->P->cl[rspans->P->numcl[e]]); i++) {
        for (uint j = 0; j < size_dequeue(lspans->P->cl[lspans->P->numcl[f]]); j++) {
            uint q = lefread_dequeue(rspans->P->cl[rspans->P->numcl[e]], i);
            uint t = lefread_dequeue(lspans->P->cl[lspans->P->numcl[f]], j);

            int target[rspans->cay->size_alpha];
            for (uint a = 0; a < rspans->cay->size_alpha; a++) {
                target[a] = rspans->span_trees[q][a] + lspans->span_trees[t][a];
            }

            if (solve_system_amt(MAT, target, MAT->r, MAT->c)) {
                rigins_dequeue(q, p1);
                rigins_dequeue(t, p2);
            }
        }
    }

    fmpz_mat_clear(MAT);
}

subsemi *get_amt_kernel(morphism *M, sub_level) {
    // Allocation of the submonoid
    subsemi *ker = init_subsemi(M);
    MALLOC(ker->rels, 1);
    ker->neut = ONE;

    // We use the separation algorithm
    ker->level = LV_REG;

    // Computation of the elements of the kernel
    ker->size = 0;
    CALLOC(ker->mono_in_sub, M->r_cayley->size_graph);
    compute_amt_kernel_regular(M, ker->mono_in_sub, &ker->size);

    // Computation of the other tables of the submonoid
    compute_maps_subsemi(ker);

    // The neutral element is 1
    ker->neut = ker->mono_to_sub[ONE];

    // Computing the idempotent list
    compute_idems_subsemi(ker);

    // Computes the Green's relations R and L (restriction of those of the original monoid)
    MALLOC(ker->rels, 1);
    ker->rels->RCL = restrict_parti(M->rels->RCL, ker->size, ker->mono_in_sub, ker->mono_to_sub);
    ker->rels->LCL = restrict_parti(M->rels->LCL, ker->size, ker->mono_in_sub, ker->mono_to_sub);

    // Computes the J-classes
    // We first order the elements of the Kernel according to the J order of the original monoid
    // (useful to have the J-classes of the Kernel in a topological order)
    uint *jord = compute_jord_subsemi(ker);
    compute_jrel_subsemi(ker, jord);
    free(jord);

    // Computes the H-classes
    h_green_compute(ker->rels);

    // Groups and regular elements
    gr_green_compute(ker->idem_list, ker->rels);

    return ker;
}
