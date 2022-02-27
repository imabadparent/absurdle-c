/**
 * @author      : styx
 * @file        : game
 * @created     : 2022-02-17T08:47:47-0500
 */
#ifndef GAME_H
#define GAME_H

typedef enum {false, true} bool;

#define GUESS_SIZE 6
#define WORD_LIST_PATH "../words"

enum guess_exit_codes {
    GUESS_OK,
    GUESS_QUIT,
    GUESS_NOT_WORD,
};

int run();
int stop();

#endif /* GAME_H */

