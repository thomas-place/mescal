#include "type_binheap.h"

/*************************/
/* Fonctions auxiliaires */
/*************************/

/* La fonction de comparaison pour les tas d'entiers */
bool fcmp_int(void *x, void *y)
{
    return *(int *)x < *(int *)y;
}

bool fcmp_uint(void *x, void *y)
{
    return *(uint *)x < *(uint *)y;
}

/* Fonctions de navigation dans un arbre représenté par un tableau */
uint left_binheap(uint i)
{
    return 2 * i + 1;
}
uint right_binheap(uint i)
{
    return 2 * i + 2;
}
uint parent_binheap(uint i)
{
    return (i - 1) / 2;
}
bool isvalid_binheap(p_binheap p, uint i)
{
    return i < p->size_heap;
}

/* Modification de la taille du tableau */
void grow_binheap(p_binheap p)
{
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    p->size_array = p->size_array * 2;
    REALLOC(p->array, p->size_array);
}
void shrink_binheap(p_binheap p)
{
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
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

/* Création d'un tas vide */
p_binheap create_binheap(bool (*fc)(void *, void *))
{
    p_binheap new;
    MALLOC(new, 1);
    MALLOC(new->array, 1);
    new->size_array = 1;
    new->size_heap = 0;
    new->fc = fc;
    return new;
}

/* Suppression */
void delete_binheap(p_binheap p)
{
    CHECK_NULL(1, p, "The binary heap");
    free(p->array);
    free(p);
}

/* Test du vide */
bool isempty_binheap(p_binheap p)
{
    return p->size_heap == 0;
}

/* Récupération de la taille */
int getsize_binheap(p_binheap p)
{
    return p->size_heap;
}

/* Insertion d'une valeur */
static void void_swap(void **a, void **b)
{
    CHECK_NULL(2, a, "Le premier entier à échanger", b, "Le second entier à échanger");
    void *tmp = *a;
    *a = *b;
    *b = tmp;
}

static void percup_binheap(p_binheap p, uint i)
{
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    while (i != 0 && (*p->fc)(p->array[i], p->array[parent_binheap(i)]))
    {
        void_swap(p->array + i, p->array + parent_binheap(i));
        i = parent_binheap(i);
    }
}

void push_binheap(p_binheap p, void *val)
{
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    if (p->size_array == p->size_heap)
    {
        grow_binheap(p);
    }
    p->array[p->size_heap] = val;
    percup_binheap(p, p->size_heap);
    p->size_heap++;
}

/* Récupération du minimum sans le retirer */
void *peekmin_binheap(p_binheap p)
{
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    return p->array[0];
}

/* Récupération du minimum en le retirant */
static void percdown_binheap(p_binheap p, uint i)
{
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    uint left = left_binheap(i);
    uint right = right_binheap(i);

    while ((isvalid_binheap(p, left) && (*p->fc)(p->array[left], p->array[i])) || (isvalid_binheap(p, right) && (*p->fc)(p->array[right], p->array[i])))
    {
        if (!isvalid_binheap(p, right) || (*p->fc)(p->array[left], p->array[right]))
        {
            void_swap(p->array + i, p->array + left);
            i = left;
            left = left_binheap(i);
            right = right_binheap(i);
        }
        else
        {
            void_swap(p->array + i, p->array + right);
            i = right;
            left = left_binheap(i);
            right = right_binheap(i);
        }
    }
}

void *popmin_binheap(p_binheap p)
{
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    void *res = p->array[0];
    p->size_heap--;
    p->array[0] = p->array[p->size_heap];
    percdown_binheap(p, 0);
    if (p->size_array > 1 && p->size_heap <= p->size_array / 4)
    {
        shrink_binheap(p);
    }
    return res;
}
