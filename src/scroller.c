#include <ncurses.h>
#include <stdlib.h>
#include "scroller.h"
#include "side_win.h"
#include "main_win.h"
#include "globals.h"
#include "window_helpers.h"

void scroll_one(int win_n)
{
	WINDOW *win = get_win(win_n);
	struct _win_scroller *scroller = get_win_scroller(win_n);
	int attr = SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE);
	int old_index = scroller->prev_highlighted;
	int new_index = scroller->highlighted;
	int top = scroller->top;
	int col = 1;

	wmove(win, old_index - top, 0);
	wclrtoeol(win);
	char *entry;
	get_win_entry(win_n, &entry, old_index);
	mvwprintw(win, old_index - top, col, entry);
	free(entry);

	wmove(win, new_index - top, 0);
	wclrtoeol(win);
	wattron(win, attr);
	get_win_entry(win_n, &entry, new_index);
	if (win_n == MAIN_WIN)
		col += 2;
	mvwprintw(win, new_index - top, col, entry);
	free(entry);
	wattroff(win, attr);
	wnoutrefresh(win);
}

void scroll_up(int win_n)
{
	WINDOW *win = get_win(win_n);
	struct _win_scroller *scroller = get_win_scroller(win_n);
	int max_entries = get_max_entries(win_n);
	if (scroller->top < scroller->highlighted) {
		scroll_one(win_n);
	} else {
		if (scroller->top > 0) {
			scroller->top -= 1;
		} else {
			int val = ((max_entries - 1) - (getmaxy(win)));
			scroller->top = val < 0 ? 0 : val;
			scroller->prev_highlighted = scroller->highlighted;
			scroller->highlighted = max_entries - 1;
		}
		print_new_win(win_n);
	}
}

void scroll_down(int win_n)
{
	WINDOW *win = get_win(win_n);
	struct _win_scroller *scroller = get_win_scroller(win_n);
	int max_entries = get_max_entries(win_n);
	if (scroller->top + getmaxy(win) - 1 > scroller->highlighted) {
		scroll_one(win_n);
	} else {
		if (scroller->highlighted - scroller->top <= getmaxy(win)) {
			scroller->top += 1;
		} else {
			scroller->top = 0;
			scroller->prev_highlighted = scroller->highlighted;
			scroller->highlighted = 0;
		}
		print_new_win(win_n);
	}
}

