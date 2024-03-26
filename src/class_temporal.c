/*********************************************/
/* Fonctions sur les classes associées à UTL */
/*********************************************/

#include "class_temporal.h"

/******/
/* TL */
/******/

// Membership monoids
bool is_da_mono(p_cayley M, p_green G, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking if the monoid M is in DA.\n");
        fprintf(out, "#### All regular elements s ∊ M (i.e. s is J-equivalent to an idemptotent) must be idempotents.\n");
    }
    uint nb_idem = size_vertices(M->idem_list);
    uint nb_regular = size_vertices(G->regular_list);

    // Le monoide est dans DA si et seulement si il y a autant d'éléments réguliers que d'idempotent
    if (nb_idem != nb_regular)
    {
        if (out)
        {
            // On regarde toutes les éléments du monoide
            for (uint s = 0; s < M->trans->size_graph; s++)
            {
                // On prend la première qui est régulier et non-idempotent
                if (!M->idem_array[s] && G->regular_array[s])
                {
                    fprintf(out, "#### This monoid is NOT in DA.\n");
                    fprintf(out, "#### For instance, ");
                    cayley_print_name(M, s, out);
                    fprintf(out, " is a non-idempotent regular element.\n");
                    return false;
                }
            }
        }
        return false;
    }

    if (out)
    {
        fprintf(out, "#### This monoid is in DA.\n");
    }
    return true;
}

bool is_da_submono(p_green_sub S, char* name, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s is in DA. Displaying the %s.\n", name, name);
        print_full_green_sub(S, false, stdout);
    }

    p_cayley M = S->original;
    p_green G = S->originalrels;

    // Si le sous-monoide est le monoide entier
    if (S->mono_in_sub == NULL) {
        return is_da_mono(M, G, out);
    }

    uint nb_idem = size_vertices(S->idem_list);
    uint nb_regular = size_vertices(S->rels->regular_list);

    // Le monoide est dans DA si et seulement si il y a autant d'éléments réguliers que d'idempotent
    if (nb_idem != nb_regular)
    {
        if (out)
        {
            // On regarde toutes les éléments du monoide
            for (uint s = 0; s < S->size; s++)
            {
                // On prend la première qui est non-triviale
                if (!S->idem_array[s] && S->rels->regular_array[s])
                {
                    fprintf(out, "#### This monoid is NOT in DA.\n");
                    fprintf(out, "#### For instance, ");
                    cayley_print_name(M, S->sub_to_mono[s], out);
                    fprintf(out, " is a non-idempotent regular element.\n");
                    return false;
                }
            }

            for (uint s = 0; s < M->trans->size_graph; s++)
            {
                // On prend la première qui est non-triviale
                uint c = S->rels->JCL->numcl[s];
                if (!M->idem_array[S->sub_to_mono[s]] && mem_vertices_sorted(c, S->rels->regular_list))
                {
                    fprintf(out, "#### This monoid is NOT in DA.\n");
                    fprintf(out, "#### For instance, ");
                    cayley_print_name(M, S->sub_to_mono[s], out);
                    fprintf(out, " is a non-idempotent element in a regular J-class.\n");
                    return false;
                }
            }
        }
        return false;
    }

    if (out)
    {
        fprintf(out, "#### This monoid is in DA.\n");
    }
    return true;
}

bool is_da_orbmono(p_orbits L, char* class, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s-orbits are in DA.\n", class);
        fprintf(out, "#### The list of all idempotents:\n");
        cayley_print_idems(L->sub->original, out);
    }
    char nameorb[100];


    if (L->oneonly) {
        if (out) {
            fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Checking only the latter.\n", class, class);
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of 1", class);
        return is_da_submono(L->sub, nameorb, out);
    }
    p_cayley M = L->sub->original;
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        if (out)
        {
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of %s", class, cayley_get_name(M, e));
        // On teste si l'orbite est dans DA
        if (!is_da_submono(L->orbits[i], nameorb, out))
        {
            return false;
        }
    }
    return true;
}

