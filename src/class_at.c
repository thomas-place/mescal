#include "class_at.h"

/**************/
/* Membership */
/**************/

bool is_at_mono(p_cayley M, FILE* out)
{
    bool idem = is_idem_mono(M, out);
    bool comm = is_comm_mono(M, out);
    if (comm && idem)
    {
        return true;
    }
    return false;
}

bool is_lt_mono(p_cayley M, p_green G, p_orbits L, FILE* out)
{
    if (out) {
        fprintf(stdout, "#### Computing the DD-orbits of the morphism and checking if they are idempotent and commutative.\n");
    }

    // Cas où le neutre possède un antécédent non-vide: on se ramène à AT-membership
    if (cayley_neutstrict(M, G)) {
        if (out) {
            fprintf(stdout, "#### Since the neutral element 1 has a nonempty antecedent, the DD-orbit of 1 is the whole monoid.\n");
            fprintf(stdout, "#### Therefore, it suffices to check that the monoid is idempotent and commutative.\n");
        }
        return is_at_mono(M, stdout);
    }

    // Cas où le seul antécédent de 1 est le mot vide
    // Tableau qui sauvegardera les idempotents déjà traité avec un autre (cas où f appartient à eMe pour un e déjà traité)
    bool edone[M->trans->size_graph];
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        edone[lefread_vertices(M->idem_list, i)] = false;
    }

    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        if (out)
        {
            print_sep_line(100, out);
            fprintf(out, "#### DD-orbit of the idempotent ");
            cayley_print_name(M, e, out);
            printf(".\n");
            print_full_green_sub(L->orbits[i], false, out);
        }
        if (!edone[e])
        {
            // On sauvegarde tous les idempotents dans eMe
            for (uint j = 0; j < L->orbits[i]->size; j++)
            {
                edone[L->orbits[i]->sub_to_mono[j]] = true;
            }
            // On teste si eMe est idempotent commutatif
            bool idem = is_idem_submono(L->orbits[i], out);
            bool comm = is_comm_submono(L->orbits[i], out);
            if (!idem || !comm)
            {
                return false;
            }
        }
        else
        {
            // Sinon on sait déjà que eMe est idempotent commuatif (déjà traité comme sous-semigroup d'un fMf précédent)
            if (out)
            {
                fprintf(out, "#### The monoid is idempotent.\n");
                fprintf(out, "#### The monoid is commutative.\n");
            }
        }
    }
    return true;
}

bool is_att_mono(p_cayley M, p_green G, FILE* out)
{
    bool aper = is_aperiodic_mono(M, G, out);
    bool comm = is_comm_mono(M, out);
    if (comm && aper)
    {
        return true;
    }
    return false;
}

