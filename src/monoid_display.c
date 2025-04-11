#include "monoid_display.h"



/**********************************/
/* Generic functions for printing */
/**********************************/

uint mor_fprint_name_utf8(morphism* M, uint q, FILE* out) {
    if (isempty_dequeue(M->names[q])) {
        fprintf(out, "1");
        return 1;
    }

    uint n = 1;
    uint len = fprint_letter_utf8(M->alphabet[lefread_dequeue(M->names[q], 0)], out);
    for (uint i = 1; i < size_dequeue(M->names[q]); i++) {
        if (lefread_dequeue(M->names[q], i) != lefread_dequeue(M->names[q], i - 1)) {
            if (n > 1) {
                len += fprint_power_utf8(n, out);
            }
            len += fprint_letter_utf8(M->alphabet[lefread_dequeue(M->names[q], i)], out);
            n = 1;
        }
        else {
            n++;
        }
    }
    if (n > 1) {
        len += fprint_power_utf8(n, out);
    }
    return len;
}


int mor_sprint_name_utf8(morphism* M, uint q, char* out) {
    if (isempty_dequeue(M->names[q])) {
        return sprintf(out, "1");
    }

    uint n = 1;
    int len = sprint_letter_utf8(M->alphabet[lefread_dequeue(M->names[q], 0)], out);
    for (uint i = 1; i < size_dequeue(M->names[q]); i++) {
        if (lefread_dequeue(M->names[q], i) != lefread_dequeue(M->names[q], i - 1)) {
            if (n > 1) {
                len += sprint_power_utf8(n, out + len);
            }
            len += sprint_letter_utf8(M->alphabet[lefread_dequeue(M->names[q], i)], out + len);
            n = 1;
        }
        else {
            n++;
        }
    }
    if (n > 1) {
        len += sprint_power_utf8(n, out + len);
    }
    return len;
}



static void display_power_gviz_rec(short n, FILE* out) {
    if (n > 0) {
        display_power_gviz_rec(n / 10, out);
        fprintf(out, "%d", n % 10);
    }
}

void mor_print_name_gviz(morphism* M, uint q, FILE* out) {
    if (isempty_dequeue(M->names[q])) {
        fprintf(out, "1");
    }
    else {
        uint n = 1;
        fprint_letter_gviz(M->alphabet[lefread_dequeue(M->names[q], 0)], out, false);
        for (uint i = 1; i < size_dequeue(M->names[q]); i++) {
            if (lefread_dequeue(M->names[q], i) != lefread_dequeue(M->names[q], i - 1)) {
                if (n > 1) {
                    fprintf(out, "<SUP>");
                    display_power_gviz_rec(n, out);
                    fprintf(out, "</SUP>");
                }
                fprint_letter_gviz(M->alphabet[lefread_dequeue(M->names[q], i)], out, false);
                n = 1;
            }
            else {
                n++;
            }
        }
        if (n > 1) {
            fprintf(out, "<SUP>");
            display_power_gviz_rec(n, out);
            fprintf(out, "</SUP>");
        }
    }
}

void mor_fprint_name_utf8_aligned(morphism* M, uint q, uint size_max, FILE* out) {
    uint len = mor_fprint_name_utf8(M, q, out);
    print_spaces(size_max - len, out);
}

static uint mor_get_name_length(morphism* M, uint q) {
    if (isempty_dequeue(M->names[q])) {
        // the length of the identity element is 1 (string "1")
        return 1;
    }
    // Otherwise, compute the length in a variable.
    uint l = 0;

    // Temporary variable to store the number of consecutive copies of the same letter.
    uint i = 0;

    while (i < size_dequeue(M->names[q])) {
        l += length_letter_utf8(M->alphabet[lefread_dequeue(M->names[q], i)]);
        uint j = i + 1;
        while (j < size_dequeue(M->names[q]) && lefread_dequeue(M->names[q], j) == lefread_dequeue(M->names[q], i)) {
            j++;
        }

        if (j - i >= 2) {
            l += get_uint_length(j - i);
        }
        i = j;
    }
    return l;
}

