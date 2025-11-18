#ifndef CUSTOM_MACROS
#define CUSTOM_MACROS

#include <ncurses.h>

#define _CONFIG_DIR ".config"
#define _MVM_CONFIG_DIR ".config/mvm"
#define _MVM_PROFILE_CONFIGS_DIR ".config/mvm/profiles"
#define _LOG_FILE_PATH ".config/mvm/mvm.log"

#define _MVM_PROFILE_CONFIG_PREFIX ".profile_config["
#define _MVM_PROFILE_CONFIG_KEYS_PREFIX ".profile_keys["
#define _MVM_PROFILE_CONFIG_SUFFIX "].mvm"

#define SAFE_COLOR_PAIR(n) (has_colors() == TRUE ? COLOR_PAIR(n) : A_REVERSE)

#define FULL_BLOCK L"\x2588"
#define SHADER_CHAR (has_colors() ? L"\x2551" : L"\x002A")

#define TRUE 1
#define FALSE 0

#define PAIR_ERROR 100
#define PAIR_YELLOW 101
#define PAIR_BLUE 102
#define PAIR_YELLOW_BLUE 103

#define SUCCESS 0
#define EXIT_PROGRAM -1000
#define WAIT 1000

#define FILE_OPEN_ERROR -100
#define BUFFER_SIZE_ERROR -101
#define MALLOC_FAILED -102
#define FILE_FORMAT_ERROR -103

#define NO_WIN_ENTRY_FOUND -200

#define PROFILE_IMPORT_FOLDER_ERROR -300
#define PROFILE_IMPORT_READ_ERROR -301
#define PROFILE_ENCRYPTION_FAILED -302

#define RETRIES 3
#define LINE_LIMIT 4096
#define DIRECTORY_LIMIT 1024
#define FILENAME_LIMIT 256
extern FILE *LOG;

struct _bordered_win
{
	WINDOW *border;
	WINDOW *inner;
};
struct _win_manager
{
	struct _bordered_win *logo_win;
	struct _bordered_win *main_win;
	struct _bordered_win *side_win;
	struct _bordered_win *side_win_header;
	struct _bordered_win *side_win_body;
	struct _bordered_win *cmd_win;
};
extern struct _win_manager *win_manager;

#define ON 1
#define OFF 0
extern int cmd_mode;

#define MOVEMENT 0
#define TEXT 1
extern int input_mode;

#define LOGO_WIN 0
#define MAIN_WIN 1
#define SIDE_WIN 2
#define CMD_WIN 3
extern int current_win;
extern int current_menu;

#define MAIN_MENU 0
#define PROFILE_IMPORT_MENU 1
#define PROFILES_LIST_MENU 2

struct _win_scroller
{
	int size;
	int top;
	int highlighted;
	int prev_highlighted;
};

#define SIDE_WIN_COLS 4
extern char *side_win_entries[1000][4];
extern int n_side_win_entries;
extern struct _win_scroller *side_win_scroller;
extern int col_length;

extern char *main_win_entries[1000];
extern int n_main_win_entries;
extern struct _win_scroller *main_win_scroller;

#define PROFILE_CONFIG_ENTRIES 9
#define KEYS_AAD "profile_keys:v0.1.0"
#define KEYS_AAD_LEN 20
#define CONFIGS_AAD "profile_configs:v0.1.0"
#define CONFIGS_AAD_LEN 23
// Given as <the length of the item> + 1 to account for the item's length itself and the null terminater.
#define VPN_MAX_NAME_SIZE 100
#define VPN_KEY_SIZE 100
#define VPN_SERVER_CODE_SIZE 15
// [0-9]{3}(.[0-9]{3}){3}/32
#define IPV4_ADDRESS_SIZE 19
// [0-9a-f]{4}(:[0-9a-f]{4}){7}/128
#define IPV6_ADDRESS_SIZE 44
#define MAX_PORT_SIZE 6
struct _profile_config
{
	char public_key[VPN_KEY_SIZE];
	char server_code[VPN_SERVER_CODE_SIZE];
	char ipv4_address[IPV4_ADDRESS_SIZE];
	char ipv6_address[IPV6_ADDRESS_SIZE];
	char dns[IPV4_ADDRESS_SIZE];
	char ipv4_allowed_ips[IPV4_ADDRESS_SIZE];
	char ipv6_allowed_ips[IPV6_ADDRESS_SIZE];
	char ipv4_endpoint[IPV4_ADDRESS_SIZE];
	char port[MAX_PORT_SIZE];
};
struct _profile
{
	char device_name[VPN_MAX_NAME_SIZE];
	char private_key[VPN_KEY_SIZE];
	int n_configs;
	struct _profile_config *configs[1000];
};
extern struct _profile *profile;
extern FILE *PROFILE_FILE;
extern FILE *PROFILE_FILE_KEYS;

extern const char *HOME_DIR;
extern char *MVM_LOG_PATH;

#endif
