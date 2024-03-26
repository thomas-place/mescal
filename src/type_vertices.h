/**************************************************/
/*     Implémentation des ensembles de sommets    */
/* Chaque ensemble est représenté par une dequeue */
/**************************************************/

#ifndef VERTICES_H
#define VERTICES_H

#include <stdbool.h>
#include <stdio.h>
#include "type_basic.h"

/**********************/
/* Définition du type */
/**********************/

typedef struct
{
    uint* array;     // Tableau des valeurs
    uint size_array; // Taille du tableau des valeurs
    uint left;       // Indice de la valeur à gauche du deq (si non-vide).
    uint right;      // Indice qui suit celui de la valeur à droite du deq (si non-vide).
    bool empty;      // Booléen indiquant si le deq est vide.
} vertices;
// Dans la plupart des utilisations, les objets de type "vertices" sont supposés
// triés par ordre croissant et ne contenir qu'une seul occurence de chaque élément.
// Il y a des exceptions (par exemple quand on utilise ce type pour un DFS ou un BFS)

typedef vertices* p_vertices;

/************************/
/* Fonctions primitives */
/************************/

/* Création */
p_vertices create_vertices(void);

/* Suppression */
void delete_vertices(p_vertices);

/* Taille */
uint size_vertices(p_vertices p);

/* Test du vide */
bool isempty_vertices(p_vertices);

/* Vidage de la liste */
void makeempty_vertices(p_vertices);

/*  Insérer */
void lefins_vertices(uint, p_vertices);
void rigins_vertices(uint, p_vertices);

/* Lecture */
uint lefread_vertices(p_vertices, uint i);
// Lit la ième valeur en partant de la gauche (la première à gauche a pour indice 0)
uint rigread_vertices(p_vertices, uint i);
// Lit la ième valeur en partant de la droite (la première à droite a pour indice 0)

uint* lefread_vertices_ref(p_vertices, uint i);
// Retourne l'adresse de la ième valeur en partant de la gauche (la première à gauche a pour indice 0)
uint* rigread_vertices_ref(p_vertices, uint i);
// Retourne l'adresse de la ième valeur en partant de la droite (la première à droite a pour indice 0)

/* Retirer */
uint lefpull_vertices(p_vertices);
uint rigpull_vertices(p_vertices);

/* Copie les sommets de la seconde liste à droite de la première */
void copy_vertices_right(p_vertices incopy, p_vertices tocopy, uint lag);
// L'entier lag sert à décaler les valeurs copiées (on ajoute lag à chacune d'entre elles).

/* Teste si un élément apparient à une liste de sommets */
bool mem_vertices(uint e, p_vertices p);

/* Affichage d'une liste de sommets */
void print_vertices(p_vertices p);

/* Affichage d'un ensemble de lettres */
void print_vertices_alpha(p_vertices p, FILE* out);

/* Tri par ordre croissant et suppression des doublons */
void sort_vertices(p_vertices p);

/*****************************************************************/
/* Fonctions spécifiques aux ensembles triés par ordre croissant */
/*****************************************************************/

/* Appartenance d'un élément dans une liste triée */
bool mem_vertices_sorted(uint e, p_vertices p);

/* Fusionne deux listes de sommets dans la première */
void merge_sorted_vertices(p_vertices p1, p_vertices p2);

/* Calcule l'intersection de deux listes triées de sommets */
p_vertices make_inter_sorted_vertices(p_vertices p1, p_vertices p2);

/* Fusionne toutes les listes triées contenues dans un tableau dans la liste out (supposée vide) */
void merge_array_sorted_vertices(p_vertices out, p_vertices* array, uint size);

/* Insére un nouveau sommet dans une liste */
void insert_vertices(p_vertices p1, uint q);

/* Teste si deux listes de sommets s'intersectent */
bool intersec_vertices(p_vertices p1, p_vertices p2);

#endif // VERTICES_H_
