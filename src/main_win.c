#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "main_win.h"
#include "window_helpers.h"
#include "helpers.h"
#include "init.h"

char *main_win_entries[1000];
int n_main_win_entries;
struct _win_scroller *main_win_scroller;

void clear_main_win_entries()
{
	if (get_max_entries(MAIN_WIN) == 0)
		return;
	clear_1d_entries(main_win_entries, n_main_win_entries);
	main_win_scroller->size = 0;
	main_win_scroller->top = 0;
	main_win_scroller->highlighted = 0;
	n_main_win_entries = 0;
}

int get_main_win_entry(char **entry, int index)
{
	if (get_max_entries(MAIN_WIN) <= 0)
		return NO_WIN_ENTRY_FOUND;
	int len = strlen(main_win_entries[index]);
	*entry = malloc(len + 1);
	if (!(*entry)) {
		file_log("[ERROR] - Malloc for copying main window entry failed.", NULL);
		return MALLOC_FAILED;
	}
	memcpy(*entry, main_win_entries[index], len + 1);
	return SUCCESS;
}

void add_main_win_entry(char *entry)
{
	if (get_max_entries(MAIN_WIN) >= 1000) {
		file_log("[ERROR] - Failed to add main window entry \"%s\" (too many entries)",
			entry,
			NULL);
		return;
	}
	int len = strlen(entry);
	main_win_entries[n_main_win_entries] = malloc(len + 1);
	if (!main_win_entries[n_main_win_entries]) {
		file_log("[ERROR] - Failed to add main window entry \"%s\" (at the malloc stage)",
			entry,
			NULL);
		exit_program(MALLOC_FAILED);
	}
	memcpy(main_win_entries[n_main_win_entries], entry, len + 1);
	n_main_win_entries++;
	main_win_scroller->size++;
}

