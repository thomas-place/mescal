/***************/
/* ABRs (AVLs) */
/***************/

#ifndef ABR_H
#define ABR_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "type_basic.h"
#include "type_vertices.h"
#include "macros_alloc.h"

/****************************************/
/* Version générique: pointeur sur void */
/****************************************/

struct avlnode
{
    int height;
    void *value;
    struct avlnode *left;
    struct avlnode *right;
};

typedef struct avlnode *p_avlnode;

// Fonction de comparaison pour les entiers
int avlcomp_int(void *x, void *y);

// Recherche
p_avlnode avl_search(void *val, p_avlnode p, int (*f)(void *, void *));

// Insertion
p_avlnode avl_insert(void *val, p_avlnode p, int (*f)(void *, void *));

// Calcul de la taille
int getsize_avl(p_avlnode p);

// Libération (la fonction f prise en paramêtre sert à libérer la valeur des sommets)
void avl_free_strong(p_avlnode p, void (*f)(void *));

// Fonction de tri (sert pour les tests)
void avl_sort(int *tab, int size);

/********************************/
/* Version légère pour les uint */
/********************************/

struct uint_avlnode
{
    int height;
    uint value;
    struct uint_avlnode *left;
    struct uint_avlnode *right;
};

typedef struct uint_avlnode *p_uint_avlnode;

// Recherche
p_uint_avlnode uint_avl_search(uint val, p_uint_avlnode p);

// Insertion
p_uint_avlnode uint_avl_insert(uint val, p_uint_avlnode p);

// Calcul de la taille
int getsize_uint_avl(p_uint_avlnode p);

// Récupération de la liste des valeurs dans un p_vertices
// L'arbre est libéré en même temps
void uint_avl_to_vertices(p_uint_avlnode p, p_vertices v);

#endif
