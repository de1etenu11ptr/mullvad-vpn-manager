#include <stdio.h>
#include "main_menu.h"
#include "main_win.h"
#include "profiles.h"
#include "profile_import.h"
#include "profiles_list.h"
#include "scroller.h"
#include "globals.h"
#include "window_helpers.h"

char *main_menu_choices[] = {
	"Profiles List",
	"Import New VPN Profiles",
	"View Current VPN Connection",
	"Disconnect From VPN",
	"Connect To VPN",
	"Exit Program"
};
const int n_main_menu_entries = 6;


int handle_main_menu_choice()
{
	struct _win_scroller *scroller = get_win_scroller(current_win);
	WINDOW *win = get_win(current_win);
	switch (scroller->highlighted) {
	case 0:
		int ret = profiles_list();
		if (ret == SUCCESS) {
			current_menu = PROFILES_LIST_MENU;
			return PROFILES_LIST_MENU;
		}
		return MAIN_MENU;
	case 1:
		profile_import();
		wgetch(win);
		print_new_win(current_win);
		doupdate();
		return MAIN_MENU;
	case 5:
		exit_program(EXIT_PROGRAM);
		break;
	default:
		cmd_win_print(false, A_BLINK | SAFE_COLOR_PAIR(PAIR_ERROR),
			"Option not implemented yet.",
			NULL);
		cmd_win_cooldown = 1;
		return MAIN_MENU;
	}
	return MAIN_MENU;
}

void print_main_menu()
{
	clear_main_win_entries();
	for (int i = 0; i < n_main_menu_entries; i++) {
		add_main_win_entry(main_menu_choices[i]);
	}
}
