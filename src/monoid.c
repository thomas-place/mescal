#include "monoid.h"

/*****************************************/
/* Fonctions génériques pour l'affichage */
/*****************************************/

static uint cayley_get_name_length(p_cayley M, uint q)
{
    if (isempty_vertices(M->names[q]))
    {
        // La longueur du neutre est 1 (chaine "1")
        return 1;
    }
    // Sinon
    // Un entier pour la longueur
    uint l = 0;
    // Un entier temporaire pour stocker le nombre de copies consécutives d'une
    // même lettre
    uint num;
    for (uint i = 0; i < size_vertices(M->names[q]); i++)
    {
        // printf("%d\n", i);
        // Si la lettre lue est distincte de la précédente
        if (i == 0 || lefread_vertices(M->names[q], i - 1) != lefread_vertices(M->names[q], i))
        {
            uint d = 0;
            // On compte le nombre de chiffres du nombre de copies de la lettre
            // précédente
            if (num > 1)
            {
                while (num != 0)
                {
                    d++;
                    num = num / 10;
                }
            }
            // La longueur est augmentée de ce nombre de chiffre plus la
            // nouvelle lettre lue
            l   = l + d + 1;
            // On a pour l'instant une copie de la nouvelle lettre
            num = 1;
        }
        else
        {
            // On a lu une copie de plus
            num++;
        }
    }

    uint d = 0;
    // On compte le nombre de chiffres du nombre de copies de la lettre
    // précédente
    if (num > 1)
    {
        while (num != 0)
        {
            d++;
            num = num / 10;
        }
    }
    l = l + d;
    return l;
}

// Retourne une chaîne de caractères correspondant au nom d'un élément
// char* cayley_get_name(p_cayley M, uint q) {
// char* res;
// if (isempty_vertices(M->names[q])) {
// MALLOC(res, 2);
// res[0] = '1';
// res[1] = '\0';
// }
// else {
// MALLOC(res, size_vertices(M->names[q]) + 1);
// for (uint i = 0; i < size_vertices(M->names[q]); i++) {
// res[i] = lefread_vertices(M->names[q], i) + 'a';
// }
// res[size_vertices(M->names[q])] = '\0';
// }
// return res;
// }

// Retourne une chaîne de caractères correspondant au nom d'un élément
// (factorisation des puissances)
char *cayley_get_name(p_cayley M, uint q)
{
    char *res;
    MALLOC(res, NAMELEN);
    if (isempty_vertices(M->names[q]))
    {
        res[0] = '1';
        res[1] = '\0';
    }
    else
    {
        char let[] = "a";
        let[0] = lefread_vertices(M->names[q], 0) + 'a';
        strcpy(res, let);

        // Un entier temporaire pour stocker le nombre de copies consécutives
        // d'une même lettre
        uint num = 1;

        for (uint i = 1; i < size_vertices(M->names[q]); i++)
        {
            // Si la lettre lue est distincte de la précédente
            if (lefread_vertices(M->names[q], i - 1) != lefread_vertices(M->names[q], i))
            {
                if (num > 1)
                {
                    append_power(num, res);
                }
                let[0] = 'a' + lefread_vertices(M->names[q], i);
                strcat(res, let);
                // On a pour l'instant une copie de la nouvelle lettre
                num = 1;
            }
            else
            {
                // On a lu une copie de plus
                num++;
            }
        }
        if (num > 1)
        {
            append_power(num, res);
        }
    }
    REALLOC(res, strlen(res) + 1);
    return res;
}

//// Affichage du nom de l'élément q
// void cayley_print_name(p_cayley M, uint q, FILE* out) {

// if (isempty_vertices(M->names[q])) {
// fprintf(out, "1");
// }
// else {
// for (uint i = 0; i < size_vertices(M->names[q]); i++) {
// fprintf(out, "%c", lefread_vertices(M->names[q], i) + 'a');
// }
// }
// }

//// Affichage du nom de l'élément q
// uint cayley_print_name(p_cayley M, uint q, FILE* out) {

// char* thename = cayley_get_name(M, q);
// fprintf(out, "%s", thename);
// free(thename);
// }

// Affichage du nom de l'élément q
uint cayley_print_name(p_cayley M, uint q, FILE *out)
{
    if (isempty_vertices(M->names[q]))
    {
        fprintf(out, "1");
        return 1;
    }
    else
    {
        uint n   = 1;
        fprintf(out, "%c", lefread_vertices(M->names[q], 0) + 'a');
        uint len = 1;
        for (uint i = 1; i < size_vertices(M->names[q]); i++)
        {
            if (lefread_vertices(M->names[q], i) != lefread_vertices(M->names[q], i - 1))
            {
                if (n > 1)
                {
                    print_power(n, out);
                    while (n != 0)
                    {
                        len++;
                        n = n / 10;
                    }
                }
                fprintf(out, "%c", lefread_vertices(M->names[q], i) + 'a');
                len++;
                n = 1;
            }
            else
            {
                n++;
            }
        }
        if (n > 1)
        {
            print_power(n, out);
            while (n != 0)
            {
                len++;
                n = n / 10;
            }
        }
        return len;
    }
}

