#include "logo.h"

void add_shading(WINDOW *win, int max_win_row, int row, int col)
{
	for (int i = 1; i < 4; i++) {
		if (row <= max_win_row - i && col - i >= 0) {
			if (mvwinch(win, row, col - i) == (unsigned int)*(FULLBLOCK))
				continue;
			mvwaddwstr(win, row, col - i, VERTBAR);
		}
	}
}

void add_logo(WINDOW *win)
{
	int attr;
	int start_row;
	int start_col;
	int end_row;
	int end_col;
	int max_win_col;
	int max_win_row;
	int logo_height;
	int logo_width;

	wclear(win);
	getmaxyx(win, max_win_row, max_win_col);

	logo_height = max_win_col / 5;
	logo_width = logo_height;
	start_row = (max_win_row - logo_height)/2;
	start_col = (max_win_col - logo_width)/2;
	end_row = start_row + logo_height;
	end_col = start_col + logo_width;

	mvwprintw(win, max_win_row - 2, 2, "%dx%d", max_win_col, max_win_row);
	if (max_win_col > 60) {
		attr = A_UNDERLINE | COLOR_PAIR(1);
		wattron(win, attr);
		mvwprintw(win, start_row + logo_height/2 - 1, start_col + (logo_width - 3)/2, "MVM");
		wattroff(win, attr);
	}

	for (int row = start_row; row < start_row + logo_height; row++) {
		wattron(win, COLOR_PAIR(1));
		mvwaddwstr(win, row, start_col, FULLBLOCK);
		mvwaddwstr(win, row, start_col + 1, FULLBLOCK);

		mvwaddwstr(win, row, end_col, FULLBLOCK);
		mvwaddwstr(win, row, end_col + 1, FULLBLOCK);
		wattroff(win, COLOR_PAIR(1));

		wattron(win, COLOR_PAIR(2));
		add_shading(win, end_row, row, start_col);
		add_shading(win, end_row, row, end_col);
		wattroff(win, COLOR_PAIR(2));
	}
	for (int row = start_row + logo_height/2; row < start_row + logo_height; row++) {
		int val = (row - (start_row + logo_height/2) + 1);
		wattron(win, COLOR_PAIR(1));
		mvwaddwstr(win, row, start_col + val, FULLBLOCK);
		mvwaddwstr(win, row, start_col + 1 + val, FULLBLOCK);

		mvwaddwstr(win, row, end_col - val, FULLBLOCK);
		mvwaddwstr(win, row, end_col + 1 - val, FULLBLOCK);
		wattroff(win, COLOR_PAIR(1));

		wattron(win, COLOR_PAIR(2));
		add_shading(win, end_row, row, start_col + val);
		add_shading(win, end_row, row, end_col - val);
		wattroff(win, COLOR_PAIR(2));
	}

	box(win, 0, 0);
	wrefresh(win);
}

