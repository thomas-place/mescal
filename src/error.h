/**
 * @file error.h
 * @brief Macros permettant d'afficher des messages d'erreur.
 *
 * @details Le fichier fournit les macros TRACE(), DEBUG(), INFO(), WARNING(),
 * ERROR(),
 * CRITICAL() et FATAL(), qui permettent d'afficher des messages d'erreur sur la
 * sortie d'erreur, et, pour la macro FATAL(), de terminer le programme. Les
 * messages d'erreur sont affichés avec le nom de la fonction, le nom du fichier
 * et le numéro de la ligne où l'appel à la macro est effectué.
 *
 * Les autres macros du fichier sont essentiellement destinées à implémenter les
 * macros TRACE(), DEBUG(), INFO(), WARNING(), ERROR(), CRITICAL() et FATAL().
 * Elles ne sont pas destinées à être utilisées directement par l'utilisateur
 * (mais rien ne l'empêche).
 *
 * Les macros TRACE(), DEBUG(), INFO(), WARNING(), ERROR(), CRITICAL() et
 * FATAL() prennent en argument un message à afficher, qui peut contenir des
 * spécificateurs de conversion comme dans la fonction printf. Elles peuvent
 * prendre des arguments supplémentaires, un par spécificateur de conversion.
 *
 * **Exemple d'utilisation :**
 * @code
 * #include "error.h"
 * ERROR("La variable colors vaut %d, elle devrait être supérieure à %d.",
 *        colors, MIN_COLORS);
 * @endcode
 *
 * @note Les messages d'un certain type sont affichés en fonction du niveau de
 * debug, fixé par la variable ::DEBUG_LEVEL. Par exemple, si le niveau de
 * debug est à ::ERROR, seules les macros ERROR(), CRITICAL() et FATAL()
 * afficheront leur message. Les autres macros n'afficheront rien. Par défaut,
 * le niveau de debug est ::INFO.\n\n
 * **Niveaux de debug :**\n
 * - ::OFF (seulement les messages FATAL sont affichés),
 * - ::CRITICAL (seulement les messages CRITICAL et FATAL),
 * - ::ERROR  (seulement les messages ERROR, CRITICAL et FATAL),
 * - ::WARNING (seulement les messages WARNING, ERROR, CRITICAL et FATAL),
 * - ::INFO (tous les messages sauf DEBUG et TRACE),
 * - ::DEBUG (tous les messages sauf TRACE),
 * - ::TRACE (tous les messages).
 *
 * @note Pour changer le niveau de debug, il faut définir la macro ::DEBUG_LEVEL
 * avant d'inclure error.h, typiquement à la compilation. Par exemple, pour
 * afficher tous les messages de debug, il faut définir ::DEBUG_LEVEL à ::TRACE.
 * Pour cela, on peut compiler le programme ainsi :
 * ```
 * make -B CFLAGS="-DDEBUG_LEVEL=TRACE"
 * ```
 *
 * @sa DEBUG_LEVEL et les macros TRACE(), DEBUG(), INFO(), WARNING(), ERROR(),
 * CRITICAL() et FATAL().
 */

#ifndef ERROR_H_
#define ERROR_H_


/** @brief Symbole ❗. */
#define ATTENTION "\xe2\x9d\x97"
/** @brief Symbole ✨. */
#define HINT "\xe2\x9c\xa8"
/** @brief Symbole ✅. */
#define OK "\xE2\x9C\x85"
/** @brief Symbole ❌. */
#define KO "\xE2\x9D\x8C"

// In X mode, all messages more severe than X are printed.
// clang-format off
enum debug_level {OFF,        CRITICAL, ERROR,     WARNING, INFO,    DEBUG, TRACE};
enum msg_level   {CRITICAL_L, ERROR_L,  WARNING_L, INFO_L,  DEBUG_L, TRACE_L};
// clang-format on

