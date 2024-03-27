#include "printing.h"

#define _GNU_SOURCE

bool external_viewer = false;

/**********************/
/* Printing functions */
/**********************/

// Print d'un sommet, version noms contenus dans un deq
void list_print_state(void *p, FILE *out)
{
    if (isempty_vertices(p))
    {
        fprintf(out, "E");
    }
    else
    {
        for (uint j = 0; j < size_vertices(p) - 1; j++)
        {
            fprintf(out, "%d,", lefread_vertices(p, j));
        }
        fprintf(out, "%d", lefread_vertices(p, size_vertices(p) - 1));
    }
}

static void print_expo(uint n, FILE *out)
{
    if (n != 0)
    {
        uint d = n % 10;
        print_expo(n / 10, out);
        switch (d)
        {
        case 0:
            fprintf(out, "0");
            break;
        case 1:
            fprintf(out, "1");
            break;
        case 2:
            fprintf(out, "2");
            break;
        case 3:
            fprintf(out, "3");
            break;
        case 4:
            fprintf(out, "4");
            break;
        case 5:
            fprintf(out, "5");
            break;
        case 6:
            fprintf(out, "6");
            break;
        case 7:
            fprintf(out, "7");
            break;
        case 8:
            fprintf(out, "8");
            break;
        case 9:
            fprintf(out, "9");
            break;
        default:
            break;
        }
    }
}

// Print d'un sommet, version cayley graph
static void cayley_print_state_label(void *p, FILE *out)
{
    fprintf(out, "<");
    if (isempty_vertices(p))
    {
        fprintf(out, "1");
    }
    else
    {
        uint n = 1;
        fprintf(out, "%c", lefread_vertices(p, 0) + 'a');
        for (uint i = 1; i < size_vertices(p); i++)
        {
            if (lefread_vertices(p, i) != lefread_vertices(p, i - 1))
            {
                if (n > 1)
                {
                    fprintf(out, "<SUP>");
                    print_expo(n, out);
                    fprintf(out, "</SUP>");
                    while (n != 0)
                    {
                        n = n / 10;
                    }
                }
                fprintf(out, "%c", lefread_vertices(p, i) + 'a');
                n = 1;
            }
            else
            {
                n++;
            }
        }
        if (n > 1)
        {
            fprintf(out, "<SUP>");
            print_expo(n, out);
            fprintf(out, "</SUP>");
            while (n != 0)
            {
                n = n / 10;
            }
        }
    }
    fprintf(out, ">");
}

// Print d'un sommet, version cayley graph
void cayley_print_state(void *p, FILE *out)
{
    if (isempty_vertices(p))
    {
        fprintf(out, "1");
    }
    else
    {
        for (uint j = 0; j < size_vertices(p); j++)
        {
            fprintf(out, "%c", lefread_vertices(p, j) + 'a');
        }
    }
}

// <SUP>-1 < / SUP >

/********************************/
/* Print des arêtes d'un graphe */
/********************************/

void simple_gedges_print(p_graph G, FILE *out)
{
    p_stack theedges = graph_to_edges(G);

    for (uint i = 0; i < size_stack(theedges); i++)
    { // Boucle sur les états de départ
        fprintf(out, "\"%d\" -> \"%d\" [label = \"1\"];\n", ((p_edge)read_stack(theedges, i))->in,
                ((p_edge)read_stack(theedges, i))->out);
    }
}

void simple_lgedges_print(p_stack theedges, FILE *out)
{
    for (uint i = 0; i < size_stack(theedges); i++)
    { // Boucle sur les états de départ
        fprintf(out, "\"%d\" -> \"%d\" [label = <", ((p_multi_edge)read_stack(theedges, i))->in,
                ((p_multi_edge)read_stack(theedges, i))->out);
        if (((p_multi_edge)read_stack(theedges, i))->eps)
        {
            // printf("eps\n");
            if (!isempty_vertices(((p_multi_edge)read_stack(theedges,
                                                            i))->lab) ||
                !isempty_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i))
            {
                fprintf(out, "1,");
            }
            else
            {
                fprintf(out, "1>];\n");
            }
        }
        if (!isempty_vertices(((p_multi_edge)read_stack(theedges, i))->lab))
        {
            // printf("normal\n");
            for (uint j = 0; j < size_vertices(((p_multi_edge)read_stack(theedges, i))->lab) - 1;
                 j++)
            {
                fprintf(out, "%c,",
                        lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab, j) + 'a');
            }
            if (!isempty_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i))
            {
                fprintf(out, "%c,",
                        lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab,
                                         size_vertices(((p_multi_edge)read_stack(theedges,
                                                                                 i))->lab) - 1) +
                        'a');
            }
            else
            {
                fprintf(out, "%c>];\n",
                        lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab,
                                         size_vertices(((p_multi_edge)read_stack(theedges,
                                                                                 i))->lab) - 1) +
                        'a');
            }
        }

        if (!isempty_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i))
        {
            // printf("test\n");
            for (uint j = 0; j < size_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i) - 1;
                 j++)
            {
                // printf("j: %d\n", j);
                fprintf(out, "%c<SUP>-1</SUP>,",
                        lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i, j) + 'a');
            }
            fprintf(out, "%c<SUP>-1</SUP>>];\n",
                    lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i,
                                     size_vertices(((p_multi_edge)read_stack(theedges,
                                                                             i))->lab_i) - 1) +
                    'a');
        }
    }
}

