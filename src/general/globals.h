#ifndef CUSTOM_MACROS
#define CUSTOM_MACROS

#define FULL_BLOCK L"\x2588"
#define VERT_BARS L"\x2551"

#define PAIR_ERROR 1
#define PAIR_YELLOW 2
#define PAIR_BLUE 3
#define PAIR_YELLOW_BLUE 4

#define LOGO_WIN 0
#define MAIN_WIN 1
#define SIDE_WIN 2
#define CMD_WIN 3

#define MAX_IDENTIFIER_LEN 256

#define MAIN_MENU 0
#define PROFILE_IMPORT_MENU 1

#define PROFILE_IMPORT_SUCCESS 0
#define PROFILE_IMPORT_FOLDER_ERROR 100
#define PROFILE_IMPORT_READ_ERROR 101

extern bool cmd_mode;
extern bool side_win_exists;
extern int current_win;
extern char *profile_identifer;
extern char *profile_configs_identifiers[1000];
extern char *profile_configs_server_codes[1000];
extern int n_profile_configs;
extern struct _side_win_scroller *side_win_scroller;

#endif