bool is_ltt_mono(p_cayley M, p_green G, FILE* out)
{
    if (!is_aperiodic_mono(M, G, out)) {
        return false;
    }
    if (out) {
        fprintf(out, "#### Checking whether the image S of A⁺ satisfies the equation erfsetf = etfserf\n");
        fprintf(out, "     for all r,s,t ∊ S and e,f ∊ E(S).\n");
    }
    if (cayley_neutstrict(M, G)) {
        fprintf(out, "#### Since the neutral element 1 has a nonempty antecedent, the equation holds\n");
        fprintf(out, "     if and only if the monoid is commutative.\n");
        return is_comm_mono(M, out);
    }
    bool edone[M->trans->size_graph];
    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        edone[lefread_vertices(M->idem_list, i)] = false;
    }

    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        // Si n'a pas déjà été traité comme sous-produit d'un idempotent précédent
        if (!edone[e])
        {
            bool fdone[M->trans->size_graph];
            for (uint j = i; j < size_vertices(M->idem_list); j++)
            {
                fdone[lefread_vertices(M->idem_list, j)] = false;
            }

            // On traite d'abord le cas f = e. On vérifie d'abord que eMe est commutatif
            p_vertices eM = compute_r_ideal(M, e, NULL);
            p_vertices Me = compute_l_ideal(M, e, NULL);
            p_vertices eMe = make_inter_sorted_vertices(eM, Me);
            for (uint p = 0; p < size_vertices(eMe); p++)
            {
                edone[lefread_vertices(eMe, p)] = true;
            }
            for (uint k = 0; k < size_vertices(eMe) - 1; k++)
            {
                for (uint l = k + 1; l < size_vertices(eMe); l++)
                {
                    uint s = lefread_vertices(eMe, k);
                    uint t = lefread_vertices(eMe, l);
                    if (cayley_mult(M, s, t) != cayley_mult(M, t, s))
                    {
                        if (out)
                        {
                            fprintf(out, "#### The semigroup does not satisfy the equation.\n");
                            fprintf(out, "#### For instance, the equality erfsetf = etfserf fails for e = ");
                            cayley_print_name(M, e, out);
                            fprintf(out, ", f = ");
                            cayley_print_name(M, e, out);
                            fprintf(out, ", r = ");
                            cayley_print_name(M, s, out);
                            fprintf(out, ", s = ");
                            cayley_print_name(M, e, out);
                            fprintf(out, " and t = ");
                            cayley_print_name(M, t, out);
                            fprintf(out, ".\n");
                        }
                        return false;
                    }
                }
            }
            delete_vertices(eMe);

            // On passe aux idempotents f > e
            for (uint j = i + 1; j < size_vertices(M->idem_list); j++)
            {
                uint f = lefread_vertices(M->idem_list, j);
                if (!fdone[f])
                {
                    p_vertices fM = compute_r_ideal(M, f, NULL);
                    p_vertices Mf = compute_l_ideal(M, f, NULL);
                    p_vertices fMf = make_inter_sorted_vertices(fM, Mf);
                    for (uint p = 0; p < size_vertices(fMf); p++)
                    {
                        fdone[lefread_vertices(fMf, p)] = true;
                    }
                    delete_vertices(fMf);
                    p_vertices eMf = make_inter_sorted_vertices(eM, Mf);
                    p_vertices fMe = make_inter_sorted_vertices(fM, Me);
                    delete_vertices(fM);
                    delete_vertices(Mf);
                    for (uint k = 0; k < size_vertices(eMf) - 1; k++)
                    {
                        for (uint l = k + 1; l < size_vertices(eMf); l++)
                        {
                            for (uint m = 0; m < size_vertices(fMe); m++)
                            {
                                uint r = lefread_vertices(eMf, k);
                                uint s = lefread_vertices(fMe, m);
                                uint t = lefread_vertices(eMf, l);
                                if (cayley_mult_gen(M, 3, r, s, t) != cayley_mult_gen(M, 3, t, s, r))
                                {
                                    if (out)
                                    {
                                        fprintf(out, "#### The semigroup does not satisfy the equation erfsetf = etfse.\n");
                                        fprintf(out, "#### For instance, the equality fails for e = ");
                                        cayley_print_name(M, e, out);
                                        fprintf(out, ", f = ");
                                        cayley_print_name(M, f, out);
                                        fprintf(out, ", r = ");
                                        cayley_print_name(M, r, out);
                                        fprintf(out, ", s = ");
                                        cayley_print_name(M, s, out);
                                        fprintf(out, " and t = ");
                                        cayley_print_name(M, t, out);
                                        fprintf(out, ".\n");
                                    }
                                    return false;
                                }
                            }
                        }
                    }
                    delete_vertices(eMf);
                    delete_vertices(fMe);
                }
            }
            delete_vertices(eM);
            delete_vertices(Me);
        }
    }
    if (out)
    {
        fprintf(out, "#### The semigroup satisfies the equation erfsetf = etfse for all r,s,t ∊ S and e,f ∊ E(S).\n");
    }
    return true;
}

/*************/
/* AT-orbits */
/*************/

// Calcule l'ensemble des lettres qui étiquettent des transitions de la SCC de e
// Retour sous la forme d'un tableau de Booléens
bool* compute_maxalph_scc(p_cayley M, p_parti SCCS, uint e)
{
    bool* res;
    MALLOC(res, M->trans->size_alpha);
    for (uint a = 0; a < M->trans->size_alpha; a++)
    {
        res[a] = false;
    }
    uint c = SCCS->numcl[e];
    for (uint i = 0; i < size_vertices(SCCS->cl[c]); i++)
    {
        for (uint a = 0; a < M->trans->size_alpha; a++)
        {
            if (SCCS->numcl[M->trans->edges[lefread_vertices(SCCS->cl[c], i)][a]] == c)
            {
                res[a] = true;
            }
        }
    }
    return res;
}