/**
 * @brief Niveau de debug, ::INFO par défaut.
 * @details
 * Par défaut, le niveau de debug est INFO.
 * Pour changer le niveau de debug, il faut définir la macro DEBUG_LEVEL
 * avant d'inclure error.h.
 *
 * Cela peut se faire au moment de la compilation :
 * ```
 * make -B CFLAGS="-DDEBUG_LEVEL=WARNING"
 * ```
 *
 * Par exemple, pour afficher tous les messages de debug, il faut définir
 * ::DEBUG_LEVEL à ::TRACE. Pour n'afficher que les messages de niveau FATAL, il
 * faut définir ::DEBUG_LEVEL à ::OFF.
 *
 * **Niveaux possibles :**
 * - ::OFF (seulement les messages FATAL sont affichés),
 * - ::CRITICAL (seulement les messages CRITICAL et FATAL),
 * - ::ERROR  (seulement les messages ERROR, CRITICAL et FATAL),
 * - ::WARNING (seulement les messages WARNING, ERROR, CRITICAL et FATAL),
 * - ::INFO (tous les messages sauf DEBUG et TRACE),
 * - ::DEBUG (tous les messages sauf TRACE),
 * - ::TRACE (tous les messages).
 *
 * @see enum debug_level
 */

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL INFO
#endif

// __VA_OPTS__ requires C2x. Clang issues spurious warning.

/** @brief Affiche un message d'erreur sur la sortie d'erreur.
 *
 * @details Cette macro est utilisée par les macros ::TRACE, ::DEBUG, ::INFO,
 * ::WARNING, ::ERROR, ::CRITICAL et ::FATAL.
 *
 * @param symbol Symbole à afficher (::ATTENTION, ::HINT, ::OK ou ::KO).
 * @param name Nom du message (comme "TRACE", "DEBUG", "INFO", "WARNING",
 * "ERROR", "CRITICAL" ou "FATAL").
 * @param function Nom de la fonction où le message est affiché.
 * @param file Nom du fichier où le message est affiché.
 * @param line Numéro de la ligne où le message est affiché.
 * @param msg Message à afficher. Ce message peut contenir les mêmes
 * spécificateurs de conversion que la fonction printf (comme `%d`, `%s`, etc.).
 * @param ... Arguments supplémentaires à afficher avec le message, un par
 * spécificateur de conversion.
 */

#define PRINT_ERROR(symbol, name, function, file, line, msg, ...) \
  do {                                                            \
    fprintf(stderr,                                               \
            "\n" symbol                                           \
            " ["                                                  \
            name                                                  \
            "]"                                                   \
            " - Function %s (%s:%d) -\n  "                        \
            msg "\n",                                             \
            function, file, line                                  \
            __VA_OPT__( , ) __VA_ARGS__);                         \
  } while (0)

/** @brief Affiche un message de niveau FATAL et termine le programme. */

#define FATAL(msg, ...)                                    \
  do {                                                     \
    PRINT_ERROR(KO, "FATAL", __func__, __FILE__, __LINE__, \
                msg __VA_OPT__( , ) __VA_ARGS__);          \
    exit(EXIT_FAILURE);                                    \
  } while (0)

#define PRINT_DEBUG(symbol, name, level, msg, ...)          \
  do {                                                      \
    if ((int)DEBUG_LEVEL > (int)level)                      \
    PRINT_ERROR(symbol, name, __func__, __FILE__, __LINE__, \
                msg __VA_OPT__( , ) __VA_ARGS__);           \
  } while (0)

/** @brief Affiche un message de niveau TRACE */
#define TRACE(msg, ...)             \
  PRINT_DEBUG(OK, "TRACE", TRACE_L, \
              msg, __VA_ARGS__)

/** @brief Affiche un message de niveau DEBUG */
#define DEBUG(msg, ...)               \
  PRINT_DEBUG(HINT, "DEBUG", DEBUG_L, \
              msg, __VA_ARGS__)

/** @brief Affiche un message de niveau INFO */
#define INFO(msg, ...)              \
  PRINT_DEBUG(HINT, "INFO", INFO_L, \
              msg, __VA_ARGS__)

/** @brief Affiche un message de niveau WARNING */
#define WARNING(msg, ...)                      \
  PRINT_DEBUG(ATTENTION, "WARNING", WARNING_L, \
              msg, __VA_ARGS__)

/** @brief Affiche un message de niveau ERROR */
#define ERROR(msg, ...)                    \
  PRINT_DEBUG(ATTENTION, "ERROR", ERROR_L, \
              msg, __VA_ARGS__)

/** @brief Affiche un message de niveau CRITICAL */
#define CRITICAL(msg, ...)                \
  PRINT_DEBUG(KO, "CRITICAL", CRITICAL_L, \
              msg, __VA_ARGS__)

#endif // ERROR_H_