uint mor_max_name_size(morphism* M, dequeue* sub) {
    uint size = 1;

    if (sub == NULL) {
        for (uint q = 0; q < M->r_cayley->size_graph; q++) {
            size = max(size, mor_get_name_length(M, q));
        }
        return size;
    }
    else {
        for (uint i = 0; i < size_dequeue(sub); i++) {
            size = max(size, mor_get_name_length(M, lefread_dequeue(sub, i)));
        }
        return size;
    }
}

void mor_print_sub(morphism* M, dequeue* elems, FILE* out) {
    for (uint i = 0; i < size_dequeue(elems); i++) {
        mor_fprint_name_utf8(M, lefread_dequeue(elems, i), out);
        if (i < size_dequeue(elems) - 1) {
            fprintf(out, ", ");
        }
    }
    fprintf(out, "\n");
}

void mor_print_sub_aligned(morphism* M, dequeue* elems, uint width, uint padding, FILE* out) {
    uint max_size = width - padding;
    uint use_size = 0;
    for (uint i = 0; i < size_dequeue(elems); i++) {
        uint e = lefread_dequeue(elems, i);
        uint size_elem = mor_get_name_length(M, e);

        if (size_elem + use_size + 2 > max_size || (i == size_dequeue(elems) - 1 && size_elem + use_size > max_size)) {
            for (uint j = 0; j < max_size - use_size; j++) {
                fprintf(out, " ");
            }
            fprintf(out, "│\n│");
            for (uint j = 0; j < padding; j++) {
                fprintf(out, " ");
            }
            use_size = 0;
        }
        mor_fprint_name_utf8(M, e, out);
        use_size = use_size + size_elem;
        if (i < size_dequeue(elems) - 1) {
            fprintf(out, ", ");
            use_size = use_size + 2;
        }
    }
    print_spaces(max_size - use_size, out);
    fprintf(out, "│\n");
}

void mor_print_sub_titled(morphism* M, dequeue* elems, uint length, char* name, FILE* out) {
    print_title_box(length, false, out, 1, name);
    fprintf(out, "│");

    // Print the subset.
    mor_print_sub_aligned(M, elems, length, 0, out);

    print_bot_line(length, out);
}

void mor_print_mapping(morphism* M, FILE* out) {
    for (uint a = 0; a < M->r_cayley->size_alpha; a++) {

        fprintf(out, "    ");
        fprint_letter_utf8(M->alphabet[a], out);
        fprintf(out, " ⟼ ");
        mor_fprint_name_utf8(M, M->r_cayley->edges[ONE][a], out);
        fprintf(out, "\n");
    }
}


// Affichage des idempotents
void mor_print_idems(morphism* M, FILE* out) {
    print_top_line(100, out);
    fprintf(out, "│Idempotents : ");
    mor_print_sub_aligned(M, M->idem_list, 100, 14, out);
    print_bot_line(100, out);
}

// Affichage de l'ordre syntaxique
void mor_print_order(morphism* M, FILE* out) {
    dequeue** order = mor_compute_order(M); // Calcul 
    print_top_line(100, out);

    // Calcul de la longueur maximale d'un nom
    uint size_max = 1;
    for (uint q = 0; q < M->r_cayley->size_graph; q++) {
        size_max = max(size_max, mor_get_name_length(M, q));
    }

    uint padding = 24 + size_max;

    for (uint q = 0; q < M->r_cayley->size_graph; q++) {
        fprintf(out, "│Elements larger than ");
        mor_fprint_name_utf8_aligned(M, q, size_max, out);
        fprintf(out, " : ");
        mor_print_sub_aligned(M, order[q], 100, padding, out);
        delete_dequeue(order[q]);
    }
    print_bot_line(100, out);

    free(order);

}

