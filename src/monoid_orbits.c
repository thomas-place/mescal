#include "monoid_orbits.h"
#include "monoid_kernels.h"
#include "monoid_display.h"
#include <time.h>

orbits* init_orbits(morphism* M) {
    orbits* new;
    MALLOC(new, 1);
    new->original = M;

    //Other fields are not initialized at this point
    new->orbits = NULL;
    return new;
}

void delete_orbits(orbits* L) {
    if (L == NULL) {
        return;
    }
    for (uint i = 0; i < L->nb_computed; i++) {
        if (L->orbits[i]) {
            delete_subsemi(L->orbits[i]);
        }
    }
    free(L->orbits);
    free(L);
}



/*************/
/* DD-orbits */
/*************/

// Computes the DD-orbit of a given idempotent
subsemi* compute_one_ddorb(morphism* M, uint e) {
    green* G = M->rels;
    subsemi* S = init_subsemi(M);
    S->level = LV_FULL;


    // Computation of the elements inside the orbit
    CALLOC(S->mono_in_sub, M->r_cayley->size_graph);

    // Computes all elements in the orbit
    // If e is the neutral element and there is no non-empty antecedent,
    // Then its orbit is the singleton {1}
    if (e == ONE && !mor_nonempty_neutral(M)) {
        S->size = 1;
        S->mono_in_sub[ONE] = true;
    }
    else {
        dequeue* eM = compute_r_ideal(M, e, NULL);
        dequeue* Me = compute_l_ideal(M, e, NULL);
        dequeue* eMe = make_inter_sorted_dequeue(eM, Me);
        delete_dequeue(eM);
        delete_dequeue(Me);
        S->size = size_dequeue(eMe);
        for (uint i = 0; i < S->size; i++) {
            S->mono_in_sub[lefread_dequeue(eMe, i)] = true;
        }
        delete_dequeue(eMe);
    }

    // Computation of the correspondence maps.
    compute_maps_subsemi(S);

    // The neutral element is e
    S->neut = S->mono_to_sub[e];

    // Computes the lists of idempotents
    compute_idems_subsemi(S);

    // Computes the Green's relations (restriction of those of the original monoid)
    CALLOC(S->rels, 1);
    S->rels->RCL = restrict_parti(G->RCL, S->size, S->mono_in_sub, S->mono_to_sub);
    S->rels->LCL = restrict_parti(G->LCL, S->size, S->mono_in_sub, S->mono_to_sub);
    S->rels->JCL = restrict_parti(G->JCL, S->size, S->mono_in_sub, S->mono_to_sub);
    S->rels->HCL = restrict_parti(G->HCL, S->size, S->mono_in_sub, S->mono_to_sub);

    // Groups and regular elements
    gr_green_compute(S->idem_list, S->nb_idems, S->rels);

    return S;
}

orbits* compute_ddorbits(morphism* M) {
    orbits* res;
    MALLOC(res, 1);
    res->original = M;
    MALLOC(res->orbits, M->nb_min_regular_jcl);
    res->nb_computed = M->nb_min_regular_jcl;
    res->level = LV_FULL;
    for (uint j = 0; j < M->nb_min_regular_jcl; j++) {
        res->orbits[j] = compute_one_ddorb(M, M->regular_idems[j]);
    }
    return res;
}



/**************/
/* G⁺-orbites */
/**************/

