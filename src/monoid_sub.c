#include "monoid_sub.h"
#include "monoid_display.h"
#include <limits.h>

/******************************/
/* Manipulating subsemigroups */
/******************************/


subsemi* init_subsemi(morphism* M) {
    subsemi* S;
    CALLOC(S, 1);
    S->original = M;

    // Other members are not initialized at this stage.
    S->mono_in_sub = NULL;
    S->mono_to_sub = NULL;
    S->sub_to_mono = NULL;
    S->idem_array = NULL;
    S->idem_list = NULL;
    S->rels = NULL;
    S->nb_idems = 0;
    return S;
}


void compute_maps_subsemi(subsemi* S) {
    if (!S || !S->original || !S->mono_in_sub) {
        fprintf(stderr, "Error: compute_maps_subsemi: The subsemigroup is not correctly initialized.\n");
        exit(EXIT_FAILURE);
    }

    if (S->mono_to_sub) {
        free(S->mono_to_sub);
    }

    if (S->sub_to_mono) {
        free(S->sub_to_mono);
    }

    MALLOC(S->mono_to_sub, S->original->r_cayley->size_graph);
    MALLOC(S->sub_to_mono, S->size);
    uint num = 0;
    for (uint s = 0; s < S->original->r_cayley->size_graph; s++) {
        if (S->mono_in_sub[s]) {
            S->mono_to_sub[s] = num;
            S->sub_to_mono[num] = s;
            num++;
        }
    }
}

void compute_idems_subsemi(subsemi* S) {
    if (!S || !S->original || !S->mono_in_sub || !S->sub_to_mono) {
        fprintf(stderr, "Error: compute_idems_subsemi: The subsemigroup is not correctly initialized.\n");
        exit(EXIT_FAILURE);
    }
    if (S->idem_array) {
        free(S->idem_array);
    }
    if (S->idem_list) {
        free(S->idem_list);
    }
    S->nb_idems = 0;
    MALLOC(S->idem_array, S->size);
    for (uint j = 0; j < S->size; j++) {
        S->idem_array[j] = S->original->idem_array[S->sub_to_mono[j]];
        if (S->idem_array[j]) {
            S->nb_idems++;
        }
    }
    MALLOC(S->idem_list, S->nb_idems);
    uint num = 0;
    for (uint j = 0; j < S->size; j++) {
        if (S->idem_array[j]) {
            S->idem_list[num] = j;
            num++;
        }
    }
}


uint* compute_jord_subsemi(subsemi* S) {
    green* G = S->original->rels;
    uint* jord;
    MALLOC(jord, S->size);
    uint num = 0;
    for (uint cjo = 0; cjo < G->JCL->size_par; cjo++) {
        for (uint i = 0; i < G->JCL->cl_size[cjo]; i++) {
            if (S->mono_in_sub[G->JCL->cl_elems[cjo][i]]) {
                jord[num] = S->mono_to_sub[G->JCL->cl_elems[cjo][i]];
                num++;
            }
        }
    }
    return jord;
}

void compute_jrel_subsemi(subsemi* S, uint* jord) {
    if (!S || !S->original || !S->mono_in_sub || !S->sub_to_mono || !S->idem_array || !S->idem_list || !S->rels || !S->rels->RCL || !S->rels->LCL) {
        fprintf(stderr, "Error: compute_jrel_subsemi: The subsemigroup is not correctly initialized.\n");
        exit(EXIT_FAILURE);
    }

    if (S->rels->JCL) {
        delete_parti(S->rels->JCL);
    }

    // MALLOC(S->rels->JCL, 1);
    // S->rels->JCL->size_set = S->size;
    uint* numcl;
    MALLOC(numcl, S->size);

    bool* done;
    CALLOC(done, S->size);

    uint num = 0;

    for (uint i = 0; i < S->size; i++) {
        if (!done[jord[i]]) {
            uint cr = S->rels->RCL->numcl[jord[i]];
            for (uint j = 0; j < S->rels->RCL->cl_size[cr]; j++) {
                uint s = S->rels->RCL->cl_elems[cr][j];
                if (!done[s]) {
                    uint cl = S->rels->LCL->numcl[s];
                    for (uint k = 0; k < S->rels->LCL->cl_size[cl]; k++) {
                        numcl[S->rels->LCL->cl_elems[cl][k]] = num;
                        done[S->rels->LCL->cl_elems[cl][k]] = true;
                    }
                }
            }
            num++;
        }
    }

    S->rels->JCL = create_parti(S->size, num, numcl);

    // S->rels->JCL->size_par = num;
    // MALLOC(S->rels->JCL->cl, num);
    // for (uint i = 0; i < num; i++) {
    //     S->rels->JCL->cl[i] = create_dequeue();
    // }
    // for (uint s = 0; s < S->size; s++) {
    //     rigins_dequeue(s, S->rels->JCL->cl[S->rels->JCL->numcl[s]]);
    // }
    free(done);
}


