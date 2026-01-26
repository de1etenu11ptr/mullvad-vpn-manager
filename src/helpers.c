#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include "helpers.h"
#include "globals.h"
#include "profiles.h"
#include "logo.h"
#include "init.h"

int concatstrs(char *buffer, int buffer_size, ...)
{
	memset(buffer, '\0', buffer_size);
	char *str;
	int buffer_size_rem = buffer_size;
	va_list args;
	va_start(args, buffer_size);
	while ((str = va_arg(args, char *)) != NULL) {
		int len = strlen(str);
		strncat(buffer, str, buffer_size_rem);
		if (buffer_size_rem == 0 || buffer_size_rem <= len) {
			buffer[buffer_size - 1] = '\0';
			return BUFFER_SIZE_ERROR;
		}
		buffer_size_rem -= len;
	}
	buffer[buffer_size - 1] = '\0';
	va_end(args);
	return SUCCESS;
}

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
