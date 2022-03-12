/**
 * @author      : styx
 * @file        : main
 * @created     : 2022-02-17T08:23:32-0500
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

int main(void) {
    printf("%s\n", DATADIR);
    int exit = 0;
    char input[4] = "";

    while (true) {
    exit = run();
        if (exit == ABSURDLE_WIN) {
            printf("Would you like to play again? [Y/n]\n");
            fgets(input, 4, stdin);
            if (input[0] == 'N' || input[0] == 'n') break;
        }
        else if (exit == ABSURDLE_QUIT) break;
    }
    return EXIT_SUCCESS;
}