bool is_da_orbmono_opti(p_orbits L, char* class, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s-orbits are in DA.\n", class);
        fprintf(out, "#### The list of all idempotents:\n");
        cayley_print_idems(L->sub->original, out);
    }
    char nameorb[100];

    if (L->oneonly) {
        if (out) {
            fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Checking only the latter.\n", class, class);
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of 1", class);
        return is_da_submono(L->sub, nameorb, out);
    }
    p_cayley M = L->sub->original;

    if (out) {
        fprintf(out, "#### Optimized version: omitting a %s-orbit when it is a subsemigroup of a previously\n", class);
        fprintf(out, "     checked %s-orbit.\n", class);
    }

    // Tableau qui sauvegardera les idempotents déjà traité avec un autre (cas où f appartient à eMe pour un e déjà traité)
    bool edone[M->trans->size_graph];
    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        edone[lefread_vertices(M->idem_list, i)] = false;
    }

    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);

        if (!edone[e])
        {
            if (out)
            {
                print_sep_line(100, out);
            }
            // On sauvegarde tous les idempotents dans eMe
            for (uint j = 0; j < L->orbits[i]->size; j++)
            {
                edone[L->orbits[i]->sub_to_mono[j]] = true;
            }
            sprintf(nameorb, "%s-orbit of %s", class, cayley_get_name(M, e));
            // On teste si eMe est dans DA
            if (!is_da_submono(L->orbits[i], nameorb, out))
            {
                return false;
            }
        }

    }
    return true;
}

/******/
/* FL */
/******/

// L-triviality
bool is_ltrivial_mono(p_cayley M, p_green G, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking if the monoid M is L-trivial.\n");
    }
    // Si il y a autant d'éléments que de L-classes, le monoide est L-trivial
    if (G->LCL->size_set == G->LCL->size_par)
    {
        if (out)
        {
            fprintf(out, "#### This is an L-trivial monoid.\n");
        }
        return true;
    }

    // Sinon (si on doit afficher), on va chercher un contre-exemple
    if (out)
    {
        // On regarde toutes les L-classes
        for (uint i = 0; i < G->LCL->size_par; i++)
        {
            // On prend la première qui est non-triviale
            if (size_vertices(G->LCL->cl[i]) >= 2)
            {
                uint s = lefread_vertices(G->LCL->cl[i], 0);
                uint t = lefread_vertices(G->LCL->cl[i], 1);
                fprintf(out, "#### This is NOT an L-trivial monoid.\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, s, out);
                fprintf(out, " L ");
                cayley_print_name(M, t, out);
                fprintf(out, ".\n");
                return false;
            }
        }
    }
    return false;
}

bool is_ltrivial_submono(p_green_sub S, char* name, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s is L-trivial. Displaying the %s.\n", name, name);
        print_full_green_sub(S, false, stdout);
    }
    p_cayley M = S->original;
    p_green G = S->originalrels;

    // Si le sous-monoide est le monoide entier
    if (S->mono_in_sub == NULL) {
        return is_ltrivial_mono(M, G, out);
    }

    // Si il y a autant d'éléments que de L-classes, le monoide est L-trivial
    if (S->rels->LCL->size_set == S->rels->LCL->size_par)
    {
        if (out)
        {
            fprintf(out, "#### The monoid is L-trivial.\n");
        }
        return true;
    }

    // Sinon (si on doit afficher), on va chercher un contre-exemple
    if (out)
    {
        // On regarde toutes les L-classes
        for (uint i = 0; i < S->rels->LCL->size_par; i++)
        {
            // On prend la première qui est non-triviale
            if (size_vertices(S->rels->LCL->cl[i]) >= 2)
            {
                uint s = lefread_vertices(S->rels->LCL->cl[i], 0);
                uint t = lefread_vertices(S->rels->LCL->cl[i], 1);
                fprintf(out, "#### This monoid is NOT L-trivial.\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, S->sub_to_mono[s], out);
                fprintf(out, " L ");
                cayley_print_name(M, S->sub_to_mono[t], out);
                fprintf(out, ".\n");
                return false;
            }
        }
    }
    return false;
}

