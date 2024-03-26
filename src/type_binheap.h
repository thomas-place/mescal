/*************************************************************************/
/*************************************************************************/
/** Files de priorités: implémentation d'une tas binaire par un tableau **/
/*************************************************************************/
/*************************************************************************/

#ifndef BINHEAP_H_
#define BINHEAP_H_

#include <stdio.h>
#include <stdbool.h>
#include "macros_alloc.h"
#include "type_basic.h"

// On travaille avec des tas génériques: le type des valeurs n'est pas fixé
// Pour ce faire, les éléments du tas seront des pointeurs sur void (void*).
// Le tas ne contiendra donc pas directement les valeurs mais des pointeurs
// sur celles-ci.
// Puisque le type des valeurs n'est pas fixé: la fonction de comparaison qui
// sert à ordonner les éléments du tas est une paramètre de ce tas. On utilise
// pour cela des pointeurs de fonctions: un des champs du type sera un pointeur
// sur une fonction qui associe un Booléen à deux éléments de type void* (qui
// indique si le premier est "plus petit" que le deuxième).

typedef struct
{
    void **array;
    // Tableau stockant des pointeurs vers chaque valeur du tas
    uint size_array;
    // Taille totale du tableau
    uint size_heap;
    // Taille du tas (nombres de case utilisées dans le tableau)
    bool (*fc)(void *, void *);
    // Pointeur sur la fonction de comparaison utilisée par le tas
} binheap;

typedef binheap *p_binheap;

/*************************/
/* Fonctions auxiliaires */
/*************************/

/* La fonction de comparaison pour les tas d'entiers */
bool fcmp_int(void *x, void *y);
bool fcmp_uint(void *x, void *y);

/* Fonctions de navigation dans un arbre représenté par un tableau */
uint left_binheap(uint i);
// Retourne l'indice du fils gauche du noeud à l'indice i
uint right_binheap(uint i); // Fils droit
// Retourne l'indice du fils droit du noeud à l'indice i
uint parent_binheap(uint i); // Parent
// Retourne l'indice du parent du noeud à l'indice i
bool isvalid_binheap(p_binheap p, uint i);
// Teste si l'indice i correspond à un noeud est valide dans le tas p

/* Modification de la taille du tableau */
void grow_binheap(p_binheap);
// Double la taille du tableau utilisé dans la représentation
void shrink_binheap(p_binheap);
// Divise par deux la taille du tableau utilisé dans la représentation

/************************/
/* Fonctions primitives */
/************************/

/* Création d'un tas vide */
p_binheap create_binheap(bool (*)(void *, void *));
// Le paramètre est un pointeur de fonction
// Il faut lui donner l'adresse de la fonction de comparaison à utiliser
// Par exemple, pour créer un tas d'entier on appellera la primitive avec
// l'adresse de la fonction fcmp_int ci-dessus:
// p = create_binheap(&fcmp_int);

/* Suppression */
void delete_binheap(p_binheap);

/* Test du vide */
bool isempty_binheap(p_binheap);

/* Taille */
int getsize_binheap(p_binheap);

/* Insertion d'une valeur */
void push_binheap(p_binheap, void *);

/* Récupération du minimum sans le retirer */
void *peekmin_binheap(p_binheap);

/* Récupération du minimum en le retirant */
void *popmin_binheap(p_binheap);

#endif
