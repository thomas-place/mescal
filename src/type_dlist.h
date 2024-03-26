/***************************************************/
/* Listes doublement chaînées d'entiers non-signés */
/***************************************************/

#ifndef DLIST_H_
#define DLIST_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "macros_alloc.h"
#include "type_basic.h"

struct dcell
{

    // La valeur de la cellule (non-significatif si il s'agit d'une sentinelle).
    uint val;

    // Pointeur vers la cellule suivante
    // NULL si il n'y a pas de cellule suivante (sentinelle droite)
    struct dcell *next;

    // Pointeur vers la cellule précédente
    // NULL si il n'y a pas de cellule précédente (sentinelle gauche).
    struct dcell *previous;
};

typedef struct dcell *p_dcell;

typedef struct
{
    // la taille de la liste
    uint size;

    // Un pointeur vers la sentinelle gauche
    p_dcell lsent;

    // Un pointeur vers la sentinelle droite
    p_dcell rsent;

} dlist;

typedef dlist *p_dlist;

/* Création d'une liste */
p_dlist create_dlist(void);

/* Suppression d'une liste */
void delete_dlist(p_dlist);

/* Récupération de la valeur d'une cellule */
int getvalue_dlist(p_dlist, p_dcell);

/* Modification de la valeur d'une cellule */
void modvalue_dlist(p_dlist, p_dcell, int);

/* Insertion avant/après une cellue existante */
void insertnext_dlist(p_dlist, p_dcell, int);
void insertprevious_dlist(p_dlist, p_dcell, int);

/* Suppression d'une cellule */
void deletecell_dlist(p_dlist, p_dcell);

/* Fusion de deux listes (la second est concaténée à droite de la première) */
void concat_dlist(p_dlist, p_dlist);

#endif // DLIST_H_
