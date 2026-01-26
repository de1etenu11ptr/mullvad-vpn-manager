#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include "init.h"
#include "globals.h"
#include "side_win.h"
#include "main_win.h"
#include "window_helpers.h"
#include "helpers.h"
#include "logo.h"
#include "profiles.h"


struct _bordered_win *create_bordered_window(WINDOW *parent,
	int height, int width,
	int yoffset, int xoffset,
	int attr)
{
	struct _bordered_win *w = malloc(sizeof(WINDOW *) * 2);

	WINDOW *border = derwin(
		parent,
		height,
		width,
		yoffset,
		xoffset);
	if (border == NULL)
		return NULL;
	if (attr != -1) {
		wattron(border, attr);
		box(border, 0, 0);
		wattroff(border, attr);
	}
	wnoutrefresh(border);

	WINDOW *win = derwin(border,
		getmaxy(border) - 2,
		getmaxx(border) - 2,
		1,
		1);
	wnoutrefresh(win);

	w->border = border;
	w->inner = win;
	return w;
}

void create_windows()
{
	int main_wins_width;

	if (COLS >= 160 && LINES >= 10) {
		main_wins_width = COLS/2;
		create_side_win(LINES - 3, COLS/2, 0, COLS/2);
		win_manager->logo_win = create_bordered_window(
			stdscr,
			LINES/2 - 3,
			COLS/2,
			0,
			0,
			A_NORMAL);
	} else {
		main_wins_width = COLS;
		create_side_win(LINES/2 - 3, main_wins_width, 0, 0);
	}
	col_length = getmaxx(win_manager->side_win_body->inner)/SIDE_WIN_COLS;

	int tmp = LINES - (LINES/2 - 3) - 3;
	win_manager->main_win = create_bordered_window(
		stdscr,
		tmp,
		main_wins_width,
		LINES/2-3,
		0,
		A_NORMAL);

	win_manager->cmd_win = create_bordered_window(
		stdscr,
		3,
		COLS,
		LINES - 3,
		0,
		A_NORMAL);
}

void create_side_win(int height, int width, int offsety, int offsetx)
{
	win_manager->side_win = create_bordered_window(
		stdscr,
		height,
		width,
		offsety,
		offsetx,
		A_NORMAL);
	win_manager->side_win_header = create_bordered_window(
		win_manager->side_win->inner,
		4,
		getmaxx(win_manager->side_win->inner),
		0,
		0,
		SAFE_COLOR_PAIR(PAIR_YELLOW_BLUE));
	win_manager->side_win_body = create_bordered_window(
		win_manager->side_win->inner,
		getmaxy(win_manager->side_win->inner) - 4,
		getmaxx(win_manager->side_win->inner),
		4,
		0,
		A_NORMAL);
}

void setup_ncurses()
{
	setlocale(LC_ALL, "");
	initscr();
	raw();
	noecho();

	if (has_colors() != TRUE) {
		file_log("START UP", "%s",
			"Terminal does not support colors.",
			NULL);
	} else {
		file_log("START UP", "%s",
			"Terminal supports colors.",
			NULL);
		start_color();
		if (can_change_color()) {
			file_log("START UP", "%s",
				"Setting default color options to preferred RGB values.",
				NULL);
			init_color(COLOR_YELLOW,
				(255 * 1000) / 255,
				(236 * 1000) / 255,
				(10 * 1000) / 255);
			init_color(COLOR_BLUE,
				(41 * 1000) / 255,
				(77 * 1000) / 255,
				(115 * 1000) / 255);
		}
		file_log("START UP", "%s",
			"Setting up color pairs for the program.",
			NULL);
		init_pair(PAIR_ERROR, COLOR_RED, COLOR_BLACK);
		init_pair(PAIR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
		init_pair(PAIR_BLUE, COLOR_BLUE, COLOR_BLACK);
		init_pair(PAIR_YELLOW_BLUE, COLOR_YELLOW, COLOR_BLUE);
	}
	curs_set(0);
}

void sensitive_clean_up()
{
	file_log("EXIT", "Cleaning up sensitive data.", NULL);
	clear_profile();
	free(profile);
	profile = NULL;
}

void clean_up_tui()
{
	file_log("EXIT", "Cleaning up TUI.", NULL);
	clear_side_win_entries();
	clear_main_win_entries();

	destroy_scrollers();

	delete_windows();
	free(win_manager);
	win_manager = NULL;

	endwin();
}

void clean_up()
{
	clean_up_tui();

	sensitive_clean_up();

	if (PROFILE_FILE != NULL)
		fclose(PROFILE_FILE);
	if (PROFILE_FILE_KEYS != NULL)
		fclose(PROFILE_FILE_KEYS);
	free(MVM_LOG_PATH);
	file_log("EXIT", "PROGRAM CLOSING...", NULL);
	if (LOG != NULL)
		fclose(LOG);
}

void start_up_tui()
{
	file_log("START UP", "%s",
		"Setting up TUI.",
		NULL);
	setup_ncurses();

	clear();
	win_manager = malloc(sizeof(WINDOW *) * 6 * 2);
	create_windows();

	initialize_scrollers();

	if (win_manager->logo_win != NULL)
		add_logo(win_manager->logo_win->inner);
	keypad(win_manager->main_win->inner, TRUE);

	refresh();
}

void start_up()
{
	file_log("START UP", "%s",
		"PROGRAM STARTING...",
		NULL);
	start_up_tui();

	profile = malloc(sizeof(struct _profile));
	memset(profile, 0, sizeof(struct _profile));

	cmd_mode = OFF;
	input_mode = MOVEMENT;
}

void initialize_scrollers()
{
	side_win_scroller = malloc(sizeof(struct _win_scroller));
	side_win_scroller->size = 0;
	side_win_scroller->top = 0;
	side_win_scroller->highlighted = 0;
	side_win_scroller->prev_highlighted = 0;
	main_win_scroller = malloc(sizeof(struct _win_scroller));
	main_win_scroller->size = 0;
	main_win_scroller->top = 0;
	main_win_scroller->highlighted = 0;
	main_win_scroller->prev_highlighted = 0;
}

void destroy_scrollers()
{
	free(side_win_scroller);
	side_win_scroller = NULL;
	free(main_win_scroller);
	main_win_scroller = NULL;
}

void create_subdirectory(const char *directory, const char *subdirectory)
{
	int len = strlen(directory) + strlen(subdirectory) + 1;
	char *path = malloc(len + 1);
	strcpy(path, directory);
	strcat(path, "/");
	strcat(path, subdirectory);
	if (mkdir(path, 0777) && errno != EEXIST) {
		fprintf(stderr, "\e[0mProgram config directory, \"%s\", could not be found nor created.", path);
		free(path);
		exit(errno);
	}
	free(path);
}

void setup_config_location()
{
	if ((HOME_DIR = getenv("HOME")) == NULL) {
		HOME_DIR = getpwuid(getuid())->pw_dir;
	}
	create_subdirectory(HOME_DIR, _CONFIG_DIR);
	create_subdirectory(HOME_DIR, _MVM_CONFIG_DIR);
	create_subdirectory(HOME_DIR, _MVM_PROFILE_CONFIGS_DIR);

	int len = strlen(HOME_DIR) + strlen(_LOG_FILE_PATH) + 1;
	MVM_LOG_PATH = malloc(len + 1);
	memset(MVM_LOG_PATH, '\0', len + 1);
	strcpy(MVM_LOG_PATH, HOME_DIR);
	strcat(MVM_LOG_PATH, "/");
	strcat(MVM_LOG_PATH, _LOG_FILE_PATH);
}

