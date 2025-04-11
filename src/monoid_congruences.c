/*****************************/
/* Deterministic hierarchies */
/*****************************/

#include "monoid_congruences.h"

/*************************/
/* Calcul de congruences */
/*************************/

// Computes the least partition containing the input one and compatible with
// the graph transitions: if q ≣ r then qa ≣ ra.
static void cong_fold(dgraph* G, ufind* uf) {
    // A queue that will contain the elements that are yet to treat.
    dequeue* tofold = create_dequeue();

    // Doubly linked lists memorizing edges.
    // Only the lists corresponding to a root in the union-find are filled.
    dlist* ltrans[G->size_graph][G->size_alpha];
    for (uint q = 0; q < G->size_graph; q++) {
        if (q == find_ufind(q, uf)) {
            rigins_dequeue(q, tofold);
            for (uint a = 0; a < G->size_alpha; a++) {
                ltrans[q][a] = create_dlist();
            }
        }
        else {
            for (uint a = 0; a < G->size_alpha; a++) {
                ltrans[q][a] = NULL;
            }
        }
    }

    // The lists are filled with the original transitions.
    for (uint q = 0; q < G->size_graph; q++) {
        uint r = find_ufind(q, uf);

        for (uint a = 0; a < G->size_alpha; a++) {
            insertprevious_dlist(ltrans[r][a], ltrans[r][a]->rsent, G->edges[q][a]);
        }
    }

    // As long as there exists an element to treat
    while (!isempty_dequeue(tofold)) {
        // On prend le représentant d'un sommet non-traité
        uint s = find_ufind(lefpull_dequeue(tofold), uf);

        bool folded = false;
        uint a = 0;
        while (!folded && a < G->size_alpha) {
            if (ltrans[s][a]->size > 1) // Si
                // il
                // y a
                // 2 arêtes
                // sortantes
                // étiquetées
                // par
                // a
            {
                // On prend les représentants des destinations des deux
                // premières arêtes qu'on va fusionner (si ça n'a pas été fait
                // avant)
                uint r = find_ufind(ltrans[s][a]->lsent->next->val, uf);
                uint t = find_ufind(ltrans[s][a]->lsent->next->next->val, uf);
                deletecell_dlist(ltrans[s][a], ltrans[s][a]->lsent->next); // On
                // supprime
                // la
                // première
                // des
                // deux
                // edges
                if (r != t) // Si
                    // les
                    // deux
                    // sommets
                    // adjacents
                    // n'avaient
                    // pas
                    // encore
                    // étés
                    // fusionnés
                {
                    union_ufind(r, t, uf); // On
                    // effectue
                    // la
                    // fusion
                    for (uint b = 0; b < G->size_alpha; b++) // On
                        // concatène
                        // leur
                        // listes
                        // de
                        // sommets
                        // adjacents
                    {
                        concat_dlist(ltrans[r][b], ltrans[t][b]);
                        if (find_ufind(r, uf) == r) // On
                            // ne
                            // garde
                            // que
                            // la
                            // liste
                            // du
                            // nouveau
                            // représentant
                            // de
                            // la
                            // classe
                        {
                            free(ltrans[t][b]);
                            ltrans[t][b] = NULL;
                        }
                        else {
                            free(ltrans[r][b]);
                            ltrans[r][b] = NULL;
                        }
                    }
                    rigins_dequeue(find_ufind(r, uf), tofold); // On va devoir
                    // éventuelement
                    // traiter la
                    // nouvelle
                    // classe
                    if (find_ufind(r, uf) != find_ufind(s, uf)) {
                        rigins_dequeue(find_ufind(s, uf), tofold);
                    }
                    folded = true;
                }
                else {
                    rigins_dequeue(find_ufind(s, uf), tofold);
                }
            }
            a++;
        }
    }

    // Suppression des listes
    for (uint q = 0; q < G->size_graph; q++) {
        for (uint a = 0; a < G->size_alpha; a++) {
            delete_dlist(ltrans[q][a]);
        }
    }
    delete_dequeue(tofold);
}

// Calcule de la plus petit congruence contenant une partition quelconque
// (donnée sous la forme d'un union-find)
void compute_leastcong(morphism* M, ufind* uf) {
    cong_fold(M->r_cayley, uf);
    cong_fold(M->l_cayley, uf);
}

/****************************************/
/* Partitions générées par une équation */
/****************************************/

