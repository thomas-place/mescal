/********************************************/
/* Determinisation et Minimisation des NFAs */
/********************************************/

#include "nfa_determi.h"

/************************/
/* Procédure principale */
/************************/

// Représentation temporaire d'un état de l'automate determinisé
// pendant le parcours en profondeur.  On mémorise également les
// états adjacents trouvés pour la construction du DFA ensuite.
struct detstate
{
    // L'état du DFA: un ensemble trié d'états du NFA d'origine
    p_vertices set;

    // La taille de l'alphabet
    uint size_alpha;

    // Le numéro de l'état dans le future automate det
    uint num;

    // Un tableau de taille size_alph contenant les transitions depuis l'état
    // pour chaque lettre
    struct detstate** next;
};
typedef struct detstate* p_detstate;

// Fonctions de libération d'un detstate. La première supprime le
// champ "set" alors que la seconde le laisse alloué. Cete seconde
// fonction permet de réutiliser le champ "set" comme nom de l'état
// dans le DFA construit.
static void free_detstate(void* p)
{
    if (p == NULL)
    {
        return;
    }
    delete_vertices(((p_detstate)p)->set);

    free(p);
}

// static void free_detstate_saveset(void *p)
// {
//     if (p == NULL)
//     {
//         return;
//     }
//     free(((p_detstate)p)->next);
//     free(p);
// }

// Pour le parcours profondeur, on stocke les états rencontrés
// dans un AVL. On a donc besoin d'une fonction de comparaison.
// Retour, O si =, 1 si <, -1 si >
static int comp_detstate(void* e1, void* e2)
{
    p_vertices s1 = ((p_detstate)e1)->set;
    p_vertices s2 = ((p_detstate)e2)->set;
    // On compare d'abord la taille
    if (size_vertices(s1) < size_vertices(s2))
    {
        return 1;
    }
    if (size_vertices(s1) > size_vertices(s2))
    {
        return -1;
    }
    // Sinon ordre lexico
    for (uint i = 0; i < size_vertices(s1); i++)
    {
        if (lefread_vertices(s1, i) < lefread_vertices(s2, i))
        {
            return 1;
        }
        if (lefread_vertices(s1, i) > lefread_vertices(s2, i))
        {
            return 1;
        }
    }
    return 0;
}

// Fonction qui remplit le tableau des transitions du DFA final à partir de l'AVL
// contruit pendant le parcours en profondeur.
// La fonction calcule également les nouveaux états finaux: oldfinals est la liste
// des états finaux du NFA original et boolfinals est un tableau de Booléens dans
// lequel on enregistre les nouveaux états finaux.
// Le Booléen names indique si les noms des nouveaux états (des ensembles d'états
// de l'ancien NFA) doivent être sauvegardés.
static void dfa_avl_to_table(p_avlnode AVL, p_nfa DFA, p_vertices oldfinals, bool* newfinals, bool names, state_name ntype, void* oldnames)
{
    if (AVL == NULL)
    {
        return;
    }
    else
    {
        dfa_avl_to_table(AVL->left, DFA, oldfinals, newfinals, names, ntype, oldnames);
        dfa_avl_to_table(AVL->right, DFA, oldfinals, newfinals, names, ntype, oldnames);
        p_detstate thestate = (p_detstate)AVL->value;

        // Enregistrement des transitions depuis l'état (son numéro est "thestate->num")
        for (uint a = 0; a < DFA->trans->size_alpha; a++)
        {
            lefins_vertices(thestate->next[a]->num, DFA->trans->edges[thestate->num][a]);
        }

        // Si l'état est final (il correspond à un ensemble d'états du NFA original qui
        // intersecte les anciens états finaux), on enregistre cette information.
        if (intersec_vertices(oldfinals, thestate->set))
        {
            newfinals[thestate->num] = true;
        }
        else
        {
            newfinals[thestate->num] = false;
        }

        // Enregistrement du nom de l'état (si nécessaire).
        if (names)
        {
            uint* temp;
            MALLOC(temp, size_vertices(thestate->set) + 1);
            temp[0] = size_vertices(thestate->set);
            for (uint i = 0; i < temp[0]; i++)
            {
                if (ntype == NUMBER)
                {
                    temp[i + 1] = ((uint*)oldnames)[lefread_vertices(thestate->set, i)];
                }
                else
                {
                    temp[i + 1] = lefread_vertices(thestate->set, i);
                }
            }
            ((uint**)DFA->names)[thestate->num] = temp;
        }
    }
}

