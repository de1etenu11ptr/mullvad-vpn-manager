#include <string.h>
#include <stdlib.h>
#include "side_win.h"
#include "helpers.h"
#include "globals.h"
#include "scroller.h"
#include "window_helpers.h"

char *TITLE;
char *HEADERS[SIDE_WIN_COLS];
char *side_win_entries[1000][SIDE_WIN_COLS];
int n_side_win_entries;
int col_length;
struct _win_scroller *side_win_scroller;

void clear_side_win_entries()
{
	if (n_side_win_entries == 0)
		return;
	if (n_side_win_entries > 0) {
		free(TITLE);
		TITLE = NULL;
	}
	for (int j = 0; j < SIDE_WIN_COLS; j++) {
		free(HEADERS[j]);
		HEADERS[j] = NULL;
	}
	for (int i = 0; i < get_max_entries(SIDE_WIN); i++) {
		for (int j = 0; j < SIDE_WIN_COLS; j++) {
			free(side_win_entries[i][j]);
			side_win_entries[i][j] = NULL;
		}
	}
	n_side_win_entries = 0;
	side_win_scroller->size = 0;
	side_win_scroller->top = 0;
	side_win_scroller->highlighted = 0;
}

void add_side_win_entry(char *title, char **headers, char **cols)
{
	if (TITLE == NULL) {
		TITLE = malloc(sizeof(char) * strlen(title));
	}
	memset(TITLE, '\0', strlen(TITLE));
	strcat(TITLE, title);
	if (get_max_entries(SIDE_WIN) >= 1000)
		return;
	if (get_max_entries(SIDE_WIN) == 0) {
		for (int i = 0; i < SIDE_WIN_COLS; i++) {
			if (HEADERS[i] == NULL)
				HEADERS[i] = malloc(sizeof(char) * col_length);
			strcpy(HEADERS[i], headers[i]);
		}
	}
	memset(side_win_entries[get_max_entries(SIDE_WIN)], '\0', col_length * SIDE_WIN_COLS);
	for (int i = 0; i < SIDE_WIN_COLS; i++) {
		if (side_win_entries[get_max_entries(SIDE_WIN)][i] == NULL)
			side_win_entries[get_max_entries(SIDE_WIN)][i] = malloc(sizeof(char) * col_length);
		strcpy(side_win_entries[get_max_entries(SIDE_WIN)][i], cols[i]);
	}
	n_side_win_entries++;
	side_win_scroller->size++;
}

void update_header()
{
	int attr = SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE);
	WINDOW *header_win = win_manager->side_win_header->inner;
	wmove(header_win, 0, 0);
	wclrtoeol(header_win);
	wattron(header_win, attr);
	mvwprintw(header_win, 0, (getmaxx(header_win) - strlen(TITLE))/2, TITLE);
	wattroff(header_win, attr);
	mvwprintw(header_win, 0, 0, "%d (%d)",
		get_max_entries(SIDE_WIN) - 1, side_win_scroller->highlighted);
	wnoutrefresh(header_win);
}

void create_header()
{
	int attr = SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE);
	WINDOW *header_win = win_manager->side_win_header->inner;
	werase(header_win);
	wattron(header_win, attr);
	mvwprintw(header_win, 0, (getmaxx(header_win) - strlen(TITLE))/2, TITLE);
	wattroff(header_win, attr);
	mvwprintw(header_win, 0, 0, "%d (%d)", get_max_entries(SIDE_WIN) - 1, side_win_scroller->highlighted);
	for (int j = 0; j < SIDE_WIN_COLS; j++) {
		mvwprintw(header_win, 1, col_length * j, HEADERS[j]);
	}
	wnoutrefresh(header_win);
}

int get_side_win_entry(char **entry, int index)
{
	if (get_max_entries(SIDE_WIN) <= 0)
		return NO_WIN_ENTRY_FOUND;
	*entry = malloc(COLS + 1);
	if (!(*entry)) {
		file_log("[ERROR] - Malloc for copying side window entry failed.", NULL);
		return MALLOC_FAILED;
	}
	int tracker = 0;
	for (int j = 0; j < SIDE_WIN_COLS; j++) {
		snprintf(*(entry) + tracker, COLS, "%-*.*s|",
			col_length - 1,
			col_length - 1,
			side_win_entries[index][j]);
		tracker += col_length;
	}
	(*entry)[COLS] = '\0';
	return tracker;
}