// Print de la table de multiplication
void mor_mult_print(morphism* M, FILE* out) {
    // Calcule de la table de multiplication
    mor_compute_mult(M);

    // Calcul de la longueur max du nom d'un élément
    uint length = mor_max_name_size(M, NULL);

    // Barre Horizontale haute
    fprintf(out, "┌");
    for (uint i = 0; i < length; i++) {
        fprintf(out, "─");
    }
    fprintf(out, "┬");
    for (uint i = 0; i < M->r_cayley->size_graph; i++) {
        for (uint j = 0; j < length; j++) {
            fprintf(out, "─");
        }
        if (i < M->r_cayley->size_graph - 1) {
            fprintf(out, "┬");
        }
        else {
            fprintf(out, "┐");
        }
    }
    fprintf(out, "\n");

    // Première ligne
    fprintf(out, "│");
    for (uint i = 0; i < length; i++) {
        fprintf(out, " ");
    }
    fprintf(out, "│");
    for (uint i = 0; i < M->r_cayley->size_graph; i++) {
        mor_fprint_name_utf8_aligned(M, i, length, out);
        fprintf(out, "│");
    }
    fprintf(out, "\n");

    // Barre Horizontale mid
    fprintf(out, "├");
    for (uint i = 0; i < length; i++) {
        fprintf(out, "─");
    }
    fprintf(out, "┼");
    for (uint i = 0; i < M->r_cayley->size_graph; i++) {
        for (uint j = 0; j < length; j++) {
            fprintf(out, "─");
        }
        if (i < M->r_cayley->size_graph - 1) {
            fprintf(out, "┼");
        }
        else {
            fprintf(out, "┤");
        }
    }
    fprintf(out, "\n");

    // La table

    for (uint k = 0; k < M->r_cayley->size_graph; k++) {
        fprintf(out, "│");
        mor_fprint_name_utf8_aligned(M, k, length, out);
        fprintf(out, "│");
        for (uint i = 0; i < M->r_cayley->size_graph; i++) {
            mor_fprint_name_utf8_aligned(M, M->mult[k][i], length, out);
            fprintf(out, "│");
        }
        fprintf(out, "\n");
    }

    // Barre Horizontale basse
    fprintf(out, "└");
    for (uint i = 0; i < length; i++) {
        fprintf(out, "─");
    }
    fprintf(out, "┴");
    for (uint i = 0; i < M->r_cayley->size_graph; i++) {
        for (uint j = 0; j < length; j++) {
            fprintf(out, "─");
        }
        if (i < M->r_cayley->size_graph - 1) {
            fprintf(out, "┴");
        }
        else {
            fprintf(out, "┘");
        }
    }
    fprintf(out, "\n");
}



// Output of all idempotents of a subsemigroup.
void submono_print_idems(subsemi* S, FILE* out) {
    morphism* M = S->original;
    if (S->mono_in_sub == NULL) {
        mor_print_idems(M, out);
        return;
    }
    print_top_line(100, out);
    fprintf(out, "│Idempotents : ");
    dequeue* idemsinsub = restrict_list_to_subsemi(S, M->idem_list);
    mor_print_sub_aligned(M, idemsinsub, 100, 14, out);
    print_bot_line(100, out);
    delete_dequeue(idemsinsub);
}

// Output of the syntactic order of a subsemigroup.
void submono_print_order(subsemi* S, FILE* out) {
    morphism* M = S->original;

    if (S->mono_in_sub == NULL) {
        mor_print_order(M, out);
        return;
    }
    mor_compute_order(M);
    print_top_line(100, out);

    // Computes the maximum size of a name.
    uint size_max = 1;
    for (uint i = 0; i < S->size; i++) {
        size_max = max(size_max, size_dequeue(M->names[S->sub_to_mono[i]]));
    }

    uint padding = 24 + size_max;

    for (uint i = 0; i < S->size; i++) {
        dequeue* elems = restrict_list_to_subsemi(S, M->order[S->sub_to_mono[i]]);
        fprintf(out, "│Elements larger than ");
        mor_fprint_name_utf8_aligned(M, S->sub_to_mono[i], size_max, out);
        fprintf(out, " : ");
        mor_print_sub_aligned(M, elems, 100, padding, out);
        delete_dequeue(elems);
    }
    print_bot_line(100, out);
}

