/**
 * @author      : styx
 * @file        : game
 * @created     : 2022-02-17T08:46:22-0500
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "game.h"
#include "config.h"
#include "screen.h"

bool stop_game;

struct options opt;
const char *challenge;
int current_guess = 0;

struct screen *screen;

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

    if (!opt.force_word) {
        found = true;
        goto ret;
    }

    f = fopen(DATA_DIR"/answers", "r");
    if (f == NULL) {
        return CHECK_NO_LIST;
    }
    while (!feof(f)) {
        found = true;
        fgets(word, GUESS_SIZE, f);
        if (!strncmp(guess, word, 5)) break;
        found = false;
    }
    fclose(f);
    if (found) goto ret;

    f = fopen(DATA_DIR"/words", "r");
    if (f == NULL) {
        return CHECK_NO_LIST;
    }
    while (!feof(f)) {
        found = true;
        fgets(word, GUESS_SIZE, f);
        if (!strncmp(guess, word, 5)) break;
        found = false;
    }
    fclose(f);

ret:
    free(word);
    return (found ? ABSURDLE_OK : GUESS_NOT_WORD);
}

enum absurdle_code get_guess(char *ret, bool new_row) {
    char c = 0,
         buf[GUESS_SIZE] = "";
    int b = 0,
        i = 0,
        exit = 0;
    bool too_long = false;

    buf[GUESS_SIZE-1] = '\0';

    if (new_row) {
        add_row(&screen);
        curs_set(1);
        refresh();
    }

    i = 0;
    while (c != '\n') {
        if (i < GUESS_SIZE-1) {
            b = mvwgetch(screen->rows_tail->wins[i], 1, 1);
        } else {
            curs_set(0);
            b = getch();
        }
        if (b == ',') {
            remove_row(&screen);
            return GUESS_UNDO;
        }
        if (b == 4) {
            stop();
            return ABSURDLE_QUIT;
        }
        if ((char) b == '\n') {
            if (i < GUESS_SIZE-1) continue;
            exit = check_guess(buf);
            if (exit == GUESS_NOT_WORD) {
                continue;
            }
            c = (char) b;
            break;
        }
        if (b == KEY_BACKSPACE || b == KEY_DC || b == 127) {
            if (i <= 0) continue;
            mvwaddch(screen->rows_tail->wins[i-1], 1, 1, ' ');
            curs_set(1);
            --i;
            refresh();
            continue;
        }
        if (i >= GUESS_SIZE-1) {
            continue;
        }
        c = (char) b;
        if (c >= 'A' && c <= 'Z') c = c - 32;
        if (c < 'a' || c > 'z') continue;
        mvwaddch(screen->rows_tail->wins[i], 1, 1, c);
        buf[i] = c;
        wrefresh(screen->rows_tail->wins[i]);
        refresh();

        ++i;
    }

    strncpy(ret, buf, GUESS_SIZE);

    return exit;
}

void update_keys(const char *guess, const char *res) {
    if (res == NULL) return;
    int i = 0,
        x = 0,
        y = 0,
        val[26] = { 0 };
    for (i = 0; i < ROW_SIZE && i < GUESS_SIZE; ++i) {
        get_key_location(guess[i], &y, &x);
        switch (res[i]) {
            case GOOD:
                if (val[guess[i]-'a'] >= 2) break;
                wattron(screen->keyboard[y][x], COLOR_PAIR(GOOD_COLOR));
                mvwaddch(screen->keyboard[y][x], 1, 1, guess[i]);
                attroff(COLOR_PAIR(GOOD_COLOR));
                val[guess[i]-'a'] = 2;
                break;
            case PART:
                if (val[guess[i]-'a'] >= 1) break;
                wattron(screen->keyboard[y][x], COLOR_PAIR(PART_COLOR));
                mvwaddch(screen->keyboard[y][x], 1, 1, guess[i]);
                attroff(COLOR_PAIR(PART_COLOR));
                val[guess[i]-'a'] = 1;
                break;
            case NONE:
                if (val[guess[i]-'a'] > 0) break;
                wattron(screen->keyboard[y][x], COLOR_PAIR(NONE_COLOR));
                mvwaddch(screen->keyboard[y][x], 1, 1, guess[i]);
                attroff(COLOR_PAIR(NONE_COLOR));
                val[guess[i]-'a'] = 0;
                break;
        }
        wrefresh(screen->keyboard[y][x]);
    }
}

void print_result(const char *guess, const char *res) {
    if (res == NULL) return;
    int i = 0,
        x = 0,
        y = 0,
        val[26] = { 0 };
    for (i = 0; i < ROW_SIZE && i < GUESS_SIZE; ++i) {
        get_key_location(guess[i], &y, &x);
        switch (res[i]) {
            case GOOD:
                wattron(screen->rows_tail->wins[i], COLOR_PAIR(GOOD_COLOR));
                mvwaddch(screen->rows_tail->wins[i], 1, 1, guess[i]);
                attroff(COLOR_PAIR(GOOD_COLOR));
                if (val[guess[i]-'a'] >= 2) break;
                wattron(screen->keyboard[y][x], COLOR_PAIR(GOOD_COLOR));
                mvwaddch(screen->keyboard[y][x], 1, 1, guess[i]);
                attroff(COLOR_PAIR(GOOD_COLOR));
                val[guess[i]-'a'] = 2;
                break;
            case PART:
                wattron(screen->rows_tail->wins[i], COLOR_PAIR(PART_COLOR));
                mvwaddch(screen->rows_tail->wins[i], 1, 1, guess[i]);
                attroff(COLOR_PAIR(GOOD_COLOR));
                if (val[guess[i]-'a'] >= 1) break;
                wattron(screen->keyboard[y][x], COLOR_PAIR(PART_COLOR));
                mvwaddch(screen->keyboard[y][x], 1, 1, guess[i]);
                attroff(COLOR_PAIR(PART_COLOR));
                val[guess[i]-'a'] = 1;
                break;
            case NONE:
                wattron(screen->rows_tail->wins[i], COLOR_PAIR(NONE_COLOR));
                mvwaddch(screen->rows_tail->wins[i], 1, 1, guess[i]);
                attroff(COLOR_PAIR(GOOD_COLOR));
                if (val[guess[i]-'a'] > 0) break;
                wattron(screen->keyboard[y][x], COLOR_PAIR(NONE_COLOR));
                mvwaddch(screen->keyboard[y][x], 1, 1, guess[i]);
                attroff(COLOR_PAIR(NONE_COLOR));
                val[guess[i]-'a'] = 0;
                break;
        }
        wrefresh(screen->keyboard[y][x]);
        wrefresh(screen->rows_tail->wins[i]);
    }
    refresh();
}

enum absurdle_code init_bucket(struct bucket **b) {
    int i = 0;
    FILE *f = NULL;
    char word[GUESS_SIZE+2] = "";
    static struct bucket *buc;

    buc = (struct bucket *) malloc(sizeof(struct bucket));
    buc->words = (struct word *) malloc(MAX_ANSWER_COUNT * sizeof(struct word));

    f = fopen(DATA_DIR"/answers", "r");
    if (f == NULL) {
        return CHECK_NO_LIST;
    }

    for (i = 0; !feof(f); ++i) {
        fgets(word, GUESS_SIZE+1, f);
        if (i >= MAX_ANSWER_COUNT) break;
        word[strcspn(word, "\n")] = '\0';
        buc->words[i].value = strdup(word);
        buc->words[i].result = NULL;
        memset(word, 0, GUESS_SIZE+2);
    }
    fclose(f);
    buc->size = i;

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
            ++freq[ret->words[i].value[j]-'a'];
        }
        for (j = 0; guess[j] != '\0'; ++j) {
            if (guess[j] != ret->words[i].value[j]) continue;
            res[j] = GOOD;
            --freq[guess[j]-'a'];
        }
        for (j = 0; guess[j] != '\0'; ++j) {
            if (res[j] != 0) continue;
            if (freq[guess[j]-'a'] > 0) {
                res[j] = PART;
                freq[guess[j]-'a']--;
            } else {
                res[j] = NONE;
            }
        }

        if (ret->words[i].result != NULL) free(ret->words[i].result);
        ret->words[i].result = strdup(res);
        memset(res, 0, GUESS_SIZE);
    }

    for (i = 0; i < ret->size; ++i) {
        cur_bucket_val = 0;
        for (j = 0; j < ret->size; ++j) {
            if (!strncmp(ret->words[i].result, ret->words[j].result, GUESS_SIZE)) {
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
                strncpy(res, ret->words[i].result, GUESS_SIZE);
            }
        }
        else if (cur_bucket_val > max_bucket_val) {
            max_bucket_val = cur_bucket_val;
            strncpy(res, ret->words[i].result, GUESS_SIZE);
        }
    }
    for (i = 0, j = 0; i < ret->size; ++i) {
        if (strncmp(res, ret->words[i].result, GUESS_SIZE)) continue;
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

enum absurdle_code undo_guess(char ***guesses, struct bucket **buc) {
    struct bucket *b = *buc;
    char **g = *guesses;
    int i = 0;

    clear_row(&screen);
    --current_guess;
    g[current_guess] = NULL;
    wordlist_free(b->words, b->size);
    init_bucket(&b);
    init_keyboard(&screen);
    for (i = 0; g[i] != NULL; ++i) {
        gen_buckets(g[i], &b);
        update_keys(g[i], b->result);
    }

    *guesses = g;
    *buc = b;
    return ABSURDLE_OK;
}

/**
 * start the game
 */
