#include "sep_group.h"
#include "type_hash.h"


/********************/
/*+ AMT-separation +*/
/********************/

bool solve_system_amt(fmpz_mat_t MAT, int* target, uint nb_rows, uint nb_cols) {

    // This Boolean will be set to false if we find that s is not in the kernel.
    bool inside = true;

    uint row = 0;
    for (uint a = 0; a < nb_cols; a++) {
        int val;
        if (row >= nb_rows) {
            val = 0;
        }
        else {
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

static void compute_span_forest_aux(dgraph* G, parti* sccs, uint q, uint num, num_span_forest* span, bool* visited) {

    // Queue for the BFS.
    dequeue* thequeue = create_dequeue();

    // We first mark q as visited.
    visited[q] = true;
    span->numtree[q] = num; // We set the tree number of q to num.

    // We first enqueue the edges starting at q, they will be in the spanning tree
    for (uint a = 0; a < G->size_alpha; a++) {
        // We only consider the edges internal to the R-class of e.
        if (G->edges[q][a] != UINT_MAX && (!sccs || sccs->numcl[q] == sccs->numcl[G->edges[q][a]])) {
            rigins_dequeue(q * G->size_alpha + a, thequeue);
        }
    }

    // The BFS starts here.
    while (!isempty_dequeue(thequeue)) {
        // We take the first edge (r,b,s) in the queue.
        uint p = lefpull_dequeue(thequeue);
        uint r = p / G->size_alpha;
        uint b = p % G->size_alpha;
        uint s = G->edges[r][b];

        // If s has already been treated, (r,b,s) is not in the spanning tree.
        // We put it in the queue of dropped edges.
        if (visited[s]) {
            rigins_dequeue(p, span->dropped[num]);
            continue;
        }

        // If s has not been treated, it is part of the spanning tree. We update the
        // counts of the letters on the path from e to s and we insert each edge starting
        // at s in the queue and that does not leave the class.
        visited[s] = true;
        span->numtree[s] = num; // We set the tree number of s to num.
        for (uint a = 0; a < G->size_alpha; a++) {
            span->span_forest[s][a] = span->span_forest[r][a];
            if (G->edges[s][a] != UINT_MAX && (!sccs || sccs->numcl[s] == sccs->numcl[G->edges[s][a]])) {
                rigins_dequeue(s * G->size_alpha + a, thequeue);
            }
        }
        span->span_forest[s][b]++;
    }
    delete_dequeue(thequeue);

}

num_span_forest* compute_span_forest(dgraph* G, parti* sccs, bool* allowed) {
    num_span_forest* ret;
    MALLOC(ret, 1);


    // Initialization of the trees.
    MALLOC(ret->span_forest, G->size_graph);
    MALLOC(ret->numtree, G->size_graph);
    for (uint i = 0; i < G->size_graph; i++) {
        ret->numtree[i] = UINT_MAX; // We initialize the tree number of each state to UINT_MAX.
        CALLOC(ret->span_forest[i], G->size_alpha);
    }
    ret->size_graph = G->size_graph;
    ret->size_alpha = G->size_alpha;

    //Array memorizing the elements already treated.
    bool* visited;
    CALLOC(visited, G->size_graph);


    if (sccs) {
        // We initialize the number of trees in the spanning forest to the number of SCCs.
        ret->nb_trees = sccs->size_par;
        CALLOC(ret->root, ret->nb_trees);
        CALLOC(ret->dropped, ret->nb_trees);
        uint num = 0;
        for (uint i = 0; i < G->size_graph; i++) {
            if (visited[i] || (allowed && !allowed[i])) {
                continue;
            }
            ret->root[num] = i;
            ret->dropped[num] = create_dequeue(); // We create a queue for the dropped edges of the i-th element.
            compute_span_forest_aux(G, sccs, i, num, ret, visited);
            num++;
        }
        if (num < ret->nb_trees) {
            ret->nb_trees = num;
            REALLOC(ret->root, ret->nb_trees);
            REALLOC(ret->dropped, ret->nb_trees);
        }

    }
    else {
        ret->nb_trees = 1;
        CALLOC(ret->root, ret->nb_trees);
        CALLOC(ret->dropped, ret->nb_trees);
        ret->root[0] = 0; // We set the root of the only tree to 0.
        ret->dropped[0] = create_dequeue(); // We create a queue for the dropped edges of the i-th element.
        compute_span_forest_aux(G, NULL, 0, 0, ret, visited);
    }

    free(visited);

    return ret;
}


void delete_span_forest(num_span_forest* S) {
    if (!S) {
        return;
    }
    for (uint i = 0; i < S->size_graph; i++) {
        free(S->span_forest[i]);
    }
    free(S->span_forest);
    free(S->numtree);
    for (uint i = 0; i < S->nb_trees; i++) {
        if (S->dropped[i]) {
            delete_dequeue(S->dropped[i]);
        }
    }
    free(S->dropped);
    free(S->root);
    free(S);
}





static void build_hnf_matrix(dgraph* g, num_span_forest* span, fmpz_mat_struct* MAT, uint i) {
    uint size_base = size_dequeue(span->dropped[i]);
    fmpz_mat_init(MAT, size_base, g->size_alpha);
    for (uint j = 0; j < size_dequeue(span->dropped[i]); j++) {
        uint p = lefread_dequeue(span->dropped[i], j);
        uint r = p / g->size_alpha;
        uint b = p % g->size_alpha;
        uint s = g->edges[r][b];
        for (uint a = 0; a < g->size_alpha; a++) {
            if (a != b) {
                fmpz_set_si(fmpz_mat_entry(MAT, j, a), span->span_forest[r][a] - span->span_forest[s][a]);
            }
            else {
                fmpz_set_si(fmpz_mat_entry(MAT, j, a), span->span_forest[r][a] - span->span_forest[s][a] + 1);
            }
        }
    }

    // We put the matrix in HNF.
    fmpz_mat_hnf(MAT, MAT);
}

parti* dgraph_amt_fold(dgraph* g, parti* sccs) {
    num_span_forest* span = compute_span_forest(g, sccs, NULL);

    ufind* uf = create_ufind(g->size_graph);

    for (uint i = 0; i < span->nb_trees;i++) {
        // We compute the matrix corresponding to the cycle base from the spanning tree and the list of dropped edges.
        uint size_base = size_dequeue(span->dropped[i]);
        fmpz_mat_t MAT;
        build_hnf_matrix(g, span, MAT, i);
        int vec[g->size_alpha];

        if (sccs) {
            uint rootcl = sccs->numcl[span->root[i]];
            bool* treated;
            CALLOC(treated, sccs->cl_size[rootcl]);
            for (uint h = 0; h < sccs->cl_size[rootcl]; h++) {
                if (treated[h]) {
                    continue;
                }
                uint q = sccs->cl_elems[rootcl][h];

                for (uint j = h + 1; j < sccs->cl_size[rootcl];j++) {
                    uint r = sccs->cl_elems[rootcl][j];
                    for (uint a = 0; a < g->size_alpha; a++) {
                        vec[a] = span->span_forest[q][a] - span->span_forest[r][a];
                    }
                    if (solve_system_amt(MAT, vec, size_base, g->size_alpha)) {
                        // If the system is solvable, we merge the two elements in the union find.
                        union_ufind(q, r, uf);
                        treated[j] = true;
                    }

                }
                treated[h] = true;
            }
            free(treated);
        }
        else {
            bool* treated;
            CALLOC(treated, g->size_graph);
            for (uint h = 0; h < g->size_graph; h++) {
                if (treated[h]) {
                    continue;
                }

                for (uint j = h + 1; j < g->size_graph;j++) {
                    for (uint a = 0; a < g->size_alpha; a++) {
                        vec[a] = span->span_forest[h][a] - span->span_forest[j][a];
                    }
                    if (solve_system_amt(MAT, vec, size_base, g->size_alpha)) {
                        // If the system is solvable, we merge the two elements in the union find.
                        union_ufind(h, j, uf);
                        treated[j] = true;
                    }

                }
                treated[h] = true;
            }
            free(treated);
        }


        fmpz_mat_clear(MAT);
    }
    parti* ret = ufind_to_parti(uf);
    delete_ufind(uf);
    // We delete the single spanning forest.
    delete_span_forest(span);

    return ret;
}




void compute_amt_kernel_regular(morphism* M, bool* mono_in_sub, uint* size) {
    // Computation of the spanning forest of the right Cayley graph.
    // Only idempotents are allowed to be root nodes.
    num_span_forest* span = compute_span_forest(M->r_cayley, M->rels->RCL, M->idem_array);

    for (uint i = 0; i < span->nb_trees; i++) {
        // The idempotent root of the i-th tree.
        uint e = span->root[i];

        // Number of cycles in the base of the i-th tree.
        uint size_base = size_dequeue(span->dropped[i]);

        // We compute the hnf matrix corresponding to the cycle base from the spanning tree and the list of dropped edges.
        fmpz_mat_t MAT;
        build_hnf_matrix(M->r_cayley, span, MAT, i);

        // It remains to solve the system of equations given by the HNF for each element of the R-class
        // of the idempotent e. This checks whether the element is in the kernel or not.

        for (uint k = 0; k < M->rels->RCL->cl_size[M->rels->RCL->numcl[e]]; k++) {

            // We take the k-th element of the R-class of e.
            uint s = M->rels->RCL->cl_elems[M->rels->RCL->numcl[e]][k];

            // If s is an idempotent, we skip it: it is in the kernel.
            // We do not need to check it.
            if (M->idem_array[s]) {
                mono_in_sub[s] = true;
                (*size)++;
                continue;
            }

            // We now know that s is not an idempotent.
            // We check if it is in the kernel or not.
            if (solve_system_amt(MAT, span->span_forest[s], size_base, M->r_cayley->size_alpha)) {
                mono_in_sub[s] = true;
                (*size)++;
            }
        }

        // We do not need the matrix anymore, we can delete it.
        fmpz_mat_clear(MAT);

    }
    delete_span_forest(span);
}


void build_hnf_matrix_two(dgraph* g1, dgraph* g2, num_span_forest* span1, num_span_forest* span2, uint q1, uint q2, fmpz_mat_t MAT) {
    uint i1 = span1->numtree[q1];
    uint i2 = span2->numtree[q2];
    uint size_base1 = size_dequeue(span1->dropped[i1]);
    uint size_base2 = size_dequeue(span2->dropped[i2]);
    uint size_base = size_base1 + size_base2;
    fmpz_mat_init(MAT, size_base, g1->size_alpha);

    for (uint j = 0; j < size_base1; j++) {

        // We take the dropped edge (r,b,s) from the queue.
        uint p = lefread_dequeue(span1->dropped[i1], j);
        uint r = p / span1->size_alpha;
        uint b = p % span1->size_alpha;
        uint s = g1->edges[r][b];

        // We fill the matrix row corresponding to the cycle closed by this dropped edge.
        for (uint a = 0; a < span1->size_alpha; a++) {
            if (a != b) {
                fmpz_set_si(fmpz_mat_entry(MAT, j, a), span1->span_forest[r][a] - span1->span_forest[s][a]);
            }
            else {
                fmpz_set_si(fmpz_mat_entry(MAT, j, a), span1->span_forest[r][a] - span1->span_forest[s][a] + 1);
            }
        }
    }
    for (uint j = 0; j < size_base2; j++) {

        // We take the dropped edge (r,b,s) from the queue.
        uint p = lefread_dequeue(span2->dropped[i2], j);
        uint r = p / span2->size_alpha;
        uint b = p % span2->size_alpha;
        uint s = g2->edges[r][b];

        // We fill the matrix row corresponding to the cycle closed by this dropped edge.
        for (uint a = 0; a < span2->size_alpha; a++) {
            if (a != b) {
                fmpz_set_si(fmpz_mat_entry(MAT, j + size_base1, a), span2->span_forest[r][a] - span2->span_forest[s][a]);
            }
            else {
                fmpz_set_si(fmpz_mat_entry(MAT, j + size_base1, a), span2->span_forest[r][a] - span2->span_forest[s][a] + 1);
            }
        }
    }

    // We put the matrix in HNF.
    fmpz_mat_hnf(MAT, MAT);
}




void compute_amt_pairs_regular(morphism* M, num_span_forest* rspan, num_span_forest* lspan, uint e, uint f, dequeue* p1, dequeue* p2) {

    // We compute the matrix corresponding to the cycle base (R-class of e + L-class of f).
    uint size_ebase = size_dequeue(rspan->dropped[rspan->numtree[e]]);
    uint size_fbase = size_dequeue(lspan->dropped[lspan->numtree[f]]);
    fmpz_mat_t MAT;
    fmpz_mat_init(MAT, size_ebase + size_fbase, rspan->size_alpha);

    for (uint j = 0; j < size_ebase; j++) {

        // We take the dropped edge (r,b,s) from the queue.
        uint p = lefread_dequeue(rspan->dropped[rspan->numtree[e]], j);
        uint r = p / rspan->size_alpha;
        uint b = p % rspan->size_alpha;
        uint s = M->r_cayley->edges[r][b];

        // We fill the matrix row corresponding to the cycle closed by this dropped edge.
        for (uint a = 0; a < rspan->size_alpha; a++) {
            if (a != b) {
                fmpz_set_si(fmpz_mat_entry(MAT, j, a), rspan->span_forest[r][a] - rspan->span_forest[s][a]);
            }
            else {
                fmpz_set_si(fmpz_mat_entry(MAT, j, a), rspan->span_forest[r][a] - rspan->span_forest[s][a] + 1);
            }
        }
    }
    for (uint j = 0; j < size_fbase; j++) {

        // We take the dropped edge (r,b,s) from the queue.
        uint p = lefread_dequeue(lspan->dropped[lspan->numtree[f]], j);
        uint r = p / lspan->size_alpha;
        uint b = p % lspan->size_alpha;
        uint s = M->l_cayley->edges[r][b];

        // We fill the matrix row corresponding to the cycle closed by this dropped edge.
        for (uint a = 0; a < lspan->size_alpha; a++) {
            if (a != b) {
                fmpz_set_si(fmpz_mat_entry(MAT, j + size_ebase, a), lspan->span_forest[r][a] - lspan->span_forest[s][a]);
            }
            else {
                fmpz_set_si(fmpz_mat_entry(MAT, j + size_ebase, a), lspan->span_forest[r][a] - lspan->span_forest[s][a] + 1);
            }
        }
    }

    // We put the matrix in HNF.
    fmpz_mat_hnf(MAT, MAT);



    for (uint i = 0; i < M->rels->RCL->cl_size[M->rels->RCL->numcl[e]]; i++) {
        for (uint j = 0; j < M->rels->LCL->cl_size[M->rels->LCL->numcl[f]]; j++) {
            uint q = M->rels->RCL->cl_elems[M->rels->RCL->numcl[e]][i];
            uint t = M->rels->LCL->cl_elems[M->rels->LCL->numcl[f]][j];

            int target[rspan->size_alpha];
            for (uint a = 0; a < rspan->size_alpha; a++) {
                target[a] = rspan->span_forest[q][a] + lspan->span_forest[t][a];
            }

            if (solve_system_amt(MAT, target, MAT->r, MAT->c)) {
                rigins_dequeue(q, p1);
                rigins_dequeue(t, p2);
            }
        }
    }

    fmpz_mat_clear(MAT);
}



/****************************/
/*+ Dealing with morphisms +*/
/****************************/



typedef struct {
    uint left;
    uint right;
    uint size;

} stal_sent;


static void concat_stal_lists(uint* map, stal_sent** trans, uint asize, uint r, uint s) {
    for (uint a = 0; a < asize; a++) {
        if (trans[r][a].size == 0) {
            trans[r][a] = trans[s][a];
        }
        else if (trans[s][a].size == 0) {
            trans[s][a] = trans[r][a];
        }
        else {
            map[trans[r][a].right] = trans[s][a].left;
            trans[r][a].right = trans[s][a].right;
            trans[s][a].left = trans[r][a].left;
            trans[r][a].size += trans[s][a].size;
            trans[s][a].size = trans[r][a].size;
        }
    }
}


parti* dgraph_stal_fold(dgraph* g, parti* sccs, basis ba) {
    bool grp = false;
    switch (ba)
    {
    case BA_ST:
        return dtarjan(g, NULL, false);
        break;
    case BA_MOD:
        grp = false;
        break;
    case BA_AMT:
        return dgraph_amt_fold(g, sccs);
        break;
    case BA_GR:
        grp = true;
        break;
    default:
        fprintf(stderr, "Error: Invalid basis in dgraph_stal_fold.\n");
        exit(EXIT_FAILURE);
        break;
    }

    uint alpha_size;

    if (grp) {
        alpha_size = g->size_alpha;
    }
    else {
        alpha_size = 1;
    }

    // Allocation of the transition lists
    uint* map;
    uint* states;
    MALLOC(map, g->size_graph * g->size_alpha * 2);
    MALLOC(states, g->size_graph * g->size_alpha * 2);
    stal_sent** ltrans;
    MALLOC(ltrans, g->size_graph);
    stal_sent** litrans;
    MALLOC(litrans, g->size_graph);
    stal_sent* storage;
    CALLOC(storage, g->size_graph * alpha_size);
    stal_sent* istorage;
    CALLOC(istorage, g->size_graph * alpha_size);


    for (uint q = 0; q < g->size_graph; q++) {
        ltrans[q] = storage + q * alpha_size;
        litrans[q] = istorage + q * alpha_size;
        for (uint a = 0; a < alpha_size; a++) {
            ltrans[q][a].left = UINT_MAX;
            ltrans[q][a].right = UINT_MAX;
            ltrans[q][a].size = 0;
            litrans[q][a].left = UINT_MAX;
            litrans[q][a].right = UINT_MAX;
            litrans[q][a].size = 0;
        }
    }


    // Fill lists with original transitions (only transitions internal to sccs are kept).
    uint index = 0;
    for (uint q = 0; q < g->size_graph; q++) {
        for (uint a = 0; a < g->size_alpha; a++) {
            uint r = g->edges[q][a];
            if (r == UINT_MAX || (sccs && sccs->numcl[r] != sccs->numcl[q])) {
                continue; // Skip transitions not in the same SCC
            }
            uint b;
            if (grp) {
                b = a;
            }
            else {
                b = 0;
            }

            states[index] = r;
            map[index] = ltrans[q][b].left;
            ltrans[q][b].left = index;
            if (ltrans[q][b].right == UINT_MAX) {
                ltrans[q][b].right = index;
            }
            ltrans[q][b].size++;

            index++;

            states[index] = q;
            map[index] = litrans[r][b].left;
            litrans[r][b].left = index;
            if (litrans[r][b].right == UINT_MAX) {
                litrans[r][b].right = index;
            }
            litrans[r][b].size++;

            index++;
        }
    }

    // Union-find for the partition
    ufind* merge = create_ufind(g->size_graph);

    // Une file qui contient une liste de de sommets à traiter
    // Elle contient initialement tous les sommets
    dequeue* tofold = create_dequeue();
    for (uint q = 0; q < g->size_graph; q++) {
        rigins_dequeue(q, tofold);
    }


    // Tant qu'il reste un sommet à traiter
    while (!isempty_dequeue(tofold)) {
        // On prend le représentant d'un sommet non-traité
        uint q = find_ufind(lefpull_dequeue(tofold), merge);

        bool folded = false;
        uint a = 0;
        while (!folded && a < alpha_size) {

            if (ltrans[q][a].size > 1) // Si il y a 2 arêtes sortantes étiquetées par a
            {
                // On prend les représentant des destinations des deux premières
                // arêtes qu'on va fusionner (si ça n'a pas été fait avant)
                uint r = find_ufind(states[ltrans[q][a].left], merge);
                uint s = find_ufind(states[map[ltrans[q][a].left]], merge);

                ltrans[q][a].size--; // On supprime la première des deux edges
                if (ltrans[q][a].size == 0) {
                    ltrans[q][a].left = UINT_MAX;
                    ltrans[q][a].right = UINT_MAX;
                }
                else {
                    ltrans[q][a].left = map[ltrans[q][a].left];
                }
                if (r != s)                                                // Si les deux sommets adjacents n'avaient pas encore étés fusionnés
                {
                    union_ufind(r, s, merge);                       // On effectue la fusion
                    concat_stal_lists(map, ltrans, alpha_size, r, s); // On concatène leur listes de sommets adjacents
                    concat_stal_lists(map, litrans, alpha_size, r, s);
                    rigins_dequeue(find_ufind(r, merge), tofold); // On va devoir éventuelement traiter la nouvelle classe
                    if (find_ufind(r, merge) != find_ufind(q, merge)) {
                        rigins_dequeue(find_ufind(q, merge), tofold);
                    }
                    folded = true;
                }
                else {
                    rigins_dequeue(find_ufind(q, merge), tofold);
                }
            }
            else if (litrans[q][a].size > 1) // Si il y a 2 arêtes sortantes étiquetées par a-1
            {
                // On prend les représentant des destinations des deux premières
                // arêtes qu'on va fusionner (si ça n'a pas été fait avant)
                uint r = find_ufind(states[litrans[q][a].left], merge);
                uint s = find_ufind(states[map[litrans[q][a].left]], merge);
                litrans[q][a].size--; // On supprime la première des deux edges
                if (litrans[q][a].size == 0) {
                    litrans[q][a].left = UINT_MAX;
                    litrans[q][a].right = UINT_MAX;
                }
                else {
                    litrans[q][a].left = map[litrans[q][a].left];
                }
                if (r != s)                                                  // Si les deux sommets adjacents n'avaient pas encore étés fusionnés
                {
                    union_ufind(r, s, merge); // On effectue la fusion
                    concat_stal_lists(map, ltrans, alpha_size, r, s); // On concatène leur listes de sommets adjacents
                    concat_stal_lists(map, litrans, alpha_size, r, s);
                    rigins_dequeue(find_ufind(r, merge), tofold); // On va devoir éventuellement traiter la nouvelle classe
                    if (find_ufind(r, merge) != find_ufind(q, merge)) {
                        rigins_dequeue(find_ufind(q, merge), tofold);
                    }
                    folded = true;
                }
                else {
                    rigins_dequeue(find_ufind(q, merge), tofold);
                }
            }
            a++;
        }
    }

    // Cleanup
    free(map);
    free(states);
    free(ltrans);
    free(litrans);
    free(storage);
    free(istorage);
    delete_dequeue(tofold);
    parti* result = ufind_to_parti(merge);
    delete_ufind(merge);

    return result;

}


dgraph* shrink_mod(dgraph* g, parti* fold, parti* sccs) {
    dgraph* new = create_dgraph_noedges(fold->size_par, 2);
    for (uint c = 0; c < fold->size_par; c++) {
        for (uint a = 0; a < 2; a++) {
            new->edges[c][a] = UINT_MAX;
        }
    }
    for (uint q = 0; q < g->size_graph;q++) {
        uint c = fold->numcl[q];
        for (uint a = 0; a < g->size_alpha; a++) {
            uint r = g->edges[q][a];
            if (r != UINT_MAX && sccs->numcl[r] == sccs->numcl[q]) {
                new->edges[c][0] = fold->numcl[r];
                new->edges[fold->numcl[r]][1] = c;
            }
        }
    }
    return new;
}

dgraph* shrink_mod_mirror(dgraph* g, parti* fold, parti* sccs) {
    dgraph* new = create_dgraph_noedges(fold->size_par, 2);
    for (uint c = 0; c < fold->size_par; c++) {
        for (uint a = 0; a < 2; a++) {
            new->edges[c][a] = UINT_MAX;
        }
    }
    for (uint q = 0; q < g->size_graph;q++) {
        uint c = fold->numcl[q];
        for (uint a = 0; a < g->size_alpha; a++) {
            uint r = g->edges[q][a];
            if (r != UINT_MAX && sccs->numcl[r] == sccs->numcl[q]) {
                new->edges[c][1] = fold->numcl[r];
                new->edges[fold->numcl[r]][0] = c;
            }
        }
    }
    return new;
}

dgraph* shrink_grp(dgraph* g, parti* fold, parti* sccs) {
    dgraph* new = create_dgraph_noedges(fold->size_par, 2 * g->size_alpha);
    for (uint c = 0; c < fold->size_par; c++) {
        for (uint a = 0; a < new->size_alpha; a++) {
            new->edges[c][a] = UINT_MAX;
        }
    }
    for (uint q = 0; q < g->size_graph;q++) {
        uint c = fold->numcl[q];
        for (uint a = 0; a < g->size_alpha; a++) {
            uint r = g->edges[q][a];
            if (r != UINT_MAX && (!sccs || sccs->numcl[r] == sccs->numcl[q])) {
                new->edges[c][a] = fold->numcl[r];
                new->edges[fold->numcl[r]][g->size_alpha + a] = c;
            }
        }
    }
    return new;
}

dgraph* shrink_grp_mirror(dgraph* g, parti* fold, parti* sccs) {
    dgraph* new = create_dgraph_noedges(fold->size_par, 2 * g->size_alpha);
    for (uint c = 0; c < fold->size_par; c++) {
        for (uint a = 0; a < new->size_alpha; a++) {
            new->edges[c][a] = UINT_MAX;
        }
    }
    for (uint q = 0; q < g->size_graph;q++) {
        uint c = fold->numcl[q];
        for (uint a = 0; a < g->size_alpha; a++) {
            uint r = g->edges[q][a];
            if (r != UINT_MAX && sccs->numcl[r] == sccs->numcl[q]) {
                new->edges[c][g->size_alpha + a] = fold->numcl[r];
                new->edges[fold->numcl[r]][a] = c;
            }
        }
    }
    return new;
}





dgraph* dgraph_implement_fold(dgraph* g, parti* sccs, parti* fold) {

    dgraph* g_fold = create_dgraph_noedges(fold->size_par, g->size_alpha);

    // Initialize the edges of the folded graph to UINT_MAX
    for (uint i = 0; i < fold->size_par; i++) {
        for (uint a = 0; a < g->size_alpha; a++) {
            g_fold->edges[i][a] = UINT_MAX;
        }
    }

    // For each state in the original graph, copy the edges to the folded graph
    for (uint i = 0; i < g->size_graph;i++) {
        for (uint a = 0; a < g->size_alpha; a++) {
            if (g->edges[i][a] != UINT_MAX && (!sccs || sccs->numcl[g->edges[i][a]] == sccs->numcl[i])) {
                g_fold->edges[fold->numcl[i]][a] = fold->numcl[g->edges[i][a]];
            }
        }
    }
    return g_fold;

}


dfa* dfa_compute_folding(dfa* A, basis ba) {
    // If the basis is not a group basis, we compute the partition of the states.
    parti* sccs = dtarjan(A->trans, NULL, false);
    parti* fold = dgraph_stal_fold(A->trans, sccs, ba);

    dfa* ret;
    MALLOC(ret, 1);
    ret->trans = dgraph_implement_fold(A->trans, sccs, fold);
    ret->initial = fold->numcl[A->initial];

    bool* final;
    CALLOC(final, fold->size_par);


    for (uint i = 0; i < A->nb_finals; i++) {
        uint n = fold->numcl[A->finals[i]];
        final[n] = true; // We mark the state as final in the folded DFA.
    }

    ret->nb_finals = 0;

    for (uint i = 0; i < fold->size_par; i++) {
        if (final[i]) {
            ret->nb_finals++;
        }
    }

    MALLOC(ret->finals, ret->nb_finals);

    ret->nb_finals = 0;
    for (uint i = 0; i < fold->size_par; i++) {
        if (final[i]) {
            ret->finals[ret->nb_finals] = i; // We add the final state to the folded DFA.
            ret->nb_finals++;
        }
    }

    ret->order = NULL;
    ret->alphabet = duplicate_alphabet(A->alphabet, A->trans->size_alpha);
    MALLOC(ret->state_names, fold->size_par);
    for (uint q = 0; q < fold->size_par; q++) {
        uint stringsize = 0;
        for (uint i = 0; i < fold->cl_size[q]; i++) {
            uint r = fold->cl_elems[q][i];
            if (A->state_names) {
                stringsize += strlen(A->state_names[r]) + 1;
            }
            else {
                stringsize += get_uint_length(r) + 1;
            }
        }
        MALLOC(ret->state_names[q], stringsize);
        bool first = true;
        char aux[64];
        for (uint i = 0; i < fold->cl_size[q]; i++) {
            if (first) {
                first = false;
            }
            else {
                strcat(ret->state_names[q], ",");
            }

            if (A->state_names) {
                strcat(ret->state_names[q], A->state_names[fold->cl_elems[q][i]]);
            }
            else {
                sprintf(aux, "%d", fold->cl_elems[q][i]);
                strcat(ret->state_names[q], aux);
            }
        }
    }

    delete_parti(sccs);
    delete_parti(fold);
    return ret;

}




/***********************/
/*+ Inverse extension +*/
/***********************/

parti* nfa_inv_ext(nfa* A) {
    // If there are inverse transitions, we delete them.
    if (A->trans_i != NULL) {
        delete_lgraph(A->trans_i);
        A->trans_i = NULL;
    }

    // If there are epsilon transitions, we delete them.
    if (A->trans_e != NULL) {
        nfa_elimeps_mod(A);
    }

    // Calculating strongly connected components.
    parti* P = ltarjan(A->trans, NULL);

    // We can now compute the reverse transitions.
    A->trans_i = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);

    // For transition (q,a,r)
    for (uint q = 0; q < A->trans->size_graph; q++) {
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(A->trans->edges[q][a]); i++) {
                uint r = lefread_dequeue(A->trans->edges[q][a], i);
                // If q and r are in the same SCC, we add the transition (i,a^{-1},q)
                if (P->numcl[q] == P->numcl[r]) {
                    insert_dequeue(A->trans_i->edges[r][a], q);
                }
            }
        }
    }
    // We return the partition in SCCS
    return P;
}