//// Affichage du nom de l'élément q, version alignée
//// On utilise exactement size caractère en ajoutant des espaces (size doit
// être
//// suffisament grand)
// void cayley_print_name_aligned(p_cayley M, uint q, uint size_max, FILE* out)
// {
// if (isempty_vertices(M->names[q])) {
// fprintf(out, "1");
// print_spaces(size_max - 1, out);
// }
// else {
// for (uint i = 0; i < size_vertices(M->names[q]); i++) {

// fprintf(out, "%c", lefread_vertices(M->names[q], i) + 'a');
// }
// print_spaces(size_max - size_vertices(M->names[q]), out);
// }
// }

// Affichage du nom de l'élément q, version alignée
// On utilise exactement size caractères en ajoutant des espaces (size doit être
// suffisament grand)
void cayley_print_name_aligned(p_cayley M, uint q, uint size_max, FILE *out)
{
    uint len = cayley_print_name(M, q, out);
    print_spaces(size_max - len, out);
}

//// Récupération de la longueur maximale d'un nom (à utiliser avec la fonction
//// précédente)
// uint cayley_max_name_size(p_cayley M, p_vertices sub) {
// uint size = 1;

// if (sub == NULL) {

// for (uint q = 0; q < M->trans->size_graph; q++) {
// size = max(size, size_vertices(M->names[q]));
// }
// return size;
// }
// else {
// for (uint i = 0; i < size_vertices(sub); i++) {
// size = max(size, size_vertices(M->names[lefread_vertices(sub, i)]));
// }
// return size;
// }
// }

// Récupération de la longueur maximale d'un nom (à utiliser avec la fonction
// précédente)
uint cayley_max_name_size(p_cayley M, p_vertices sub)
{
    uint size = 1;

    if (sub == NULL)
    {
        for (uint q = 0; q < M->trans->size_graph; q++)
        {
            size = max(size, cayley_get_name_length(M, q));
        }
        return size;
    }
    else
    {
        for (uint i = 0; i < size_vertices(sub); i++)
        {
            size = max(size, cayley_get_name_length(M, lefread_vertices(sub, i)));
        }
        return size;
    }
}

// Affichage d'un sous ensemble d'éléments dans un cayley graph
void print_sub_cayley(p_cayley M, p_vertices elems, FILE *out)
{
    for (uint i = 0; i < size_vertices(elems); i++)
    {
        cayley_print_name(M, lefread_vertices(elems, i), out);
        if (i < size_vertices(elems) - 1)
        {
            fprintf(out, ", ");
        }
    }
    fprintf(out, "\n");
}

// Affichage d'un sous ensemble d'éléments dans un cayley graph
// Version alignement automatique
void print_sub_cayley_aligned(p_cayley M, p_vertices elems, uint length,
                              uint padding, FILE *out)
{
    uint max_size = length - padding;
    uint use_size = 0;
    for (uint i = 0; i < size_vertices(elems); i++)
    {
        uint e         = lefread_vertices(elems, i);
        uint size_elem = cayley_get_name_length(M, e); // size_vertices(M->names[e]);
        if (size_elem == 0)                            // Si on a à faire au mot
                                                       // vide, on l'affichera
                                                       // comme 1
        {
            size_elem++;
        }
        if (size_elem + use_size + 2 > max_size ||
            (i == size_vertices(elems) - 1 && size_elem + use_size > max_size))
        {
            for (uint j = 0; j < max_size - use_size; j++)
            {
                fprintf(out, " ");
            }
            fprintf(out, "│\n│");
            for (uint j = 0; j < padding; j++)
            {
                fprintf(out, " ");
            }
            use_size = 0;
        }
        cayley_print_name(M, e, out);
        // if (e == 0)
        // {
        //// Si l'élément est le neutre
        // fprintf(out, "1");
        // }
        // else
        // {
        // for (uint j = 0; j < size_elem; j++)
        // {

        // fprintf(out, "%c", lefread_vertices(M->names[e], j) + 'a');
        // }
        // }
        use_size = use_size + size_elem;
        if (i < size_vertices(elems) - 1)
        {
            fprintf(out, ", ");
            use_size = use_size + 2;
        }
    }
    print_spaces(max_size - use_size, out);
    fprintf(out, "│\n");
}

// Affichage d'un sous-ensemble d'éléments dans un cayley graph
// Version avec un titre (utilise la fonction précédente)
void print_sub_cayley_titled(p_cayley M, p_vertices elems, uint length,
                             char *name, FILE *out)
{
    print_title_box(length, false, out, 1, name);
    fprintf(out, "│");

    // Affichage du sous-ensemble
    print_sub_cayley_aligned(M, elems, length, 0, out);

    print_bot_line(length, out);
}

// Affichage des images de toutes les lettres
void cayley_print_morphism(p_cayley M, FILE *out)
{
    for (uint a = 0; a < M->trans->size_alpha; a++)
    {
        fprintf(out, "    %c ⟼ ", a + 'a');
        cayley_print_name(M, M->trans->edges[ONE][a], out);
        fprintf(out, "\n");
    }
}

/**********************************/
/* Construction à partir d'un DFA */
/**********************************/

