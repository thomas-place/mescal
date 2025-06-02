/********************************************/
/*   Structures pour stocker les machines   */
/* Ensuite spécialisées pour NFA,DFA,morphism */
/********************************************/

#include "graphs.h"
#include <limits.h>

/*****************************/
/*+ Création et suppression +*/
/*****************************/

graph* create_graph_noedges(uint size) {
    // Création du graphe
    graph* new;
    MALLOC(new, 1);

    // Définition du graph
    new->size = size;
    if (size != 0) {
        MALLOC(new->edges, size);
        for (uint q = 0; q < size; q++) {
            new->edges[q] = create_dequeue();
        }
    }
    else {
        new->edges = NULL;
    }
    return new;
}

graph* create_graph_selfloops(uint size) {
    graph* new = create_graph_noedges(size);
    for (uint q = 0; q < size; q++) {
        rigins_dequeue(q, new->edges[q]);
    }
    return new;
}

void delete_graph(graph* G) {
    if (G == NULL || G->edges == NULL) {
        free(G);
        return;
    }
    for (uint q = 0; q < G->size; q++) {
        delete_dequeue(G->edges[q]);
    }
    free(G->edges);
    free(G);
}

lgraph* create_lgraph_noedges(uint size_graph, uint size_alpha) {
    // Création du graphe
    lgraph* new;
    MALLOC(new, 1);

    // Définition du graphe
    new->size_alpha = size_alpha;
    new->size_graph = size_graph;
    if (size_graph != 0 && size_alpha != 0) {
        MALLOC(new->edges, size_graph);
        for (uint q = 0; q < size_graph; q++) {
            MALLOC(new->edges[q], size_alpha);
            for (uint a = 0; a < size_alpha; a++) {
                new->edges[q][a] = create_dequeue();
            }
        }
    }
    else {
        new->edges = NULL;
    }
    return new;
}

void delete_lgraph(lgraph* G) {
    if (G == NULL || G->edges == NULL) {
        free(G);
        return;
    }
    for (uint q = 0; q < G->size_graph; q++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            delete_dequeue(G->edges[q][a]);
        }
        free(G->edges[q]);
    }
    free(G->edges);
    free(G);
}

dgraph* create_dgraph_noedges(uint size_graph, uint size_alpha) {

    // Création du graphe
    dgraph* new;
    CALLOC(new, 1);

    // Définition du graphe
    new->size_alpha = size_alpha;
    new->size_graph = size_graph;
    if (size_graph != 0 && size_alpha != 0) {
        MALLOC(new->edges, size_graph);
        CALLOC(new->storage, size_graph * size_alpha);
        for (uint q = 0; q < size_graph; q++) {
            new->edges[q] = new->storage + q * size_alpha;
        }
    }
    else {
        new->edges = NULL;
    }
    return new;
}

void delete_dgraph(dgraph* G) {
    if (G == NULL) {
        return;
    }
    free(G->storage);
    free(G->edges);
    free(G);
}

/*******************/
/* Basic functions */
/*******************/

int graph_nb_edges(graph* G) {
    int res = 0;
    for (uint q = 0; q < G->size; q++) {
        res += size_dequeue(G->edges[q]);
    }
    return res;
}

int lgraph_nb_edges(lgraph* G) {
    int res = 0;
    for (uint q = 0; q < G->size_graph; q++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            res += size_dequeue(G->edges[q][a]);
        }
    }
    return res;
}

int dgraph_nb_edges(dgraph* G) { return G->size_graph * G->size_alpha; }

/***********/
/* Mirrors */
/***********/

graph* graph_mirror(graph* G) {
    if (!G) {
        return NULL;
    }

    graph* new = create_graph_noedges(G->size);
    for (uint q = 0; q < G->size; q++) {
        for (uint j = 0; j < size_dequeue(G->edges[q]); j++) {
            rigins_dequeue(q, new->edges[lefread_dequeue(G->edges[q], j)]);
        }
    }
    return new;
}

