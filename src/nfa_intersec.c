/************************/
/* Intersection of NFAs */
/************************/

#include "nfa_intersec.h"

/************************************************************************************************/
/* Construction classique: calcul de l'automate produit en ne gardant que les états accessibles */
/************************************************************************************************/

// Représentation temporaire d'un état de l'automate produit
// pendant le parcours en profondeur.  On mémorise également
// les états adjacents trouvés pour la construction du NFA.
struct intstate
{
    // Les états provenant des deux NFAs d'origine
    uint q1;
    uint q2;

    // La taille de l'alphabet
    uint size_alpha;

    // Le numéro de l'état dans le futur automate produit
    uint num;

    // Transitions classiques depuis l'état
    uint *nt;              // Taille size_alpha nt[a] contient le nbre de transitions étiquetées par a
    struct intstate ***tr; // Taille size_alpha tr[a][i] contient un pointeur vers l'état visité en prenant la ième transition étiquetée par a

    // Transitions epsilon depuis l'état
    uint nte;              // Nbre de transitions epsilon
    struct intstate **tre; // Taille size_alpha tre[i] contient un pointeur vers l'état visité en prenant la ième epsilon transition

    // Transitions inverses depuis l'état
    uint *nti;              // Taille size_alpha nt[a] contient le nbre de transitions inverses étiquetées par a
    struct intstate ***tri; // Taille size_alpha tr[a][i] contient un pointeur vers l'état visité en prenant la ième transition inverse étiquetée par a
};
typedef struct intstate *p_intstate;

// Fonction de libération
static void free_intstate(void *p)
{
    if (p == NULL)
    {
        return;
    }
    for (uint a = 0; a < ((p_intstate)p)->size_alpha; a++)
    {
        free(((p_intstate)p)->tr[a]);
    }
    free(((p_intstate)p)->nt);
    free(((p_intstate)p)->tr);

    free(((p_intstate)p)->tre);

    if (((p_intstate)p)->tri != NULL)
    {
        for (uint a = 0; a < ((p_intstate)p)->size_alpha; a++)
        {
            free(((p_intstate)p)->tri[a]);
        }
        free(((p_intstate)p)->nti);
        free(((p_intstate)p)->tri);
    }

    free(p);
}

