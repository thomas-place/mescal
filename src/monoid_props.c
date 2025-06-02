#include "monoid_props.h"
#include "monoid_display.h"



char green_rel_array[4] = { 'H', 'L', 'R', 'J' };


/***********/
/* Trivial */
/***********/


bool is_trivial_monoid(morphism* M, uint* c) {
    if (M->r_cayley->size_graph == 1) {
        return true;
    }
    if (c) {
        c[0] = ONE;
        c[1] = 1;
    }
    return false;
}

bool is_trivial_semigroup(morphism* M, uint* c) {
    green* G = M->rels;
    if (M->r_cayley->size_graph == 1 || (M->r_cayley->size_graph == 2 && G->JCL->size_par == 2)) {
        return true;
    }

    if (c) {
        if (M->r_cayley->size_graph == 2) {
            c[0] = ONE;
            c[1] = 1;
        }
        else {
            c[0] = 1;
            c[1] = 2;
        }
    }
    return false;
}


bool is_trivial_subsemi(subsemi* S, uint* c) {
    if (S->size == 1) {
        return true;
    }
    if (c) {
        c[0] = S->sub_to_mono[0];
        c[1] = S->sub_to_mono[1];
    }
    return false;
}

bool is_trivial_orbmono(orbits* L, uint* c) {
    for (uint i = 0; i < L->nb_computed; i++) {
        if (!is_trivial_subsemi(L->orbits[i], c)) {
            if (c) {
                c[2] = L->orbits[i]->sub_to_mono[L->orbits[i]->neut];
            }
            return false;
        }
    }
    return true;
}



/**********/
/* Groups */
/**********/


bool is_group_mono(morphism* M, uint* c) {
    green* G = M->rels;
    // If there is only one J-class, the monoid is a group.
    if (G->JCL->size_par == 1) {
        return true;
    }
    // Otherwise, an element that has no inverse.
    if (c) {
        c[0] = G->JCL->cl_elems[1][0];
    }
    return false;
}


bool is_group_semigroup(morphism* M, uint* c) {
    green* G = M->rels;
    // If there is only one J-class in the semigroup, then it is a group.
    if (G->JCL->size_par == 1 || (G->JCL->size_par == 2 && G->JCL->cl_size[G->JCL->numcl[ONE]] == 1)) {
        return true;
    }

    // Otherwise, two elements that are not J-equivalent are a counterexample.
    if (c) {
        if (G->JCL->cl_size[G->JCL->numcl[ONE]] != 1) {
            c[0] = ONE;
            c[1] = G->JCL->cl_elems[1][0];// lefread_dequeue(G->JCL->cl[1], 0);
        }
        else {
            c[0] = G->JCL->cl_elems[1][0];//lefread_dequeue(G->JCL->cl[1], 0);
            c[1] = G->JCL->cl_elems[2][0];//lefread_dequeue(G->JCL->cl[2], 0);
        }

    }
    return false;
}

bool is_group_subsemi(subsemi* S, uint* c) {
    // If there is only one J-class in the subsemigroup, then it is a group.
    if (S->rels->JCL->size_par == 1) {
        return true;
    }

    // Otherwise, an element that has no inverse is a counterexample.
    if (c) {
        if (S->rels->JCL->numcl[S->neut] == 0) {
            c[0] = S->sub_to_mono[S->rels->JCL->cl_elems[1][0]];
        }
        else {
            c[0] = S->sub_to_mono[S->rels->JCL->cl_elems[0][0]];
        }
    }
    return false;
}

bool is_group_orbmono(orbits* L, uint* c) {
    for (uint i = 0; i < L->nb_computed; i++) {
        if (!is_group_subsemi(L->orbits[i], c)) {
            if (c) {
                c[1] = L->orbits[i]->sub_to_mono[L->orbits[i]->neut];
            }
            return false;
        }
    }
    return true;
}


bool is_letterind_mono(morphism* M, uint* c) {
    for (uint a = 1; a < M->r_cayley->size_alpha; a++) {
        if (M->r_cayley->edges[0][a] != M->r_cayley->edges[0][0]) {
            if (c) {
                c[0] = a;
            }
            return false;
        }
    }
    return true;
}




/*****************/
/* Commutativité */
/*****************/


bool is_comm_mono(morphism* M, uint* c) {

    for (uint a = 0; a < M->r_cayley->size_alpha - 1; a++) {
        for (uint b = a + 1; b < M->r_cayley->size_alpha; b++) {
            uint av = M->r_cayley->edges[ONE][a];
            uint bv = M->r_cayley->edges[ONE][b];

            uint ab = M->r_cayley->edges[av][b];
            uint ba = M->r_cayley->edges[bv][a];
            if (ab != ba) {
                if (c) {
                    c[0] = av;
                    c[1] = bv;
                }
                return false;
            }
        }
    }
    return true;
}

bool is_comm_subsemi(subsemi* S, uint* c) {
    // The original morphism
    morphism* M = S->original;

    for (uint s = 0; s < S->size - 1; s++) {
        for (uint t = s + 1; t < S->size; t++) {
            uint st = mor_mult(M, S->sub_to_mono[s], S->sub_to_mono[t]);
            uint ts = mor_mult(M, S->sub_to_mono[t], S->sub_to_mono[s]);
            if (st != ts) {
                if (c) {
                    c[0] = S->sub_to_mono[s];
                    c[1] = S->sub_to_mono[t];
                }
                return false;
            }
        }
    }
    return true;
}