subsemi* compute_one_gplusorb(subsemi* ker, uint e) {

    // The original morphism and its Green's relations.
    morphism* M = ker->original;

    subsemi* S = init_subsemi(M);
    S->level = ker->level;


    // Computation of the elements inside the orbit
    CALLOC(S->mono_in_sub, M->r_cayley->size_graph);



    // Computes all elements in the orbit
    // If e is the neutral element and there is no non-empty antecedent,
    // Then its orbit is the singleton {1}
    if (e == ONE && !mor_nonempty_neutral(M)) {
        S->size = 1;
        S->mono_in_sub[ONE] = true;
    }
    else {
        dequeue* eM = compute_r_ideal(M, e, ker->mono_in_sub);
        dequeue* Me = compute_l_ideal(M, e, ker->mono_in_sub);
        dequeue* eMe = make_inter_sorted_dequeue(eM, Me);
        delete_dequeue(eM);
        delete_dequeue(Me);
        S->size = size_dequeue(eMe);
        for (uint i = 0; i < S->size; i++) {
            uint s = lefread_dequeue(eMe, i);
            S->mono_in_sub[s] = true;
        }
        delete_dequeue(eMe);
    }

    // Computation of the correspondence maps.
    compute_maps_subsemi(S);

    // The neutral element is e
    S->neut = S->mono_to_sub[e];

    // Computes the lists of idempotents
    compute_idems_subsemi(S);

    // Computes the Green's relations (restriction of those of the MOD-kernel)
    CALLOC(S->rels, 1);
    S->rels->RCL = restrict_parti_subset(ker->rels->RCL, S->size, S->mono_in_sub, S->mono_to_sub, ker->sub_to_mono);
    S->rels->LCL = restrict_parti_subset(ker->rels->LCL, S->size, S->mono_in_sub, S->mono_to_sub, ker->sub_to_mono);
    S->rels->JCL = restrict_parti_subset(ker->rels->JCL, S->size, S->mono_in_sub, S->mono_to_sub, ker->sub_to_mono);
    S->rels->HCL = restrict_parti_subset(ker->rels->HCL, S->size, S->mono_in_sub, S->mono_to_sub, ker->sub_to_mono);


    // Groups and regular elements.
    gr_green_compute(S->idem_list, S->nb_idems, S->rels);
    return S;
}

orbits* compute_gplusorbits(subsemi* S) {
    morphism* M = S->original;
    orbits* res;
    MALLOC(res, 1);
    res->original = M;
    MALLOC(res->orbits, M->nb_min_regular_jcl);
    res->nb_computed = M->nb_min_regular_jcl;
    res->level = S->level;
    for (uint j = 0; j < M->nb_min_regular_jcl; j++) {
        res->orbits[j] = compute_one_gplusorb(S, M->regular_idems[j]);
    }
    return res;
}




/*************/
/* AT-orbits */
/*************/

// Computes the set of letters that label transitions from the SCC of e
// Return is made as a Boolean array indexed by the alphabet
bool* compute_maxalph_scc(morphism* M, uint e) {
    bool* res;
    MALLOC(res, M->r_cayley->size_alpha);
    for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
        res[a] = false;
    }
    uint c = M->rels->RCL->numcl[e];
    for (uint i = 0; i < M->rels->RCL->cl_size[c]; i++) {
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            if (M->rels->RCL->numcl[M->r_cayley->edges[M->rels->RCL->cl_elems[c][i]][a]] == c) {
                res[a] = true;
            }
        }
    }
    return res;
}

// Merges two alphabets into a new one and tests simultanoeously if these alphabets were equal
static bool merge_subalph(morphism* M, bool* m, bool* s1, bool* s2) {
    bool res = true;
    for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
        if (s1[a] != s2[a]) {
            res = false;
        }
        m[a] = s1[a] && s2[a];
    }
    return res;
}

// Tests if e and f have antecedents sharing a common alphabet
static bool dgraph_seek_alph(dgraph* G, uint ini, uint e, uint f, bool* alph) {

    // Array to memorize visited vertices
    bool visited[G->size_graph];
    for (uint q = 0; q < G->size_graph; q++) {
        visited[q] = false;
    }

    // We memorize the starting vertex
    visited[ini] = true;

    dequeue* thestack = create_dequeue();
    rigins_dequeue(ini, thestack);

    // DFS
    while (!isempty_dequeue(thestack)) {
        if (visited[e] && visited[f]) {
            delete_dequeue(thestack);
            return true;
        }
        uint q = rigpull_dequeue(thestack);

        if (alph) {
            for (uint a = 0; a < G->size_alpha; a++) {
                if (alph[a] && !visited[G->edges[q][a]]) {
                    visited[G->edges[q][a]] = true;
                    rigins_dequeue(G->edges[q][a], thestack);
                }
            }
        }
        else {
            for (uint a = 0; a < G->size_alpha; a++) {

                if (!visited[G->edges[q][a]]) {
                    visited[G->edges[q][a]] = true;
                    rigins_dequeue(G->edges[q][a], thestack);
                }
            }
        }
    }

    delete_dequeue(thestack);
    return false;
}


