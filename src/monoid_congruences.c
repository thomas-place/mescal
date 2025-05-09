/*****************************/
/* Deterministic hierarchies */
/*****************************/

#include "monoid_congruences.h"
#include "monoid_display.h"

 /******************************/
 /* Computation of congruences */
 /******************************/

// Computes the least partition containing the input one and compatible with
// the graph transitions: if q ≣ r then qa ≣ ra.
static void cong_fold(dgraph* G, ufind* uf) {
    // A queue that will contain the elements that are yet to treat.
    dequeue* tofold = create_dequeue();

    // Doubly linked lists memorizing edges.
    // Only the lists corresponding to a root in the union-find are filled.
    dlist* ltrans[G->size_graph][G->size_alpha];
    for (uint q = 0; q < G->size_graph; q++) {
        if (q == find_ufind(q, uf)) {
            rigins_dequeue(q, tofold);
            for (uint a = 0; a < G->size_alpha; a++) {
                ltrans[q][a] = create_dlist();
            }
        }
        else {
            for (uint a = 0; a < G->size_alpha; a++) {
                ltrans[q][a] = NULL;
            }
        }
    }

    // The lists are filled with the original transitions.
    for (uint q = 0; q < G->size_graph; q++) {
        uint r = find_ufind(q, uf);

        for (uint a = 0; a < G->size_alpha; a++) {
            insertprevious_dlist(ltrans[r][a], ltrans[r][a]->rsent, G->edges[q][a]);
        }
    }

    // As long as there exists an element to treat
    while (!isempty_dequeue(tofold)) {

        // We take the representative of the class of the element to treat
        uint s = find_ufind(lefpull_dequeue(tofold), uf);

        bool folded = false;
        uint a = 0;
        while (!folded && a < G->size_alpha) {

            // If there are two outgoing edges labelled by a
            if (ltrans[s][a]->size > 1)
            {
                // We take the representatives of the destinations of the two
                // first edges (if it has not been done before)
                uint r = find_ufind(ltrans[s][a]->lsent->next->val, uf);
                uint t = find_ufind(ltrans[s][a]->lsent->next->next->val, uf);

                // We delete the first edge
                deletecell_dlist(ltrans[s][a], ltrans[s][a]->lsent->next);

                // If the two adjacent vertices have not been merged yet
                if (r != t)
                {
                    // We merge them
                    union_ufind(r, t, uf);

                    // Concatenation of the lists of adjacent vertices
                    for (uint b = 0; b < G->size_alpha; b++)
                    {
                        concat_dlist(ltrans[r][b], ltrans[t][b]);

                        // We only keep the list of the new representative
                        if (find_ufind(r, uf) == r)
                        {
                            free(ltrans[t][b]);
                            ltrans[t][b] = NULL;
                        }
                        else {
                            free(ltrans[r][b]);
                            ltrans[r][b] = NULL;
                        }
                    }

                    // We possibly have to treat the new class
                    rigins_dequeue(find_ufind(r, uf), tofold);
                    if (find_ufind(r, uf) != find_ufind(s, uf)) {
                        rigins_dequeue(find_ufind(s, uf), tofold);
                    }
                    folded = true;
                }
                else {
                    rigins_dequeue(find_ufind(s, uf), tofold);
                }
            }
            a++;
        }
    }

    // Release of the lists
    for (uint q = 0; q < G->size_graph; q++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            delete_dlist(ltrans[q][a]);
        }
    }
    delete_dequeue(tofold);
}

// Computes the least congruence containing the partition given as input.
// The partition is given as a union-find partition
void compute_leastcong(morphism* M, ufind* uf) {
    cong_fold(M->r_cayley, uf);
    cong_fold(M->l_cayley, uf);
}



ufind* iden_green_mono(morphism* M, green_relation grel) {
    ufind* theuf;
    switch (grel)
    {
    case H_GREEN:
        theuf = parti_to_ufind(M->rels->HCL);
        break;
    case L_GREEN:
        theuf = parti_to_ufind(M->rels->LCL);
        break;
    case R_GREEN:
        theuf = parti_to_ufind(M->rels->RCL);
        break;
    case J_GREEN:
        theuf = parti_to_ufind(M->rels->JCL);
        break;
    default:
        return NULL;
        break;
    }
    compute_leastcong(M, theuf);
    return theuf;
}

