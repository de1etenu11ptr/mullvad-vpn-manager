#ifndef CUSTOM_HELPERS
#define CUSTOM_HELPERS

#include <ncurses.h>

struct _win_manager
{
	WINDOW *logo_win_border;
	WINDOW *logo_win;
	WINDOW *main_win_border;
	WINDOW *main_win;
	WINDOW *side_win_border;
	WINDOW *side_win;
	WINDOW *cmd_win_border;
	WINDOW *cmd_win;
};

struct _side_win_scroller
{
	int size;
	int top;
	int highlighted;
};

WINDOW *new_win(int height, int width, int starty, int startx);
WINDOW *new_pad(int height, int width, int starty, int startx);
void create_wins(struct _win_manager *win_manager);
void delete_win(WINDOW *local_win);
void initialize_globals();

void setup_ncurses();
void reset(WINDOW *win);

WINDOW *get_win(struct _win_manager *win_manager, int win);

#endif
