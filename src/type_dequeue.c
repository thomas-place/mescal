/*****************************************/
/* Implémentations des listes de sommets */
/*  Utilisation d'une dequeue par liste  */
/*****************************************/

#include "type_dequeue.h"
#include "alloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*************************/
/* Fonctions auxiliaires */
/*************************/

// Double la taille du tableau utilisé dans la liste.
static void grow_dequeue(dequeue *p) {
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    uint n        = p->size_array;
    p->size_array = n * 2;
    REALLOC(p->array, p->size_array);
    if (p->right <= p->left && !(p->empty)) {
        for (uint i = 0; i < p->right; i++) {
            p->array[n + i] = p->array[i];
        }
        p->right = n + p->right;
    }
}

// Inversion d'un sous-tableau donné
static void reverse_array(uint *array, uint l, uint r) {
    uint i, j;
    for (i = l, j = r; i < j; i++, j--) {
        uint temp = array[i];
        array[i]  = array[j];
        array[j]  = temp;
    }
}

// Divise par deux la taille du tableau utilisé dans la représentation
// (on supposera que seulement la moitié des cases sont utilisées dans
// la représentation).
static void shrink_dequeue(dequeue *p) {
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    if (p->size_array == 1) {
        return;
    }

    uint size = size_dequeue(p);

    // Rotation du tableau pour mettre la valeur gauche du tuyau
    // dans la case 0

    if (p->left != 0) {
        reverse_array(p->array, 0, p->left - 1);
        reverse_array(p->array, p->left, p->size_array - 1);
        reverse_array(p->array, 0, p->size_array - 1);
    }

    // Mise à jour des informations
    p->size_array = p->size_array / 2;
    p->left       = 0;
    if (size <= p->size_array) {
        p->right = size;
    } else {
        p->right = p->size_array;
    }

    // Réallocation

    REALLOC(p->array, p->size_array);
}

/************************/
/* Fonctions primitives */
/************************/

/* Création */
dequeue *create_dequeue(void) {
    dequeue *new;
    MALLOC(new, 1);
    MALLOC(new->array, 1);
    new->size_array = 1;
    new->left       = 0;
    new->right      = 0;
    new->empty      = true;
    return new;
}

/* Suppression */
void delete_dequeue(dequeue *p) {
    if (p == NULL) {
        return;
    }
    free(p->array);
    free(p);
}

/* Test du vide */
bool isempty_dequeue(dequeue *p) {
    return p->empty;
}

/* Vidage d'une deq*/
void makeempty_dequeue(dequeue *p) {
    p->left  = 0;
    p->right = 0;
    p->empty = true;
}

/* Taille */
uint size_dequeue(dequeue *p) {
    if (p->empty) {
        return 0;
    } else if (p->left < p->right) {
        return p->right - p->left;
    } else {
        return (p->size_array - p->left) + p->right;
    }
}

/* Lecture */
uint lefread_dequeue(dequeue *p, uint i) {
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    if (size_dequeue(p) - 1 < i) {
        fprintf(stderr,
                "Error, the dequeue is not large enough.\nSize: %d.\nIndex "
                "read: %d\n",
                size_dequeue(p), i);
        exit(EXIT_FAILURE);
    }
    return p->array[(p->left + i) % p->size_array];
}
uint rigread_dequeue(dequeue *p, uint i) {
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    if (size_dequeue(p) - 1 < i) {
        fprintf(stderr,
                "Error, the dequeue is not large enough.\nSize: %d.\nIndex "
                "read: %d\n",
                size_dequeue(p), i);
        exit(EXIT_FAILURE);
    }
    if (p->right > i) {
        // printf("Read there 1:%d\n", p->right - i - 1);
        return p->array[p->right - i - 1];
    } else {
        // printf("Read there 12:%d\n", p->size_array - (i + 1));
        return p->array[p->size_array - (i + 1)];
    }
}

/* Insérer */
void lefins_dequeue(uint val, dequeue *p) {
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    if (p->right <= p->left && !(p->empty)) {
        grow_dequeue(p);
    }
    if (p->left == 0) {
        p->left = p->size_array - 1;
    } else {
        p->left--;
    }
    p->array[p->left] = val;
    p->empty          = false;
}

void rigins_dequeue(uint val, dequeue *p) {
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    if (p->right <= p->left && !(p->empty)) {
        grow_dequeue(p);
    }
    p->array[p->right] = val;
    p->empty           = false;
    if (p->right == p->size_array - 1) {
        p->right = 0;
    } else {
        p->right++;
    }
}