lgraph* lgraph_mirror(lgraph* G) {
    if (!G) {
        return NULL;
    }
    lgraph* new = create_lgraph_noedges(G->size_graph, G->size_alpha);
    for (uint q = 0; q < G->size_graph; q++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            for (uint j = 0; j < size_dequeue(G->edges[q][a]); j++) {
                rigins_dequeue(q, new->edges[lefread_dequeue(G->edges[q][a], j)][a]);
            }
        }
    }
    return new;
}

lgraph* dgraph_mirror(dgraph* G) {
    if (!G) {
        return NULL;
    }
    lgraph* new = create_lgraph_noedges(G->size_graph, G->size_alpha);
    for (uint q = 0; q < G->size_graph; q++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            rigins_dequeue(q, new->edges[G->edges[q][a]][a]);
        }
    }
    return new;
}

/************/
/* Parcours */
/************/

void graph_search_update(graph_stype T, graph* G, dequeue* ini, bool* visited) {
    uint(*f)(dequeue*);
    if (T == DFS) {
        f = rigpull_dequeue;
    }
    else {
        f = lefpull_dequeue;
    }
    while (!isempty_dequeue(ini)) {
        uint q = f(ini);
        if (visited[q]) {
            continue;
        }
        visited[q] = true;

        for (uint j = 0; j < size_dequeue(G->edges[q]); j++) {
            rigins_dequeue(lefread_dequeue(G->edges[q], j), ini);
        }
    }
}

dequeue* graph_search(graph_stype T, graph* G, dequeue* ini, bool* rest) {
    // Tableau pour mémoriser les sommets visités
    bool* visited;
    CALLOC(visited, G->size);


    graph_search_update(T, G, ini, visited);

    // Construction de la liste des sommets accessibles
    dequeue* res = create_dequeue();
    for (uint q = 0; q < G->size; q++) {
        if (visited[q] && (!rest || rest[q])) {
            rigins_dequeue(q, res);
        }
    }

    free(visited);
    return res;
}

void lgraph_search_update(graph_stype T, lgraph* G, dequeue* ini, bool* alph, bool* visited) {
    uint(*f)(dequeue*);
    if (T == DFS) {
        f = rigpull_dequeue;
    }
    else {
        f = lefpull_dequeue;
    }
    if (alph) {
        while (!isempty_dequeue(ini)) {
            uint q = f(ini);
            if (visited[q]) {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G->size_alpha; a++) {
                if (alph[a]) {
                    for (uint j = 0; j < size_dequeue(G->edges[q][a]); j++) {

                        rigins_dequeue(lefread_dequeue(G->edges[q][a], j), ini);
                    }
                }
            }
        }
    }
    else {
        while (!isempty_dequeue(ini)) {
            uint q = f(ini);
            if (visited[q]) {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G->size_alpha; a++) {
                for (uint j = 0; j < size_dequeue(G->edges[q][a]); j++) {
                    rigins_dequeue(lefread_dequeue(G->edges[q][a], j), ini);
                }
            }
        }
    }
}

dequeue* lgraph_search(graph_stype T, lgraph* G, dequeue* ini, bool* alph, bool* rest) {

    // Tableau pour mémoriser les sommets visités
    bool* visited;
    CALLOC(visited, G->size_graph);


    lgraph_search_update(T, G, ini, alph, visited);
    dequeue* res = create_dequeue();
    for (uint q = 0; q < G->size_graph; q++) {
        if (visited[q] && (!rest || rest[q])) {
            rigins_dequeue(q, res);
        }
    }

    free(visited);

    return res;
}

void dgraph_search_update(graph_stype T, dgraph* G, dequeue* ini, bool* alph, bool* visited) {
    uint(*f)(dequeue*);
    if (T == DFS) {
        f = rigpull_dequeue;
    }
    else {
        f = lefpull_dequeue;
    }
    if (alph) {
        while (!isempty_dequeue(ini)) {
            uint q = f(ini);
            if (visited[q]) {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G->size_alpha; a++) {
                if (alph[a]) {
                    rigins_dequeue(G->edges[q][a], ini);
                }
            }
        }
    }
    else {
        while (!isempty_dequeue(ini)) {
            uint q = f(ini);
            if (visited[q]) {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G->size_alpha; a++) {
                rigins_dequeue(G->edges[q][a], ini);
            }
        }
    }
}

