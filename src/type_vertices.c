/*****************************************/
/* Implémentations des listes de sommets */
/*  Utilisation d'une dequeue par liste  */
/*****************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "type_vertices.h"
#include "macros_alloc.h"

/*************************/
/* Fonctions auxiliaires */
/*************************/

// Double la taille du tableau utilisé dans la liste.
static void grow_vertices(p_vertices p)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    uint n = p->size_array;
    p->size_array = n * 2;
    REALLOC(p->array, p->size_array);
    if (p->right <= p->left && !(p->empty))
    {

        for (uint i = 0; i < p->right; i++)
        {
            p->array[n + i] = p->array[i];
        }
        p->right = n + p->right;
    }
}

// Inversion d'un sous-tableau donné
static void reverse_array(uint* array, uint l, uint r)
{
    uint i, j;
    for (i = l, j = r; i < j; i++, j--)
    {
        uint temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Divise par deux la taille du tableau utilisé dans la représentation
// (on supposera que seulement la moitié des cases sont utilisées dans
// la représentation).
static void shrink_vertices(p_vertices p)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    if (p->size_array == 1)
    {
        return;
    }

    uint size = size_vertices(p);

    // Rotation du tableau pour mettre la valeur gauche du tuyau
    // dans la case 0

    if (p->left != 0)
    {
        reverse_array(p->array, 0, p->left - 1);
        reverse_array(p->array, p->left, p->size_array - 1);
        reverse_array(p->array, 0, p->size_array - 1);
    }

    // Mise à jour des informations
    p->size_array = p->size_array / 2;
    p->left = 0;
    if (size <= p->size_array)
    {
        p->right = size;
    }
    else
    {
        p->right = p->size_array;
    }

    // Réallocation

    REALLOC(p->array, p->size_array);
}

/************************/
/* Fonctions primitives */
/************************/

/* Création */
p_vertices create_vertices(void)
{
    p_vertices new;
    MALLOC(new, 1);
    MALLOC(new->array, 1);
    new->size_array = 1;
    new->left = 0;
    new->right = 0;
    new->empty = true;
    return new;
}

/* Suppression */
void delete_vertices(p_vertices p)
{
    if (p == NULL)
    {
        return;
    }
    free(p->array);
    free(p);
}

/* Test du vide */
bool isempty_vertices(p_vertices p)
{
    return p->empty;
}

/* Vidage d'une deq*/
void makeempty_vertices(p_vertices p)
{
    p->left = 0;
    p->right = 0;
    p->empty = true;
}

/* Taille */
uint size_vertices(p_vertices p)
{
    if (p->empty)
    {
        return 0;
    }
    else if (p->left < p->right)
    {
        return p->right - p->left;
    }
    else
    {
        return (p->size_array - p->left) + p->right;
    }
}

/* Lecture */
uint lefread_vertices(p_vertices p, uint i)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    if (size_vertices(p) - 1 < i)
    {
        fprintf(stderr, "Error, the list of vertices is not large enough.\nSize: %d.\nIndex read: %d\n", size_vertices(p), i);
        exit(EXIT_FAILURE);
    }
    return p->array[(p->left + i) % p->size_array];
}
uint rigread_vertices(p_vertices p, uint i)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    if (size_vertices(p) - 1 < i)
    {
        fprintf(stderr, "Error, the list of vertices is not large enough.\nSize: %d.\nIndex read: %d\n", size_vertices(p), i);
        exit(EXIT_FAILURE);
    }
    if (p->right > i)
    {
        // printf("Read there 1:%d\n", p->right - i - 1);
        return p->array[p->right - i - 1];
    }
    else
    {
        // printf("Read there 12:%d\n", p->size_array - (i + 1));
        return p->array[p->size_array - (i + 1)];
    }
}