ufind* iden_bpolmod_mono(morphism* M) {
    green* G = M->rels;
    // Special case when there exists a neutral letter. We simply return the partition into J-classes.
    if (mor_neutral_letter(M, NULL)) {
        return parti_to_ufind(G->JCL);
    }

    ufind* uf = create_ufind(M->r_cayley->size_graph);

    nfa* AR = morphism_to_dfa_rcl(M);
    nfa* AL = morphism_to_dfa_lcl(M);
    nfa* UR = nfa_proj_unary(AR);
    nfa* UL = nfa_proj_unary(AL);
    delete_nfa(AR);
    delete_nfa(AL);
    parti* SCCSR = nfa_inv_ext(UR);
    parti* SCCSL = nfa_inv_ext(UL);
    parti* FOLDR = nfa_stal_fold(UR, SCCSR);
    parti* FOLDL = nfa_stal_fold(UL, SCCSL);
    delete_parti(SCCSR);
    delete_parti(SCCSL);

    nfa* BR = nfa_merge_states(UR, FOLDR);
    nfa* BL = nfa_merge_states(UL, FOLDL);

    delete_nfa(UR);
    delete_nfa(UL);
    lgraph* temp = BL->trans;
    BL->trans = BL->trans_i;
    BL->trans_i = temp;

    uint size = 0;

    // Boucle sur tous les idempotents e = qr
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        uint e = lefread_dequeue(M->idem_list, i);

        // Boucle sur tous les idempotents f = st
        for (uint j = 0; j < size_dequeue(M->idem_list); j++) {
            uint f = lefread_dequeue(M->idem_list, j);
            uint ef = mor_mult(M, e, f);
            nfa_prod_pair* pairs = nfa_intersec_reach(BR, BL, FOLDR->numcl[e], FOLDL->numcl[f], &size);
            for (uint p = 0; p < size; p++) {
                uint c = pairs[p].q1;
                uint d = pairs[p].q2;
                // printf("%d %d\n", c, d);

                for (uint g = 0; g < size_dequeue(FOLDR->cl[c]); g++) {
                    uint q = lefread_dequeue(FOLDR->cl[c], g);
                    // mor_fprint_name_utf8(M, q, out);
                    // printf("  num: %d \n", q);
                    //  Il suffit de tester les q réguliers

                    for (uint k = 0; k < size_dequeue(FOLDL->cl[d]); k++) {
                        uint t = lefread_dequeue(FOLDL->cl[d], k);
                        // mor_fprint_name_utf8(M, s, out);
                        // printf("  num: %d \n", s);
                        union_ufind(ef, mor_mult(M, q, t), uf);
                    }
                }
            }
            free(pairs);
        }
    }

    delete_nfa(BR);
    delete_nfa(BL);
    delete_parti(FOLDR);
    delete_parti(FOLDL);
    return uf;
}

ufind* iden_blockg_mono(morphism* M) {
    green* G = M->rels;
    ufind* uf = create_ufind(M->r_cayley->size_graph);
    for (uint i = 0; i < size_dequeue(M->idem_list) - 1; i++) {
        uint e = lefread_dequeue(M->idem_list, i);
        for (uint j = 0; j < size_dequeue(G->RCL->cl[G->RCL->numcl[e]]); j++) {
            uint s = lefread_dequeue(G->RCL->cl[G->RCL->numcl[e]], j);
            if (M->idem_array[s]) {
                union_ufind(e, s, uf);
            }
        }
        for (uint j = 0; j < size_dequeue(G->LCL->cl[G->LCL->numcl[e]]); j++) {
            uint s = lefread_dequeue(G->LCL->cl[G->LCL->numcl[e]], j);
            if (M->idem_array[s]) {
                union_ufind(e, s, uf);
            }
        }
    }
    return uf;
}

