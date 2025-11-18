#ifndef CUSTOM_GENERAL_WIN_MENU
#define CUSTOM_GENERAL_WIN_MENU

#include <ncurses.h>
#include <stdarg.h>
#include <stdbool.h>
#include "globals.h"

int print_updated_wins();
int print_win(int win_n);
void print_new_win(int win_n);

struct _bordered_win *get_bwin(int bwin);
WINDOW *get_win(int win);

int get_max_entries(int win_n);

struct _win_scroller *get_win_scroller(int win_n);

int get_win_entry(int win_n, char **entry, int index);

void reset_win(WINDOW *w);
void delete_win(WINDOW *local_win);
void delete_bordered_win(struct _bordered_win *bw);
void delete_windows();

void dim_win_border(struct _bordered_win *bwin);
void highlight_win_border(struct _bordered_win *bwin);

void cmd_win_print(bool update, int attr, const char *fmt, ...);

#endif