void nfa_remove_inv(nfa* A) {
    if (A->trans_i != NULL) {
        delete_lgraph(A->trans_i);
        A->trans_i = NULL;
    }
}

/***********************************/
/*+ Separation by group languages +*/
/***********************************/



parti* nfa_stal_fold(nfa* A, parti* sccs) {
    if (A->trans_e != NULL) {
        fprintf(stderr, "Stallings folding only works for NFAs without epsilon transitions. Returned NULL.\n");
        return NULL;
    }
    if (A->trans_i == NULL) {
        fprintf(stderr, "Stallings folding only works for NFAs whose inverse transitions have already been computed. Returned NULL.\n");
        return NULL;
    }

    // Union-find for the partition
    ufind* merge = create_ufind(A->trans->size_graph);

    // Compute the number of transitions.
    uint nb_trans = 0;
    for (uint q = 0; q < A->trans_i->size_graph; q++) {
        for (uint a = 0; a < A->trans_i->size_alpha; a++) {
            {
                nb_trans += size_dequeue(A->trans_i->edges[q][a]);
            }
        }
    }

    // Allocation of the transition lists
    uint* map;
    uint* states;
    MALLOC(map, nb_trans * 2);
    MALLOC(states, nb_trans * 2);
    stal_sent** ltrans;
    MALLOC(ltrans, A->trans->size_graph);
    stal_sent** litrans;
    MALLOC(litrans, A->trans->size_graph);
    stal_sent* storage;
    CALLOC(storage, A->trans->size_graph * A->trans->size_alpha);
    stal_sent* istorage;
    CALLOC(istorage, A->trans->size_graph * A->trans->size_alpha);


    // Fill lists with original transitions (only transitions internal to sccs are kept).
    uint index = 0;
    for (uint q = 0; q < A->trans->size_graph; q++) {
        ltrans[q] = storage + q * A->trans->size_alpha;
        litrans[q] = istorage + q * A->trans->size_alpha;
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            uint j = index;
            for (uint i = 0; i < size_dequeue(A->trans->edges[q][a]); i++) {
                uint r = lefread_dequeue(A->trans->edges[q][a], i);
                if (sccs->numcl[r] == sccs->numcl[q]) {
                    states[j] = r;
                    map[j] = j + 1;
                    j++;
                    ltrans[q][a].size++;
                }
            }
            if (j != index) {
                ltrans[q][a].left = index;
                ltrans[q][a].right = j - 1;
                map[j - 1] = UINT_MAX;
                index = j;
            }
            else {
                ltrans[q][a].left = UINT_MAX;
                ltrans[q][a].right = UINT_MAX;
            }
            for (uint i = 0; i < size_dequeue(A->trans_i->edges[q][a]); i++) {
                uint r = lefread_dequeue(A->trans_i->edges[q][a], i);
                states[j] = r;
                map[j] = j + 1;
                j++;
                litrans[q][a].size++;
            }
            if (j != index) {
                litrans[q][a].left = index;
                litrans[q][a].right = j - 1;
                map[j - 1] = UINT_MAX;
                index = j;
            }
            else {
                litrans[q][a].left = UINT_MAX;
                litrans[q][a].right = UINT_MAX;
            }
        }
    }


    // Une file qui contient une liste de de sommets à traiter
    // Elle contient initialement tous les sommets
    dequeue* tofold = create_dequeue();
    for (uint q = 0; q < A->trans->size_graph; q++) {
        rigins_dequeue(q, tofold);
    }

    // Tant qu'il reste un sommet à traiter
    while (!isempty_dequeue(tofold)) {
        // On prend le représentant d'un sommet non-traité
        uint q = find_ufind(lefpull_dequeue(tofold), merge);

        bool folded = false;
        uint a = 0;
        while (!folded && a < A->trans->size_alpha) {
            if (ltrans[q][a].size > 1) // Si il y a 2 arêtes sortantes étiquetées par a
            {
                // On prend les représentant des destinations des deux premières
                // arêtes qu'on va fusionner (si ça n'a pas été fait avant)
                uint r = find_ufind(states[ltrans[q][a].left], merge);
                uint s = find_ufind(states[map[ltrans[q][a].left]], merge);
                ltrans[q][a].size--; // On supprime la première des deux edges
                if (ltrans[q][a].size == 0) {
                    ltrans[q][a].left = UINT_MAX;
                    ltrans[q][a].right = UINT_MAX;
                }
                else {
                    ltrans[q][a].left = map[ltrans[q][a].left];
                }
                if (r != s)                                                // Si les deux sommets adjacents n'avaient pas encore étés fusionnés
                {
                    union_ufind(r, s, merge);                       // On effectue la fusion
                    concat_stal_lists(map, ltrans, A->trans->size_alpha, r, s); // On concatène leur listes de sommets adjacents
                    concat_stal_lists(map, litrans, A->trans->size_alpha, r, s);
                    rigins_dequeue(find_ufind(r, merge), tofold); // On va devoir éventuelement traiter la nouvelle classe
                    if (find_ufind(r, merge) != find_ufind(q, merge)) {
                        rigins_dequeue(find_ufind(q, merge), tofold);
                    }
                    folded = true;
                }
                else {
                    rigins_dequeue(find_ufind(q, merge), tofold);
                }
            }
            else if (litrans[q][a].size > 1) // Si il y a 2 arêtes sortantes étiquetées par a-1
            {
                // On prend les représentant des destinations des deux premières
                // arêtes qu'on va fusionner (si ça n'a pas été fait avant)
                uint r = find_ufind(states[litrans[q][a].left], merge);
                uint s = find_ufind(states[map[litrans[q][a].left]], merge);
                litrans[q][a].size--; // On supprime la première des deux edges
                if (litrans[q][a].size == 0) {
                    litrans[q][a].left = UINT_MAX;
                    litrans[q][a].right = UINT_MAX;
                }
                else {
                    litrans[q][a].left = map[litrans[q][a].left];
                }
                if (r != s)                                                  // Si les deux sommets adjacents n'avaient pas encore étés fusionnés
                {
                    union_ufind(r, s, merge); // On effectue la fusion
                    concat_stal_lists(map, ltrans, A->trans->size_alpha, r, s); // On concatène leur listes de sommets adjacents
                    concat_stal_lists(map, litrans, A->trans->size_alpha, r, s);
                    rigins_dequeue(find_ufind(r, merge), tofold); // On va devoir éventuellement traiter la nouvelle classe
                    if (find_ufind(r, merge) != find_ufind(q, merge)) {
                        rigins_dequeue(find_ufind(q, merge), tofold);
                    }
                    folded = true;
                }
                else {
                    rigins_dequeue(find_ufind(q, merge), tofold);
                }
            }
            a++;
        }
    }


    // Cleanup
    free(map);
    free(states);
    free(ltrans);
    free(litrans);
    free(storage);
    delete_dequeue(tofold);
    parti* result = ufind_to_parti_refined(merge, sccs);
    delete_ufind(merge);

    return result;
}