ufind* iden_knast_mono(orbits* L) {
    morphism* M = L->original;
    green* G = L->original->rels;

    if (mor_nonempty_neutral(M)) {
        return parti_to_ufind(G->JCL);
    }

    // Sinon, on créé le union-find en utilisant l'équation de Knast
    ufind* uf = create_ufind(M->r_cayley->size_graph);

    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {


        // Union des J-classes dans l'orbite de e.
        for (uint k = 0; k < L->orbits[i]->rels->JCL->size_par; k++) {
            for (uint j = 0; j < size_dequeue(L->orbits[i]->rels->JCL->cl[k]); j++) {
                union_ufind(L->orbits[i]->sub_to_mono[lefread_dequeue(L->orbits[i]->rels->JCL->cl[k], 0)], L->orbits[i]->sub_to_mono[lefread_dequeue(L->orbits[i]->rels->JCL->cl[k], j)],
                    uf);
            }
        }

        // Boucles sur les idempotents f (le cas e = f a été traité avec les orbites et le cas f < e est traité quand e et f sont inversés).
        for (uint j = i + 1; j < M->nb_min_regular_jcl; j++) {

            uint f = M->regular_idems[j];

            // Intersection de MfM et eMe
            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            dequeue* Mf = compute_l_ideal(M, f, NULL);
            dequeue* fM = compute_r_ideal(M, f, NULL);

            // Les idempotents dans l'intersection de MfM et eMe
            dequeue* idems = make_inter_sorted_dequeue(MfM, M->idem_list);
            delete_dequeue(MfM);

            // Boucle sur les idempotents g = eqfre
            for (uint x = 0; x < size_dequeue(idems); x++) {
                uint g = lefread_dequeue(idems, x);

                // Les eqf tels que eqf R g
                dequeue* lset = make_inter_sorted_dequeue(Mf, G->RCL->cl[G->RCL->numcl[g]]);
                for (uint u = 0; u < size_dequeue(lset); u++) {
                    uint q = lefread_dequeue(lset, u);

                    // Boucle sur les idempotents h = esfte
                    for (uint y = 0; y < size_dequeue(idems); y++) {
                        uint h = lefread_dequeue(idems, y);
                        uint gh = mor_mult(M, g, h);

                        // Les fte tels que fte L h
                        dequeue* rset = make_inter_sorted_dequeue(fM, G->LCL->cl[G->LCL->numcl[h]]);
                        for (uint v = 0; v < size_dequeue(rset); v++) {
                            uint t = lefread_dequeue(rset, v);
                            union_ufind(gh, mor_mult(M, q, t), uf);
                        }
                        delete_dequeue(rset);
                    }
                }
                delete_dequeue(lset);
            }
            delete_dequeue(idems);
            delete_dequeue(fM);
            delete_dequeue(Mf);
        }
    }

    return uf;
}

ufind* iden_qknast_mono(orbits* L, subsemi* mker) {
    morphism* M = L->original;
    green* G = L->original->rels;
    // Special case when there exists a neutral letter. We simply return the partition into J-classes.
    if (mor_neutral_letter(M, NULL)) {
        return parti_to_ufind(G->JCL);
    }

    ufind* uf = create_ufind(M->r_cayley->size_graph);

    // Boucles sur tous les idempotents e
    for (uint i = 0; i < M->nb_min_regular_jcl; i++) {


        // Union de toute l'orbite de e.
        for (uint k = 0; k < L->orbits[i]->rels->JCL->size_par; k++) {
            for (uint j = 0; j < size_dequeue(L->orbits[i]->rels->JCL->cl[k]); j++) {
                union_ufind(L->orbits[i]->sub_to_mono[lefread_dequeue(L->orbits[i]->rels->JCL->cl[k], 0)], L->orbits[i]->sub_to_mono[lefread_dequeue(L->orbits[i]->rels->JCL->cl[k], j)],
                    uf);
            }
        }
        // Boucles sur les idempotents f (le cas e = f a été traité avec les orbites et le cas f < e est traité quand e et f sont inversés).
        for (uint j = i + 1; j < M->nb_min_regular_jcl; j++) {
            uint f = M->regular_idems[j];

            // Intersection de MfM et eMe
            dequeue* MfM = compute_j_ideal(M, f, L->orbits[i]->mono_in_sub);

            // Mf et fM restreints au MOD-kernel.
            dequeue* Mf = compute_l_ideal(M, f, mker->mono_in_sub);
            dequeue* fM = compute_r_ideal(M, f, mker->mono_in_sub);

            // Les idempotents dans l'intersection de MfM et eMe
            dequeue* idems = make_inter_sorted_dequeue(MfM, M->idem_list);
            delete_dequeue(MfM);

            // Boucle sur les idempotents g = eqfre
            for (uint x = 0; x < size_dequeue(idems); x++) {
                uint g = lefread_dequeue(idems, x);

                // Les eqf tels que eqf R g
                dequeue* lset = make_inter_sorted_dequeue(Mf, G->RCL->cl[G->RCL->numcl[g]]);
                for (uint u = 0; u < size_dequeue(lset); u++) {
                    uint q = lefread_dequeue(lset, u);

                    // Boucle sur les idempotents h = esfte
                    for (uint y = 0; y < size_dequeue(idems); y++) {
                        uint h = lefread_dequeue(idems, y);
                        uint gh = mor_mult(M, g, h);

                        // Les fte tels que fte L h
                        dequeue* rset = make_inter_sorted_dequeue(fM, G->LCL->cl[G->LCL->numcl[h]]);
                        for (uint v = 0; v < size_dequeue(rset); v++) {
                            uint t = lefread_dequeue(rset, v);
                            union_ufind(gh, mor_mult(M, q, t), uf);
                        }
                        delete_dequeue(rset);
                    }
                }
                delete_dequeue(lset);
            }
            delete_dequeue(idems);
            delete_dequeue(fM);
            delete_dequeue(Mf);
        }
    }
    return uf;
}