void named_lgedges_print(p_stack theedges, p_nfa A, FILE *out)
{
    for (uint i = 0; i < size_stack(theedges); i++)
    { // Boucle sur les états de départ
        fprintf(out, "\"");
        nfa_print_name(A, ((p_multi_edge)read_stack(theedges, i))->in, out);
        // (*f)(names[((p_multi_edge)read_stack(theedges, i))->in], out);
        fprintf(out, "\" -> \"");
        nfa_print_name(A, ((p_multi_edge)read_stack(theedges, i))->out, out);
        // (*f)(names[((p_multi_edge)read_stack(theedges, i))->out], out);
        fprintf(out, "\" [label = <");
        if (((p_multi_edge)read_stack(theedges, i))->eps)
        {
            // printf("eps\n");
            if (!isempty_vertices(((p_multi_edge)read_stack(theedges,
                                                            i))->lab) ||
                !isempty_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i))
            {
                fprintf(out, "1,");
            }
            else
            {
                fprintf(out, "1>];\n");
            }
        }
        if (!isempty_vertices(((p_multi_edge)read_stack(theedges, i))->lab))
        {
            // printf("normal\n");
            for (uint j = 0; j < size_vertices(((p_multi_edge)read_stack(theedges, i))->lab) - 1;
                 j++)
            {
                fprintf(out, "%c,",
                        lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab, j) + 'a');
            }
            if (!isempty_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i))
            {
                fprintf(out, "%c,",
                        lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab,
                                         size_vertices(((p_multi_edge)read_stack(theedges,
                                                                                 i))->lab) - 1) +
                        'a');
            }
            else
            {
                fprintf(out, "%c>];\n",
                        lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab,
                                         size_vertices(((p_multi_edge)read_stack(theedges,
                                                                                 i))->lab) - 1) +
                        'a');
            }
        }

        if (!isempty_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i))
        {
            // printf("test\n");
            for (uint j = 0; j < size_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i) - 1;
                 j++)
            {
                // printf("j: %d\n", j);
                fprintf(out, "%c<SUP>-1</SUP>,",
                        lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i, j) + 'a');
            }
            fprintf(out, "%c<SUP>-1</SUP>>];\n",
                    lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab_i,
                                     size_vertices(((p_multi_edge)read_stack(theedges,
                                                                             i))->lab_i) - 1) +
                    'a');
        }
    }
}

void named_gedges_print(p_graph G, void **names, void (*f) (void *, FILE *), FILE *out)
{
    p_stack theedges = graph_to_edges(G);
    for (uint i = 0; i < size_stack(theedges); i++)
    { // Boucle sur les états de départ
        fprintf(out, "\"");
        (*f) (names[((p_edge)read_stack(theedges, i))->in], out);
        fprintf(out, "\" -> \"");
        (*f) (names[((p_edge)read_stack(theedges, i))->out], out);
        fprintf(out, "\" [label = \"1\"];\n");
    }
}

// void named_lgedges_print(p_lgraph G, void **names, void (*f)(void *, FILE *),
// FILE *out)
// {
// p_stack theedges = lgraph_to_multi_edges(0, G);
// for (uint i = 0; i < size_stack(theedges); i++)
// { // Boucle sur les états de départ
// fprintf(out, "\"");
// (*f)(names[((p_multi_edge)read_stack(theedges, i))->in], out);
// fprintf(out, "\" -> \"");
// (*f)(names[((p_multi_edge)read_stack(theedges, i))->out], out);
// fprintf(out, "\" [label = \"");
// for (uint j = 0; j < size_vertices(((p_multi_edge)read_stack(theedges,
// i))->lab) - 1; j++)
// {
// fprintf(out, "%c,", lefread_vertices(((p_multi_edge)read_stack(theedges,
// i))->lab, j) + 'a');
// }
// fprintf(out, "%c\"];\n", lefread_vertices(((p_multi_edge)read_stack(theedges,
// i))->lab,
// size_vertices(((p_multi_edge)read_stack(theedges, i))->lab) - 1) + 'a');
// }
// }

