#include <locale.h>
#include <stdlib.h>
#include <ncurses.h>
#include "helper.h"
#include "globals.h"

WINDOW *new_win(int height, int width, int starty, int startx)
{
	WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	box(local_win, 0, 0);
	wnoutrefresh(local_win);
	return local_win;
}

WINDOW *new_pad(int height, int width, int starty, int startx)
{
	WINDOW *local_pad;
	local_pad = newpad(height, width);
	box(local_pad, 0, 0);
	pnoutrefresh(local_pad, 0, 0, starty, startx,
		starty + height,
		startx + width);
	return local_pad;
}

void create_wins(struct _win_manager *win_manager)
{
	WINDOW *border;
	int main_wins_width;

	if (COLS >= 160) {
		main_wins_width = COLS/2;
		win_manager->side_win_border = new_win(LINES - 3,
			main_wins_width,
			0,
			COLS/2);
		border = win_manager->side_win_border;
		win_manager->side_win = derwin(border,
			getmaxy(border) - 2,
			getmaxx(border) - 2,
			1,
			1);
		side_win_exists = true;
	} else {
		main_wins_width = COLS;
	}
	win_manager->logo_win_border = new_win(LINES/2 - 3, main_wins_width, 0, 0);
	border = win_manager->logo_win_border;
	win_manager->logo_win = derwin(border,
		getmaxy(border) - 2,
		getmaxx(border) - 2,
		1,
		1);
	win_manager->main_win_border = new_win(LINES/2, main_wins_width, LINES/2 - 3, 0);
	border = win_manager->main_win_border;
	win_manager->main_win = derwin(border,
		getmaxy(border) - 2,
		getmaxx(border) - 2,
		1,
		1);
	win_manager->cmd_win_border = new_win(3, COLS, LINES - 3, 0);
	border = win_manager->cmd_win_border;
	win_manager->cmd_win = derwin(border,
		getmaxy(border) - 2,
		getmaxx(border) - 2,
		1,
		1);
}

void delete_win(WINDOW *local_win)
{
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wnoutrefresh(local_win);
	delwin(local_win);
}

void initialize_globals()
{
	int max_len = sizeof(char) * MAX_IDENTIFIER_LEN;
	profile_identifer = malloc(max_len);
	for (int i = 0; i < 1000; i++) {
		profile_configs_identifiers[i] = malloc(max_len);
		profile_configs_server_codes[i] = malloc(max_len);
	}

	side_win_scroller = malloc(sizeof(int) * 3);
}

void setup_ncurses()
{
	setlocale(LC_ALL, "");
	initscr();
	start_color();
	cbreak();
	noecho();

	if (can_change_color()) {
		init_color(COLOR_YELLOW, (255 * 1000) / 255, (236 * 1000) / 255, (10 * 1000) / 255);
		init_color(COLOR_BLUE, (41 * 1000) / 255, (77 * 1000) / 255, (115 * 1000) / 255);
	}
	init_pair(PAIR_ERROR, COLOR_RED, COLOR_BLACK);
	init_pair(PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	init_pair(PAIR_BLUE, COLOR_BLUE, COLOR_BLACK);
	init_pair(PAIR_YELLOW_BLUE, COLOR_YELLOW, COLOR_BLUE);
	curs_set(0);
}

void reset(WINDOW *win)
{
	wclear(win);
	wmove(win, 0, 0);
	wnoutrefresh(win);
}

WINDOW *get_win(struct _win_manager *win_manager, int win)
{
	switch (win) {
	case LOGO_WIN:
		return win_manager->logo_win;
	case MAIN_WIN:
		return win_manager->main_win;
	case SIDE_WIN:
		return win_manager->side_win;
	case CMD_WIN:
		return win_manager->cmd_win;
	default:
		return win_manager->main_win;
	}
}