/***********/
/* Display */
/***********/



void print_infos_green(morphism* M, FILE* out) {
    fprintf(out, "#### Size of the monoid                 : %d.\n", M->r_cayley->size_graph);
    fprintf(out, "#### Number of idempotents              : %d.\n", size_dequeue(M->idem_list));
    fprintf(out, "#### Number of regular elements         : %d.\n", M->rels->nb_regular_elems);
    fprintf(out, "#### Number of J-classes                : %d.\n", M->rels->JCL->size_par);
    fprintf(out, "#### Number of L-classes                : %d.\n", M->rels->LCL->size_par);
    fprintf(out, "#### Number of R-classes                : %d.\n", M->rels->RCL->size_par);
    fprintf(out, "#### Number of H─classes                : %d.\n", M->rels->HCL->size_par);
}




void print_infos_green_sub(subsemi* S, FILE* out) {
    if (S->level == LV_REG) {
        fprintf(out, "#### For optimization purposes, only the regular elements are computed.\n");
        fprintf(out, "#### Number of idempotents       : %d.\n", size_dequeue(S->idem_list));
        fprintf(out, "#### Number of regular elements  : %d.\n", S->rels->nb_regular_elems);
        fprintf(out, "#### Number of regular J-classes : %d.\n", S->rels->JCL->size_par);
        fprintf(out, "#### Number of regular L-classes : %d.\n", S->rels->LCL->size_par);
        fprintf(out, "#### Number of regular R-classes : %d.\n", S->rels->RCL->size_par);
        fprintf(out, "#### Number of regular H─classes : %d.\n", S->rels->HCL->size_par);
        return;
    }

    if (S->level == LV_GREG) {

        uint nonreg = S->size - S->rels->nb_regular_elems;

        fprintf(out, "#### For optimization purposes, the Green relations are only computed over the regular elements.\n");
        fprintf(out, "#### Size of the monoid          : %d.\n", S->rels->HCL->size_set);
        fprintf(out, "#### Number of idempotents       : %d.\n", size_dequeue(S->idem_list));
        fprintf(out, "#### Number of regular elements  : %d.\n", S->rels->nb_regular_elems);
        fprintf(out, "#### Number of regular J-classes : %d.\n", S->rels->JCL->size_par - nonreg);
        fprintf(out, "#### Number of regular L-classes : %d.\n", S->rels->LCL->size_par - nonreg);
        fprintf(out, "#### Number of regular R-classes : %d.\n", S->rels->RCL->size_par - nonreg);
        fprintf(out, "#### Number of regular H─classes : %d.\n", S->rels->HCL->size_par - nonreg);
        return;
    }

    if (S->level == LV_FULL) {
        fprintf(out, "#### Size of the monoid                 : %d.\n", S->size);
        fprintf(out, "#### Number of idempotents              : %d.\n", size_dequeue(S->idem_list));
        fprintf(out, "#### Number of regular elements         : %d.\n", S->rels->nb_regular_elems);
        fprintf(out, "#### Number of J-classes                : %d.\n", S->rels->JCL->size_par);
        fprintf(out, "#### Number of L-classes                : %d.\n", S->rels->LCL->size_par);
        fprintf(out, "#### Number of R-classes                : %d.\n", S->rels->RCL->size_par);
        fprintf(out, "#### Number of H─classes                : %d.\n", S->rels->HCL->size_par);
        return;
    }
}

