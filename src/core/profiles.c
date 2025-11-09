#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "../general/helper.h"
#include "../general/globals.h"
#include "profiles.h"
#include "../ui/profile_import_menu.h"

char *profile_identifer;
char *profile_configs_identifiers[1000];
char *profile_configs_server_codes[1000];
int n_profile_configs;

void display_file(WINDOW *side_win, int row, char *dir, char *filename) {
	// Being very generous in terms of memory allocation here.
	strncpy(profile_configs_identifiers[n_profile_configs],
		filename,
		MAX_IDENTIFIER_LEN);
	if (row < getmaxy(side_win)) {
		if (row == 0)
			wattron(side_win, COLOR_PAIR(PAIR_YELLOW_BLUE));
		mvwprintw(side_win, row, 2, "\"%s/%s\"",
			dir,
			profile_configs_identifiers[n_profile_configs]);
		if (row == 0)
			wattroff(side_win, COLOR_PAIR(PAIR_YELLOW_BLUE));
	}
	n_profile_configs++;
}

int open_directory(struct _win_manager *win_manager, char *directory)
{
	n_profile_configs = 0;
	DIR *dir = opendir(directory);
	struct dirent *dir_entry;
	if (dir == NULL) {
		wattron(win_manager->main_win, COLOR_PAIR(PAIR_ERROR));
		mvwprintw(win_manager->main_win,
			getcury(win_manager->main_win),
			2, "Failed to open directory. Please ensure you gave the correct path. (\"%s\")",
			directory);
		wmove(win_manager->main_win,
			getcury(win_manager->main_win) + 1, 0);
		wattroff(win_manager->main_win, COLOR_PAIR(PAIR_ERROR));
		return PROFILE_IMPORT_FOLDER_ERROR;
	}
	errno = 0;
	dir_entry = readdir(dir);
	WINDOW *side_win = win_manager->side_win;
	if (side_win_exists)
		reset(win_manager->side_win);
	for (int j = 0; dir_entry != NULL; dir_entry = readdir(dir)) {
		if (strcmp(dir_entry->d_name, "") != 0 &&
			strcmp(dir_entry->d_name, ".") != 0 &&
			strcmp(dir_entry->d_name, "..") != 0 &&
			side_win_exists)
		{
			display_file(side_win, j, directory, dir_entry->d_name);
			wmove(side_win, 1, 2);
			j++;
		}

	}
	if (side_win_exists) {
		wnoutrefresh(side_win);
		side_win_scroller->size = n_profile_configs;
		side_win_scroller->top = 0;
		side_win_scroller->highlighted = 0;
	}
	if (errno != 0) {
		wattron(win_manager->main_win, COLOR_PAIR(PAIR_ERROR));
		mvwprintw(win_manager->main_win,
			getcury(win_manager->main_win), 2, "READ ERROR");
		wmove(win_manager->main_win,
			getcury(win_manager->main_win) + 1, 0);
		wattroff(win_manager->main_win, COLOR_PAIR(PAIR_ERROR));
		return PROFILE_IMPORT_READ_ERROR;
	}
	closedir(dir);
	return PROFILE_IMPORT_SUCCESS;
}

void read_directory(struct _win_manager *win_manager)
{
	WINDOW *win = win_manager->main_win;
	char directory[4096];
	mvwprintw(win, getcury(win), 2,
		"Please enter the full path name to the folder containing the Mullvad WireGuard Configuration files:");
	wmove(win, getcury(win) + 1, 2);

	echo();
	wgetnstr(win, directory, 4096);
	noecho();

	if (directory[4095] != '\0' && directory[4095] != '\n' &&
		directory[4095] != '\r' && directory[4095] != EOF)
	{
		mvwprintw(win, getcury(win), 2,
			"The folder is too deeply nested. Please consider moving it to a more easily accessible location.");
		wmove(win, getcury(win) + 1, 0);
	} else {
		open_directory(win_manager, directory);
	}

	wattron(win, A_BLINK | COLOR_PAIR(PAIR_YELLOW_BLUE));
	mvwprintw(win, getcury(win), 2, "Click any character to continue.");
	wmove(win, getcury(win) + 1, 0);
	wattroff(win, A_BLINK | COLOR_PAIR(PAIR_YELLOW_BLUE));
	wnoutrefresh(win);
}