bool is_ltrivial_orbmono(p_orbits L, char* class, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s-orbits are L-trivial.\n", class);
        fprintf(out, "#### The list of all idempotents:\n");
        cayley_print_idems(L->sub->original, out);
    }
    char nameorb[100];

    if (L->oneonly) {
        if (out) {
            fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Checking only the latter.\n", class, class);
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of 1", class);
        return is_ltrivial_submono(L->sub, nameorb, out);
    }

    p_cayley M = L->sub->original;
    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        if (out)
        {
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of %s", class, cayley_get_name(M, e));

        // On teste si l'orbit' de e est L-triviale
        if (!is_ltrivial_submono(L->orbits[i], nameorb, out))
        {
            return false;
        }
    }
    return true;
}

bool is_ltrivial_orbmono_opti(p_orbits L, char* class, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s-orbits are L-trivial.\n", class);
        fprintf(out, "#### The list of all idempotents:\n");
        cayley_print_idems(L->sub->original, out);
    }
    char nameorb[100];

    if (L->oneonly) {
        if (out) {
            fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Checking only the latter.\n", class, class);
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of 1", class);
        return is_ltrivial_submono(L->sub, nameorb, out);
    }

    p_cayley M = L->sub->original;

    if (out) {
        fprintf(out, "#### Optimized version: omitting a %s-orbit when it is a subsemigroup of a previously\n", class);
        fprintf(out, "     checked %s-orbit.\n", class);
    }

    // Tableau qui sauvegardera les idempotents déjà traité avec un autre (cas où f appartient à eMe pour un e déjà traité)
    bool edone[M->trans->size_graph];
    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        edone[lefread_vertices(M->idem_list, i)] = false;
    }

    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);

        if (!edone[e])
        {
            if (out)
            {
                print_sep_line(100, out);
            }

            // On sauvegarde tous les idempotents se trouvant dans eMe
            for (uint j = 0; j < size_vertices(L->orbits[i]->idem_list); j++)
            {
                edone[L->orbits[i]->sub_to_mono[lefread_vertices(L->orbits[i]->idem_list, j)]] = true;
            }
            sprintf(nameorb, "%s-orbit of %s", class, cayley_get_name(M, e));

            // On teste si eMe est L-trivial
            if (!is_ltrivial_submono(L->orbits[i], nameorb, out))
            {
                return false;
            }
        }
    }
    return true;
}

// R-triviality
bool is_rtrivial_mono(p_cayley M, p_green G, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking if the monoid M is R-trivial.\n");
    }
    // Si il y a autant d'éléments que de R-classes, le monoide est R-trivial
    if (G->RCL->size_set == G->RCL->size_par)
    {
        if (out)
        {
            fprintf(out, "#### The monoid is R-trivial.\n");
        }
        return true;
    }

    // Sinon (si on doit afficher), on va chercher un contre-exemple
    if (out)
    {
        // On regarde toutes les R-classes
        for (uint i = 0; i < G->RCL->size_par; i++)
        {
            // On prend la première qui est non-triviale
            if (size_vertices(G->RCL->cl[i]) >= 2)
            {
                uint s = lefread_vertices(G->RCL->cl[i], 0);
                uint t = lefread_vertices(G->RCL->cl[i], 1);
                fprintf(out, "#### The monoid is NOT R-trivial.\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, s, out);
                fprintf(out, " L ");
                cayley_print_name(M, t, out);
                fprintf(out, ".\n");
                return false;
            }
        }
    }
    return false;
}