// Computes the maximal shared alphabet of the idempotents e and f
// The return is made as a Boolean array indexed by the alphabet
// If there exists no shared alphabet, the return is NULL
bool* compute_maxalph_com_scc(morphism* M, uint e, uint f) {
    // Alphabet of the SCC of e
    bool* ealph = compute_maxalph_scc(M, e);
    // Alphabet of the SCC of f
    bool* falph = compute_maxalph_scc(M, f);

    bool* merge;
    MALLOC(merge, M->r_cayley->size_alpha);

    // While the two alphabets are distinct, we consider their intersection (the largest candidate for a common alphabet).
    while (!merge_subalph(M, merge, ealph, falph)) {

        // We check if e and f are still reachable using only the intersection (but not necessarily in full)
        if (!dgraph_seek_alph(M->r_cayley, ONE, e, f, merge)) {
            // If this is not possible, e and f do not have a common alphabet, we return NULL
            free(merge);
            free(ealph);
            free(falph);
            return NULL;
        }

        // We now compute the alphabets of the SCCs of e and f when we restrict the alphabet to the merged one
        parti* rSCCS = dtarjan(M->r_cayley, merge, false);

        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            // We reinitialize the letter a for ealph and falph
            ealph[a] = false;
            falph[a] = false;

            // If the letter a is in the intersection, we look for an internal transition labeled by a in the SCCs restricted to merge of e and f
            if (merge[a]) {
                // We go through all the states of the SCC of e as long as we have not seen a transition a
                for (uint i = 0; i < rSCCS->cl_size[rSCCS->numcl[e]]; i++) {
                    if (rSCCS->numcl[M->r_cayley->edges[rSCCS->cl_elems[rSCCS->numcl[e]][i]][a]] == rSCCS->numcl[e]) {
                        ealph[a] = true;
                        break;
                    }
                }
                // We go through all the states of the SCC of f as long as we have not seen a transition a
                for (uint i = 0; i < rSCCS->cl_size[rSCCS->numcl[f]]; i++) {
                    if (rSCCS->numcl[M->r_cayley->edges[rSCCS->cl_elems[rSCCS->numcl[f]][i]][a]] == rSCCS->numcl[f]) {
                        falph[a] = true;
                        break;
                    }
                }
            }
        }
        delete_parti(rSCCS);
    }
    free(ealph);
    free(falph);
    return merge;
}

