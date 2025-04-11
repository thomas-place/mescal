#ifndef ALLOC_H_
#define ALLOC_H_

/**
 * @file alloc.h
 * @brief Macros and functions to help memory allocation.
 *
 * @details The macros MALLOC(), CALLOC() and REALLOC() from this file make
 * it possible to allocate dynamic memory. If they fail, they print an error
 * message on the standard error, indicating the function, file and line
 * where the allocation failed, and they then quit the program.
 *
 * The function multiple_free() is used to free several pointers at once.
 *
 * @attention None of these pointers should be `NULL`, and all must have
 * been dynamically allocated before the call.
 *
 * The macro CHECK_NULL() checks whether one or several pointers are non-`NULL`.
 *
 * The function check_null() shouldn't be used directly, it is used by the
 * macro CHECK_NULL().
 */

#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Macro for allocating memory using the malloc() function.
 * @param p Pointer to be allocated.
 * @param num_objects Number of objects to allocate.
 *
 * @details
 * Allocates memory to store `num_objects`, each of type `*p`.
 *
 * **Example of use:**
 * ```
 * struct foo *p;
 * MALLOC(p, 10); // Allocate an array of 10 struct foo.
 * ```
 *
 * If allocation fails, displays an error message and quits the program.
 *
 * @attention The `p` pointer must have been declared, but must not point to a
 * zone that has already been dynamically allocated (otherwise a memory leak
 * will occur). If `p` already points to a dynamically allocated zone, use
 * REALLOC.
 *
 * @see REALLOC()
 */

#define MALLOC(p, num_objects)                                                 \
    do {                                                                       \
        DEBUG("Allocating  %lu objects.", (unsigned long)(num_objects));       \
        void *tmp = malloc(sizeof *(p) * (num_objects));                       \
        if (!tmp)                                                              \
            FATAL("Malloc error.");                                            \
        (p) = tmp;                                                             \
    } while (0)

/**
 * @brief Macro for allocating memory using the calloc() function.
 * @param p Pointer to be allocated.
 * @param num_objects Number of objects to allocate.
 *
 * @details
 * Allocates memory, initialized at 0, to store `num_objects`, * each of type
 * `*p`.
 *
 * **Example of use:**
 * ```
 * struct foo *p;
 * CALLOC(p, 10); // Allocates an array of 10 struct foo, initialized to 0.
 * ```
 *
 * If allocation fails, displays an error message and quits the program.
 *
 * @attention The  pointer `p` must have been declared, but must not point to an
 * already dynamically allocated area (otherwise a memory leak will occur). If
 * `p` already points to a dynamically allocated area, use REALLOC.
 *
 * @sa REALLOC().
 */
#define CALLOC(p, num_objects)                                                 \
    do {                                                                       \
        DEBUG("Callocating  %lu objects.", (unsigned long)(num_objects));      \
        void *tmp = calloc((num_objects), sizeof *(p));                        \
        if (!tmp)                                                              \
            FATAL("Calloc error.");                                            \
        (p) = tmp;                                                             \
    } while (0)

/**
 * @brief Macro for reallocating memory using the realloc() function.
 * @param p Pointer to be allocated.
 * @param num_objects New number of objects to allocate.
 *
 * Reallocates memory pointed to by a pointer `p`, whose value must have been
 * previously returned by a dynamic allocation function (`malloc`, `calloc`,
 * `realloc` or one of the macros in this file).
 *
 * If allocation fails, displays an error message and quits the program.
 *
 * **Example of use:**
 * ```
 * struct foo *p;
 * MALLOC(p, 10); // Allocate an array of 10 struct foo.
 * // ...
 * REALLOC(p, 20); // Re-allocate an array of 20 struct foo by copying the
 *                 // original array at the begining of the new array, and
 *                 // assign the address of the first cell in this array to `p`.
 * ```
 *
 * If allocation fails, displays an error message and quits the program.
 *
 * @attention
 * The `p` pointer must have been declared and point to an already
 * dynamically allocated zone.
 * @sa MALLOC(), CALLOC()
 */
#define REALLOC(p, num_objects)                                                \
    do {                                                                       \
        DEBUG("Reallocating  %lu objects.", (unsigned long)(num_objects));     \
        void *tmp = realloc((p), sizeof *(p) * (num_objects));                 \
        if (!tmp) {                                                            \
            FATAL("Realloc error.");                                           \
        }                                                                      \
        (p) = tmp;                                                             \
    } while (0)

/**
 * @brief Do not use this function directly; instead, use the macro
 * ::CHECK_NULL(), which includes the error's location in the source code
 * in the message.
 *
 * @sa CHECK_NULL()
 */
void check_null(const char *function, char *file, int line, int n, ...);

/**
 * @brief Macro to test if one or more pointers are NULL.
 * @param n Number of pointers to test.
 * @param ... Pointers to test, alternating with strings.
 *
 * There must be one pointer to test for each string, and vice versa.
 *
 * The string following each pointer allows customizing the error message.
 *
 * @details
 * **Example of use:**
 * ```
 * CHECK_NULL(3, p1, "Name1", p2, "Name2", p3, "Name3");
 * ```
 * Here, 3 is the number of pointers to check, and `p1`, `p2`, and `p3` are the
 * pointers. If `p1` and `p3` are NULL, the macro will display an error message
 * in the following form:
 ***
    ❌ <span style="color:#ff0000">[CRITICAL]</span> Name1 is NULL!\n
    ❌ <span style="color:#ff0000">[CRITICAL]</span> Name3 is NULL!
 ***
 * @sa check_null()
 */

#define CHECK_NULL(n, ...)                                                     \
    check_null(__func__, __FILE__, __LINE__, n __VA_OPT__(, ) __VA_ARGS__)

/**
 * @brief Function to free memory using the free() function.
 *
 * Calls free on a variable number of arguments.
 * The last argument must be `NULL`.
 *
 * Example of use:
 * ```
 * int *p1, *p2, *p3;
 *
 * MALLOC(p1, 10); // Allocates an array of 10 ints.
 * MALLOC(p2, 10);
 * MALLOC(p3, 10);
 * // ...
 * multiple_free(p1, p2, p3, NULL);
 * ```
 *
 * @attention This function is dangerous and should only be used if you know
 * what you are doing! The pointers to be freed **must not** be NULL.
 * In the example above, if `p2` is NULL, `p3` will not be freed, which
 * can cause a memory leak.
 */

void multiple_free(void *p, ...);

#endif // ALLOC_H_
