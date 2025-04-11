#include "monoid_orbits.h"

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
    MALLOC(S->rels, 1);
    S->rels->RCL = restrict_parti(G->RCL, S->size, S->mono_in_sub, S->mono_to_sub);
    S->rels->LCL = restrict_parti(G->LCL, S->size, S->mono_in_sub, S->mono_to_sub);
    S->rels->JCL = restrict_parti(G->JCL, S->size, S->mono_in_sub, S->mono_to_sub);
    S->rels->HCL = restrict_parti(G->HCL, S->size, S->mono_in_sub, S->mono_to_sub);

    // Groups and regular elements
    gr_green_compute(S->idem_list, S->rels);

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
        res->orbits[j] = compute_one_ddorb(M, M->min_regular_idems[j]);
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
    MALLOC(S->rels, 1);
    S->rels->RCL = restrict_parti_subset(ker->rels->RCL, S->size, S->mono_in_sub, S->mono_to_sub, ker->sub_to_mono);
    S->rels->LCL = restrict_parti_subset(ker->rels->LCL, S->size, S->mono_in_sub, S->mono_to_sub, ker->sub_to_mono);
    S->rels->JCL = restrict_parti_subset(ker->rels->JCL, S->size, S->mono_in_sub, S->mono_to_sub, ker->sub_to_mono);
    S->rels->HCL = restrict_parti_subset(ker->rels->HCL, S->size, S->mono_in_sub, S->mono_to_sub, ker->sub_to_mono);


    // Groups and regular elements.
    gr_green_compute(S->idem_list, S->rels);
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
        res->orbits[j] = compute_one_gplusorb(S, M->min_regular_idems[j]);
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
    for (uint i = 0; i < size_dequeue(M->rels->RCL->cl[c]); i++) {
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            if (M->rels->RCL->numcl[M->r_cayley->edges[lefread_dequeue(M->rels->RCL->cl[c], i)][a]] == c) {
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
        parti* rSCCS = dtarjan_alph(M->r_cayley, merge);

        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            // We reinitialize the letter a for ealph and falph
            ealph[a] = false;
            falph[a] = false;

            // If the letter a is in the intersection, we look for an internal transition labeled by a in the SCCs restricted to merge of e and f
            if (merge[a]) {
                // We go through all the states of the SCC of e as long as we have not seen a transition a
                for (uint i = 0; i < size_dequeue(rSCCS->cl[rSCCS->numcl[e]]); i++) {
                    if (rSCCS->numcl[M->r_cayley->edges[lefread_dequeue(rSCCS->cl[rSCCS->numcl[e]], i)][a]] == rSCCS->numcl[e]) {
                        ealph[a] = true;
                        break;
                    }
                }
                // We go through all the states of the SCC of f as long as we have not seen a transition a
                for (uint i = 0; i < size_dequeue(rSCCS->cl[rSCCS->numcl[f]]); i++) {
                    if (rSCCS->numcl[M->r_cayley->edges[lefread_dequeue(rSCCS->cl[rSCCS->numcl[f]], i)][a]] == rSCCS->numcl[f]) {
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

subsemi* compute_one_ptorb(morphism* M, uint e) {
    bool* malph = compute_maxalph_scc(M, e);
    subsemi* S = init_subsemi(M);
    S->level = LV_FULL;

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


    // Computes the lists of idempotents
    compute_idems_subsemi(S);

    // Computes the Green's relations.
    // We first compute the classes R,L,J in the original monoid by restricting the alphabet to malph
    parti* right = dtarjan_alph(M->r_cayley, malph);

    parti* left = dtarjan_alph(M->l_cayley, malph);

    lgraph* jgraph = ldgraphs_to_lgraph(0, 2, 2, M->r_cayley, M->l_cayley);
    parti* thej = ltarjan_alph(jgraph, malph);
    delete_lgraph(jgraph);

    // We may now compute the Green's relations in the submonoid
    MALLOC(S->rels, 1);

    S->rels->RCL = restrict_parti(right, S->size, S->mono_in_sub, S->mono_to_sub);
    S->rels->LCL = restrict_parti(left, S->size, S->mono_in_sub, S->mono_to_sub);
    S->rels->JCL = restrict_parti(thej, S->size, S->mono_in_sub, S->mono_to_sub);

    // H-classes
    h_green_compute(S->rels);

    // Groups and regular elements
    gr_green_compute(S->idem_list, S->rels);

    delete_parti(left);
    delete_parti(right);
    delete_parti(thej);
    free(malph);
    return S;
}

orbits* compute_ptorbits(morphism* M) {
    orbits* res;
    MALLOC(res, 1);
    res->original = M;
    MALLOC(res->orbits, M->rels->nb_regular_jcl);
    res->nb_computed = M->rels->nb_regular_jcl;
    res->level = LV_FULL;
    for (uint j = 0; j < M->rels->nb_regular_jcl; j++) {
        res->orbits[j] = compute_one_ptorb(M, M->rels->regular_idems[j]);
    }
    return res;
}



/******************/
/* BPol(G)-orbits */
/******************/

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
    mem_dequeue_sorted(e, G->RCL->cl[rcl], &je);
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

uint** compute_jmult(morphism* M, dequeue* set, dequeue* jcl) {
    green* G = M->rels;

    // The table of multiplication we are going to fill.
    uint** table;
    MALLOC(table, size_dequeue(set));

    // A Boolean array for the DFS.
    bool* visited;
    MALLOC(visited, size_dequeue(jcl));

    // Two stacks for the DFS.
    dequeue* fromcur = create_dequeue();
    dequeue* fromone = create_dequeue();

    // For every index of an element in set
    for (uint i = 0; i < size_dequeue(set); i++) {

        // The element.
        uint s = lefread_dequeue(set, i);

        // Allocation of the line of the table: multiplication of this elements with the J-class.
        MALLOC(table[i], size_dequeue(jcl));

        // Initializing the structures for the computation.
        for (uint j = 0; j < size_dequeue(jcl); j++) {
            visited[j] = false;
            table[i][j] = M->r_cayley->size_graph; // Value used to mark a multiplication that goes out of the J-class
        }

        // For every element e of the J-class
        for (uint j = 0; j < size_dequeue(jcl); j++) {
            uint e = lefread_dequeue(jcl, j);

            // If e is an idempotent such that e L s (we have s = se)
            // We compute the multiplications with all elements in the R-class.
            if (M->idem_array[e] && G->LCL->numcl[e] == G->LCL->numcl[s]) {

                rigins_dequeue(e, fromone);
                rigins_dequeue(s, fromcur);
                while (!isempty_dequeue(fromone)) {
                    uint t = rigpull_dequeue(fromone);
                    uint st = rigpull_dequeue(fromcur);

                    uint ist, jt;

                    if (!mem_dequeue_sorted(st, set, &ist) || !mem_dequeue_sorted(t, jcl, &jt) || visited[jt]) {
                        continue;
                    }
                    visited[jt] = true;
                    table[i][jt] = st;
                    for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
                        rigins_dequeue(M->r_cayley->edges[t][a], fromone);
                        rigins_dequeue(M->r_cayley->edges[st][a], fromcur);
                    }
                }
            }
        }
    }
    delete_dequeue(fromcur);
    delete_dequeue(fromone);
    free(visited);
    return table;
}

bool** compute_polgpairs(subsemi* S, dequeue* rideal, uint rcl) {
    morphism* M = S->original;
    green* G = S->original->rels;


    // The J-class that contains the R-class.
    uint jcl = G->JCL->numcl[lefread_dequeue(G->RCL->cl[rcl], 0)];

    // Computes the multiplication table between the R-class and the J-class.
    uint** table = compute_jmult(M, G->RCL->cl[rcl], G->JCL->cl[jcl]);



    dequeue* jc_inker = create_dequeue();
    for (uint i = 0; i < size_dequeue(G->JCL->cl[jcl]); i++) {
        if (S->mono_in_sub[lefread_dequeue(G->JCL->cl[jcl], i)]) {

            rigins_dequeue(i, jc_inker);
        }
    }

    // Preparation of the array to return
    bool** visited;
    MALLOC(visited, size_dequeue(rideal));
    for (uint i = 0; i < size_dequeue(rideal); i++) {
        CALLOC(visited[i], size_dequeue(G->RCL->cl[rcl]));
    }

    // Stacks that store the new pairs to treat
    dequeue* topstack = create_dequeue();
    dequeue* botstack = create_dequeue();

    // We start start from a trivial pair built from an element in the R-class
    rigins_dequeue(lefread_dequeue(G->RCL->cl[rcl], 0), topstack);
    rigins_dequeue(lefread_dequeue(G->RCL->cl[rcl], 0), botstack);



    // DFS
    while (!isempty_dequeue(topstack)) {
        uint q = rigpull_dequeue(topstack);
        uint r = rigpull_dequeue(botstack);

        uint iq, ir;

        if (!mem_dequeue_sorted(q, rideal, &iq) || !mem_dequeue_sorted(r, G->RCL->cl[rcl], &ir) || visited[iq][ir]) {
            continue;
        }
        visited[iq][ir] = true;

        // For each letter a, we add the pair (qa,ra) 
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            rigins_dequeue(M->r_cayley->edges[q][a], topstack);
            rigins_dequeue(M->r_cayley->edges[r][a], botstack);
        }


        // For each element s in the kernel such that s J e, we take the pair (q,rs)
        for (uint j = 0; j < size_dequeue(jc_inker); j++) {
            uint s = table[ir][lefread_dequeue(jc_inker, j)];

            // We are only intersted in this pair if rs R e
            if (s != M->r_cayley->size_graph) {
                rigins_dequeue(q, topstack);
                rigins_dequeue(s, botstack);
            }
        }
    }
    delete_dequeue(jc_inker);
    delete_dequeue(topstack);
    delete_dequeue(botstack);
    for (uint i = 0; i < size_dequeue(G->RCL->cl[rcl]); i++) {
        free(table[i]);
    }
    free(table);
    return visited;
}

subsemi* compute_one_bpgorb(subsemi* S, uint e, sub_level level) {
    morphism* M = S->original;
    green* G = S->original->rels;

    // The idempotent whose R-class we must compute

    // The R-class of e
    uint rcl = G->RCL->numcl[e];

    // We compute the set eM
    dequeue* eM = compute_r_ideal(M, e, NULL);

    // Computes the Pol(G)-pairs
    bool** polgp = compute_polgpairs(S, eM, rcl);

    subsemi* theorb = compute_one_orbit_from_pairs(M, e, eM, polgp, level);


    for (uint q = 0; q < size_dequeue(eM); q++) {
        free(polgp[q]);
    }
    free(polgp);

    delete_dequeue(eM);

    return theorb;
}

orbits* compute_bpgorbits(subsemi* S, sub_level level) {
    morphism* M = S->original;
    orbits* res;
    MALLOC(res, 1);
    res->original = M;
    MALLOC(res->orbits, M->rels->nb_regular_jcl);
    res->nb_computed = M->rels->nb_regular_jcl;
    if (level == LV_FULL) {
        res->level = LV_FULL;
    }
    else {
        res->level = LV_GREG;
    }
    for (uint j = 0; j < M->rels->nb_regular_jcl; j++) {
        res->orbits[j] = compute_one_bpgorb(S, M->rels->regular_idems[j], level);
    }
    return res;
}



/*******************/
/* BPol(G⁺)-orbits */
/*******************/

bool** compute_polgpluspairs(orbits* L, dequeue* rideal, uint rcl) {
    morphism* M = L->original;
    green* G = L->original->rels;


    // The J-class that contains the R-class.
    uint jcl = G->JCL->numcl[lefread_dequeue(G->RCL->cl[rcl], 0)];

    // Computes the multiplication table between the R-class and the J-class.
    uint** table = compute_jmult(M, G->RCL->cl[rcl], G->JCL->cl[jcl]);


    dequeue** right_idems;
    MALLOC(right_idems, size_dequeue(rideal));
    for (uint i = 0; i < size_dequeue(rideal); i++) {
        right_idems[i] = create_dequeue();
    }

    dequeue** jc_inorbit;
    MALLOC(jc_inorbit, M->nb_min_regular_jcl);
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {
        uint f = M->min_regular_idems[i];
        dequeue* Mf = compute_l_ideal(M, f, NULL);
        for (uint j = 0; j < size_dequeue(rideal); j++) {
            if (intersec_dequeue(Mf, rideal)) {
                rigins_dequeue(i, right_idems[j]);
            }
        }
        delete_dequeue(Mf);
        jc_inorbit[i] = create_dequeue();
        for (uint j = 0; j < size_dequeue(G->JCL->cl[jcl]); j++) {
            if (L->orbits[i]->mono_in_sub[lefread_dequeue(G->JCL->cl[jcl], j)]) {
                rigins_dequeue(j, jc_inorbit[i]);
            }
        }
    }

    // Preparation of the array to return.
    bool** visited;
    MALLOC(visited, size_dequeue(rideal));
    for (uint i = 0; i < size_dequeue(rideal); i++) {
        CALLOC(visited[i], size_dequeue(G->RCL->cl[rcl]));
    }

    // Stacks to store the new pairs to treat.
    dequeue* topstack = create_dequeue();
    dequeue* botstack = create_dequeue();

    // We start from a trivial pair built from an element in the R-class.
    rigins_dequeue(lefread_dequeue(G->RCL->cl[rcl], 0), topstack);
    rigins_dequeue(lefread_dequeue(G->RCL->cl[rcl], 0), botstack);


    // DFS
    while (!isempty_dequeue(topstack)) {
        uint q = rigpull_dequeue(topstack);
        uint r = rigpull_dequeue(botstack);

        uint iq, ir;

        if (!mem_dequeue_sorted(q, rideal, &iq) || !mem_dequeue_sorted(r, G->RCL->cl[rcl], &ir) || visited[iq][ir]) {
            continue;
        }
        visited[iq][ir] = true;


        // For each letter a, we add the pair (qa,ra)
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            rigins_dequeue(M->r_cayley->edges[q][a], topstack);
            rigins_dequeue(M->r_cayley->edges[r][a], botstack);
        }

        // For each idempotent that stabilizes q to the right.
        for (uint k = 0; k < size_dequeue(right_idems[iq]); k++) {
            uint num = lefread_dequeue(right_idems[iq], k);
            for (uint j = 0; j < size_dequeue(jc_inorbit[num]); j++) {
                uint s = table[ir][lefread_dequeue(jc_inorbit[num], j)];

                // We are only interested in this pair if rs R e
                if (s != M->r_cayley->size_graph) {
                    rigins_dequeue(q, topstack);
                    rigins_dequeue(s, botstack);
                }
            }
        }
    }
    for (uint i = 0; i < size_dequeue(rideal); i++) {
        delete_dequeue(right_idems[i]);
    }
    free(right_idems);
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {
        delete_dequeue(jc_inorbit[i]);
    }
    free(jc_inorbit);
    delete_dequeue(topstack);
    delete_dequeue(botstack);
    for (uint i = 0; i < size_dequeue(G->RCL->cl[rcl]); i++) {
        free(table[i]);
    }
    free(table);
    return visited;
}

subsemi* compute_one_bpgplusorb(orbits* L, uint e, sub_level level) {
    morphism* M = L->original;
    green* G = L->original->rels;

    // The idempotent whose R-class we must compute.

    // The R-class of e.
    uint rcl = G->RCL->numcl[e];

    // We compute the set eM.
    dequeue* eM = compute_r_ideal(M, e, NULL);


    // We compute the Pol(G⁺)-pairs.
    bool** polgp = compute_polgpluspairs(L, eM, rcl);

    subsemi* theorb = compute_one_orbit_from_pairs(M, e, eM, polgp, level);

    for (uint q = 0; q < size_dequeue(eM); q++) {
        free(polgp[q]);
    }
    free(polgp);

    delete_dequeue(eM);

    return theorb;
}
#include "monoid_display.h"

orbits* compute_bpgplusorbits(orbits* S, sub_level level) {
    morphism* M = S->original;
    orbits* res;
    MALLOC(res, 1);
    res->original = M;
    MALLOC(res->orbits, M->rels->nb_regular_jcl);
    res->nb_computed = M->rels->nb_regular_jcl;
    if (level == LV_FULL) {
        res->level = LV_FULL;
    }
    else {
        res->level = LV_GREG;
    }
    for (uint j = 0; j < M->rels->nb_regular_jcl; j++) {
        res->orbits[j] = compute_one_bpgplusorb(S, M->rels->regular_idems[j], level);
    }
    return res;
}


