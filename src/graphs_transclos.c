/*********************************/
/* Transitive closure procedures */
/*********************************/

#include "graphs_transclos.h"

/**************************/
/* Fonctions sur les DAGs */
/**************************/

// // Prend un graphe quelconque et la liste de ses SCCs en entrée (déjà calculée)
// // Retourne le DAG obtenu en prenant chaque SCC comme sommet
// graph *compute_dag_of_sccs(graph *G, parti *clist)
// {
//     // Précalcul: liste des classes adjacentes pour chaque sommet
//     dequeue *adjclass[G->size];
//     for (uint q = 0; q < G->size; q++)
//     {
//         adjclass[q] = create_dequeue();
//         uint start = G->intervals[q];
//         uint end = G->intervals[q + 1];
//         for (uint i = start; i < end; i++)
//         {
//             uint c = clist->numcl[G->storage[i]]; // la classe du ième sommet adjacent à q

//             if ((isempty_dequeue(adjclass[q]) || c != rigread_dequeue(adjclass[q], 0)) && c != clist->numcl[q])
//                 rigins_dequeue(c, adjclass[q]);
//         }
//     }

//     // Création du nouveau graph
//     graph *new = create_graph_noedges(clist->size_par);
//     for (uint c = 0; c < clist->size_par; c++)
//     {
//         for (uint i = 0; i < clist->cl_size[c]; i++)
//         {
//             merge_sorted_dequeue(new->edges[c], adjclass[clist->cl_elems[c][i]]);
//         }
//     }
//     for (uint q = 0; q < clist->size_set; q++)
//     {
//         delete_dequeue(adjclass[q]);
//     }

//     return new;
// }

// // Fonction auciliaire pour le tri topologique
// static void topo_sort_dag_visit(uint q, graph *G, dequeue *thesort, bool *marked)
// {
//     // printf("the q: %d\n", q);
//     if (marked[q])
//     {
//         return;
//     }
//     else
//     {
//         for (uint i = 0; i < size_dequeue(G->edges[q]); i++)
//         {
//             uint r = lefread_dequeue(G->edges[q], i);
//             if (r != q)
//             {
//                 topo_sort_dag_visit(r, G, thesort, marked);
//             }
//         }
//         marked[q] = true;
//         lefins_dequeue(q, thesort);
//     }
// }

// // Prend un DAG en entrée (ne termine pas sur un graphe quelconque)
// // Calcule un tri topologique de ses sommets
// dequeue *topo_sort_dag(graph *G)
// {
//     // Future liste contenant le tri topologique
//     dequeue *thesort = create_dequeue();

//     // Tableaux de Booléens pour marquer les sommets
//     bool marked[G->size];
//     for (uint i = 0; i < G->size; i++)
//     {
//         marked[i] = false;
//     }

//     // Calcul
//     for (uint q = 0; q < G->size; q++)
//     {
//         topo_sort_dag_visit(q, G, thesort, marked);
//     }
//     return thesort;
// }

// dequeue *topo_sort_dag_start(graph *G, uint start)
// {
//     // Future liste contenant le tri topologique
//     dequeue *thesort = create_dequeue();

//     // Tableaux de Booléens pour marquer les sommets
//     bool marked[G->size];
//     for (uint i = 0; i < G->size; i++)
//     {
//         marked[i] = false;
//     }

//     topo_sort_dag_visit(start, G, thesort, marked);
//     return thesort;
// }

// // Calcule la clôture transitive d'un DAG
// // Si topo_sort == NULL, les sommets doivent être ordonnés selon un tri topologique
// // Sinon, topo_sort doit contenir un tri topologique des sommets
// graph *compute_tclos_dag(graph *G, dequeue *topo_sort)
// {

//     // dequeue* topo_sort = topo_sort_dag(G);
//     graph *TC = create_graph_noedges(G->size);
//     for (uint q = 0; q < G->size; q++)
//     {
//         TC->edges[q] = create_dequeue();
//         rigins_dequeue(q, TC->edges[q]);
//     }
//     for (uint i = 0; i < G->size; i++)
//     {
//         uint q;
//         if (topo_sort == NULL) // Dans ce cas, on suppose que les sommets sont déjà dans l'ordre topologique
//         {
//             q = G->size - 1 - i;
//         }
//         else // Sinon, on prend l'ordre donné par topo_sort
//         {
//             q = rigread_dequeue(topo_sort, i);
//         }

//         for (uint j = 0; j < size_dequeue(G->edges[q]); j++)
//         {
//             merge_sorted_dequeue(TC->edges[q], TC->edges[lefread_dequeue(G->edges[q], j)]);
//         }
//     }
//     // delete_dequeue(topo_sort);
//     return TC;
// }

