#include "type_boolarray.h"


barray* create_barray(uint size) {
    uint size_malloc;
    if (size % 8 == 0) {
        size_malloc = size / 8;
    }
    else {
        size_malloc = size / 8 + 1;
    }

    barray* new;
    MALLOC(new, 1);
    new->size_array = size_malloc;
    new->size = size;
    CALLOC(new->array, size_malloc);
    return new;
}


uint getsize_barray(barray* a) {
    return a->size;
}

void delete_barray(barray* a) {
    if (a) {
        free(a->array);
        free(a);
    }
}

void settrue_barray(barray* a, uint i) {
    uint q = i / 8;
    uint r = i % 8;

    uchar mask = 128 >> r;
    a->array[q] = a->array[q] | mask;
}

void setfalse_barray(barray* a, uint i) {
    uint q = i / 8;
    uint r = i % 8;

    uchar mask = 128 >> r;
    a->array[q] = a->array[q] & ~mask;
}

bool getval_barray(barray* a, uint i) {
    uint q = i / 8;
    uint r = i % 8;

    uchar mask = 128 >> r;
    return a->array[q] & mask;
}

barray* or_barray(barray* a1, barray* a2) {
    if (!a1 || !a2 || a1->size != a2->size || a1->size_array != a2->size_array) {
        return NULL;
    }
    barray* ret = create_barray(a1->size);
    for (uint i = 0; i < ret->size_array;i++) {
        ret->array[i] = a1->array[i] | a2->array[i];
    }
    return ret;
}

void or_barray_mod(barray* a1, barray* a2) {
    if (!a1 || !a2 || a1->size != a2->size || a1->size_array != a2->size_array) {
        return;
    }
    for (uint i = 0; i < a1->size_array;i++) {
        a1->array[i] = a1->array[i] | a2->array[i];
    }
}
barray* and_barray(barray* a1, barray* a2) {
    if (!a1 || !a2 || a1->size != a2->size || a1->size_array != a2->size_array) {
        return NULL;
    }
    barray* ret = create_barray(a1->size);
    for (uint i = 0; i < ret->size_array;i++) {
        ret->array[i] = a1->array[i] & a2->array[i];
    }
    return ret;
}

void and_barray_mod(barray* a1, barray* a2) {
    if (!a1 || !a2 || a1->size != a2->size || a1->size_array != a2->size_array) {
        return;
    }
    for (uint i = 0; i < a1->size_array;i++) {
        a1->array[i] = a1->array[i] & a2->array[i];
    }
}

barray* copy_barray(barray* a) {
    if (!a) {
        return NULL;
    }
    barray* ret = create_barray(a->size);
    for (uint i = 0; i < ret->size_array;i++) {
        ret->array[i] = a->array[i];
    }
    return ret;
}
