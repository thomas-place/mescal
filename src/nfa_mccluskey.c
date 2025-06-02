#include "nfa_mccluskey.h"

mccluskey_auto* nfa_to_mccluskey(nfa* A) {
    if (!A) {
        return NULL;
    }

    mccluskey_auto* grid;
    MALLOC(grid, 1);
    grid->size = A->trans->size_graph + 2;
    MALLOC(grid->matrix, grid->size);
    for (uint i = 0;i < grid->size;i++) {
        CALLOC(grid->matrix[i], grid->size);
    }

    for (uint i = 0; i < size_dequeue(A->initials); i++) {
        grid->matrix[0][lefread_dequeue(A->initials, i) + 2] = reg_epsilon();
    }

    for (uint i = 0; i < size_dequeue(A->finals); i++) {
        grid->matrix[lefread_dequeue(A->finals, i) + 2][1] = reg_epsilon();
    }

    for (uint q = 0; q < A->trans->size_graph;q++) {
        for (uint a = 0; a < A->trans->size_alpha;a++) {
            for (uint i = 0; i < size_dequeue(A->trans->edges[q][a]); i++) {
                uint r = lefread_dequeue(A->trans->edges[q][a], i);
                regexp* new = reg_letter_ext(A->alphabet[a]);
                if (grid->matrix[q + 2][r + 2]) {
                    grid->matrix[q + 2][r + 2] = reg_union(grid->matrix[q + 2][r + 2], new);
                }
                else {
                    grid->matrix[q + 2][r + 2] = new;
                }
            }
        }
    }

    if (A->trans_e) {
        for (uint q = 0; q < A->trans->size_graph;q++) {
            for (uint i = 0; i < size_dequeue(A->trans_e->edges[q]); i++) {
                uint r = lefread_dequeue(A->trans_e->edges[q], i);
                regexp* new = reg_epsilon();
                if (grid->matrix[q + 2][r + 2]) {
                    grid->matrix[q + 2][r + 2] = reg_union(grid->matrix[q + 2][r + 2], new);
                }
                else {
                    grid->matrix[q + 2][r + 2] = new;
                }

            }
        }

    }


    return grid;
}


mccluskey_auto* dfa_to_mccluskey(dfa* A) {
    if (!A) {
        return NULL;
    }

    mccluskey_auto* grid;
    MALLOC(grid, 1);
    grid->size = A->trans->size_graph + 2;
    MALLOC(grid->matrix, grid->size);
    for (uint i = 0;i < grid->size;i++) {
        CALLOC(grid->matrix[i], grid->size);
    }

    grid->matrix[0][A->initial + 2] = reg_epsilon();


    for (uint i = 0; i < A->nb_finals; i++) {
        grid->matrix[A->finals[i] + 2][1] = reg_epsilon();
    }

    for (uint q = 0; q < A->trans->size_graph;q++) {
        for (uint a = 0; a < A->trans->size_alpha;a++) {
            uint r = A->trans->edges[q][a];
            regexp* new = reg_letter_ext(A->alphabet[a]);
            if (grid->matrix[q + 2][r + 2]) {
                grid->matrix[q + 2][r + 2] = reg_union(grid->matrix[q + 2][r + 2], new);
            }
            else {
                grid->matrix[q + 2][r + 2] = new;
            }

        }
    }



    return grid;
}

static regexp* aux_mccluskey(mccluskey_auto* grid) {
    for (uint q = grid->size - 1; q > 1; q--) {
        for (uint r = 0; r < q; r++) {
            if (grid->matrix[r][q]) {
                for (uint s = 0; s < q; s++) {
                    if (grid->matrix[q][s]) {
                        regexp* new;
                        if (grid->matrix[q][q])
                        {
                            new = reg_concat(reg_copy(grid->matrix[r][q]), reg_concat(reg_star(reg_copy(grid->matrix[q][q])), reg_copy(grid->matrix[q][s])));
                        }
                        else {
                            new = reg_concat(reg_copy(grid->matrix[r][q]), reg_copy(grid->matrix[q][s]));

                        }
                        if (grid->matrix[r][s]) {
                            grid->matrix[r][s] = reg_union(grid->matrix[r][s], new);
                        }
                        else {
                            grid->matrix[r][s] = new;
                        }
                    }

                }
            }

        }
    }

    regexp* ret = reg_copy(grid->matrix[0][1]);
    for (uint i = 0;i < grid->size;i++) {
        for (uint j = 0;j < grid->size;j++) {
            reg_free(grid->matrix[i][j]);
        }
        free(grid->matrix[i]);
    }
    free(grid->matrix);
    free(grid);



    return ret;
}

regexp* nfa_mccluskey(nfa* A) {
    nfa* B = nfa_trim(A);
    mccluskey_auto* grid = nfa_to_mccluskey(B);
    nfa_delete(B);

    return aux_mccluskey(grid);
}



regexp* dfa_mccluskey(dfa* A) {
    mccluskey_auto* grid = dfa_to_mccluskey(A);
    return aux_mccluskey(grid);
}
