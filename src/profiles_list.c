#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "globals.h"
#include "helpers.h"
#include "profiles_list.h"
#include "main_win.h"
#include "main_menu.h"
#include "profiles.h"
#include "window_helpers.h"

int profiles_list()
{
	int directory_len = strlen(HOME_DIR) + strlen(_MVM_PROFILE_CONFIGS_DIR) + 1;
	char directory[directory_len + 1];
	memset(directory, '\0', directory_len + 1);

	memcpy(directory, HOME_DIR, directory_len);
	strcat(directory, "/");
	strcat(directory, _MVM_PROFILE_CONFIGS_DIR);

	directory[directory_len] = '\0';
	DIR *dir = opendir(directory);
	if (dir == NULL) {
		cmd_win_print(false, SAFE_COLOR_PAIR(PAIR_ERROR),
			"Failed to open directory. Please ensure you gave the correct path. (\"%s\")",
			directory,
			NULL);
		file_log("ERROR", "Failed to open directory given to import. (\"%s\")",
			directory,
			NULL);
		return PROFILES_LIST_FOLDER_ERROR;
	}
	struct dirent *dir_entry;
	dir_entry = readdir(dir);
	WINDOW *main_win = win_manager->main_win->inner;
	clear_main_win_entries();
	reset_win(main_win);
	clear_profile();
	int j = 0;
	int prefix_len = strlen(_MVM_PROFILE_CONFIG_PREFIX);
	char tmp[VPN_MAX_NAME_SIZE];
	for (; dir_entry != NULL; dir_entry = readdir(dir)) {
		if (strcmp(dir_entry->d_name, "") != 0 &&
			strcmp(dir_entry->d_name, ".") != 0 &&
			strcmp(dir_entry->d_name, "..") != 0 &&
			strncmp(_MVM_PROFILE_CONFIG_PREFIX, dir_entry->d_name, prefix_len) == 0)
		{
			memset(tmp, '\0', VPN_MAX_NAME_SIZE);
			char *end = memccpy(tmp, &dir_entry->d_name[prefix_len],
				']',
				VPN_MAX_NAME_SIZE);
			if (end == NULL) continue;
			memset(&tmp[strlen(tmp) - 1], '\0', 1);
			add_main_win_entry(tmp);
			j++;
		}
	}
	if (j == 0) {
		print_main_menu();
		return PROFILES_LIST_ERROR;
	}
	print_new_win(MAIN_WIN);
	doupdate();
	return SUCCESS;
}

int load_profile(int choice) {
	char *entry;
	char *profile_file_path;
	char *contents;
	if (get_main_win_entry(&entry, choice) != SUCCESS)
		return GENERIC_FAIL;
	create_str(&profile_file_path, 7,
		HOME_DIR,
		"/",
		_MVM_PROFILE_CONFIGS_DIR,
		"/",
		_MVM_PROFILE_CONFIG_KEYS_PREFIX,
		entry,
		_MVM_PROFILE_CONFIG_SUFFIX,
		NULL);
	read_file(&contents, profile_file_path);
	file_log("LOG", "Contents: %s", contents, NULL);
	free(entry);
	free(profile_file_path);
	free(contents);
	return SUCCESS;
}
