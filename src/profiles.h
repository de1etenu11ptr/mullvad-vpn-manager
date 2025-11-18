#ifndef CUSTOM_PROFILES
#define CUSTOM_PROFILES

#include <ncurses.h>
#include "helpers.h"

void clear_profile();
char **create_array_from_str(char *str, char sep, int size, int max_word_len);

#endif
