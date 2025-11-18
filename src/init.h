#ifndef CUSTOM_INIT
#define CUSTOM_INIT

struct _bordered_win *create_bordered_window(WINDOW *parent,
	int height, int width,
	int yoffset, int xoffset,
	int attr);
void create_windows();
void create_side_win(int height, int width, int offsety, int offsetx);
void setup_ncurses();

void sensitive_clean_up();
void clean_up_tui();
void clean_up();
void start_up_tui();
void start_up();
void initialize_scrollers();
void destroy_scrollers();

void setup_config_location();

#endif
