/***************************/
/* Implémentation des NFAs */
/***************************/

#include "nfa.h"

/***************************************************************/
/* Affichage des noms si ce sont des pointeurs sur des entiers */
/***************************************************************/

// Affiche le nom du ième état d'un NFA
void nfa_print_name(p_nfa A, uint i, FILE *out)
{
    switch (A->ntype)
    {
    case NUMBER:
        fprintf(out, "%d", ((uint *)A->names)[i]);
        break;
    case PAIR: {
        // printf("start\n");
        // printf("size: %d i: %d\n", A->trans->size_graph, i);
        // ((uint **)A->names)[i];
        fprintf(out, "(%d,%d)", ((uint **)A->names)[i][0], ((uint **)A->names)[i][1]);
        // printf("yo\n");
        break;
    }
    case TUPLE:
        fprintf(out, "(");
        for (uint j = 1; j < ((uint **)A->names)[i][0]; j++)
        {
            fprintf(out, "%d,", ((uint **)A->names)[i][j]);
        }
        fprintf(out, "%d)", ((uint **)A->names)[i][((uint **)A->names)[i][0]]);
        break;
    case SET:
        if (((uint **)A->names)[i][0] == 0)
        {
            fprintf(out, "∅");
        }
        else
        {
            fprintf(out, "{");
            for (uint j = 1; j < ((uint **)A->names)[i][0]; j++)
            {
                fprintf(out, "%d,", ((uint **)A->names)[i][j]);
            }
            fprintf(out, "%d}", ((uint **)A->names)[i][((uint **)A->names)[i][0]]);
        }
        break;
    default:
        fprintf(out, "%d", i);
    }
}

// Initialise la table des noms suivant le type choisit dans le NFA
void nfa_init_names(p_nfa A)
{
    switch (A->ntype)
    {
    case NONAME:
        A->names = NULL;
        break;
    case NUMBER: {
        uint *temp;
        MALLOC(temp, A->trans->size_graph);
        A->names = temp;
        break;
    }
    case PAIR:
    case TUPLE:
    case SET: {
        uint **ttemp;
        MALLOC(ttemp, A->trans->size_graph);
        A->names = ttemp;
        break;
    }
    default:
        return;
    }
}

// Copie le nom du jème état du NFA B dans le ième état du NFA A
// Les tables doivent avoit été initialisées et les types de noms
// doivent être compatibles entre A et B
void nfa_copy_name(p_nfa A, uint i, p_nfa B, uint j)
{
    switch (A->ntype)
    {
    case NONAME:
        printf("Error: Cannot copy a name in a NFA which is not using names.\n");
        return;
        break;
    case NUMBER:
        if (B->ntype == NONAME)
        {
            ((uint *)A->names)[i] = j;
            return;
        }
        else if (B->ntype == NUMBER)
        {
            ((uint *)A->names)[i] = ((uint *)B->names)[j];
            return;
        }
        else
        {
            printf("Error: Name types are incompatible.\n");
            return;
        }
        break;
    case PAIR:
        if (B->ntype == PAIR)
        {
            uint *pair;
            MALLOC(pair, 2);
            pair[0]                = ((uint **)B->names)[j][0];
            pair[1]                = ((uint **)B->names)[j][1];
            ((uint **)A->names)[i] = pair;
            return;
        }
        else
        {
            printf("Error: Name types are incompatible.\n");
            return;
        }
        break;
    case TUPLE:
        if (B->ntype == TUPLE)
        {
            uint *tuple;
            MALLOC(tuple, ((uint **)B->names)[j][0] + 1);
            tuple[0] = ((uint **)B->names)[j][0];
            for (uint k = 1; k <= tuple[0]; k++)
            {
                tuple[k] = ((uint **)B->names)[j][k];
            }
            ((uint **)A->names)[i] = tuple;
            return;
        }
        else
        {
            printf("Error: Name types are incompatible.\n");
            return;
        }
        break;
    case SET:
        if (B->ntype == SET)
        {
            uint *set;
            MALLOC(set, ((uint **)B->names)[j][0] + 1);
            set[0] = ((uint **)B->names)[j][0];
            for (uint k = 1; k <= set[0]; k++)
            {
                set[k] = ((uint **)B->names)[j][k];
            }
            ((uint **)A->names)[i] = set;
            return;
        }
        else
        {
            printf("Error: Name types are incompatible.\n");
            return;
        }
        break;
    default:
        printf("Error: Names can only be copied when the names types are compatible.\n");
        return;
    }
}

// Efface les noms des états d'un NFA
// (si les états ont des noms)
void nfa_delete_names(p_nfa A)
{
    switch (A->ntype)
    {
    case NUMBER:
        free(A->names);
        break;
    case PAIR:
    case TUPLE:
    case SET:
        for (uint i = 0; i < A->trans->size_graph; i++)
        {
            free(((uint **)A->names)[i]);
        }
        free(A->names);
        break;
    default:
        break;
    }
    A->ntype = NONAME;
    A->names = NULL;
}

/*********************************/
/* Initialisation et suppression */
/*********************************/

static p_graph create_dummy_eps(uint size)
{
    p_graph res = create_graph_noedges(size);
    for (uint q = 0; q < size; q++)
    { // Crée une self-loop epsilon sur chaque état (ignorée à l'affichage)
        lefins_vertices(q, res->edges[q]);
    }
    return res;
}

// Initialise (si besoin) le graphe des transitions epsilon
void init_epstrans(p_nfa A)
{
    if (A->etrans == NULL)
    {
        A->etrans = create_dummy_eps(A->trans->size_graph);
    }
}

// Crée un NFA qui reconnait le langage vide
p_nfa create_emptylang(uint size_alpha)
{
    if (size_alpha == 0)
    {
        printf("Error, an alphabet should contain at least one symbol.\n Returned a NULL pointer.");
        return NULL;
    }
    p_nfa A;
    MALLOC(A, 1);
    A->initials = create_vertices();
    A->finals   = create_vertices();
    A->trans    = create_lgraph_noedges(0, size_alpha);
    A->etrans   = NULL;
    A->itrans   = NULL;
    A->ntype    = NONAME;
    A->names    = NULL;
    return A;
}

// Créé un NFA qui reconnait le singleton {epsilon}
p_nfa create_sing_epsilon(uint size_alpha)
{
    if (size_alpha == 0)
    {
        printf("Error, an alphabet should contain at least one symbol.\n Returned a NULL pointer.");
        return NULL;
    }
    p_nfa A;
    MALLOC(A, 1);
    A->initials = create_vertices();
    lefins_vertices(0, A->initials);
    A->finals   = create_vertices();
    lefins_vertices(0, A->finals);
    A->trans    = create_lgraph_noedges(1, size_alpha);
    A->etrans   = NULL;
    A->itrans   = NULL;
    A->ntype    = NONAME;
    A->names    = NULL;
    return A;
}