static void iden_green_subsemi_aux(subsemi* S, green_relation grel, ufind* theuf) {
    parti* theparti;
    switch (grel)
    {
    case H_GREEN:
        theparti = S->rels->HCL;
        break;
    case L_GREEN:
        theparti = S->rels->LCL;
        break;
    case R_GREEN:
        theparti = S->rels->RCL;
        break;
    case J_GREEN:
        theparti = S->rels->JCL;
        break;
    default:
        return;
        break;
    }
    for (uint i = 0; i < theparti->size_par; i++) {
        uint s = lefread_dequeue(theparti->cl[theparti->numcl[i]], 0);
        for (uint j = 1; j < size_dequeue(theparti->cl[theparti->numcl[i]]); j++) {
            uint t = lefread_dequeue(theparti->cl[theparti->numcl[i]], j);
            union_ufind(S->sub_to_mono[s], S->sub_to_mono[t], theuf);
        }
    }
}

ufind* iden_green_subsemi(subsemi* S, green_relation grel) {
    ufind* theuf = create_ufind(S->original->r_cayley->size_graph);
    iden_green_subsemi_aux(S, grel, theuf);
    compute_leastcong(S->original, theuf);
    return theuf;
}

ufind* iden_green_orbmono(orbits* L, green_relation grel) {
    ufind* theuf = create_ufind(L->original->r_cayley->size_graph);
    for (uint i = 0; i < L->nb_computed; i++) {
        iden_green_subsemi_aux(L->orbits[i], grel, theuf);
    }
    compute_leastcong(L->original, theuf);
    return theuf;
}

/***************************************/
/* Partitions generated by an equation */
/***************************************/

ufind* iden_bpolmod_mono(morphism* M) {
    green* G = M->rels;

    // If there exists a neutral letter, we return the partition into J-classes.
    if (mor_neutral_letter(M, NULL)) {
        return parti_to_ufind(G->JCL);
    }

    // Otherwise, we create a trivial union-find
    ufind* uf = create_ufind(M->r_cayley->size_graph);


    nfa* AR = morphism_to_dfa_rcl(M);
    nfa* AL = morphism_to_dfa_lcl(M);
    nfa* UR = nfa_proj_unary(AR);
    nfa* UL = nfa_proj_unary(AL);
    delete_nfa(AR);
    delete_nfa(AL);
    parti* SCCSR = nfa_inv_ext(UR);
    parti* SCCSL = nfa_inv_ext(UL);
    parti* FOLDR = nfa_stal_fold(UR, SCCSR);
    parti* FOLDL = nfa_stal_fold(UL, SCCSL);
    delete_parti(SCCSR);
    delete_parti(SCCSL);

    nfa* BR = nfa_merge_states(UR, FOLDR);
    nfa* BL = nfa_merge_states(UL, FOLDL);

    delete_nfa(UR);
    delete_nfa(UL);

    lgraph* temp = BL->trans;
    BL->trans = BL->trans_i;
    BL->trans_i = temp;

    uint size = 0;

    // Loop over all idempotents e = qr
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        uint e = lefread_dequeue(M->idem_list, i);

        // Loop over all idempotents f = st
        for (uint j = 0; j < size_dequeue(M->idem_list); j++) {
            uint f = lefread_dequeue(M->idem_list, j);
            uint ef = mor_mult(M, e, f);
            nfa_prod_pair* pairs = nfa_intersec_reach(BR, BL, FOLDR->numcl[e], FOLDL->numcl[f], &size);
            for (uint p = 0; p < size; p++) {
                uint c = pairs[p].q1;
                uint d = pairs[p].q2;

                for (uint g = 0; g < size_dequeue(FOLDR->cl[c]); g++) {
                    uint q = lefread_dequeue(FOLDR->cl[c], g);
                    for (uint k = 0; k < size_dequeue(FOLDL->cl[d]); k++) {
                        uint t = lefread_dequeue(FOLDL->cl[d], k);
                        union_ufind(ef, mor_mult(M, q, t), uf);
                    }
                }
            }
            free(pairs);
        }
    }
    delete_nfa(BR);
    delete_nfa(BL);
    delete_parti(FOLDR);
    delete_parti(FOLDL);

    compute_leastcong(M, uf);
    return uf;
}


