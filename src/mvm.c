#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include "general/globals.h"
#include "general/helper.h"
#include "core/key_manager.h"
#include "ui/logo/logo.h"
#include "ui/main_menu.h"
#include "ui/side_win_menu.h"
#include "ui/profile_import_menu.h"

struct _win_manager *win_manager;
bool cmd_mode = false;
bool side_win_exists = false;
int current_win;
static int current_menu;
static int highlighted_choice;
static int n_choices;

void clean_up();
void main_loop();
void handle_choice(int choice);
void print_updated_menu();
void print_typed_chr(int c);

int main()
{
	if (sodium_init() < 0) {
		printf("Sodium library failed to initialize. It is not safe to use the program currently.");
		return 0;
	}
	win_manager = malloc(sizeof(WINDOW *) * 4 * 2);
	setup_ncurses();
	refresh();
	initialize_globals();

	create_wins(win_manager);
	doupdate();

	add_logo(win_manager->logo_win);
	keypad(win_manager->main_win, TRUE);

	main_loop();

	clean_up();
	return 0;
}


void clean_up()
{
	delete_win(win_manager->logo_win);
	delete_win(win_manager->main_win);
	delete_win(win_manager->side_win);
	delete_win(win_manager->cmd_win);
	free(win_manager);
	endwin();
}

void main_loop()
{
	int c;
	WINDOW *win;

	current_menu = MAIN_MENU;
	current_win = MAIN_WIN;
	highlighted_choice = 0;
	print_updated_menu();
	doupdate();
	while (true) {
		win = get_win(win_manager, current_win);
		keypad(win, TRUE);
		c = wgetch(win);
		print_typed_chr(c);
		switch (c) {
		case (int) 'k':
		case KEY_UP:
			if (highlighted_choice <= 0)
				highlighted_choice = n_choices - 1;
			else
				highlighted_choice--;
			break;
		case (int) 'j':
		case KEY_DOWN:
			if (highlighted_choice >= n_choices - 1)
				highlighted_choice = 0;
			else
				highlighted_choice++;
			break;
		case 10: // Enter key.
			handle_choice(highlighted_choice);
			break;
		case 9: // Tab key.
			keypad(win, FALSE);
			if (current_win == MAIN_WIN && side_win_scroller->size > 0) {
				current_win = SIDE_WIN;
				highlighted_choice = side_win_scroller->highlighted;
			} else {
				current_win = MAIN_WIN;
				highlighted_choice = 0;
			}
			break;
		case (int) 'q':
			goto end;
		}
		print_updated_menu();
		doupdate();
	}
	end:
	return;
}

void handle_choice(int choice)
{
	if (current_win == SIDE_WIN)
		return;
	switch (current_menu) {
	case MAIN_MENU:
		current_menu = handle_main_menu_choice(choice);
		highlighted_choice = 0;
		break;
	case PROFILE_IMPORT_MENU:
		current_menu = PROFILE_IMPORT_MENU;
		highlighted_choice = 0;
		break;
	}
}

void print_updated_menu()
{
	WINDOW *win = get_win(win_manager, current_win);
	reset(win);
	if (current_win == SIDE_WIN) {
		print_side_win_menu(win, highlighted_choice);
		n_choices = side_win_scroller->size;
		return;
	}
	switch (current_menu) {
	case MAIN_MENU:
		print_main_menu(win, highlighted_choice);
		n_choices = get_n_main_menu_choices();
		break;
	case PROFILE_IMPORT_MENU:
		print_profile_import_menu(win_manager);
		current_menu = MAIN_MENU;
		break;
	}
}

void print_typed_chr(int c)
{
	wmove(win_manager->cmd_win, 1, 0);
	wclrtoeol(win_manager->cmd_win);
	mvwprintw(win_manager->cmd_win, 1, 2, "%d", c);
	wnoutrefresh(win_manager->cmd_win);
}