// Créé un NFA qui reconnait le langage A^* de tous les mots
p_nfa create_freemonolang(uint size_alpha)
{
    if (size_alpha == 0)
    {
        printf("Error, an alphabet should contain at least one symbol.\n Returned a NULL pointer.");
        return NULL;
    }
    p_nfa A;
    MALLOC(A, 1);
    A->initials = create_vertices();
    lefins_vertices(0, A->initials);
    A->finals   = create_vertices();
    lefins_vertices(0, A->finals);
    A->trans    = create_lgraph_noedges(1, size_alpha);
    for (uint a = 0; a < size_alpha; a++)
    {
        rigins_vertices(0, A->trans->edges[0][a]);
    }
    A->etrans = NULL;
    A->itrans = NULL;
    A->names  = NULL;
    A->ntype  = NONAME;
    return A;
}

// Créé un NFA qui reconnait le langage A^+ de tous les mots non-vides
p_nfa create_freesemilang(uint size_alpha)
{
    if (size_alpha == 0)
    {
        printf("Error, an alphabet should contain at least one symbol.\n Returned a NULL pointer.");
        return NULL;
    }
    p_nfa A;
    MALLOC(A, 1);
    A->initials = create_vertices();
    lefins_vertices(0, A->initials);
    A->finals   = create_vertices();
    lefins_vertices(1, A->finals);
    A->trans    = create_lgraph_noedges(2, size_alpha);
    for (uint a = 0; a < size_alpha; a++)
    {
        rigins_vertices(1, A->trans->edges[0][a]);
        rigins_vertices(1, A->trans->edges[1][a]);
    }
    A->etrans = NULL;
    A->itrans = NULL;
    A->names  = NULL;
    A->ntype  = NONAME;
    return A;
}

// Création d'un automate qui reconnait un singleton dont le mot contient une
// seule lettre
p_nfa create_sing_letter(uint size_alpha, uint letter)
{
    if (size_alpha <= letter)
    {
        printf("Warning, not a valid letter for this alphabet.\n Returned a NULL pointer.\n");
        return NULL;
    }
    p_nfa A;
    MALLOC(A, 1);
    A->initials = create_vertices();
    lefins_vertices(0, A->initials);
    A->finals   = create_vertices();
    lefins_vertices(1, A->finals);
    A->trans    = create_lgraph_noedges(2, size_alpha);
    lefins_vertices(1, A->trans->edges[0][letter]);
    A->etrans   = NULL;
    A->itrans   = NULL;
    A->names    = NULL;
    A->ntype    = NONAME;
    return A;
}

// Création d'un automate qui reconnait un singleton
p_nfa create_sing(uint size_alpha, uint *word, uint length)
{
    for (uint i = 0; i < length; i++)
    {
        if (size_alpha <= word[i])
        {
            printf("Warning, not a valid word for this alphabet.\n Returned a NULL pointer.\n");
            return NULL;
        }
    }

    p_nfa A;
    MALLOC(A, 1);
    A->initials = create_vertices();
    lefins_vertices(0, A->initials);
    A->finals   = create_vertices();
    lefins_vertices(length, A->finals);
    A->trans    = create_lgraph_noedges(length + 1, size_alpha);
    for (uint i = 0; i < length; i++)
    {
        lefins_vertices(i + 1, A->trans->edges[i][word[i]]);
    }
    A->etrans = NULL;
    A->itrans = NULL;
    A->ntype  = NONAME;
    A->names  = NULL;
    return A;
}

//// Supprime les noms des états dans un NFA (si il y en a)
// void delete_names_nfa(p_nfa A)
// {
// if (A->names != NULL)
// {
// if (A->delete_name == NULL)
// {
// fprintf(stderr, "NFA error: the name deletion function should not be NULL
// when there are names\n");
// exit(EXIT_FAILURE);
// }
// for (uint q = 0; q < A->trans->size_graph; q++)
// {
// (*A->delete_name)(A->names[q]);
// }
// A->delete_name = NULL;
// A->print_name = NULL;
// A->names = NULL;
// A->copy_name = NULL;
// }
// }

// Désallocation d'un NFA (la fonction sert à supprimer la liste de noms)
void delete_nfa(p_nfa A)
{
    if (A == NULL)
    {
        return;
    }
    nfa_delete_names(A);
    delete_vertices(A->initials);
    delete_vertices(A->finals);
    delete_graph(A->etrans);
    delete_lgraph(A->trans);
    delete_lgraph(A->itrans);
    free(A);
}

// Recopie du NFA B sur le NFA A (les anciens éléments de A sont supprimés, B
// est libéré)
void overwrite_nfa(p_nfa A, p_nfa B)
{
    if (A == NULL || B == NULL)
    {
        return;
    }
    nfa_delete_names(A);
    A->ntype    = B->ntype;
    A->names    = B->names;
    delete_vertices(A->initials);
    A->initials = B->initials;
    delete_vertices(A->finals);
    A->finals   = B->finals;
    delete_graph(A->etrans);
    A->etrans   = B->etrans;
    delete_lgraph(A->trans);
    A->trans    = B->trans;
    delete_lgraph(A->itrans);
    A->itrans   = B->itrans;
    free(B);
}

// Génération d'un NFA aléatoire
p_nfa nfa_random(uint size_alpha, uint min_size, uint max_size)
{
    min_size = max(1, min_size);
    uint sizea = max(1, size_alpha);

    uint size  = min_size + (rand() % (1 + max_size - min_size));

    p_nfa A;
    MALLOC(A, 1);
    A->initials = create_vertices();
    rigins_vertices(0, A->initials);
    A->finals   = create_vertices();
    if (!(rand() % 4))
    {
        rigins_vertices(0, A->finals);
    }
    for (uint i = 1; i < size; i++)
    {
        if (!(rand() % 4))
        {
            rigins_vertices(i, A->finals);
        }
        if (!(rand() % 10))
        {
            rigins_vertices(i, A->initials);
        }
    }
    if (isempty_vertices(A->finals))
    {
        rigins_vertices(size - 1, A->finals);
    }
    A->trans = create_lgraph_noedges(size, sizea);
    for (uint q = 0; q < size; q++)
    {
        for (uint a = 0; a < sizea; a++)
        {
            if (rand() % 8 == 0)
            {
                rigins_vertices(q, A->trans->edges[q][a]);
            }

            uint count = 0;
            for (uint r = q + 1; r < size; r++)
            {
                if (rand() % (2 + count++) == 0)
                {
                    rigins_vertices(r, A->trans->edges[q][a]);
                }
            }
            for (uint r = 0; r < q; r++)
            {
                if (rand() % (2 + count++) == 0)
                {
                    rigins_vertices(r, A->trans->edges[q][a]);
                }
            }
        }
    }
    A->etrans = NULL;
    A->itrans = NULL;
    A->names  = NULL;
    A->ntype  = NONAME;
    return A;
}

