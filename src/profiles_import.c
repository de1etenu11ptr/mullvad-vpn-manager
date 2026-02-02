#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "profiles_import.h"
#include "helpers.h"
#include "globals.h"
#include "profiles.h"
#include "side_win.h"
#include "window_helpers.h"
#include "encryption.h"

void profiles_import()
{
	WINDOW *win = win_manager->main_win->inner;
	char directory[DIRECTORY_LIMIT];
	memset(directory, '\0', DIRECTORY_LIMIT);
	int col = 1;

	wclear(win);
	wmove(win, 0, 0);
	mvwprintw(win, getcury(win), col,
		"Please enter the full path name to the folder containing the Mullvad WireGuard Configuration files:");
	wmove(win, getcury(win) + 1, col);

	echo();
	wgetnstr(win, directory, DIRECTORY_LIMIT);
	noecho();

	if (directory[DIRECTORY_LIMIT - 1] != '\0' &&
		directory[DIRECTORY_LIMIT - 1] != '\n' &&
		directory[DIRECTORY_LIMIT - 1] != '\r' &&
		directory[DIRECTORY_LIMIT - 1] != EOF) {
		cmd_win_print(false, SAFE_COLOR_PAIR(PAIR_ERROR),
			"The folder is too deeply nested (or the name is simply too long. Please consider moving it to a more easily accessible location.",
			NULL);
		file_log("ERROR", "The folder provided to import from is too deeply nested.",
			NULL);
	} else {
		file_log("IMPORT", "Importing profile configurations from the directory \"%s\"",
			directory,
			NULL);
		open_directory(directory);
	}

	wattron(win, A_BLINK | SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE));
	werase(win);
	mvwprintw(win, getcury(win), col, "Click any character to continue.");
	wmove(win, getcury(win) + 1, 0);
	wattroff(win, A_BLINK | SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE));
	print_win(SIDE_WIN);
	doupdate();
	wgetch(win);
}

int open_directory(char *directory)
{
	char cpy[DIRECTORY_LIMIT];
	DIR *dir = opendir(directory);
	struct dirent *dir_entry;
	if (dir == NULL) {
		cmd_win_print(false, SAFE_COLOR_PAIR(PAIR_ERROR),
			"Failed to open directory. Please ensure you gave the correct path. (\"%s\")",
			directory,
			NULL);
		file_log("ERROR", "Failed to open directory given to import. (\"%s\")",
			directory,
			NULL);
		return PROFILE_IMPORT_FOLDER_ERROR;
	}
	errno = 0;
	dir_entry = readdir(dir);
	WINDOW *side_win = win_manager->side_win_body->inner;
	clear_side_win_entries();
	reset_win(side_win);
	clear_profile();
	for (int j = 0; dir_entry != NULL; dir_entry = readdir(dir)) {
		if (strcmp(dir_entry->d_name, "") != 0 &&
			strcmp(dir_entry->d_name, ".") != 0 &&
			strcmp(dir_entry->d_name, "..") != 0)
		{
			memcpy(cpy, directory, DIRECTORY_LIMIT);
			if (read_profile_config(cpy, dir_entry) != SUCCESS) {
				clear_side_win_entries();
				goto end;
			}
			display_profile_config(profile->configs[j]);
			j++;
		}
	}
	side_win_scroller->size = get_max_entries(SIDE_WIN);
	side_win_scroller->top = 0;
	side_win_scroller->highlighted = 0;
	if (encrypt_profile() == SUCCESS) {
		file_log("IMPORT", "Imported profile: \"%s\"",
			profile->device_name,
			NULL);
	} else {
		clear_side_win_entries();
		file_log("IMPORT", "Failed to import profile: \"%s\"",
			profile->device_name,
			NULL);
	}
	end:;
	clear_profile();
	closedir(dir);
	return SUCCESS;
}

int read_profile_config(char *dir, struct dirent *dir_entry)
{
	char *line = malloc(sizeof(char) * LINE_LIMIT);
	int dir_len = strlen(dir);
	int file_len = _D_EXACT_NAMLEN(dir_entry);

	char full_path[dir_len + file_len + 2];
	strncpy(full_path, dir, dir_len + 1);
	strcat(full_path, "/");
	strncat(full_path, dir_entry->d_name, file_len + 1);

	FILE *file = fopen(full_path, "r");
	if (file == NULL) {
		cmd_win_print(false, SAFE_COLOR_PAIR(PAIR_ERROR),
			"Failed to read file: \"%s\"",
			full_path,
			NULL);
		file_log("ERROR", "Failed to read file: \"%s\"", full_path, NULL);
		return FILE_OPEN_ERROR;
	}

	if (profile->configs[profile->n_configs] == NULL)
		profile->configs[profile->n_configs] = malloc(sizeof(struct _profile_config));

	char *stop_ptr = strchr(dir_entry->d_name, '.');
	int len = stop_ptr - dir_entry->d_name;
	char filename[len + 1];
	strncpy(filename, dir_entry->d_name, len);
	filename[len] = '\0';

	struct _profile_config *config = profile->configs[profile->n_configs];
	strncpy(config->server_code, filename, len + 1);
	while (!feof(file)) {
		memset(line, '\0', LINE_LIMIT);
		read_line(file, line, LINE_LIMIT);
		int ret = handle_line(line, config);
		if (ret != SUCCESS) {
			file_log("ERROR", "Failed to read file likely due to formatting: \"%s\"",
				full_path,
				NULL);
			return FILE_FORMAT_ERROR;
		}
		memset(line, '\0', LINE_LIMIT);
	}
	profile->n_configs++;

	free(line);
	line = NULL;
	fclose(file);
	return SUCCESS;
}

