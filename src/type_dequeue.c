#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "type_dequeue.h"
#include "macros_alloc.h"

/*************************/
/* Fonctions auxiliaires */
/*************************/

static void grow_dequeue(p_dequeue p)
{
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
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
static void reverse_array(void **array, uint l, uint r)
{
    uint i, j;
    for (i = l, j = r; i < j; i++, j--)
    {
        void *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

static void shrink_dequeue(p_dequeue p)
{
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    if (p->size_array == 1)
    {
        return;
    }

    uint size = size_dequeue(p);

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
p_dequeue create_dequeue(void)
{
    p_dequeue new;
    MALLOC(new, 1);
    MALLOC(new->array, 1);
    new->size_array = 1;
    new->left = 0;
    new->right = 0;
    new->empty = true;
    return new;
}

/* Suppression */
void delete_dequeue(p_dequeue p)
{
    if (p == NULL)
    {
        return;
    }
    free(p->array);
    free(p);
}

/* Test du vide */
bool isempty_dequeue(p_dequeue p)
{
    return p->empty;
}

/* Vidage d'une deq*/
void makeempty_dequeue(p_dequeue p)
{
    p->left = 0;
    p->right = 0;
    p->empty = true;
}

/* Taille */
uint size_dequeue(p_dequeue p)
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
void *leftread(p_dequeue p, uint i)
{
    if (size_dequeue(p) - 1 < i)
    {
        fprintf(stderr, "Error, the dequeue is not large enough %d, %d\n", size_dequeue(p), i);
        exit(EXIT_FAILURE);
    }
    return p->array[(p->left + i) % p->size_array];
}
void *rightread(p_dequeue p, uint i)
{
    if (size_dequeue(p) - 1 < i)
    {
        fprintf(stderr, "Error, the dequeue is not large enough %d, %d\n", size_dequeue(p), i);
        exit(EXIT_FAILURE);
    }
    if (p->right > i)
    {
        return p->array[p->right - i - 1];
    }
    else
    {
        return p->array[p->size_array - (i + 1)];
    }
}

// int *leftread_ref(p_dequeue p, int i)
// {
//     if (getsize_dequeue(p) - 1 < i)
//     {
//         fprintf(stderr, "Error, the dequeue is not large enough %d, %d\n", getsize_dequeue(p), i);
//         exit(EXIT_FAILURE);
//     }
//     return (p->array + (p->left + i) % p->size_array);
// }
// int *rightread_ref(p_dequeue p, int i)
// {
//     if (getsize_dequeue(p) - 1 < i)
//     {
//         fprintf(stderr, "Error, the dequeue is not large enough %d, %d\n", getsize_dequeue(p), i);
//         exit(EXIT_FAILURE);
//     }
//     if (p->right - i > 0)
//     {
//         return (p->array + p->right - i - 1);
//     }
//     else
//     {
//         return (p->array + p->size_array - (i + 1));
//     }
// }

/* Insérer */
void leftinsert(void *val, p_dequeue p)
{
    if (p->right <= p->left && !(p->empty))
    {
        grow_dequeue(p);
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

void rightinsert(void *val, p_dequeue p)
{
    if (p->right <= p->left && !(p->empty))
    {
        grow_dequeue(p);
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
void *leftpull(p_dequeue p)
{
    void *val = p->array[p->left];
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
    if (p->size_array >= 2 && size_dequeue(p) <= p->size_array / 4)
    {
        shrink_dequeue(p);
    }

    return val;
}
void *rightpull(p_dequeue p)
{
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
    if (p->size_array >= 2 && size_dequeue(p) <= p->size_array / 4)
    {
        shrink_dequeue(p);
    }
    return p->array[p->right];
}

// /* Création d'une copie avec décalage */
// void copy_dequeue_right(p_dequeue tocopy, int lag, p_dequeue in)
// {
//     int n = getsize_dequeue(tocopy);
//     for (int i = 0; i < n; i++)
//     {
//         rightinsert(lag + leftread(tocopy, i), in);
//     }
// }

// /* Fusionne deux deq triées dans la première */
// void merge_sorted_dequeues(p_dequeue p1, p_dequeue p2)
// {
//     int n1 = getsize_dequeue(p1);
//     int n2 = getsize_dequeue(p2);
//     int tab[n1 + n2];
//     int i = 0; // indice dans p2
//     int j = 0; // indice dans tab
//     while (!isempty_dequeue(p1) || i < n2)
//     {
//         if (!isempty_dequeue(p1) && i < n2 && leftread(p1, 0) == leftread(p2, i))
//         {
//             tab[j] = leftpull(p1);
//             i++;
//             j++;
//         }
//         else if (!isempty_dequeue(p1) && (i >= n2 || leftread(p1, 0) < leftread(p2, i)))
//         {
//             tab[j] = leftpull(p1);
//             j++;
//         }
//         else
//         {
//             tab[j] = leftread(p2, i);
//             i++;
//             j++;
//         }
//     }
//     for (int k = 0; k < j; k++)
//     {
//         rightinsert(tab[k], p1);
//     }
// }

// /* Teste si deux dequeues triées s'intersectent */
// bool intersec_sorted_dequeues(p_dequeue p1, p_dequeue p2)
// {
//     int n1 = getsize_dequeue(p1);
//     int n2 = getsize_dequeue(p2);
//     int i = 0; // indice dans p1
//     int j = 0; // indice dans p2
//     while (i < n1 && j < n2)
//     {
//         if (leftread(p1, i) == leftread(p2, j))
//         {
//             return true;
//         }
//         else if (leftread(p1, i) < leftread(p2, j))
//         {
//             i++;
//         }
//         else
//         {
//             j++;
//         }
//     }
//     return false;
// }

// /* Teste si un élément apparient à une dequeue*/
// bool memb_dequeue(int e, p_dequeue p)
// {
//     for (int i = 0; i < getsize_dequeue(p); i++)
//     {
//         if (e == leftread(p, i))
//         {
//             return true;
//         }
//     }
//     return false;
// }

// /* Affichage */
// void print_dequeue(p_dequeue p)
// {
//     printf("Tuyau:");
//     if (isempty_dequeue(p))
//     {
//         printf(" vide\n");
//         return;
//     }
//     int i = p->left;
//     if (i < p->right)
//     {
//         while (i < p->right)
//         {
//             printf("  %d", p->array[i]);
//             i++;
//         }
//     }
//     else
//     {
//         while (i < p->size_array)
//         {
//             printf("  %d", p->array[i]);
//             i++;
//         }
//         i = 0;
//         while (i < p->right)
//         {
//             printf("  %d", p->array[i]);
//             i++;
//         }
//     }
//     printf("\n");
// }
