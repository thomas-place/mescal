#ifndef LANGUAGES_H_
#define LANGUAGES_H_

#include <stdlib.h>
#include "class_at.h"
#include "class_concath.h"
#include "class_group.h"
#include "monoid_ideals.h"
#include "monoid_green.h"
#include "monoid.h"
#include "nfa.h"
#include "nfa_determi.h"
#include "regexp.h"
#include "reg2aut.h"

/*****************************************/
/* Les objets manipulables dans le shell */
/*****************************************/

// Objet 1: le langage

// Un langage peut être spécifié soit par une REGEXP, soit par un NFA
typedef enum {
  SPE_REG,
  SPE_NFA,
} langtype;

typedef struct language
{
  char* name;
  langtype type;
  union {
    regexp* reg;
    int nfa;
  } spe;
  // regexp* regexp;
  int minauto;
  int syntmor;
} language;

// Objet 2: l'automate
typedef struct automata
{
  nfa* nfa; // Le NFA

} automata;


// Objet 3: le morphisme
typedef struct morphism
{
  /* Seul champ obligatoire */
  cayley* cayley; // Représentation par le graphe de cayley 

  /* Champs optionnel (NULL si pas utilisés) */
  green* green; // Relations de Green
  green_sub* mker; // MOD-kernel
  green_sub* gker; // GR-kernel
  orbits* ddorbs; // DD-orbites
  orbits* mporbs; // MOD⁺-orbites
  orbits* atorbs; // AT-orbites
  orbits* bpmorbs; // BPol(MOD)-orbites
  orbits* bpgorbs; // BPol(GR)-orbites
  orbits* bpddorbs; // BPol(DD)-orbites
  orbits* bpmporbs; // BPol(MOD⁺)-orbites
} morphism;



/***********************************/
/* Objet générique pour le tableau */
/***********************************/

typedef enum object_type
{
  LANGUAGE,
  AUTOMATA,
  MORPHISM
} object_type;

typedef struct object {
  char* name; // Nom de l'objet
  object_type type; // Type d'objet
  union {
    language* lan;
    automata* aut;
    morphism* mor;
  } theob;
  // void* theob; // L'objet lui-même
  int parent; // Indice d'un éventuel objet parent (-1 si il n'y en a pas)
} object;

// Le tableau (variables globale)
// extern language* languages[];
extern object* objects[];
extern int nb_objects;

/************************/
/* Création/Suppression */
/************************/

object* object_language_new_reg(char* name, regexp* regexp);
object* object_language_new_nfa(char* name, int i);
object* object_automata_new(char* name, nfa* A);
object* object_morphism_new(char* name, cayley* M);
void object_free(object* object);

/*************************************/
/* Récupération/insertion d'un objet */
/*************************************/

int object_get_from_name(char* name);
int object_delete_from_name(char* name);

int object_add_language_reg(char* name, regexp* regexp);
int object_add_language_nfa(char* name, int i);
int object_add_morphism(char* name, cayley* M);
int object_add_automata(char* name, nfa* A);

void object_sort_array(void);

/****************************/
/* Affichage d'informations */
/****************************/

void object_print_langs(void);

void object_print_autos(void);

void object_print_morps(void);

/************************************************/
/* Calculs d'informations sur un objet existant */
/************************************************/

// Calcul du DFA minimal d'un langage
int shell_compute_minimal(int i);

// Calcul du morphisme syntaxique d'un langage
int shell_compute_syntac(int i);

// Calcul de la table de multiplication d'un morphism
void shell_compute_mult(morphism*);

// Calcul des relations de green d'un morphisme
void shell_compute_green(morphism*);

// Calcul de l'ordre syntactique d'un morphisme
void shell_compute_order(morphism*);

// Calcul du MOD-kernel d'un morphisme
void shell_compute_mker(morphism*);

// Calcul du GR-kernel d'un morphisme
void shell_compute_gker(morphism*);

// Calcul des DD-orbites
void shell_compute_ddorbs(morphism*);

// Calcul des MOD⁺-orbites
void shell_compute_mporbs(morphism*);

// Calcul des AT-orbites
void shell_compute_atorbs(morphism*);

// Calcul des BPol(MOD)-orbites
void shell_compute_bpmorbs(morphism*);
p_orbits shell_get_bpmorbs(morphism* mor);

// Calcul des BPol(GR)-orbites
void shell_compute_bpgorbs(morphism*);

// Calcul des BPol(DD)-orbites
void shell_compute_bpddorbs(morphism*);
p_orbits shell_get_bpddorbs(morphism*);

// Calcul des BPol(MOD⁺)-orbites
void shell_compute_bpmporbs(morphism*);
p_orbits shell_get_bpmporbs(morphism* mor);

#endif // LANGUAGES_H_