bool is_rtrivial_submono(p_green_sub S, char* name, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s is R-trivial. Displaying the %s.\n", name, name);
        print_full_green_sub(S, false, stdout);
    }
    p_cayley M = S->original;
    p_green G = S->originalrels;

    // Si le sous-monoide est le monoide entier
    if (S->mono_in_sub == NULL) {
        return is_rtrivial_mono(M, G, out);
    }

    // Si il y a autant d'éléments que de R-classes, le monoide est R-trivial
    if (S->rels->RCL->size_set == S->rels->RCL->size_par)
    {
        if (out)
        {
            fprintf(out, "#### The monoid is R-trivial.\n");
        }
        return true;
    }

    // Sinon (si on doit afficher), on va chercher un contre-exemple
    if (out)
    {
        // On regarde toutes les R-classes
        for (uint i = 0; i < S->rels->RCL->size_par; i++)
        {
            // On prend la première qui est non-triviale
            if (size_vertices(S->rels->RCL->cl[i]) >= 2)
            {
                uint s = lefread_vertices(S->rels->RCL->cl[i], 0);
                uint t = lefread_vertices(S->rels->RCL->cl[i], 1);
                fprintf(out, "#### This monoid is NOT R-trivial.\n");
                fprintf(out, "#### For instance, ");
                cayley_print_name(M, S->sub_to_mono[s], out);
                fprintf(out, " L ");
                cayley_print_name(M, S->sub_to_mono[t], out);
                fprintf(out, ".\n");
                return false;
            }
        }
    }
    return false;
}

bool is_rtrivial_orbmono(p_orbits L, char* class, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s-orbits are R-trivial.\n", class);
        fprintf(out, "#### The list of all idempotents:\n");
        cayley_print_idems(L->sub->original, out);
    }
    char nameorb[100];

    if (L->oneonly) {
        if (out) {
            fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Checking only the latter.\n", class, class);
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of 1", class);
        return is_rtrivial_submono(L->sub, nameorb, out);
    }

    p_cayley M = L->sub->original;


    for (uint i = 0; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);
        if (out)
        {
            print_sep_line(100, out);
        }


        sprintf(nameorb, "%s-orbit of %s", class, cayley_get_name(M, e));

        // On teste si l'orbit' de e est R-triviale
        if (!is_rtrivial_submono(L->orbits[i], nameorb, out))
        {
            return false;
        }
    }
    return true;
}

bool is_rtrivial_orbmono_opti(p_orbits L, char* class, FILE* out)
{
    if (out) {
        fprintf(out, "#### Checking whether the %s-orbits are R-trivial.\n", class);
        fprintf(out, "#### The list of all idempotents:\n");
        cayley_print_idems(L->sub->original, out);
    }
    char nameorb[100];

    if (L->oneonly) {
        if (out) {
            fprintf(out, "#### All %s-orbits are sub-semigroups of the %s-orbit of 1. Checking only the latter.\n", class, class);
            print_sep_line(100, out);
        }
        sprintf(nameorb, "%s-orbit of 1", class);
        return is_rtrivial_submono(L->sub, nameorb, out);
    }

    p_cayley M = L->sub->original;

    if (out) {
        fprintf(out, "#### Optimized version: omitting a %s-orbit when it is a subsemigroup of a previously\n", class);
        fprintf(out, "     checked %s-orbit.\n", class);
    }

    // Tableau qui sauvegardera les idempotents déjà traité avec un autre (cas où f appartient à eMe pour un e déjà traité)
    bool edone[M->trans->size_graph];
    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        edone[lefread_vertices(M->idem_list, i)] = false;
    }

    for (uint i = 1; i < size_vertices(M->idem_list); i++)
    {
        uint e = lefread_vertices(M->idem_list, i);

        if (!edone[e])
        {
            if (out)
            {
                print_sep_line(100, out);
            }

            // On sauvegarde tous les idempotents se trouvant dans eMe
            for (uint j = 0; j < size_vertices(L->orbits[i]->idem_list); j++)
            {
                edone[L->orbits[i]->sub_to_mono[lefread_vertices(L->orbits[i]->idem_list, j)]] = true;
            }
            sprintf(nameorb, "%s-orbit of %s", class, cayley_get_name(M, e));

            // On teste si eMe est R-trivial
            if (!is_rtrivial_submono(L->orbits[i], nameorb, out))
            {
                return false;
            }
        }
    }
    return true;
}
