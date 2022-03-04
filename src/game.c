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

enum absurdle_code check_guess(const char *guess) {
    char *word = malloc(GUESS_SIZE);
    bool found;
    FILE *f;

    f = fopen(WORD_LIST_PATH, "r");
    if (f == NULL) {
        printf("word list file could not be located");
        return CHECK_NO_LIST;
    }
    while (!feof(f)) {
        found = true;
        fgets(word, GUESS_SIZE, f);
        if (strncmp(guess, word, 5) == 0) break;
        found = false;
    }
    fclose(f);

    f = fopen(ANSWER_LIST_PATH, "r");
    if (f == NULL) {
        printf("answer list file could not be located");
        return CHECK_NO_LIST;
    }
    while (!feof(f)) {
        found = true;
        fgets(word, GUESS_SIZE, f);
        if (strncmp(guess, word, 5) == 0) break;
        found = false;
    }
    fclose(f);

    free(word);
    return (found ? ABSURDLE_OK : GUESS_NOT_WORD);
}

/**
 * get guess
 */
enum absurdle_code get_guess(char *ret) {
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
        return GUESS_TOO_SHORT;
    } else if (too_long) {
        return GUESS_TOO_LONG;
    }
    strncpy(ret, buf, GUESS_SIZE);

    return check_guess(ret);
}

void print_result(unsigned char *res) {
    int i = 0;
    char out[GUESS_SIZE] = { 0 };
    for (i = 0; i < GUESS_SIZE-1; ++i) {
        printf("%d\n", i);
        if (res[i] == GOOD) out[i] = 'a';
        else if (res[i] == PART) out[i] = 'b';
        else if (res[i] == NONE) out[i] = 'c';
    }
    printf("%s\n", out);
}

enum absurdle_code gen_buckets(const char *guess, struct bucket *ret) {
    int i = 0,
        j = 0,
        k = 0,
        max_bucket_val = 0;
    static unsigned char res[GUESS_SIZE] = { 0 };
    for (i = 0; i < ret->size; ++i) {
        printf("%s\n", ret->words[i]);
        for (j = 0; j < guess[j] != '\0'; ++j) {
            if (guess[j] == ret->words[i][j]) {
                res[j] = GOOD;
                continue;
            }
            for (k = 0; k < GUESS_SIZE-1; ++k) {
                if (guess[j] == ret->words[i][j]) {
                    res[j] = PART;
                    break;
                }
            }
        }
        print_result(res);
        memset(res, 0, GUESS_SIZE);
    }

    return max_bucket_val;
}


void wordlist_free(char **list, const size_t size) {
    int i = 0;

    for (i = 0; i < size; ++i) {
        free(list[i]);
        list[i] = NULL;
    }
}

/**
 * start the game
 */
int run() {
    char *guess = NULL,
         *word;
    int i = 0,
        exit = 0;
    struct bucket *buc = NULL;
    FILE *f = NULL;

    stop_game = false;
    guess = (char *) malloc(GUESS_SIZE);
    word  = (char *) malloc(GUESS_SIZE+2);
    buc   = (struct bucket *) malloc(MAX_BUCKETS * sizeof(struct bucket));
    buc->words = (char **) malloc((GUESS_SIZE+2) * MAX_ANSWER_COUNT);
    f = fopen(ANSWER_LIST_PATH, "r");
    if (f == NULL) {
        return CHECK_NO_LIST;
    }
    for (i = 0; !feof(f); ++i) {
        fgets(word, GUESS_SIZE+1, f);
        if (i >= MAX_ANSWER_COUNT) break;
        buc->words[i] = strdup(word);
        strcpy(word, "");
    }
    printf("%d\n", i);
    fclose(f);
    buc->size = i;

    printf("Guess a five letter word!\n");
    while (!stop_game) {
        strcpy(guess, "");

        exit = get_guess(guess); /* get guess */
        while (exit != 0 && !stop_game) {
            switch (exit) {
                case GUESS_NOT_WORD:
                    printf("Not in word list, guess again\n");
                    break;
                case GUESS_TOO_SHORT:
                    printf("Your guess was too short!\n");
                    break;
                case GUESS_TOO_LONG:
                    printf("Your guess was too long!\n");
                    break;
            }
            exit = get_guess(guess); /* get guess */
        }
        gen_buckets(guess, buc); /* generate buckets and select smallest one */
        print_result(buc->res);
        /* show results to player */

        /* check win condition */
        /*ask to play again */
    }
    free(guess);
    free(word);
    wordlist_free(buc->words, MAX_ANSWER_COUNT);
    free(buc->words);
    free(buc);
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