bool is_com_orbmono(orbits* L, uint* c) {
    for (uint i = 0; i < L->nb_computed; i++) {
        if (!is_comm_subsemi(L->orbits[i], c)) {
            if (c) {
                c[2] = L->orbits[i]->sub_to_mono[L->orbits[i]->neut];
            }
            return false;
        }
    }
    return true;
}

bool is_comm_ltt_mono(orbits* L, uint* c) {

    // The original morphism
    morphism* M = L->original;

    // We first check whether the DD-orbits are commutative (the case e = f)
    if (!is_com_orbmono(L, c)) {
        if (c) {
            uint e = c[2];
            uint q = c[0];
            uint s = c[1];
            c[0] = q;
            c[1] = ONE;
            c[2] = s;
            c[3] = e;
            c[4] = e;
        }

        return false;
    }

    // We now check the equation for the cases when (e ≠ f).
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {
        uint e = M->regular_idems[i];
        dequeue* eM = compute_r_ideal(M, e, NULL);
        dequeue* Me = compute_l_ideal(M, e, NULL);

        // For every idempotent f in the list (the case e = f is treated with the DD-orbits).
        for (uint j = 0; j < M->nb_min_regular_jcl; j++) {
            if (i == j) {
                continue;
            }

            uint f = M->regular_idems[j];

            dequeue* fM = compute_r_ideal(M, f, NULL);
            dequeue* Mf = compute_l_ideal(M, f, NULL);
            dequeue* eMf = make_inter_sorted_dequeue(eM, Mf);
            dequeue* fMe = make_inter_sorted_dequeue(fM, Me);
            delete_dequeue(fM);
            delete_dequeue(Mf);
            for (uint k = 0; k < size_dequeue(eMf) - 1; k++) {
                for (uint l = k + 1; l < size_dequeue(eMf); l++) {
                    for (uint m = 0; m < size_dequeue(fMe); m++) {
                        uint r = lefread_dequeue(eMf, k);
                        uint s = lefread_dequeue(fMe, m);
                        uint t = lefread_dequeue(eMf, l);
                        if (mor_mult_gen(M, 3, r, s, t) != mor_mult_gen(M, 3, t, s, r)) {
                            if (c) {
                                c[0] = r;
                                c[1] = s;
                                c[2] = t;
                                c[3] = e;
                                c[4] = f;
                            }
                            delete_dequeue(eMf);
                            delete_dequeue(fMe);
                            delete_dequeue(eM);
                            delete_dequeue(Me);
                            return false;
                        }
                    }
                }
            }
            delete_dequeue(eMf);
            delete_dequeue(fMe);
        }
        delete_dequeue(eM);
        delete_dequeue(Me);
    }
    return true;
}

/***************/
/* Idempotence */
/***************/

// Idempotence
bool is_idem_mono(morphism* M, uint* c) {
    if (M->nb_idems == M->r_cayley->size_graph) {
        return true;
    }

    if (c) {
        for (uint q = 0; q < M->r_cayley->size_graph; q++) {
            if (M->nb_idems <= q || M->idem_list[q] != q) {
                c[0] = q;
                break;
            }
        }
    }
    return false;
}


bool is_idem_subsemi(subsemi* S, uint* c) {
    if (S->nb_idems == S->size) {
        return true;
    }

    if (c) {
        for (uint q = 0; q < S->size; q++) {
            if (S->nb_idems <= q || S->idem_list[q] != q) {
                c[0] = S->sub_to_mono[q];
                break;
            }
        }
    }
    return false;
}

bool is_idem_orbmono(orbits* L, uint* c) {
    for (uint i = 0; i < L->nb_computed; i++) {
        if (!is_idem_subsemi(L->orbits[i], c)) {
            if (c) {
                c[1] = L->orbits[i]->sub_to_mono[L->orbits[i]->neut];
            }
            return false;
        }
    }
    return true;
}



/*******************/
/* H,R,L,J-trivial */
/*******************/



static parti* grel_to_parti(green* G, green_relation P) {
    switch (P) {
    case H_GREEN:
        return G->HCL;
        break;
    case L_GREEN:
        return G->LCL;
        break;
    case R_GREEN:
        return G->RCL;
        break;
    case J_GREEN:
        return G->JCL;
        break;
    default:
        return NULL;
        break;
    }
}

bool is_gtrivial_mono(morphism* M, green_relation P, uint* c) {
    parti* thepar = grel_to_parti(M->rels, P);

    // If there are as many elements as P-classes, the monoid is P-trivial
    if (thepar->size_set == thepar->size_par) {
        return true;
    }

    if (c) {
        // We look for a counterexample
        // We consider all idempotents representing a regular J-class
        for (uint i = 0; i < M->nb_regular_jcl; i++) {
            uint e = M->regular_idems[i];
            // We take the first one that is non-trivial
            if (thepar->cl_size[thepar->numcl[e]] > 1) {
                uint s = thepar->cl_elems[thepar->numcl[e]][0];
                if (s == e) {
                    s = thepar->cl_elems[thepar->numcl[e]][1];
                }
                c[0] = e;
                c[1] = s;
                break;
            }
        }
    }



    return false;
}