dequeue* dgraph_search(graph_stype T, dgraph* G, dequeue* ini, bool* alph, bool* rest) {

    // Tableau pour mémoriser les sommets visités
    bool* visited;
    CALLOC(visited, G->size_graph);

    for (uint q = 0; q < G->size_graph; q++) {
        visited[q] = false;
    }

    dgraph_search_update(T, G, ini, alph, visited);
    dequeue* res = create_dequeue();
    for (uint q = 0; q < G->size_graph; q++) {
        if (visited[q] && (!rest || rest[q])) {
            rigins_dequeue(q, res);
        }
    }
    free(visited);

    return res;
}


void twin_dgraph_search_update(graph_stype T, dgraph* G1, dgraph* G2, dequeue* ini, bool* alph, bool* visited) {
    if (G1->size_graph != G2->size_graph || G1->size_alpha != G2->size_alpha) {
        fprintf(stderr, "Error: the two graphs have different sizes.\n");
        return;
    }


    uint(*f)(dequeue*);
    if (T == DFS) {
        f = rigpull_dequeue;
    }
    else {
        f = lefpull_dequeue;
    }

    if (alph) {
        while (!isempty_dequeue(ini)) {
            uint q = f(ini);
            if (visited[q]) {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G1->size_alpha; a++) {
                if (alph[a]) {
                    rigins_dequeue(G1->edges[q][a], ini);
                    rigins_dequeue(G2->edges[q][a], ini);
                }
            }
        }
    }
    else {
        while (!isempty_dequeue(ini)) {
            uint q = f(ini);
            if (visited[q]) {
                continue;
            }
            visited[q] = true;
            for (uint a = 0; a < G1->size_alpha; a++) {
                rigins_dequeue(G1->edges[q][a], ini);
                rigins_dequeue(G2->edges[q][a], ini);
            }
        }
    }
}

dequeue* twin_dgraph_search(graph_stype T, dgraph* G1, dgraph* G2, dequeue* ini, bool* alph, bool* rest) {
    if (G1->size_graph != G2->size_graph || G1->size_alpha != G2->size_alpha) {
        fprintf(stderr, "Error: the two graphs have different sizes.\n");
        return NULL;
    }

    // Tableau pour mémoriser les sommets visités
    bool* visited;
    CALLOC(visited, G1->size_graph);


    twin_dgraph_search_update(T, G1, G2, ini, alph, visited);
    dequeue* res = create_dequeue();
    for (uint q = 0; q < G1->size_graph; q++) {
        if (visited[q] && (!rest || rest[q])) {
            rigins_dequeue(q, res);
        }
    }

    free(visited);

    return res;
}



/********************************/
/*+ Disjoint merging of graphs +*/
/********************************/

graph* merge_graphs(uint* lag, uint n, ...) {
    if (n == 0 || !lag) {
        return NULL;
    }

    va_list list;
    va_start(list, n);
    uint size = 0;
    graph* glist[n];
    uint k = 0;

    for (uint i = 0; i < n; i++) {
        glist[i] = va_arg(list, graph*);
        lag[i] = size;
        if (glist[i]) {
            size += glist[i]->size;
            k++;
        }
    }

    if (k == 0) {
        return NULL;
    }

    graph* new = create_graph_noedges(size);
    for (uint i = 0; i < n; i++) {
        if (glist[i]) {
            for (uint q = 0; q < glist[i]->size; q++) {
                copy_dequeue_right(new->edges[q + lag[i]], glist[i]->edges[q], lag[i]);
            }
        }
    }
    return new;
}

