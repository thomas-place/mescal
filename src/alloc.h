/**
 * @file alloc.h
 * @brief Macros et fonctions d'allocation mémoire.
 *
 * @details Les macros MALLOC(), CALLOC() et REALLOC() définies dans ce fichier
 * permettent d'allouer de la mémoire dynamiquement, en vérifiant que
 * l'allocation a réussi. En cas d'échec, elles affichent un message d'erreur
 * sur la sortie erreur, en indiquant la fonction, le fichier et la ligne où
 * l'allocation a échoué, et quittent le programme.
 *
 * La fonction multiple_free() permet de libérer plusieurs pointeurs en
 * un seul appel (mais attention, aucun de ces pointeurs ne doit être `NULL`, et
 * tous doivent avoir été alloués dynamiquement avant cette libération).
 *
 * La macro CHECK_NULL() permet de vérifier qu'un ou plusieurs pointeurs ne sont
 * pas `NULL`.
 *
 * La fonction check_null() n'est pas destinée à être utilisée directement, mais
 * est utilisée par la macro CHECK_NULL().
 */

#ifndef ALLOC_H_
#define ALLOC_H_

#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Macro pour allouer de la mémoire en utilisant la fonction malloc().
 * @param p Pointeur à allouer.
 * @param num_objects Nombre d'objets à allouer.
 *
 * @details
 * Alloue de la mémoire permettant de stocker `num_objects`, chacun de type
 * `*p`.
 *
 * **Exemple d'utilisation :**
 * ```
 * backmove *p;
 * MALLOC(p, 10); // Alloue un tableau de 10 backmove.
 * ```
 *
 * Si l'allocation échoue, affiche un message d'erreur et quitte le programme.
 *
 * @attention
 * Le pointeur `p` doit avoir été déclaré, mais ne pas pointer vers une zone
 * déjà allouée dynamiquement (sous peine de créer une fuite mémoire).
 * Si `p` pointe déjà vers une zone allouée dynamiquement, il faut utiliser
 * REALLOC.
 *
 * @see REALLOC()
 */

#define MALLOC(p, num_objects)                                       \
  do {                                                               \
    DEBUG("Allocating  %lu objects.", (unsigned long)(num_objects)); \
    void *tmp = malloc(sizeof *(p) * (num_objects));                 \
    if (!tmp)                                                        \
    FATAL("Malloc error.");                                          \
    (p) = tmp;                                                       \
  } while (0)

/**
 * @brief Macro pour allouer de la mémoire en utilisant la fonction calloc().
 * @param p Pointeur à allouer.
 * @param num_objects Nombre d'objets à allouer.
 *
 * @details
 * Alloue de la mémoire initialisée à 0, permettant de stocker `num_objects`,
 * chacun de type `*p`.
 *
 * **Exemple d'utilisation :**
 * ```
 * backmove *p;
 * CALLOC(p, 10); // Alloue un tableau de 10 backmove, initialisé à 0.
 * ```
 *
 * Si l'allocation échoue, affiche un message d'erreur et quitte le programme.
 *
 * @attention
 * Le pointeur `p` doit avoir été déclaré, mais ne pas pointer vers une zone
 * déjà allouée dynamiquement (sous peine de créer une fuite mémoire).
 * Si `p` pointe déjà vers une zone allouée dynamiquement, il faut utiliser
 * REALLOC.
 *
 * @sa REALLOC()
 */

#define CALLOC(p, num_objects)                                        \
  do {                                                                \
    DEBUG("Callocating  %lu objects.", (unsigned long)(num_objects)); \
    void *tmp = calloc((num_objects), sizeof *(p));                   \
    if (!tmp)                                                         \
    FATAL("Calloc error.");                                           \
    (p) = tmp;                                                        \
  }                                                                   \
  while (0)

