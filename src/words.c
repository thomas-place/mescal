#include "words.h"

uint length_letter_utf8(letter l) {
    if (l.num >= 0) {
        return 1 + get_uint_length(l.num);
    }
    return 1;
}

uint fprint_letter_utf8(letter l, FILE* out) {
    switch (l.lab) {
    case ' ':
        fprintf(out, "␠");
        break;
    case '\n':
        fprintf(out, "⏎");
        break;
    default:
        fprintf(out, "%c", l.lab);
        break;
    }
    if (l.num >= 0) {
        return 1 + fprint_subsc_utf8(l.num, out);
    }
    return 1;
}

int sprint_letter_utf8(letter l, char* out) {
    int n = 0;
    switch (l.lab) {
    case ' ':
        n += sprintf(out, "␠");
        break;
    case '\n':
        n += sprintf(out, "⏎");
        break;
    default:
        n += sprintf(out, "%c", l.lab);
        break;
    }
    if (l.num >= 0) {
        return n + sprint_subsc_utf8(l.num, out + n);
    }
    return n;
}


#include "words.h"

static void fprint_letter_gviz_rec(short n, FILE* out) {
    if (n > 0) {
        fprint_letter_gviz_rec(n / 10, out);
        fprintf(out, "%d", n % 10);
    }
}

void fprint_letter_gviz(letter l, FILE* out, bool inv) {
    switch (l.lab) {
    case ' ':
        fprintf(out, "␠");
        break;
    case '\n':
        fprintf(out, "⏎");
        break;
    default:
        fprintf(out, "%c", l.lab);
        break;
    }
    if (l.num >= 0) {
        fprintf(out, "<SUB>");
        if (l.num == 0) {
            fprintf(out, "0");
        }
        else {
            fprint_letter_gviz_rec(l.num, out);
        }
        fprintf(out, "</SUB>");
    }
    if (inv) {
        fprintf(out, "<SUP>-1</SUP>");
    }
}

void fprint_letter_latex(letter l, FILE* out, bool inv) {
    switch (l.lab) {
    case ' ':
        fprintf(out, "␠");
        break;
    case '\n':
        fprintf(out, "⏎");
        break;
    default:
        fprintf(out, "%c", l.lab);
        break;
    }
    if (l.num >= 0) {
        fprintf(out, "_{%d}", l.num);
    }
    if (inv) {
        fprintf(out, "^{-1}");
    }
}


int compare_letters(const void* p1, const void* p2) {
    const letter* l1 = (const letter*)p1;
    const letter* l2 = (const letter*)p2;
    if (l1->lab < l2->lab) {
        return -1;
    }
    if (l1->lab > l2->lab) {
        return 1;
    }

    if (l1->num < l2->num && l2->num > -1) {
        return -1;
    }
    if (l1->num > l2->num && l1->num > -1) {
        return 1;
    }

    return 0;
}


letter* duplicate_alphabet(const letter* alph, uint size) {
    letter* new;
    MALLOC(new, size);
    for (uint i = 0; i < size; i++) {
        new[i] = alph[i];
    }
    return new;
}



