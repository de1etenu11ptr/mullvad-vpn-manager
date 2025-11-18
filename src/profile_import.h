#ifndef CUSTOM_PROFILE_IMPORT_MENU
#define CUSTOM_PROFILE_IMPORT_MENU

#include <dirent.h>
#include "helpers.h"

void profile_import();
int open_directory(char *directory);
int read_profile_config(char *dir, struct dirent *dir_entry);
void display_profile_config(struct _profile_config *profile_config);
int handle_line(char *line, struct _profile_config *config);

#endif