// Retourne l'équivalence obtenue en identifiant qui devraient être égaux selon
// l'équation de MPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// MPol(C) sur le monoide
// La congruence C canonique est passée en entrée
ufind* iden_mpolc_mono(morphism* M, parti* C) {
    // Création du Union-Find
    ufind* theuf = create_ufind(M->r_cayley->size_graph);

    // Pour toute classe de l'équivalence C
    for (uint c = 0; c < C->size_par; c++) {
        if (size_dequeue(C->cl[c]) < 2) {
            continue;
        }
        // On prend un premier élément s dans la classe
        for (uint i = 0; i < size_dequeue(C->cl[c]); i++) {

            uint s = lefread_dequeue(C->cl[c], i);

            // Les idempotents dans sM et Ms
            dequeue* sM = compute_r_ideal(M, s, M->idem_array);
            dequeue* Ms = compute_l_ideal(M, s, M->idem_array);

            // On prend un sécond élément distinct t dans la classe
            for (uint j = 0; j < size_dequeue(C->cl[c]); j++) {
                if (i != j) {
                    uint t = lefread_dequeue(C->cl[c], j);
                    for (uint k = 0; k < size_dequeue(sM); k++) {
                        uint sq = lefread_dequeue(sM, k);
                        uint sqs = mor_mult(M, sq, s);
                        uint sqt = mor_mult(M, sq, t);

                        for (uint l = 0; l < size_dequeue(Ms); l++) {
                            uint rs = lefread_dequeue(Ms, l);

                            uint sqsrs = mor_mult(M, sqs, rs);
                            uint sqtrs = mor_mult(M, sqt, rs);
                            union_ufind(sqsrs, sqtrs, theuf);
                        }
                    }
                }
            }
            delete_dequeue(sM);
            delete_dequeue(Ms);
        }
    }
    return theuf;
}

// Retourne l'équivalence obtenue en identifiant qui devraient être égaux selon
// l'équation de LPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// LPol(C) sur le monoide
// La congruence C canonique est passée en entrée
ufind* iden_lpolc_mono(morphism* M, parti* C) {
    // Création du Union-Find
    ufind* theuf = create_ufind(M->r_cayley->size_graph);

    // Pour tout idempotent e
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        uint e = lefread_dequeue(M->idem_list, i);
        dequeue* rideal = compute_r_ideal(M, e, NULL);
        dequeue* req = make_inter_sorted_dequeue(C->cl[C->numcl[e]], rideal);
        delete_dequeue(rideal);

        // On fusionne e avec tous les et tels que et C-eq e
        for (uint j = 0; j < size_dequeue(req); j++) {
            uint et = lefread_dequeue(req, j);
            union_ufind(e, et, theuf);
        }
        delete_dequeue(req);
    }

    return theuf;
}

// Retourne l'équivalence obtenue en identifiant qui devraient être égaux selon
// l'équation de RPol(C)
// A combiner avec compute_leastcong pour avoir la congruence canonique de
// RPol(C) sur le monoide
// La congruence C canonique est passée en entrée
ufind* iden_rpolc_mono(morphism* M, parti* C) { // Création du Union-Find
    ufind* theuf = create_ufind(M->r_cayley->size_graph);

    // Pour tout idempotent e
    for (uint i = 0; i < size_dequeue(M->idem_list); i++) {
        uint e = lefread_dequeue(M->idem_list, i);
        dequeue* lideal = compute_l_ideal(M, e, NULL);
        dequeue* leq = make_inter_sorted_dequeue(C->cl[C->numcl[e]], lideal);
        delete_dequeue(lideal);

        // On fusionne e avec tous les te tels que te C-eq e
        for (uint j = 0; j < size_dequeue(leq); j++) {
            uint te = lefread_dequeue(leq, j);
            union_ufind(e, te, theuf);
        }
        delete_dequeue(leq);
    }
    return theuf;
}

