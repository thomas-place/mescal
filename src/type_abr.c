#include "type_abr.h"

/****************************************/
/* Version générique: pointeur sur void */
/****************************************/

/* La fonction de comparaison pour les entiers */
int avlcomp_int(void *x, void *y)
{
    if (*(int *)x < *(int *)y)
    {
        return 1;
    }
    else if (*(int *)x == *(int *)y)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/**************************************/
/* Fonctions auxiliaires et rotations */
/**************************************/

static int get_height_avl(p_avlnode p)
{
    if (p == NULL)
    {
        return -1;
    }
    else
    {
        return p->height;
    }
}

// Répare la hauteur si elle n'est fausse que à la racine
static void fix_height(p_avlnode p)
{
    p->height = max(get_height_avl(p->left), get_height_avl(p->right)) + 1;
}

// p should not be NULL
static int get_balance_avl(p_avlnode p)
{
    return get_height_avl(p->right) - get_height_avl(p->left);
}

static p_avlnode rotate_left(p_avlnode p)
{
    p_avlnode rightson = p->right;
    if (rightson == NULL)
    {
        printf("Warning, can't rotate left if there is no right son.\n");
        return NULL;
    }
    p->right = rightson->left;
    fix_height(p);
    rightson->left = p;
    fix_height(rightson);
    return rightson;
}

static p_avlnode rotate_right(p_avlnode p)
{
    p_avlnode leftson = p->left;
    if (leftson == NULL)
    {
        printf("Warning, can't rotate right if there is no left son.\n");
        return NULL;
    }
    p->left = leftson->right;
    fix_height(p);
    leftson->right = p;
    fix_height(leftson);
    return leftson;
}

static p_avlnode rotate_left_right(p_avlnode p)
{
    if (p->left == NULL)
    {
        printf("Warning, can't rotate left-right if there is no left son.\n");
        return NULL;
    }
    p->left = rotate_left(p->left);
    return rotate_right(p);
}

static p_avlnode rotate_right_left(p_avlnode p)
{
    if (p->right == NULL)
    {
        printf("Warning, can't rotate right-left if there is no right son.\n");
        return NULL;
    }
    p->right = rotate_right(p->right);
    return rotate_left(p);
}

// Répare l'équilibre à la racine
static p_avlnode repare_balance_avl(p_avlnode p)
{
    if (p == NULL)
    {
        return NULL;
    }
    int b = get_balance_avl(p);
    if (b == 2)
    {
        int bd = get_balance_avl(p->right);
        if (bd >= 0)
        {
            return rotate_left(p);
        }
        else
        {
            return rotate_right_left(p);
        }
    }
    else if (b == -2)
    {
        int bg = get_balance_avl(p->left);
        if (bg <= 0)
        {
            return rotate_right(p);
        }
        else
        {
            return rotate_left_right(p);
        }
    }
    return p;
}

/**************/
/* Opérations */
/**************/

p_avlnode avl_search(void *val, p_avlnode p, int (*f)(void *, void *))
{
    if (p == NULL)
    {
        return NULL;
    }
    else if ((*f)(val, p->value) == 0)
    {
        // Si la valeur est déjà présente à la racine
        return p;
    }
    else if ((*f)(val, p->value) >= 1)
    {
        // Si la valeur est strictement inférieure à celle de la racine
        return avl_search(val, p->left, f);
    }
    else
    {
        // Si la valeur est strictement supérieure à celle de la racine
        return avl_search(val, p->right, f);
    }
}

p_avlnode avl_insert(void *val, p_avlnode p, int (*f)(void *, void *))
{
    if (p == NULL)
    {
        p_avlnode new;
        MALLOC(new, 1);
        new->height = 0;
        new->left = NULL;
        new->right = NULL;
        new->value = val;
        return new;
    }
    else if ((*f)(val, p->value) == 0)
    {
        // Si la valeur est déjà présente à la racine
        return p;
    }
    else if ((*f)(val, p->value) >= 1)
    {
        // Si la valeur est strictement inférieure à celle de la racine
        p->left = avl_insert(val, p->left, f);
        return repare_balance_avl(p);
    }
    else
    {
        // Si la valeur est strictement supérieure à celle de la racine
        p->right = avl_insert(val, p->right, f);
        return repare_balance_avl(p);
    }
}

int getsize_avl(p_avlnode p)
{
    if (p == NULL)
    {
        return 0;
    }
    else
    {
        return getsize_avl(p->left) + getsize_avl(p->right) + 1;
    }
}

static int avl_to_array(p_avlnode tree, int *tab, int num)
{
    if (tree == NULL)
    {
        return num;
    }
    else
    {
        int newnum = avl_to_array(tree->left, tab, num);
        printf("%d\n", *(int *)tree->value);
        tab[newnum] = *(int *)tree->value;
        return avl_to_array(tree->right, tab, newnum + 1);
    }
}

void avl_sort(int *tab, int size)
{
    p_avlnode tree = NULL;
    for (int i = 0; i < size; i++)
    {
        // printf("%d\n", tab[i]);
        tree = avl_insert(&tab[i], tree, &avlcomp_int);
    }
    int temp[size];
    avl_to_array(tree, temp, 0);
    for (int i = 0; i < size; i++)
    {
        tab[i] = temp[i];
    }
}

void avl_free_strong(p_avlnode p, void (*f)(void *))
{
    if (p == NULL)
    {
        return;
    }
    else
    {
        avl_free_strong(p->left, f);
        avl_free_strong(p->right, f);
        (*f)(p->value);
        free(p);
    }
}

/********************************/
/* Version légère pour les uint */
/********************************/

/**************************************/
/* Fonctions auxiliaires et rotations */
/**************************************/

static int get_height_uint_avl(p_uint_avlnode p)
{
    if (p == NULL)
    {
        return -1;
    }
    else
    {
        return p->height;
    }
}

// Répare la hauteur si elle n'est fausse que à la racine
static void uint_fix_height(p_uint_avlnode p)
{
    p->height = max(get_height_uint_avl(p->left), get_height_uint_avl(p->right)) + 1;
}

// p should not be NULL
static int get_balance_uint_avl(p_uint_avlnode p)
{
    return get_height_uint_avl(p->right) - get_height_uint_avl(p->left);
}

static p_uint_avlnode uint_rotate_left(p_uint_avlnode p)
{
    p_uint_avlnode rightson = p->right;
    if (rightson == NULL)
    {
        printf("Warning, can't rotate left if there is no right son.\n");
        return NULL;
    }
    p->right = rightson->left;
    uint_fix_height(p);
    rightson->left = p;
    uint_fix_height(rightson);
    return rightson;
}

static p_uint_avlnode uint_rotate_right(p_uint_avlnode p)
{
    p_uint_avlnode leftson = p->left;
    if (leftson == NULL)
    {
        printf("Warning, can't rotate right if there is no left son.\n");
        return NULL;
    }
    p->left = leftson->right;
    uint_fix_height(p);
    leftson->right = p;
    uint_fix_height(leftson);
    return leftson;
}

static p_uint_avlnode uint_rotate_left_right(p_uint_avlnode p)
{
    if (p->left == NULL)
    {
        printf("Warning, can't rotate left-right if there is no left son.\n");
        return NULL;
    }
    p->left = uint_rotate_left(p->left);
    return uint_rotate_right(p);
}

static p_uint_avlnode uint_rotate_right_left(p_uint_avlnode p)
{
    if (p->right == NULL)
    {
        printf("Warning, can't rotate right-left if there is no right son.\n");
        return NULL;
    }
    p->right = uint_rotate_right(p->right);
    return uint_rotate_left(p);
}

// Répare l'équilibre à la racine
static p_uint_avlnode repare_balance_uint_avl(p_uint_avlnode p)
{
    if (p == NULL)
    {
        return NULL;
    }
    int b = get_balance_uint_avl(p);
    if (b == 2)
    {
        int bd = get_balance_uint_avl(p->right);
        if (bd >= 0)
        {
            return uint_rotate_left(p);
        }
        else
        {
            return uint_rotate_right_left(p);
        }
    }
    else if (b == -2)
    {
        int bg = get_balance_uint_avl(p->left);
        if (bg <= 0)
        {
            return uint_rotate_right(p);
        }
        else
        {
            return uint_rotate_left_right(p);
        }
    }
    return p;
}

/**************/
/* Opérations */
/**************/

p_uint_avlnode uint_avl_search(uint val, p_uint_avlnode p)
{
    if (p == NULL)
    {
        return NULL;
    }
    else if (val == p->value)
    {
        // Si la valeur est déjà présente à la racine
        return p;
    }
    else if (val < p->value)
    {
        // Si la valeur est strictement inférieure à celle de la racine
        return uint_avl_search(val, p->left);
    }
    else
    {
        // Si la valeur est strictement supérieure à celle de la racine
        return uint_avl_search(val, p->right);
    }
}

p_uint_avlnode uint_avl_insert(uint val, p_uint_avlnode p)
{
    if (p == NULL)
    {
        p_uint_avlnode new;
        MALLOC(new, 1);
        new->height = 0;
        new->left = NULL;
        new->right = NULL;
        new->value = val;
        return new;
    }
    else if (val == p->value)
    {
        // Si la valeur est déjà présente à la racine
        return p;
    }
    else if (val < p->value)
    {
        // Si la valeur est strictement inférieure à celle de la racine
        p->left = uint_avl_insert(val, p->left);
        return repare_balance_uint_avl(p);
    }
    else
    {
        // Si la valeur est strictement supérieure à celle de la racine
        p->right = uint_avl_insert(val, p->right);
        return repare_balance_uint_avl(p);
    }
}

int getsize_uint_avl(p_uint_avlnode p)
{
    if (p == NULL)
    {
        return 0;
    }
    else
    {
        return getsize_uint_avl(p->left) + getsize_uint_avl(p->right) + 1;
    }
}

// Récupération de la liste des valeurs dans un p_vertices
void uint_avl_to_vertices(p_uint_avlnode p, p_vertices v)
{
    if (p == NULL)
    {
        return;
    }
    else
    {
        uint_avl_to_vertices(p->left, v);
        rigins_vertices(p->value, v);
        uint_avl_to_vertices(p->right, v);
        free(p);
    }
}

// static int uint_avl_to_array(p_uint_avlnode tree, int *tab, int num)
// {
//     if (tree == NULL)
//     {
//         return num;
//     }
//     else
//     {
//         int newnum = avl_to_array(tree->left, tab, num);
//         printf("%d\n", *(int *)tree->value);
//         tab[newnum] = *(int *)tree->value;
//         return avl_to_array(tree->right, tab, newnum + 1);
//     }
// }