// Release of a subsemigroup.
// Does not release the original morphism and its Green relations.
void delete_subsemi(subsemi* S) {
    if (S == NULL) {
        return;
    }

    free(S->mono_in_sub);
    free(S->mono_to_sub);
    free(S->sub_to_mono);
    free(S->idem_array);
    free(S->idem_list);
    delete_green(S->rels);
    free(S);
}

// Given a list of elements in a monoid and a subsemigroup of a monoid, returns the sublist
// restricted to the elements inside the subsemigroup.
dequeue* restrict_list_to_subsemi(subsemi* S, dequeue* l) {
    dequeue* result = create_dequeue();
    for (uint i = 0; i < size_dequeue(l); i++) {
        if (S->mono_in_sub == NULL || S->mono_in_sub[lefread_dequeue(l, i)]) {
            rigins_dequeue(lefread_dequeue(l, i), result);
        }
    }

    return result;
}





void green_compute_sub(subsemi* S) {


    graph* cayr = create_graph_noedges(S->size);
    graph* cayl = create_graph_noedges(S->size);
    graph* cayj = create_graph_noedges(S->size);

    // For each element q in N
    for (uint q = 0; q < S->size; q++) {
        // Initializing the arrays that store the elements in qN and Nq.
        bool rmult[S->size];
        bool lmult[S->size];
        for (uint s = 0; s < S->size; s++) {
            rmult[s] = false;
            lmult[s] = false;
        }
        // Computation of the elements in qN and Nq.
        for (uint r = 0; r < S->size; r++) {
            rmult[S->mono_to_sub[mor_mult(S->original, S->sub_to_mono[q], S->sub_to_mono[r])]] = true;
            lmult[S->mono_to_sub[mor_mult(S->original, S->sub_to_mono[r], S->sub_to_mono[q])]] = true;
        }
        for (uint s = 0; s < S->size; s++) {
            if (rmult[s]) {
                rigins_dequeue(s, cayr->edges[q]);
            }
            if (lmult[s]) {
                rigins_dequeue(s, cayl->edges[q]);
            }
            if (rmult[s] || lmult[s]) {
                rigins_dequeue(s, cayj->edges[q]);
            }
        }
    }

    CALLOC(S->rels, 1);

    // Computes the R-classes.
    S->rels->RCL = tarjan(cayr);
    delete_graph(cayr);

    // Computes the L-classes.
    S->rels->LCL = tarjan(cayl);
    delete_graph(cayl);

    // Computes the J-classes.
    S->rels->JCL = tarjan(cayj);
    delete_graph(cayj);


    // Computes the H-classes
    h_green_compute(S->rels);

    // Computes the groups and the regular elements.

    gr_green_compute(S->idem_list, S->nb_idems, S->rels);
}




