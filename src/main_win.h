#ifndef CUSTOM_MAIN_WIN_MENU
#define CUSTOM_MAIN_WIN_MENU

#include <ncurses.h>
#include "helpers.h"

void clear_main_win_entries();
int get_main_win_entry(char **entry, int index);
void add_main_win_entry(char *entry);

#endif