void named_dgedges_print(p_dgraph G, void **names, void (*f) (void *, FILE *), FILE *out)
{
    p_stack theedges = dgraph_to_multi_edges(G);
    for (uint i = 0; i < size_stack(theedges); i++)
    { // Boucle sur les états de départ
        fprintf(out, "<");
        (*f) (names[((p_multi_edge)read_stack(theedges, i))->in], out);
        fprintf(out, "> -> <");
        (*f) (names[((p_multi_edge)read_stack(theedges, i))->out], out);
        fprintf(out, "> [label = <");
        for (uint j = 0; j < size_vertices(((p_multi_edge)read_stack(theedges, i))->lab) - 1; j++)
        {
            fprintf(out, "%c,",
                    lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab, j) + 'a');
        }
        fprintf(out, "%c>];\n",
                lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab,
                                 size_vertices(((p_multi_edge)read_stack(theedges,
                                                                         i))->lab) - 1) + 'a');
    }
}

/******************/
/* Print d'un NFA */
/******************/

// Répartition des états initiaux-finaux en trois catégories:
// initial seulement, final seulement, les deux en même temps
static void sort_spe_states(p_nfa A, p_vertices inionly, p_vertices finonly, p_vertices mixed)
{
    uint i = 0;
    uint j = 0;
    while (i < size_vertices(A->initials) || j < size_vertices(A->finals))
    {
        // printf("i: %d, j:%d \n", i, j);
        // printf("Deq size: %d\n",getsize_vertices(A->initials));
        if (j >= size_vertices(A->finals) ||
            (i < size_vertices(A->initials) && (lefread_vertices(A->initials,
                                                                 i) < lefread_vertices(A->finals,
                                                                                       j))))
        {
            // printf("Test %d\n", lefread_vertices(A->initials, i));
            rigins_vertices(lefread_vertices(A->initials, i), inionly);
            i++;
        }
        else if (i < size_vertices(A->initials) && lefread_vertices(A->initials,
                                                                    i) ==
                 lefread_vertices(A->finals, j))
        {
            rigins_vertices(lefread_vertices(A->initials, i), mixed);
            i++;
            j++;
        }
        else
        {
            rigins_vertices(lefread_vertices(A->finals, j), finonly);
            j++;
        }
    }
}

