/***********************************************/
/***********************************************/
/** Implémentation des piles par les tableaux **/
/***********************************************/
/***********************************************/

#ifndef STACK_H_
#define STACK_H_

#include <stdbool.h>
#include <stdarg.h>
#include "type_basic.h"

/***********************/
/* Déclaration du type */
/***********************/

typedef struct
{
    uint size_array; // La taille totale du tableau alloué en mémoire
    uint size_stack; // Le nombre d'éléments dans la pile représentée
    void **array;    // Un pointeur vers le tableau contenant des pointeurs vers les valeurs de la pile représentée
} stack;

typedef stack *p_stack; // on manipule des pointeurs sur des piles

/*************************/
/* Fonctions auxiliaires */
/*************************/
void grow_stack(p_stack);
// Double la taille du tableau utilisé dans la représentation

void shrink_stack(p_stack);
// Divise par deux la taille du tableau utilisé dans la représentation
// (on supposera que moins de la moitié des cases sont utilisées pour stocker des valeurs)

/************************/
/* Fonctions primitives */
/************************/

/* Création */
p_stack create_stack(void);

/* Suppression */
void delete_stack(p_stack);

/* Test du vide */
bool isempty_stack(p_stack);

/* Taille */
uint size_stack(p_stack p);

/* Lecture */
void *read_stack(p_stack p, uint i);
// Lit la ième valeur dans la pile
// Les valeurs sont numérotées en partant de zéro et la valeur d'indice 0 est en bas de la pile.

/*  Dépiler */
void *pop(p_stack p);

/* Empiler */
void push(void *val, p_stack p);

/* Tri */
void quick_sort_stack(p_stack p, int (*)(void *, void *));

#endif // STACK_H_