ufind* iden_bpolamt_mono(morphism* M) {

    ufind* uf = create_ufind(M->r_cayley->size_graph);

    // Computation of the spanning trees for all (regular) R-classes and L-classes
    num_span_trees* rspans = compute_num_span_trees(M, true);
    num_span_trees* lspans = compute_num_span_trees(M, false);

    // Loop over all idempotents e = qr
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {

        uint e = lefread_dequeue(M->idem_list, i);

        // Loop over all idempotents f = st
        for (uint j = 0; j < size_dequeue(M->idem_list); j++) {
            uint f = lefread_dequeue(M->idem_list, j);
            uint ef = mor_mult(M, e, f);


            // We compute the anti AMT-pairs (q,t) where q is in the R-class of e and t is in the L-class of f
            dequeue* p1 = create_dequeue();
            dequeue* p2 = create_dequeue();


            compute_amt_pairs_regular(rspans, lspans, e, f, p1, p2);


            for (uint p = 0; p < size_dequeue(p1); p++) {
                uint q = lefread_dequeue(p1, p);
                uint t = lefread_dequeue(p2, p);
                union_ufind(ef, mor_mult(M, q, t), uf);
            }
            delete_dequeue(p1);
            delete_dequeue(p2);
        }
    }
    delete_num_span_trees(rspans);
    delete_num_span_trees(lspans);

    compute_leastcong(M, uf);
    return uf;
}

ufind* iden_blockg_mono(morphism* M) {
    green* G = M->rels;
    ufind* uf = create_ufind(M->r_cayley->size_graph);
    for (uint i = 0; i < G->RCL->size_par;i++) {
        if (!G->regular_array[lefread_dequeue(G->RCL->cl[i], 0)]) {
            continue;
        }
        uint e = lefread_dequeue(G->RCL->cl[i], 0);
        for (uint j = 1; j < size_dequeue(G->RCL->cl[i]); j++) {
            uint f = lefread_dequeue(G->RCL->cl[i], j);
            if (M->idem_array[f]) {
                if (M->idem_array[e]) {
                    union_ufind(e, f, uf);
                }
                else {
                    e = f;
                }
            }
        }
    }

    for (uint i = 0; i < G->LCL->size_par;i++) {
        uint e = lefread_dequeue(G->LCL->cl[i], 0);
        if (!G->regular_array[e]) {
            continue;
        }
        for (uint j = 1; j < size_dequeue(G->LCL->cl[i]); j++) {
            uint f = lefread_dequeue(G->LCL->cl[i], j);
            if (M->idem_array[f]) {
                if (M->idem_array[e]) {
                    union_ufind(e, f, uf);
                }
                else {
                    e = f;
                }
            }
        }
    }

    compute_leastcong(M, uf);
    return uf;
}

