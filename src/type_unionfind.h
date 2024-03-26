/***************/
/* Union- Find */
/***************/

#ifndef UNIONFIND_H
#define UNIONFIND_H

#include "stdbool.h"
#include "stdlib.h"
#include "type_basic.h"
#include "macros_alloc.h"
#include "type_vertices.h"

/*******************/
/* Type UNION-FIND */
/*******************/

typedef struct
{
    uint size_tab; // Taille des tableaux
    uint size_set; // Taille de l'ensemble partitionné
    uint size_par; // Taille de la partition
    uint *parent;  // Tableau de la relation parent
    uint *rank;    // Tableau des rangs (significatif seulement pour les racines)
    uint *sizec;   // Tableau des tailles des classes (significatif seulement pour les racines)
} ufind;

typedef ufind *p_ufind;

// Crée un union-find de l'ensemble des éléments de 0 à size-1
// Initialement, la partition est composée de singletons
p_ufind
create_ufind(uint size);

// Suppression
void delete_ufind(p_ufind uf);

// Récupération d'information
uint sizeset_ufind(p_ufind uf);

uint sizepar_ufind(p_ufind uf);

// Agrandit la partition en ajoutant un élément
void makeset_ufind(p_ufind uf);

// Recherche
uint find_ufind(uint i, p_ufind uf);

// Récupération de la taille de la classe
uint sizeclass_ufind(uint i, p_ufind uf);

// Union
void union_ufind(uint i, uint j, p_ufind uf);

// Affichage
void print_ufind(p_ufind uf);

#endif