// Représentation d'un état du graphe de Cayley construit à partir d'un DFA
// complet et des transitions qui partent de celui-ci
struct cayley_state {
    // Nombre d'états du DFA utilisé dans la construction
    uint size_graph;

    // Un état: une permutation de {0,...,size_auto-1}
    uint *state;

    // Le nom de l'état
    p_vertices name;

    // La taille de l'alphabet
    uint size_alpha;

    // Le numéro de l'état dans le future graphe de Cayley
    uint num;

    // Un tableau de taille size_alph contenant les transitions depuis l'état
    // pour chaque lettre
    struct cayley_state **next;
};

typedef struct cayley_state *p_cayley_state;

// Fonction de libération
static void free_cayley_state(void *p)
{
    if (p == NULL)
    {
        return;
    }
    free(((p_cayley_state)p)->state); // Suppression de la permutation
    free(((p_cayley_state)p)->next);  // Suppression de la table des transitions
    free(p);
}

// La fonction de comparaison de deux objets de type p_cayley_state (utilise la
// précédente) Pour les AVLs
static int comp_cayley_state(void *p1, void *p2)
{
    if (((p_cayley_state)p1)->size_graph != ((p_cayley_state)p2)->size_graph)
    {
        fprintf(stderr, "Error, the cayley states should have the same size\n");
        exit(EXIT_FAILURE);
    }
    for (uint i = 0; i < ((p_cayley_state)p1)->size_graph; i++)
    {
        if (((p_cayley_state)p1)->state[i] < ((p_cayley_state)p2)->state[i])
        {
            return 1;
        }
        if (((p_cayley_state)p1)->state[i] > ((p_cayley_state)p2)->state[i])
        {
            return -1;
        }
    }
    return 0;
}

// Fonction auxiliaire
static void cayley_avl_to_table(p_avlnode tree, p_nfa A, p_cayley thegraph,
                                bool *newfinals, bool *theidems)
{
    if (tree == NULL)
    {
        return;
    }
    else
    {
        cayley_avl_to_table(tree->left, A, thegraph, newfinals, theidems);
        cayley_avl_to_table(tree->right, A, thegraph, newfinals, theidems);
        p_cayley_state thestate = (p_cayley_state)tree->value;

        // Affectation des transitions
        for (uint a = 0; a < thegraph->trans->size_alpha; a++)
        {
            thegraph->trans->edges[thestate->num][a] = thestate->next[a]->num;
        }

        // Est-ce un élément de l'ensemble acceptant ?
        if (mem_vertices_sorted(thestate->state[0], A->finals))
        {
            newfinals[thestate->num] = true;
        }
        else
        {
            newfinals[thestate->num] = false;
        }

        // Est-ce un idempotent ?
        theidems[thestate->num] = true;
        uint q = 0;
        while (theidems[thestate->num] && q < thestate->size_graph)
        {
            if (thestate->state[q] != thestate->state[thestate->state[q]])
            {
                theidems[thestate->num] = false;
            }
            else
            {
                q++;
            }
        }
        // Enregistrement des noms
        thegraph->names[thestate->num] = thestate->name;
    }
}