ufind* iden_knast_mono(orbits* L) {
    morphism* M = L->original;
    green* G = L->original->rels;


    // If the neutral element has a nonempty antecedent, we return the partition into J-classes.
    if (mor_nonempty_neutral(M)) {
        return parti_to_ufind(G->JCL);
    }

    // Otherwise, the union-find is created using Knast's equation.
    ufind* uf = create_ufind(M->r_cayley->size_graph);


    // Loop over all minimal idempotents e
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {


        // Union of all J-classes in the DD-orbit of e.
        for (uint k = 0; k < L->orbits[i]->rels->JCL->size_par; k++) {
            uint s = lefread_dequeue(L->orbits[i]->rels->JCL->cl[k], 0);
            for (uint j = 1; j < size_dequeue(L->orbits[i]->rels->JCL->cl[k]); j++) {
                uint t = lefread_dequeue(L->orbits[i]->rels->JCL->cl[k], j);
                union_ufind(L->orbits[i]->sub_to_mono[s], L->orbits[i]->sub_to_mono[t], uf);
            }
        }

        // Loop over the minimal idempotents f (case e= f is treated bith the orbit of e, cases f < e are treated by the symmetrical case e < f).
        for (uint j = i + 1; j < M->nb_min_regular_jcl; j++) {

            uint f = M->regular_idems[j];

            // Intersection of MfM and eMe (orbit of e)
            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            // Idempotents in the intersection of MfM and eMe
            dequeue* idems = make_inter_sorted_dequeue(MfM, M->idem_list);
            delete_dequeue(MfM);

            // Ideals
            dequeue* Mf = compute_l_ideal(M, f, NULL);
            dequeue* fM = compute_r_ideal(M, f, NULL);



            // Loop over all idempotents g = eqfre
            for (uint x = 0; x < size_dequeue(idems); x++) {
                uint g = lefread_dequeue(idems, x);

                // The elements eqf such that eqf R g
                dequeue* lset = make_inter_sorted_dequeue(Mf, G->RCL->cl[G->RCL->numcl[g]]);
                for (uint u = 0; u < size_dequeue(lset); u++) {
                    uint q = lefread_dequeue(lset, u);

                    // Loop over all idempotents h = esfte
                    for (uint y = 0; y < size_dequeue(idems); y++) {
                        uint h = lefread_dequeue(idems, y);
                        uint gh = mor_mult(M, g, h);

                        // The elements fte such that fte L h
                        dequeue* rset = make_inter_sorted_dequeue(fM, G->LCL->cl[G->LCL->numcl[h]]);
                        for (uint v = 0; v < size_dequeue(rset); v++) {
                            uint t = lefread_dequeue(rset, v);
                            union_ufind(gh, mor_mult(M, q, t), uf);
                        }
                        delete_dequeue(rset);
                    }
                }
                delete_dequeue(lset);
            }
            delete_dequeue(idems);
            delete_dequeue(fM);
            delete_dequeue(Mf);
        }
    }

    compute_leastcong(M, uf);
    return uf;
}

ufind* iden_qknast_mono(orbits* L, subsemi* mker) {
    morphism* M = L->original;
    green* G = L->original->rels;


    // Special case when there exists a neutral letter. We simply return the partition into J-classes.
    if (mor_neutral_letter(M, NULL)) {
        return parti_to_ufind(G->JCL);
    }

    ufind* uf = create_ufind(M->r_cayley->size_graph);

    // Loop over all minimal idempotents e
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {


        // Union of all J-classes in the MOD⁺-orbit of e.
        for (uint k = 0; k < L->orbits[i]->rels->JCL->size_par; k++) {
            uint s = lefread_dequeue(L->orbits[i]->rels->JCL->cl[k], 0);
            for (uint j = 1; j < size_dequeue(L->orbits[i]->rels->JCL->cl[k]); j++) {
                uint t = lefread_dequeue(L->orbits[i]->rels->JCL->cl[k], j);
                union_ufind(L->orbits[i]->sub_to_mono[s], L->orbits[i]->sub_to_mono[t], uf);
            }
        }

        // Loop over the minimal idempotents f (case e= f is treated bith the orbit of e, cases f < e are treated by the symmetrical case e < f).
        for (uint j = i + 1; j < M->nb_min_regular_jcl; j++) {
            uint f = M->regular_idems[j];

            // Intersection of MfM and the MOD⁺-orbit of e
            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            // Restriction to the idempotents
            dequeue* idems = make_inter_sorted_dequeue(MfM, M->idem_list);
            delete_dequeue(MfM);


            // Ideals Mf et fM restricted to the MOD-kernel
            dequeue* Mf = compute_l_ideal(M, f, mker->mono_in_sub);
            dequeue* fM = compute_r_ideal(M, f, mker->mono_in_sub);



            // Loop over the idempotents g = eqfre
            for (uint x = 0; x < size_dequeue(idems); x++) {
                uint g = lefread_dequeue(idems, x);

                // The eqf in the MOD-kernel such that eqf R g
                dequeue* lset = make_inter_sorted_dequeue(Mf, G->RCL->cl[G->RCL->numcl[g]]);
                for (uint u = 0; u < size_dequeue(lset); u++) {
                    uint q = lefread_dequeue(lset, u);

                    // Loop over the idempotents h = esfte
                    for (uint y = 0; y < size_dequeue(idems); y++) {
                        uint h = lefread_dequeue(idems, y);
                        uint gh = mor_mult(M, g, h);

                        // The fte in the MOD-kernel such fte L h
                        dequeue* rset = make_inter_sorted_dequeue(fM, G->LCL->cl[G->LCL->numcl[h]]);
                        for (uint v = 0; v < size_dequeue(rset); v++) {
                            uint t = lefread_dequeue(rset, v);
                            union_ufind(gh, mor_mult(M, q, t), uf);
                        }
                        delete_dequeue(rset);
                    }
                }
                delete_dequeue(lset);
            }
            delete_dequeue(idems);
            delete_dequeue(fM);
            delete_dequeue(Mf);
        }
    }

    compute_leastcong(M, uf);
    return uf;
}






