#ifndef CUSTOM_SIDE_WIN_MENU
#define CUSTOM_SIDE_WIN_MENU

#include <ncurses.h>
#include "helpers.h"

void clear_side_win_entries();
void update_header();
void create_header();
void add_side_win_entry(char *title, char **headers, char **cols);
int get_side_win_entry(char **entry, int index);

#endif
