/**
 * @author      : styx
 * @file        : game
 * @created     : 2022-02-17T08:47:47-0500
 */
#ifndef GAME_H
#define GAME_H

typedef enum {false, true} bool;

#define GUESS_SIZE 6
#define MAX_BUCKETS 243
#define MAX_ANSWER_COUNT 2315
#define WORD_LIST_PATH "../lists/words"
#define ANSWER_LIST_PATH "../lists/answers"

#define NONE '.'
#define PART '?'
#define GOOD '!'

enum absurdle_code {
    ABSURDLE_OK,
    GUESS_QUIT,
    GUESS_NOT_WORD,
    GUESS_TOO_SHORT,
    GUESS_TOO_LONG,
    CHECK_NO_LIST
};

struct word {
    const char *value;
    char *result;
};

struct bucket {
    char *result;
    struct word *words;
    unsigned long size;
};

int run();
int stop();

#endif /* GAME_H */