// Ne fonctionne qu'avec un DFA complet
p_cayley dfa_to_right_cayley(p_nfa A)
{
    if (!nfa_is_det(A) || !nfa_is_comp(A))
    {
        printf("Error: The construction of the transition morphism requires a "
               "Complete DFA. Returned NULL\n");
        return NULL;
    }

    // Pile qui contiendra les éléments à traiter
    p_dequeue thequeue = create_dequeue();

    // Un avl qui contiendra les éléments déjà construits
    p_avlnode thetree = NULL;

    // Compteur du nombre d'éléments construits
    uint num = 0;

    // Empilement du premier élément (le neutre)
    p_cayley_state neutral;
    MALLOC(neutral, 1);
    neutral->num        = num;
    num++;
    neutral->name       = create_vertices();       // Le nom de cet élément est
                                                   // le mot vide
    neutral->size_alpha = A->trans->size_alpha;
    neutral->size_graph = A->trans->size_graph;
    neutral->next       = NULL;                    // Les élément suivants ne
                                                   // sont pas encore calculés
    MALLOC(neutral->state, neutral->size_graph);
    for (uint q = 0; q < neutral->size_graph; q++) // Cet élément est l'identité
    {
        neutral->state[q] = q;
    }

    thetree = avl_insert(neutral, thetree,
                         &comp_cayley_state);      // Insertion dans l'ensemble
                                                   // des
    // éléments déjà rencontrés
    leftinsert(neutral, thequeue);                 // Cet élément est à traiter

    while (!isempty_dequeue(thequeue))
    {
        // Récupération de l'élément qu'on va traiter
        p_cayley_state theelem = (p_cayley_state)rightpull(thequeue);

        // Calcul des transitions à partir de notre élément
        MALLOC(theelem->next, theelem->size_alpha);
        for (uint a = 0; a < theelem->size_alpha; a++)
        {
            // On crée le (potentiel) nouvel élément obtenu en multipliant a à
            // droite
            p_cayley_state new;
            MALLOC(new, 1);
            new->size_graph = A->trans->size_graph;
            MALLOC(new->state, new->size_graph);
            for (uint q = 0; q < neutral->size_graph;
                 q++)                                  // Calcul de la
                                                       // permutation
                                                       // correspondante
            {
                new->state[q] =
                    lefread_vertices(A->trans->edges[theelem->state[q]][a], 0);
            }

            // On regarde si cet état a en fait déjà été construit
            p_avlnode thenode = avl_search(new, thetree, &comp_cayley_state);

            if (thenode == NULL)                       // Si cet état n'a pas
                                                       // encore été construit
            // printf("NULL\n");
            {
                new->size_alpha = A->trans->size_alpha;
                new->next       = NULL;                // On ne connaît pas
                                                       // encore les élément
                                                       // suivants
                new->num        = num;
                num++;
                new->name       = create_vertices();   // Création du nom à
                                                       // partir de celui de
                // l'élément précédent
                copy_vertices_right(new->name, theelem->name, 0);
                rigins_vertices(a, new->name);
                theelem->next[a] =
                    new;                               // On l'affecte comme
                                                       // successeur pour la
                                                       // lettre a
                thetree          = avl_insert(
                    new, thetree, &comp_cayley_state); // On le mémorise dans
                                                       // l'ensemble
                // des éléments rencontrés
                leftinsert(new, thequeue);             // Ce nouvel état élément
                                                       // est à traiter
            }
            else                                       // Sinon, cet état a déjà
                                                       // été construit
            // printf("not NULL\n");
            {
                theelem->next[a] =
                    ((p_cayley_state)
                     thenode->value);                  // On affecte la version
                                                       // déjà construite comme
                // successeur pour la lettre a
                free(new->state);                      // On supprime la copie
                                                       // qu'on vient de créer
                free(new);
            }
        }
    }
    delete_dequeue(thequeue);

    // Construction du Cayley graph
    p_cayley M;
    MALLOC(M, 1);
    M->trans =
        create_dgraph_noedges(num, A->trans->size_alpha); // Création du graph
    MALLOC(M->names, num);                                // Noms des éléments
    M->mult      = NULL;                                  // Initialement, on ne
                                                          // crée pas la table
                                                          // de multiplication
    M->leftgraph = NULL;                                  // Initialement, on ne
                                                          // crée pas le graphe
                                                          // gauche
    M->order     = NULL;                                  // Initialement, pas
                                                          // d'ordre calculé
    MALLOC(M->accept_array, num);                         // Tableau de
                                                          // l'ensemble
                                                          // acceptant
    MALLOC(M->idem_array, num);                           // Tableau des
                                                          // idempotents

    // Création des transitions, des idempotents et détection des éléments
    // acceptants
    cayley_avl_to_table(thetree, A, M, M->accept_array, M->idem_array);

    // Libération de l'AVL
    avl_free_strong(thetree, &free_cayley_state);

    // Création des listes d'idempotents et d'éléments acceptant
    M->accept_list = create_vertices();
    M->idem_list   = create_vertices();

    for (uint i = 0; i < num; i++)
    {
        if (M->accept_array[i])
        {
            // printf("Test: %d\n", i);
            rigins_vertices(i, M->accept_list);
        }
        if (M->idem_array[i])
        {
            rigins_vertices(i, M->idem_list);
        }
    }

    // Affectation de l'éventuelle lettre neutre
    M->neutlet = 0;
    for (uint a = 0; a < M->trans->size_alpha; a++)
    {
        if (M->trans->edges[ONE][a] == ONE)
        {
            M->neutlet = a + 'a';
            return M;
        }
    }
    return M;
}

// Conversion d'un cayley en DFA
p_nfa cayley_to_dfa(p_cayley M)
{
    p_nfa A;
    MALLOC(A, 1);
    A->trans    = dgraph_to_lgraph(M->trans);
    A->etrans   = NULL;
    A->itrans   = NULL;
    A->names    = NULL;

    A->initials = create_vertices();
    rigins_vertices(0, A->initials);
    A->finals   = create_vertices();
    copy_vertices_right(A->finals, M->accept_list, 0);
    return A;
}

// Conversion d'un cayley gauche en DFA
p_nfa left_cayley_to_dfa(p_cayley M)
{
    compute_left_cayley(M);
    p_nfa A;
    MALLOC(A, 1);
    A->trans    = dgraph_to_lgraph(M->leftgraph);
    A->etrans   = NULL;
    A->itrans   = NULL;
    A->names    = NULL;

    A->initials = create_vertices();
    rigins_vertices(0, A->initials);
    A->finals   = create_vertices();
    copy_vertices_right(A->finals, M->accept_list, 0);
    return A;
}

// Suppression d'un cayley

void free_cayley(p_cayley M)
{
    /* Champs obligatoires */

    // Libération de la table des noms
    for (uint i = 0; i < M->trans->size_graph; i++)
    {
        delete_vertices(M->names[i]);
    }

    // Libération des éléments acceptants
    free(M->accept_array);
    delete_vertices(M->accept_list);

    // Libération des idempotents
    free(M->idem_array);
    delete_vertices(M->idem_list);

    /* Champs optionnels */

    // Libération du graph gauche (si besoin)
    if (M->leftgraph != NULL)
    {
        delete_dgraph(M->leftgraph);
    }

    // Libération de la table de multiplication (si besoin)
    if (M->mult != NULL)
    {
        for (uint i = 0; i < M->trans->size_graph; i++)
        {
            free(M->mult[i]);
        }
        free(M->mult);
    }

    // Libération de l'ordre (si besoin)
    if (M->order != NULL)
    {
        for (uint i = 0; i < M->trans->size_graph; i++)
        {
            delete_vertices(M->order[i]);
        }
    }

    // Libération du graphe
    delete_dgraph(M->trans);
    free(M);
}