// Storage of the pairs for the Dyck construction.
static uint dyck_size = 0; // Full size of the dyck_eps array.
static uint dyck_elem = 0; // Number of tuples stored in the dyck_eps array.
static uint dyck_auto = 0; // Number of states in the NFA.
static uint* dyck_eps = NULL; // Array that stores the epsilon transitions (a single one uses 2 cells). Size: dyck_size * 2.

// Initialize the arrays used in the product construction.
static void dyck_init(uint size, uint states) {
    size = max(size, 2); // Ensures that the size is at least 2.
    dyck_size = size;
    dyck_elem = 0;
    dyck_auto = states;
    MALLOC(dyck_eps, dyck_size << 1);
}

// Deletes the arrays used in the product construction and resets the variables.
static void dyck_delete() {
    free(dyck_eps);
    dyck_eps = NULL;
    dyck_size = 0;
    dyck_elem = 0;
    dyck_auto = 0;
}

// Doubles the size of the arrays used in the subset construction if necessary.
static void dyck_grow() {
    if (dyck_size <= dyck_elem) {
        dyck_size <<= 1;
        REALLOC(dyck_eps, dyck_size << 1);
    }
}

// Checks if two pairs stored in the dyck_eps array are equal.
static bool dyck_equal(uint i, uint j) {
    uint zi = i << 1; // The index of the first element.
    uint zj = j << 1; // The index of the second element.
    return (dyck_eps[zi] == dyck_eps[zj]) && (dyck_eps[zi + 1] == dyck_eps[zj + 1]);
}

