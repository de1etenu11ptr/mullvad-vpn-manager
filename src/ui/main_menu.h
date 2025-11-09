#ifndef CUSTOM_MAIN_WIN
#define CUSTOM_MAIN_WIN

#include <ncurses.h>
#include "../general/helper.h"

void print_main_menu(WINDOW *win, int highlight);
int handle_main_menu_choice(int choice);
int get_n_main_menu_choices();

#endif