ufind* iden_bpolamtp_mono(orbits* L) {
    morphism* M = L->original;

    ufind* uf = create_ufind(M->r_cayley->size_graph);


    // Computation of the spanning trees for all (regular) R-classes and L-classes
    num_span_trees* rspans = compute_num_span_trees(M, true);
    num_span_trees* lspans = compute_num_span_trees(M, false);


    // Loop over all minimal idempotents e.
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {

        // Loop over all minimal idempotents f (the case f < e is treated when e and f are inverted).
        for (uint j = i; j < M->nb_min_regular_jcl; j++) {
            uint f = M->regular_idems[j];

            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            dequeue* candidates = make_inter_sorted_dequeue(M->idem_list, MfM);
            delete_dequeue(MfM);

            for (uint k = 0; k < size_dequeue(candidates); k++) {
                uint g = lefread_dequeue(candidates, k);
                for (uint l = 0; l < size_dequeue(candidates); l++) {
                    uint h = lefread_dequeue(candidates, l);
                    uint gh = mor_mult(M, g, h);

                    // We compute the anti AMT-pairs (q,t) where q is in the R-class of g and t is in the L-class of h
                    dequeue* p1 = create_dequeue();
                    dequeue* p2 = create_dequeue();
                    compute_amt_pairs_regular(rspans, lspans, g, h, p1, p2);

                    for (uint p = 0; p < size_dequeue(p1); p++) {
                        uint q = lefread_dequeue(p1, p);
                        uint t = lefread_dequeue(p2, p);
                        if (mor_mult(M, q, f) != q || mor_mult(M, f, t) != t) {
                            continue;
                        }
                        union_ufind(gh, mor_mult(M, q, t), uf);
                    }
                    delete_dequeue(p1);
                    delete_dequeue(p2);
                }
            }
            delete_dequeue(candidates);
        }
    }
    delete_num_span_trees(rspans);
    delete_num_span_trees(lspans);
    compute_leastcong(M, uf);
    return uf;
}

