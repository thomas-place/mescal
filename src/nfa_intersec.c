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
typedef struct intstate {
    // Les états provenant des deux NFAs d'origine
    uint q1;
    uint q2;

    // La taille de l'alphabet
    uint size_alpha;

    // Le numéro de l'état dans le futur automate produit
    uint num;

    // Transitions classiques depuis l'état
    uint* nt;               // Taille size_alpha nt[a] contient le nbre de transitions étiquetées par a
    struct intstate*** tr;  // Taille size_alpha tr[a][i] contient un pointeur vers l'état visité en prenant la ième transition étiquetée par a


    // Transitions epsilon depuis l'état
    uint nte;              // Nbre de transitions epsilon
    struct intstate** tre; // Taille size_alpha tre[i] contient un pointeur vers l'état visité en prenant la ième epsilon transition

    // Transitions inverses depuis l'état
    uint* nti;              // Taille size_alpha nt[a] contient le nbre de transitions inverses étiquetées par a
    struct intstate*** tri; // Taille size_alpha tr[a][i] contient un pointeur vers l'état visité en prenant la ième transition inverse étiquetée par a
} intstate;



// Fonction de libération
static void free_intstate(void* p)
{
    if (p == NULL)
    {
        return;
    }
    for (uint a = 0; a < ((intstate*)p)->size_alpha; a++)
    {
        free(((intstate*)p)->tr[a]);
    }
    free(((intstate*)p)->nt);
    free(((intstate*)p)->tr);

    free(((intstate*)p)->tre);

    if (((intstate*)p)->tri != NULL)
    {
        for (uint a = 0; a < ((intstate*)p)->size_alpha; a++)
        {
            free(((intstate*)p)->tri[a]);
        }
        free(((intstate*)p)->nti);
        free(((intstate*)p)->tri);
    }

    free(p);
}