void nfa_print(p_nfa A, FILE *out)
{
    // if (A->names == NULL)
    // {
    // printf("Error, the names table is empty\n");
    // return;
    // }
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n");

    // Tri des états initiaux/finaux (on isole ceux qui sont les deux en même
    // temps)
    p_vertices inionly = create_vertices();
    p_vertices finonly = create_vertices();
    p_vertices mixed   = create_vertices();
    sort_spe_states(A, inionly, finonly, mixed);

    // États initiaux (coloriés en bleu-vert)
    if (!isempty_vertices(inionly))
    {
        fprintf(out, "node [fillcolor=\"blue:green\",style=filled,shape = circle];");
        for (uint i = 0; i < size_vertices(inionly); i++)
        {
            fprintf(out, " \"");
            // printf("i :%d\n", lefread_vertices(inionly, i));
            nfa_print_name(A, lefread_vertices(inionly, i), out);
            // if (A->names != NULL)
            // {
            // (*A->print_name)(A->names[lefread_vertices(inionly, i)], out);
            // }
            // else
            // {
            // fprintf(out, "%d", lefread_vertices(inionly, i));
            // }
            fprintf(out, "\"");
        }
        fprintf(out, ";\n");
    }

    // États finaux (double ronds)
    if (!isempty_vertices(finonly))
    {
        fprintf(out, "node [style=solid,shape = doublecircle];");
        for (uint i = 0; i < size_vertices(finonly); i++)
        {
            fprintf(out, " \"");
            nfa_print_name(A, lefread_vertices(finonly, i), out);
            // if (A->names != NULL)
            // {
            // (*A->print_name)(A->names[lefread_vertices(finonly, i)], out);
            // }
            // else
            // {
            // fprintf(out, "%d", lefread_vertices(finonly, i));
            // }
            fprintf(out, "\"");
        }
        fprintf(out, ";\n");
    }

    // États initiaux et finaux en même temps
    if (!isempty_vertices(mixed))
    {
        fprintf(out, "node [fillcolor=\"blue:green\",style=filled,shape = doublecircle];");
        for (uint i = 0; i < size_vertices(mixed); i++)
        {
            fprintf(out, " \"");
            nfa_print_name(A, lefread_vertices(mixed, i), out);
            // if (A->names != NULL)
            // {
            // (*A->print_name)(A->names[lefread_vertices(mixed, i)], out);
            // }
            // else
            // {
            // fprintf(out, "%d", lefread_vertices(mixed, i));
            // }
            fprintf(out, "\"");
        }
        fprintf(out, ";\n");
    }
    delete_vertices(inionly);
    delete_vertices(finonly);
    delete_vertices(mixed);

    fprintf(out, "node [style=solid,shape = circle];\n");
    p_stack theedges;

    // Calcul de l'ensemble de transitions
    if (A->etrans == NULL && A->itrans == NULL)
    {
        theedges = lgraph_to_multi_edges(0, A->trans);
    }
    else if (A->itrans == NULL)
    {
        theedges = lgraph_to_multi_edges(1, A->trans, A->etrans);
    }
    else if (A->etrans == NULL)
    {
        theedges = lgraph_to_multi_edges(2, A->trans, A->itrans);
    }
    else
    {
        theedges = lgraph_to_multi_edges(3, A->trans, A->itrans, A->etrans);
    }

    // Affichage
    if (A->names != NULL)
    {
        named_lgedges_print(theedges, A, out);
    }
    else
    {
        simple_lgedges_print(theedges, out);
    }

    fprintf(out, "}\n");
}

// void simple_nfa_print(p_nfa A, FILE *out)
// {
// fprintf(out, "digraph {\n");
// fprintf(out, "gradientangle=90\n");
// fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
// fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
// fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
// fprintf(out, "rankdir=LR;\n");

//// Tri des états initiaux/finaux (on isole ceux qui sont les deux en même
// temps)
// p_vertices inionly = create_vertices();
// p_vertices finonly = create_vertices();
// p_vertices mixed = create_vertices();
// sort_spe_states(A, inionly, finonly, mixed);

//// États initiaux (coloriés en bleu-vert)
// if (!isempty_vertices(inionly))
// {
// fprintf(out, "node [fillcolor=\"blue:green\",style=filled,shape = circle];");
// for (uint i = 0; i < size_vertices(inionly); i++)
// {
// fprintf(out, " \"%d\"", lefread_vertices(inionly, i));
// }
// fprintf(out, ";\n");
// }

//// États finaux (double ronds)
// if (!isempty_vertices(finonly))
// {
// fprintf(out, "node [style=solid,shape = doublecircle];");
// for (uint i = 0; i < size_vertices(finonly); i++)
// {
// fprintf(out, " \"%d\"", lefread_vertices(finonly, i));
// }
// fprintf(out, ";\n");
// }

//// États initiaux et finaux en même temps
// if (!isempty_vertices(mixed))
// {
// fprintf(out, "node [fillcolor=\"blue:green\",style=filled,shape =
// doublecircle];");
// for (uint i = 0; i < size_vertices(mixed); i++)
// {
// fprintf(out, " \"%d\"", lefread_vertices(mixed, i));
// }
// fprintf(out, ";\n");
// }
// delete_vertices(inionly);
// delete_vertices(finonly);
// delete_vertices(mixed);

// fprintf(out, "node [style=solid,shape = circle];\n");
// p_stack theedges;
// if (A->etrans == NULL && A->itrans == NULL)
// {
// theedges = lgraph_to_multi_edges(0, A->trans);
// }
// else if (A->itrans == NULL)
// {
// theedges = lgraph_to_multi_edges(1, A->trans, A->etrans);
// }
// else if (A->etrans == NULL)
// {
// theedges = lgraph_to_multi_edges(2, A->trans, A->itrans);
// }
// else
// {
// theedges = lgraph_to_multi_edges(3, A->trans, A->itrans, A->etrans);
// }
// simple_lgedges_print(theedges, out);
// fprintf(out, "}\n");
// }