// Hash function for the pairs stored in the dyck_eps array.
static uint dyck_hash(uint i, uint size_hash) {
    uint e = i << 1; // The index of the element in the array.
    uint hash = 0;

    uint a = 0x9e3779b9; // fractional bits of the golden ratio


    hash = dyck_eps[e] * a % size_hash; // Hashing the first element of the pair
    hash = (hash * (dyck_auto + 1) + dyck_eps[e + 1] * a) % size_hash; // Hashing the second element of the pair
    return hash;
}



// Calcul du NFA obtenu en effectuant le folding et en ajoutant ensuite
// des transitions espilon entre les paires d'états liées par un mot de
// Dyck.
// Les transitions inverses doivent déjà avoir été calculées et on doit
// fournir les partitions en SCCS et celle correspondant au folding.
nfa* nfa_dyck_ext(nfa* A, parti* FOLD) {
    if (!FOLD) {
        return NULL;
    }
    if (A->trans_e != NULL) {
        fprintf(stderr, "Adding Dyck transitions only works for NFAs without epsilon transitions. Returned NULL.\n");
        return NULL;
    }
    if (A->trans_i == NULL) {
        fprintf(stderr, "Inverse transitions must have already been computed. Returned NULL.\n");
        return NULL;
    }
    nfa* B = nfa_merge_states(A, FOLD);
    nfa* Bm = nfa_mirror(B);

    graph* eps = create_graph_noedges(B->trans->size_graph);
    graph* epsm = create_graph_noedges(B->trans->size_graph);

    dequeue* in = create_dequeue();
    dequeue* out = create_dequeue();
    for (uint i = 0; i < B->trans->size_graph; i++) {
        rigins_dequeue(i, in);
        rigins_dequeue(i, out);
    }

    uchar power = get_uint_lbinary(B->trans->size_graph) + 2; // We compute the initial power of two for the size of the hash table.
    uint thesize = 1U << power; // The size of the hash table is 2^power.

    dyck_init(thesize, B->trans->size_graph); // Initialize the arrays used in the product construction.

    // Initialize the hash table.
    hash_table* thehash = create_hash_table(power, &dyck_hash, &dyck_equal);




    while (!isempty_dequeue(in)) {
        uint q = lefpull_dequeue(in);
        uint r = lefpull_dequeue(out);
        // printf("%d %d\n", q, r);

        dyck_eps[dyck_elem << 1] = q; // Store the first element of the pair.
        dyck_eps[(dyck_elem << 1) + 1] = r; // Store the second element of the pair.
        if (dyck_elem != hash_table_insert(thehash, dyck_elem)) {
            continue;
        }
        else {
            // If the pair is already in the hash table, we skip it.
            dyck_elem++;
            dyck_grow(); // Ensure that the arrays are large enough.
        }


        insert_dequeue(eps->edges[q], r);
        insert_dequeue(epsm->edges[r], q);

        // aa-1 et a-1 a
        for (uint a = 0; a < B->trans->size_alpha; a++) {
            for (uint i = 0; i < size_dequeue(Bm->trans->edges[q][a]); i++) {
                for (uint j = 0; j < size_dequeue(B->trans_i->edges[r][a]); j++) {

                    rigins_dequeue(lefread_dequeue(Bm->trans->edges[q][a], i), in);
                    rigins_dequeue(lefread_dequeue(B->trans_i->edges[r][a], j), out);
                }
            }

            for (uint i = 0; i < size_dequeue(Bm->trans_i->edges[q][a]); i++) {
                for (uint j = 0; j < size_dequeue(B->trans->edges[r][a]); j++) {
                    rigins_dequeue(lefread_dequeue(Bm->trans_i->edges[q][a], i), in);
                    rigins_dequeue(lefread_dequeue(B->trans->edges[r][a], j), out);
                }
            }
        }

        // transclos
        for (uint i = 0; i < size_dequeue(eps->edges[r]); i++) {
            rigins_dequeue(q, in);
            rigins_dequeue(lefread_dequeue(eps->edges[r], i), out);
        }

        for (uint i = 0; i < size_dequeue(epsm->edges[q]); i++) {
            rigins_dequeue(lefread_dequeue(epsm->edges[q], i), in);
            rigins_dequeue(r, out);
        }
    }
    delete_dequeue(in);
    delete_dequeue(out);
    delete_graph(epsm);
    delete_hash_table(thehash);
    dyck_delete(); // Delete the arrays .






    B->trans_e = eps;

    return B;
}