// Génération d'un DFA aléatoire
p_nfa dfa_random(uint size_alpha, uint min_size, uint max_size)
{
    min_size = max(1, min_size);
    uint sizea = max(1, size_alpha);

    uint size  = min_size + (rand() % (1 + max_size - min_size));

    p_nfa A;
    MALLOC(A, 1);
    A->initials = create_vertices();
    rigins_vertices(0, A->initials);
    A->finals   = create_vertices();
    int cf = 0;
    for (int i = size - 1; i >= 0; i--)
    {
        if (!(rand() % (2 + cf++)))
        {
            rigins_vertices(i, A->finals);
        }
    }
    if (isempty_vertices(A->finals))
    {
        rigins_vertices(size - 1, A->finals);
    }
    A->trans = create_lgraph_noedges(size, sizea);
    for (uint q = 0; q < size; q++)
    {
        for (uint a = 0; a < sizea; a++)
        {
            uint count = 0;
            uint r     = (q + 1) % size;
            ;
            while (count == 0)
            {
                if (r >= q && rand() % 2 == 0)
                {
                    rigins_vertices(r, A->trans->edges[q][a]);
                    count = 1;
                }
                else if (rand() % 8 == 0)
                {
                    rigins_vertices(r, A->trans->edges[q][a]);
                    count = 1;
                }
                else
                {
                    r = (r + 1) % size;
                }
            }
        }
    }
    A->etrans = NULL;
    A->itrans = NULL;
    A->names  = NULL;
    A->ntype  = NONAME;
    return A;
}

/***********************************/
/* Opérations simples sur les NFAs */
/***********************************/

// Copie d'un NFA
p_nfa nfa_copy(p_nfa A)
{
    p_nfa B;
    MALLOC(B, 1);

    // Copie du graphe des transitions.
    B->trans = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
    for (uint q = 0; q < B->trans->size_graph; q++)
    {
        for (uint a = 0; a < B->trans->size_alpha; a++)
        {
            for (uint i = 0; i < size_vertices(A->trans->edges[q][a]); i++)
            {
                rigins_vertices(lefread_vertices(A->trans->edges[q][a], i), B->trans->edges[q][a]);
            }
        }
    }
    // Initiaux
    B->initials = create_vertices();
    for (uint i = 0; i < size_vertices(A->initials); i++)
    {
        rigins_vertices(lefread_vertices(A->initials, i), B->initials);
    }

    // Finals
    B->finals = create_vertices();
    for (uint i = 0; i < size_vertices(A->finals); i++)
    {
        rigins_vertices(lefread_vertices(A->finals, i), B->finals);
    }

    // Pas de nom dans une copie
    B->ntype = NONAME;
    B->names = NULL;

    // Copie des transitions epsilon si nécessaire
    if (A->etrans)
    {
        B->etrans = create_graph_noedges(A->etrans->size);
        for (uint q = 0; q < B->etrans->size; q++)
        {
            for (uint i = 0; i < size_vertices(A->etrans->edges[q]); i++)
            {
                rigins_vertices(lefread_vertices(A->etrans->edges[q], i), B->etrans->edges[q]);
            }
        }
    }

    // Copie des transitions inverses si nécessaire
    if (A->itrans)
    {
        B->itrans = create_lgraph_noedges(A->trans->size_graph, A->itrans->size_alpha);
        for (uint q = 0; q < B->itrans->size_graph; q++)
        {
            for (uint a = 0; a < B->itrans->size_alpha; a++)
            {
                for (uint i = 0; i < size_vertices(A->itrans->edges[q][a]); i++)
                {
                    rigins_vertices(lefread_vertices(A->itrans->edges[q][a], i),
                                    B->itrans->edges[q][a]);
                }
            }
        }
    }

    return B;
}

// Union disjointe d'un nombre arbitraire de NFAs
p_nfa nfa_union_gen(uint n, ...)
{
    // Création de l'automate
    p_nfa UNION;
    MALLOC(UNION, 1);
    UNION->itrans = NULL; // L'union ne prend pas en charge les transitions
                          // inverses
    UNION->ntype  = NONAME;
    UNION->names  = NULL; // Pas de noms pour l'union

    // Récupération des inputs
    va_list list;
    va_start(list, n);
    p_nfa tab[n];
    p_lgraph lgtab[n];
    bool eps = false;
    for (uint i = 0; i < n; i++)
    {
        tab[i]   = va_arg(list, p_nfa);
        lgtab[i] = tab[i]->trans;
        if (tab[i]->etrans != NULL)
        {
            eps = true;
        }
    }

    // Création du nouveau graphe des transitions
    uint lag[n];
    UNION->trans = merge_lgraphs(lag, n, lgtab);

    // Création de l'éventuel nouveau graph des transitions epsilon
    if (eps)
    {
        bool neweps[n];
        p_graph epstab[n];
        for (uint i = 0; i < n; i++)
        {
            if (tab[i]->etrans)
            {
                epstab[i] = tab[i]->etrans;
                neweps[i] = false;
            }
            else
            {
                epstab[i] = create_dummy_eps(tab[i]->trans->size_graph);
                neweps[i] = true;
            }
        }
        // printf("Print 4\n");
        UNION->etrans = merge_graphs(lag, n, epstab);
        for (uint i = 0; i < n; i++)
        {
            if (neweps[i])
            {
                delete_graph(epstab[i]);
            }
        }
    }
    else
    {
        UNION->etrans = NULL;
    }
    // Création des listes d'états finaux et initiaux
    UNION->initials = create_vertices();
    UNION->finals   = create_vertices();
    for (uint i = 0; i < n; i++)
    {
        copy_vertices_right(UNION->initials, tab[i]->initials, lag[i]);
        copy_vertices_right(UNION->finals, tab[i]->finals, lag[i]);
    }
    return UNION;
}

// Union disjointe de deux nfas
p_nfa nfa_union(p_nfa A, p_nfa B)
{
    return nfa_union_gen(2, A, B);
}

// Concaténation d'un nombre arbitraire de NFAs
p_nfa nfa_concat_gen(uint n, ...)
{
    // Récupération des inputs en alternant avec de nouveaux noeuds
    // intermédiaires pour les concaténations
    // Initialisation simultanée des transitions epsilon (si nécessaire)
    va_list list;
    va_start(list, n);
    p_nfa tab[n];
    p_lgraph transtab[2 * n - 1];
    p_graph etranstab[2 * n - 1];
    bool neweps[n];
    for (uint i = 0; i < n - 1; i++)
    {
        tab[i]          = va_arg(list, p_nfa);
        transtab[2 * i] = tab[i]->trans;
        if (tab[i]->etrans)
        {
            etranstab[2 * i] = tab[i]->etrans;
            neweps[i]        = false;
        }
        else
        {
            etranstab[2 * i] = create_dummy_eps(tab[i]->trans->size_graph);
            neweps[i]        = true;
        }
        // init_epstrans(tab[i]);
        // etranstab[2 * i] = tab[i]->etrans;
        transtab[2 * i + 1]  = create_lgraph_noedges(1, 1);
        etranstab[2 * i + 1] = create_graph_noedges(1);
    }

    tab[n - 1] = va_arg(list, p_nfa);
    if (tab[n - 1]->etrans)
    {
        etranstab[2 * (n - 1)] = tab[n - 1]->etrans;
        neweps[n - 1]          = false;
    }
    else
    {
        etranstab[2 * (n - 1)] = create_dummy_eps(tab[(n - 1)]->trans->size_graph);
        neweps[n - 1]          = true;
    }

    transtab[2 * (n - 1)] = tab[n - 1]->trans;

    // Création du nouveau NFA
    p_nfa CONCAT;
    MALLOC(CONCAT, 1);

    CONCAT->itrans = NULL; // La concaténation ne prend pas en charge les
                           // transitions inverses
    CONCAT->ntype  = NONAME;
    CONCAT->names  = NULL; // Pas de noms pour la concaténation

    uint lag[2 * n - 1];
    CONCAT->trans  = merge_lgraphs(lag, 2 * n - 1, transtab);
    CONCAT->etrans = merge_graphs(lag, 2 * n - 1, etranstab);

    // Création des listes d'états finaux et initiaux
    CONCAT->initials = create_vertices();
    copy_vertices_right(CONCAT->initials, tab[0]->initials, 0);
    CONCAT->finals   = create_vertices();
    copy_vertices_right(CONCAT->finals, tab[n - 1]->finals, lag[2 * (n - 1)]);

    // Création des transitions epsilon
    for (uint j = 0; j < n - 1; j++)
    {
        for (uint i = 0; i < size_vertices(tab[j]->finals); i++)
        {
            // Pour chaque état final du premier automate
            // On insère une transi epsilon vers l'état intermédiaire
            // (à la fin pour préserver l'ordre)
            rigins_vertices(lag[2 * j + 1],
                            CONCAT->etrans->edges[lefread_vertices(tab[j]->finals,
                                                                   i) + lag[2 * j]]);
        }
        for (uint i = 0; i < size_vertices(tab[j + 1]->initials); i++)
        {
            // Pour chaque état initial du second automate
            // On insère une transi epsilon depuis l'état intermédiaire
            // (au début pour préserver l'ordre)
            lefins_vertices(lag[2 * (j + 1)] + lefread_vertices(tab[j + 1]->initials, i),
                            CONCAT->etrans->edges[lag[2 * j + 1]]);
        }
    }
    for (uint i = 0; i < n - 1; i++)
    {
        if (neweps[i])
        {
            delete_graph(etranstab[2 * i]);
        }
        delete_graph(etranstab[2 * i + 1]);

        delete_lgraph(transtab[2 * i + 1]);
    }

    if (neweps[n - 1])
    {
        delete_graph(etranstab[2 * (n - 1)]);
    }

    return CONCAT;
}

