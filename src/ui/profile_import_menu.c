#include "profile_import_menu.h"
#include "../general/helper.h"
#include "../general/globals.h"
#include "../core/profiles.h"

int print_profile_import_menu(struct _win_manager *win_manager)
{
	read_directory(win_manager);
	return MAIN_MENU;
}