static void print_jclass_green_aux(morphism* M, green* G, uint* remap, uint jclass, uint padding, FILE* out) {
    // An element in the J-class (used to compute various parameters).
    uint elem = lefread_dequeue(G->JCL->cl[jclass], 0);

    // Size of the H-classes in the J-class.
    uint size_hc = size_dequeue(G->HCL->cl[G->HCL->numcl[elem]]);
    // Size of the R-classes in the J-class.
    uint size_rc = size_dequeue(G->RCL->cl[G->RCL->numcl[elem]]);
    // Size of the L-classes in the J-class.
    uint size_lc = size_dequeue(G->LCL->cl[G->LCL->numcl[elem]]);

    uint row_length = size_rc / size_hc;
    uint col_length = size_lc / size_hc;

    // Maximum name size in the J-class to display.
    uint name_size;
    if (remap) {
        dequeue* origelem = create_dequeue();
        for (uint i = 0; i < size_dequeue(G->JCL->cl[jclass]); i++) {
            rigins_dequeue(remap[lefread_dequeue(G->JCL->cl[jclass], i)], origelem);
        }
        name_size = mor_max_name_size(M, origelem);
        delete_dequeue(origelem);
    }
    else {
        name_size = mor_max_name_size(M, G->JCL->cl[jclass]);
    }

    // Number of elements displayed per line in an H-class.
    double db = sqrt(size_hc);
    uint num_elem_line = (uint)ceil(db);

    uint height_hclass;
    if (num_elem_line * (num_elem_line - 1) < size_hc) {
        height_hclass = num_elem_line;
    }
    else {
        height_hclass = num_elem_line - 1;
    }

    // Length of a line in an H-class.
    uint h_line_size = name_size * num_elem_line + num_elem_line + 3;

    // Sorting the J-class in an array.
    uint* thejclass = green_sorted_jclass(G, jclass);


    // Output of the first line.
    print_spaces(padding, out);
    fprintf(out, "┌");
    for (uint i = 0; i < row_length; i++) {
        print_copies_string(h_line_size, "─", out);
        if (i == row_length - 1) {
            fprintf(out, "┐\n");
        }
        else {
            fprintf(out, "┬");
        }
    }

    // Output of the classes.
    for (uint j = 0; j < col_length; j++) // Loop on the L-classes.
    {
        print_spaces(padding, out);
        // Top empty line.
        fprintf(out, "│");
        for (uint i = 0; i < row_length; i++) {
            uint s = thejclass[(j * size_rc) + i * size_hc];

            if (G->group_array[s]) {
                print_spaces(h_line_size - 1, out);
                fprintf(out, "*");
            }
            else {
                print_spaces(h_line_size, out);
            }
            if (i == row_length - 1) {
                fprintf(out, "│\n");
            }
            else {
                fprintf(out, "│");
            }
        }

        for (uint k = 0; k < height_hclass; k++) {
            print_spaces(padding, out);
            for (uint i = 0; i < row_length; i++) {
                fprintf(out, "│  ");
                for (uint h = 0; h < num_elem_line; h++) {
                    uint count = k * (num_elem_line)+h;
                    if (count < size_hc) {
                        if (remap) {
                            mor_fprint_name_utf8_aligned(M, remap[thejclass[(j * size_rc) + i * size_hc + count]], name_size, out);
                        }
                        else {
                            mor_fprint_name_utf8_aligned(M, thejclass[(j * size_rc) + i * size_hc + count], name_size, out);
                        }
                    }
                    else {
                        print_spaces(name_size, out);
                    }

                    fprintf(out, " ");
                }
                fprintf(out, " ");
            }
            fprintf(out, "│\n");
        }

        print_spaces(padding, out);

        // Bottom empty line.
        fprintf(out, "│");
        for (uint i = 0; i < row_length; i++) {
            print_copies_string(h_line_size, " ", out);
            if (i == row_length - 1) {
                fprintf(out, "│\n");
            }
            else {
                fprintf(out, "│");
            }
        }
        print_spaces(padding, out);
        if (j == col_length - 1) {

            // Output of the last line.
            fprintf(out, "└");
            for (uint i = 0; i < row_length; i++) {
                print_copies_string(h_line_size, "─", out);
                if (i == row_length - 1) {
                    fprintf(out, "┘\n");
                }
                else {
                    fprintf(out, "┴");
                }
            }
        }
        else {
            // Output of an intermediate line.
            fprintf(out, "├");
            for (uint i = 0; i < row_length; i++) {
                print_copies_string(h_line_size, "─", out);
                if (i == row_length - 1) {
                    fprintf(out, "┤\n");
                }
                else {
                    fprintf(out, "┼");
                }
            }
        }
    }
    free(thejclass);

}