// void named_nfa_print(p_nfa A, void (*f)(void *, FILE *), FILE *out)
// {
// if (A->names == NULL)
// {
// printf("Error, the names table is empty\n");
// return;
// }
// fprintf(out, "digraph {\n");
// fprintf(out, "gradientangle=90\n");
// fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
// fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
// fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
// fprintf(out, "rankdir=LR;\n");

//// Tri des états initiaux/finaux (on isole ceux qui sont les deux en même
// temps)
// p_vertices inionly = create_vertices();
// p_vertices finonly = create_vertices();
// p_vertices mixed = create_vertices();
// sort_spe_states(A, inionly, finonly, mixed);

//// États initiaux (coloriés en bleu-vert)
// if (!isempty_vertices(inionly))
// {
// fprintf(out, "node [fillcolor=\"blue:green\",style=filled,shape = circle];");
// for (uint i = 0; i < size_vertices(inionly); i++)
// {
// fprintf(out, " \"");
// (*f)(A->names[lefread_vertices(inionly, i)], out);
// fprintf(out, "\"");
// }
// fprintf(out, ";\n");
// }

//// États finaux (double ronds)
// if (!isempty_vertices(finonly))
// {
// fprintf(out, "node [style=solid,shape = doublecircle];");
// for (uint i = 0; i < size_vertices(finonly); i++)
// {
// fprintf(out, " \"");
// (*f)(A->names[lefread_vertices(finonly, i)], out);
// fprintf(out, "\"");
// }
// fprintf(out, ";\n");
// }

//// États initiaux et finaux en même temps
// if (!isempty_vertices(mixed))
// {
// fprintf(out, "node [fillcolor=\"blue:green\",style=filled,shape =
// doublecircle];");
// for (uint i = 0; i < size_vertices(mixed); i++)
// {
// fprintf(out, " \"");
// (*f)(A->names[lefread_vertices(mixed, i)], out);
// fprintf(out, "\"");
// }
// fprintf(out, ";\n");
// }
// delete_vertices(inionly);
// delete_vertices(finonly);
// delete_vertices(mixed);

// fprintf(out, "node [style=solid,shape = circle];\n");
// p_stack theedges;
// if (A->etrans == NULL && A->itrans == NULL)
// {
// theedges = lgraph_to_multi_edges(0, A->trans);
// }
// else if (A->itrans == NULL)
// {
// theedges = lgraph_to_multi_edges(1, A->trans, A->etrans);
// }
// else if (A->etrans == NULL)
// {
// theedges = lgraph_to_multi_edges(2, A->trans, A->itrans);
// }
// else
// {
// theedges = lgraph_to_multi_edges(3, A->trans, A->itrans, A->etrans);
// }
// named_lgedges_print(theedges, (void **)(A->names), f, out);
// fprintf(out, "}\n");
// }

/***************************/
/* Print d'un Cayley graph */
/***************************/

void cayley_print(p_cayley M, FILE *out)
{
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n");

    // Print des sommets
    for (uint i = 0; i < M->trans->size_graph; i++)
    {
        fprintf(out, " %d ", i);
        if (M->accept_array[i])
        {
            fprintf(out, "[style=solid,shape = doublecircle,label =");
        }
        else
        {
            fprintf(out, "[style=solid,shape = circle,label =");
        }

        cayley_print_state_label(M->names[i], out);
        fprintf(out, "];\n");
    }

    p_stack theedges = dgraph_to_multi_edges(M->trans);
    for (uint i = 0; i < size_stack(theedges); i++)
    { // Boucle sur les états de départ
        fprintf(out, " %d -> %d ", ((p_multi_edge)read_stack(theedges, i))->in,
                ((p_multi_edge)read_stack(theedges, i))->out);
        fprintf(out, "[label = <");
        for (uint j = 0; j < size_vertices(((p_multi_edge)read_stack(theedges, i))->lab) - 1; j++)
        {
            fprintf(out, "%c,",
                    lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab, j) + 'a');
        }
        fprintf(out, "%c>];\n",
                lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab,
                                 size_vertices(((p_multi_edge)read_stack(theedges,
                                                                         i))->lab) - 1) + 'a');
    }
    fprintf(out, "}\n");
}