// Pour le parcours profondeur, on stocke les états rencontrés
// dans un AVL. On a donc besoin d'une fonction de comparaison.
// Retour, O si =, 1 si <, -1 si >
static int comp_intstate(void *s, void *t)
{
    if (((p_intstate)s)->q1 < ((p_intstate)t)->q1)
    {
        return 1;
    }
    else if (((p_intstate)s)->q1 > ((p_intstate)t)->q1)
    {
        return -1;
    }
    else
    {
        if (((p_intstate)s)->q2 < ((p_intstate)t)->q2)
        {
            return 1;
        }
        else if (((p_intstate)s)->q2 > ((p_intstate)t)->q2)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

// Fonction qui remplit le tableau des transitions de l'automate produit à partir
// de l'AVL contruit pendant le parcours en profondeur.
// La fonction calcule également les nouveaux états finaux: finals est un AVL qui
// contient les paires d'états finaux des deux NFAs d'origine. On
// des états finaux du NFA original et boolfinals est un tableau de Booléens dans
// lequel on enregistre les nouveaux états finaux.
// Le Booléen names indique si les noms des nouveaux états (des paires d'états
// de l'ancien NFA) doivent être sauvegardés.
static void inter_avl_to_table(p_avlnode tree, p_nfa PROD, bool names, state_name ntype1, void *oldnames1, state_name ntype2, void *oldnames2)
{
    if (tree == NULL)
    {
        return;
    }
    else
    {
        inter_avl_to_table(tree->left, PROD, names, ntype1, oldnames1, ntype2, oldnames2);
        inter_avl_to_table(tree->right, PROD, names, ntype1, oldnames1, ntype2, oldnames2);
        p_intstate thestate = (p_intstate)tree->value;

        // Transitions classiques
        for (uint a = 0; a < PROD->trans->size_alpha; a++)
        {
            // On utilise un tas pour trier les états accessibles par ordre croissant
            p_binheap tempheap = create_binheap(&fcmp_int);
            for (uint i = 0; i < thestate->nt[a]; i++)
            {
                push_binheap(tempheap, &thestate->tr[a][i]->num);
            }
            for (uint i = 0; i < thestate->nt[a]; i++)
            {
                rigins_vertices(*(int *)popmin_binheap(tempheap), PROD->trans->edges[thestate->num][a]);
            }
            delete_binheap(tempheap);
        }

        // Transitions epsilon
        if (PROD->etrans != NULL)
        {
            // On utilise un tas pour trier les états accessibles par ordre croissant
            p_binheap tempheap = create_binheap(&fcmp_int);
            for (uint i = 0; i < thestate->nte; i++)
            {
                push_binheap(tempheap, &thestate->tre[i]->num);
            }
            for (uint i = 0; i < thestate->nte; i++)
            {
                rigins_vertices(*(int *)popmin_binheap(tempheap), PROD->etrans->edges[thestate->num]);
            }
            delete_binheap(tempheap);
        }

        // Transitions inverses
        if (PROD->itrans != NULL)
        {
            // On utilise un tas pour trier les états accessibles par ordre croissant
            for (uint a = 0; a < PROD->trans->size_alpha; a++)
            {
                p_binheap tempheap = create_binheap(&fcmp_int);
                for (uint i = 0; i < thestate->nti[a]; i++)
                {
                    push_binheap(tempheap, &thestate->tri[a][i]->num);
                }
                for (uint i = 0; i < thestate->nti[a]; i++)
                {
                    rigins_vertices(*(int *)popmin_binheap(tempheap), PROD->itrans->edges[thestate->num][a]);
                }
                delete_binheap(tempheap);
            }
        }

        if (names)
        {
            uint *temp;
            MALLOC(temp, 2);
            if (ntype1 == NUMBER)
            {
                temp[0] = ((uint *)oldnames1)[thestate->q1];
            }
            else
            {
                temp[0] = thestate->q1;
            }

            if (ntype2 == NUMBER)
            {
                temp[1] = ((uint *)oldnames2)[thestate->q2];
            }
            else
            {
                temp[1] = thestate->q2;
            }

            ((uint **)PROD->names)[thestate->num] = temp;

            // p_vertices thename = create_vertices();
            // rigins_vertices(thestate->q1, thename);
            // rigins_vertices(thestate->q2, thename);
            // PROD->names[thestate->num] = thename;
        }
    }
}

// Intersection (commence par éliminer les transitions epsilon)
p_nfa nfa_intersect(p_nfa A1, p_nfa A2, bool names)
{
    // On teste si il existe des transitions epsilon dans l'un des deux automates
    bool has_etrans = false;
    if (A1->etrans != NULL || A2->etrans != NULL)
    {
        init_epstrans(A1); // On initialise dans les deux NFAs
        init_epstrans(A2);
        has_etrans = true; // On mémorise cette information
    }

    // On teste si il existe des transitions epsilon dans les deux automates
    bool has_itrans = true;
    if (A1->itrans == NULL || A2->itrans == NULL) // Si au moins l'un des NFAs n'a pas de transitions inverses
    {
        delete_lgraph(A1->itrans); // On supprime les transitions inverses dans les deux NFAs
        A1->itrans = NULL;
        delete_lgraph(A2->itrans);
        A2->itrans = NULL;
        has_itrans = false; // On mémorise cette information
    }

    // Calcul du nouvel alphabet
    uint thealph = max(A1->trans->size_alpha, A2->trans->size_alpha);
    uint comalph = min(A1->trans->size_alpha, A2->trans->size_alpha);

    // Pile pour le parcours en profondeur;
    p_stack thestack = create_stack();

    // Un avl qui contiendra les états de l'automate produit déjà rencontrés
    p_avlnode thetree = NULL;

    // Compteur du nombre d'états créés
    uint num = 0;

    // On créé également un tas binaire dans lequel on va mémoriser les numéros des états finaux (on utilise un tas pour les trier en même temps)
    p_binheap heapfinals = create_binheap(&fcmp_int);

    // Le nouveau NFA (on va directement construire la liste des états initiaux)
    p_nfa PROD;
    MALLOC(PROD, 1);                    // Création du NFA
    PROD->initials = create_vertices(); // Création de la liste contenant les états initiaux

    // Création et empilement des états initiaux (paires d'états initiaux)
    for (uint i1 = 0; i1 < size_vertices(A1->initials); i1++)
    {
        for (uint i2 = 0; i2 < size_vertices(A2->initials); i2++)
        {
            p_intstate newini;
            MALLOC(newini, 1);
            newini->q1 = lefread_vertices(A1->initials, i1);
            newini->q2 = lefread_vertices(A2->initials, i2);
            newini->size_alpha = thealph;
            newini->num = num;
            rigins_vertices(num, PROD->initials);
            // printf("num: %d\n", num);
            num++;
            push(newini, thestack);
            thetree = avl_insert(newini, thetree, &comp_intstate);
        }
    }

    // Parcours en profondeur
    while (!isempty_stack(thestack))
    {
        // Récupération de l'état qu'on va traiter
        p_intstate thestate = (p_intstate)pop(thestack);
        // printf("The state : %d,%d\n", thestate->q1, thestate->q2);

        // Mémorisation du numéro si l'état est final
        if (mem_vertices_sorted(thestate->q1, A1->finals) && mem_vertices_sorted(thestate->q2, A2->finals))
        {
            push_binheap(heapfinals, &thestate->num);
        }

        // Calcul des transitions classiques à partir de thestate
        MALLOC(thestate->nt, thealph);
        MALLOC(thestate->tr, thealph);
        for (uint a = 0; a < comalph; a++)
        {
            // Nombre de transitions sortantes étiquetées par a
            thestate->nt[a] = size_vertices(A1->trans->edges[thestate->q1][a]) * size_vertices(A2->trans->edges[thestate->q2][a]);
            MALLOC(thestate->tr[a], thestate->nt[a]);
            uint k = 0; // Compteur des transitions traitées

            // Pour chaque transition
            for (uint i1 = 0; i1 < size_vertices(A1->trans->edges[thestate->q1][a]); i1++)
            {
                for (uint i2 = 0; i2 < size_vertices(A2->trans->edges[thestate->q2][a]); i2++)
                {
                    // On crée un (potentiel) nouvel état
                    p_intstate newstate;
                    MALLOC(newstate, 1);
                    newstate->q1 = lefread_vertices(A1->trans->edges[thestate->q1][a], i1);
                    newstate->q2 = lefread_vertices(A2->trans->edges[thestate->q2][a], i2);

                    // printf("Trans: %c-> %d,%d\n", a, newstate->q1, newstate->q2);
                    // On regarde si cet état a en fait déjà été construit
                    p_avlnode thenode = avl_search(newstate, thetree, &comp_intstate);
                    if (thenode == NULL)
                    { // Si cet état n'a pas encore été construit
                        newstate->size_alpha = thealph;
                        newstate->nt = NULL;
                        newstate->tr = NULL;
                        newstate->num = num; // On affecte un numéro à ce nouvel état
                        num++;

                        thestate->tr[a][k] = newstate; // On peut affecter la kième transition
                        k++;
                        thetree = avl_insert(newstate, thetree, &comp_intstate); // On mémorise le nouvel état dans l'ensemble des états rencontrés
                        push(newstate, thestack);                                // Ce nouvel état est à traiter
                    }
                    else
                    { // Sinon, cet état a déjà été construit
                        // printf("not NULL\n");
                        thestate->tr[a][k] = ((p_intstate)thenode->value); // On peut affecter la kième transition
                        k++;
                        free(newstate); // On supprime la copie qu'on vient de créer
                    }
                }
            }
        }

        // Calcul des transitions epsilon à partir de thestate
        if (has_etrans)
        {
            // Nombre de transitions epsilon
            thestate->nte = size_vertices(A1->etrans->edges[thestate->q1]) * size_vertices(A2->etrans->edges[thestate->q2]);
            MALLOC(thestate->tre, thestate->nte);
            uint k = 0; // Compteur des transitions traitées

            // Pour chaque transition
            for (uint i1 = 0; i1 < size_vertices(A1->etrans->edges[thestate->q1]); i1++)
            {
                for (uint i2 = 0; i2 < size_vertices(A2->etrans->edges[thestate->q2]); i2++)
                {
                    // On crée un (potentiel) nouvel état
                    p_intstate newstate;
                    MALLOC(newstate, 1);
                    newstate->q1 = lefread_vertices(A1->etrans->edges[thestate->q1], i1);
                    newstate->q2 = lefread_vertices(A2->etrans->edges[thestate->q2], i2);

                    // On regarde si cet état a en fait déjà été construit
                    p_avlnode thenode = avl_search(newstate, thetree, &comp_intstate);
                    if (thenode == NULL)
                    { // Si cet état n'a pas encore été construit
                        newstate->size_alpha = thealph;
                        newstate->num = num; // On affecte un numéro à ce nouvel état
                        num++;

                        thestate->tre[k] = newstate; // On peut affecter la kième epsilon transition
                        k++;
                        thetree = avl_insert(newstate, thetree, &comp_intstate); // On mémorise le nouvel état dans l'ensemble des états rencontrés
                        push(newstate, thestack);                                // Ce nouvel état est à traiter
                    }
                    else
                    { // Sinon, cet état a déjà été construit
                        // printf("not NULL\n");
                        thestate->tre[k] = ((p_intstate)thenode->value); // On peut affecter la kième transition
                        k++;
                        free(newstate); // On supprime la copie qu'on vient de créer
                    }
                }
            }
        }

        // Calcul des transitions inverse
        if (has_itrans)
        {
            MALLOC(thestate->nti, thealph);
            MALLOC(thestate->tri, thealph);
            for (uint a = 0; a < comalph; a++)
            {
                // Nombre de transitions inverses sortantes étiquetées par a
                thestate->nti[a] = size_vertices(A1->itrans->edges[thestate->q1][a]) * size_vertices(A2->itrans->edges[thestate->q2][a]);
                MALLOC(thestate->tri[a], thestate->nti[a]);
                uint k = 0; // Compteur des transitions traitées

                // Pour chaque transition inverse
                for (uint i1 = 0; i1 < size_vertices(A1->itrans->edges[thestate->q1][a]); i1++)
                {
                    for (uint i2 = 0; i2 < size_vertices(A2->itrans->edges[thestate->q2][a]); i2++)
                    {
                        // On crée un (potentiel) nouvel état
                        p_intstate newstate;
                        MALLOC(newstate, 1);
                        newstate->q1 = lefread_vertices(A1->itrans->edges[thestate->q1][a], i1);
                        newstate->q2 = lefread_vertices(A2->itrans->edges[thestate->q2][a], i2);

                        // On regarde si cet état a en fait déjà été construit
                        p_avlnode thenode = avl_search(newstate, thetree, &comp_intstate);
                        if (thenode == NULL)
                        { // Si cet état n'a pas encore été construit
                            newstate->size_alpha = thealph;
                            newstate->nt = NULL;
                            newstate->tr = NULL;
                            newstate->num = num; // On affecte un numéro à ce nouvel état
                            num++;

                            thestate->tri[a][k] = newstate; // On peut affecter la kième transition
                            k++;
                            thetree = avl_insert(newstate, thetree, &comp_intstate); // On mémorise le nouvel état dans l'ensemble des états rencontrés
                            push(newstate, thestack);                                // Ce nouvel état est à traiter
                        }
                        else
                        { // Sinon, cet état a déjà été construit
                            // printf("not NULL\n");
                            thestate->tri[a][k] = ((p_intstate)thenode->value); // On peut affecter la kième transition
                            k++;
                            free(newstate); // On supprime la copie qu'on vient de créer
                        }
                    }
                }
            }
        }
    }
    // Le parcours en profondeur est terminé, plus besoin de la pile
    delete_stack(thestack);

    // Construction du nouveau NFA (continued)
    PROD->trans = create_lgraph_noedges(num, thealph); // Création du graph
    if (has_etrans)
    {
        PROD->etrans = create_graph_noedges(num);
    }
    else
    {
        PROD->etrans = NULL;
    }
    if (has_itrans)
    {
        PROD->itrans = create_lgraph_noedges(num, thealph);
    }
    else
    {
        PROD->itrans = NULL;
    }

    if (names)
    { // Si on doit mémoriser les noms
        PROD->ntype = PAIR;
        nfa_init_names(PROD);
    }
    else
    {
        PROD->ntype = NONAME;
        PROD->names = NULL;
    }

    // Création des transitions et des noms
    inter_avl_to_table(thetree, PROD, names, A1->ntype, A1->names, A2->ntype, A2->names);

    // Il ne reste plus qu'à créer et remplir la liste des états finaux
    PROD->finals = create_vertices(); // Création de la liste contenant les états finaux
    while (!isempty_binheap(heapfinals))
    {
        rigins_vertices(*(uint *)popmin_binheap(heapfinals), PROD->finals);
    }
    delete_binheap(heapfinals);

    avl_free_strong(thetree, &free_intstate);

    // print_vertices(PROD->initials);

    return PROD;
}