// Fusionne deux alphabets dans un nouveau et teste en même temps si ces alphabets étaient égaux
static bool merge_subalph(p_cayley M, bool* m, bool* s1, bool* s2)
{
    bool res = true;
    for (uint a = 0; a < M->trans->size_alpha; a++)
    {
        if (s1[a] != s2[a])
        {
            res = false;
        }
        m[a] = s1[a] && s2[a];
    }
    return res;
}

// Retourne true si e et f ont tous les deux un antécédent d'alphabet alph
static bool dgraph_seek_alph(p_dgraph G, uint ini, uint e, uint f, bool* alph)
{
    // Tableau pour mémoriser les sommets visités
    bool visited[G->size_graph];
    for (uint q = 0; q < G->size_graph; q++)
    {
        visited[q] = false;
    }

    // On y mémorise le sommet de départ
    visited[ini] = true;

    p_vertices thestack = create_vertices();
    rigins_vertices(ini, thestack);

    // Parcours DFS
    while (!isempty_vertices(thestack))
    {
        if (visited[e] && visited[f])
        {
            delete_vertices(thestack);
            return true;
        }
        uint q = rigpull_vertices(thestack);

        if (alph)
        {
            for (uint a = 0; a < G->size_alpha; a++)
            {
                if (alph[a] && !visited[G->edges[q][a]])
                {
                    visited[G->edges[q][a]] = true;
                    rigins_vertices(G->edges[q][a], thestack);
                }
            }
        }
        else
        {
            for (uint a = 0; a < G->size_alpha; a++)
            {

                if (!visited[G->edges[q][a]])
                {
                    visited[G->edges[q][a]] = true;
                    rigins_vertices(G->edges[q][a], thestack);
                }
            }
        }
    }

    delete_vertices(thestack);
    return false;
}

// Calcule l'alphabet commun maximum des idempotents e et f
// Le retour se fait sous la forme d'un tableau de Booléens
// Si il n'y a pas d'alphabet commun, on retourne NULL
bool* compute_maxalph_com_scc(p_cayley M, p_parti SCCS, uint e, uint f)
{
    // Alphabet de la SCC de e
    bool* ealph = compute_maxalph_scc(M, SCCS, e);
    // Alphabet de la SCC de f
    bool* falph = compute_maxalph_scc(M, SCCS, f);
    // print_booltab_alph(ealph, M->trans->size_alpha, stdout);
    // print_booltab_alph(falph, M->trans->size_alpha, stdout);
    bool* merge;
    MALLOC(merge, M->trans->size_alpha);

    // Tant que les deux alphabets sont distincts, on va considérer leur intersection (le plus grand candidat pour un alphabet commun)
    while (!merge_subalph(M, merge, ealph, falph))
    {

        // On teste si e et f restent tous les deux atteignables en n'utilisant que l'intersection (mais pas forcément en entier)
        if (!dgraph_seek_alph(M->trans, ONE, e, f, merge))
        {
            // Si ce n'est pas possible, e et f n'ont pas d'alphabet commun, on retourne NULL
            free(merge);
            free(ealph);
            free(falph);
            return NULL;
        }

        // print_booltab_alph(merge, M->trans->size_alpha, stdout);

        // On va maintenant calculer les alphabets des SCCS de e et f quand on restreint l'alphabet à merge
        p_parti rSCCS = dtarjan_alph(M->trans, merge);
        // printf("size_set: %d, size_par: %d\n", rSCCS->size_set, rSCCS->size_par);

        for (uint a = 0; a < M->trans->size_alpha; a++)
        {
            // On réunitialise la lettre a pour ealph et falph
            ealph[a] = false;
            falph[a] = false;

            // Si la lettre a est dans l'intersection, on cherche une tarnsition interne étiquetée par a dans les SCCS restreintes à merge de e et f
            if (merge[a])
            {
                uint i = 0;
                // On passe en revue tous les états de la SCC de e tant qu'on a pas vu une transition a
                while (!ealph[a] && i < size_vertices(rSCCS->cl[rSCCS->numcl[e]]))
                {
                    // Si on a une transition a qui reste dans la SCC de e
                    if (rSCCS->numcl[M->trans->edges[lefread_vertices(rSCCS->cl[rSCCS->numcl[e]], i)][a]] == rSCCS->numcl[e])
                    {
                        ealph[a] = true;
                    }
                    i++;
                }

                i = 0;
                // On passe maintenant en revue tous les états de la SCC de f tant qu'on a pas vu une transition a
                while (!falph[a] && i < size_vertices(rSCCS->cl[rSCCS->numcl[f]]))
                {
                    if (rSCCS->numcl[M->trans->edges[lefread_vertices(rSCCS->cl[rSCCS->numcl[f]], i)][a]] == rSCCS->numcl[f])
                    {
                        falph[a] = true;
                    }
                    i++;
                }
            }
        }
        // print_booltab_alph(ealph, M->trans->size_alpha, stdout);
        // print_booltab_alph(falph, M->trans->size_alpha, stdout);
        delete_parti(rSCCS);
    }
    // printf("\n");
    free(ealph);
    free(falph);
    return merge;
}

