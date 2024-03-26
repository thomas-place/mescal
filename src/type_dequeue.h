/**********************************************/
/**********************************************/
/** Implémentation des tuyaux par un tableau **/
/**********************************************/
/**********************************************/

#ifndef DEQUEUE_H_
#define DEQUEUE_H_

#include <stdbool.h>
#include "type_basic.h"

/**********************/
/* Définition du type */
/**********************/

typedef struct
{
    void **array;    // Tableau des valeurs
    uint size_array; // Taille du tableau des valeurs
    uint left;       // Indice de la valeur à gauche du tuyau (si non-vide).
    uint right;      // Indice qui suit celui de la valeur à droite du tuyau (si non-vide).
    bool empty;      // Booléen indiquant si le tuyau est vide.
} dequeue;

typedef dequeue *p_dequeue; // on manipule des pointeurs sur des tuyaux

/************************/
/* Fonctions primitives */
/************************/

/* Création */
p_dequeue create_dequeue(void);

/* Suppression */
void delete_dequeue(p_dequeue);

/* Taille */
uint size_dequeue(p_dequeue p);

/* Test du vide */
bool isempty_dequeue(p_dequeue);

/* Vidage d'une deq*/
void makeempty_dequeue(p_dequeue);

/*  Insérer */
void leftinsert(void *, p_dequeue);
void rightinsert(void *, p_dequeue);

/* Lecture */
void *leftread(p_dequeue, uint i);
// Lit la ième valeur en partant de la gauche (la première à gauche a pour indice 0)
void *rightread(p_dequeue, uint i);
// Lit la ième valeur en partant de la droite (la première à droite a pour indice 0)

// int *leftread_ref(p_dequeue, int i);
// // Retourne l'adresse de la ième valeur en partant de la gauche (la première à gauche a pour indice 0)
// int *rightread_ref(p_dequeue, int i);
// // Retourne l'adresse de la ième valeur en partant de la droite (la première à droite a pour indice 0)

/* Retirer */
void *leftpull(p_dequeue);
void *rightpull(p_dequeue);

// /* Copie une deq à droite d'une autre deq avec un décalage de lag sur les valeurs */
// void copy_dequeue_right(p_dequeue tocopy, int lag, p_dequeue in);

// /* Fusionne deux deq triées dans la première */
// void merge_sorted_dequeues(p_dequeue p1, p_dequeue p2);

// /* Teste si deux dequeues triées s'intersectent */
// bool intersec_sorted_dequeues(p_dequeue p1, p_dequeue p2);

// /* Teste si un élément apparient à une dequeue*/
// bool memb_dequeue(int e, p_dequeue p);

// /* Affichage d'un tuyau */
// void print_dequeue(p_dequeue p);
// // Cette primitive est déjà écrite. Utilisez-la pour vos tests.

#endif // DEQUEUE_H_
