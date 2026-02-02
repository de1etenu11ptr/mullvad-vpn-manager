#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include "window_helpers.h"
#include "helpers.h"
#include "globals.h"
#include "profiles.h"
#include "logo.h"
#include "init.h"

void file_log(const char *type, const char *fmt, ...)
{
	if (LOG == NULL)
		return;
	va_list args;
	va_start(args, fmt);

	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	char time_str[64];
	strftime(time_str, sizeof(time_str),
		"%Y-%m-%d %H:%M:%S",
		t);
	fprintf(LOG, "[%s] - [%s] - ", time_str, type);

	vfprintf(LOG, fmt, args);
	fputc('\n', LOG);

	fflush(LOG);

	va_end(args);
}

void create_str(char **final_str, int count, ...) {
	unsigned int final_str_size = 1;
	va_list args;
	va_start(args, count);

	for (int i = 0; i < count; i++) {
		char *str = va_arg(args, char *);
		final_str_size += strlen(str);
	}

	*final_str = malloc(final_str_size);
	memset(*final_str, '\0', final_str_size);
	va_end(args);

	va_start(args, count);

	for (int i = 0; i < count; i++) {
		char *str = va_arg(args, char *);
		strcat(*final_str, str);
	}

	va_end(args);
}

int create_str_array(char ***array, char *str, char delim) {
	int max_len = 0, no_strs = 1;
	int full_len = strlen(str);
	for (int i = 0, tmp = 0; i < full_len; i++) {
		if (str[i] == delim) {
			if (tmp > max_len)
				max_len = tmp;
			tmp = 0;
			no_strs++;
			continue;
		}
		tmp++;
	}
	*array = malloc(sizeof(char *) * no_strs);
	for (int i = 0, j = 0; i < no_strs; i++) {
		(*array)[i] = malloc(sizeof(char) * (max_len + 1));
		memset((*array)[i], '\0', max_len + 1);
		while (str[j] != '\0' && str[j] != EOF) {
			if (str[j] == delim) {
				str = &(str[j+1]);
				j = 0;
				break;
			}
			(*array)[i][j] = str[j];
			j++;
		}
	}
	return no_strs;
}

FILE *open_file(char *file_path, char *type) {
	FILE *file = fopen(file_path, type);
	if (file == NULL) {
		cmd_win_print(false, SAFE_COLOR_PAIR(PAIR_ERROR),
			"Failed to read file: \"%s\"",
			file_path,
			NULL);
		file_log("ERROR", "Failed to read file: \"%s\"", file_path, NULL);
		return NULL;
	}
	return file;
}

int read_file(char **content, char *file_path) {
	FILE *file;
	if ((file = open_file(file_path, "r")) == NULL)
		return FILE_OPEN_ERROR;
	int size = 0;
	while (!feof(file)) {
		size++;
		fgetc(file);
	}
	fseek(file, 0, SEEK_SET);
	*content = malloc(size + 1);
	size = 0;
	while (!feof(file)) {
		*(*content + size) = fgetc(file);
		size++;
	}
	fclose(file);
	return SUCCESS;
}

void exit_program(int err)
{
	clean_up();
	exit(err);
}

void small_clean_up()
{
	delete_windows();
	clear();
}

void small_start_up()
{
	create_windows();
	if (win_manager->logo_win != NULL)
		add_logo(win_manager->logo_win->inner);
	if (get_max_entries(SIDE_WIN) > 0)
		print_new_win(SIDE_WIN);
	print_new_win(MAIN_WIN);
	keypad(win_manager->main_win->inner, TRUE);
	refresh();
}


int read_line(FILE *stream, char *buffer, int buffer_size)
{
	char c;
	int i = 0;

	c = fgetc(stream);
	while (c == '\0' ||
		c == ' ' ||
		c == '\t' ||
		c == '\n' ||
		c == '\r') {
		c = fgetc(stream);
		continue;
	}
	if (c == EOF) {
		buffer[0] = '\0';
		return EOF;
	}
	ungetc(c, stream);

	while ((c = fgetc(stream)) != EOF &&
		c != '\0' &&
		c != '\n' &&
		c != '\r' && i < buffer_size) {
		if (i >= buffer_size - 1) {
			buffer[buffer_size - 1] = '\0';
			return BUFFER_SIZE_ERROR;
		}
		buffer[i] = c;
		i++;
	}

	buffer[buffer_size - 1] = '\0';
	return SUCCESS;

}

void clear_sensitive_2d_entries(char ***entries, int n_entries, int n_cols)
{

	for (int i = 0; i < n_entries; i++) {
		for (int j = 0; j < n_cols; j++) {
			int len = strlen(entries[i][j]);
			memset(entries[i][j], '\0', len);
			free(entries[i][j]);
			entries[i][j] = NULL;
		}
	}
}

void clear_sensitive_1d_entries(char **entries, int n_entries)
{
	for (int i = 0; i < n_entries; i++) {
		int len = strlen(entries[i]);
		memset(entries[i], '\0', len);
		free(entries[i]);
		entries[i] = NULL;
	}
}

void clear_2d_entries(char ***entries, int n_entries, int n_cols)
{
	for (int i = 0; i < n_entries; i++) {
		for (int j = 0; j < n_cols; j++) {
			free(entries[i][j]);
			entries[i][j] = NULL;
		}
	}
}

void clear_1d_entries(char **entries, int n_entries)
{
	for (int i = 0; i < n_entries; i++) {
		free(entries[i]);
		entries[i] = NULL;
	}
}

int error_check(int ret)
{
	return ret;
}
