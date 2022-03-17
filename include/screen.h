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

struct screen {
    WINDOW *root,
           *cur_row[ROW_SIZE];
    int row_number,
        x,
        y;
};

void init(struct screen **screen);
void add_row(struct screen **screen);
void clear_row(struct screen **screen);

#endif /* SCREEN_H */