/**********************************************************************************/
/* Calcul du niveau dans une hiérarchie de négations en logique temporelle
   unaire */
   /**********************************************************************************/

   // Calcul du niveau dans la hiérarchie MPol. On passe un "prototype" de la
   // congruence canonique C en entrée.
   // (la plus petit congruence contenant la paertition doit être la congruence
   // canonique C)
   // On suppose que l'input est dans UPol(C) (ne termine pas dans le cas
   // contraire)
short hdet_mpol_level(morphism* M, ufind* C, FILE* out) {
    if (C->size_par == C->size_set) {
        if (out) {
            fprintf(out, "#### The language has level 1.\n");
        }
        return 1;
    }
    if (out) {
        fprintf(out, "#### The language does not have level 1.\n");
    }
    ufind* theuf;
    compute_leastcong(M, C);
    parti* thepr = ufind_to_parti(C);
    short level = 2;
    while (true) {
        theuf = iden_mpolc_mono(M, thepr);
        delete_parti(thepr);
        if (theuf->size_par == theuf->size_set) {
            delete_ufind(theuf);
            if (out) {
                fprintf(out, "#### The language has level %d.\n", level);
            }
            return level;
        }
        if (out) {
            fprintf(out, "#### The language does not have level %d.\n", level);
        }
        level++;
        compute_leastcong(M, theuf);
        thepr = ufind_to_parti(theuf);
        delete_ufind(theuf);
    }
}

// Calcul du niveau dans les hiéarchies LPol/RPol de base C: 1 = RPol(C),LPol(C)
// 2 = RPol₂(C),LPol₂(C)...
void hdet_lrpol_level(morphism* M, ufind* C, FILE* out, short* minf, short* minp) {
    if (C->size_par == C->size_set) {
        if (out) {
            fprintf(out, "#### The language has level 1 in both the future hierarchy and the past hierarchy.\n");
        }
        *minf = 1;
        *minp = 1;
    }
    ufind* ufleft;
    ufind* ufright;
    compute_leastcong(M, C);
    parti* pleft = ufind_to_parti(C);
    parti* pright = ufind_to_parti(C);
    short level = 1;
    while (true) {
        ufleft = iden_lpolc_mono(M, pright);
        ufright = iden_rpolc_mono(M, pleft);
        delete_parti(pright);
        delete_parti(pleft);
        if (ufleft->size_par == ufleft->size_set && ufright->size_par == ufright->size_set) {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out) {
                fprintf(out, "#### The language has level %d in both the future hierarchy and the past hierarchy.\n", level);
            }
            *minf = level;
            *minp = level;
            return;
        }
        else if (ufleft->size_par == ufleft->size_set) {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out && level % 2) {
                fprintf(out, "#### The language has level %d in the past hierarchy but not in the future hierarchy.\n", level);
                fprintf(out, "#### The language has level %d in the future hierarchy.\n", level + 1);
                *minf = level + 1;
                *minp = level;
            }
            else if (out) {
                fprintf(out, "#### The language has level %d in the future hierarchy but not in the past hierarchy.\n", level);
                fprintf(out, "#### The language has level %d in the past hierarchy.\n", level + 1);
                *minf = level;
                *minp = level + 1;
            }
            return;
        }
        else if (ufright->size_par == ufright->size_set) {
            delete_ufind(ufleft);
            delete_ufind(ufright);
            if (out && level % 2) {
                fprintf(out, "#### The language has level %d in the future hierarchy but not in the past hierarchy.\n", level);
                fprintf(out, "#### The language has level %d in the past hierarchy.\n", level + 1);
                *minf = level;
                *minp = level + 1;
            }
            else if (out) {
                fprintf(out, "#### The language has level %d in the past hierarchy but not in the future hierarchy.\n", level);
                fprintf(out, "#### The language has level %d in the future hierarchy.\n", level + 1);
                *minf = level + 1;
                *minp = level;
            }

            return;
        }

        if (out) {
            fprintf(out, "#### The language does not have level %d in either hierarchy.\n", level);
        }
        level++;
        compute_leastcong(M, ufleft);
        compute_leastcong(M, ufright);
        pleft = ufind_to_parti(ufleft);
        pright = ufind_to_parti(ufright);
        delete_ufind(ufleft);
        delete_ufind(ufright);
    }
}
