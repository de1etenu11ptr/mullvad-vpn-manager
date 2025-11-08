#ifndef CUSTOMLOGO
#define CUSTOMLOGO

#include <ncurses.h>
#include "../../general/macros.h"

void add_shading(WINDOW *win, int max_win_row, int row, int col);
void add_logo(WINDOW *win);

#endif
