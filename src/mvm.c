#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdbool.h>
#include "globals.h"
#include "init.h"
#include "helpers.h"
#include "encryption.h"
#include "profiles.h"
#include "cmdline.h"
#include "main_menu.h"
#include "logo.h"
#include "side_win.h"
#include "scroller.h"
#include "profile_import.h"
#include "profiles_list.h"
#include "window_helpers.h"

FILE *LOG;
const char *HOME_DIR;
char *MVM_LOG_PATH;

struct _win_manager *win_manager;
int current_win;
int current_menu;
int cmd_mode;
int input_mode;

const int interactive_windows[] = {
	MAIN_WIN,
	SIDE_WIN
};
const int n_interactive_windows = 2;

int cgetch(WINDOW *win, bool handle);

int handle_movement_key(int direction)
{
	struct _win_scroller *scroller = get_win_scroller(current_win);
	int max_entries = get_max_entries(current_win);
	if (max_entries <= 1) return SUCCESS;
	if (direction == (int) 'k') {
		scroller->prev_highlighted = scroller->highlighted;
		if (scroller->highlighted <= 0)
			scroller->highlighted = max_entries - 1;
		else
			scroller->highlighted--;
	} else if (direction == (int) 'j') {
		scroller->prev_highlighted = scroller->highlighted;
		if (scroller->highlighted >= max_entries - 1)
			scroller->highlighted = 0;
		else
			scroller->highlighted++;
	}
	return SUCCESS;
}

int handle_choice()
{
	if (current_win == SIDE_WIN)
		return SUCCESS;
	// In what menu was the "choice" selected
	struct _win_scroller *scroller = get_win_scroller(current_win);
	int ret = SUCCESS;
	switch (current_menu) {
	case MAIN_MENU:
		ret = handle_main_menu_choice();
		scroller->prev_highlighted = scroller->highlighted;
		scroller->highlighted = 0;
		break;
	case PROFILES_LIST_MENU:
		current_menu = MAIN_MENU;
		load_profile(scroller->highlighted);
		print_main_menu();
		print_new_win(MAIN_WIN);
		wnoutrefresh(get_win(MAIN_WIN));
		doupdate();
		break;
	}
	return SUCCESS;
}

struct _win_scroller *find_next_interactive_window()
{
	int prev_win = -1;
	struct _win_scroller *prev_scroller = NULL;
	struct _win_scroller *scroller = get_win_scroller(current_win);
	int found = FALSE;
	int start_search = OFF;
	for (int i = 0; i < n_interactive_windows; i++) {
		int tmp;
		if (interactive_windows[i] == current_win) {
			start_search = ON;
			if (i == n_interactive_windows - 1)
				continue;
			tmp = interactive_windows[i + 1];
			if (get_max_entries(tmp) <= 0)
				continue;
			found = TRUE;
			current_win = tmp;
			scroller = get_win_scroller(current_win);
			break;
		}
		if (start_search == OFF && prev_scroller == NULL) {
			tmp = interactive_windows[i];
			if (get_max_entries(tmp) <= 0)
				continue;
			scroller = get_win_scroller(tmp);
			prev_win = tmp;
		}
		if (start_search == OFF || interactive_windows[i] == current_win)
			continue;
		tmp = interactive_windows[i];
		if (get_max_entries(tmp) <= 0)
			continue;
		scroller = get_win_scroller(tmp);
		current_win = tmp;
		found = TRUE;
		break;
	}
	if (found == FALSE) {
		if (prev_win != -1) {
			current_win = prev_win;
			scroller = get_win_scroller(current_win);
		}
	}
	return scroller;
}


int cgetch(WINDOW *win, bool handle)
{
	int result;
	int c = wgetch(win);
	if (!handle)
		return c;
	switch (c) {
	case 3:
	case 4:
		exit_program(EXIT_PROGRAM);
		break;
	case (int) '/':
		if (current_win != SIDE_WIN)
			break;
		cmd_mode = true;
		result = cmdline_search(current_win);
		cmd_mode = false;
		return result;
	case (int) 'k':
	case KEY_UP:
		handle_movement_key('k');
		break;
	case (int) 'j':
	case KEY_DOWN:
		handle_movement_key('j');
		break;
	case 10: // Enter key.
		handle_choice();
		break;
	case 9: // Tab key.
		dim_win_border(get_bwin(current_win));
		struct _win_scroller *scroller = find_next_interactive_window();
		highlight_win_border(get_bwin(current_win));
		cmd_win_print(false, 0, "", NULL);
		break;
	}
	return c;
}

int main_loop()
{
	WINDOW *win;

	current_menu = MAIN_MENU;
	current_win = MAIN_WIN;
	print_updated_wins();
	highlight_win_border(get_bwin(current_win));
	doupdate();
	while (true) {
		win = get_win(current_win);
		keypad(win, TRUE);
		int ret = cgetch(win, true);
		error_check(ret);
		if (ret == KEY_RESIZE)
			return KEY_RESIZE;
		error_check(print_updated_wins());
		doupdate();
		keypad(win, FALSE);
	}
	return SUCCESS;
}

int main()
{
	setup_config_location();

	LOG = fopen(MVM_LOG_PATH, "a+");
	if (LOG == NULL) {
		fprintf(stderr, "\e[0;31mFailed to open log file. It is advised the user investigate if logging is preferred.\n\e[0m");
		return 0;
	}

	if (sodium_init() < 0) {
		fprintf(stderr, "\e[0;31mSodium library failed to initialize. It is not safe to use the program currently.\n\e[0m");
		file_log("ERROR", "Sodium library failed to initialize. It is not safe to use the program currently.", NULL);
		return 0;
	}

	start_up();

	int resizes = 0;
	while (main_loop() == KEY_RESIZE) {
		small_clean_up();
		small_start_up();
		resizes++;
		if (resizes > 10) {
			file_log("TROLL", "I'm tired boss...", NULL);
			resizes = 0;
		}
	}

	clean_up();
	return 0;
}