// Concaténation de deux nfas
p_nfa nfa_concat(p_nfa A, p_nfa B)
{
    return nfa_concat_gen(2, A, B);
}

// Etoile de Kleene d'un nfa
p_nfa nfa_star(p_nfa A)
{
    // Tableaux pour la fonction de fusion
    p_graph etranstab[2];
    p_lgraph transtab[2];

    // Noeud intermédiaire pour relier le NFA avec des transitions epsilon
    etranstab[0] = create_graph_noedges(1);
    lefins_vertices(0, etranstab[0]->edges[0]);
    transtab[0]  = create_lgraph_noedges(1, 1);

    // Automate input
    if (A->etrans)
    {
        etranstab[1] = A->etrans;
    }
    else
    {
        etranstab[1] = create_dummy_eps(A->trans->size_graph);
    }
    transtab[1] = A->trans;

    // Création de l'automate
    p_nfa STAR;
    MALLOC(STAR, 1);
    uint lag[2];
    STAR->trans  = merge_lgraphs(lag, 2, transtab);
    STAR->etrans = merge_graphs(lag, 2, etranstab);

    delete_graph(etranstab[0]);
    delete_lgraph(transtab[0]);
    if (!A->etrans)
    {
        delete_graph(etranstab[1]);
    }

    STAR->itrans = NULL; // L'étoile ne prend pas en charge les transitions
                         // inverses
    STAR->ntype  = NONAME;
    STAR->names  = NULL; // L'étoile de noms pour la concaténation

    // Création des listes d'états finaux et initiaux
    STAR->initials = create_vertices();
    lefins_vertices(0, STAR->initials);
    STAR->finals   = create_vertices();
    lefins_vertices(0, STAR->finals);

    // printf("Size: %d\n", STAR->etrans->size);
    // printf("Size finalsa: %d\n", size_vertices(A->finals));
    // Création des transitions epsilon
    for (uint i = 0; i < size_vertices(A->finals); i++)
    {
        // printf("test: %d\n", rigread_vertices(A->finals, i));

        // Pour chaque état final de A on insère une transition vers le nouvel
        // état 0
        lefins_vertices(0, STAR->etrans->edges[1 + rigread_vertices(A->finals, i)]);
    }

    for (uint i = 0; i < size_vertices(A->initials); i++)
    {
        // Pour chaque état initial de A on va insérer une transition depuis le
        // nouvel état 0
        rigins_vertices(1 + lefread_vertices(A->initials, i), STAR->etrans->edges[0]);
    }

    return STAR;
}

// Etoile de Kleene stricte (+) d'un nfa
p_nfa nfa_star_plus(p_nfa A)
{
    // Tableaux pour la fonction de fusion
    p_graph etranstab[2];
    p_lgraph transtab[2];

    // Noeud intermédiaire pour relier le NFA avec des transitions epsilon
    etranstab[0] = create_graph_noedges(1);
    lefins_vertices(0, etranstab[0]->edges[0]);
    transtab[0]  = create_lgraph_noedges(1, 1);

    // Automate input
    if (A->etrans)
    {
        etranstab[1] = A->etrans;
    }
    else
    {
        etranstab[1] = create_dummy_eps(A->trans->size_graph);
    }
    transtab[1] = A->trans;

    // Création de l'automate
    p_nfa STAR;
    MALLOC(STAR, 1);

    STAR->itrans = NULL; // L'étoile ne prend pas en charge les transitions
                         // inverses
    STAR->ntype  = NONAME;
    STAR->names  = NULL; // L'étoile de noms pour la concaténation

    uint lag[2];
    STAR->trans  = merge_lgraphs(lag, 2, transtab);
    STAR->etrans = merge_graphs(lag, 2, etranstab);

    delete_graph(etranstab[0]);
    delete_lgraph(transtab[0]);
    if (!A->etrans)
    {
        delete_graph(etranstab[1]);
    }

    // Création des listes d'états finaux et initiaux
    STAR->initials = create_vertices();
    lefins_vertices(0, STAR->initials);
    STAR->finals   = create_vertices();
    copy_vertices_right(STAR->finals, A->finals, 1); // Les états finaux sont
                                                     // ceux de A ( étoile
                                                     // stricte)

    // printf("Size: %d\n", STAR->etrans->size);
    // printf("Size finalsa: %d\n", size_vertices(A->finals));
    // Création des transitions epsilon
    for (uint i = 0; i < size_vertices(A->finals); i++)
    {
        // printf("test: %d\n", rigread_vertices(A->finals, i));

        // Pour chaque état final de A on insère une transition vers le nouvel
        // état 0
        lefins_vertices(0, STAR->etrans->edges[1 + rigread_vertices(A->finals, i)]);
    }

    for (uint i = 0; i < size_vertices(A->initials); i++)
    {
        // Pour chaque état initial de A on va insérer une transition depuis le
        // nouvel état 0
        rigins_vertices(1 + lefread_vertices(A->initials, i), STAR->etrans->edges[0]);
    }
    return STAR;
}