ufind* iden_bpolgrp_mono(orbits* L) {

    morphism* M = L->original;
    ufind* uf = create_ufind(M->r_cayley->size_graph);

    nfa* AR = morphism_to_dfa_rcl(M);
    nfa* AL = morphism_to_dfa_lcl(M);
    parti* SCCSR = nfa_inv_ext(AR);
    parti* SCCSL = nfa_inv_ext(AL);
    parti* FOLDR = nfa_stal_fold(AR, SCCSR);
    parti* FOLDL = nfa_stal_fold(AL, SCCSL);
    delete_parti(SCCSR);
    delete_parti(SCCSL);

    nfa* BR = nfa_merge_states(AR, FOLDR);
    nfa* BL = nfa_merge_states(AL, FOLDL);

    delete_nfa(AR);
    delete_nfa(AL);

    lgraph* temp = BL->trans;
    BL->trans = BL->trans_i;
    BL->trans_i = temp;


    uint size = 0;

    // Loop over all idempotents e.
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {

        // Loop over all idempotents f (the case f < e is treated when e and f are inverted).
        for (uint j = i; j < M->nb_min_regular_jcl; j++) {
            uint f = M->regular_idems[j];

            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            dequeue* candidates = make_inter_sorted_dequeue(M->idem_list, MfM);
            delete_dequeue(MfM);

            for (uint k = 0; k < size_dequeue(candidates); k++) {
                uint g = lefread_dequeue(candidates, k);
                for (uint l = 0; l < size_dequeue(candidates); l++) {
                    uint h = lefread_dequeue(candidates, l);
                    uint gh = mor_mult(M, g, h);
                    nfa_prod_pair* pairs = nfa_intersec_reach(BR, BL, FOLDR->numcl[g], FOLDL->numcl[h], &size);
                    for (uint p = 0; p < size; p++) {
                        uint c = pairs[p].q1;
                        uint d = pairs[p].q2;

                        for (uint x = 0; x < size_dequeue(FOLDR->cl[c]); x++) {
                            uint q = lefread_dequeue(FOLDR->cl[c], x);
                            if (mor_mult(M, q, f) != q) {
                                continue;
                            }

                            for (uint y = 0; y < size_dequeue(FOLDL->cl[d]); y++) {
                                uint t = lefread_dequeue(FOLDL->cl[d], y);
                                if (mor_mult(M, f, t) != t) {
                                    continue;
                                }
                                union_ufind(gh, mor_mult(M, q, t), uf);
                            }
                        }
                    }
                    free(pairs);
                }
            }
            delete_dequeue(candidates);
        }
    }
    delete_nfa(BR);
    delete_nfa(BL);
    delete_parti(FOLDR);
    delete_parti(FOLDL);

    compute_leastcong(M, uf);
    return uf;
}



ufind* iden_mpolc_mono(morphism* M, parti* C) {


    // Creation of the Union-Find
    ufind* theuf = create_ufind(M->r_cayley->size_graph);

    // For each class in the equivalence C
    for (uint c = 0; c < C->size_par; c++) {
        if (size_dequeue(C->cl[c]) < 2) {
            continue;
        }

        // We take the first element s in the class.
        for (uint i = 0; i < size_dequeue(C->cl[c]); i++) {

            uint s = lefread_dequeue(C->cl[c], i);

            // Idempotents in sM and Ms
            dequeue* sM = compute_r_ideal(M, s, M->idem_array);
            dequeue* Ms = compute_l_ideal(M, s, M->idem_array);

            // We take a second distinct element t in the class
            for (uint j = 0; j < size_dequeue(C->cl[c]); j++) {
                if (i == j) {
                    continue;
                }
                uint t = lefread_dequeue(C->cl[c], j);
                for (uint k = 0; k < size_dequeue(sM); k++) {
                    uint sq = lefread_dequeue(sM, k);
                    uint sqs = mor_mult(M, sq, s);
                    uint sqt = mor_mult(M, sq, t);

                    for (uint l = 0; l < size_dequeue(Ms); l++) {
                        uint rs = lefread_dequeue(Ms, l);

                        uint sqsrs = mor_mult(M, sqs, rs);
                        uint sqtrs = mor_mult(M, sqt, rs);
                        union_ufind(sqsrs, sqtrs, theuf);
                    }
                }
            }
            delete_dequeue(sM);
            delete_dequeue(Ms);
        }
    }

    compute_leastcong(M, theuf);
    return theuf;
}