bool is_gtrivial_subsemi(subsemi* S, green_relation P, uint* c) {

    parti* thepar = grel_to_parti(S->rels, P);

    // If there are as many elements as P-classes, the monoid is P-trivial
    if (thepar->size_set == thepar->size_par) {
        return true;
    }

    if (c) {
        // We look at all idempotents
        for (uint i = 0; i < S->nb_idems; i++) {
            uint e = S->idem_list[i];
            // We take the first one whose P-class is not trivial
            if (thepar->cl_size[thepar->numcl[e]] > 1) {
                uint s = thepar->cl_elems[thepar->numcl[e]][0];
                if (s == e) {
                    s = thepar->cl_elems[thepar->numcl[e]][1];
                }
                c[0] = S->sub_to_mono[e];
                c[1] = S->sub_to_mono[s];
                break;
            }
        }
    }
    return false;
}

bool is_gtrivial_orbmono(orbits* L, green_relation P, uint* c) {
    for (uint i = 0; i < L->nb_computed; i++) {
        // We check if the orbit is P-trivial
        if (!is_gtrivial_subsemi(L->orbits[i], P, c)) {
            if (c) {
                c[2] = L->orbits[i]->sub_to_mono[L->orbits[i]->neut];
            }
            return false;
        }
    }
    return true;
}

bool is_htrivial_generators(morphism* M, uint* c) {
    parti* H = M->rels->HCL;
    if (H->cl_size[H->numcl[ONE]] > 1) {
        if (c) {
            c[0] = ONE;
            c[1] = H->cl_elems[H->numcl[ONE]][0];
            if (c[1] == ONE) {
                c[1] = H->cl_elems[H->numcl[ONE]][1];
            }
        }
        return false;
    }


    for (uint i = 0; i < M->r_cayley->size_alpha; i++) {
        uint a = M->r_cayley->edges[ONE][i];
        if (H->cl_size[H->numcl[a]] > 1) {
            if (c) {
                c[0] = a;
                c[1] = H->cl_elems[H->numcl[a]][0];
                if (c[1] == a) {
                    c[1] = H->cl_elems[H->numcl[a]][1];
                }
            }
            return false;
        }
    }
    return true;
}

/******/
/* DA */
/******/

bool is_da_mono(morphism* M, uint* c) {
    green* G = M->rels;

    uint nb_idem = M->nb_idems;

    // The monoid is in DA if and only if there are as many regular elements as idempotents.
    if (nb_idem == G->nb_regular_elems) {
        return true;
    }

    if (c) {
        // We look at all the elements of the monoid
        for (uint s = 0; s < M->r_cayley->size_graph; s++) {

            // We take the first one that is regular and non-idempotent
            if (!M->idem_array[s] && G->regular_array[s]) {
                c[0] = s;
                break;
            }
        }

    }


    return false;
}


bool is_da_subsemi(subsemi* S, uint* c) {
    // The original morphism.
    uint nb_idem = S->nb_idems;

    // The subsemigroup is in DA if and only if there are as many regular elements as idempotents.
    if (nb_idem == S->rels->nb_regular_elems) {
        return true;
    }

    if (c) {
        for (uint s = 0; s < S->size; s++) {
            // We take the first one that is regular and non-idempotent
            if (!S->idem_array[s] && S->rels->regular_array[s]) {
                c[0] = S->sub_to_mono[s];
                break;
            }
        }
    }
    return false;
}

bool is_da_orbmono(orbits* L, uint* c) {
    for (uint i = 0; i < L->nb_computed; i++) {
        // We check if the orbit is in DA
        if (!is_da_subsemi(L->orbits[i], c)) {
            if (c) {
                c[1] = L->orbits[i]->sub_to_mono[L->orbits[i]->neut];
            }
            return false;
        }
    }
    return true;
}

/****************/
/* J-saturated  */
/****************/

bool is_jsat_mono(morphism* M, uint* c) {
    if (!M->order) {
        fprintf(stderr, "Error: The order of the morphism is not computed.\n");
        exit(EXIT_FAILURE);
    }
    //mor_compute_order(M);
    if (M->r_cayley->size_graph == M->order_size[ONE]) {
        return true;
    }
    if (c) {
        uint i = 0;
        while (i < M->order_size[ONE] && M->order[ONE][i] == i) {
            i++;
        }
        c[0] = i;
    }
    return false;
}

bool is_ejsat_mono(morphism* M, uint* c) {
    if (!M->order) {
        fprintf(stderr, "Error: The order of the morphism is not computed.\n");
        exit(EXIT_FAILURE);
    }
    //mor_compute_order(M);
    uint i = 0;
    uint j = 0;
    while (i < M->nb_idems) {
        if (j >= M->order_size[ONE] || M->order[ONE][j] > M->idem_list[i]) {
            if (c) {
                c[0] = M->idem_list[i];
            }
            return false;
        }
        else if (M->order[ONE][j] < M->idem_list[i]) {
            j++;
        }
        else {
            i++;
            j++;
        }
    }
    return true;
}

