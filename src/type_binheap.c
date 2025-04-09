#include "type_binheap.h"

/*************************/
/* Fonctions auxiliaires */
/*************************/

/* La fonction de comparaison pour les tas d'entiers */
int fcmp_int(void* x, void* y) {
    return *(int*)x - *(int*)y;
}

int fcmp_uint(void* x, void* y) {
    if (*(uint*)x < *(uint*)y) {
        return -1;
    }
    else if (*(uint*)x == *(uint*)y) {
        return 0;
    }
    else {
        return 1;
    }
}

/* Fonctions de navigation dans un arbre représenté par un tableau */
static uint left_binheap(uint i) {
    return 2 * i + 1;
}

static uint right_binheap(uint i) {
    return 2 * i + 2;
}

static uint parent_binheap(uint i) {
    return (i - 1) / 2;
}

static bool isvalid_binheap(binheap* p, uint i) {
    return i < p->size_heap;
}

/* Modification de la taille du tableau */
static void grow_binheap(binheap* p) {
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    p->size_array = p->size_array * 2;
    REALLOC(p->array, p->size_array);
}

static void shrink_binheap(binheap* p) {
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    if (p->size_array == 1) {
        return;
    }
    p->size_array = p->size_array / 2;
    REALLOC(p->array, p->size_array);
}

/************************/
/* Fonctions primitives */
/************************/

/* Création d'un tas vide */
binheap* create_binheap(int (*fc) (void*, void*)) {
    binheap* new;
    MALLOC(new, 1);
    MALLOC(new->array, 1);
    new->size_array = 1;
    new->size_heap = 0;
    new->fc = fc;
    return new;
}

/* Suppression */
void delete_binheap(binheap* p) {
    CHECK_NULL(1, p, "The binary heap");
    free(p->array);
    free(p);
}

/* Test du vide */
bool isempty_binheap(binheap* p) {
    return p->size_heap == 0;
}

/* Récupération de la taille */
int getsize_binheap(binheap* p) {
    return p->size_heap;
}

/* Insertion d'une valeur */
static void void_swap(void** a, void** b) {
    CHECK_NULL(2, a, "Le premier entier à échanger", b, "Le second entier à échanger");
    void* tmp = *a;
    *a = *b;
    *b = tmp;
}

static void percudequeuebinheap(binheap* p, uint i) {
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    while (i != 0 && p->fc(p->array[i], p->array[parent_binheap(i)]) < 0) {
        void_swap(p->array + i, p->array + parent_binheap(i));
        i = parent_binheap(i);
    }
}

void push_binheap(binheap* p, void* val) {
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    if (p->size_array == p->size_heap) {
        grow_binheap(p);
    }
    p->array[p->size_heap] = val;
    percudequeuebinheap(p, p->size_heap);
    p->size_heap++;
}

/* Récupération du minimum sans le retirer */
void* peekmin_binheap(binheap* p) {
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    return p->array[0];
}

/* Récupération du minimum en le retirant */
static void percdown_binheap(binheap* p, uint i) {
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    uint left = left_binheap(i);
    uint right = right_binheap(i);

    while ((isvalid_binheap(p, left) && p->fc(p->array[left], p->array[i]) < 0) || (isvalid_binheap(p, right) && p->fc(p->array[right], p->array[i]) < 0)) {
        if (!isvalid_binheap(p, right) || p->fc(p->array[left], p->array[right]) < 0) {
            void_swap(p->array + i, p->array + left);
            i = left;
            left = left_binheap(i);
            right = right_binheap(i);
        }
        else {
            void_swap(p->array + i, p->array + right);
            i = right;
            left = left_binheap(i);
            right = right_binheap(i);
        }
    }
}

void* popmin_binheap(binheap* p) {
    CHECK_NULL(2, p, "The binary heap", p->array, "The array used to implement the binary heap");
    void* res = p->array[0];
    p->size_heap--;
    p->array[0] = p->array[p->size_heap];
    percdown_binheap(p, 0);
    if (p->size_array > 1 && p->size_heap <= p->size_array / 4) {
        shrink_binheap(p);
    }
    return res;
}
