/*******************************************************************************

 * Name        : snooze.c

 * Author      : Michael Zylka, Christian Szablewski-Paz

 * Date        : 4/9/2021

 * Description : make a program to hangle signals with sleep

 * Pledge : I pledge my honor that I have abided by the Stevens Honor System.

 ******************************************************************************/





#include <signal.h>

#include <stdbool.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <errno.h>



/* TODO: Implement signal handler */

volatile sig_atomic_t sigVal = 0;

void catch_signal(int sig) {

    sigVal = sig;

}



/**

 * Description:

 * The 'snooze' program takes in a single parameter, which is the number

 * of seconds (no less than 1) the program should sleep.

 *

 * It catches the SIGINT signal. When doing so, it should stop sleeping and

 * print how long it actually slept.

 *

 * For example, if the user runs "./snooze 5" and then types CTRL+C after 3

 * seconds, the program should output:

 * Slept for 3 of the 5 seconds allowed.

 *

 * If the user runs "./snooze 5" and never types CTRL+C, the program should

 * output:

 * Slept for 5 of the 5 seconds allowed.

 */

int main(int argc, char *argv[]) {

    struct sigaction sa;

    memset(&sa, 0, sizeof(struct sigaction));

    sa.sa_handler = catch_signal;

    sigemptyset(&sa.sa_mask);

    // TODO: Print the usage message "Usage: %s <seconds>\n" and return in

    // failure if the argument <seconds> is not present.

    if (argc != 2) {

        fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);

        return EXIT_FAILURE;

    }



    // TODO: Parse the argument, and accept only a positive int. If the argument

    // is invalid, error out with the message:

    // "Error: Invalid number of seconds '%s' for max snooze time.\n",

    // where %s is whatever argument the user supplied.



    if (atoi(argv[1]) < 1) {

        fprintf(stderr, "Error: Invalid number of seconds '%s' for max snooze time.\n", argv[1]);

        return EXIT_FAILURE;

    }

    int maxSleep = atoi(argv[1]);



    // TODO: Create a sigaction to handle SIGINT.

    if (sigaction(SIGINT, &sa, NULL) < 0) {

        fprintf(stderr, "Error: Could not register signal handler. %s\n", strerror(errno));

        return EXIT_FAILURE;

    }



    // TODO: Loop and sleep for 1 second at a time, being able to stop looping

    // when the signal is processed.

    

    int sleepTime = 0;

    while (sleepTime < maxSleep) {

        if (sigVal == SIGINT) {

            break;

        }

        sleep(1);

        sleepTime++;

    }



    printf("Slept for %d of the %d seconds allowed.\n", sleepTime, maxSleep);

    return EXIT_SUCCESS;

}

