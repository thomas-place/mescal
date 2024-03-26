/**********************************************/
/* Calcul et affichage des relations de Green */
/**********************************************/

#ifndef GREEN_H
#define GREEN_H

#include <stdbool.h>
#include "tools.h"
#include "monoid.h"
#include "graphs_tarjan.h"
#include <math.h>

typedef struct
{
    // Partitions selon les quatres relations de Green
    p_parti JCL, LCL, RCL, HCL;

    // Tableau de Booléens pour marquer les éléments réguliers du monoide
    bool* regular_array;
    // Liste des éléments réguliers du monoide
    p_vertices regular_list;

    // Tableau de Booléens pour marquer les H-classes qui sont des groupes
    bool* group_array;
    // Liste des numéros de H-classes qui sont des groupes
    p_vertices group_list;

} green;

typedef green* p_green;

typedef struct
{
    // Le morphisme original
    p_cayley original;

    // Les relations de Green du morphisme original
    p_green originalrels;

    // La taille du sous-ensemble
    uint size;

    // The neutral element
    uint neut;

    // Par défaut, les autres champs sont NULL (cas où le sous-monoide est le monoide entier)

    // Tableau de taille size, pour tout élément s du sous-monoide, associe l'élément correspondant dans le monoide d'origine
    uint* sub_to_mono;

    // Tableau de la taille du monoide d'origine, à tout élément associe l'élément correspondant dans le sous-monoide (si il existe, non significatif sinon)
    bool* mono_in_sub;
    uint* mono_to_sub;

    // Sauvegardes des éléments idempotents
    p_vertices idem_list;
    bool* idem_array;

    // Les relations de Green du sous-monoide (numérotés selon l'ordre donné sub_to_mono)
    p_green rels;

} green_sub;

typedef green_sub* p_green_sub;

/********************************/
/* Calcul à partir d'un monoide */
/********************************/

// Calcule les relations L, J et R à partir du monoide
void jlr_green_compute(p_cayley, p_green);

// Calcule les relations L, J et R à partir dans un sous-monoide
// Les autres champs doivent déjà avoir été calculés (à utiliser
// si il n'y a pas de meilleure solution, quadratique)
void jlr_green_compute_sub(p_cayley, p_green_sub);

// Calcule la relation J à partir de L et R
// Attention, cette approche ne donne pas un classement par ordre topologique des J-classes (pas assez d'info dans L et R)
void j_green_compute(p_green);

// Calcule la relation H à partir de J, L et R (qui doivent déjà avoir été calculées)
void h_green_compute(p_green);

// Calcule les informations les Groupes dans G
// Calcule les information sur les élément réguliers
// (J, L, R et H doivent avoir été déjà calculées)
// On doit passer la liste des idempotents en entrée
void gr_green_compute(p_vertices idem_list, p_green G);

// Calcul complet des relations de Green
p_green cayley_green_compute(p_cayley);

void delete_green(p_green);


// Teste si le neutre a un antécédent non-vide
// (revient à teste si il existe une lettre J-équivalente à 1)
bool cayley_neutstrict(p_cayley, p_green);

/*************************/
/* Fonctions d'affichage */
/*************************/

// Affichage des informations sur les relations de Green
void print_infos_green(p_green, FILE* out);

// Affichage du diagramme boite à oeufs (©TM JEP)
void print_jclass_green(p_cayley, p_green, uint, uint, FILE* out);

// Affichage complet des informations sur les relations de Green d'un monoide
void print_full_green(p_cayley, p_green, bool, FILE* out);

void view_morphism(cayley* cayley, green* G);

/**********************************/
/* Manipulation des sous-monoides */
/**********************************/

// Initialisation d'un p_green_sub à partir d'un morphisme
// À la création le sous-monoide est égal au monoide original
p_green_sub init_submono(p_cayley, p_green);

// Suppression d'un sous-monoide
void delete_green_sub(p_green_sub);

// Pour une liste d'éléments du monoide d'origine, retourne la liste obtenue en ne gardant que ceux présents dans le sous-monoide
p_vertices restrict_list_to_submono(p_green_sub, p_vertices);

// Idempotence
bool is_idem_submono(p_green_sub, FILE* out);

// Commutativité
bool is_comm_submono(p_green_sub, FILE* out);

// Affichage du diagramme boite à oeufs (©TM JEP) pour les sous-monoïdes
void print_jclass_green_sub(p_green_sub S, uint jclass, uint padding, FILE* out);

// Affichage complet des informations sur les relations de Green d'un sous-monoide
void print_full_green_sub(p_green_sub, bool title, FILE* out);

// Affichage des idempotents dans un sous-monoide
void submono_print_idems(p_green_sub, FILE*);

// Affichage de l'ordre dans un sous-monoide
void submono_print_order(p_green_sub, FILE*);

#endif