bool is_jsat_subsemi(subsemi* S, uint ind, uint* c) {
    if (!S->original->order) {
        fprintf(stderr, "Error: The order of the morphism is not computed.\n");
        exit(EXIT_FAILURE);
    }
    //morphism* M = S->original;
    //mor_compute_order(M);

    uint i = 0;
    uint j = 0;
    while (i < S->size) {
        if (j >= S->original->order_size[ind] || S->original->order[ind][j] > S->sub_to_mono[i]) {
            if (c) {
                c[0] = S->sub_to_mono[i];
            }
            return false;
        }
        else if (S->original->order[ind][j] < S->sub_to_mono[i]) {
            j++;
        }
        else {
            i++;
            j++;
        }
    }
    return true;
}


bool is_jsat_orbmono(orbits* L, uint* c) {
    if (!L->original->order) {
        fprintf(stderr, "Error: The order of the morphism is not computed.\n");
        exit(EXIT_FAILURE);
    }

    for (uint i = 0; i < L->nb_computed; i++) {

        if (!is_jsat_subsemi(L->orbits[i], i, c)) {
            if (c) {
                c[1] = L->orbits[i]->sub_to_mono[L->orbits[i]->neut];
            }
            return false;
        }
    }
    return true;
}

/*********/
/* Knast */
/*********/

// Block-group
bool is_blockg_mono(morphism* M, uint* c) {
    green* G = M->rels;
    uint ridem[G->RCL->size_par];
    for (uint i = 0; i < G->RCL->size_par; i++) {
        ridem[i] = ONE;
    }
    uint lidem[G->LCL->size_par];
    for (uint i = 0; i < G->LCL->size_par; i++) {
        lidem[i] = ONE;
    }

    // Loop over all idempotents
    for (uint i = 1; i < M->nb_idems; i++) {
        // For each idempotent e, we check if there is a unique idempotent f
        // such that e R f and a unique idempotent g such that g L e
        uint e = M->idem_list[i];

        // The R-class of e
        uint cr = G->RCL->numcl[e];

        if (ridem[cr] != ONE) {
            if (c) {
                c[0] = ridem[cr];
                c[1] = e;
                c[2] = (uint)R_GREEN;

            }
            return false;
        }
        else {
            // Else, we record the fact that e is an idempotent of the R-class
            ridem[cr] = e;
        }

        // The L-class of e
        uint cl = G->LCL->numcl[e];
        if (lidem[cl] != ONE) {
            if (c) {
                c[0] = lidem[cl];
                c[1] = e;
                c[2] = (uint)L_GREEN;
            }
            return false;
        }
        else {
            // Else, we record the fact that e is an idempotent of the L-class
            lidem[cl] = e;
        }
    }
    return true;
}


static void make_cexample_from_jtriv(morphism* M, uint* cexa) {
    if (!cexa) {
        return;
    }
    uint e = cexa[0];
    uint s = cexa[1];



    if (M->rels->RCL->numcl[e] == M->rels->RCL->numcl[s]) {
        cexa[0] = s;
        cexa[1] = get_rlink(M, M->rels->RCL, e, s);
        cexa[2] = ONE;
        cexa[3] = ONE;
        return;
    }
    else {
        cexa[0] = ONE;
        cexa[1] = ONE;
        cexa[2] = get_llink(M, M->rels->LCL, e, s);
        cexa[3] = s;

        return;

    }
}








bool is_bpolmod_mono(subsemi* S, uint* cexa) {

    // We first check whether the MOD-kernel is J-trivial.
    if (!is_gtrivial_subsemi(S, J_GREEN, cexa)) {
        if (cexa) {
            make_cexample_from_jtriv(S->original, cexa);
        }
        return false;
    }



    morphism* M = S->original;

    parti* FOLDR = mor_stal_fold(M, false, true);
    parti* FOLDL = mor_stal_fold(M, false, false);

    dgraph* gr = shrink_mod(M->r_cayley, FOLDR, M->rels->RCL);
    dgraph* glinv = shrink_mod_mirror(M->l_cayley, FOLDL, M->rels->LCL);




    uint size = 0;

    // Loop over all idempotents e = qr
    for (uint i = 0; i < M->nb_idems; i++) {
        uint e = M->idem_list[i];

        // Loop over all idempotents f = st
        for (uint j = 0; j < M->nb_idems; j++) {
            uint f = M->idem_list[j];
            uint ef = mor_mult(M, e, f);


            prod_pair* pairs = dgraph_intersec(gr, glinv, FOLDR->numcl[e], FOLDL->numcl[f], &size);

            for (uint p = 0; p < size; p++) {
                uint c = pairs[p].q1;
                uint d = pairs[p].q2;

                for (uint g = 0; g < FOLDR->cl_size[c]; g++) {
                    uint q = FOLDR->cl_elems[c][g];
                    // Il suffit de tester les q réguliers

                    for (uint k = 0; k < FOLDL->cl_size[d]; k++) {
                        uint t = FOLDL->cl_elems[d][k];

                        if (ef != mor_mult(M, q, t)) {
                            if (cexa) {
                                cexa[0] = q;
                                cexa[1] = get_rlink(M, M->rels->RCL, e, q);
                                cexa[2] = get_llink(M, M->rels->LCL, f, t);
                                cexa[3] = t;
                            }
                            free(pairs);
                            delete_dgraph(gr);
                            delete_dgraph(glinv);
                            delete_parti(FOLDR);
                            delete_parti(FOLDL);
                            return false;
                        }
                    }
                }
            }
            free(pairs);
        }
    }

    delete_dgraph(gr);
    delete_dgraph(glinv);
    delete_parti(FOLDR);
    delete_parti(FOLDL);
    return true;
}