lgraph* merge_lgraphs(uint* lag, uint n, ...) {
    if (n == 0 || !lag) {
        return NULL;
    }

    va_list list;
    va_start(list, n);
    lgraph* glist[n];
    uint size_graph = 0;
    uint k = 0;

    for (uint i = 0; i < n; i++) {
        glist[i] = va_arg(list, lgraph*);
        lag[i] = size_graph;
        if (glist[i]) {
            size_graph += glist[i]->size_graph;
            k++;
        }
    }
    if (k == 0) {
        return NULL;
    }
    uint size_alpha = 0;
    for (uint i = 0; i < n; i++) {
        if (glist[i]) {
            size_alpha = glist[i]->size_alpha;
            break;
        }
    }
    for (uint i = 0; i < n; i++) {
        if (glist[i] && size_alpha != glist[i]->size_alpha) {
            fprintf(stderr, "Error: tried to merge graphs with distinct alphabet sizes.\n");
            return NULL;
        }
    }

    // Création du nouveau graph
    lgraph* new = create_lgraph_noedges(size_graph, size_alpha);
    for (uint i = 0; i < n; i++) {
        if (glist[i]) {
            // Pour tout sommet du graphe i
            for (uint q = 0; q < glist[i]->size_graph; q++) {
                // Pour toute lettre a
                for (uint a = 0; a < glist[i]->size_alpha; a++) {
                    // On copie les sommets adjacents en décalant leurs numéros
                    copy_dequeue_right(new->edges[q + lag[i]][a], glist[i]->edges[q][a], lag[i]);
                }
            }
        }
    }
    return new;
}

/*************************************************/
/*+ Transformations et fusion jointe de graphes +*/
/*************************************************/

graph* ldgraphs_to_graph(uint ng, uint nlg, uint ndg, uint n, ...) {
    if (n == 0 || ng + nlg + ndg != n) { // Si il n'y a pas d'arguments ou qu'ils sont inconsistants
        return NULL;
    }
    // Récupération des inputs
    va_list list;
    va_start(list, n);
    uint size = 0;
    graph* G = NULL;

    // lgraph* lginputs[nlg + 1];
    // dgraph* dginputs[ndg + 1];
    if (ng != 0) {
        graph* ginput;

        for (uint i = 0; i < ng; i++) {
            ginput = va_arg(list, graph*);
            if (i == 0) {
                size = ginput->size;
                G = create_graph_noedges(size);
            }
            else if (ginput->size != size) {
                printf("Error, the graphs should all have the same size\n");
                delete_graph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++) {
                merge_sorted_dequeue(G->edges[q], ginput->edges[q]);
            }
        }
    }
    if (nlg != 0) {
        // printf("test\n");
        lgraph* lginput;
        for (uint i = 0; i < nlg; i++) {
            lginput = va_arg(list, lgraph*);
            if (i == 0 && ng == 0) {
                size = lginput->size_graph;
                // printf("thesize %d\n", size);
                G = create_graph_noedges(size);
            }
            else if (lginput->size_graph != size) {
                printf("Error, the graphs should all have the same size\n");
                delete_graph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++) {
                for (uint a = 0; a < lginput->size_alpha; a++) {
                    merge_sorted_dequeue(G->edges[q], lginput->edges[q][a]);
                }
            }
        }
    }

    if (ndg != 0) {
        dgraph* dginput;
        for (uint i = 0; i < ndg; i++) {
            dginput = va_arg(list, dgraph*);
            if (i == 0 && ng == 0 && nlg == 0) {
                size = dginput->size_graph;
                G = create_graph_noedges(size);
            }
            else if (dginput->size_graph != size) {
                printf("Error, the graphs should all have the same size\n");
                delete_graph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++) {
                for (uint a = 0; a < dginput->size_alpha; a++) {
                    insert_dequeue(G->edges[q], dginput->edges[q][a]);
                }
            }
        }
    }

    return G;
}

