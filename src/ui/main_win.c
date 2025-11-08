#include "main_win.h"

static const char *main_choices[] = {
	"View Current VPN Connection",
	"Disconnect From VPN",
	"Connect To VPN"
};
static const int no_choices = sizeof(main_choices) / sizeof(char *);

void build_main_win(WINDOW *win, int highlight)
{
	wclear(win);
	for (int i = 0; i < no_choices; i++) {
		if (i != highlight) {
			mvwprintw(win, 2 + i, 2, main_choices[i]);
		} else {
			wattron(win, A_REVERSE);
			mvwprintw(win, 2 + i, 5, main_choices[i]);
			wattroff(win, A_REVERSE);
		}
	}
	box(win, 0, 0);
	wrefresh(win);
}