// Print du graphe gauche
void cayley_left_print(p_cayley M, FILE *out)
{
    if (M->leftgraph == NULL)
    {
        compute_left_cayley(M);
    }
    fprintf(out, "digraph {\n");
    fprintf(out, "gradientangle=90\n");
    fprintf(out, "fontname=\"Helvetica,Arial,sans-serif\"\n");
    fprintf(out, "resolution= \"200.0,0.0\"");
    fprintf(out, "node [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");
    fprintf(out, "rankdir=LR;\n");
    fprintf(out, "node [style=solid,shape = doublecircle];");

    // Print des sommets
    for (uint i = 0; i < M->trans->size_graph; i++)
    {
        fprintf(out, " %d ", i);
        if (M->accept_array[i])
        {
            fprintf(out, "[style=solid,shape = doublecircle,label =");
        }
        else
        {
            fprintf(out, "[style=solid,shape = circle,label =");
        }

        cayley_print_state_label(M->names[i], out);
        fprintf(out, "];\n");
    }

    p_stack theedges = dgraph_to_multi_edges(M->leftgraph);
    for (uint i = 0; i < size_stack(theedges); i++)
    { // Boucle sur les états de départ
        fprintf(out, " %d -> %d ", ((p_multi_edge)read_stack(theedges, i))->in,
                ((p_multi_edge)read_stack(theedges, i))->out);
        fprintf(out, "[label = <");
        for (uint j = 0; j < size_vertices(((p_multi_edge)read_stack(theedges, i))->lab) - 1; j++)
        {
            fprintf(out, "%c,",
                    lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab, j) + 'a');
        }
        fprintf(out, "%c>];\n",
                lefread_vertices(((p_multi_edge)read_stack(theedges, i))->lab,
                                 size_vertices(((p_multi_edge)read_stack(theedges,
                                                                         i))->lab) - 1) + 'a');
    }
    fprintf(out, "}\n");

    // for (uint i = 0; i < size_vertices(M->accept_list); i++)
    // {
    // fprintf(out, " \"");
    // cayley_print_state(M->names[lefread_vertices(M->accept_list, i)], out);
    // fprintf(out, "\"");
    // }
    // fprintf(out, ";\n");

    // fprintf(out, "node [style=solid,shape = circle];\n");
    // named_dgedges_print(M->leftgraph, (void**)M->names, &cayley_print_state,
    // out);
    // fprintf(out, "}\n");
}

/**************************/
/* Affichage sur le shell */
/**************************/

// Affichage d'un NFA
void view_nfa(nfa *thenfa)
{
    char tmp_filename[] = "/tmp/nfa-XXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    char png_filename[1 + strlen(tmp_filename)];

    strcpy(png_filename, tmp_filename);
    strcpy(png_filename + strlen(tmp_filename) - 3, "png");

    FILE *f_tmp = fdopen(d, "w");

    nfa_print(thenfa, f_tmp);

    fclose(f_tmp);

    // char *command = multiple_strcat("dot -Tpng ", tmp_filename, "| ./imgcat
    // -W
    // auto ", NULL);
    char *command;

    if (!external_viewer)
    {
        command =
            multiple_strcat("dot -Tpng ", tmp_filename, "| ./imgcat -W auto", NULL);
    }
    else
    {
        command = multiple_strcat("dot -Tpng ", tmp_filename, " -o ", png_filename,
                                  "&& open ", png_filename, NULL);
    }
    printf("%s\n", command);
    system(command);

    free(command);
}

void view_cayley(cayley *thecayley)
{
    char tmp_filename[] = "/tmp/cay-XXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    // char png_filename[1 + strlen(tmp_filename)];

    // strcpy(png_filename, tmp_filename);
    // strcpy(png_filename + strlen(tmp_filename) - 3, "png");

    FILE *f_tmp = fdopen(d, "w");

    cayley_print(thecayley, f_tmp);

    fclose(f_tmp);

    char *command =
        multiple_strcat("dot -Tpng ", tmp_filename, "| ./imgcat -W auto ", NULL);
    // " -o ", png_filename,
    // " && open ", png_filename, NULL);
    system(command);
    free(command);
}

void view_left_cayley(cayley *thecayley)
{
    char tmp_filename[] = "/tmp/cay-XXX.dot";
    int d               = mkostemps(tmp_filename, 4, O_APPEND);
    // char png_filename[1 + strlen(tmp_filename)];

    // strcpy(png_filename, tmp_filename);
    // strcpy(png_filename + strlen(tmp_filename) - 3, "png");

    FILE *f_tmp = fdopen(d, "w");

    cayley_left_print(thecayley, f_tmp);

    fclose(f_tmp);

    char *command =
        multiple_strcat("dot -Tpng ", tmp_filename, "| ./imgcat -W auto ", NULL);

    // " -o ", png_filename, " && open ", png_filename,
    system(command);
    free(command);
}