void display_profile_config(struct _profile_config *profile_config)
{
	// Don't have more than SIDE_WIN_COLS elements...
	char *cols[] = {
		profile_config->server_code,
		profile_config->dns,
		profile_config->ipv4_endpoint,
		profile_config->port
	};
	char *headers[] = {
		"Server Code",
		"DNS",
		"Endpoint",
		"Port"
	};
	add_side_win_entry(profile->device_name, headers, cols);
}

int handle_line(char *line, struct _profile_config *config)
{
	int len = 0;
	char *tmpline;
	char **tmp = NULL;
	if (
		strncmp(line, "# Device:", 9) == 0 &&
		(len = strlen((tmpline = line + 10))) < VPN_MAX_NAME_SIZE - 1)
	{
		strncpy(profile->device_name, tmpline, VPN_MAX_NAME_SIZE);
		profile->device_name[VPN_MAX_NAME_SIZE - 1] = '\0';
	} else if (
		strncmp(line, "PrivateKey ", 11) == 0 &&
		(len = strlen((tmpline = line + 13))) < VPN_KEY_SIZE - 1)
	{
		strncpy(profile->private_key, tmpline, VPN_KEY_SIZE);
		profile->private_key[VPN_KEY_SIZE - 1] = '\0';
	} else if (
		strncmp(line, "Address ", 8) == 0 &&
		(len = strlen((tmpline = line + 10))) < IPV4_ADDRESS_SIZE + IPV6_ADDRESS_SIZE + 1)
	{
		tmp = create_array_from_str(tmpline, ',', 2, len);
		strncpy(config->ipv4_address, tmp[0], IPV4_ADDRESS_SIZE);
		config->ipv4_address[IPV4_ADDRESS_SIZE - 1] = '\0';
		strncpy(config->ipv6_address, tmp[1], IPV6_ADDRESS_SIZE);
		config->ipv6_address[IPV6_ADDRESS_SIZE - 1] = '\0';
		clear_sensitive_1d_entries(tmp, 2);
		free(tmp);
		tmp = NULL;
	} else if (
		strncmp(line, "DNS ", 4) == 0 &&
		(len = strlen((tmpline = line + 6))) < IPV4_ADDRESS_SIZE - 1)
	{
		strncpy(config->dns, tmpline, IPV4_ADDRESS_SIZE);
		config->dns[IPV4_ADDRESS_SIZE - 1] = '\0';
	} else if (
		strncmp(line, "PublicKey ", 10) == 0 &&
		(len = strlen((tmpline = line + 12))) < VPN_KEY_SIZE - 1)
	{
		strncpy(config->public_key, tmpline, VPN_KEY_SIZE);
		config->public_key[VPN_KEY_SIZE - 1] = '\0';
	} else if (
		strncmp(line, "AllowedIPs ", 11) == 0 &&
		(len = strlen((tmpline = line + 13))) < IPV4_ADDRESS_SIZE + IPV6_ADDRESS_SIZE + 1)
	{
		tmp = create_array_from_str(tmpline, ',', 2, len);
		strncpy(config->ipv4_allowed_ips, tmp[0], IPV4_ADDRESS_SIZE);
		config->ipv4_allowed_ips[IPV4_ADDRESS_SIZE - 1] = '\0';
		strncpy(config->ipv6_allowed_ips, tmp[1], IPV6_ADDRESS_SIZE);
		config->ipv6_allowed_ips[IPV6_ADDRESS_SIZE - 1] = '\0';
		clear_sensitive_1d_entries(tmp, 2);
		free(tmp);
		tmp = NULL;
	} else if (
		strncmp(line, "Endpoint ", 9) == 0 &&
		(len = strlen((tmpline = line + 11))) < IPV4_ADDRESS_SIZE + MAX_PORT_SIZE + 1)
	{
		tmp = create_array_from_str(tmpline, ':', 2, len);
		strncpy(config->ipv4_endpoint, tmp[0], IPV4_ADDRESS_SIZE);
		config->ipv4_endpoint[IPV4_ADDRESS_SIZE - 1] = '\0';
		strncpy(config->port, tmp[1], MAX_PORT_SIZE);
		config->port[MAX_PORT_SIZE - 1] = '\0';
		clear_sensitive_1d_entries(tmp, 2);
		free(tmp);
		tmp = NULL;
	}
	return SUCCESS;
}


