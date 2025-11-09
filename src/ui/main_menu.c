#include <stdio.h>
#include "main_menu.h"
#include "../core/profiles.h"
#include "../general/globals.h"

const char *main_menu_choices[] = {
	"View Current VPN Connection",
	"Disconnect From VPN",
	"Connect To VPN",
	"Import New VPN Profiles"
};
const int n_main_menu_choices = sizeof(main_menu_choices) / sizeof(char *);

void print_main_menu(WINDOW *win, int highlight)
{
	int attr = has_colors() ? COLOR_PAIR(PAIR_YELLOW_BLUE) : A_REVERSE;
	for (int i = 0, y = getcury(win); i < n_main_menu_choices; i++, y++) {
		if (i != highlight) {
			mvwprintw(win, y, 2, main_menu_choices[i]);
		} else {
			wattron(win, attr);
			mvwprintw(win, y, 2, main_menu_choices[i]);
			wattroff(win, attr);
		}
	}
	wnoutrefresh(win);
}

int handle_main_menu_choice(int choice)
{
	switch (choice) {
	case 3:
		return PROFILE_IMPORT_MENU;
	default:
		return MAIN_MENU;
	}
}

int get_n_main_menu_choices()
{
	return n_main_menu_choices;
}