// Procédure de séparation complète
bool decid_grp_sep(nfa* I1, nfa* I2, bool details, FILE* out) {
    // Gestion des cas triviaux
    if (isempty_dequeue(I1->finals)) {
        if (out && details) {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(I2->finals)) {
        if (out && details) {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (I1 == I2) {
        if (out && details) {
            fprintf(out, "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }
    if (out) {
        fprintf(out, "#### Using the GR-separation algorithm based on inverse extension.\n\n");
    }

    // Phase 1: Calcul des transitions inverses
    parti* SCCS1 = nfa_inv_ext(I1);
    parti* SCCS2 = nfa_inv_ext(I2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 1: adding the inverse transitions inside the strongly connected components.\n");
        fprintf(out, "#### NFA of the first input language extended with inverse transitions.\n");
        view_nfa(I1);
        fprintf(out, "#### NFA of the second input language extended with inverse transitions.\n");
        view_nfa(I2);
    }

    // Phase 2: calcul des transitions induites par les mots de Dyck puis
    // éliminations de celles-ci
    parti* FOLD1 = nfa_stal_fold(I1, SCCS1);
    delete_parti(SCCS1);
    nfa* A1 = nfa_dyck_ext(I1, FOLD1);
    delete_parti(FOLD1);
    nfa_remove_inv(I1);
    nfa_elimeps_mod(A1);
    parti* FOLD2 = nfa_stal_fold(I2, SCCS2);
    delete_parti(SCCS2);
    nfa* A2 = nfa_dyck_ext(I2, FOLD2);
    delete_parti(FOLD2);
    nfa_remove_inv(I2);
    nfa_elimeps_mod(A2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "\n****************** Phase 2: Computing the epsilon transitions induced by Dyck words and eliminating them.\n");
        fprintf(out, "#### Construction on the NFA of the first input language.\n");
        view_nfa(A1);
        fprintf(out, "#### Construction on the NFA of the second input language.\n");
        view_nfa(A2);
    }

    // Phase 3: Calcul de l'intersection

    nfa* INTERSECT = nfa_intersect(A1, A2, true);
    nfa_delete(A1);
    nfa_delete(A2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "\n****************** Phase 3: Computing the intersection of the resulting NFAs.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_dequeue(INTERSECT->finals)) {
        if (out && details) {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        nfa_delete(INTERSECT);
        return true;
    }
    else {
        if (out && details) {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        nfa_delete(INTERSECT);
        return false;
    }
}

/*******************************************/
/*+ Séparation par les langages de modulo +*/
/*******************************************/

nfa* nfa_proj_unary(nfa* A) {
    if (A->trans_i != NULL) {
        fprintf(stderr, "Cannot project a NFA with inverse transitions. Returned NULL.\n");
        return NULL;
    }
    // Élimination des éventuelles transitions epsilon
    nfa_elimeps_mod(A);

    // Création du nouveau NFA
    nfa* B;
    CALLOC(B, 1);
    B->initials = create_dequeue();
    B->finals = create_dequeue();

    copy_dequeue_right(B->initials, A->initials, 0);
    copy_dequeue_right(B->finals, A->finals, 0);
    B->trans = create_lgraph_noedges(A->trans->size_graph, 1);

    B->state_names = copy_all_names(A->state_names, A->trans->size_graph);

    MALLOC(B->alphabet, 1);
    B->alphabet[0].lab = '$';
    B->alphabet[0].num = -1;

    // Création des transitions
    for (uint q = 0; q < B->trans->size_graph; q++) {
        dequeue* array[A->trans->size_alpha];
        for (uint a = 0; a < A->trans->size_alpha; a++) {
            array[a] = A->trans->edges[q][a];
        }
        merge_array_sorted_dequeue(B->trans->edges[q][0], array, A->trans->size_alpha);
    }
    return B;
}

bool decid_mod_sep(nfa* I1, nfa* I2, bool details, FILE* out) {
    // Gestion des cas triviaux
    if (isempty_dequeue(I1->finals)) {
        if (out && details) {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(I2->finals)) {
        if (out && details) {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (I1 == I2) {
        if (out && details) {
            fprintf(out, "#### These two inputs refer to the same nonempty language.\n");
        }
        return false;
    }
    if (out) {
        fprintf(out, "#### Using the reduction to GR-separation (over a unary alphabet).\n\n");
    }

    nfa_remove_inv(I1);
    nfa_remove_inv(I2);

    // Projection sur un alphabet unaire
    nfa* U1 = nfa_proj_unary(I1);
    nfa* U2 = nfa_proj_unary(I2);

    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "#### Reduction to GR-separation: projection of the two inputs on a unary alphabet.\n");
        fprintf(out, "#### Projection of the first input language:\n");
        view_nfa(U1);
        fprintf(out, "#### Projection of the second input language:\n");
        view_nfa(U2);
        fprintf(out, "#### Now applying the GR-separation algorithm based on inverse extension to the resulting languages.\n\n");
    }

    // Phase 1: Calcul des transitions inverses
    parti* SCCS1 = nfa_inv_ext(U1);
    parti* SCCS2 = nfa_inv_ext(U2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "#### Phase 1: adding the inverse transitions inside the strongly connected components.\n");
        fprintf(out, "#### NFA of the first input language extended with inverse transitions.\n");
        view_nfa(U1);
        fprintf(out, "#### NFA of the second input language extended with inverse transitions.\n");
        view_nfa(U2);
    }

    // Phase 2: calcul des transitions induites par les mots de Dyck puis
    // éliminations de celles-ci
    parti* FOLD1 = nfa_stal_fold(U1, SCCS1);
    delete_parti(SCCS1);
    nfa* A1 = nfa_dyck_ext(U1, FOLD1);
    delete_parti(FOLD1);
    nfa_delete(U1);
    nfa_elimeps_mod(A1);
    parti* FOLD2 = nfa_stal_fold(U2, SCCS2);
    delete_parti(SCCS2);
    nfa* A2 = nfa_dyck_ext(U2, FOLD2);
    delete_parti(FOLD2);
    nfa_delete(U2);
    nfa_elimeps_mod(A2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "\n****************** Phase 2: Computing the epsilon transitions induced by Dyck words and eliminating them.\n");
        fprintf(out, "#### Construction on the NFA of the first input language.\n");
        view_nfa(A1);
        fprintf(out, "#### Construction on the NFA of the second input language.\n");
        view_nfa(A2);
    }

    // Phase 3: Calcul de l'intersection

    nfa* INTERSECT = nfa_intersect(A1, A2, true);
    nfa_delete(A1);
    nfa_delete(A2);
    if (out && details) {
        print_sep_line(100, out);
        fprintf(out, "\n****************** Phase 3: Computing the intersection of the resulting NFAs.\n");
        view_nfa(INTERSECT);
    }
    if (isempty_dequeue(INTERSECT->finals)) {
        if (out && details) {
            fprintf(out, "#### This NFA recognizes the empty language\n");
        }
        nfa_delete(INTERSECT);
        return true;
    }
    else {
        if (out && details) {
            fprintf(out, "#### This NFA recognizes a nonempty language\n");
        }

        nfa_delete(INTERSECT);
        return false;
    }
}

/******************************************/
/*+ Séparation par les langages triviaux +*/
/******************************************/

bool decid_st_sep(nfa* I1, nfa* I2, FILE* out) {
    if (out) {
        fprintf(out, "#### The input languages are ST-separable if and only if one of them is empty.\n\n");
    }
    // Gestion des cas triviaux
    if (isempty_dequeue(I1->finals)) {
        if (out) {
            fprintf(out, "#### The first input language is empty.\n");
        }
        return true;
    }
    if (isempty_dequeue(I2->finals)) {
        if (out) {
            fprintf(out, "#### The second input language is empty.\n");
        }
        return true;
    }
    if (out) {
        fprintf(out, "#### Both inputs languages are nonempty.\n");
    }
    return false;
}
