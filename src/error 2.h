/**
 * @file error.h
 * @brief Macros for displaying error messages.
 *
 * @details This file provides the macros TRACE(), DEBUG(), INFO(), WARNING(),
 * ERROR(), CRITICAL(), and FATAL(), which allow displaying error messages on
 * the error output, and, for the FATAL() macro, terminating the program. Error
 * messages are displayed with the function name, file name, and line number
 * where the macro call is made.
 *
 * The other macros in the file are primarily intended to implement the
 * TRACE(), DEBUG(), INFO(), WARNING(), ERROR(), CRITICAL(), and FATAL() macros.
 * They are not intended to be used directly by the user (but nothing prevents
 * it).
 *
 * The TRACE(), DEBUG(), INFO(), WARNING(), ERROR(), CRITICAL(), and FATAL()
 * macros take as an argument a message to display, which can contain
 * conversion specifiers as in the printf function. They can take additional
 * arguments, one per conversion specifier.
 *
 * **Example of use:**
 * @code
 * #include "error.h"
 * ERROR("The variable colors is %d, it should be greater than %d.",
 *        colors, MIN_COLORS);
 * @endcode
 *
 * @note Messages of a certain type are displayed depending on the debug level,
 * set by the ::DEBUG_LEVEL variable. For example, if the debug level is set to
 * ::ERROR, only the ERROR(), CRITICAL(), and FATAL() macros will display their
 * message. The other macros will display nothing. By default, the debug level
 * is ::INFO.\n\n
 * **Debug levels:**\n
 * - ::OFF (only FATAL messages are displayed),
 * - ::CRITICAL (only CRITICAL and FATAL messages),
 * - ::ERROR  (only ERROR, CRITICAL, and FATAL messages),
 * - ::WARNING (only WARNING, ERROR, CRITICAL, and FATAL messages),
 * - ::INFO (all messages except DEBUG and TRACE),
 * - ::DEBUG (all messages except TRACE),
 * - ::TRACE (all messages).
 *
 * @note To change the debug level, you must define the ::DEBUG_LEVEL macro
 * before including error.h, typically at compilation. For example, to display
 * all debug messages, you must set ::DEBUG_LEVEL to ::TRACE. To do this, you
 * can compile the program as follows:
 * ```
 * make -B CFLAGS="-DDEBUG_LEVEL=TRACE"
 * ```
 *
 * @sa DEBUG_LEVEL and the macros TRACE(), DEBUG(), INFO(), WARNING(), ERROR(),
 * CRITICAL(), and FATAL().
 */
#ifndef ERROR_H_
#define ERROR_H_

/** @brief Symbole ❗. */
#define ATTENTION "\xe2\x9d\x97"
/** @brief Symbole ✨. */
#define HINT      "\xe2\x9c\xa8"
/** @brief Symbole ✅. */
#define OK        "\xE2\x9C\x85"
/** @brief Symbole ❌. */
#define KO        "\xE2\x9D\x8C"

// In X mode, all messages more severe than X are printed.
// clang-format off
enum debug_level {OFF,        CRITICAL, ERROR,     WARNING, INFO,    DEBUG, TRACE};
enum msg_level   {CRITICAL_L, ERROR_L,  WARNING_L, INFO_L,  DEBUG_L, TRACE_L};
// clang-format on

/** @brief Debug level, ::INFO by default.
 * @details
 * By default, the debug level is INFO.
 * To change the debug level, you must define the DEBUG_LEVEL macro
 * before including error.h.
 *
 * This can be done at compilation:
 * ```
 * make -B CFLAGS="-DDEBUG_LEVEL=WARNING"
 * ```
 *
 * For example, to display all debug messages, you must set
 * ::DEBUG_LEVEL to ::TRACE. To display only FATAL-level messages,
 * you must set ::DEBUG_LEVEL to ::OFF.
 *
 * **Possible levels:**
 * - ::OFF (only FATAL messages are displayed),
 * - ::CRITICAL (only CRITICAL and FATAL messages),
 * - ::ERROR  (only ERROR, CRITICAL, and FATAL messages),
 * - ::WARNING (only WARNING, ERROR, CRITICAL, and FATAL messages),
 * - ::INFO (all messages except DEBUG and TRACE),
 * - ::DEBUG (all messages except TRACE),
 * - ::TRACE (all messages).
 *
 * @see enum debug_level
 */

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL INFO
#endif

// __VA_OPTS__ requires C2x. Clang issues spurious warning.

/** @brief Displays an error message on the error output.
 *
 * @details This macro is used by the macros ::TRACE, ::DEBUG, ::INFO,
 * ::WARNING, ::ERROR, ::CRITICAL, and ::FATAL.
 *
 * @param symbol Symbol to display (::ATTENTION, ::HINT, ::OK, or ::KO).
 * @param name Name of the message (such as "TRACE", "DEBUG", "INFO", "WARNING",
 * "ERROR", "CRITICAL", or "FATAL").
 * @param function Name of the function where the message is displayed.
 * @param file Name of the file where the message is displayed.
 * @param line Line number where the message is displayed.
 * @param msg Message to display. This message can contain the same
 * conversion specifiers as the printf function (such as `%d`, `%s`, etc.).
 * @param ... Additional arguments to display with the message, one per
 * conversion specifier.
 */

#define PRINT_ERROR(symbol, name, function, file, line, msg, ...)              \
    do {                                                                       \
        fprintf(stderr,                                                        \
                "\n" symbol " [" name "]"                                      \
                " - Function %s (%s:%d) -\n  " msg "\n",                       \
                function, file, line __VA_OPT__(, ) __VA_ARGS__);              \
    } while (0)

/** @brief Prints a FATAL message and end the program. */
#define FATAL(msg, ...)                                                        \
    do {                                                                       \
        PRINT_ERROR(KO, "FATAL", __func__, __FILE__, __LINE__,                 \
                    msg __VA_OPT__(, ) __VA_ARGS__);                           \
        exit(EXIT_FAILURE);                                                    \
    } while (0)

#define PRINT_DEBUG(symbol, name, level, msg, ...)                             \
    do {                                                                       \
        if ((int)DEBUG_LEVEL > (int)level)                                     \
            PRINT_ERROR(symbol, name, __func__, __FILE__, __LINE__,            \
                        msg __VA_OPT__(, ) __VA_ARGS__);                       \
    } while (0)

/** @brief Prints a TRACE message */
#define TRACE(msg, ...) PRINT_DEBUG(OK, "TRACE", TRACE_L, msg, __VA_ARGS__)

/** @brief Print a DEBUG message */
#define DEBUG(msg, ...) PRINT_DEBUG(HINT, "DEBUG", DEBUG_L, msg, __VA_ARGS__)

/** @brief Print an INFO message */
#define INFO(msg, ...)  PRINT_DEBUG(HINT, "INFO", INFO_L, msg, __VA_ARGS__)

/** @brief Print a WARNING message */
#define WARNING(msg, ...)                                                      \
    PRINT_DEBUG(ATTENTION, "WARNING", WARNING_L, msg, __VA_ARGS__)

/** @brief Print an ERROR message */
#define ERROR(msg, ...)                                                        \
    PRINT_DEBUG(ATTENTION, "ERROR", ERROR_L, msg, __VA_ARGS__)

/** @brief Print a CRITICAL message */
#define CRITICAL(msg, ...)                                                     \
    PRINT_DEBUG(KO, "CRITICAL", CRITICAL_L, msg, __VA_ARGS__)

#endif // ERROR_H_
