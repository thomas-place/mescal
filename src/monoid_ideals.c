/*********************/
/* Ideals of monoids */
/*********************/

#include "monoid_ideals.h"

/**************************/
/* Computations of ideals */
/**************************/


dequeue* compute_r_ideal(morphism* M, uint s, bool* rest) {
    dequeue* ini = create_dequeue();
    rigins_dequeue(s, ini);
    dequeue* res = dgraph_search(DFS, M->r_cayley, ini, NULL, rest);
    delete_dequeue(ini);
    return res;
}


dequeue* compute_l_ideal(morphism* M, uint s, bool* rest) {
    dequeue* ini = create_dequeue();
    rigins_dequeue(s, ini);
    dequeue* res = dgraph_search(DFS, M->l_cayley, ini, NULL, rest);
    delete_dequeue(ini);
    return res;
}

dequeue* compute_j_ideal(morphism* M, uint s, bool* rest) {
    dequeue* ini = create_dequeue();
    rigins_dequeue(s, ini);
    dequeue* res = lgraph_search(DFS, M->j_order, ini, NULL, rest);
    delete_dequeue(ini);
    return res;
}

dequeue* compute_r_ideal_alph(morphism* M, uint s, bool* alph, bool* rest) {
    dequeue* ini = create_dequeue();
    rigins_dequeue(s, ini);
    dequeue* res = dgraph_search(DFS, M->r_cayley, ini, alph, rest);
    delete_dequeue(ini);
    return res;
}


dequeue* compute_l_ideal_alph(morphism* M, uint s, bool* alph, bool* rest) {
    dequeue* ini = create_dequeue();
    rigins_dequeue(s, ini);
    dequeue* res = dgraph_search(DFS, M->l_cayley, ini, alph, rest);
    delete_dequeue(ini);
    return res;
}

dequeue* compute_j_ideal_alph(morphism* M, uint s, bool* alph, bool* rest) {
    dequeue* ini = create_dequeue();
    rigins_dequeue(s, ini);
    dequeue* res = lgraph_search(DFS, M->j_order, ini, alph, rest);
    delete_dequeue(ini);
    return res;
}



uint get_rlink(morphism* M, parti* R, uint g, uint r) {
    dequeue* queue = create_dequeue();
    dequeue* elems = create_dequeue();
    rigins_dequeue(r, queue);
    rigins_dequeue(ONE, elems);
    bool* visited;
    CALLOC(visited, M->r_cayley->size_graph);

    while (!isempty_dequeue(queue)) {
        uint rs = lefpull_dequeue(queue);
        uint s = lefpull_dequeue(elems);
        if (rs == g) {
            delete_dequeue(queue);
            delete_dequeue(elems);
            free(visited);
            return mor_mult(M, s, g);
        }
        if (visited[rs]) {
            continue;
        }
        visited[rs] = true;
        for (uint a = 0; a < M->r_cayley->size_alpha; a++) {
            if (R->cl[g] == R->cl[M->r_cayley->edges[rs][a]]) {
                rigins_dequeue(M->r_cayley->edges[rs][a], queue);
                rigins_dequeue(M->r_cayley->edges[s][a], elems);
            }
        }
    }
    delete_dequeue(queue);
    delete_dequeue(elems);
    free(visited);
    return ONE;
}


uint get_llink(morphism* M, parti* L, uint h, uint t) {
    dequeue* queue = create_dequeue();
    dequeue* elems = create_dequeue();
    rigins_dequeue(t, queue);
    rigins_dequeue(ONE, elems);
    bool* visited;
    CALLOC(visited, M->r_cayley->size_graph);

    while (!isempty_dequeue(queue)) {
        uint st = lefpull_dequeue(queue);
        uint s = lefpull_dequeue(elems);
        if (st == h) {
            delete_dequeue(queue);
            delete_dequeue(elems);
            free(visited);
            return mor_mult(M, h, s);
        }
        if (visited[st]) {
            continue;
        }
        visited[st] = true;
        for (uint a = 0; a < M->l_cayley->size_alpha; a++) {
            if (L->cl[h] == L->cl[M->l_cayley->edges[st][a]]) {
                rigins_dequeue(M->l_cayley->edges[st][a], queue);
                rigins_dequeue(M->l_cayley->edges[s][a], elems);
            }
        }
    }
    delete_dequeue(queue);
    delete_dequeue(elems);
    free(visited);
    return ONE;
}
