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
    init_pair(NONE_COLOR, COLOR_BLACK, COLOR_WHITE);
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
    s->keyboard[0][0] = NULL;

    *scr = s;
}

void init_keyboard(struct screen **scr) {
    struct screen *s = *scr;
    char letters[KEYBOARD_ROW_NUM][KEYBOARD_ROW_LEN] = {
        {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
        {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\0'},
        {'z', 'x', 'c', 'v', 'b', 'n', 'm', '\0'}
    };
    static WINDOW *keys[KEYBOARD_ROW_NUM][KEYBOARD_ROW_LEN] = { 0 };
    int i = 0,
        j = 0,
        offset = KEYBOARD_OFFSET;
    for (i = 0; i < KEYBOARD_ROW_NUM; ++i) {
        for (j = 0; j < KEYBOARD_ROW_LEN; ++j) {
            if (letters[i][j] == '\0') break;
            if (i == 1) offset = KEYBOARD_OFFSET + 1;
            else if (i == 2) offset = KEYBOARD_OFFSET + 4;
            keys[i][j] = newwin(3, 3, i*3+1, (3*j)+offset);
            box(keys[i][j], 0, 0);
            mvwaddch(keys[i][j], 1, 1, letters[i][j]);
            wrefresh(keys[i][j]);
            s->keyboard[i][j] = keys[i][j];
        }
    }
    *scr = s;
}

void get_key_location(char key, int *y, int *x) {
    int i = 0,
        j = 0;
    char letters[KEYBOARD_ROW_NUM][KEYBOARD_ROW_LEN] = {
        {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
        {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\0'},
        {'z', 'x', 'c', 'v', 'b', 'n', 'm', '\0'}
    };
    for (i = 0; i < KEYBOARD_ROW_NUM; ++i) {
        for (j = 0; j < KEYBOARD_ROW_LEN; ++j) {
            if (letters[i][j] == key) {
                *y = i;
                *x = j;
            }
        }
    }
}

void add_row(struct screen **scr) {
    struct screen *s = *scr;
    static WINDOW *row[ROW_SIZE] = { 0 };
    int i = 0;
    int col = s->row_number*3+1;

    for (i = 0; i < ROW_SIZE; ++i) {
        row[i] = newwin(3, 3, col, 3*(i));
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

