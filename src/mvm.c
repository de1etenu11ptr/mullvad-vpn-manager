#include <locale.h>
#include "general/macros.h"
#include "general/helper.h"
#include "ui/logo/logo.h"
#include "ui/main_win.h"

WINDOW *logo_win;
WINDOW *main_win;
WINDOW *side_win;

void setup_ncurses();
void clean_up();

int main()
{
	setup_ncurses();
	refresh();

	// ISO/ANSI defined screen size is 80x24 (given as <characters per line>x<lines per window>).
	{
		int main_wins_width;
		if (COLS >= 160) {
			main_wins_width = COLS/2;
			side_win = new_win(LINES, main_wins_width, 0, COLS/2);
		} else {
			main_wins_width = COLS;
		}
		logo_win = new_win(LINES/2, main_wins_width, 0, 0);
		main_win = new_win(LINES/2, main_wins_width, LINES/2, 0);
	}
	add_logo(logo_win);
	build_main_win(main_win, 0);
	keypad(main_win, TRUE);
	refresh();

	getch();
	clean_up();
	return 0;
}

void setup_ncurses()
{
	setlocale(LC_ALL, "");
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	/*
	Provided the terminal supports the initc capability
	Check capability with has_colors() and can_change_color().
	Define the color using init_color():
	// Scale the RGB values (max 255) to the ncurses range (max 1000)
	int r = ; // 1000
	int g = (236 * 1000) / 255; // ~925
	int b = (10 * 1000) / 255;  // ~39
	// Define a new color (e.g., as color index 8, if available)
	init_color(8, r, g, b);
	*/
	if (can_change_color()) {
		init_color(COLOR_YELLOW, (255 * 1000) / 255, (236 * 1000) / 255, (10 * 1000) / 255);
		init_color(COLOR_BLUE, (41 * 1000) / 255, (77 * 1000) / 255, (115 * 1000) / 255);
	}
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	curs_set(0);
}

void clean_up()
{
	delete_win(logo_win);
	endwin();
}