bool is_bpolamt_mono(subsemi* S, uint* cexa) {

    // We first check whether the AMT-kernel is J-trivial.
    if (!is_gtrivial_subsemi(S, J_GREEN, cexa)) {
        if (cexa) {
            make_cexample_from_jtriv(S->original, cexa);
        }
        return false;
    }

    morphism* M = S->original;


    // Computation of the spanning trees for all (regular) R-classes and L-classes
    num_span_trees* rspans = compute_num_span_trees(M, true);
    num_span_trees* lspans = compute_num_span_trees(M, false);

    // Loop over all idempotents e = qr
    for (uint i = 0; i < M->nb_idems; i++) {

        uint e = M->idem_list[i];

        // Loop over all idempotents f = st
        for (uint j = 0; j < M->nb_idems; j++) {
            uint f = M->idem_list[j];
            uint ef = mor_mult(M, e, f);


            // We compute the anti AMT-pairs (q,t) where q is in the R-class of e and t is in the L-class of f
            dequeue* p1 = create_dequeue();
            dequeue* p2 = create_dequeue();


            compute_amt_pairs_regular(rspans, lspans, e, f, p1, p2);


            for (uint p = 0; p < size_dequeue(p1); p++) {
                uint q = lefread_dequeue(p1, p);
                uint t = lefread_dequeue(p2, p);

                if (ef != mor_mult(M, q, t)) {
                    if (cexa) {
                        cexa[0] = q;
                        cexa[1] = get_rlink(M, M->rels->RCL, e, q);
                        cexa[2] = get_llink(M, M->rels->LCL, f, t);
                        cexa[3] = t;
                    }
                    delete_dequeue(p1);
                    delete_dequeue(p2);
                    delete_num_span_trees(rspans);
                    delete_num_span_trees(lspans);
                    return false;


                }
            }
            delete_dequeue(p1);
            delete_dequeue(p2);
        }
    }
    delete_num_span_trees(rspans);
    delete_num_span_trees(lspans);
    return true;
}

