#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include "cmdline.h"
#include "side_win.h"
#include "globals.h"
#include "helpers.h"
#include "window_helpers.h"

int cmd_win_cooldown = 0;

int cmdline_quick_move() {
	return SUCCESS;
}

int cmdline_search(int win_n)
{
	WINDOW *win = win_manager->cmd_win->inner;
	char *str = malloc(sizeof(char) * getmaxx(win));
	int c;
	int i = 0;
	int found = -1;
	cmd_win_print(true, A_BLINK, "/", NULL);
	keypad(get_win(current_win), false);
	keypad(win, true);
	wmove(win, 0, 0);
	while (true) {
		c = wgetch(win);
		if (found != -1 && (c == (int) 'n' || c == (int) 'N')) {
			int tmp;
			if (c == (int) 'n')
				tmp = search_entries(win_n, str, i, found, true);
			else
				tmp = search_entries(win_n, str, i, found, false);
			found = tmp;
			cmd_win_print(false, A_BLINK, "/", NULL);
			mvwaddstr(win, 0, 2, str);
			doupdate();
			continue;
		}
		switch(c){
		case 3:
		case 4:
			free(str);

			keypad(win, false);
			keypad(get_win(current_win), true);

			exit_program(EXIT_PROGRAM);
			break;
		case 27:
			cmd_win_print(false, 0, "", NULL);
			free(str);
			return SUCCESS;
		case (int) '\r':
		case 10: // Enter key.
			if (found != -1)
				break;
			str[i] = '\0';
			found = search_entries(win_n, str, i, found, true);
			if (found == -1)
				return SUCCESS;
			break;
		case (int) '\b':
		case KEY_BACKSPACE:
		case 127:
			if (i <= 0 || found != -1)
				break;
			i--;
			str[i] = '\0';
			mvwaddch(win, 0, i + 2, ' ');
			wmove(win, 0, i);
			break;
		default:
			if (found != -1) {
				beep();
				break;
			}
			if (i < getmaxx(win) - 1 && isprint(c)) {
				str[i] = (char) c;
				str[i + 1] = '\0';
				mvwaddch(win, 0, i + 2, (char) c);
				i++;
			} else {
				beep();
			}
		}
	}
}

int search_substr(char *str, int offset, char *match, int search_len)
{
	char *substr = str + offset;
	return strncmp(substr, match, search_len);
}

int search_entries(int win_n, char *match, int search_len, int found, int step)
{
	step = step == 0 ? 1 : step;
	WINDOW *win = win_manager->cmd_win->inner;
	struct _win_scroller *scroller = get_win_scroller(win_n);
	int i = found != -1 ?
		(found + step < get_max_entries(win_n) && found + step > 0 ? found + step : -1) :
		0;
	for (; i < get_max_entries(win_n) && i >= 0; i += step) {
		char *str = side_win_entries[i][0];
		int len = strlen(str);
		for (int j = 0; j < len; j++) {
			if (search_substr(str, j, match, search_len) == 0) {
				mvwaddstr(win, 0, 2, match);
				scroller->prev_highlighted = scroller->highlighted;
				scroller->highlighted = i;
				print_win(win_n);
				doupdate();
				return i;
			}
		}
	}
	cmd_win_print(true, A_BLINK | SAFE_COLOR_PAIR(PAIR_ERROR),
		"\"*%s*\" not found.",
		match,
		NULL);
	return found;
}