// Élimination des états non-accessibles et non-co-accessibles
// Le NFA produit n'est pas nécessairement complet
p_nfa nfa_trim(p_nfa A)
{
    // Un graphe non-étiqueté contenant toutes les transitions possibles
    p_graph simple;
    if (A->etrans == NULL && A->itrans == NULL)
    {
        simple = ldgraphs_to_graph(0, 1, 0, 1, A->trans);
    }
    else if (A->itrans == NULL)
    {
        simple = ldgraphs_to_graph(1, 1, 0, 2, A->etrans, A->trans);
    }
    else if (A->etrans == NULL)
    {
        simple = ldgraphs_to_graph(0, 2, 0, 2, A->trans, A->itrans);
    }
    else
    {
        simple = ldgraphs_to_graph(1, 2, 0, 3, A->etrans, A->trans, A->itrans);
    }
    make_tclos_graph(simple);

    // Calcul des composantes fortements connexes
    p_parti PSCC = tarjan(simple);

    // Calcul du DAG des sccs
    p_graph GSCC = compute_dag_of_sccs(simple, PSCC);

    /*  Calcul des états accessibles */

    // Tableau pour mémoriser les sccs accessibles
    bool acc[GSCC->size];
    for (uint c = 0; c < GSCC->size; c++)
    {
        acc[c] = false;
    }

    // Enregistrement des états initiaux comme accessibles
    for (uint i = 0; i < size_vertices(A->initials); i++)
    {
        uint c = PSCC->numcl[lefread_vertices(A->initials, i)];
        acc[c] = true;
    }

    // Parcours: on viste les classes dans l'ordre du tri topologique
    for (uint c = 0; c < PSCC->size_par; c++)
    {
        if (acc[c])
        {
            for (uint i = 0; i < size_vertices(GSCC->edges[c]); i++)
            {
                acc[lefread_vertices(GSCC->edges[c], i)] = true;
            }
        }
    }

    /* Restriction aux états co-accessibles */

    // Tableau pour mémoriser les sccs co-accessibles ET accessibles

    bool coacc[GSCC->size];
    for (uint c = 0; c < GSCC->size; c++)
    {
        coacc[c] = false;
    }

    // Enregistrement des états finaux qui sont accessibles
    for (uint i = 0; i < size_vertices(A->finals); i++)
    {
        uint c = PSCC->numcl[lefread_vertices(A->finals, i)];
        if (acc[c])
        {
            coacc[c] = true;
        }
    }

    // Parcours: on viste les classes dans l'ordre inverse du tri topologique
    for (uint c = PSCC->size_par; c > 0; c--)
    {
        // printf("num: %d\n", c);
        if (acc[c - 1] && !coacc[c - 1]) // Si une scc est accessible et n'a pas
                                         // été déjà marqué co-accessible à
                                         // l'initialisation
        {
            uint i = 0;
            while (!coacc[c - 1] && i < size_vertices(GSCC->edges[c - 1]))
            {
                if (coacc[lefread_vertices(GSCC->edges[c - 1], i)])
                {
                    coacc[c - 1] = true;
                }
                i++;
            }
        }
    }

    /* Construction de l'automate élagué */
    p_nfa B;
    MALLOC(B, 1);

    // Numérotation des états. L'ordre respecte celui de l'automate original
    uint numbers[A->trans->size_graph]; // Mémorise l'association entre les
                                        // anciens états et les nouveaux
    uint num = 0;                       // Compteur
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (coacc[PSCC->numcl[q]])
        {
            // printf("q: %d num: %d\n", q, num);
            numbers[q] = num;
            num++;
        }
    }

    // Création des nouveaux ensembles

    // Les états initiaux sont tous ceux qui sont coaccessibles dans l'automate
    // orignal (modulo renommage)
    B->initials = create_vertices();
    for (uint i = 0; i < size_vertices(A->initials); i++)
    {
        if (coacc[PSCC->numcl[lefread_vertices(A->initials, i)]])
        {
            rigins_vertices(numbers[lefread_vertices(A->initials, i)], B->initials);
        }
    }

    // Les états finaux sont tous ceux qui sont accessibles dans l'automate
    // orignal (modulo renommage)
    B->finals = create_vertices();
    for (uint i = 0; i < size_vertices(A->finals); i++)
    {
        if (acc[PSCC->numcl[lefread_vertices(A->finals, i)]])
        {
            rigins_vertices(numbers[lefread_vertices(A->finals, i)], B->finals);
        }
    }

    // Création des transitions classiques
    B->trans = create_lgraph_noedges(num, A->trans->size_alpha);
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        if (coacc[PSCC->numcl[q]])                                                      //
                                                                                        //
                                                                                        // Si
                                                                                        //
                                                                                        // l'état
                                                                                        //
                                                                                        // est
                                                                                        //
                                                                                        // utile
        {
            for (uint a = 0; a < A->trans->size_alpha; a++)
            {
                for (uint i = 0; i < size_vertices(A->trans->edges[q][a]); i++)
                {
                    if (coacc[PSCC->numcl[lefread_vertices(A->trans->edges[q][a], i)]]) //
                                                                                        //
                                                                                        // Si
                                                                                        //
                                                                                        // l'état
                                                                                        //
                                                                                        // adjacent
                                                                                        //
                                                                                        // est
                                                                                        //
                                                                                        // utile
                                                                                        //
                                                                                        // aussi
                    {
                        rigins_vertices(numbers[lefread_vertices(A->trans->edges[q][a], i)],
                                        B->trans->edges[numbers[q]][a]);
                    }
                }
            }
        }
    }

    // Création des transitions epsilon
    B->etrans = NULL;
    if (A->etrans != NULL)
    {
        init_epstrans(B);
        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            if (coacc[PSCC->numcl[q]])
            {
                for (uint i = 0; i < size_vertices(A->etrans->edges[q]); i++)
                {
                    if (coacc[PSCC->numcl[lefread_vertices(A->etrans->edges[q], i)]]) //
                                                                                      //
                                                                                      // Si
                                                                                      //
                                                                                      // l'état
                                                                                      //
                                                                                      // adjacent
                                                                                      //
                                                                                      // est
                                                                                      //
                                                                                      // utile
                                                                                      //
                                                                                      // aussi
                    {
                        rigins_vertices(numbers[lefread_vertices(A->etrans->edges[q], i)],
                                        B->etrans->edges[numbers[q]]);
                    }
                }
            }
        }
    }

    // Création des transitions inverses
    B->itrans = NULL;
    if (A->itrans != NULL)
    {
        B->itrans = create_lgraph_noedges(num, A->trans->size_alpha);
        for (uint q = 0; q < A->itrans->size_graph; q++)
        {
            if (coacc[PSCC->numcl[q]])
            {
                for (uint a = 0; a < A->itrans->size_alpha; a++)
                {
                    for (uint i = 0; i < size_vertices(A->itrans->edges[q][a]); i++)
                    {
                        if (coacc[PSCC->numcl[lefread_vertices(A->itrans->edges[q][a], i)]]) //
                                                                                             //
                                                                                             // Si
                                                                                             //
                                                                                             // l'état
                                                                                             //
                                                                                             // adjacent
                                                                                             //
                                                                                             // est
                                                                                             //
                                                                                             // utile
                                                                                             //
                                                                                             // aussi
                        {
                            rigins_vertices(numbers[lefread_vertices(A->itrans->edges[q][a], i)],
                                            B->itrans->edges[numbers[q]][a]);
                        }
                    }
                }
            }
        }
    }

    // Gestion des noms: on réutilise les noms des états de A
    if (A->ntype == NONAME)
    {
        B->ntype = NUMBER;
    }
    else
    {
        B->ntype = A->ntype;
    }

    nfa_init_names(B);
    for (uint q = 0; q < A->trans->size_graph; q++) // Pour chaque état de A
    {
        if (coacc[PSCC->numcl[q]])                  // Si cet état est gardé
                                                    // dans le nouvel automate
        {
            // Le nom de q devient celui du nouvel état correspondant dans B
            nfa_copy_name(B, numbers[q], A, q);
        }
    }

    // Suppression de la partition en SCCs (plus nécessaire)
    delete_parti(PSCC);
    delete_graph(GSCC);

    return B;
}

