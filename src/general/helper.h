#include <ncurses.h>
#include "macros.h"

WINDOW *new_win(int height, int width, int starty, int startx);
void delete_win(WINDOW *local_win);
