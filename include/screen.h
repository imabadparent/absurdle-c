/**
 * @author      : styx
 * @file        : screen
 * @created     : 2022-03-16T14:01:46-0400
 */

#ifndef SCREEN_H
#define SCREEN_H
#include <ncurses.h>

#define ROW_SIZE 5
#define NONE_COLOR 1
#define PART_COLOR 2
#define GOOD_COLOR 3

#define KEYBOARD_ROW_LEN 10
#define KEYBOARD_ROW_NUM 3
#define KEYBOARD_OFFSET 20

struct screen {
    WINDOW *root,
           *cur_row[ROW_SIZE],
           *keyboard[KEYBOARD_ROW_NUM][KEYBOARD_ROW_LEN];
    int row_number,
        x,
        y;
};

void init(struct screen **screen);
void init_keyboard(struct screen **screen);
void add_row(struct screen **screen);
void clear_row(struct screen **screen);
void get_key_location(char key, int *y, int *x);

#endif /* SCREEN_H */

