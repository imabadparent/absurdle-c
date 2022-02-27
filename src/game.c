/**
 * @author      : styx (styx@$HOSTNAME)
 * @file        : game
 * @created     : 2022-02-17T08:46:22-0500
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

bool stop_game;

int check_guess(char *guess) {
    char *word = malloc(GUESS_SIZE);
    bool found;
    FILE *f;

    f = fopen(WORD_LIST_PATH, "r");

    while (!feof(f)) {
        found = true;
        fgets(word, GUESS_SIZE, f);
        printf("%s\n", guess);
        if (strncmp(guess, word, 5) == 0) break;
        found = false;
    }

    fclose(f);
    free(word);
    return (found ? GUESS_OK : GUESS_NOT_WORD);
}

/**
 * get guess
 */
int get_guess(char **ret) {
    char c = 0,
         buf[GUESS_SIZE] = "";
    int i = 0;
    bool too_long = false;

    buf[GUESS_SIZE-1] = '\0';

    i = 0;
    for (c = fgetc(stdin); c != '\0' && c != '\n'; c = fgetc(stdin)) {
        if (feof(stdin)) {
            stop();
            return GUESS_QUIT;
        }
        if (c >= 'A' && c <= 'Z') c = c - 32;
        if (c < 'a' || c > 'z') continue;
        if (i >= GUESS_SIZE) {
            too_long = true;
            continue;
        }

        buf[i] = c;
        ++i;
    }
    if (i < GUESS_SIZE-1) {
        printf("Your guess was too short!\n");
    } else if (too_long) {
        printf("Your guess was too long!\n");
    }
    strcpy(*ret, buf);

    return check_guess(*ret);
}

int gen_buckets(char *guess, char **ret) {
    int max_bucket_val;

    return max_bucket_val;
}

/**
 * start the game
 */
int run() {
    char *guess,
         *res;
    int exit;

    stop_game = false;
    guess = malloc(GUESS_SIZE);

    printf("Guess a five letter word!\n");
    while (!stop_game) {
        strcpy(guess, "");

        exit = get_guess(&guess); /* get guess */
        while (exit != 0 && !stop_game) {
            if (exit == GUESS_NOT_WORD) printf("Not in word list, guess again\n");
            exit = get_guess(&guess); /* get guess */
        }
        /* generate buckets and select smallest one */
        /* show results to player */

        /* check win condition */
        /*ask to play again */
    }
    free(guess);
    printf("Game Over\n");
    return 0;
}

/**
 * end the game
 */
int stop() {
    stop_game = true;

    return 0;
}

