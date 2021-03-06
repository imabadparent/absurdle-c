/**
 * @author      : styx
 * @file        : screen
 * @created     : 2022-03-16T14:00:15-0400
 */

#include <ncurses.h>
#include <stdlib.h>
#include "game.h"
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
    init_pair(NO_COLOR, -1, -1);
    getmaxyx(s->root, y, x);
    clear();
    noecho();
    scrollok(s->root, false);
    wrefresh(s->root);

    s->row_number = 0;
    s->rows_tail = NULL;
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
    struct row *r = NULL;
    int i = 0;
    int y = 0;
    int overflow = (getmaxy(s->root)-1)/3;

    r = (struct row *) malloc(sizeof(struct row));
    r->prev = NULL;
    r->next = NULL;
    r->wins[0] = NULL;

    ++s->row_number;
    for (i = 0; i < ROW_SIZE; ++i) {
        y = (s->row_number-1)%overflow*3+1;
        row[i] = newwin(3, 3, y, 3*(i));
        box(row[i], 0, 0);
        wrefresh(row[i]);
        r->wins[i] = row[i];
    }
    if (s->rows_tail != NULL) {
        r->prev = s->rows_tail;
        s->rows_tail->next = r;
    }
    s->rows_tail = r;
    refresh();
    *scr = s;
}

void redraw_row(struct screen **scr, int rownum, const char *g, const char *res) {
    struct screen *s = *scr;
    struct row *r = s->rows_tail;
    int i = 0;
    int y = 0;
    int overflow = (getmaxy(s->root)-1)/3;

    if (rownum > s->row_number) {
        return;
    }
    while (r != NULL) {
        if (i == rownum) break;
        ++i;
        r = r->prev;
    }
    fprintf(stderr, "%d\n", i);

    for (i = 0; i < ROW_SIZE; ++i) {
        if (rownum == 0) y = overflow*3-2;
        else y = (rownum)%overflow*3+1;
        delwin(r->wins[i]);
        r->wins[i] = newwin(3, 3, y, 3*(i));
        box(r->wins[i], 0, 0);
        switch (res[i]) {
            case GOOD:
                wattron(r->wins[i], COLOR_PAIR(GOOD_COLOR));
                mvwaddch(r->wins[i], 1, 1, g[i]);
                wattroff(r->wins[i], COLOR_PAIR(GOOD_COLOR));
                break;
            case PART:
                wattron(r->wins[i], COLOR_PAIR(PART_COLOR));
                mvwaddch(r->wins[i], 1, 1, g[i]);
                wattroff(r->wins[i], COLOR_PAIR(PART_COLOR));
                break;
            case NONE:
                wattron(r->wins[i], COLOR_PAIR(NONE_COLOR));
                mvwaddch(r->wins[i], 1, 1, g[i]);
                wattroff(r->wins[i], COLOR_PAIR(NONE_COLOR));
                break;
        }
        wrefresh(r->wins[i]);
    }

    *scr = s;
}

void remove_row(struct screen **scr) {
    int i = 0;
    struct screen *s = *scr;
    if (s->rows_tail == NULL || s->rows_tail->prev == NULL) return;
    struct row *prev = s->rows_tail->prev;

    for (i = 0; i < ROW_SIZE; ++i) {
        mvwaddch(s->rows_tail->wins[i], 1, 1, ' ');
        wborder(s->rows_tail->wins[i], ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wrefresh(s->rows_tail->wins[i]);
        delwin(s->rows_tail->wins[i]);
    }
    if (s->rows_tail->next != NULL) {
        s->rows_tail->next->prev = prev;
    }
    prev->next = s->rows_tail->next;
    free(s->rows_tail);
    s->rows_tail = prev;
    --s->row_number;
    *scr = s;
}

void remove_all_rows(struct screen **scr) {
    int i = 0;
    struct screen *s = *scr;
    for (i = 0; i < s->row_number; ++i) {
        remove_row(&s);
    }
    if (s->rows_tail == NULL) {
        fprintf(stderr, "ummm\n");
        return;
    }
    for (i = 0; i < ROW_SIZE; ++i) {
        mvwaddch(s->rows_tail->wins[i], 1, 1, ' ');
        wborder(s->rows_tail->wins[i], ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wrefresh(s->rows_tail->wins[i]);
        delwin(s->rows_tail->wins[i]);
    }
    free(s->rows_tail);
    s->rows_tail = NULL;
    *scr = s;

}

void clear_row(struct screen **scr) {
    struct screen *s = *scr;
    int i = 0;

    for (i = 0; i < ROW_SIZE; ++i) {
        wattron(s->rows_tail->wins[i], COLOR_PAIR(NO_COLOR));
        mvwaddch(s->rows_tail->wins[i], 1, 1, ' ');
        wrefresh(s->rows_tail->wins[i]);
        wattroff(s->rows_tail->wins[i], COLOR_PAIR(NO_COLOR));
    }
    refresh();
    *scr = s;
}

