#include <stdlib.h>
#include <string.h>
#include "window_helpers.h"
#include "globals.h"
#include "main_menu.h"
#include "main_win.h"
#include "side_win.h"
#include "scroller.h"

int print_updated_wins()
{
	switch (current_win) {
	case MAIN_WIN:
		if (get_max_entries(MAIN_WIN) == 0) {
			werase(get_win(current_win));
			print_main_menu();
		}
		print_win(MAIN_WIN);
		break;
	case SIDE_WIN:
		print_win(SIDE_WIN);
		break;
	default:
		if (get_max_entries(MAIN_WIN) == 0)
			print_main_menu();
		print_win(MAIN_WIN);
		current_win = MAIN_WIN;
		break;
	}
	return SUCCESS;
}

int print_win(int win_n)
{
	WINDOW *win = get_win(win_n);
	struct _win_scroller *scroller = get_win_scroller(win_n);
	int max_entries = get_max_entries(win_n);
	if (max_entries == 0) {
		scroller->top = 0;
		scroller->highlighted = 0;
		scroller->size = 0;
		werase(win);
		return SUCCESS;
	}
	if (scroller->size == 0) {
		scroller->top = 0;
		scroller->highlighted = 0;
		scroller->size = max_entries;
		print_new_win(win_n);
		cmd_win_print(false, 0, "%d (%d)",
			scroller->size - 1,
			scroller->highlighted,
			NULL);
		return SUCCESS;
	}
	int diff = scroller->prev_highlighted - scroller->highlighted;
	if (diff == 1) {
		scroll_down(win_n);
	} else if (diff == -1) {
		scroll_up(win_n);
	} else if (diff != 0) {
		if (scroller->highlighted == 0) {
			scroller->top = 0;
		} else if (scroller->highlighted == max_entries - 1) {
			int val = ((max_entries) - (getmaxy(win) - 3));
			scroller->top = val < 0 ? 0 : val;
			scroller->highlighted = max_entries - 1;
		} else {
			int tmp = ((max_entries) - (getmaxy(win) - 3));
			int val = tmp > scroller->highlighted && tmp >= 0 ?
				scroller->highlighted :
				tmp;
			scroller->top = val < 0 ? 0 : val;
		}
		print_new_win(win_n);
	} else {
		scroller->size = max_entries;
		print_new_win(win_n);
	}
	scroller->size = max_entries;
	if (win_n == SIDE_WIN) {
		update_header();
		wnoutrefresh(win_manager->side_win_header->inner);
	}
	if (diff == -1 || diff == 1) {
		wnoutrefresh(win);
	}
	cmd_win_print(false, 0, "%d (%d)",
		scroller->size - 1,
		scroller->highlighted,
		NULL);
	return SUCCESS;
}

void print_new_win(int win_n)
{
	WINDOW *win = get_win(win_n);
	struct _win_scroller *scroller = get_win_scroller(win_n);
	werase(win);

	int attr = SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE);
	int top = scroller->top;
	int max_entries = get_max_entries(win_n);

	for (int i = 0;
		top + i < max_entries &&
		i < getmaxy(win); i++) {
		int col = 1;
		if (top + i == scroller->highlighted) {
			if (win_n == MAIN_WIN)
				col += 2;
			wattron(win, attr);
		}
		char *entry;
		get_win_entry(win_n, &entry, top + i);
		mvwprintw(win, i, col, entry);
		wattroff(win, attr);
		free(entry);
	}
	wnoutrefresh(win);
	scroller->size = max_entries;
	if (win_n == SIDE_WIN)
		create_header();
	cmd_win_print(false, 0, "%d (%d)",
		max_entries - 1,
		scroller->highlighted,
		NULL);
}

struct _bordered_win *get_bwin(int bwin)
{
	switch (bwin) {
	case LOGO_WIN:
		return win_manager->logo_win;
	case MAIN_WIN:
		return win_manager->main_win;
	case SIDE_WIN:
		return win_manager->side_win_body;
	case CMD_WIN:
		return win_manager->cmd_win;
	default:
		return win_manager->main_win;
	}
}

WINDOW *get_win(int win)
{
	switch (win) {
	case LOGO_WIN:
		return win_manager->logo_win->inner;
	case MAIN_WIN:
		return win_manager->main_win->inner;
	case SIDE_WIN:
		return win_manager->side_win_body->inner;
	case CMD_WIN:
		return win_manager->cmd_win->inner;
	default:
		return win_manager->main_win->inner;
	}
}


int get_max_entries(int win_n)
{
	switch(win_n) {
	case MAIN_WIN:
		return n_main_win_entries;
	case SIDE_WIN:
		return n_side_win_entries;
	default:
		return 0;
	}
	return 0;
}

struct _win_scroller *get_win_scroller(int win_n)
{
	switch(win_n) {
	case MAIN_WIN:
		return main_win_scroller;
	case SIDE_WIN:
		return side_win_scroller;
	default:
		return NULL;
	}
}

int get_win_entry(int win_n, char **entry, int index)
{
	switch (win_n) {
	case MAIN_WIN:
		return get_main_win_entry(entry, index);
	case SIDE_WIN:
		return get_side_win_entry(entry, index);
	default:
		return 0;
	}
}

void reset_win(WINDOW *w)
{
	werase(w);
	wmove(w, 0, 0);
	wnoutrefresh(w);
}

void delete_win(WINDOW *local_win)
{
	if (local_win == NULL)
		return;
	wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wnoutrefresh(local_win);
	delwin(local_win);
}

void delete_bordered_win(struct _bordered_win *bw)
{
	if (bw == NULL)
		return;
	delete_win(bw->inner);
	delete_win(bw->border);
	free(bw);
	bw = NULL;
}

void delete_windows()
{
	delete_bordered_win(win_manager->logo_win);

	delete_bordered_win(win_manager->main_win);

	delete_bordered_win(win_manager->side_win_body);
	delete_bordered_win(win_manager->side_win_header);
	delete_bordered_win(win_manager->side_win);

	delete_bordered_win(win_manager->cmd_win);
}

void dim_win_border(struct _bordered_win *bwin)
{
	WINDOW *b = bwin->border;
	wattron(b, A_NORMAL);
	box(b, 0, 0);
	wattroff(b, A_NORMAL);
	wnoutrefresh(b);
}

void highlight_win_border(struct _bordered_win *bwin)
{
	WINDOW *b = bwin->border;
	wattron(b, SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE));
	box(b, 0, 0);
	wattroff(b, SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE));
	wnoutrefresh(b);
}

void cmd_win_print(bool update, int attr, const char *fmt, ...)
{
	WINDOW *win = win_manager->cmd_win->inner;
	int maxx = getmaxx(win);
	char *buffer = malloc(sizeof(char) * maxx);
	char *cur = malloc(sizeof(char) * maxx);

	va_list args;
	va_start(args, fmt);

	vsnprintf(buffer, maxx, fmt, args);

	va_end(args);

	mvwinstr(win, 0, 1, cur);
	if (strncmp(buffer, cur, maxx) == 0) {
		free(buffer);
		buffer = NULL;
		free(cur);
		cur = NULL;
	}

	werase(win);
	wattron(win, A_BLINK | attr);
	mvwprintw(win, 0, 1, "%s", buffer);
	wattroff(win, A_BLINK | attr);
	free(buffer);
	buffer = NULL;
	free(cur);
	cur = NULL;
	wnoutrefresh(win);
	if (update)
		doupdate();
}