/* Retirer */
uint lefpull_dequeue(dequeue *p) {
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    uint val = p->array[p->left];
    if (p->left == p->size_array - 1) {
        p->left = 0;
    } else {
        p->left++;
    }
    if (p->left == p->right) {
        p->empty = true;
    }
    if (p->size_array >= 2 && size_dequeue(p) <= p->size_array / 4) {
        shrink_dequeue(p);
    }

    return val;
}
uint rigpull_dequeue(dequeue *p) {
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    if (p->right == 0) {
        p->right = p->size_array - 1;
    } else {
        p->right--;
    }
    if (p->left == p->right) {
        p->empty = true;
    }
    if (p->size_array >= 2 && size_dequeue(p) <= p->size_array / 4) {
        shrink_dequeue(p);
    }
    return p->array[p->right];
}

/* Création d'une copie avec décalage */
void copy_dequeue_right(dequeue *incopy, dequeue *tocopy, uint lag) {
    uint n = size_dequeue(tocopy);
    for (uint i = 0; i < n; i++) {
        rigins_dequeue(lag + lefread_dequeue(tocopy, i), incopy);
    }
}

/* Teste si un élément appartient à une dequeue*/
bool mem_dequeue(uint e, dequeue *p) {
    for (uint i = 0; i < size_dequeue(p); i++) {
        if (e == lefread_dequeue(p, i)) {
            return true;
        }
    }
    return false;
}

/* Affichage */
void print_dequeue(dequeue *p) {
    if (isempty_dequeue(p)) {
        printf(" vide\n");
        return;
    }
    uint i = p->left;
    if (i < p->right) {
        while (i < p->right) {
            printf("  %d", p->array[i]);
            i++;
        }
    } else {
        while (i < p->size_array) {
            printf("  %d", p->array[i]);
            i++;
        }
        i = 0;
        while (i < p->right) {
            printf("  %d", p->array[i]);
            i++;
        }
    }
    printf("\n");
}

static void aux_quick_sort_dequeue_norepeat(uint *array, uint i, uint j) {
    if (i + 1 >= j) { // Il y a moins d'un élément
        return;
    } else {
        uint pivot = array[i];
        uint l     = j;

        for (uint k = j - 1; i < k; k--) {
            if (pivot < array[k]) {
                l--;
                uint temp = array[l];
                array[l]  = array[k];
                array[k]  = temp;
            }
        }
        uint temp    = array[l - 1];
        array[l - 1] = array[i];
        array[i]     = temp;

        aux_quick_sort_dequeue_norepeat(array, i, l - 1);
        aux_quick_sort_dequeue_norepeat(array, l, j);
    }
}

static bool is_sorted_dequeue(uint *array, uint len) {
    // printf("begin\n");
    for (uint i = 0; i < len - 1; i++) {
        if (array[i] >= array[i + 1]) {
            //      printf("end\n");
            return false;
        }
    }
    // printf("end\n");
    return true;
}

/* Tri par ordre croissant et suppression des doublons */
void sort_dequeue_norepeat(dequeue *p) {
    // printf("mid1\n");
    uint len = size_dequeue(p);
    // Rotation du tableau pour mettre la valeur gauche du tuyau dans la case 0
    if (p->left != 0) {
        reverse_array(p->array, 0, p->left - 1);
        reverse_array(p->array, p->left, p->size_array - 1);
        reverse_array(p->array, 0, p->size_array - 1);
    }
    p->left  = 0;
    p->right = len;
    // printf("mid2\n");
    if (!is_sorted_dequeue(p->array, len)) {
        // printf("mid3\n");
        aux_quick_sort_dequeue_norepeat(p->array, 0, len);
    }
    // printf("mid4\n");
    uint d = 0;
    for (uint i = 1; i < len; i++) {
        if (p->array[i] == p->array[i - 1]) {
            d++;
        } else {
            p->array[i - d] = p->array[i];
        }
    }
    p->right = len - d;
    // printf("end\n");
}

/*****************************************************************/
/* Fonctions spécifiques aux ensembles triés par ordre croissant */
/*****************************************************************/

static bool mem_dequeue_sorted_aux(uint e, dequeue *p, uint i, uint j,
                                   uint *ind) {
    if (j <= i) {
        return false;
    } else {
        uint mid = (i + j) / 2;
        lefread_dequeue(p, mid);
        if (lefread_dequeue(p, mid) == e) {
            if (ind) {
                *ind = mid;
            }
            return true;
        } else if (lefread_dequeue(p, mid) < e) {
            return mem_dequeue_sorted_aux(e, p, mid + 1, j, ind);
        } else {
            return mem_dequeue_sorted_aux(e, p, i, mid, ind);
        }
    }
}

bool mem_dequeue_sorted(uint e, dequeue *p, uint *ind) {
    return mem_dequeue_sorted_aux(e, p, 0, size_dequeue(p), ind);
}

