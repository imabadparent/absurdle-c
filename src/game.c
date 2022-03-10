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

void wordlist_free(struct word *list, const size_t size) {
    int i = 0;

    for (i = 0; i < size; ++i) {
        free((void *) list[i].value);
        free((void *) list[i].result);
    }
    if (i != 0) free(list);
}

enum absurdle_code check_guess(const char *guess) {
    char *word = malloc(GUESS_SIZE);
    bool found;
    FILE *f;

    f = fopen(ANSWER_LIST_PATH, "r");
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
    if (found) goto ret;

    f = fopen(WORD_LIST_PATH, "r");
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

ret:
    free(word);
    return (found ? ABSURDLE_OK : GUESS_NOT_WORD);
}

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

void print_result(char *res) {
    if (res == NULL) return;
    printf("%s\n", res);
}

enum absurdle_code init_bucket(struct bucket **b) {
    int i = 0;
    FILE *f = NULL;
    char word[GUESS_SIZE+2] = "";
    static struct bucket *buc;

    buc = (struct bucket *) malloc(sizeof(struct bucket));
    buc->words = (struct word *) malloc(MAX_ANSWER_COUNT * sizeof(struct word));

    f = fopen(ANSWER_LIST_PATH, "r");
    if (f == NULL) {
        return CHECK_NO_LIST;
    }

    for (i = 0; !feof(f); ++i) {
        fgets(word, GUESS_SIZE+1, f);
        if (i >= MAX_ANSWER_COUNT) break;
        word[strcspn(word, "\n")] = '\0';
        buc->words[i].value = strdup(word);
        buc->words[i].result = NULL;
        strcpy(word, "");
    }
    fclose(f);
    buc->size = i;
    printf("%d\n", i);

    *b = buc;

    return ABSURDLE_OK;
}

enum absurdle_code gen_buckets(const char *guess, struct bucket **b) {
    int i = 0,
        j = 0,
        k = 0,
        cur_bucket_val = 0,
        max_bucket_val = 0,
        freq[26] = { 0 };
    char res[GUESS_SIZE] = { 0 };
    struct bucket *ret = *b;
    struct word *ret_words = (struct word *) malloc(MAX_ANSWER_COUNT * sizeof(struct word));

    for (i = 0; i < ret->size; ++i) {
        for (j = 0; j < 26; ++j) {
            freq[j] = 0;
        }
        for (j = 0; ret->words[i].value[j] != '\0'; ++j) {
            freq[ret->words[i].value[j]-'a']++;
        }
        for (j = 0; ret->words[i].value[j] != '\0'; ++j) {
            if (freq[guess[j]-'a'] > 0) {
                res[j] = PART;
                freq[guess[j]-'a']--;
            } else {
                res[j] = NONE;
            }
        }
        for (j = 0; guess[j] != '\0'; ++j) {
            if (guess[j] == ret->words[i].value[j]) {
                res[j] = GOOD;
                freq[guess[j]-'a']--;
                continue;
            }
        }

        if (ret->words[i].result != NULL) free(ret->words[i].result);
        ret->words[i].result = strdup(res);
        memset(res, 0, GUESS_SIZE);
    }

    for (i = 0; i < ret->size; ++i) {
        cur_bucket_val = 0;
        for (j = 0; j < ret->size; ++j) {
            if (strncmp(ret->words[i].result, ret->words[j].result, GUESS_SIZE) == 0) {
                ++cur_bucket_val;
            }
        }
        if (cur_bucket_val == max_bucket_val) {
            int cur_sum = 0;
            int pre_sum = 0;
            for (j = 0; j < GUESS_SIZE; ++j) {
                switch (ret->words[i].result[j]) {
                    case GOOD:
                        cur_sum += 3;
                        break;
                    case PART:
                        cur_sum += 1;
                        break;
                    case NONE:
                        cur_sum += 0;
                        break;
                }
            }
            for (j = 0; j < GUESS_SIZE; ++j) {
                switch (res[j]) {
                    case GOOD:
                        pre_sum += 3;
                        break;
                    case PART:
                        pre_sum += 1;
                        break;
                    case NONE:
                        pre_sum += 0;
                        break;
                }
            }
            if (cur_sum < pre_sum) {
                max_bucket_val = cur_bucket_val;
                strcpy(res, ret->words[i].result);
            }
        }
        else if (cur_bucket_val > max_bucket_val) {
            max_bucket_val = cur_bucket_val;
            strcpy(res, ret->words[i].result);
        }
    }
    for (i = 0, j = 0; i < ret->size; ++i) {
        if (strncmp(res, ret->words[i].result, GUESS_SIZE) != 0) continue;
        ret_words[j].value = strdup(ret->words[i].value);
        ret_words[j].result = strdup(ret->words[i].result);
        ++j;
    }
    wordlist_free(ret->words, ret->size);
    ret->words = ret_words;
    ret->result = strdup(res);
    ret->size = j;
    *b = ret;

    return max_bucket_val;
}


/**
 * start the game
 */
int run() {
    char *guess = NULL;
    int exit = 0;
    struct bucket *buc = NULL;

    stop_game = false;
    guess = (char *) malloc(GUESS_SIZE);

    init_bucket(&buc);
    buc->result = NULL;

    printf("Guess a five letter word!\n");
    while (!stop_game) {
        memset(guess, 0, GUESS_SIZE);

        exit = get_guess(guess); /* get guess */
        while (exit != ABSURDLE_OK && exit != GUESS_QUIT && !stop_game) {
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
        if (exit == GUESS_QUIT) continue;
        gen_buckets(guess, &buc); /* generate buckets and select smallest one */
        print_result(buc->result);
        /* show results to player */

        /* check win condition */
        /*ask to play again */
    }
    free(guess);
    wordlist_free(buc->words, buc->size);
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