uint* lefread_vertices_ref(p_vertices p, uint i)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    if (size_vertices(p) - 1 < i)
    {
        fprintf(stderr, "Error, the list of vertices is not large enough.\nSize: %d.\nIndex read: %d\n", size_vertices(p), i);
        exit(EXIT_FAILURE);
    }
    return (p->array + (p->left + i) % p->size_array);
}
uint* rigread_vertices_ref(p_vertices p, uint i)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    if (size_vertices(p) - 1 < i)
    {
        fprintf(stderr, "Error, the list of vertices is not large enough.\nSize: %d.\nIndex read: %d\n", size_vertices(p), i);
        exit(EXIT_FAILURE);
    }
    if (p->right > i)
    {
        return (p->array + p->right - i - 1);
    }
    else
    {
        return (p->array + p->size_array - (i + 1));
    }
}

/* Insérer */
void lefins_vertices(uint val, p_vertices p)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    if (p->right <= p->left && !(p->empty))
    {
        grow_vertices(p);
    }
    if (p->left == 0)
    {
        p->left = p->size_array - 1;
    }
    else
    {
        p->left--;
    }
    p->array[p->left] = val;
    p->empty = false;
}

void rigins_vertices(uint val, p_vertices p)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    if (p->right <= p->left && !(p->empty))
    {
        grow_vertices(p);
    }
    p->array[p->right] = val;
    p->empty = false;
    if (p->right == p->size_array - 1)
    {
        p->right = 0;
    }
    else
    {
        p->right++;
    }
}

/* Retirer */
uint lefpull_vertices(p_vertices p)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    uint val = p->array[p->left];
    if (p->left == p->size_array - 1)
    {
        p->left = 0;
    }
    else
    {
        p->left++;
    }
    if (p->left == p->right)
    {
        p->empty = true;
    }
    if (p->size_array >= 2 && size_vertices(p) <= p->size_array / 4)
    {
        shrink_vertices(p);
    }

    return val;
}
uint rigpull_vertices(p_vertices p)
{
    CHECK_NULL(2, p, "The list of vertices", p->array, "The array in the list of vertices");
    if (p->right == 0)
    {
        p->right = p->size_array - 1;
    }
    else
    {
        p->right--;
    }
    if (p->left == p->right)
    {
        p->empty = true;
    }
    if (p->size_array >= 2 && size_vertices(p) <= p->size_array / 4)
    {
        shrink_vertices(p);
    }
    return p->array[p->right];
}

/* Création d'une copie avec décalage */
void copy_vertices_right(p_vertices incopy, p_vertices tocopy, uint lag)
{
    uint n = size_vertices(tocopy);
    for (uint i = 0; i < n; i++)
    {
        rigins_vertices(lag + lefread_vertices(tocopy, i), incopy);
    }
}

/* Teste si un élément appartient à une vertices*/
bool mem_vertices(uint e, p_vertices p)
{
    for (uint i = 0; i < size_vertices(p); i++)
    {
        if (e == lefread_vertices(p, i))
        {
            return true;
        }
    }
    return false;
}

/* Affichage */
void print_vertices(p_vertices p)
{
    if (isempty_vertices(p))
    {
        printf(" vide\n");
        return;
    }
    uint i = p->left;
    if (i < p->right)
    {
        while (i < p->right)
        {
            printf("  %d", p->array[i]);
            i++;
        }
    }
    else
    {
        while (i < p->size_array)
        {
            printf("  %d", p->array[i]);
            i++;
        }
        i = 0;
        while (i < p->right)
        {
            printf("  %d", p->array[i]);
            i++;
        }
    }
    printf("\n");
}

/* Affichage d'une liste de lettres */
void print_vertices_alpha(p_vertices p, FILE* out)
{
    if (isempty_vertices(p))
    {
        fprintf(out, "∅.\n");
        return;
    }
    fprintf(out, "{");
    uint i = p->left;
    if (i < p->right)
    {
        while (i < p->right - 1)
        {
            fprintf(out, "%c,", p->array[i] + 'a');
            i++;
        }
        fprintf(out, "%c", p->array[p->right - 1] + 'a');
    }
    else if (p->right == 0)
    {
        while (i < p->size_array - 1)
        {
            fprintf(out, "%c", p->array[i] + 'a');
            i++;
        }
        fprintf(out, "%c", p->array[p->size_array - 1] + 'a');
    }
    else
    {
        while (i < p->size_array)
        {
            fprintf(out, "%c", p->array[i] + 'a');
            i++;
        }
        i = 0;
        while (i < p->right - 1)
        {
            fprintf(out, "%c", p->array[i] + 'a');
            i++;
        }
        fprintf(out, "%c", p->array[p->right - 1] + 'a');
    }
    fprintf(out, "}.\n");
}