// Double la taille du tableau utilisé dans la liste.
static void grow_word(word* p) {
    CHECK_NULL(2, p, "The list of word", p->array, "The array in the list of word");
    uint n = p->size_array;
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
static void reverse_array_word(letter* array, uint l, uint r) {
    uint i, j;
    for (i = l, j = r; i < j; i++, j--) {
        letter temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Divise par deux la taille du tableau utilisé dans la représentation
// (on supposera que seulement la moitié des cases sont utilisées dans
// la représentation).
static void shrink_word(word* p) {
    CHECK_NULL(2, p, "The list of word", p->array, "The array in the list of word");
    if (p->size_array == 1) {
        return;
    }

    uint size = size_word(p);

    // Rotation du tableau pour mettre la valeur gauche du tuyau
    // dans la case 0

    if (p->left != 0) {
        reverse_array_word(p->array, 0, p->left - 1);
        reverse_array_word(p->array, p->left, p->size_array - 1);
        reverse_array_word(p->array, 0, p->size_array - 1);
    }

    // Mise à jour des informations
    p->size_array = p->size_array / 2;
    p->left = 0;
    if (size <= p->size_array) {
        p->right = size;
    }
    else {
        p->right = p->size_array;
    }

    // Réallocation

    REALLOC(p->array, p->size_array);
}

/* Création */
word* create_empty_word(void) {
    word* new;
    MALLOC(new, 1);
    MALLOC(new->array, 1);
    new->size_array = 1;
    new->left = 0;
    new->right = 0;
    new->empty = true;
    return new;
}

/* Suppression */
void delete_word(word* p) {
    if (p == NULL) {
        return;
    }
    free(p->array);
    free(p);
}

/* Test du vide */
bool isempty_word(const word* p) { return p->empty; }

/* Taille */
uint size_word(const word* p) {
    if (p->empty) {
        return 0;
    }
    else if (p->left < p->right) {
        return p->right - p->left;
    }
    else {
        return (p->size_array - p->left) + p->right;
    }
}

/* Lecture */
letter lefread_word(const word* p, uint i) {
    CHECK_NULL(2, p, "The list of word", p->array, "The array in the list of word");
    if (size_word(p) - 1 < i) {
        fprintf(stderr, "Error, the list of word is not large enough.\nSize: %d.\nIndex read: %d\n", size_word(p), i);
        exit(EXIT_FAILURE);
    }
    return p->array[(p->left + i) % p->size_array];
}
letter rigread_word(const word* p, uint i) {
    CHECK_NULL(2, p, "The list of word", p->array, "The array in the list of word");
    if (size_word(p) - 1 < i) {
        fprintf(stderr, "Error, the list of word is not large enough.\nSize: %d.\nIndex read: %d\n", size_word(p), i);
        exit(EXIT_FAILURE);
    }
    if (p->right > i) {
        // printf("Read there 1:%d\n", p->right - i - 1);
        return p->array[p->right - i - 1];
    }
    else {
        // printf("Read there 12:%d\n", p->size_array - (i + 1));
        return p->array[p->size_array - (i + 1)];
    }
}

/* Insérer */
void lefcon_word(letter val, word* p) {
    CHECK_NULL(2, p, "The list of word", p->array, "The array in the list of word");
    if (p->right <= p->left && !(p->empty)) {
        grow_word(p);
    }
    if (p->left == 0) {
        p->left = p->size_array - 1;
    }
    else {
        p->left--;
    }
    p->array[p->left] = val;
    p->empty = false;
}

void rigcon_word(letter val, word* p) {
    CHECK_NULL(2, p, "The list of word", p->array, "The array in the list of word");
    if (p->right <= p->left && !(p->empty)) {
        grow_word(p);
    }
    p->array[p->right] = val;
    p->empty = false;
    if (p->right == p->size_array - 1) {
        p->right = 0;
    }
    else {
        p->right++;
    }
}

/* Retirer */
letter lefpull_word(word* p) {
    CHECK_NULL(2, p, "The list of word", p->array, "The array in the list of word");
    letter val = p->array[p->left];
    if (p->left == p->size_array - 1) {
        p->left = 0;
    }
    else {
        p->left++;
    }
    if (p->left == p->right) {
        p->empty = true;
    }
    if (p->size_array >= 2 && size_word(p) <= p->size_array / 4) {
        shrink_word(p);
    }

    return val;
}
letter rigpull_word(word* p) {
    CHECK_NULL(2, p, "The list of word", p->array, "The array in the list of word");
    if (p->right == 0) {
        p->right = p->size_array - 1;
    }
    else {
        p->right--;
    }
    if (p->left == p->right) {
        p->empty = true;
    }
    if (p->size_array >= 2 && size_word(p) <= p->size_array / 4) {
        shrink_word(p);
    }
    return p->array[p->right];
}

/* Création d'une copie avec décalage */
void concatenate_word(word* l, const word* r) {
    uint n = size_word(r);
    for (uint i = 0; i < n; i++) {
        rigcon_word(lefread_word(r, i), l);
    }
}

void display_word(const word* w, FILE* out) {

    uint len = size_word(w);
    if (len == 0) {
        printf("ε");
    }
    else {
        uint n = 1;
        fprint_letter_utf8(lefread_word(w, 0), out);

        for (uint i = 1; i < len; i++) {
            if (lefread_word(w, i).lab != lefread_word(w, i - 1).lab || lefread_word(w, i).num != lefread_word(w, i - 1).num) {
                if (n > 1) {
                    fprint_power_utf8(n, out);
                }

                fprint_letter_utf8(lefread_word(w, i), out);
                n = 1;
            }
            else {
                n++;
            }
        }
        if (n > 1) {
            fprint_power_utf8(n, out);
        }
    }
}

letter* get_alphabet_word(const word* w, uint* l) {

    uint len = size_word(w);
    if (len == 0) {
        *l = 0;
        return NULL;
    }

    letter* ret;
    MALLOC(ret, len);
    for (uint i = 0; i < len; i++) {
        ret[i] = lefread_word(w, i);
    }
    qsort(ret, len, sizeof(letter), compare_letters);

    uint i = 0;
    uint j = 1;
    while (j < len) {
        while (j < len && compare_letters(&ret[i], &ret[j]) == 0) {
            j++;
        }
        if (j < len) {
            i++;
            ret[i] = ret[j];
            j++;
        }
    }
    *l = i + 1;
    REALLOC(ret, *l);
    return ret;
}