ufind* iden_lpolc_mono(morphism* M, parti* C) {
    // Creation of the Union-Find
    ufind* theuf = create_ufind(M->r_cayley->size_graph);

    // For every idempotent e
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        uint e = lefread_dequeue(M->idem_list, i);
        dequeue* rideal = compute_r_ideal(M, e, NULL);
        dequeue* req = make_inter_sorted_dequeue(C->cl[C->numcl[e]], rideal);
        delete_dequeue(rideal);

        // We merge e with all element et such that et C-eq e
        for (uint j = 0; j < size_dequeue(req); j++) {
            uint et = lefread_dequeue(req, j);
            union_ufind(e, et, theuf);
        }
        delete_dequeue(req);
    }


    compute_leastcong(M, theuf);
    return theuf;
}

ufind* iden_rpolc_mono(morphism* M, parti* C) {


    // Creation of the Union-Find
    ufind* theuf = create_ufind(M->r_cayley->size_graph);

    // For every idempotent e
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        uint e = lefread_dequeue(M->idem_list, i);
        dequeue* lideal = compute_l_ideal(M, e, NULL);
        dequeue* leq = make_inter_sorted_dequeue(C->cl[C->numcl[e]], lideal);
        delete_dequeue(lideal);

        // We merge e with all element te such that te C-eq e
        for (uint j = 0; j < size_dequeue(leq); j++) {
            uint te = lefread_dequeue(leq, j);
            union_ufind(e, te, theuf);
        }
        delete_dequeue(leq);
    }

    compute_leastcong(M, theuf);
    return theuf;
}



// Calcul du niveau dans les hiéarchies LPol/RPol de base C: 1 = RPol(C),LPol(C)
// 2 = RPol₂(C),LPol₂(C)...
void hdet_lrpol_level(morphism* M, ufind* C, FILE* out, short* minf, short* minp) {
    if (C->size_par == C->size_set) {
        if (out) {
            fprintf(out, "#### The language has level 1 in both the future hierarchy and the past hierarchy.\n");
        }
        *minf = 1;
        *minp = 1;
    }
    ufind* ufleft;
    ufind* ufright;
    compute_leastcong(M, C);
    parti* pleft = ufind_to_parti(C);
    parti* pright = ufind_to_parti(C);
    short level = 1;
    while (true) {
        ufleft = iden_lpolc_mono(M, pright);
        ufright = iden_rpolc_mono(M, pleft);
        delete_parti(pright);
        delete_parti(pleft);
        if (ufleft->size_par == ufleft->size_set && ufright->size_par == ufright->size_set) {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out) {
                fprintf(out, "#### The language has level %d in both the future hierarchy and the past hierarchy.\n", level);
            }
            *minf = level;
            *minp = level;
            return;
        }
        else if (ufleft->size_par == ufleft->size_set) {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out && level % 2) {
                fprintf(out, "#### The language has level %d in the past hierarchy but not in the future hierarchy.\n", level);
                fprintf(out, "#### The language has level %d in the future hierarchy.\n", level + 1);
                *minf = level + 1;
                *minp = level;
            }
            else if (out) {
                fprintf(out, "#### The language has level %d in the future hierarchy but not in the past hierarchy.\n", level);
                fprintf(out, "#### The language has level %d in the past hierarchy.\n", level + 1);
                *minf = level;
                *minp = level + 1;
            }
            return;
        }
        else if (ufright->size_par == ufright->size_set) {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out && level % 2) {
                fprintf(out, "#### The language has level %d in the future hierarchy but not in the past hierarchy.\n", level);
                fprintf(out, "#### The language has level %d in the past hierarchy.\n", level + 1);
                *minf = level;
                *minp = level + 1;
            }
            else if (out) {
                fprintf(out, "#### The language has level %d in the past hierarchy but not in the future hierarchy.\n", level);
                fprintf(out, "#### The language has level %d in the future hierarchy.\n", level + 1);
                *minf = level + 1;
                *minp = level;
            }

            return;
        }

        if (out) {
            fprintf(out, "#### The language does not have level %d in either hierarchy.\n", level);
        }
        level++;
        compute_leastcong(M, ufleft);
        compute_leastcong(M, ufright);
        pleft = ufind_to_parti(ufleft);
        pright = ufind_to_parti(ufright);
        delete_ufind(ufleft);
        delete_ufind(ufright);
    }
}