static void aux_quick_sort_vertices(uint* array, uint i, uint j) {
    // printf("i: %d, j:%d\n", i, j);
    if (i + 1 >= j) { // Il y a moins d'un élément
        return;
    }
    else {
        uint pivot = array[i];
        uint l = j;

        for (uint k = j - 1; i < k; k--) {
            if (pivot < array[k]) {
                l--;
                uint temp = array[l];
                array[l] = array[k];
                array[k] = temp;
            }
        }
        uint temp = array[l - 1];
        array[l - 1] = array[i];
        array[i] = temp;

        aux_quick_sort_vertices(array, i, l - 1);
        aux_quick_sort_vertices(array, l, j);
    }
}

static bool is_sorted_vertices(uint* array, uint len) {
    for (uint i = 0; i < len - 1;i++) {
        if (array[i] > array[i + 1]) {
            return false;
        }
    }
    return true;
}

/* Tri par ordre croissant et suppression des doublons */
void sort_vertices(p_vertices p) {

    uint len = size_vertices(p);
    // Rotation du tableau pour mettre la valeur gauche du tuyau dans la case 0
    if (p->left != 0)
    {
        reverse_array(p->array, 0, p->left - 1);
        reverse_array(p->array, p->left, p->size_array - 1);
        reverse_array(p->array, 0, p->size_array - 1);
    }
    p->left = 0;
    p->right = len;
    if (!is_sorted_vertices(p->array, len)) {
        aux_quick_sort_vertices(p->array, 0, len);
    }

    uint d = 0;
    for (uint i = 1; i < len;i++) {
        if (p->array[i] == p->array[i - 1]) {
            d++;
        }
        else {
            p->array[i - d] = p->array[i];
        }

    }
    p->right = len - d;



}

/*****************************************************************/
/* Fonctions spécifiques aux ensembles triés par ordre croissant */
/*****************************************************************/

/* Appartenance d'un élément dans une liste triée */
static bool mem_vertices_sorted_aux(uint e, p_vertices p, uint i, uint j)
{
    if (j <= i)
    {
        return false;
    }
    else
    {

        uint mid = (i + j) / 2;
        lefread_vertices(p, mid);
        if (lefread_vertices(p, mid) == e)
        {
            return true;
        }
        else if (lefread_vertices(p, mid) < e)
        {
            return mem_vertices_sorted_aux(e, p, mid + 1, j);
        }
        else
        {
            return mem_vertices_sorted_aux(e, p, i, mid);
        }
    }
}

bool mem_vertices_sorted(uint e, p_vertices p)
{
    return mem_vertices_sorted_aux(e, p, 0, size_vertices(p));
}

/* Fusionne deux listes triées */
void merge_sorted_vertices(p_vertices p1, p_vertices p2)
{
    if (p1 == p2)
    {
        // Si ce sont les deux mêmes listes
        return;
    }
    uint n1 = size_vertices(p1);
    uint n2 = size_vertices(p2);
    // uint tab[n1 + n2];
    uint i = 0; // Nb d'éléments traités dans p1
    uint j = 0; // Indice dans p2

    while (i < n1 || j < n2)
    {
        if (i == n1 || (j < n2 && lefread_vertices(p2, j) < lefread_vertices(p1, 0)))
        {
            if ((i == 0 && j == 0) || lefread_vertices(p2, j) != rigread_vertices(p1, 0))
            {
                rigins_vertices(lefread_vertices(p2, j), p1);
            }
            j++;
        }
        else
        {
            if ((i == 0 && j == 0) || lefread_vertices(p1, 0) != rigread_vertices(p1, 0))
            {
                rigins_vertices(lefpull_vertices(p1), p1);
                i++;
            }
        }
    }

    // while (!isempty_vertices(p1) || i < n2)
    // {
    //     if (!isempty_vertices(p1) && i < n2 && lefread_vertices(p1, 0) == lefread_vertices(p2, i))
    //     {
    //         tab[j] = lefpull_vertices(p1);
    //         i++;
    //         j++;
    //     }
    //     else if (!isempty_vertices(p1) && (i >= n2 || lefread_vertices(p1, 0) < lefread_vertices(p2, i)))
    //     {
    //         tab[j] = lefpull_vertices(p1);
    //         j++;
    //     }
    //     else
    //     {
    //         tab[j] = lefread_vertices(p2, i);
    //         i++;
    //         j++;
    //     }
    // }
    // for (uint k = 0; k < j; k++)
    // {
    //     rigins_vertices(tab[k], p1);
    // }
}