void merge_sorted_dequeue(dequeue *p1, dequeue *p2) {
    // printf("test\n");
    if (p1 == p2) {
        // Si ce sont les deux mêmes listes
        return;
    }
    uint n1 = size_dequeue(p1);
    uint n2 = size_dequeue(p2);
    // uint tab[n1 + n2];
    uint i  = 0; // Nb d'éléments traités dans p1
    uint j  = 0; // Indice dans p2

    while (i < n1 || j < n2) {
        if (i == n1 ||
            (j < n2 && lefread_dequeue(p2, j) < lefread_dequeue(p1, 0))) {
            if ((i == 0 && j == 0) ||
                lefread_dequeue(p2, j) != rigread_dequeue(p1, 0)) {
                rigins_dequeue(lefread_dequeue(p2, j), p1);
            }
            j++;
        } else {
            if ((i == 0 && j == 0) ||
                lefread_dequeue(p1, 0) != rigread_dequeue(p1, 0)) {
                rigins_dequeue(lefpull_dequeue(p1), p1);
                i++;
            }
        }
    }
}

dequeue *make_inter_sorted_dequeue(dequeue *p1, dequeue *p2) {
    dequeue *res = create_dequeue();
    uint n1      = size_dequeue(p1);
    uint n2      = size_dequeue(p2);
    uint i       = 0; // Indice dans p1
    uint j       = 0; // Indice dans p2

    while (i < n1 && j < n2) {
        if (lefread_dequeue(p1, i) < lefread_dequeue(p2, j)) {
            i++;
        } else if (lefread_dequeue(p1, i) > lefread_dequeue(p2, j)) {
            j++;
        } else {
            rigins_dequeue(lefread_dequeue(p1, i), res);
            i++;
            j++;
        }
    }
    return res;
}

/* Insére un nouveau sommet dans une liste */
void insert_dequeue(dequeue *p1, uint q) {
    if (isempty_dequeue(p1) || rigread_dequeue(p1, 0) < q) {
        rigins_dequeue(q, p1);
    } else if (rigread_dequeue(p1, 0) == q) {
        return;
    } else {
        uint r = rigpull_dequeue(p1);
        insert_dequeue(p1, q);
        rigins_dequeue(r, p1);
    }
}

/* Teste si deux dequeues triées s'intersectent */
bool intersec_dequeue(dequeue *p1, dequeue *p2) {
    if (!p1 || !p2) {
        return false;
    }
    if (p1 == p2) {
        // Si ce sont les deux mêmes listes
        return true;
    }
    uint n1 = size_dequeue(p1);
    uint n2 = size_dequeue(p2);
    uint i  = 0; // indice dans p1
    uint j  = 0; // indice dans p2
    while (i < n1 && j < n2) {
        if (lefread_dequeue(p1, i) == lefread_dequeue(p2, j)) {
            return true;
        } else if (lefread_dequeue(p1, i) < lefread_dequeue(p2, j)) {
            i++;
        } else {
            j++;
        }
    }
    return false;
}

dequeue *dequeue_to_indices(dequeue *main, dequeue *sub) {
    dequeue *ret = create_dequeue();
    uint j       = 0;
    uint i       = 0;

    while (i < size_dequeue(sub) && j < size_dequeue(main)) {
        if (lefread_dequeue(sub, i) == lefread_dequeue(main, j)) {
            rigins_dequeue(j, ret);
            i++;
        }
        j++;
    }
    return ret;
}

/* Fusionne toutes les listes triées contenues dans un tableau dans la liste out
 * (supposée vide) */
void merge_array_sorted_dequeue(dequeue *out, dequeue **array, uint size) {
    // Calcul du nombre total d'éléments à traiter et initialisations des
    // indices
    uint max_size = 0;
    uint index[size];
    for (uint i = 0; i < size; i++) {
        max_size = max_size + size_dequeue(array[i]);
        index[i] = 0;
    }

    // Insertion des éléments dans la liste out
    uint j = 0;
    while (j < max_size) {
        // Calcul de la valeur minimale
        uint k = 0;
        while (index[k] >= size_dequeue(array[k])) {
            k++;
        }
        uint min = lefread_dequeue(array[k], index[k]);
        for (uint i = k + 1; i < size; i++) {
            if (index[i] < size_dequeue(array[i]) &&
                lefread_dequeue(array[i], index[i]) < min) {
                min = lefread_dequeue(array[i], index[i]);
            }
        }

        // On avance l'index de chaque liste contenant la valeur min
        for (uint i = 0; i < size; i++) {
            if (index[i] < size_dequeue(array[i]) &&
                lefread_dequeue(array[i], index[i]) == min) {
                index[i]++;
                j++;
            }
        }

        // On insère la valeur dans out
        rigins_dequeue(min, out);
    }
}