/************************************************/
/* Calculs des informations sur un Cayley graph */
/************************************************/

// Calcule le graph de Cayley gauche
void compute_left_cayley(p_cayley M)
{
    if (M->leftgraph != NULL)                        // Si le calcul a déjà été
                                                     // fait
    {
        return;
    }
    M->leftgraph = create_dgraph_noedges(
        M->trans->size_graph, M->trans->size_alpha); // Création du graph
    p_vertices fromlet = create_vertices();
    p_vertices fromone = create_vertices();
    bool visited[M->trans->size_graph];
    for (uint a = 0; a < M->trans->size_alpha; a++)
    {
        makeempty_vertices(fromlet);
        makeempty_vertices(fromone);
        for (uint s = 0; s < M->trans->size_graph; s++)
        {
            visited[s] = false;
        }
        rigins_vertices(ONE, fromone);
        rigins_vertices(M->trans->edges[ONE][a], fromlet);
        visited[ONE] = true;

        while (!isempty_vertices(fromone))
        {
            uint s  = rigpull_vertices(fromone);
            uint as = rigpull_vertices(fromlet);
            M->leftgraph->edges[s][a] = as;
            for (uint b = 0; b < M->trans->size_alpha; b++)
            {
                if (!visited[M->trans->edges[s][b]])
                {
                    rigins_vertices(M->trans->edges[s][b], fromone);
                    rigins_vertices(M->trans->edges[as][b], fromlet);
                    visited[M->trans->edges[s][b]] = true;
                }
            }
        }
    }
    delete_vertices(fromlet);
    delete_vertices(fromone);
}

// Calcule la table de multiplication d'un Cayley graph
void compute_mult(p_cayley M)
{
    if (M->mult != NULL) // Si la table a déjà été calculée
    {
        return;
    }
    // Création du tableau
    MALLOC(M->mult, M->trans->size_graph);
    p_vertices fromcur = create_vertices();
    p_vertices fromone = create_vertices();
    bool visited[M->trans->size_graph];
    // Pour chaque élément
    for (uint q = 0; q < M->trans->size_graph; q++)
    {
        // Création du tableau des multiplications de q
        MALLOC(M->mult[q], M->trans->size_graph);

        // Initialisation des strucutures
        makeempty_vertices(fromcur);
        makeempty_vertices(fromone);
        for (uint s = 0; s < M->trans->size_graph; s++)
        {
            visited[s] = false;
        }

        rigins_vertices(ONE, fromone);
        rigins_vertices(q, fromcur);
        visited[ONE] = true;

        while (!isempty_vertices(fromone))
        {
            uint s  = rigpull_vertices(fromone);
            uint qs = rigpull_vertices(fromcur);
            M->mult[q][s] = qs;
            for (uint a = 0; a < M->trans->size_alpha; a++)
            {
                if (!visited[M->trans->edges[s][a]])
                {
                    rigins_vertices(M->trans->edges[s][a], fromone);
                    rigins_vertices(M->trans->edges[qs][a], fromcur);
                    visited[M->trans->edges[s][a]] = true;
                }
            }
        }
    }
    delete_vertices(fromcur);
    delete_vertices(fromone);
}

// Calcule le mirroir du graphe de Cayley droit
p_lgraph cayley_rmirror(p_cayley M)
{
    p_lgraph themirror =
        create_lgraph_noedges(M->trans->size_graph, M->trans->size_alpha);

    for (uint q = 0; q < themirror->size_graph; q++)
    {
        for (uint a = 0; a < themirror->size_alpha; a++)
        {
            insert_vertices(themirror->edges[M->trans->edges[q][a]][a], q);
        }
    }
    return themirror;
}

// Calcule le mirroir du graphe de Cayley gauche
p_lgraph cayley_lmirror(p_cayley M)
{
    compute_left_cayley(M); // Calcule le graph gauche si nécessaire
    p_lgraph themirror =
        create_lgraph_noedges(M->trans->size_graph, M->trans->size_alpha);

    for (uint q = 0; q < themirror->size_graph; q++)
    {
        for (uint a = 0; a < themirror->size_alpha; a++)
        {
            insert_vertices(themirror->edges[M->leftgraph->edges[q][a]][a], q);
        }
    }
    return themirror;
}

// Calcule l'ordre syntaxique

// Retourne la valeur du ième bit de mem (en partant de la droite)
// 1 <= pos <= 8
static uchar get_nth_bit(uchar mem, uchar pos)
{
    mem = mem >> (pos - 1);
    return mem & 1;
}