// Pour le parcours profondeur, on stocke les états rencontrés
// dans un AVL. On a donc besoin d'une fonction de comparaison.
// Retour, O si =, 1 si <, -1 si >
static int compintstate(void* s, void* t) {
    if (((intstate*)s)->q1 < ((intstate*)t)->q1) {
        return 1;
    }
    else if (((intstate*)s)->q1 > ((intstate*)t)->q1) {
        return -1;
    }
    else {
        if (((intstate*)s)->q2 < ((intstate*)t)->q2) {
            return 1;
        }
        else if (((intstate*)s)->q2 > ((intstate*)t)->q2) {
            return -1;
        }
        else {
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
static void inter_avl_to_table(avlnode* tree, nfa* PROD, bool names, char** oldnames1, char** oldnames2) {
    if (tree == NULL) {
        return;
    }
    else {
        inter_avl_to_table(tree->left, PROD, names, oldnames1, oldnames2);
        inter_avl_to_table(tree->right, PROD, names, oldnames1, oldnames2);
        intstate* thestate = (intstate*)tree->value;

        // Transitions classiques
        for (uint a = 0; a < PROD->trans->size_alpha; a++) {
            // On utilise un tas pour trier les états accessibles par ordre croissant
            binheap* tempheap = create_binheap(&fcmp_int);
            for (uint i = 0; i < thestate->nt[a]; i++) {
                push_binheap(tempheap, &thestate->tr[a][i]->num);
            }
            for (uint i = 0; i < thestate->nt[a]; i++) {
                rigins_dequeue(*(int*)popmin_binheap(tempheap),
                    PROD->trans->edges[thestate->num][a]);
            }
            delete_binheap(tempheap);
        }

        // Transitions epsilon
        if (PROD->trans_e != NULL)
        {
            // On utilise un tas pour trier les états accessibles par ordre croissant
            binheap* tempheap = create_binheap(&fcmp_int);
            for (uint i = 0; i < thestate->nte; i++)
            {
                push_binheap(tempheap, &thestate->tre[i]->num);
            }
            for (uint i = 0; i < thestate->nte; i++)
            {
                rigins_dequeue(*(int*)popmin_binheap(tempheap), PROD->trans_e->edges[thestate->num]);
            }
            delete_binheap(tempheap);
        }

        // Transitions inverses
        if (PROD->trans_i != NULL)
        {
            // On utilise un tas pour trier les états accessibles par ordre croissant
            for (uint a = 0; a < PROD->trans->size_alpha; a++)
            {
                binheap* tempheap = create_binheap(&fcmp_int);
                for (uint i = 0; i < thestate->nti[a]; i++)
                {
                    push_binheap(tempheap, &thestate->tri[a][i]->num);
                }
                for (uint i = 0; i < thestate->nti[a]; i++)
                {
                    rigins_dequeue(*(int*)popmin_binheap(tempheap), PROD->trans_i->edges[thestate->num][a]);
                }
                delete_binheap(tempheap);
            }
        }

        if (names) {
            ushort size = 4;
            if (oldnames1) {
                size += strlen(oldnames1[thestate->q1]);
            }
            else {
                size += num_length(thestate->q1);
            }

            if (oldnames2) {

                size += strlen(oldnames2[thestate->q2]);
            }
            else {
                size += num_length(thestate->q2);
            }
            MALLOC(PROD->state_names[thestate->num], size);

            if (oldnames1) {
                if (oldnames2) {
                    sprintf(PROD->state_names[thestate->num], "(%s,%s)", oldnames1[thestate->q1], oldnames2[thestate->q2]);
                }
                else {
                    sprintf(PROD->state_names[thestate->num], "(%s,%d)", oldnames1[thestate->q1], thestate->q2);
                }
            }
            else {
                if (oldnames2) {
                    sprintf(PROD->state_names[thestate->num], "(%d,%s)", thestate->q1, oldnames2[thestate->q2]);
                }
                else {
                    sprintf(PROD->state_names[thestate->num], "(%d,%d)", thestate->q1, thestate->q2);
                }
            }

            //           printf("%s\n", PROD->state_names[thestate->num]);


        }
    }
}

// Intersection (commence par éliminer les transitions epsilon)
nfa* nfa_intersect(nfa* A1, nfa* A2, bool names) {



    // On étend les alphabet des deux automates
    A1 = nfa_copy_exalpha(A1, A2->alphabet, A2->trans->size_alpha);
    A2 = nfa_copy_exalpha(A2, A1->alphabet, A1->trans->size_alpha);


    // Calcul du nouvel alphabet
    uint thealph = A1->trans->size_alpha;
    uint comalph = A1->trans->size_alpha;


    // On teste si il existe des transitions epsilon dans l'un des deux automates
    bool has_etrans = false;
    if (A1->trans_e || A2->trans_e)
    {
        if (!A1->trans_e) {
            A1->trans_e = create_graph_selfloops(A1->trans->size_graph);
        }
        if (!A2->trans_e) {
            A2->trans_e = create_graph_selfloops(A2->trans->size_graph);
        }
        has_etrans = true; // On mémorise cette information

    }

    // On teste si il existe des transitions epsilon dans les deux automates
    bool has_itrans = true;
    if (!A1->trans_i || !A2->trans_i) // Si au moins l'un des NFAs n'a pas de transitions inverses
    {
        delete_lgraph(A1->trans_i); // On supprime les transitions inverses dans les deux NFAs
        A1->trans_i = NULL;
        delete_lgraph(A2->trans_i);
        A2->trans_i = NULL;
        has_itrans = false; // On mémorise cette information
    }

    // Pile pour le parcours en profondeur;
    dequeue_gen* thestack = create_dequeue_gen();

    // Un avl qui contiendra les états de l'automate produit déjà rencontrés
    avlnode* thetree = NULL;

    // Compteur du nombre d'états créés
    uint num = 0;

    // On créé également un tas binaire dans lequel on va mémoriser les numéros des états finaux (on utilise un tas pour les trier en même temps)
    binheap* heapfinals = create_binheap(&fcmp_int);

    // Le nouveau NFA (on va directement construire la liste des états initiaux)
    nfa* PROD;
    MALLOC(PROD, 1);                    // Création du NFA
    PROD->alphabet = nfa_duplicate_alpha(A1);

    PROD->initials = create_dequeue(); // Création de la liste contenant les états initiaux

    // Création et empilement des états initiaux (paires d'états initiaux)
    for (uint i1 = 0; i1 < size_dequeue(A1->initials); i1++) {
        for (uint i2 = 0; i2 < size_dequeue(A2->initials); i2++) {
            intstate* newini;
            MALLOC(newini, 1);
            newini->q1 = lefread_dequeue(A1->initials, i1);
            newini->q2 = lefread_dequeue(A2->initials, i2);
            newini->size_alpha = thealph;
            newini->num = num;
            rigins_dequeue(num, PROD->initials);
            // printf("num: %d\n", num);
            num++;
            rigins_dequeue_gen(newini, thestack);
            thetree = avl_insert(newini, thetree, compintstate, NULL);
        }
    }

    // Parcours en profondeur
    while (!isempty_dequeue_gen(thestack)) {
        // Récupération de l'état qu'on va traiter
        intstate* thestate = (intstate*)rigpull_dequeue_gen(thestack);
        // printf("The state : %d,%d\n", thestate->q1, thestate->q2);

        // Mémorisation du numéro si l'état est final
        if (mem_dequeue_sorted(thestate->q1, A1->finals, NULL) && mem_dequeue_sorted(thestate->q2, A2->finals, NULL)) {
            push_binheap(heapfinals, &thestate->num);
        }

        // Calcul des transitions classiques à partir de thestate
        MALLOC(thestate->nt, thealph);
        MALLOC(thestate->tr, thealph);
        for (uint a = 0; a < comalph; a++) {
            // Nombre de transitions sortantes étiquetées par a
            thestate->nt[a] = size_dequeue(A1->trans->edges[thestate->q1][a]) *
                size_dequeue(A2->trans->edges[thestate->q2][a]);
            MALLOC(thestate->tr[a], thestate->nt[a]);
            uint k = 0; // Compteur des transitions traitées

            // Pour chaque transition
            for (uint i1 = 0; i1 < size_dequeue(A1->trans->edges[thestate->q1][a]); i1++) {
                for (uint i2 = 0; i2 < size_dequeue(A2->trans->edges[thestate->q2][a]); i2++) {
                    // On crée un (potentiel) nouvel état
                    intstate* newstate;
                    MALLOC(newstate, 1);
                    newstate->q1 = lefread_dequeue(A1->trans->edges[thestate->q1][a], i1);
                    newstate->q2 = lefread_dequeue(A2->trans->edges[thestate->q2][a], i2);

                    // printf("Trans: %c-> %d,%d\n", a, newstate->q1, newstate->q2);
                    // On regarde si cet état a en fait déjà été construit
                    avlnode* thenode = avl_search(newstate, thetree, compintstate);
                    if (thenode == NULL) {                                                  // Si cet état n'a pas encore été construit
                        newstate->size_alpha = thealph;
                        newstate->nt = NULL;
                        newstate->tr = NULL;
                        newstate->num = num;                                         // On affecte un numéro à ce nouvel état
                        num++;

                        thestate->tr[a][k] = newstate;                                      // On peut affecter la kième transition
                        k++;
                        thetree = avl_insert(newstate, thetree, compintstate, NULL); // On mémorise le nouvel état dans l'ensemble des états
                        // rencontrés
                        rigins_dequeue_gen(newstate, thestack);                                           // Ce nouvel état est à traiter
                    }
                    else {                                                                  // Sinon, cet état a déjà été construit
                        // printf("not NULL\n");
                        thestate->tr[a][k] = ((intstate*)thenode->value);                  // On peut affecter la kième transition
                        k++;
                        free(newstate);                                                     // On supprime la copie qu'on vient de créer
                    }
                }
            }
        }
        // Calcul des transitions epsilon à partir de thestate
        if (has_etrans)
        {
            // Nombre de transitions epsilon
            thestate->nte = size_dequeue(A1->trans_e->edges[thestate->q1]) * size_dequeue(A2->trans_e->edges[thestate->q2]);
            MALLOC(thestate->tre, thestate->nte);
            uint k = 0; // Compteur des transitions traitées

            // Pour chaque transition
            for (uint i1 = 0; i1 < size_dequeue(A1->trans_e->edges[thestate->q1]); i1++)
            {
                for (uint i2 = 0; i2 < size_dequeue(A2->trans_e->edges[thestate->q2]); i2++)
                {
                    // On crée un (potentiel) nouvel état
                    intstate* newstate;
                    MALLOC(newstate, 1);
                    newstate->q1 = lefread_dequeue(A1->trans_e->edges[thestate->q1], i1);
                    newstate->q2 = lefread_dequeue(A2->trans_e->edges[thestate->q2], i2);

                    // On regarde si cet état a en fait déjà été construit
                    avlnode* thenode = avl_search(newstate, thetree, &compintstate);
                    if (thenode == NULL)
                    { // Si cet état n'a pas encore été construit
                        newstate->size_alpha = thealph;
                        newstate->num = num; // On affecte un numéro à ce nouvel état
                        num++;

                        thestate->tre[k] = newstate; // On peut affecter la kième epsilon transition
                        k++;
                        thetree = avl_insert(newstate, thetree, &compintstate, NULL); // On mémorise le nouvel état dans l'ensemble des états rencontrés
                        rigins_dequeue_gen(newstate, thestack);                                // Ce nouvel état est à traiter
                    }
                    else
                    { // Sinon, cet état a déjà été construit
                        // printf("not NULL\n");
                        thestate->tre[k] = ((intstate*)thenode->value); // On peut affecter la kième transition
                        k++;
                        free(newstate); // On supprime la copie qu'on vient de créer
                    }
                }
            }
        }

        // Calcul des transitions inverses
        if (has_itrans)
        {
            MALLOC(thestate->nti, thealph);
            MALLOC(thestate->tri, thealph);
            for (uint a = 0; a < comalph; a++)
            {
                // Nombre de transitions inverses sortantes étiquetées par a
                thestate->nti[a] = size_dequeue(A1->trans_i->edges[thestate->q1][a]) * size_dequeue(A2->trans_i->edges[thestate->q2][a]);
                MALLOC(thestate->tri[a], thestate->nti[a]);
                uint k = 0; // Compteur des transitions traitées

                // Pour chaque transition inverse
                for (uint i1 = 0; i1 < size_dequeue(A1->trans_i->edges[thestate->q1][a]); i1++)
                {
                    for (uint i2 = 0; i2 < size_dequeue(A2->trans_i->edges[thestate->q2][a]); i2++)
                    {
                        // On crée un (potentiel) nouvel état
                        intstate* newstate;
                        MALLOC(newstate, 1);
                        newstate->q1 = lefread_dequeue(A1->trans_i->edges[thestate->q1][a], i1);
                        newstate->q2 = lefread_dequeue(A2->trans_i->edges[thestate->q2][a], i2);

                        // On regarde si cet état a en fait déjà été construit
                        avlnode* thenode = avl_search(newstate, thetree, compintstate);
                        if (thenode == NULL)
                        { // Si cet état n'a pas encore été construit
                            newstate->size_alpha = thealph;
                            newstate->nt = NULL;
                            newstate->tr = NULL;
                            newstate->num = num; // On affecte un numéro à ce nouvel état
                            num++;

                            thestate->tri[a][k] = newstate; // On peut affecter la kième transition
                            k++;
                            thetree = avl_insert(newstate, thetree, &compintstate, NULL); // On mémorise le nouvel état dans l'ensemble des états rencontrés
                            rigins_dequeue_gen(newstate, thestack);                                // Ce nouvel état est à traiter
                        }
                        else
                        { // Sinon, cet état a déjà été construit
                            // printf("not NULL\n");
                            thestate->tri[a][k] = ((intstate*)thenode->value); // On peut affecter la kième transition
                            k++;
                            free(newstate); // On supprime la copie qu'on vient de créer
                        }
                    }
                }
            }
        }
    }
    // Le parcours en profondeur est terminé, plus besoin de la pile
    delete_dequeue_gen(thestack);

    // Construction du nouveau NFA (continued)
    PROD->trans = create_lgraph_noedges(num, thealph); // Création du graph
    if (has_etrans)
    {
        PROD->trans_e = create_graph_noedges(num);
    }
    else
    {
        PROD->trans_e = NULL;
    }
    if (has_itrans)
    {
        PROD->trans_i = create_lgraph_noedges(num, thealph);
    }
    else
    {
        PROD->trans_i = NULL;
    }

    if (names) {                                       // Si on doit mémoriser les noms
        MALLOC(PROD->state_names, num);
    }
    else {
        PROD->state_names = NULL;
    }

    // Création des transitions et des noms
    inter_avl_to_table(thetree, PROD, names, A1->state_names, A2->state_names);


    // Il ne reste plus qu'à créer et remplir la liste des états finaux
    PROD->finals = create_dequeue(); // Création de la liste contenant les états finaux
    while (!isempty_binheap(heapfinals)) {
        rigins_dequeue(*(uint*)popmin_binheap(heapfinals), PROD->finals);
    }
    delete_binheap(heapfinals);
    avl_free_strong(thetree, &free_intstate);
    // print_dequeue(PROD->initials);
    delete_nfa(A1);
    delete_nfa(A2);
    return PROD;
}





static int comp_prod_pairs(void* s, void* t) {
    if (((nfa_prod_pair*)s)->q1 < ((nfa_prod_pair*)t)->q1) {
        return 1;
    }
    else if (((nfa_prod_pair*)s)->q1 > ((nfa_prod_pair*)t)->q1) {
        return -1;
    }
    else {
        if (((nfa_prod_pair*)s)->q2 < ((nfa_prod_pair*)t)->q2) {
            return 1;
        }
        else if (((nfa_prod_pair*)s)->q2 > ((nfa_prod_pair*)t)->q2) {
            return -1;
        }
        else {
            return 0;
        }
    }
}


static void nfa_intersec_reach_fill(avlnode* tree, nfa_prod_pair* array, uint* i) {
    if (tree == NULL) {
        return;
    }
    nfa_intersec_reach_fill(tree->left, array, i);
    array[*i] = *((nfa_prod_pair*)tree->value);
    *i += 1;
    free(tree->value);
    nfa_intersec_reach_fill(tree->right, array, i);
    free(tree);
}







nfa_prod_pair* nfa_intersec_reach(nfa* A1, nfa* A2, uint s1, uint s2, uint* size) {


    if ((A1->trans_e && !A2->trans_e) || (A2->trans_e && !A1->trans_e)) {
        return NULL;
    }

    if ((A1->trans_i && !A2->trans_i) || (A2->trans_i && !A1->trans_i)) {
        return NULL;
    }

    if (A1->trans->size_alpha != A2->trans->size_alpha) {
        return NULL;
    }



    // Pile pour le parcours en profondeur;
    dequeue_gen* thequeue = create_dequeue_gen();



    // Un avl qui contiendra les états de l'automate produit déjà rencontrés
    avlnode* visited = NULL;

    nfa_prod_pair* newini;
    MALLOC(newini, 1);
    newini->q1 = s1;
    newini->q2 = s2;
    lefins_dequeue_gen(newini, thequeue);


    *size = 0;
    // Parcours en largeur
    while (!isempty_dequeue_gen(thequeue)) {
        // Récupération de l'état qu'on va traiter
        nfa_prod_pair* thestate = (nfa_prod_pair*)rigpull_dequeue_gen(thequeue);

        avlnode* thenode = avl_search(thestate, visited, comp_prod_pairs);
        if (thenode) {
            free(thestate);
            continue;
        }
        visited = avl_insert(thestate, visited, comp_prod_pairs, NULL);
        *size += 1;

        // Calcul des transitions classiques à partir de thestate
        for (uint a = 0; a < A1->trans->size_alpha; a++) {
            for (uint i1 = 0; i1 < size_dequeue(A1->trans->edges[thestate->q1][a]); i1++) {
                uint r1 = lefread_dequeue(A1->trans->edges[thestate->q1][a], i1);
                for (uint i2 = 0; i2 < size_dequeue(A2->trans->edges[thestate->q2][a]); i2++) {
                    uint r2 = lefread_dequeue(A2->trans->edges[thestate->q2][a], i2);
                    // On crée un nouvel état
                    intstate* newstate;
                    MALLOC(newstate, 1);
                    newstate->q1 = r1;
                    newstate->q2 = r2;
                    lefins_dequeue_gen(newstate, thequeue);
                }
            }
        }

        if (A1->trans_e) {
            for (uint i1 = 0; i1 < size_dequeue(A1->trans_e->edges[thestate->q1]); i1++) {
                uint r1 = lefread_dequeue(A1->trans_e->edges[thestate->q1], i1);
                for (uint i2 = 0; i2 < size_dequeue(A2->trans_e->edges[thestate->q2]); i2++) {
                    uint r2 = lefread_dequeue(A2->trans_e->edges[thestate->q2], i2);
                    // On crée un nouvel état
                    intstate* newstate;
                    MALLOC(newstate, 1);
                    newstate->q1 = r1;
                    newstate->q2 = r2;
                    lefins_dequeue_gen(newstate, thequeue);
                }
            }
        }


        // Calcul des transitions inverses
        if (A1->trans_i)
        {
            for (uint a = 0; a < A1->trans->size_alpha; a++)
            {   // Pour chaque transition inverse
                for (uint i1 = 0; i1 < size_dequeue(A1->trans_i->edges[thestate->q1][a]); i1++)
                {
                    uint r1 = lefread_dequeue(A1->trans_i->edges[thestate->q1][a], i1);
                    for (uint i2 = 0; i2 < size_dequeue(A2->trans_i->edges[thestate->q2][a]); i2++)
                    {
                        uint r2 = lefread_dequeue(A2->trans_i->edges[thestate->q2][a], i2);
                        // On crée un (potentiel) nouvel état
                        intstate* newstate;
                        MALLOC(newstate, 1);
                        newstate->q1 = r1;
                        newstate->q2 = r2;
                        lefins_dequeue_gen(newstate, thequeue);
                    }
                }
            }
        }
    }
    // Le parcours en profondeur est terminé, plus besoin de la pile
    delete_dequeue_gen(thequeue);
    nfa_prod_pair* ret;
    MALLOC(ret, *size);

    uint i = 0;
    nfa_intersec_reach_fill(visited, ret, &i);
    return ret;

}