// Calcule la AT-orbite associée à l'idempotent e
p_green_sub compute_one_atorb(p_cayley M, p_green G, p_parti SCCS, uint e)
{
    bool* malph = compute_maxalph_scc(M, SCCS, e);
    p_green_sub S = init_submono(M, G);

    // Calcul de l'ensemble des éléments
    p_vertices eM = compute_r_ideal_alph(M, e, malph, NULL);
    p_vertices Me = compute_l_ideal_alph(M, e, malph, NULL);
    p_vertices eMe = make_inter_sorted_vertices(eM, Me);

    S->size = size_vertices(eMe);

    MALLOC(S->sub_to_mono, size_vertices(eMe));
    MALLOC(S->mono_in_sub, M->trans->size_graph);
    MALLOC(S->mono_to_sub, M->trans->size_graph);

    for (uint q = 0; q < M->trans->size_graph; q++)
    {
        S->mono_in_sub[q] = false;
    }
    for (uint i = 0; i < size_vertices(eMe); i++)
    {
        S->sub_to_mono[i] = lefread_vertices(eMe, i);
        S->mono_to_sub[lefread_vertices(eMe, i)] = i;
        S->mono_in_sub[lefread_vertices(eMe, i)] = true;
    }
    S->neut = S->mono_to_sub[e];
    delete_vertices(eM);
    delete_vertices(Me);
    delete_vertices(eMe);

    // Calcul des listes d'idempotents
    S->idem_list = create_vertices();
    MALLOC(S->idem_array, S->size);
    for (uint s = 0; s < S->size; s++)
    {
        if (M->idem_array[S->sub_to_mono[s]])
        {
            S->idem_array[s] = true;
            rigins_vertices(s, S->idem_list);
        }
        else
        {
            S->idem_array[s] = false;
        }
    }

    // Calcul des relations de Green
    // On va commencer par calculer les classes R,L,J dans le monoide original en restreignant l'alphabet à malph
    p_parti right = dtarjan_alph(M->trans, malph);

    compute_left_cayley(M);
    p_parti left = dtarjan_alph(M->leftgraph, malph);

    p_lgraph jgraph = ldgraphs_to_lgraph(0, 2, 2, M->trans, M->leftgraph);
    p_parti thej = ltarjan_alph(jgraph, malph);
    delete_lgraph(jgraph);

    // On peut maintenant remplir les relations de Green de la AT-orbit
    MALLOC(S->rels, 1);
    uint num;

    // R-classes
    MALLOC(S->rels->RCL, 1);
    S->rels->RCL->size_set = S->size;
    MALLOC(S->rels->RCL->numcl, S->size);
    num = 0;
    for (uint i = 0; i < right->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(right->cl[i]); j++)
        {
            if (S->mono_in_sub[lefread_vertices(right->cl[i], j)])
            {
                inter = true;
                S->rels->RCL->numcl[S->mono_to_sub[lefread_vertices(right->cl[i], j)]] = num;
            }
        }
        if (inter)
        {
            num++;
        }
    }

    MALLOC(S->rels->RCL->cl, num);
    S->rels->RCL->size_par = num;
    for (uint i = 0; i < num; i++)
    {
        S->rels->RCL->cl[i] = create_vertices();
    }

    for (uint s = 0; s < S->size; s++)
    {
        rigins_vertices(s, S->rels->RCL->cl[S->rels->RCL->numcl[s]]);
    }

    // L-classes
    MALLOC(S->rels->LCL, 1);
    S->rels->LCL->size_set = S->size;
    MALLOC(S->rels->LCL->numcl, S->size);
    num = 0;
    for (uint i = 0; i < left->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(left->cl[i]); j++)
        {
            if (S->mono_in_sub[lefread_vertices(left->cl[i], j)])
            {
                inter = true;
                S->rels->LCL->numcl[S->mono_to_sub[lefread_vertices(left->cl[i], j)]] = num;
            }
        }
        if (inter)
        {
            num++;
        }
    }
    MALLOC(S->rels->LCL->cl, num);
    S->rels->LCL->size_par = num;
    for (uint i = 0; i < num; i++)
    {
        S->rels->LCL->cl[i] = create_vertices();
    }

    for (uint s = 0; s < S->size; s++)
    {
        rigins_vertices(s, S->rels->LCL->cl[S->rels->LCL->numcl[s]]);
    }

    // J-classes
    MALLOC(S->rels->JCL, 1);
    S->rels->JCL->size_set = S->size;
    MALLOC(S->rels->JCL->numcl, S->size);
    num = 0;
    for (uint i = 0; i < thej->size_par; i++)
    {
        bool inter = false;
        for (uint j = 0; j < size_vertices(thej->cl[i]); j++)
        {
            if (S->mono_in_sub[lefread_vertices(thej->cl[i], j)])
            {
                inter = true;
                S->rels->JCL->numcl[S->mono_to_sub[lefread_vertices(thej->cl[i], j)]] = num;
            }
        }
        if (inter)
        {
            num++;
        }
    }
    MALLOC(S->rels->JCL->cl, num);
    S->rels->JCL->size_par = num;
    for (uint i = 0; i < num; i++)
    {
        S->rels->JCL->cl[i] = create_vertices();
    }

    for (uint s = 0; s < S->size; s++)
    {
        rigins_vertices(s, S->rels->JCL->cl[S->rels->JCL->numcl[s]]);
    }

    // H-classes
    h_green_compute(S->rels);

    // Groupes et J-classes régulières
    gr_green_compute(S->idem_list, S->rels);

    delete_parti(left);
    delete_parti(right);
    delete_parti(thej);
    free(malph);
    return S;
}