// Modifier la valeur du ième bit de mem (en partant de la droite)
// 1 <= pos <= 8, 0 <= val <= 1
static void set_nth_bit(uchar *mem, uchar val, uchar pos)
{
    uchar mask = 1 << (pos - 1);
    if (val == 0)
    {
        mask = ~mask;
        *mem = *mem & mask;
    }
    else
    {
        *mem = *mem | mask;
    }
}

// Calcule de l'ordre syntaxique complet
void compute_syntac_order(p_cayley M)
{
    if (M->order != NULL)
    {
        return;
    }
    uint thesize = M->trans->size_graph;

    // On utilisera les 4 premiers bits à droite pour
    // coder les informations nécessaires au parcours
    // uchar thepairs[thesize][thesize];

    uchar **thepairs;
    MALLOC(thepairs, thesize);

    // On commence par tout initialiser à 00000011 (c'est-à-dire 3)
    for (uint q = 0; q < M->trans->size_graph; q++)
    {
        MALLOC(thepairs[q], thesize);
        for (uint r = 0; r < M->trans->size_graph; r++)
        {
            thepairs[q][r] = 3;
        }
    }

    // Stacks for the two DFSs
    p_vertices first_stack_1 = create_vertices();
    p_vertices first_stack_2 = create_vertices();
    p_vertices secon_stack_1 = create_vertices();
    p_vertices secon_stack_2 = create_vertices();

    // On empile les paires (non-acceptant,acceptant) pour le DFS1 et les paires
    // (acceptant,non-acceptant) pour le DFS2
    for (uint i = 0; i < size_vertices(M->accept_list);
         i++)                            // Boucle sur les éléments acceptants
    {
        uint j = 0;
        uint r = 0;
        while (r < M->trans->size_graph) // Boucle sur tous les éléments en
        // identifiants les acceptants
        {
            if (size_vertices(M->accept_list) <= j ||
                r < lefread_vertices(M->accept_list, j))
            {
                set_nth_bit(&thepairs[r][lefread_vertices(M->accept_list, i)], 1,
                            3);          // On marque la paire comme ayant été
                                         // visitée pour le
                // DF1 (bit 3)
                rigins_vertices(r, first_stack_1);
                rigins_vertices(lefread_vertices(M->accept_list, i), first_stack_2);

                set_nth_bit(&thepairs[lefread_vertices(M->accept_list, i)][r], 1,
                            4);          // On marque la paire comme ayant été
                                         // visitée pour le
                // DF2 (bit 4)
                rigins_vertices(lefread_vertices(M->accept_list, i), secon_stack_1);
                rigins_vertices(r, secon_stack_2);

                r++;
            }
            else
            {
                r++;
                j++;
            }
        }
    }

    // Calcul des graphes miroirs
    p_lgraph rmirror = cayley_rmirror(M);
    p_lgraph lmirror = cayley_lmirror(M);

    // Premier DFS
    while (!isempty_vertices(first_stack_1))
    {
        uint q = rigpull_vertices(first_stack_1);
        uint r = rigpull_vertices(first_stack_2);

        set_nth_bit(&thepairs[q][r], 0, 2); // On met le 2ème bit à 0, DFS1

        for (uint a = 0; a < M->trans->size_alpha; a++)
        {
            for (uint i = 0; i < size_vertices(rmirror->edges[q][a]); i++)
            {
                for (uint j = 0; j < size_vertices(rmirror->edges[r][a]); j++)
                {
                    if (get_nth_bit(thepairs[lefread_vertices(rmirror->edges[q][a], i)]
                                    [lefread_vertices(rmirror->edges[r][a], j)],
                                    3) == 0)
                    {
                        set_nth_bit(&thepairs[lefread_vertices(rmirror->edges[q][a], i)]
                                    [lefread_vertices(rmirror->edges[r][a], j)],
                                    1, 3);
                        rigins_vertices(lefread_vertices(rmirror->edges[q][a], i),
                                        first_stack_1);
                        rigins_vertices(lefread_vertices(rmirror->edges[r][a], j),
                                        first_stack_2);
                    }
                }
            }

            for (uint i = 0; i < size_vertices(lmirror->edges[q][a]); i++)
            {
                for (uint j = 0; j < size_vertices(lmirror->edges[r][a]); j++)
                {
                    if (get_nth_bit(thepairs[lefread_vertices(lmirror->edges[q][a], i)]
                                    [lefread_vertices(lmirror->edges[r][a], j)],
                                    3) == 0)
                    {
                        set_nth_bit(&thepairs[lefread_vertices(lmirror->edges[q][a], i)]
                                    [lefread_vertices(lmirror->edges[r][a], j)],
                                    1, 3);
                        rigins_vertices(lefread_vertices(lmirror->edges[q][a], i),
                                        first_stack_1);
                        rigins_vertices(lefread_vertices(lmirror->edges[r][a], j),
                                        first_stack_2);
                    }
                }
            }
        }
    }

    // Second DFS
    while (!isempty_vertices(secon_stack_1))
    {
        uint q = rigpull_vertices(secon_stack_1);
        uint r = rigpull_vertices(secon_stack_2);

        set_nth_bit(&thepairs[q][r], 0, 1); // On met le 1er bit à 0, DFS2

        for (uint a = 0; a < M->trans->size_alpha; a++)
        {
            for (uint i = 0; i < size_vertices(rmirror->edges[q][a]); i++)
            {
                for (uint j = 0; j < size_vertices(rmirror->edges[r][a]); j++)
                {
                    if (get_nth_bit(thepairs[lefread_vertices(rmirror->edges[q][a], i)]
                                    [lefread_vertices(rmirror->edges[r][a], j)],
                                    4) == 0)
                    {
                        set_nth_bit(&thepairs[lefread_vertices(rmirror->edges[q][a], i)]
                                    [lefread_vertices(rmirror->edges[r][a], j)],
                                    1, 4);
                        rigins_vertices(lefread_vertices(rmirror->edges[q][a], i),
                                        secon_stack_1);
                        rigins_vertices(lefread_vertices(rmirror->edges[r][a], j),
                                        secon_stack_2);
                    }
                }
            }

            for (uint i = 0; i < size_vertices(lmirror->edges[q][a]); i++)
            {
                for (uint j = 0; j < size_vertices(lmirror->edges[r][a]); j++)
                {
                    if (get_nth_bit(thepairs[lefread_vertices(lmirror->edges[q][a], i)]
                                    [lefread_vertices(lmirror->edges[r][a], j)],
                                    4) == 0)
                    {
                        set_nth_bit(&thepairs[lefread_vertices(lmirror->edges[q][a], i)]
                                    [lefread_vertices(lmirror->edges[r][a], j)],
                                    1, 4);
                        rigins_vertices(lefread_vertices(lmirror->edges[q][a], i),
                                        secon_stack_1);
                        rigins_vertices(lefread_vertices(lmirror->edges[r][a], j),
                                        secon_stack_2);
                    }
                }
            }
        }
    }

    // Affectation dans la structure de données
    MALLOC(M->order, M->trans->size_graph);
    for (uint q = 0; q < M->trans->size_graph; q++)
    {
        M->order[q] = create_vertices();
        for (uint r = 0; r < M->trans->size_graph; r++)
        {
            if (get_nth_bit(thepairs[q][r], 1) == 1)
            {
                rigins_vertices(r, M->order[q]);
            }
        }
        free(thepairs[q]);
    }
    free(thepairs);
}