void print_jclass_green(morphism* M, uint jclass, uint padding, FILE* out) {
    print_jclass_green_aux(M, M->rels, NULL, jclass, padding, out);
}

void print_jclass_subsemi(subsemi* S, uint jclass, uint padding, FILE* out) {
    print_jclass_green_aux(S->original, S->rels, S->sub_to_mono, jclass, padding, out);
}

void print_full_green(morphism* M, bool title, FILE* out) {
    if (title) {
        print_title_box(0, true, out, 1, "Green relations of the monoid.");
    }
    print_infos_green(M, out);

    if (M->r_cayley->size_graph <= 1000) {
        fprintf(out, "#### The J-classes (ordered using a topological sort).\n");
        for (uint i = M->rels->JCL->size_par; i > 0; i--) {
            print_jclass_green(M, i - 1, 5, out);
        }
    }
}

void print_full_subsemi(subsemi* S, FILE* out) {

    print_infos_green_sub(S, out);
    if (S->level != LV_FULL) {

        if (S->size <= 1000) {
            fprintf(out, "#### The regular J-classes (ordered using a topological sort).\n");
            for (uint i = S->rels->JCL->size_par; i > 0; i--) {
                uint s = lefread_dequeue(S->rels->JCL->cl[i - 1], 0);
                if (S->rels->regular_array[s]) {
                    print_jclass_subsemi(S, i - 1, 5, out);
                }
            }
        }
    }
    else {
        if (S->size <= 1000) {
            fprintf(out, "#### The J-classes (ordered using a topological sort).\n");
            for (uint i = S->rels->JCL->size_par; i > 0; i--) {
                print_jclass_subsemi(S, i - 1, 5, out);
            }
        }
    }

}



static void shell_print_alphabet(morphism* M) {
    if (M->r_cayley->size_alpha == 0) {
        fprintf(stdout, "∅");
        return;
    }

    fprintf(stdout, "{");
    fprint_letter_utf8(M->alphabet[0], stdout);
    for (uint i = 1; i < M->r_cayley->size_alpha; i++) {
        fprintf(stdout, ",");
        fprint_letter_utf8(M->alphabet[i], stdout);
    }
    fprintf(stdout, "}\n");
}

void view_morphism(morphism* themorphism, FILE* out) {
    fprintf(out, "#### Alphabet  : ");
    shell_print_alphabet(themorphism);
    fprintf(out, "#### Monoid    :\n");
    print_full_green(themorphism, false, out);
    fprintf(out, "#### Morphism  :\n");
    mor_print_mapping(themorphism, out);
}

void print_one_orb(orbits* L, uint ob_num, char* class, FILE* out) {
    if (L->orbits[ob_num]) {
        print_sep_line(100, out);
        fprintf(out, "#### %s-orbit of the idempotent ", class);
        mor_fprint_name_utf8(L->original, L->orbits[ob_num]->sub_to_mono[L->orbits[ob_num]->neut], out);
        printf(".\n");
        print_full_subsemi(L->orbits[ob_num], out);
    }
}


void print_all_orbs(orbits* L, char* class, FILE* out) {
    fprintf(out, "#### Printing the %s-orbits:\n", class);
    fprintf(out, "#### Only a part of the orbits are computed.\n");
    fprintf(out, "#### All non-computed orbits are a subsemigroup of a computed one.\n");
    for (uint i = 0; i < L->nb_computed; i++) {
        print_one_orb(L, i, class, out);
    }
    return;
}