lgraph* ldgraphs_to_lgraph(uint nlg, uint ndg, uint n, ...) {
    if (n == 0 || nlg + ndg != n) { // Si il n'y a pas d'arguments ou qu'ils sont inconsistants
        return NULL;
    }
    // Récupération des inputs
    va_list list;
    va_start(list, n);
    uint size = 0;
    uint asize;
    lgraph* G = NULL;

    if (nlg != 0) {
        // printf("test\n");
        lgraph* lginput;
        for (uint i = 0; i < nlg; i++) {
            lginput = va_arg(list, lgraph*);
            if (i == 0) {
                size = lginput->size_graph;
                asize = lginput->size_alpha;
                // printf("thesize %d\n", size);
                G = create_lgraph_noedges(size, asize);
            }
            else if (lginput->size_graph != size) {
                printf("Error, the graphs should all have the same size\n");
                delete_lgraph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++) {
                for (uint a = 0; a < lginput->size_alpha; a++) {
                    merge_sorted_dequeue(G->edges[q][a], lginput->edges[q][a]);
                }
            }
        }
    }

    if (ndg != 0) {
        dgraph* dginput;
        for (uint i = 0; i < ndg; i++) {
            dginput = va_arg(list, dgraph*);
            if (i == 0 && nlg == 0) {
                size = dginput->size_graph;
                asize = dginput->size_alpha;
                G = create_lgraph_noedges(size, asize);
            }
            else if (dginput->size_graph != size) {
                printf("Error, the graphs should all have the same size\n");
                delete_lgraph(G);
                return NULL;
            }
            for (uint q = 0; q < size; q++) {
                for (uint a = 0; a < dginput->size_alpha; a++) {
                    insert_dequeue(G->edges[q][a], dginput->edges[q][a]);
                }
            }
        }
    }

    return G;
}


lgraph* dgraph_to_lgraph(dgraph* DG) {
    lgraph* LG = create_lgraph_noedges(DG->size_graph, DG->size_alpha);
    for (uint q = 0; q < DG->size_graph; q++) {
        for (uint a = 0; a < DG->size_alpha; a++) {
            rigins_dequeue(DG->edges[q][a], LG->edges[q][a]);
        }
    }
    return LG;
}


graph* lgraph_to_graph(lgraph* lg) {
    graph* G = create_graph_noedges(lg->size_graph);
    dequeue** array;
    MALLOC(array, lg->size_alpha);
    for (uint q = 0; q < lg->size_graph; q++) {
        merge_array_sorted_dequeue(G->edges[q], lg->edges[q], lg->size_alpha);
    }
    return G;

}

graph* lgraph_to_graph_alpha(lgraph* lg, bool* alph) {
    graph* G = create_graph_noedges(lg->size_graph);
    uint n = lg->size_alpha;
    if (alph) {
        n = 0;
        for (uint a = 0; a < lg->size_alpha; a++) {
            if (alph[a]) {
                n++;
            }
        }
    }
    dequeue** array;
    MALLOC(array, n);

    for (uint q = 0; q < lg->size_graph; q++) {
        uint i = 0;
        for (uint a = 0; a < lg->size_alpha; a++) {
            if (alph[a]) {
                array[i] = lg->edges[q][a];
                i++;
            }
        }
        merge_array_sorted_dequeue(G->edges[q], array, n);
    }
    return G;

}



graph* dgraph_to_graph(dgraph* dg) {
    graph* G = create_graph_noedges(dg->size_graph);
    for (uint q = 0; q < dg->size_graph; q++) {
        for (uint a = 0; a < dg->size_alpha; a++) {
            insert_dequeue(G->edges[q], dg->edges[q][a]);
        }
    }
    return G;

}

graph* dgraph_to_graph_alpha(dgraph* dg, bool* alph) {
    graph* G = create_graph_noedges(dg->size_graph);
    for (uint q = 0; q < dg->size_graph; q++) {
        for (uint a = 0; a < dg->size_alpha; a++) {
            if (alph[a]) {
                insert_dequeue(G->edges[q], dg->edges[q][a]);
            }
        }
    }
    return G;

}


/*****************************************************************/
/*+ Récupération des sommets adjacents à un ensemble de sommets +*/
/*****************************************************************/

dequeue* lgraph_reachable(lgraph* G, dequeue* start, uint a) {
    dequeue* tomerge[size_dequeue(start)];
    for (uint i = 0; i < size_dequeue(start); i++) {
        tomerge[i] = G->edges[lefread_dequeue(start, i)][a];
    }
    dequeue* end = create_dequeue();
    merge_array_sorted_dequeue(end, tomerge, size_dequeue(start));
    return end;
}