// Procédure de déterminisation. Le Booléen names indique si les noms des nouveaux états
// (des ensembles d'états de l'ancien NFA) doivent être sauvegardés dans le DFA.
p_nfa nfa_determinize(p_nfa A, bool names)
{
    // On commence par éliminer les (éventuelles) transitions epsilon
    nfa_elimeps_mod(A);

    // La pile utilisée pour le parcours en profondeur.
    p_stack thestack = create_stack();

    // L'AVL qu'on utilise pour mémoriser les états du futur DFA déjà construits pendant
    // le parcours en profondeur.
    p_avlnode tree = NULL;

    // Compteur qui servira à compter et numéroter les états créés
    int num = 0;

    // Construction de l'état initial du futur DFA.
    p_detstate start;
    MALLOC(start, 1);
    start->set = create_vertices();
    copy_vertices_right(start->set, A->initials, 0); // L'état initial est l'ensemble des états initiaux du NFA original
    start->size_alpha = A->trans->size_alpha;
    start->num = num; // L'état initial aura le numéro 0
    num++;
    start->next = NULL;                             // Les transitions seront enregistrées plus tard en explorant les états adjacents
    tree = avl_insert(start, tree, &comp_detstate); // Insertion dans l'ensemble des états déjà rencontrés
    push(start, thestack);                          // Cet état est à traiter

    while (!isempty_stack(thestack))
    {
        // Récupération de l'état qu'on va traiter
        p_detstate thestate = (p_detstate)pop(thestack);

        // Calcul des transitions à partir de thestate
        MALLOC(thestate->next, A->trans->size_alpha);
        for (uint a = 0; a < A->trans->size_alpha; a++) // Pour chaque lettre a
        {
            // On crée un (potentiel) nouvel état à traiter pour la transition étiquetée par a
            // On n'affecte que l'ensemble d'états correspondant dans le NFA d'origine
            // En effte, cette information suffit pour comparer cet état avec ceux dans l'AVL
            p_detstate new;
            MALLOC(new, 1);
            new->set = lgraph_reachable(A->trans, thestate->set, a);

            // On regarde dans l'AVL si cet état a en fait déjà été construit
            p_avlnode thenode = avl_search(new, tree, &comp_detstate);
            if (thenode == NULL)
            {
                // Si cet état est nouveau, on termine sa construction
                new->size_alpha = A->trans->size_alpha;
                new->num = num;
                num++;
                new->next = NULL;

                // Ce nouvel état est adjacent à l'état courant pour la lettre a
                thestate->next[a] = new;

                // On mémorise ce nouvel état et on le met dans la pile pour l'explorer ensuite
                tree = avl_insert(new, tree, &comp_detstate);
                push(new, thestack);
            }
            else
            {
                // Si l'état n'est pas nouveau, on affecte la version déjà construite comme adjacent pour la lettre a
                thestate->next[a] = ((p_detstate)thenode->value);

                // On supprime la copie qu'on vient de créer
                delete_vertices(new->set);
                free(new);
            }
        }
    }
    // Le parcours en profondeur est fini, la pile n'est plus nécessaire
    delete_stack(thestack);

    // Construction du DFA
    p_nfa DFA;
    MALLOC(DFA, 1);                                                // Création du DFA
    DFA->initials = create_vertices();                             // Création de la liste contenant l'état initial
    lefins_vertices(0, DFA->initials);                             // Par construction, l'état initial a le numéro 0
    DFA->trans = create_lgraph_noedges(num, A->trans->size_alpha); // Création du graph
    DFA->etrans = NULL;                                            // Pas de transitions epsilon
    DFA->itrans = NULL;                                            // Pas de transitions inverses
    if (names)
    {
        DFA->ntype = SET;
        nfa_init_names(DFA);
    }
    else
    {
        DFA->ntype = NONAME;
        DFA->names = NULL;
    }

    // Enregistrement des transitions et des états finaux

    bool tempfinals[num];                                                          // Tableau temporaire pour enregistrer les futurs états finaux
    dfa_avl_to_table(tree, DFA, A->finals, tempfinals, names, A->ntype, A->names); // Création des transitions

    DFA->finals = create_vertices();
    for (int i = 0; i < num; i++) // Enregistrement des états finaux
    {
        if (tempfinals[i])
        {
            rigins_vertices(i, DFA->finals);
        }
    }

    avl_free_strong(tree, &free_detstate);
    // if (names)
    // {
    //     avl_free_strong(tree, &free_detstate_saveset);
    // }
    // else
    // {
    //     avl_free_strong(tree, &free_detstate);
    // }

    // print_dequeue(DFA->finals);

    return DFA;
}

/********************************************/
/* Procédures basées sur la determinisation */
/********************************************/

// Minimisation (Brzozowski)
p_nfa nfa_minimize(p_nfa A)
{
    p_nfa D = nfa_elimeps(A);
    p_nfa B = nfa_mirror(D);
    delete_nfa(D);
    p_nfa C = nfa_determinize(B, false);
    delete_nfa(B);
    B = nfa_mirror(C);
    delete_nfa(C);
    C = nfa_determinize(B, false);
    delete_nfa(B);
    return C;
}

// Complementation
p_nfa nfa_complement(p_nfa A)
{
    p_nfa B = nfa_determinize(A, false);
    p_vertices newfinals = create_vertices();
    for (uint q = 0; q < B->trans->size_graph; q++)
    {
        if (isempty_vertices(B->finals) || q < lefread_vertices(B->finals, 0))
        {
            rigins_vertices(q, newfinals);
        }
        else
        {
            lefpull_vertices(B->finals);
        }
    }
    delete_vertices(B->finals);
    B->finals = newfinals;
    return B;
}