/* Calcule l'intersection de deux listes triées de sommets */
p_vertices make_inter_sorted_vertices(p_vertices p1, p_vertices p2)
{
    p_vertices res = create_vertices();
    uint n1 = size_vertices(p1);
    uint n2 = size_vertices(p2);
    uint i = 0; // Indice dans p1
    uint j = 0; // Indice dans p2

    while (i < n1 && j < n2)
    {
        if (lefread_vertices(p1, i) < lefread_vertices(p2, j))
        {
            i++;
        }
        else if (lefread_vertices(p1, i) > lefread_vertices(p2, j))
        {
            j++;
        }
        else
        {
            rigins_vertices(lefread_vertices(p1, i), res);
            i++;
            j++;
        }
    }
    return res;
}

/* Fusionne toutes les listes triées contenues dans un tableau dans la liste out (supposée vide) */
void merge_array_sorted_vertices(p_vertices out, p_vertices* array, uint size)
{
    // Calcul du nombre total d'éléments à traiter et initialisations des indices
    uint max_size = 0;
    uint index[size];
    for (uint i = 0; i < size; i++)
    {
        max_size = max_size + size_vertices(array[i]);
        index[i] = 0;
    }

    // Insertion des éléments dans la liste out
    uint j = 0;
    while (j < max_size)
    {
        // Calcul de la valeur minimale
        uint k = 0;
        while (index[k] >= size_vertices(array[k]))
        {
            k++;
        }
        uint min = lefread_vertices(array[k], index[k]);
        for (uint i = k + 1; i < size; i++)
        {
            if (index[i] < size_vertices(array[i]) && lefread_vertices(array[i], index[i]) < min)
            {
                min = lefread_vertices(array[i], index[i]);
            }
        }

        // On avance l'index de chaque liste contenant la valeur min
        for (uint i = 0; i < size; i++)
        {
            if (index[i] < size_vertices(array[i]) && lefread_vertices(array[i], index[i]) == min)
            {
                index[i]++;
                j++;
            }
        }

        // On insère la valeur dans out
        rigins_vertices(min, out);
    }
}

/* Insére un nouveau sommet dans une liste */
void insert_vertices(p_vertices p1, uint q)
{
    if (isempty_vertices(p1) || rigread_vertices(p1, 0) < q)
    {
        rigins_vertices(q, p1);
    }
    else if (rigread_vertices(p1, 0) == q)
    {
        return;
    }
    else
    {
        uint r = rigpull_vertices(p1);
        insert_vertices(p1, q);
        rigins_vertices(r, p1);
    }
}

/* Teste si deux verticess triées s'intersectent */
bool intersec_vertices(p_vertices p1, p_vertices p2)
{
    if (p1 == p2)
    {
        // Si ce sont les deux mêmes listes
        return true;
    }
    uint n1 = size_vertices(p1);
    uint n2 = size_vertices(p2);
    uint i = 0; // indice dans p1
    uint j = 0; // indice dans p2
    while (i < n1 && j < n2)
    {
        if (lefread_vertices(p1, i) == lefread_vertices(p2, j))
        {
            return true;
        }
        else if (lefread_vertices(p1, i) < lefread_vertices(p2, j))
        {
            i++;
        }
        else
        {
            j++;
        }
    }
    return false;
}