int run(struct options o, struct screen **scr) {
    char *guess = NULL,
         input[4] = "",
         **guesses = NULL,
         **tmp = NULL;
    int exit = 0,
        r = 0,
        i = 0,
        guess_list_size = GUESS_LIMIT;
    bool win = false,
         new_row = true;
    struct bucket *buc = NULL;

    stop_game = false;
    opt = o;
    guess = (char *) malloc(GUESS_SIZE);
    guesses = (char **) malloc(guess_list_size*sizeof(char *));

    screen = *scr;
    refresh();

    init_bucket(&buc);
    buc->result = NULL;

    if (opt.challenge_mode) {
        srand(time(0));
        r = rand() % (MAX_ANSWER_COUNT);
        challenge = strdup(buc->words[r].value);

        mvwprintw(screen->root, 0, 0, "Your word is: %s", challenge);
    } else {
        mvwprintw(screen->root, 0, 0, "Guess a five letter word!");
    }
    refresh();

    while (!stop_game) {
        memset(guess, 0, GUESS_SIZE);

        exit = get_guess(guess, new_row); /* get guess */
        if (exit == GUESS_UNDO) {
            new_row = false;
            undo_guess(&guesses, &buc);
            continue;
        }
        else new_row = true;
        if (exit == GUESS_QUIT) break;
        if (current_guess >= guess_list_size) {
            guess_list_size *= 2;
            tmp = (char **) realloc(guesses, guess_list_size*sizeof(char *));
            if (tmp == NULL) return ABSURDLE_QUIT; /* out of memory */
            guesses = tmp;
        }
        guesses[current_guess] = strdup(guess);
        ++current_guess;
        gen_buckets(guess, &buc); /* generate buckets and select smallest one */
        print_result(guess, buc->result); /* show results to player */

        /* check win/lose condition(s) */
        if (opt.challenge_mode) {
            bool fail = true;
            for (i = 0; i < buc->size; ++i) {
                if (!strncmp(challenge, buc->words[i].value, GUESS_SIZE)) {
                    fail = false;
                }
            }
            if (fail) {
                mvwprintw(screen->root, screen->y-1, 0,
                        "It is no longer possible for %s to be the secret word!",
                       challenge);
                win = true;
                goto win;
            }
        }
        win = !(strncmp(buc->result, WIN, GUESS_SIZE-1));
win:
        if (!win) continue;
        curs_set(1);
        mvwprintw(screen->root, getmaxy(screen->root)-1, 0, "Undo last guess [y/N] ");
        echo();
        refresh();
        wgetstr(screen->root, input);
        if (input[0] == 'Y' || input[0] == 'y') {
            noecho();
            undo_guess(&guesses, &buc);
            new_row = false;
            continue;
        }
        return ABSURDLE_WIN;
    }
    free(guess);
    for (i = 0; i < current_guess; ++i) {
        free(guesses[i]);
    }
    wordlist_free(buc->words, buc->size);
    free(buc->result);
    free(buc);
    if (opt.challenge_mode) free((void *) challenge);
    return ABSURDLE_QUIT;
}

/**
 * end the game
 */
int stop() {
    stop_game = true;
    return 0;
}

