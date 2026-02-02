#ifndef CUSTOM_HELPERS
#define CUSTOM_HELPERS

#include <ncurses.h>
#include <stdarg.h>
#include <stdbool.h>
#include "globals.h"


struct _bordered_win *create_bordered_window(WINDOW *parent, int height, int width, int yoffset, int xoffset, int attr);
void bwrefresh(struct _bordered_win *bw);
void create_windows();
void delete_win(WINDOW *local_win);
void delete_bordered_win(struct _bordered_win *bw);
void delete_windows();
void initialize_globals();

void setup_ncurses();
void reset_win(WINDOW *w);

void file_log(const char *type, const char *fmt, ...);
void create_str(char **final_str, int count, ...);
int read_file(char **content, char *file_path);
FILE *open_file(char *file_path, char *type);

void exit_program(int err);

void small_clean_up();
void small_start_up();

WINDOW *get_win(int bwin);
struct _bordered_win *get_bwin(int bwin);

void cmd_win_print(bool update, int attr, const char *fmt, ...);
int read_line(FILE *stream, char *buffer, int buffer_size);

void clear_sensitive_2d_entries(char ***entries, int n_entries, int n_cols);
void clear_sensitive_1d_entries(char **entries, int n_entries);
void clear_2d_entries(char ***entries, int n_entries, int n_cols);
void clear_1d_entries(char **entries, int n_entries);

int error_check(int ret);

#endif
