#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "type_stack.h"
#include "macros_alloc.h"

/*************************/
/* Fonctions auxiliaires */
/*************************/

void grow_stack(p_stack p)
{
    CHECK_NULL(2, p, "The stack", p->array, "The array used to implement the stack");
    p->size_array = p->size_array * 2;
    REALLOC(p->array, p->size_array);
}

void shrink_stack(p_stack p)
{
    CHECK_NULL(2, p, "The stack", p->array, "The array used to implement the stack");
    if (p->size_array == 1)
    {
        return;
    }
    p->size_array = p->size_array / 2;
    REALLOC(p->array, p->size_array);
}

/************************/
/* Fonctions primitives */
/************************/

/* Création */
p_stack create_stack(void)
{
    p_stack new;
    MALLOC(new, 1);
    MALLOC(new->array, 1);
    new->size_array = 1;
    new->size_stack = 0;
    return new;
}

/* Suppression */
void delete_stack(p_stack p)
{
    CHECK_NULL(1, p, "The stack");
    free(p->array);
    free(p);
}

/* Test du vide */
bool isempty_stack(p_stack p)
{
    CHECK_NULL(2, p, "The stack", p->array, "The array used to implement the stack");
    return p->size_stack == 0;
}

/* Taille*/
uint size_stack(p_stack p)
{
    CHECK_NULL(2, p, "The stack", p->array, "The array used to implement the stack");
    return p->size_stack;
}

/* Lecture */
void *read_stack(p_stack p, uint i)
{
    CHECK_NULL(2, p, "The stack", p->array, "The array used to implement the stack");
    if (i < 0 || p->size_stack <= i)
    {
        fprintf(stderr, "Error, tried to read a non-valid index insider a stack");
        exit(EXIT_FAILURE);
    }
    return p->array[i];
}

/*  Dépiler */
void *pop(p_stack p)
{
    CHECK_NULL(2, p, "The stack", p->array, "The array used to implement the stack");
    if (p->size_stack == 0)
    {
        printf("Warning: popped an empty stack\n");
    }
    if (p->size_stack != 0)
    {
        p->size_stack--;
    }
    if (p->size_array >= 2 && p->size_stack <= p->size_array / 4)
    {
        shrink_stack(p);
    }
    return p->array[p->size_stack];
}

/* Empiler */
void push(void *val, p_stack p)
{
    CHECK_NULL(2, p, "The stack", p->array, "The array used to implement the stack");
    if (p->size_array == p->size_stack)
    {
        grow_stack(p);
    }
    p->array[p->size_stack] = val;
    p->size_stack++;
}

/* Tri */

// Tri entre les indices i et j-1 (inclus)
static void aux_quick_sort_stack(p_stack p, uint i, uint j, int (*comp)(void *, void *))
{
    if (i >= j - 1)
    { // Il y a moins d'un élément
        return;
    }
    else
    {
        void *pivot = p->array[i];
        uint l = j;

        for (uint k = j - 1; i < k; k--)
        {
            // If current element is greater than the pivot
            if ((*comp)(pivot, p->array[k]) == 1)
            {
                l--;
                void *temp = p->array[l];
                p->array[l] = p->array[k];
                p->array[k] = temp;
            }
        }
        void *temp = p->array[l - 1];
        p->array[l - 1] = p->array[i];
        p->array[i] = temp;
        aux_quick_sort_stack(p, i, l - 1, comp);
        aux_quick_sort_stack(p, l + 1, j, comp);
    }
}

void quick_sort_stack(p_stack p, int (*comp)(void *, void *))
{
    aux_quick_sort_stack(p, 0, size_stack(p), comp);
}
