#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "type_dequeue_gen.h"
#include "alloc.h"

/*************************/
/* Fonctions auxiliaires */
/*************************/

static void grow_dequeue_gen(dequeue_gen* p)
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
static void reverse_array(void** array, uint l, uint r)
{
    uint i, j;
    for (i = l, j = r; i < j; i++, j--)
    {
        void* temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

static void shrink_dequeue_gen(dequeue_gen* p)
{
    CHECK_NULL(2, p, "The dequeue", p->array, "The array in the dequeue");
    if (p->size_array == 1)
    {
        return;
    }

    uint size = size_dequeue_gen(p);

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
dequeue_gen* create_dequeue_gen(void)
{
    dequeue_gen* new;
    MALLOC(new, 1);
    MALLOC(new->array, 1);
    new->size_array = 1;
    new->left = 0;
    new->right = 0;
    new->empty = true;
    return new;
}

/* Suppression */
void delete_dequeue_gen(dequeue_gen* p)
{
    if (p == NULL)
    {
        return;
    }
    free(p->array);
    free(p);
}

/* Test du vide */
bool isempty_dequeue_gen(dequeue_gen* p)
{
    return p->empty;
}

/* Vidage d'une deq*/
void makeempty_dequeue_gen(dequeue_gen* p)
{
    p->left = 0;
    p->right = 0;
    p->empty = true;
}

/* Taille */
uint size_dequeue_gen(dequeue_gen* p)
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
void* lefread_dequeue_gen(dequeue_gen* p, uint i)
{
    if (size_dequeue_gen(p) - 1 < i)
    {
        fprintf(stderr, "Error, the queue is not large enough %d, %d\n", size_dequeue_gen(p), i);
        exit(EXIT_FAILURE);
    }
    return p->array[(p->left + i) % p->size_array];
}
void* rigread_dequeue_gen(dequeue_gen* p, uint i)
{
    if (size_dequeue_gen(p) - 1 < i)
    {
        fprintf(stderr, "Error, the queue is not large enough %d, %d\n", size_dequeue_gen(p), i);
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



/* Insérer */
void lefins_dequeue_gen(void* val, dequeue_gen* p)
{
    if (p->right <= p->left && !(p->empty))
    {
        grow_dequeue_gen(p);
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

void rigins_dequeue_gen(void* val, dequeue_gen* p)
{
    if (p->right <= p->left && !(p->empty))
    {
        grow_dequeue_gen(p);
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
void* lefpull_dequeue_gen(dequeue_gen* p)
{
    void* val = p->array[p->left];
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
    if (p->size_array >= 2 && size_dequeue_gen(p) <= p->size_array / 4)
    {
        shrink_dequeue_gen(p);
    }

    return val;
}
void* rigpull_dequeue_gen(dequeue_gen* p)
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
    if (p->size_array >= 2 && size_dequeue_gen(p) <= p->size_array / 4)
    {
        shrink_dequeue_gen(p);
    }
    return p->array[p->right];
}