// Élimination des états non-accessibles (modifie le NFA originel)
void nfa_trim_mod(p_nfa A)
{
    p_nfa B = nfa_trim(A);
    overwrite_nfa(A, B);
}

// Élimination des transitions epsilon
p_nfa nfa_elimeps(p_nfa A)
{
    if (A->etrans == NULL)
    {
        // Si il n'y a pas de transitions epsilon
        return nfa_copy(A);
    }

    // Calcul du graphe des SCCs des transitions epsilon
    p_parti sccs = tarjan(A->etrans);

    // On fusionne les composantes fortement connexes de sccs de transitions
    // epsilon
    // Par construction, les nouveaux états du DAG (des trans eps) sont ordonnés
    // selon un tri topologique
    p_nfa B = nfa_merge_states(A, sccs);

    // Clôture transitive des transitions epsilon dans B
    for (uint q = B->etrans->size; q > 0; q--)
    {
        for (uint j = 0; j < size_vertices(B->etrans->edges[q - 1]); j++)
        {
            merge_sorted_vertices(B->etrans->edges[q - 1],
                                  B->etrans->edges[lefread_vertices(B->etrans->edges[q - 1], j)]);
        }
    }

    // Enregistrement des nouveaux états finaux
    bool tempfin[B->trans->size_graph]; // Tableau temporaire pour marquer les
                                        // états finaux
    for (uint q = 0; q < B->trans->size_graph; q++)
    {
        tempfin[q] = false;
    }
    for (uint i = 0; i < size_vertices(B->finals); i++)
    {
        tempfin[lefread_vertices(B->finals, i)] = true;
    }
    for (uint q = 0; q < B->trans->size_graph; q++)
    {
        uint i = 0;
        while (!tempfin[q] && i < size_vertices(B->etrans->edges[q]))
        {
            if (tempfin[lefread_vertices(B->etrans->edges[q], i)])
            {
                tempfin[q] = true;
            }
            i++;
        }
    }
    makeempty_vertices(B->finals); // On vide l'ancienne liste des états finaux
    for (uint q = 0; q < B->trans->size_graph; q++)
    {
        if (tempfin[q])
        {
            rigins_vertices(q, B->finals);
        }
    }

    // Nouvelles transitions
    for (uint q = 0; q < B->trans->size_graph; q++)
    {
        for (uint i = 0; i < size_vertices(B->etrans->edges[q]); i++)
        {
            if (q != lefread_vertices(B->etrans->edges[q], i))
            {
                // Transition classiques
                for (uint a = 0; a < B->trans->size_alpha; a++)
                {
                    merge_sorted_vertices(B->trans->edges[q][a],
                                          B->trans->edges[lefread_vertices(B->etrans->edges[q],
                                                                           i)][a]);
                }

                // Transition inverses
                if (B->itrans != NULL)
                {
                    for (uint a = 0; a < B->itrans->size_alpha; a++)
                    {
                        merge_sorted_vertices(B->itrans->edges[q][a],
                                              B->itrans->edges[lefread_vertices(B->etrans->edges[q],
                                                                                i)][a]);
                    }
                }
            }
        }
    }
    // Libération des epsilons transitions
    delete_graph(B->etrans);
    B->etrans = NULL;

    nfa_trim_mod(B);
    return B;
}

// Élimination des transitions epsilon (modifie le NFA originel)
void nfa_elimeps_mod(p_nfa A)
{
    if (A->etrans == NULL)
    {
        return;
    }
    p_nfa B = nfa_elimeps(A);
    overwrite_nfa(A, B);
}

// Miroir
p_nfa nfa_mirror(p_nfa A)
{
    // Création du miroir
    p_nfa themirror;
    MALLOC(themirror, 1);
    themirror->ntype = NONAME;
    themirror->names = NULL;

    // Création des états initiaux et finaux (swap de l'automate d'origine)
    themirror->initials = create_vertices();
    themirror->finals   = create_vertices();
    copy_vertices_right(themirror->initials, A->finals, 0);
    copy_vertices_right(themirror->finals, A->initials, 0);

    // Création du graph des transitions
    themirror->trans = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
    for (uint q = 0; q < A->trans->size_graph; q++)
    { // On affecte les transitions
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            for (uint i = 0; i < size_vertices(A->trans->edges[q][a]); i++)
            {
                insert_vertices(themirror->trans->edges[lefread_vertices(A->trans->edges[q][a],
                                                                         i)][a], q);
            }
        }
    }

    // Création des transitions epsilon (si besoin)
    if (A->etrans != NULL)
    {
        init_epstrans(themirror);

        for (uint q = 0; q < A->etrans->size; q++)
        { // On affecte les transitions
            for (uint i = 0; i < size_vertices(A->etrans->edges[q]); i++)
            {
                insert_vertices(themirror->etrans->edges[lefread_vertices(A->etrans->edges[q], i)],
                                q);
            }
        }
    }

    // Création des transitions inverses (si besoin)
    if (A->itrans != NULL)
    {
        themirror->itrans = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
        for (uint q = 0; q < A->trans->size_graph; q++)
        { // On affecte les transitions
            for (uint a = 0; a < A->trans->size_alpha; a++)
            {
                for (uint i = 0; i < size_vertices(A->itrans->edges[q][a]); i++)
                {
                    insert_vertices(themirror->itrans->edges[lefread_vertices(
                                                                 A->itrans->edges[q][a], i)][a], q);
                }
            }
        }
    }

    return themirror;
}

/***********************/
/* Information sur nfa */
/***********************/

// Calcule le nombre de transitions dans un preautomate
int nfa_nb_trans(p_nfa A)
{
    uint nb = 0;
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; q < A->trans->size_alpha; a++)
        {
            nb = nb + size_vertices(A->trans->edges[q][a]);
        }
    }
    if (A->itrans != NULL)
    {
        for (uint q = 0; q < A->itrans->size_graph; q++)
        {
            for (uint a = 0; q < A->itrans->size_alpha; a++)
            {
                nb = nb + size_vertices(A->itrans->edges[q][a]);
            }
        }
    }
    if (A->etrans != NULL)
    {
        for (uint q = 0; q < A->etrans->size; q++)
        {
            nb = nb + size_vertices(A->etrans->edges[q]);
        }
    }
    return nb;
}