// Calcule toutes les AT-orbites associées
p_orbits compute_all_atorbs(p_cayley M, p_green G)
{
    p_orbits res = init_orbits(init_submono(M, G));
    res->nb_idems = size_vertices(M->idem_list);
    MALLOC(res->orbits, res->nb_idems);
    res->oneonly = false;
    p_parti SCCS = dtarjan(M->trans);

    for (uint i = 0; i < res->nb_idems; i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        res->orbits[i] = compute_one_atorb(M, G, SCCS, e);
    }
    delete_parti(SCCS);
    return res;
}

// Affiche toutes les orbites enregistrées
void print_all_atorbs(p_cayley M, p_orbits L, bool title, FILE* out)
{

    if (title)
    {
        print_title_box(0, true, out, 1, "AT-orbits of the morphism.");
    }

    fprintf(out, "#### The list of all idempotents:\n");
    cayley_print_idems(M, out);

    fprintf(out, "#### Printing the AT-orbits of all idempotents:\n");

    for (uint i = 0; i < L->nb_idems; i++)
    {
        print_sep_line(100, out);
        fprintf(out, "#### AT-orbit of the idempotent ");
        cayley_print_name(M, lefread_vertices(M->idem_list, i), out);
        printf(".\n");

        print_full_green_sub(L->orbits[i], false, out);
    }
}

/************/
/* Covering */
/************/

// Structure pout stocker les paires rencontrée dans le DFA à l'intérieur d'un AVL
struct at_explore
{
    // Élément du monoide
    uint elem;

    // // Taille de l'alphabet
    // uint size_alpha;

    // Alphabet calculé: le ième bit de poids faible détermine si la ième lettre est présente
    ulong thealph;

    // // Pour chaque lettre, un pointeur sur l'état atteint
    // struct at_explore **next;
};

typedef struct at_explore* p_at_explore;