// Affichage des idempotents
void cayley_print_idems(p_cayley M, FILE *out)
{
    print_top_line(100, out);

    fprintf(out, "│Idempotents : ");
    print_sub_cayley_aligned(M, M->idem_list, 100, 14, out);
    print_bot_line(100, out);
}

// Affichage de l'ordre syntaxique
void cayley_print_order(p_cayley M, FILE *out)
{
    compute_syntac_order(M); // Calcul (si nécessaire de l'ordre)
    print_top_line(100, out);
    // print_title_box(100, false, out, 1, "Computation on the syntactic
    // morphism.
    // Syntactic order.");

    // Calcul de la longueur maximale d'un nom
    uint size_max = 1;
    for (uint q = 0; q < M->trans->size_graph; q++)
    {
        size_max = max(size_max, size_vertices(M->names[q]));
    }

    uint padding = 24 + size_max;

    for (uint q = 0; q < M->trans->size_graph; q++)
    {
        fprintf(out, "│Elements larger than ");
        cayley_print_name_aligned(M, q, size_max, out);
        fprintf(out, " : ");
        print_sub_cayley_aligned(M, M->order[q], 100, padding, out);
    }
    print_bot_line(100, out);
}

// Print de la table de multiplication
void cayley_mult_print(p_cayley M, FILE *out)
{
    // Calcule de la table de multiplication
    compute_mult(M);

    // Calcul de la longueur max du nom d'un élément
    uint length = cayley_max_name_size(M, NULL);

    // Barre Horizontale haute
    fprintf(out, "┌");
    for (uint i = 0; i < length; i++)
    {
        fprintf(out, "─");
    }
    fprintf(out, "┬");
    for (uint i = 0; i < M->trans->size_graph; i++)
    {
        for (uint j = 0; j < length; j++)
        {
            fprintf(out, "─");
        }
        if (i < M->trans->size_graph - 1)
        {
            fprintf(out, "┬");
        }
        else
        {
            fprintf(out, "┐");
        }
    }
    fprintf(out, "\n");

    // Première ligne
    fprintf(out, "│");
    for (uint i = 0; i < length; i++)
    {
        fprintf(out, " ");
    }
    fprintf(out, "│");
    for (uint i = 0; i < M->trans->size_graph; i++)
    {
        cayley_print_name_aligned(M, i, length, out);
        fprintf(out, "│");
    }
    fprintf(out, "\n");

    // Barre Horizontale mid
    fprintf(out, "├");
    for (uint i = 0; i < length; i++)
    {
        fprintf(out, "─");
    }
    fprintf(out, "┼");
    for (uint i = 0; i < M->trans->size_graph; i++)
    {
        for (uint j = 0; j < length; j++)
        {
            fprintf(out, "─");
        }
        if (i < M->trans->size_graph - 1)
        {
            fprintf(out, "┼");
        }
        else
        {
            fprintf(out, "┤");
        }
    }
    fprintf(out, "\n");

    // La table

    for (uint k = 0; k < M->trans->size_graph; k++)
    {
        fprintf(out, "│");
        cayley_print_name_aligned(M, k, length, out);
        fprintf(out, "│");
        for (uint i = 0; i < M->trans->size_graph; i++)
        {
            cayley_print_name_aligned(M, M->mult[k][i], length, out);
            fprintf(out, "│");
        }
        fprintf(out, "\n");
    }

    // Barre Horizontale basse
    fprintf(out, "└");
    for (uint i = 0; i < length; i++)
    {
        fprintf(out, "─");
    }
    fprintf(out, "┴");
    for (uint i = 0; i < M->trans->size_graph; i++)
    {
        for (uint j = 0; j < length; j++)
        {
            fprintf(out, "─");
        }
        if (i < M->trans->size_graph - 1)
        {
            fprintf(out, "┴");
        }
        else
        {
            fprintf(out, "┘");
        }
    }
    fprintf(out, "\n");
}

