/**
 * @file interrupt.h
 * @brief
 * Functions to handle interrupts and timeouts.
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include <setjmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "error.h"

/**
 * @brief
 * The default timeout value. This is the number of seconds before the computation is interrupted.
 */
#define TIMEOUT_DEFAULT 5

/**
 * @brief The timeout value. It is set to 0 if no timeout is set, and to the
 * number of seconds before the timeout otherwise, by functions that perform
 * long computations in order to be interrupted.
 */
extern int timeout_value;

/**
 * The timeout flag. Indicates if a timeout occurred. It is set to true by the
 * timeout handler and should be checked by the program when it starts long
 * computations, to know if it should stop.
 */
extern bool timeout_flag;

/**
 * @brief
 * The default limit value. This is the size of an object above which the computation is interrupted.
 */
#define LIMIT_DEFAULT 1000000

/**
 * @brief The limit value. It is set to 0 if no limit is set, and to the number
 * of elements of an object before a computation is interrupted otherwise, by
 * functions that perform long computations.
 */
extern unsigned int limit_value;

/**
 * The limit flag. Indicates whether a timeout has occurred. It is checked by
 * the program when it starts long computations, to know whether it should stop.
 */
extern bool limit_flag;

/**
 * @brief
 * The interrupt flag. Indicates if the interrupt signal was received.
 * It is set to true by the interrupt handler and should be checked by the
 * program when it starts long computations, to know if it should stop.
 */
extern bool interrupt_flag;

/**
 * @brief
 * Ignores the timeout signal.
 */
void ignore_timeout(void);

/**
 * @brief
 * Sets an alarm and listens to the SIGALRM signal.
 */
void listen_timeout(int value //!< The number of seconds before the timeout.
);

/**
 * @brief
 * Ignores the interrupt signal.
 */
void ignore_interrupt(void);

/**
 * @brief
 * Listens to the interrupt signal.
 */
void listen_interrupt(void);

/**
 * @brief
 * The interrupt handler.
 */
void handler(int signum //!< The signal number. Should be SIGINT.
);

/**
 * @brief
 * Listens to all signals (SIGINT and SIGALRM).
 */
void listening_mode(int value);

/**
 * @brief
 * Ignores all signals and cancel pending alarm, if any.
 */
void normal_mode(void);

#endif // INTERRUPT_H_
