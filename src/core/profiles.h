#ifndef CUSTOM_PROFILES
#define CUSTOM_PROFILES

#include <ncurses.h>
#include "../general/helper.h"

void read_directory(struct _win_manager *win_manager);
int open_directory(struct _win_manager *win_manager, char *directory);

#endif