/************************************/
/* Opérations sur les Cayley graphs */
/************************************/

uint cayley_mult(p_cayley M, uint s, uint t)
{
    if (s >= M->trans->size_graph || t >= M->trans->size_graph)
    {
        printf("Error, these are not elements of the monoid\n");
        return M->trans->size_graph;
    }
    if (M->mult != NULL)
    {
        return M->mult[s][t];
    }
    uint r = s;
    for (uint i = 0; i < size_vertices(M->names[t]); i++)
    {
        r = M->trans->edges[r][lefread_vertices(M->names[t], i)];
    }
    return r;
}

uint cayley_mult_gen(p_cayley M, uint n, ...)
{
    va_list liste;
    va_start(liste, n);

    uint elem = ONE;

    // Calcul
    for (uint i = 0; i < n; i++)
    {
        elem = cayley_mult(M, elem, va_arg(liste, uint));
    }
    return elem;
}

uint cayley_omega(p_cayley M, uint s)
{
    if (s >= M->trans->size_graph)
    {
        printf("Error, these are not elements of the monoid\n");
        return M->trans->size_graph;
    }
    uint s2 = cayley_mult(M, s, s);
    uint q  = s;
    uint r  = s2;
    while (q != r)
    {
        q = cayley_mult(M, q, s);
        r = cayley_mult(M, r, s2);
    }
    return q;
}

bool cayley_elem_from_string(p_cayley M, char *w, uint *res)
{
    uint len = strlen(w);
    *res = ONE;
    for (uint i = 0; i < len; i++)
    {
        uint a = w[i] - 'a';
        if (w[i] >= 'a' && a < M->trans->size_alpha)
        {
            *res = M->trans->edges[*res][a];
        }
        else
        {
            return false;
        }
    }
    return true;
}

// Image d'un mot passé sous ma forme d'une chaîne de caractère
void cayley_print_image(p_cayley M, char *w, FILE *out)
{
    uint s;
    if (cayley_elem_from_string(M, w, &s))
    {
        fprintf(out, "The image of the word %s is ", w);
        cayley_print_name(M, s, out);
        fprintf(out, ".\n");
    }
    else
    {
        printf("Error: this is not a valid word for this morphism.\n");
        return;
    }
}

//
bool cayley_num_idem(p_cayley M, uint s, uint *res)
{
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        if (lefread_vertices(M->idem_list, i) == s)
        {
            *res = i;
            return true;
        }
    }
    return false;
}

/**********************************/
/* Tests de propriétés classiques */
/**********************************/

// Idempotence
bool is_idem_mono(p_cayley M, FILE *out)
{
    if (out)
    {
        fprintf(out, "#### Checking if the monoid is idempotent.\n");
    }
    if (size_vertices(M->idem_list) == M->trans->size_graph)
    {
        if (out)
        {
            fprintf(out, "#### The monoid is idempotent.\n");
        }

        return true;
    }
    else
    {
        if (out)
        {
            fprintf(out, "#### The monoid is NOT idempotent.\n");
            for (uint q = 0; q < M->trans->size_graph; q++)
            {
                if (size_vertices(M->idem_list) <= q ||
                    lefread_vertices(M->idem_list, q) != q)
                {
                    fprintf(out, "#### For instance, ");
                    cayley_print_name(M, q, out);
                    fprintf(out, " is not an idempotent\n");
                    break;
                }
            }
        }
        return false;
    }
}

// Commutativité
bool is_comm_mono(p_cayley M, FILE *out)
{
    if (out)
    {
        fprintf(out, "#### Checking if the monoid is commutative.\n");
    }
    for (uint a = 0; a < M->trans->size_alpha - 1; a++)
    {
        for (uint b = a + 1; b < M->trans->size_alpha; b++)
        {
            uint av = M->trans->edges[ONE][a];
            uint bv = M->trans->edges[ONE][b];

            uint ab = cayley_mult(M, av, bv);
            uint ba = cayley_mult(M, bv, av);
            if (ab != ba)
            {
                fprintf(out, "#### The monoid is NOT commutative.\n");
                fprintf(out, "#### For instance, %c%c ≠ %c%c.\n", a + 'a', b + 'a',
                        b + 'a', a + 'a');
                return false;
            }
        }
    }
    fprintf(out, "#### The monoid is commutative.\n");
    return true;
}