// Fonction de comparaison pour les AVLs
static int at_explore_comp(void* e1, void* e2)
{
    if (((p_at_explore)e1)->thealph < ((p_at_explore)e2)->thealph)
    {
        return 1;
    }
    else if (((p_at_explore)e1)->thealph > ((p_at_explore)e2)->thealph)
    {
        return -1;
    }
    else
    {
        if (((p_at_explore)e1)->elem < ((p_at_explore)e2)->elem)
        {
            return 1;
        }
        else if (((p_at_explore)e1)->elem > ((p_at_explore)e2)->elem)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

// Extraction de l'information de l'AVL une fois le DFS fini
static void at_avl_to_table(p_avlnode thetree, p_vertices* table)
{
    if (thetree == NULL)
    {
        return;
    }
    else
    {
        at_avl_to_table(thetree->left, table);
        rigins_vertices(((p_at_explore)thetree->value)->elem, table[((p_at_explore)thetree->value)->thealph]);

        at_avl_to_table(thetree->right, table);
        free(thetree);
    }
}

p_vertices* at_table_cayley(p_cayley M)
{
    // Création de la pile pour le DFS
    p_stack thestack = create_stack();

    // Création de l'AVL pour mémoriser les sommets déjà visités
    p_avlnode thetree = NULL;

    // Création du sommet initial
    p_at_explore inode;
    MALLOC(inode, 1);
    inode->elem = ONE;
    // inode->size_alpha = M->trans->size_alpha;
    inode->thealph = 0; // Le mot vide a un alphabet vide
    // inode->next = NULL; // On ne connait pas encore les sommets adjacents

    push(inode, thestack);
    thetree = avl_insert(inode, thetree, &at_explore_comp);

    while (!isempty_stack(thestack))
    {
        // On prend un sommet non-traité
        p_at_explore thenode = pop(thestack);
        // MALLOC(thenode->next, M->trans->size_alpha);
        for (uint a = 0; a < M->trans->size_alpha; a++)
        {
            // printf("Letter %d\n", a);
            // Création du potentiel nouveau sommet
            p_at_explore newnode;
            MALLOC(newnode, 1);
            newnode->elem = M->trans->edges[thenode->elem][a];
            ulong mask = 1 << a;

            newnode->thealph = thenode->thealph | mask;
            // newnode->size_alpha = M->trans->size_alpha;
            // newnode->next = NULL;

            // Recherche dans les sommets déjà visités
            p_avlnode found = avl_search(newnode, thetree, &at_explore_comp);

            if (found == NULL)
            {
                // printf("test 1\n");
                // thenode->next[a] = newnode;
                push(newnode, thestack);
                thetree = avl_insert(newnode, thetree, &at_explore_comp);
                // printf("test 11\n");
            }
            else
            {
                // printf("test 2\n");

                // thenode->next[a] = found->value;
                free(newnode);

                // printf("test 22\n");
            }
        }
    }

    // Création de la table à retourner
    p_vertices* theresult;
    ulong result_size = 1 << M->trans->size_alpha;
    MALLOC(theresult, result_size);
    for (uint i = 0; i < result_size; i++)
    {
        theresult[i] = create_vertices();
    }
    at_avl_to_table(thetree, theresult);
    return theresult;
}

void print_at_table(p_cayley M, p_vertices* table, FILE* out)
{
    uint max_length = 100; // Longueur d'une ligne
    print_title_box(100, false, out, 2, "Computation on the syntactic morphism. For each sub-alphabet, lists the elements", "which are the image of a word with this sub-alphabet.");

    // Récupération d'information pour l'alignement
    ulong table_size = 1 << M->trans->size_alpha;    // Nombre de sous-alphabets
    uint prefix_size = M->trans->size_alpha * 2 + 2; // Longueur-maximale de l'affichage d'un sous-alphabet

    // L'unique élément d'antécédent eps (d'alphabet vide) est 1
    // fprintf(out, "Number of sub-alphabets: %lu\n", table_size);
    fprintf(out, "│∅");
    for (uint i = 0; i < prefix_size - 1; i++)
    {
        fprintf(out, " ");
    }
    fprintf(out, ": 1");
    for (uint i = 0; i < max_length - prefix_size - 3; i++)
    {
        fprintf(out, " ");
    }
    fprintf(out, "│\n");

    // Pour les alphabets non-vides
    for (ulong i = 1; i < table_size; i++)
    {
        fprintf(out, "│{");
        ulong j = i;
        uint length = 1;
        for (uint a = 0; a < M->trans->size_alpha; a++)
        {
            if (j % 2 == 1)
            {
                length++;
                fprintf(out, "%c", a + 'a');
                if (j / 2 != 0)
                {
                    fprintf(out, ",");
                    length++;
                }
            }
            j = j / 2;
        }
        printf("}");
        length++;
        for (uint k = 0; k < prefix_size - length; k++)
        {
            fprintf(out, " ");
        }
        printf(": ");
        print_sub_cayley_aligned(M, table[i], max_length, prefix_size + 2, out);
    }

    // Fin du tableau
    print_bot_line(max_length, out);
}