subsemi* compute_one_ptorb(morphism* M, uint e, sub_level level) {

#ifdef DEBUG_ORBITS
    printf("\nComputing the BPol(ST)-orbit of ");
    mor_fprint_name_utf8(M, e, stdout);
    printf(".\n");
    ulong thetime = time(NULL);
#endif

    bool* malph = compute_maxalph_scc(M, e);
    subsemi* S = init_subsemi(M);
    S->level = level;


    // Computes the set of elements in the orbit
    dequeue* eM = compute_r_ideal_alph(M, e, malph, NULL);
    dequeue* Me = compute_l_ideal_alph(M, e, malph, NULL);
    dequeue* eMe = make_inter_sorted_dequeue(eM, Me);


    S->size = size_dequeue(eMe);

    MALLOC(S->sub_to_mono, size_dequeue(eMe));
    MALLOC(S->mono_in_sub, M->r_cayley->size_graph);
    MALLOC(S->mono_to_sub, M->r_cayley->size_graph);

    for (uint q = 0; q < M->r_cayley->size_graph; q++) {
        S->mono_in_sub[q] = false;
    }
    for (uint i = 0; i < size_dequeue(eMe); i++) {
        S->sub_to_mono[i] = lefread_dequeue(eMe, i);
        S->mono_to_sub[lefread_dequeue(eMe, i)] = i;
        S->mono_in_sub[lefread_dequeue(eMe, i)] = true;
    }
    S->neut = S->mono_to_sub[e];
    delete_dequeue(eM);
    delete_dequeue(Me);
    delete_dequeue(eMe);

#ifdef DEBUG_ORBITS
    printf("Elements computed. Elements: %d. Time: %f seconds.\n", S->size, difftime(time(NULL), thetime));
#endif


    // Computes the lists of idempotents
    compute_idems_subsemi(S);

#ifdef DEBUG_ORBITS
    printf("Idempotents computed. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif
    // Computes the Green's relations.
    // We first compute the classes R,L,J in the original monoid by restricting the alphabet to malph



    // lgraph* jgraph = ldgraphs_to_lgraph(0, 2, 2, M->r_cayley, M->l_cayley);
    // parti* thej = ltarjan_alph(jgraph, malph);
    // delete_lgraph(jgraph);

    // We may now compute the Green's relations in the submonoid
    CALLOC(S->rels, 1);

    if (level == LV_FULL) {
        parti* right = dtarjan(M->r_cayley, malph, false);
        parti* left = dtarjan(M->l_cayley, malph, false);
        parti* thej = dualdtarjan(M->r_cayley, M->l_cayley, malph, false);
        free(malph);
        S->rels->RCL = restrict_parti(right, S->size, S->mono_in_sub, S->mono_to_sub);
        S->rels->LCL = restrict_parti(left, S->size, S->mono_in_sub, S->mono_to_sub);
        S->rels->JCL = restrict_parti(thej, S->size, S->mono_in_sub, S->mono_to_sub);
        delete_parti(left);
        delete_parti(right);
        delete_parti(thej);

        // H-classes
        h_green_compute(S->rels);

        // Groups and regular elements
        gr_green_compute(S->idem_list, S->nb_idems, S->rels);
    }
    else {
        green_compute_sub_reg(S);
    }



#ifdef DEBUG_ORBITS
    printf("Green relations computed. The end. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif


    return S;


}

orbits* compute_ptorbits(morphism* M, sub_level level) {
    orbits* res;
    MALLOC(res, 1);
    res->original = M;
    MALLOC(res->orbits, M->nb_regular_jcl);
    res->nb_computed = M->nb_regular_jcl;
    if (level == LV_FULL) {
        res->level = LV_FULL;
    }
    else {
        res->level = LV_GREG;
    }

    for (uint j = 0; j < M->nb_regular_jcl; j++) {
        res->orbits[j] = compute_one_ptorb(M, M->regular_idems[j], level);
    }
    return res;
}



/******************/
/* BPol(G)-orbits */
/******************/







static void bpg_fold(morphism* M, uint rcl, parti** fold, dgraph** g_fold, bpg_type type) {
    dgraph* rcl_g = mor_extract_rcl(M, rcl);
    switch (type)
    {
    case BPG_MOD:
        *fold = dgraph_stal_fold(rcl_g, false);
        break;
    case BPG_GR:
        *fold = dgraph_stal_fold(rcl_g, true);
        break;
    case BPG_AMT:
        *fold = compute_amt_fold(rcl_g);
        break;
    default:
        fprintf(stderr, "Error: Invalid bpg_type in bpg_fold.\n");
        delete_dgraph(rcl_g);
        *fold = NULL;
        *g_fold = NULL;
        return;
        break;
    }

    *g_fold = create_dgraph_noedges((*fold)->size_par, M->r_cayley->size_alpha);
    for (uint i = 0; i < (*fold)->size_par; i++) {
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            (*g_fold)->edges[i][a] = UINT_MAX;
        }
    }

    for (uint i = 0; i < rcl_g->size_graph;i++) {
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            if (rcl_g->edges[i][a] != UINT_MAX) {
                (*g_fold)->edges[(*fold)->numcl[i]][a] = (*fold)->numcl[rcl_g->edges[i][a]];
            }
        }
    }
    delete_dgraph(rcl_g);
}



