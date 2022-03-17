/**
 * @author      : styx
 * @file        : main
 * @created     : 2022-02-17T08:23:32-0500
 */

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "screen.h"

#include "config.h"

void read_config(const char *path, struct options *opt) {
    char buf[64],
         *key,
         *value;
    FILE *f = fopen(path, "r");
    static struct options o = {
        .challenge_mode = false,
        .force_word = true
    };

    if (f == NULL) {
        return;
    }
    while (!feof(f)) {
        fscanf(f, "%s\n", buf);
        key = strtok(buf, "=");
        value = strtok(NULL, "=");
        if (!strncmp(key, "challenge_mode", 14)) {
            if (!strncmp(value, "true", 4)) o.challenge_mode = true;
        }
        if (!strncmp(key, "force_word", 10)) {
            if (!strncmp(value, "false", 5)) o.force_word = false;
        }
    }
    *opt = (struct options) o;

}

int main(void) {
    int exit = 0;
    char input[4] = "",
         *conf_dir = "",
         conf_path[1024] = "";
    struct options opt = { 0 };
    struct screen *scr;

    scr = (struct screen *) malloc(sizeof(struct screen));
#ifdef USE_XDG_CONF_DIR
    conf_dir = getenv("XDG_CONFIG_HOME");
    if (conf_dir == NULL) {
        conf_dir = getenv("HOME");
        sprintf(conf_path, "%s/.config/absurdle-c/config", conf_dir);
    } else {
        sprintf(conf_path, "%s/absurdle-c/config", conf_dir);
    }
#else
    conf_path = CONFIG_DIR;
#endif
    read_config(conf_path, &opt);

    scr->root = NULL;
     while (true) {
        init(&scr);
        curs_set(1);
        exit = run(opt, &scr);
        if (exit == ABSURDLE_WIN) {
            mvwprintw(scr->root, scr->row_number*3+1, 0, "Would you like to play again? [Y/n]");
            echo();
            refresh();
            wgetstr(scr->root, input);
            if (input[0] == 'N' || input[0] == 'n') break;
        }
        else if (exit == ABSURDLE_QUIT) break;
        endwin();
    }
    return EXIT_SUCCESS;
}