/**
 * @brief Macro pour réallouer de la mémoire en utilisant la fonction realloc().
 * @param p Pointeur à allouer.
 * @param num_objects Nouveau nombre d'objets à allouer.
 *
 * Ré-alloue la mémoire pointée par un pointeur p, dont la valeur doit avoir été
 * retournée précédemment par une fonction d'allocation dynamique (malloc,
 * calloc, realloc ou une des macros de ce fichier).
 *
 * Si l'allocation échoue, affiche un message d'erreur et quitte le programme.
 *
 * **Exemple d'utilisation :**
 * ```
 * backmove *p;
 * MALLOC(p, 10); // Alloue un tableau de 10 backmove.
 * // ...
 * REALLOC(p, 20); // Ré-alloue un tableau de 20 backmove en copiant le tableau
 *                 // précédent en début ce nouveay tableau, et affecte à p
 *                 // l'adresse de la première case de ce tableau.
 * ```
 *
 * Si l'allocation échoue, affiche un message d'erreur et quitte le programme.
 *
 * @attention
 * Le pointeur `p` doit avoir été déclaré et pointer vers une zone
 * déjà allouée dynamiquement.
 *
 * @sa MALLOC(), CALLOC()
 */

#define REALLOC(p, num_objects)                                        \
  do {                                                                 \
    DEBUG("Reallocating  %lu objects.", (unsigned long)(num_objects)); \
    void *tmp = realloc((p), sizeof *(p) * (num_objects));             \
    if (!tmp) {                                                        \
      FATAL("Realloc error.");                                         \
    }                                                                  \
    (p) = tmp;                                                         \
  } while (0)


/**
 * @brief Macro permettant de tester si un ou plusieurs pointeurs sont NULL.
 * @param n Nombre de pointeurs à tester.
 * @param ... Pointeurs à tester, en alternance avec des chaînes de caractères.
 *
 * Il doit y avoir un pointeur à tester pour chaque chaîne de caractères, et
 * inversement.
 *
 * La chaîne de caractères qui suit chaque pointeur  permet de personnaliser le
 * message d'erreur.
 *
 * @details
 * **Exemple d'utilisation :**
 * ```
 * CHECK_NULL(3, p1, "Name1", p2, "Name2", p3, "Name3");
 * ```
 * Ici, 3 est le nombre de pointeurs à vérifier, et `p1`, `p2` et `p3` sont les
 * pointeurs. Si `p1` et `p3` sont NULL, la macro affichera un message d'erreur
 * de la forme :
 ***
    [CRITICAL] Name1 is NULL!\n
    [CRITICAL] Name3 is NULL!
 ***
 * @sa check_null()
 */

#define CHECK_NULL(n, ...)         \
  check_null                       \
    (__func__, __FILE__, __LINE__, \
    n __VA_OPT__( , ) __VA_ARGS__)

/**
 * @brief Ne pas utiliser cette fonction directement, mais à la place la macro
 * ::CHECK_NULL(), qui indique dans le message la position de l'erreur dans le
 * source.
 *
 * @sa CHECK_NULL()
 */

void check_null(const char *function, char *file, int line, int n, ...);

/**
 * @brief Fonction pour libérer de la mémoire en utilisant la fonction free().
 *
 * Appelle free sur un nombre variable d'arguments.
 * Le dernier argument doit être NULL.
 *
 * Exemple d'utilisation :
 * ```
 * int *p1, *p2, *p3;
 *
 * MALLOC(p1, 10); // Alloue un tableau de 10 int.
 * MALLOC(p2, 10);
 * MALLOC(p3, 10);
 * // ...
 * multiple_free(p1, p2, p3, NULL);
 * ```
 *
 * @attention Cette fonction est dangereuse et il faut savoir ce que vous faites
 * si vous l'utilisez ! Les pointeurs à libérer **ne doivent pas** être NULL.
 * Dans l'exemple précédent, si `p2` est NULL, on ne libère pas `p3` et cela
 * peut créer une fuite mémoire.
 */

void multiple_free(void *p, ...);

#endif // ALLOC_H_
