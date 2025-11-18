#include "logo.h"
#include "globals.h"

void add_shading(WINDOW *win, int max_win_row, int row, int col)
{
	for (int i = 1; i < 4; i++) {
		if (row <= max_win_row - i && col - i >= 0) {
			if (mvwinch(win, row, col - i) != ' ')
				continue;
			mvwaddwstr(win, row, col - i, SHADER_CHAR);
		}
	}
}

void add_logo(WINDOW *win)
{
	int attr;
	int row;
	int start_row;
	int start_col;
	int end_row;
	int end_col;
	int max_win_col;
	int max_win_row;
	int logo_height;
	int logo_width;

	werase(win);
	getmaxyx(win, max_win_row, max_win_col);

	logo_height = max_win_col / 5;
	logo_width = logo_height;
	start_row = (max_win_row - logo_height)/2;
	start_col = (max_win_col - logo_width)/2;
	end_row = start_row + logo_height;
	end_col = start_col + logo_width;

	mvwprintw(win, getmaxy(win) - 1, 2, "%dx%d", COLS, LINES);
	if (max_win_col > 60) {
		attr = A_UNDERLINE | SAFE_COLOR_PAIR(PAIR_YELLOW);
		wattron(win, attr);
		mvwprintw(win,
			start_row + logo_height/2 - 1,
			start_col + (logo_width - 3)/2,
			"MVM");
		wattroff(win, attr);
	}
	row = start_row;
	for (; row < start_row + logo_height; row++) {
		wattron(win, SAFE_COLOR_PAIR(PAIR_YELLOW));
		mvwaddwstr(win, row, start_col, FULL_BLOCK);
		mvwaddwstr(win, row, start_col + 1, FULL_BLOCK);

		mvwaddwstr(win, row, end_col, FULL_BLOCK);
		mvwaddwstr(win, row, end_col + 1, FULL_BLOCK);
		wattroff(win, SAFE_COLOR_PAIR(PAIR_YELLOW));

		wattron(win, SAFE_COLOR_PAIR(PAIR_BLUE));
		add_shading(win, end_row, row, start_col);
		add_shading(win, end_row, row, end_col);
		wattroff(win, SAFE_COLOR_PAIR(PAIR_BLUE));
	}
	row = start_row + logo_height/2;
	for (; row < start_row + logo_height; row++) {
		int val = (row - (start_row + logo_height/2) + 1);
		wattron(win, SAFE_COLOR_PAIR(PAIR_YELLOW));
		mvwaddwstr(win, row, start_col + val, FULL_BLOCK);
		mvwaddwstr(win, row, start_col + 1 + val, FULL_BLOCK);

		mvwaddwstr(win, row, end_col - val, FULL_BLOCK);
		mvwaddwstr(win, row, end_col + 1 - val, FULL_BLOCK);
		wattroff(win, SAFE_COLOR_PAIR(PAIR_YELLOW));

		wattron(win, SAFE_COLOR_PAIR(PAIR_BLUE));
		add_shading(win, end_row, row, start_col + val);
		add_shading(win, end_row, row, end_col - val);
		wattroff(win, SAFE_COLOR_PAIR(PAIR_BLUE));
	}

	wnoutrefresh(win);
}