// Transitive closure of a DAG
// Vertices must be ordered according to a topological sort
graph *compute_tclos_dag(graph *G, bool self)
{
    dequeue **TC;
    MALLOC(TC, G->size);
    uint nedge = 0;
    for (uint i = 0; i < G->size; i++)
    {
        uint q = G->size - 1 - i;
        TC[q] = create_dequeue();
        rigins_dequeue(q, TC[q]);
        uint start = GSTART(G, q);
        uint end = GEND(G, q);
        for (uint j = start; j < end; j++)
        {
            merge_sorted_dequeue(TC[q], TC[G->storage[j]]);
        }
        nedge += size_dequeue(TC[q]) - (self ? 0 : 1); // -1 because we don't count the loop to itself
    }

    graph *ngraph;
    MALLOC(ngraph, 1);
    ngraph->size = G->size;
    ngraph->size_edges = nedge;
    MALLOC(ngraph->intervals, G->size + 1);
    MALLOC(ngraph->storage, nedge);
    ngraph->intervals[0] = 0;
    for (uint i = 0; i < G->size; i++)
    {

        uint idx = ngraph->intervals[i];
        for (uint j = (self ? 0 : 1); j < size_dequeue(TC[i]); j++)
        {
            ngraph->storage[idx++] = lefread_dequeue(TC[i], j);
        }
        ngraph->intervals[i + 1] = idx;
        delete_dequeue(TC[i]);
    }
    free(TC);
    return ngraph;
}

graph *compute_tclos_dgraph(dgraph *G, parti *sccs, bool self)
{
    bool input_sccs = true;
    if (!sccs)
    {
        sccs = dtarjan(G, NULL, false);
        input_sccs = false;
    }
    graph *dag_sccs = dgraph_merge(G, sccs);
    graph *tc_dag_sccs = compute_tclos_dag(dag_sccs, self);
    delete_graph(dag_sccs);
    if (!input_sccs)
    {
        delete_parti(sccs);
    }
    return tc_dag_sccs;
}

graph *tclos_reduction_graph(bool **adg, uint size)
{
    dequeue **edges;
    MALLOC(edges, size);
    for (uint i = 0; i < size; i++)
    {
        edges[i] = create_dequeue();
        for (uint j = 0; j < size; j++)
        {
            if (adg[i][j] && i != j)
            {
                rigins_dequeue(j, edges[i]);
            }
        }
    }

    uint tsize = 0;
    dequeue **newedges;
    MALLOC(newedges, size);
    for (uint q = 0; q < size; q++)
    {
        bool *clos;
        CALLOC(clos, size);
        for (uint j = 0; j < size_dequeue(edges[q]); j++)
        {
            uint r = lefread_dequeue(edges[q], j);
            for (uint k = 0; k < size_dequeue(edges[r]); k++)
            {
                uint s = lefread_dequeue(edges[r], k);
                clos[s] = true;
            }
        }
        newedges[q] = create_dequeue();
        for (uint j = 0; j < size_dequeue(edges[q]); j++)
        {
            uint r = lefread_dequeue(edges[q], j);
            if (!clos[r])
            {
                rigins_dequeue(r, newedges[q]);
            }
        }
        tsize += size_dequeue(newedges[q]);
        free(clos);
    }

    graph *ngraph = create_graph_noedges(size);
    ngraph->size_edges = tsize;
    MALLOC(ngraph->storage, tsize);
    ngraph->intervals[0] = 0;
    for (uint q = 0; q < size; q++)
    {
        uint idx = ngraph->intervals[q];
        for (uint j = 0; j < size_dequeue(newedges[q]); j++)
        {
            ngraph->storage[idx++] = lefread_dequeue(newedges[q], j);
        }
        ngraph->intervals[q + 1] = idx;
        delete_dequeue(newedges[q]);
        delete_dequeue(edges[q]);
    }

    free(newedges);
    free(edges);
    return ngraph;
}

/*****************************************/
/* Fonctions sur les graphes quelconques */
/*****************************************/

// Prend un graphe quelconque en entrée et l'étend en faisant sa clôture transitive
// void make_tclos_graph(graph* G)
// {

//     // Calcul du graphe des SCCs
//     parti* clist = tarjan(G);
//     graph* dag_of_sccs = compute_dag_of_sccs(G, clist);

//     // graph_printing_test(dag_of_sccs, stdout);

//     // Calcul de la cloture transitive du graphe des SCCs
//     // Les sommets sont déjà triés selon un ordre topologique (conséquence de tarjan)
//     graph* tc_dag_of_scss = compute_tclos_dag(dag_of_sccs, NULL);

//     // graph_printing_test(tc_dag_of_scss, stdout);

//     for (uint q = 0; q < G->size; q++)
//     {
//         uint c = clist->numcl[q]; // La classe de q
//         for (uint i = 0; i < size_dequeue(tc_dag_of_scss->edges[c]); i++)
//         { // On lit les classes adjacentes dans la TC
//             uint d = lefread_dequeue(tc_dag_of_scss->edges[c], i);
//             merge_sorted_dequeue(G->edges[q], clist->cl[d]);
//         }
//     }

//     // Libération des objets temporaires
//     delete_parti(clist);
//     delete_graph(dag_of_sccs);
//     delete_graph(tc_dag_of_scss);
// }
