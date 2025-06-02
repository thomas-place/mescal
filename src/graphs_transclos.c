/*********************************/
/* Transitive closure procedures */
/*********************************/

#include "graphs_transclos.h"

/**************************/
/* Fonctions sur les DAGs */
/**************************/

// Prend un graphe quelconque et la liste de ses SCCs en entrée (déjà calculée)
// Retourne le DAG obtenu en prenant chaque SCC comme sommet
graph* compute_dag_of_sccs(graph* G, parti* clist)
{
    // Précalcul: liste des classes adjacentes pour chaque sommet
    dequeue* adjclass[G->size];
    for (uint q = 0; q < G->size; q++)
    {
        adjclass[q] = create_dequeue();
        for (uint i = 0; i < size_dequeue(G->edges[q]); i++) // Boucle sur les sommets adjacents à q
        {
            uint c = clist->numcl[lefread_dequeue(G->edges[q], i)]; // la classe du ième sommet adjacent à q

            if ((isempty_dequeue(adjclass[q]) || c != rigread_dequeue(adjclass[q], 0)) && c != clist->numcl[q])
                rigins_dequeue(c, adjclass[q]);
        }
    }

    // Création du nouveau graph
    graph* new = create_graph_noedges(clist->size_par);
    for (uint c = 0; c < clist->size_par; c++)
    {
        for (uint i = 0; i < clist->cl_size[c]; i++)
        {
            merge_sorted_dequeue(new->edges[c], adjclass[clist->cl_elems[c][i]]);
        }
    }
    for (uint q = 0; q < clist->size_set; q++)
    {
        delete_dequeue(adjclass[q]);
    }

    return new;
}

// Fonction auciliaire pour le tri topologique
static void topo_sort_dag_visit(uint q, graph* G, dequeue* thesort, bool* marked)
{
    // printf("the q: %d\n", q);
    if (marked[q])
    {
        return;
    }
    else
    {
        for (uint i = 0; i < size_dequeue(G->edges[q]); i++)
        {
            uint r = lefread_dequeue(G->edges[q], i);
            if (r != q)
            {
                topo_sort_dag_visit(r, G, thesort, marked);
            }
        }
        marked[q] = true;
        lefins_dequeue(q, thesort);
    }
}

// Prend un DAG en entrée (ne termine pas sur un graphe quelconque)
// Calcule un tri topologique de ses sommets
dequeue* topo_sort_dag(graph* G)
{
    // Future liste contenant le tri topologique
    dequeue* thesort = create_dequeue();

    // Tableaux de Booléens pour marquer les sommets
    bool marked[G->size];
    for (uint i = 0; i < G->size; i++)
    {
        marked[i] = false;
    }

    // Calcul
    for (uint q = 0; q < G->size; q++)
    {
        topo_sort_dag_visit(q, G, thesort, marked);
    }
    return thesort;
}

dequeue* topo_sort_dag_start(graph* G, uint start)
{
    // Future liste contenant le tri topologique
    dequeue* thesort = create_dequeue();

    // Tableaux de Booléens pour marquer les sommets
    bool marked[G->size];
    for (uint i = 0; i < G->size; i++)
    {
        marked[i] = false;
    }

    topo_sort_dag_visit(start, G, thesort, marked);
    return thesort;
}

// Calcule la clôture transitive d'un DAG
// Si topo_sort == NULL, les sommets doivent être ordonnés selon un tri topologique
// Sinon, topo_sort doit contenir un tri topologique des sommets
graph* compute_tclos_dag(graph* G, dequeue* topo_sort)
{

    // dequeue* topo_sort = topo_sort_dag(G);
    graph* TC = create_graph_noedges(G->size);
    for (uint q = 0; q < G->size; q++)
    {
        TC->edges[q] = create_dequeue();
        rigins_dequeue(q, TC->edges[q]);
    }
    for (uint i = 0; i < G->size; i++)
    {
        uint q;
        if (topo_sort == NULL) // Dans ce cas, on suppose que les sommets sont déjà dans l'ordre topologique
        {
            q = G->size - 1 - i;
        }
        else // Sinon, on prend l'ordre donné par topo_sort
        {
            q = rigread_dequeue(topo_sort, i);
        }

        for (uint j = 0; j < size_dequeue(G->edges[q]); j++)
        {
            merge_sorted_dequeue(TC->edges[q], TC->edges[lefread_dequeue(G->edges[q], j)]);
        }
    }
    // delete_dequeue(topo_sort);
    return TC;
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
