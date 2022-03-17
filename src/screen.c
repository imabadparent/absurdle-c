/**
 * @author      : styx
 * @file        : screen
 * @created     : 2022-03-16T14:00:15-0400
 */

#include <ncurses.h>
#include "screen.h"

void init(struct screen **scr) {
    struct screen *s = *scr;
    int x, y;

    if (s->root != NULL) endwin();
    s->root = initscr();
    cbreak();
    use_default_colors();
    start_color();
    init_pair(NONE_COLOR, COLOR_WHITE, -1);
    init_pair(PART_COLOR, COLOR_BLACK, COLOR_YELLOW);
    init_pair(GOOD_COLOR, COLOR_BLACK, COLOR_GREEN);
    getmaxyx(s->root, y, x);
    clear();
    noecho();
    scrollok(s->root, true);
    wprintw(s->root, "Absurdle!");
    wrefresh(s->root);

    s->row_number = 0;
    s->cur_row[0] = NULL;

    *scr = s;
}

void add_row(struct screen **scr) {
    struct screen *s = *scr;
    static WINDOW *row[ROW_SIZE] = { 0 };
    int i = 0;

    for (i = 0; i < ROW_SIZE; ++i) {
        row[i] = newwin(3, 3, s->row_number*3+1, 3*(i));
        box(row[i], 0, 0);
        wrefresh(row[i]);
        s->cur_row[i] = row[i];
    }
    ++s->row_number;
    refresh();
    *scr = s;
}

void clear_row(struct screen **scr) {
    struct screen *s = *scr;
    int i = 0;

    for (i = 0; i < ROW_SIZE; ++i) {
        mvwaddch(s->cur_row[i], 1, 1, ' ');
        wrefresh(s->cur_row[i]);
    }
    refresh();
    *scr = s;
}