// Teste si un nfa est déterministe
bool nfa_is_det(p_nfa A)
{
    if (size_vertices(A->initials) > 1 || A->etrans != NULL)
    {
        return false;
    }
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            if (size_vertices(A->trans->edges[q][a]) > 1)
            {
                return false;
            }
        }
    }
    if (A->itrans != NULL)
    {
        for (uint q = 0; q < A->itrans->size_graph; q++)
        {
            for (uint a = 0; a < A->itrans->size_alpha; a++)
            {
                if (size_vertices(A->itrans->edges[q][a]) > 1)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

// Teste si un nfa est complet
bool nfa_is_comp(p_nfa A)
{
    if (isempty_vertices(A->initials))
    {
        return false;
    }
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            if (isempty_vertices(A->trans->edges[q][a]))
            {
                return false;
            }
        }
    }
    if (A->itrans != NULL)
    {
        for (uint q = 0; q < A->itrans->size_graph; q++)
        {
            for (uint a = 0; a < A->itrans->size_alpha; a++)
            {
                if (isempty_vertices(A->itrans->edges[q][a]))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

// Teste si un nfa reconnait le langage vide
bool nfa_is_empty(p_nfa A)
{
    p_nfa B  = nfa_trim(A);
    // printf("Size: %d\n", A->trans->size_graph);
    bool res = isempty_vertices(B->finals);
    delete_nfa(B);
    return res;
}

// Teste si un mot est accepté par un NFA
bool nfa_accepts(p_nfa A, char *word)
{
    if (isempty_vertices(A->initials) || isempty_vertices(A->finals))
    {
        return false;
    }

    // Intialisation de l'ensemble d'états pour le parcours
    p_vertices states = create_vertices();
    copy_vertices_right(states, A->initials, 0);
    // print_dequeue(states);

    // Lecture du mot
    uint i          = 0;
    p_vertices temp = create_vertices();
    while (word[i] != '\0')
    {
        uint letter = word[i] - 'a';
        if (A->trans->size_alpha <= letter)
        {
            printf("Error, the word contains a non-valid letter for this automaton\n");
            return false;
        }

        // On retire chaque état de départ dans l'ensemble courant
        // et on insère les états atteignables en lisant la lettre
        while (!isempty_vertices(states))
        {
            int q = lefpull_vertices(states);
            merge_sorted_vertices(temp, A->trans->edges[q][letter]);
        }
        // print_dequeue(temp);
        if (isempty_vertices(temp))
        {
            // Si aucun état n'était atteignable
            delete_vertices(temp);
            delete_vertices(states);
            return false;
        }
        copy_vertices_right(states, temp, 0);
        makeempty_vertices(temp);
        i++;
        // printf("%c\n", letter + 'a');
        // print_dequeue(states);
    }

    delete_vertices(temp);

    // On teste si on a atteint un état final
    uint j = 0;
    for (i = 0; i < size_vertices(states); i++)
    {
        if (j >= size_vertices(A->finals))
        {
            return false;
        }
        else if (lefread_vertices(states, i) == lefread_vertices(A->initials, i))
        {
            return true;
        }
        else if (lefread_vertices(states, i) > lefread_vertices(A->initials, i))
        {
            j++;
        }
    }
    return false;
}

// Calcule les états qui sont atteints par un mot dans un NFA.
p_vertices nfa_compute_runs(p_nfa A, char *word)
{
    p_vertices states = create_vertices();

    if (isempty_vertices(A->initials))
    {
        return states;
    }

    copy_vertices_right(states, A->initials, 0);

    // Lecture du mot
    uint i          = 0;
    p_vertices temp = create_vertices();
    while (word[i] != '\0')
    {
        uint letter = word[i] - 'a';
        if (word[i] < 'a' || A->trans->size_alpha <= letter)
        {
            delete_vertices(temp);
            delete_vertices(states);
            return NULL;
        }

        // On retire chaque état de départ dans l'ensemble courant
        // et on insère les états atteignables en lisant la lettre
        while (!isempty_vertices(states))
        {
            int q = lefpull_vertices(states);
            merge_sorted_vertices(temp, A->trans->edges[q][letter]);
        }
        copy_vertices_right(states, temp, 0);

        if (isempty_vertices(temp))
        {
            // Si aucun état n'était atteignable
            delete_vertices(temp);
            return states;
        }

        makeempty_vertices(temp);
        i++;
        // printf("%c\n", letter + 'a');
        // print_dequeue(states);
    }

    delete_vertices(temp);
    return states;
}

// Retourne l'état atteint à partir de s en lisant le mot w dans le DFA A
// (le déterminisme n'est pas vérifier)
uint dfa_function(p_nfa A, uint s, p_vertices w)
{
    uint e = s;
    for (uint i = 0; i < size_vertices(w); i++)
    {
        e = lefread_vertices(A->trans->edges[e][lefread_vertices(w, i)], 0);
    }
    return e;
}

// Teste si un DFA est commutatif
bool dfa_is_comm(p_nfa A, FILE *out)
{
    if (!nfa_is_det(A))
    {
        if (out)
        {
            fprintf(out, "#### Error: the commutativity test is only meaningful for DFAs.\n");
        }
        return false;
    }
    if (out)
    {
        fprintf(out, "#### Checking if the DFA is commutative.\n");
    }
    for (uint a = 0; a < A->trans->size_alpha; a++)
    {
        for (uint b = a + 1; b < A->trans->size_alpha; b++)
        {
            for (uint q = 0; q < A->trans->size_graph; q++)
            {
                uint r = lefread_vertices(A->trans->edges[lefread_vertices(A->trans->edges[q][b],
                                                                           0)][a], 0);
                uint s = lefread_vertices(A->trans->edges[lefread_vertices(A->trans->edges[q][a],
                                                                           0)][b], 0);
                if (r != s)
                {
                    if (out)
                    {
                        fprintf(out, "#### The DFA is NOT commutative.\n");
                        fprintf(out, "#### For instance, (");
                        nfa_print_name(A, q, out);
                        fprintf(out, ",%c%c) ⟶ ", a + 'a', b + 'a');
                        nfa_print_name(A, s, out);
                        fprintf(out, " and (");
                        nfa_print_name(A, q, out);
                        fprintf(out, ",%c%c) ⟶ ", b + 'a', a + 'a');
                        nfa_print_name(A, r, out);
                        fprintf(out, ".\n");
                    }
                    return false;
                }
            }
        }
    }
    if (out)
    {
        fprintf(out, "#### The DFA is commutative.\n");
    }
    return true;
}

/***********************/
/* Partition d'un NFA  */
/***********************/

// Fusion des états d'un NFA selon une partition prise en entrée
// Les nouveaux états sont les classes
// On a une transition (c,a,c') si les classes c,c' contiennent des états q,q'
// tels que l'automate d'origine avait une transition (q,a,q')
// De la même façon, une classe est c est initiale (resp. finale) si elle
// contient
// un état initial (resp. final).
p_nfa nfa_merge_states(p_nfa A, p_parti P)
{
    // Création de l'automate fusionné
    p_nfa B;
    MALLOC(B, 1);

    // Création des listes d'états initiaux et finaux
    B->initials = create_vertices();
    p_uint_avlnode initree = NULL;
    for (uint i = 0; i < size_vertices(A->initials); i++)
    {
        initree = uint_avl_insert(P->numcl[lefread_vertices(A->initials, i)], initree);
    }
    uint_avl_to_vertices(initree, B->initials);

    B->finals = create_vertices();
    p_uint_avlnode fintree = NULL;
    for (uint i = 0; i < size_vertices(A->finals); i++)
    {
        fintree = uint_avl_insert(P->numcl[lefread_vertices(A->finals, i)], fintree);
    }
    uint_avl_to_vertices(fintree, B->finals);

    // Création du graphe des transition classiques
    B->trans = create_lgraph_noedges(P->size_par, A->trans->size_alpha);
    for (uint c = 0; c < P->size_par; c++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            p_uint_avlnode thetree = NULL;
            for (uint i = 0; i < size_vertices(P->cl[c]); i++)
            {
                for (uint j = 0;
                     j < size_vertices(A->trans->edges[lefread_vertices(P->cl[c], i)][a]); j++)
                {
                    thetree =
                        uint_avl_insert(P->numcl[lefread_vertices(A->trans->edges[lefread_vertices(
                                                                                      P->cl[c],
                                                                                      i)][a], j)],
                                        thetree);
                }
            }
            uint_avl_to_vertices(thetree, B->trans->edges[c][a]);
        }
    }

    // Création du graphe des transition inverses (si besoin)
    if (A->itrans != NULL)
    {
        B->itrans = create_lgraph_noedges(P->size_par, A->itrans->size_alpha);
        for (uint c = 0; c < P->size_par; c++)
        {
            for (uint a = 0; a < A->itrans->size_alpha; a++)
            {
                p_uint_avlnode thetree = NULL;
                for (uint i = 0; i < size_vertices(P->cl[c]); i++)
                {
                    for (uint j = 0;
                         j < size_vertices(A->itrans->edges[lefread_vertices(P->cl[c], i)][a]); j++)
                    {
                        thetree =
                            uint_avl_insert(P->numcl[lefread_vertices(
                                                         A->itrans->edges[lefread_vertices(P->cl[c],
                                                                                           i)][a],
                                                         j)], thetree);
                    }
                }
                uint_avl_to_vertices(thetree, B->itrans->edges[c][a]);
            }
        }
    }
    else
    {
        B->itrans = NULL;
    }

    // Création du graphe des transitions epsilon (si besoin)
    if (A->etrans != NULL)
    {
        B->etrans = create_graph_noedges(P->size_par);
        for (uint c = 0; c < P->size_par; c++)
        {
            p_uint_avlnode thetree = NULL;
            for (uint i = 0; i < size_vertices(P->cl[c]); i++)
            {
                for (uint j = 0; j < size_vertices(A->etrans->edges[lefread_vertices(P->cl[c], i)]);
                     j++)
                {
                    thetree =
                        uint_avl_insert(P->numcl[lefread_vertices(A->etrans->edges[lefread_vertices(
                                                                                       P->cl[c],
                                                                                       i)], j)],
                                        thetree);
                }
            }
            uint_avl_to_vertices(thetree, B->etrans->edges[c]);
        }
    }
    else
    {
        B->etrans = NULL;
    }

    // Gestion des noms: Chaque état reprend le nom d'un représentant de sa
    // classe
    if (A->ntype == NONAME)
    {
        B->ntype = NUMBER;
    }
    else
    {
        B->ntype = A->ntype;
    }

    nfa_init_names(B);
    for (uint q = 0; q < P->size_par; q++)
    {
        nfa_copy_name(B, q, A, lefread_vertices(P->cl[q], 0));
    }

    return B;
}

// Récupération du graphe étiqueté obtenu en ne gardant que
// les transitions internes à une partition prise en entrée
// (typiquement utilisé pour la partition en SCCs)
p_lgraph nfa_get_lab_parti(p_nfa A, p_parti P)
{
    // Calcul du graphe
    p_lgraph G = create_lgraph_noedges(A->trans->size_graph, A->trans->size_alpha);
    for (uint q = 0; q < G->size_graph; q++)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {
            for (uint i = 0; i < size_vertices(A->trans->edges[q][a]); i++)
            {
                if (P->numcl[q] == P->numcl[lefread_vertices(A->trans->edges[q][a], i)])
                {
                    rigins_vertices(lefread_vertices(A->trans->edges[q][a], i), G->edges[q][a]);
                }
            }
        }
    }
    return G;
}

// Génération d'un nfa à partir d'un lgraph et d'un unique état initial
// Seuls les états accessibles sont conservés. Si l'automate n'est pas
// complet, celui-ci est complété en ajoutant un état supplémentaire.
p_nfa lgraph_to_nfa(p_lgraph G, uint ini)
{
    // La pile pour le parcours en profondeur
    p_vertices thestack = create_vertices();

    // Un tableau pour mémoriser les états recontrés
    bool visited[G->size_graph];
    for (uint q = 0; q < G->size_graph; q++)
    {
        visited[q] = false;
    }

    // Un Booléen pour mémoriser si l'automate est complet
    bool is_complete = true;

    // Un compteur qui compte le nombre d'états rencontrés
    uint num = 0;

    // Empilement de l'état initial
    rigins_vertices(ini, thestack);
    visited[ini] = true;
    num++;

    while (!isempty_vertices(thestack))
    {
        uint state = rigpull_vertices(thestack);
        for (uint a = 0; a < G->size_alpha; a++)
        {
            if (isempty_vertices(G->edges[state][a]))
            {
                is_complete = false;
            }
            else
            {
                for (uint i = 0; i < size_vertices(G->edges[state][a]); i++)
                {
                    if (!visited[lefread_vertices(G->edges[state][a], i)])
                    {
                        rigins_vertices(lefread_vertices(G->edges[state][a], i), thestack);
                        num++;
                        visited[lefread_vertices(G->edges[state][a], i)] = true;
                    }
                }
            }
        }
    }

    // Calcul de la taille de l'automate
    uint size_auto = num;
    if (!is_complete)
    { // Si l'automate n'était pas complet, on rajoute un état
        size_auto++;
    }

    // Création de l'automate
    p_nfa A;
    MALLOC(A, 1);
    A->trans    = create_lgraph_noedges(size_auto, G->size_alpha);
    A->initials = create_vertices();
    A->finals   = create_vertices();
    A->ntype    = NUMBER;
    A->etrans   = NULL;
    A->itrans   = NULL;

    // Calcul de l'association entre les nouveaux états et les anciens

    uint *ATOG;
    MALLOC(ATOG, size_auto);
    A->names = ATOG;

    uint GTOA[G->size_graph];
    uint t = 0;
    for (uint q = 0; q < G->size_graph; q++)
    {
        if (visited[q])
        {
            GTOA[q] = t;
            ATOG[t] = q;
            if (q == ini)
            {
                rigins_vertices(t, A->initials);
            }
            t++;
        }
    }

    // Affectation des transtions
    for (uint s = 0; s < num; s++)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {
            if (isempty_vertices(G->edges[ATOG[s]][a]))
            {
                rigins_vertices(num, A->trans->edges[s][a]);
            }
            else
            {
                for (uint i = 0; i < size_vertices(G->edges[ATOG[s]][a]); i++)
                {
                    rigins_vertices(GTOA[lefread_vertices(G->edges[ATOG[s]][a], i)],
                                    A->trans->edges[s][a]);
                }
            }
        }
    }
    if (!is_complete)
    {
        for (uint a = 0; a < G->size_alpha; a++)
        {
            rigins_vertices(num, A->trans->edges[num][a]);
        }
    }

    return A;
}