void green_compute_sub_reg(subsemi* S) {

    // The original Green relations.
    green* G = S->original->rels;


    // Array of regular R-classes
    // Each regular element will be mapped to an idempotent in its R-class
    // Each non-regular element will be mapped to UINT_MAX
    uint* rarray;
    MALLOC(rarray, S->size);

    // Array of regular L-classes
    // Each regular element will be mapped to an idempotent in its L-class
    // Each non-regular element will be mapped to UINT_MAX
    uint* larray;
    MALLOC(larray, S->size);

    // Initialization of the arrays
    for (uint i = 0; i < S->size; i++) {
        rarray[i] = UINT_MAX;
        larray[i] = UINT_MAX;
    }


    // Filling the arrays: loop over all idempotents in the subsemigroup
    // First, we restrict the regular classes to the elements in the subsemigroup
    for (uint i = 0; i < S->nb_idems; i++) {

        // Index of the idempotent in the subsemigroup
        uint sube = S->idem_list[i];

        // Index of the idempotent in the original monoid
        uint e = S->sub_to_mono[sube];

        // If the idempotent has not already been handled as being part of the R-class of another idempotent
        if (rarray[sube] == UINT_MAX) {
            // For each element in the original R-class of the idempotent
            for (uint j = 0; j < G->RCL->cl_size[G->RCL->numcl[e]]; j++) {
                uint s = G->RCL->cl_elems[G->RCL->numcl[e]][j];
                // If the element is in the subsemigroup
                if (S->mono_in_sub[s]) {
                    rarray[S->mono_to_sub[s]] = sube;
                }
            }

        }

        // If the idempotent has not already been handled as being part of the L-class of another idempotent
        if (larray[sube] == UINT_MAX) {
            // For each element in the original L-class of the idempotent
            for (uint j = 0; j < G->LCL->cl_size[G->LCL->numcl[e]]; j++) {
                uint s = G->LCL->cl_elems[G->LCL->numcl[e]][j];
                // If the element is in the subsemigroup
                if (S->mono_in_sub[s]) {
                    larray[S->mono_to_sub[s]] = sube;
                }
            }
        }
    }


    // For each regular element of the subsemigroup
    // We are now going to check whether it remains regular or not

    // Array to keep track of the elements that have already been handled
    bool* done;
    CALLOC(done, S->size);

    // For each element of the subsemigroup
    for (uint subs = 0; subs < S->size; subs++) {
        //mor_fprint_name_utf8(S->original, S->sub_to_mono[subs], stdout);
        //printf("\n");

        // If the element has already been handled
        if (done[subs]) {
            continue;
        }

        // The idempotent that was R-equivalent to the element (if it exists)
        uint sube = rarray[subs];
        // The idempotent that was L-equivalent to the element (if it exists)
        uint subf = larray[subs];

        // If one of the two idempotents does not exist, the element is not regular anymore.
        if (sube == UINT_MAX || subf == UINT_MAX) {
            rarray[subs] = UINT_MAX;
            larray[subs] = UINT_MAX;
            continue;
        }


        uint e = S->sub_to_mono[sube];
        uint f = S->sub_to_mono[subf];


        // The R-class of e restricted to the elements in the subsemigroup
        dequeue* re = create_dequeue();
        for (uint i = 0; i < G->RCL->cl_size[G->RCL->numcl[e]]; i++) {
            uint s = G->RCL->cl_elems[G->RCL->numcl[e]][i];
            if (S->mono_in_sub[s]) {
                rigins_dequeue(s, re);
            }
        }
        // The R-class of f restricted to the elements in the subsemigroup
        dequeue* rf = create_dequeue();
        for (uint i = 0; i < G->RCL->cl_size[G->RCL->numcl[f]]; i++) {
            uint s = G->RCL->cl_elems[G->RCL->numcl[f]][i];
            if (S->mono_in_sub[s]) {
                rigins_dequeue(s, rf);
            }
        }
        // The L-class of e restricted to the elements in the subsemigroup
        dequeue* le = create_dequeue();
        for (uint i = 0; i < G->LCL->cl_size[G->LCL->numcl[e]]; i++) {
            uint s = G->LCL->cl_elems[G->LCL->numcl[e]][i];
            if (S->mono_in_sub[s]) {
                rigins_dequeue(s, le);
            }
        }
        // The L-class of e restricted to the elements in the subsemigroup
        dequeue* lf = create_dequeue();
        for (uint i = 0; i < G->LCL->cl_size[G->LCL->numcl[f]]; i++) {
            uint s = G->LCL->cl_elems[G->LCL->numcl[f]][i];
            if (S->mono_in_sub[s]) {
                rigins_dequeue(s, lf);
            }
        }


        // Intersection of the R-class of e and the L-class of f
        // Nonempty: it contains subs
        dequeue* reilf = make_inter_sorted_dequeue(re, lf);


        // Intersection of the L-class of e and the R-class of f
        // Possibly empty. In this case, none of the elements in reilf are regular
        // Otherwise, all elements in reilf and leirf are regular
        dequeue* leirf = make_inter_sorted_dequeue(le, rf);


        delete_dequeue(re);
        delete_dequeue(rf);
        delete_dequeue(le);
        delete_dequeue(lf);



        // If the intersection leirf is empty, none of the elements in reilf are regular
        if (isempty_dequeue(leirf)) {
            for (uint i = 0; i < size_dequeue(reilf); i++) {
                uint t = lefread_dequeue(reilf, i);
                uint subt = S->mono_to_sub[t];
                rarray[subt] = UINT_MAX;
                larray[subt] = UINT_MAX;
            }
        }

        // All elements in the intersections are now handled.
        for (uint i = 0; i < size_dequeue(reilf); i++) {
            uint t = lefread_dequeue(reilf, i);
            uint subt = S->mono_to_sub[t];
            done[subt] = true;
        }
        for (uint i = 0; i < size_dequeue(leirf); i++) {
            uint t = lefread_dequeue(leirf, i);
            uint subt = S->mono_to_sub[t];
            done[subt] = true;
        }



        delete_dequeue(reilf);
        delete_dequeue(leirf);
    }

    free(done);
    // We can now compute the Green relations.


    // Initialization
    CALLOC(S->rels, 1);


    // Computation of the R-classes and L-classes.
    uint* rnumcl;
    uint* lnumcl;
    MALLOC(rnumcl, S->size);
    MALLOC(lnumcl, S->size);

    uint rsize_par = 0;
    uint lsize_par = 0;


    // Computation of the classes indices for regular elements (we keep the ordering of the original monoid)
    for (uint i = 0; i < G->RCL->size_par; i++) {
        bool inter = false;
        for (uint j = 0; j < G->RCL->cl_size[i]; j++) {
            uint s = G->RCL->cl_elems[i][j];
            if (S->mono_in_sub[s] && rarray[S->mono_to_sub[s]] != UINT_MAX) {
                inter = true;
                rnumcl[S->mono_to_sub[s]] = rsize_par;
            }

        }
        if (inter) {
            rsize_par++;
        }
    }


    for (uint i = 0; i < G->LCL->size_par; i++) {
        bool inter = false;
        for (uint j = 0; j < G->LCL->cl_size[i]; j++) {
            uint s = G->LCL->cl_elems[i][j];
            if (S->mono_in_sub[s] && larray[S->mono_to_sub[s]] != UINT_MAX) {
                inter = true;
                lnumcl[S->mono_to_sub[s]] = lsize_par;
            }

        }
        if (inter) {
            lsize_par++;
        }
    }

    // Computation of the classes indices for the non-regular elements (placed at the end)
    for (uint t = 0; t < S->size;t++) {
        if (rarray[t] == UINT_MAX) {
            rnumcl[t] = rsize_par;
            rsize_par++;
        }
        if (larray[t] == UINT_MAX) {
            lnumcl[t] = lsize_par;
            lsize_par++;
        }
    }

    // Computation of the classes
    S->rels->RCL = create_parti(S->size, rsize_par, rnumcl);
    S->rels->LCL = create_parti(S->size, lsize_par, lnumcl);
    // MALLOC(S->rels->RCL->cl, S->rels->RCL->size_par);
    // for (uint c = 0; c < S->rels->RCL->size_par; c++) {
    //     S->rels->RCL->cl[c] = create_dequeue();
    // }
    // MALLOC(S->rels->LCL->cl, S->rels->LCL->size_par);
    // for (uint c = 0; c < S->rels->LCL->size_par; c++) {
    //     S->rels->LCL->cl[c] = create_dequeue();
    // }
    // for (uint s = 0; s < S->size; s++) {
    //     rigins_dequeue(s, S->rels->RCL->cl[S->rels->RCL->numcl[s]]);
    //     rigins_dequeue(s, S->rels->LCL->cl[S->rels->LCL->numcl[s]]);
    // }

    free(larray);
    free(rarray);

    // Computes the J-classes

    uint* jord = compute_jord_subsemi(S);


    uint* jnumcl;
    MALLOC(jnumcl, S->size);
    uint jsize_par = 0;
    // MALLOC(S->rels->JCL, 1);
    // MALLOC(S->rels->JCL->numcl, S->size);
    // S->rels->JCL->size_set = S->size;
    // S->rels->JCL->size_par = 0;


    for (uint i = 0; i < S->size; i++) {
        jnumcl[i] = UINT_MAX;
    }

    for (uint h = 0; h < S->size; h++) {
        uint t = jord[h];
        if (jnumcl[t] != UINT_MAX) {
            continue;
        }
        uint i = S->rels->RCL->numcl[t];
        for (uint j = 0; j < S->rels->RCL->cl_size[i]; j++) {
            uint s = S->rels->RCL->cl_elems[i][j];
            if (jnumcl[s] != UINT_MAX) {
                continue;
            }
            uint n = S->rels->LCL->numcl[s];
            for (uint k = 0; k < S->rels->LCL->cl_size[n]; k++) {
                jnumcl[S->rels->LCL->cl_elems[n][k]] = jsize_par;
            }
        }
        jsize_par++;
    }

    S->rels->JCL = create_parti(S->size, jsize_par, jnumcl);



    // MALLOC(S->rels->JCL->cl, S->rels->JCL->size_par);
    // for (uint c = 0; c < S->rels->JCL->size_par; c++) {
    //     S->rels->JCL->cl[c] = create_dequeue();
    // }
    // for (uint i = 0; i < S->size; i++) {
    //     rigins_dequeue(i, S->rels->JCL->cl[S->rels->JCL->numcl[i]]);
    // }

    free(jord);



    // Computes the H-classes
    h_green_compute(S->rels);

    // Computes the groups and the regular elements.

    gr_green_compute(S->idem_list, S->nb_idems, S->rels);

}






