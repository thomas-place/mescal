/**
 * @file alloc.c
 * @brief Fonctions d'allocation mémoire.
 */

#include "macros_alloc.h"

void multiple_free(void *p, ...) {
  va_list ap;
  va_start(ap, p);
  while (p != NULL) {
    free(p);
    p = va_arg(ap, void *);
  }
  va_end(ap);
}

void check_null(const char *function, char *file, int line, int n, ...) {
  va_list ap;
  char *s;

  va_start(ap, n);
  for (int i = 0; i < n; i++) {
    void *p = va_arg(ap, void *);
    s = va_arg(ap, char *);
    if (p == NULL)
      PRINT_ERROR(KO, "CRITICAL", function, file, line, "%s is NULL.", s);
  }
}