subsemi* compute_one_bpgorb(morphism* M, uint e, sub_level level, bpg_type type) {

#ifdef DEBUG_ORBITS
    switch (type)
    {
    case BPG_MOD:
        printf("\nComputing the BPol(MOD)-orbit of ");
        break;
    case BPG_GR:
        printf("\nComputing the BPol(GR)-orbit of ");
        break;
    case BPG_AMT:
        printf("\nComputing the BPol(AMT)-orbit of ");
        break;
    default:
        printf("\nComputing the BPol(G)-orbit of ");
        break;
    }
    mor_fprint_name_utf8(M, e, stdout);
    printf(".\n");
    ulong thetime = time(NULL);
#endif

    green* G = M->rels;

    // The R-class of e
    uint rcl = G->RCL->numcl[e];
    parti* rcl_fold;
    dgraph* rcl_g_fold;

    bpg_fold(M, rcl, &rcl_fold, &rcl_g_fold, type);

#ifdef DEBUG_ORBITS
    printf("Folding of the R-class computed. Elements: %d. Time: %f seconds.\n", rcl_fold->size_par, difftime(time(NULL), thetime));
#endif



    // Computation of the Pol(G)-pairs
    uint size;
    prod_pair* prod = dgraph_intersec(M->r_cayley, rcl_g_fold, G->RCL->cl_elems[rcl][0], rcl_fold->numcl[0], &size);
    delete_dgraph(rcl_g_fold);


#ifdef DEBUG_ORBITS
    printf("Pol(G)-pairs computed. Size: %d, Time: %f.\n", size, difftime(time(NULL), thetime));
#endif
    // Restriction to the Pol(G)-pairs (ese,e)

    uint je;
    mem_array_sorted(e, G->RCL->cl_elems[rcl], G->RCL->cl_size[rcl], &je);
    uint et = rcl_fold->numcl[je];
    delete_parti(rcl_fold);
    dequeue* protorb = create_dequeue();
    for (uint i = 0; i < size; i++) {
        if (prod[i].q2 == et) {
            rigins_dequeue(prod[i].q1, protorb);
        }
    }
    sort_dequeue_norepeat(protorb);


    dequeue* Me = compute_l_ideal(M, e, NULL);
    dequeue* orbset = make_inter_sorted_dequeue(protorb, Me);
    delete_dequeue(Me);

#ifdef DEBUG_ORBITS
    printf("Elements in the orbits computed. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif

    // We may now construct the orbit.
    subsemi* theorb = init_subsemi(M);
    if (level == LV_FULL) {
        theorb->level = LV_FULL;
    }
    else {
        theorb->level = LV_GREG;
    }


    theorb->size = 0; // The size will be computed later
    CALLOC(theorb->mono_in_sub, M->r_cayley->size_graph);
    MALLOC(theorb->mono_to_sub, M->r_cayley->size_graph);


    for (uint k = 0; k < size_dequeue(orbset); k++) {
        uint ese = lefread_dequeue(orbset, k);
        theorb->mono_in_sub[ese] = true;
        theorb->mono_to_sub[ese] = theorb->size;
        theorb->size++;
    }
    delete_dequeue(protorb);
    delete_dequeue(orbset);

    MALLOC(theorb->sub_to_mono, theorb->size);
    uint n = 0;
    for (uint s = 0; s < M->r_cayley->size_graph; s++) {
        if (theorb->mono_in_sub[s]) {
            theorb->sub_to_mono[n] = s;
            n++;
        }
    }
    theorb->neut = theorb->mono_to_sub[e];
#ifdef DEBUG_ORBITS
    printf("Orbit initialized. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif

    compute_idems_subsemi(theorb);

#ifdef DEBUG_ORBITS
    printf("Idempotents computed. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif

    if (level == LV_FULL) {
        green_compute_sub(theorb);
    }
    else {
        green_compute_sub_reg(theorb);
    }

#ifdef DEBUG_ORBITS
    printf("Green relations computed. The end. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif


    return theorb;
}

orbits* compute_bpgorbits(morphism* M, sub_level level, bpg_type type) {
    orbits* res;
    MALLOC(res, 1);
    res->original = M;
    MALLOC(res->orbits, M->nb_regular_jcl);
    res->nb_computed = M->nb_regular_jcl;
    if (level == LV_FULL) {
        res->level = LV_FULL;
    }
    else {
        res->level = LV_GREG;
    }
    for (uint j = 0; j < M->nb_regular_jcl; j++) {
        res->orbits[j] = compute_one_bpgorb(M, M->regular_idems[j], level, type);
    }
    return res;
}



/*******************/
/* BPol(G⁺)-orbits */
/*******************/


subsemi* compute_one_orbit_from_pairs(morphism* M, uint e, dequeue* eM, bool** pairs, sub_level level) {
    green* G = M->rels;
    // The R-class of e
    uint rcl = G->RCL->numcl[e];


    // We compute the set Me.
    dequeue* Me = compute_l_ideal(M, e, NULL);



    // We may now compute the orbit of e

    subsemi* theorb = init_subsemi(M);
    if (level == LV_FULL) {
        theorb->level = LV_FULL;
    }
    else {
        theorb->level = LV_GREG;
    }



    theorb->size = 0; // The size will be computed later
    CALLOC(theorb->mono_in_sub, M->r_cayley->size_graph);
    MALLOC(theorb->mono_to_sub, M->r_cayley->size_graph);


    // Computes the orbit of e: if (ese,e) is a Pol(G)-pair, then ese is in the orbit.

    uint je;
    mem_array_sorted(e, G->RCL->cl_elems[rcl], G->RCL->cl_size[rcl], &je);
    //mem_dequeue_sorted(e, G->RCL->cl[rcl], &je);
    dequeue* protorb = create_dequeue();
    for (uint i = 0; i < size_dequeue(eM); i++) {
        if (pairs[i][je]) {
            rigins_dequeue(lefread_dequeue(eM, i), protorb);
        }
    }
    dequeue* orbset = make_inter_sorted_dequeue(protorb, Me);

    for (uint k = 0; k < size_dequeue(orbset); k++) {
        uint ese = lefread_dequeue(orbset, k);
        theorb->mono_in_sub[ese] = true;
        theorb->mono_to_sub[ese] = theorb->size;
        theorb->size++;
    }
    delete_dequeue(protorb);
    delete_dequeue(orbset);

    MALLOC(theorb->sub_to_mono, theorb->size);
    uint n = 0;
    for (uint s = 0; s < M->r_cayley->size_graph; s++) {
        if (theorb->mono_in_sub[s]) {
            theorb->sub_to_mono[n] = s;
            n++;
        }
    }
    theorb->neut = theorb->mono_to_sub[e];

    delete_dequeue(Me);

    compute_idems_subsemi(theorb);

    if (level == LV_FULL) {
        green_compute_sub(theorb);
    }
    else {
        green_compute_sub_reg(theorb);
    }
    return theorb;
}

// Computes the elements stabilized by the minimal idempotents in a R-class. 
static void compute_idemstable(morphism* M, uint rcl, dequeue*** mults, bool*** table) {
    green* G = M->rels;
    dequeue** ret1;
    bool** ret2;
    MALLOC(ret1, M->nb_min_regular_jcl - 1);
    MALLOC(ret2, M->nb_min_regular_jcl - 1);
    for (uint i = 1; i < M->nb_min_regular_jcl; i++) {
        ret1[i - 1] = create_dequeue();
        MALLOC(ret2[i - 1], G->RCL->cl_size[rcl]);
        for (uint j = 0; j < G->RCL->cl_size[rcl]; j++) {
            uint q = G->RCL->cl_elems[rcl][j];
            if (mor_mult(M, q, M->regular_idems[i]) == q) {
                rigins_dequeue(q, ret1[i - 1]);
                ret2[i - 1][j] = true;
            }
            else {
                ret2[i - 1][j] = false;
            }
        }
    }
    *mults = ret1;
    *table = ret2;
}



subsemi* compute_one_bpddorb(morphism* M, uint e, sub_level level) {

    // If there exists a non-empty neutral element, the BPol(DD)-orbit is the same as the BPol(ST)-orbit
    // (and the latter is much faster to compute).
    if (mor_nonempty_neutral(M)) {
        return compute_one_ptorb(M, e, level);
    }

    green* G = M->rels;
    // The R-class of e.
    uint rcl = G->RCL->numcl[e];

    // Table indicating which elements of the R-class are stable under right multiplication by a minimal idempotent.
    dequeue** mults;
    bool** idemstable;
    compute_idemstable(M, rcl, &mults, &idemstable);

    // We compute the set eM.
    dequeue* eM = compute_r_ideal(M, e, NULL);


    // Preparation of the array of visited pairs.
    bool** visited;
    MALLOC(visited, size_dequeue(eM));
    for (uint i = 0; i < size_dequeue(eM); i++) {
        CALLOC(visited[i], G->RCL->cl_size[rcl]);
    }

    // Stacks to store the new pairs to treat.
    dequeue* topstack = create_dequeue();
    dequeue* botstack = create_dequeue();
    dequeue* rigidem = create_dequeue();

    // We start from a trivial pair built from an element in the R-class.
    rigins_dequeue(G->RCL->cl_elems[rcl][0], topstack);
    rigins_dequeue(G->RCL->cl_elems[rcl][0], botstack);
    rigins_dequeue(0, rigidem);

    // DFS
    while (!isempty_dequeue(topstack)) {
        uint q = rigpull_dequeue(topstack);
        uint r = rigpull_dequeue(botstack);
        uint l = rigpull_dequeue(rigidem);

        uint iq, ir;
        mem_dequeue_sorted(q, eM, &iq);
        mem_array_sorted(r, G->RCL->cl_elems[rcl], G->RCL->cl_size[rcl], &ir);
        if (visited[iq][ir]) {
            continue;
        }
        visited[iq][ir] = true;


        // For each letter a, we add the pair (qa,ra)
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            if (G->RCL->numcl[M->r_cayley->edges[r][a]] != rcl) {
                continue;
            }
            rigins_dequeue(M->r_cayley->edges[q][a], topstack);
            rigins_dequeue(M->r_cayley->edges[r][a], botstack);
            rigins_dequeue(0, rigidem);
        }

        for (uint k = 1; k < M->nb_min_regular_jcl; k++) {
            if (l == k) {
                continue;
            }
            uint f = M->regular_idems[k];
            uint qf = mor_mult(M, q, f);
            if (qf != q || !idemstable[k - 1][ir]) {
                continue;
            }
            for (uint j = 0; j < size_dequeue(mults[k - 1]); j++) {
                rigins_dequeue(q, topstack);
                rigins_dequeue(lefread_dequeue(mults[k - 1], j), botstack);
                rigins_dequeue(k, rigidem);
            }
        }
    }

    delete_dequeue(topstack);
    delete_dequeue(botstack);
    delete_dequeue(rigidem);
    for (uint k = 0; k < M->nb_min_regular_jcl - 1; k++) {
        free(idemstable[k]);
        delete_dequeue(mults[k]);
    }
    free(idemstable);
    free(mults);

    subsemi* theorb = compute_one_orbit_from_pairs(M, e, eM, visited, level);

    for (uint q = 0; q < size_dequeue(eM); q++) {
        free(visited[q]);
    }
    free(visited);
    delete_dequeue(eM);
    return theorb;
}




static void compute_jmult_orbits(morphism* M, uint rcl, bpg_type type, dequeue**** mults) {

    // We first compute the folding of the R-class according to the type.
    dgraph* rcl_g = mor_extract_rcl(M, rcl);
    parti* fold;
    switch (type)
    {
    case BPG_MOD:
        fold = dgraph_stal_fold(rcl_g, false);
        break;
    case BPG_GR:
        fold = dgraph_stal_fold(rcl_g, true);
        break;
    case BPG_AMT:
        fold = compute_amt_fold(rcl_g);
        break;
    default:
        fprintf(stderr, "Error: Invalid bpg_type in compute_jmult_orbits.\n");
        delete_dgraph(rcl_g);
        *mults = NULL;
        return;
        break;
    }
    delete_dgraph(rcl_g);

    // Preparation of the array of stable elements.
    green* G = M->rels;
    dequeue*** ret1;
    bool** table;
    MALLOC(ret1, M->nb_min_regular_jcl - 1);
    MALLOC(table, M->nb_min_regular_jcl - 1);
    for (uint i = 1; i < M->nb_min_regular_jcl; i++) {
        MALLOC(ret1[i - 1], G->RCL->cl_size[rcl]);
        MALLOC(table[i - 1], G->RCL->cl_size[rcl]);
        for (uint j = 0; j < G->RCL->cl_size[rcl]; j++) {
            uint q = G->RCL->cl_elems[rcl][j];
            if (mor_mult(M, q, M->regular_idems[i]) == q) {
                table[i - 1][j] = true;
            }
            else {
                table[i - 1][j] = false;
            }
        }
    }


    // For every index of an element in set
    for (uint k = 1; k < M->nb_min_regular_jcl; k++) {
        for (uint j = 0; j < G->RCL->cl_size[rcl]; j++) {
            if (!table[k - 1][j]) {
                ret1[k - 1][j] = NULL;
                continue;
            }
            ret1[k - 1][j] = create_dequeue();

            for (uint h = 0; h < fold->cl_size[fold->numcl[j]]; h++) {
                uint r = G->RCL->cl_elems[rcl][fold->cl_elems[fold->numcl[j]][h]];

                if (r != G->RCL->cl_elems[rcl][j] && table[k - 1][fold->cl_elems[fold->numcl[j]][h]]) {
                    rigins_dequeue(r, ret1[k - 1][j]);
                }
            }
        }
    }
    for (uint i = 0; i < M->nb_min_regular_jcl - 1; i++) {
        free(table[i]);
    }
    free(table);
    delete_parti(fold);
    *mults = ret1;
}


subsemi* compute_one_bpgplusorb(morphism* M, uint e, sub_level level, bpg_type type) {
    // If the type is BPG_ST, we compute the BPol(DD)-orbit.
    if (type == BPG_ST) {
        return compute_one_bpddorb(M, e, level);
    }

    // If there exists a non-empty neutral element, the BPol(G⁺)-orbit is the same as the BPol(G)-orbit
    // (and the latter is much faster to compute).
    if (mor_nonempty_neutral(M)) {
        return compute_one_bpgorb(M, e, level, type);
    }

#ifdef DEBUG_ORBITS
    switch (type)
    {
    case BPG_MOD:
        printf("\nComputing the BPol(MOD⁺)-orbit of ");
        break;
    case BPG_GR:
        printf("\nComputing the BPol(GR⁺)-orbit of ");
        break;
    case BPG_AMT:
        printf("\nComputing the BPol(AMT⁺)-orbit of ");
        break;
    default:
        printf("\nComputing the BPol(G⁺)-orbit of ");
        break;
    }
    mor_fprint_name_utf8(M, e, stdout);
    printf(".\n");
    ulong thetime = time(NULL);
#endif

    green* G = M->rels;

    // The R-class of e.
    uint rcl = G->RCL->numcl[e];
    // We compute the set eM.
    dequeue* eM = compute_r_ideal(M, e, NULL);

    // We compute the Pol(G⁺)-pairs in eM x rcl. 
    dequeue*** mults;
    compute_jmult_orbits(M, rcl, type, &mults);

#ifdef DEBUG_ORBITS
    printf("Multiplication table computed. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif


    // Preparation of the array of visited pairs.
    bool** visited;
    MALLOC(visited, size_dequeue(eM));
    for (uint i = 0; i < size_dequeue(eM); i++) {
        CALLOC(visited[i], G->RCL->cl_size[rcl]);
    }

    // Stacks to store the new pairs to treat.
    dequeue* topstack = create_dequeue();
    dequeue* botstack = create_dequeue();
    dequeue* rigidem = create_dequeue();

    // We start from a trivial pair built from an element in the R-class.
    rigins_dequeue(G->RCL->cl_elems[rcl][0], topstack);
    rigins_dequeue(G->RCL->cl_elems[rcl][0], botstack);
    rigins_dequeue(0, rigidem);

    // DFS
    while (!isempty_dequeue(topstack)) {
        uint q = rigpull_dequeue(topstack);
        uint r = rigpull_dequeue(botstack);
        uint l = rigpull_dequeue(rigidem);

        uint iq, ir;
        mem_dequeue_sorted(q, eM, &iq);
        mem_array_sorted(r, G->RCL->cl_elems[rcl], G->RCL->cl_size[rcl], &ir);
        if (visited[iq][ir]) {
            continue;
        }
        visited[iq][ir] = true;


        // For each letter a, we add the pair (qa,ra)
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            if (G->RCL->numcl[M->r_cayley->edges[r][a]] != rcl) {
                continue;
            }
            rigins_dequeue(M->r_cayley->edges[q][a], topstack);
            rigins_dequeue(M->r_cayley->edges[r][a], botstack);
            rigins_dequeue(0, rigidem);
        }

        for (uint k = 1; k < M->nb_min_regular_jcl; k++) {
            if (l == k) {
                continue;
            }
            uint f = M->regular_idems[k];
            uint qf = mor_mult(M, q, f);
            if (qf != q || !mults[k - 1][ir]) {
                continue;
            }
            for (uint j = 0; j < size_dequeue(mults[k - 1][ir]); j++) {
                rigins_dequeue(q, topstack);
                rigins_dequeue(lefread_dequeue(mults[k - 1][ir], j), botstack);
                rigins_dequeue(k, rigidem);
            }
        }
    }

#ifdef DEBUG_ORBITS
    printf("DFS done. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif

    delete_dequeue(topstack);
    delete_dequeue(botstack);
    delete_dequeue(rigidem);
    for (uint k = 1; k < M->nb_min_regular_jcl; k++) {
        for (uint i = 0; i < G->RCL->cl_size[rcl]; i++) {
            if (mults[k - 1][i]) {
                delete_dequeue(mults[k - 1][i]);
            }
        }

        free(mults[k - 1]);
    }

    subsemi* theorb = compute_one_orbit_from_pairs(M, e, eM, visited, level);

#ifdef DEBUG_ORBITS
    printf("Orbit computed. Time: %f seconds.\n", difftime(time(NULL), thetime));
#endif

    for (uint q = 0; q < size_dequeue(eM); q++) {
        free(visited[q]);
    }
    free(visited);
    delete_dequeue(eM);
    return theorb;
}






orbits* compute_bpgplusorbits(morphism* M, sub_level level, bpg_type type) {
    orbits* res;
    MALLOC(res, 1);
    res->original = M;
    MALLOC(res->orbits, M->nb_regular_jcl);
    res->nb_computed = M->nb_regular_jcl;
    if (level == LV_FULL) {
        res->level = LV_FULL;
    }
    else {
        res->level = LV_GREG;
    }
    for (uint j = 0; j < M->nb_regular_jcl; j++) {
        res->orbits[j] = compute_one_bpgplusorb(M, M->regular_idems[j], level, type);
    }
    return res;
}