// Knast
bool is_knast_mono(orbits* L, uint* cexa) {


    // We first check whether the DD-orbits are J-trivial.
    if (!is_gtrivial_orbmono(L, J_GREEN, cexa)) {
        if (cexa) {
            uint e = cexa[2];
            make_cexample_from_jtriv(L->original, cexa);
            cexa[4] = e;
            cexa[5] = e;
        }
        return false;
    }



    morphism* M = L->original;
    green* G = L->original->rels;


    // Loop over all minimal idempotents e
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {
        uint e = M->regular_idems[i];

        // Loop over all minimal idempotents f
        // The case e = f is already handled: we checked if the DD-orbits are J-trivial.
        // The case f < e is treated when e and f are inverted.
        for (uint j = i + 1; j < M->nb_min_regular_jcl; j++) {
            uint f = M->regular_idems[j];

            // Intersection of MfM and eMe
            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            dequeue* Mf = compute_l_ideal(M, f, NULL);
            dequeue* fM = compute_r_ideal(M, f, NULL);

            // Idempotents in the intersection of MfM and eMe            
            dequeue* idems = make_inter_sorted_dequeue_array(MfM, M->idem_list, M->nb_idems);

            delete_dequeue(MfM);

            // Loop over idempotents g = eqfre
            for (uint x = 0; x < size_dequeue(idems); x++) {
                uint g = lefread_dequeue(idems, x);

                // The elements eqf such that eqf R g
                dequeue* lset = make_inter_sorted_dequeue_array(Mf, G->RCL->cl_elems[G->RCL->numcl[g]], G->RCL->cl_size[G->RCL->numcl[g]]);
                //dequeue* lset = make_inter_sorted_dequeue(Mf, G->RCL->cl[G->RCL->numcl[g]]);
                for (uint u = 0; u < size_dequeue(lset); u++) {
                    uint q = lefread_dequeue(lset, u);

                    // Loop over idempotents h = esfte
                    for (uint y = 0; y < size_dequeue(idems); y++) {
                        uint h = lefread_dequeue(idems, y);
                        uint gh = mor_mult(M, g, h);

                        // The elements fte such that fte L h
                        dequeue* rset = make_inter_sorted_dequeue_array(fM, G->LCL->cl_elems[G->LCL->numcl[h]], G->LCL->cl_size[G->LCL->numcl[h]]);
                        //dequeue* rset = make_inter_sorted_dequeue(fM, G->LCL->cl[G->LCL->numcl[h]]);
                        for (uint v = 0; v < size_dequeue(rset); v++) {
                            uint t = lefread_dequeue(rset, v);
                            if (gh != mor_mult(M, q, t)) {
                                if (cexa) {
                                    // Generation of a counterexample if necessary.
                                    cexa[0] = q;
                                    cexa[1] = get_rlink(M, M->rels->RCL, g, q);
                                    cexa[2] = get_llink(M, M->rels->LCL, h, t);
                                    cexa[3] = t;
                                    cexa[4] = e;
                                    cexa[5] = f;
                                }

                                delete_dequeue(rset);
                                delete_dequeue(lset);
                                delete_dequeue(idems);
                                delete_dequeue(fM);
                                delete_dequeue(Mf);
                                return false;
                            }
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
    return true;
}

// Knast on the a strict kernel
bool is_knast_ker(orbits* L, subsemi* ker, uint* cexa) {

    // We first check whether the G⁺-orbits are J-trivial.
    if (!is_gtrivial_orbmono(L, J_GREEN, cexa)) {
        if (cexa) {
            uint e = cexa[2];
            make_cexample_from_jtriv(L->original, cexa);
            cexa[4] = e;
            cexa[5] = e;
        }
        return false;
    }


    morphism* M = ker->original;
    green* G = ker->original->rels;



    // Loop over all idempotents e
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {
        uint e = M->regular_idems[i];

        // Loop over all idempotents f
        // The case e = f is already handled: we checked if the G⁺-orbits are J-trivial.
        // The case f < e is treated when e and f are inverted.
        for (uint j = i + 1; j < M->nb_min_regular_jcl; j++) {
            uint f = M->regular_idems[j];

            // Intersection of MfM and eMe
            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            // Mf and fM restricted to the G-kernel.
            dequeue* Mf = compute_l_ideal(M, f, ker->mono_in_sub);
            dequeue* fM = compute_r_ideal(M, f, ker->mono_in_sub);

            // Idempotents in the intersection of MfM and eMe.
            dequeue* idems = make_inter_sorted_dequeue_array(MfM, M->idem_list, M->nb_idems);
            delete_dequeue(MfM);

            // Loop over idempotents g = eqfre.
            for (uint x = 0; x < size_dequeue(idems); x++) {
                uint g = lefread_dequeue(idems, x);

                // The elements eqf such that eqf R g.
                dequeue* lset = make_inter_sorted_dequeue_array(Mf, G->RCL->cl_elems[G->RCL->numcl[g]], G->RCL->cl_size[G->RCL->numcl[g]]);
                //dequeue* lset = make_inter_sorted_dequeue(Mf, G->RCL->cl[G->RCL->numcl[g]]);
                for (uint u = 0; u < size_dequeue(lset); u++) {
                    uint q = lefread_dequeue(lset, u);

                    // Loop over idempotents h = esfte.
                    for (uint y = 0; y < size_dequeue(idems); y++) {
                        uint h = lefread_dequeue(idems, y);
                        uint gh = mor_mult(M, g, h);


                        // The elements fte such that fte L h.
                        dequeue* rset = make_inter_sorted_dequeue_array(fM, G->LCL->cl_elems[G->LCL->numcl[h]], G->LCL->cl_size[G->LCL->numcl[h]]);
                        //dequeue* rset = make_inter_sorted_dequeue(fM, G->LCL->cl[G->LCL->numcl[h]]);
                        for (uint v = 0; v < size_dequeue(rset); v++) {
                            uint t = lefread_dequeue(rset, v);
                            if (gh != mor_mult(M, q, t)) {
                                if (cexa) {
                                    cexa[0] = q;
                                    cexa[1] = get_rlink(M, M->rels->RCL, g, q);
                                    cexa[2] = get_llink(M, M->rels->LCL, h, t);
                                    cexa[3] = t;
                                    cexa[4] = e;
                                    cexa[5] = f;
                                }
                                delete_dequeue(rset);
                                delete_dequeue(lset);
                                delete_dequeue(idems);
                                delete_dequeue(fM);
                                delete_dequeue(Mf);
                                return false;
                            }
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
    return true;
}

bool is_bpolamtp_mono(orbits* L, uint* cexa) {
    morphism* M = L->original;

    // We first check if the AMT⁺-orbits are J-trivial.
    if (!is_gtrivial_orbmono(L, J_GREEN, cexa)) {
        if (cexa) {
            uint e = cexa[2];
            make_cexample_from_jtriv(L->original, cexa);
            cexa[4] = e;
            cexa[5] = e;
        }
        return false;
    }


    // Computation of the spanning trees for all (regular) R-classes and L-classes
    num_span_trees* rspans = compute_num_span_trees(M, true);
    num_span_trees* lspans = compute_num_span_trees(M, false);


    // Loop over all idempotents e.
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {
        uint e = M->regular_idems[i];

        // Loop over all idempotents f (the case f < e is treated when e and f are inverted).
        for (uint j = i; j < M->nb_min_regular_jcl; j++) {
            uint f = M->regular_idems[j];

            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);
            dequeue* candidates = make_inter_sorted_dequeue_array(MfM, M->idem_list, M->nb_idems);
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


                        if (gh != mor_mult(M, q, t)) {
                            if (cexa) {
                                cexa[0] = q;
                                cexa[1] = get_rlink(M, M->rels->RCL, g, q);
                                cexa[2] = get_llink(M, M->rels->LCL, h, t);
                                cexa[3] = t;
                                cexa[4] = e;
                                cexa[5] = f;
                            }
                            delete_dequeue(p1);
                            delete_dequeue(p2);
                            delete_dequeue(candidates);
                            delete_num_span_trees(rspans);
                            delete_num_span_trees(lspans);
                            return false;
                        }
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
    return true;
}






bool is_bpolgrp_mono(orbits* L, uint* cexa) {

    // We first check if the GR⁺-orbits are J-trivial.
    if (!is_gtrivial_orbmono(L, J_GREEN, cexa)) {
        if (cexa) {
            uint e = cexa[2];
            make_cexample_from_jtriv(L->original, cexa);
            cexa[4] = e;
            cexa[5] = e;
        }
        return false;
    }

    morphism* M = L->original;

    parti* FOLDR = mor_stal_fold(M, true, true);
    parti* FOLDL = mor_stal_fold(M, true, false);

    dgraph* gr = shrink_grp(M->r_cayley, FOLDR, M->rels->RCL);
    dgraph* glinv = shrink_grp_mirror(M->l_cayley, FOLDL, M->rels->LCL);


    uint size = 0;

    // Loop over all idempotents e.
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {
        uint e = M->regular_idems[i];

        // Loop over all idempotents f (the case f < e is treated when e and f are inverted).
        for (uint j = i; j < M->nb_min_regular_jcl; j++) {
            uint f = M->regular_idems[j];

            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            dequeue* candidates = make_inter_sorted_dequeue_array(MfM, M->idem_list, M->nb_idems);
            delete_dequeue(MfM);

            for (uint k = 0; k < size_dequeue(candidates); k++) {
                uint g = lefread_dequeue(candidates, k);
                for (uint l = 0; l < size_dequeue(candidates); l++) {
                    uint h = lefread_dequeue(candidates, l);
                    uint gh = mor_mult(M, g, h);
                    prod_pair* pairs = dgraph_intersec(gr, glinv, FOLDR->numcl[g], FOLDL->numcl[h], &size);
                    for (uint p = 0; p < size; p++) {
                        uint c = pairs[p].q1;
                        uint d = pairs[p].q2;

                        for (uint x = 0; x < FOLDR->cl_size[c]; x++) {
                            uint q = FOLDR->cl_elems[c][x];
                            if (mor_mult(M, q, f) != q) {
                                continue;
                            }

                            for (uint y = 0; y < FOLDL->cl_size[d]; y++) {
                                uint t = FOLDL->cl_elems[d][y];
                                if (mor_mult(M, f, t) != t) {
                                    continue;
                                }


                                if (gh != mor_mult(M, q, t)) {
                                    if (cexa) {
                                        cexa[0] = q;
                                        cexa[1] = get_rlink(M, M->rels->RCL, g, q);
                                        cexa[2] = get_llink(M, M->rels->LCL, h, t);
                                        cexa[3] = t;
                                        cexa[4] = e;
                                        cexa[5] = f;
                                    }
                                    free(pairs);
                                    delete_dgraph(gr);
                                    delete_dgraph(glinv);
                                    delete_parti(FOLDR);
                                    delete_parti(FOLDL);
                                    delete_dequeue(candidates);
                                    return false;
                                }
                            }
                        }
                    }
                    free(pairs);
                }
            }
            delete_dequeue(candidates);
        }
    }
    delete_dgraph(gr);
    delete_dgraph(glinv);
    delete_parti(FOLDR);
    delete_parti(FOLDL);
    return true;
}



// Knast for at-sets
bool is_knast_at_mono(orbits* L, uint* cexa) {


    // We first check if the AT-orbits are J-trivial.
    if (!is_gtrivial_orbmono(L, J_GREEN, cexa)) {
        if (cexa) {
            uint e = cexa[2];
            make_cexample_from_jtriv(L->original, cexa);
            cexa[4] = e;
            cexa[5] = e;
        }
        return false;
    }

    morphism* M = L->original;
    green* G = L->original->rels;



    // Loop over all idempotents e.
    for (uint i = 0; i < M->nb_idems; i++) {
        uint e = M->idem_list[i];

        // Loop over all idempotents f > e.
        // The case f < e is treated when e and f are inverted.
        // The case e = f is already handled: we checked if the AT-orbits are J-trivial.
        for (uint j = i + 1; j < M->nb_idems; j++) {
            uint f = M->idem_list[j];

            // We compute the maximal alphabet such that there exists two words
            // with this alphabet that maps to e and f respectively.
            bool* efalph = compute_maxalph_com_scc(M, e, f);

            // If there is no such alphabet, we skip the current pair.
            if (!efalph) {
                continue;
            }

            // R-classes for the restricted alphabets.
            parti* rSCCS = dtarjan(M->r_cayley, efalph, false);

            // L-classes for the restricted alphabets.
            parti* lSCCS = dtarjan(M->l_cayley, efalph, false);

            // We compute the idempotents in eMfMe which have an antecedent in the restricted alphabet.
            dequeue* eM = compute_r_ideal_alph(M, e, efalph, M->idem_array);
            dequeue* Me = compute_l_ideal_alph(M, e, efalph, M->idem_array);
            dequeue* eMe = make_inter_sorted_dequeue(eM, Me);
            delete_dequeue(eM);
            delete_dequeue(Me);
            dequeue* MfM = compute_j_ideal_alph(M, f, efalph, M->idem_array);
            dequeue* idems = make_inter_sorted_dequeue(eMe, MfM);
            delete_dequeue(eMe);
            delete_dequeue(MfM);


            // The sets fM and Mf restricted to the restricted alphabet will be useful.
            dequeue* fM = compute_r_ideal_alph(M, f, efalph, G->regular_array);
            dequeue* Mf = compute_l_ideal_alph(M, f, efalph, G->regular_array);



            // Loop over idempotents g = eqfre
            for (uint x = 0; x < size_dequeue(idems); x++) {
                uint g = lefread_dequeue(idems, x);

                // Loop over idempotents h = esfte
                for (uint y = 0; y < size_dequeue(idems); y++) {
                    uint h = lefread_dequeue(idems, y);
                    uint gh = mor_mult(M, g, h);

                    // The elements eqf such that eqf R g (for the restricted alphabet)
                    dequeue* lset = make_inter_sorted_dequeue_array(Mf, rSCCS->cl_elems[rSCCS->numcl[g]], rSCCS->cl_size[rSCCS->numcl[g]]);
                    //dequeue* lset = make_inter_sorted_dequeue(Mf, rSCCS->cl[rSCCS->numcl[g]]);
                    for (uint u = 0; u < size_dequeue(lset); u++) {
                        uint q = lefread_dequeue(lset, u);



                        // The elements fte such that fte L h (for the restricted alphabet)
                        dequeue* rset = make_inter_sorted_dequeue_array(fM, lSCCS->cl_elems[lSCCS->numcl[h]], lSCCS->cl_size[lSCCS->numcl[h]]);
                        //dequeue* rset = make_inter_sorted_dequeue(fM, lSCCS->cl[lSCCS->numcl[h]]);
                        for (uint v = 0; v < size_dequeue(rset); v++) {
                            uint t = lefread_dequeue(rset, v);
                            if (gh != mor_mult(M, q, t)) {
                                if (cexa) {
                                    cexa[0] = q;
                                    cexa[1] = get_rlink(M, rSCCS, g, q);
                                    cexa[2] = get_llink(M, lSCCS, h, t);
                                    cexa[3] = t;
                                    cexa[4] = e;
                                    cexa[5] = f;
                                }
                                delete_dequeue(rset);
                                delete_dequeue(lset);
                                delete_dequeue(idems);
                                delete_dequeue(fM);
                                delete_dequeue(Mf);
                                delete_parti(rSCCS);
                                delete_parti(lSCCS);
                                return false;
                            }
                        }
                        delete_dequeue(rset);
                    }
                    delete_dequeue(lset);
                }
            }
            delete_dequeue(idems);
            delete_dequeue(fM);
            delete_dequeue(Mf);
            delete_parti(rSCCS);
            delete_parti(lSCCS);
            free(efalph);


        }
    }
    return true;
}

/*********************/
/* UPolBPol Equation */
/*********************/

bool is_upbp_mono(orbits* L, uint* cexa) {
    morphism* M = L->original;

    // Loop over all representative idempotents e
    for (uint i = 0; i < L->nb_computed; i++) {
        uint e = L->orbits[i]->sub_to_mono[L->orbits[i]->neut];
        // For all elements ese in the orbit of e
        for (uint k = 0; k < L->orbits[i]->size; k++) {
            uint ese = L->orbits[i]->sub_to_mono[k];
            if (ese == e) {
                // If ese = e, the equation is trivially satsfied for all t
                continue;
            }

            // We do a BFS to explore all elements t such that eset is an idempotent
            dequeue* elem = create_dequeue();
            dequeue* path = create_dequeue();
            rigins_dequeue(e, elem);
            rigins_dequeue(ese, path);
            bool* visited;
            CALLOC(visited, M->r_cayley->size_graph);
            while (!isempty_dequeue(path)) {
                uint t = lefpull_dequeue(elem);
                uint p = lefpull_dequeue(path);

                // If t is already visited, we skip it
                if (visited[t]) {
                    continue;
                }
                visited[t] = true;

                // If p is idempotent, we check the equation for this t
                if (M->idem_array[p]) {
                    if (mor_mult_gen(M, 3, p, t, p) != p) {
                        if (cexa) {
                            cexa[0] = ese;
                            cexa[1] = t;
                            cexa[2] = e;
                        }
                        free(visited);
                        delete_dequeue(elem);
                        delete_dequeue(path);
                        return false;
                    }
                }

                // We continue the BFS
                for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
                    rigins_dequeue(M->r_cayley->edges[t][a], elem);
                    rigins_dequeue(M->r_cayley->edges[p][a], path);
                }
            }
            delete_dequeue(elem);
            delete_dequeue(path);
            free(visited);


        }
    }
    return true;
}
