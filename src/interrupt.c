#include "interrupt.h"

int timeout_value = TIMEOUT_DEFAULT;

unsigned int limit_value = LIMIT_DEFAULT;

bool interrupt_flag = false, timeout_flag = false, limit_flag = false;

void ignore_timeout(void) {
    struct sigaction action;
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    // Cancel the previous alarm, if any.
    alarm(0);

    sigaction(SIGALRM, &action, NULL);
}

void listen_timeout(int value) {
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    // Cancel the previous alarm, if any.
    alarm(0);
    interrupt_flag = false;

    sigaction(SIGALRM, &action, NULL);

    alarm(value);
}

void ignore_interrupt(void) {
    struct sigaction action;
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, NULL);
}

void listen_interrupt(void) {
    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    timeout_flag = false;

    sigaction(SIGINT, &action, NULL);
}

void handler(int signum) {
    if (signum == SIGINT) {
        WARNING("#### Computation interrupted!");
        interrupt_flag = true;
        alarm(0);
        ignore_interrupt();
        return;
    }

    if (signum == SIGALRM) {
        WARNING("#### Timeout occurred!");
        timeout_flag = true;
        ignore_timeout();
        return;
    }

    // Should not happen, other signals are not catched for now.
    CRITICAL("#### Interruption by signal %d\n", signum);
    return;
}

void listening_mode(int value) {
    listen_interrupt();
    listen_timeout(value);
}

void normal_mode(void) {
    ignore_interrupt();
    ignore_timeout();
}
