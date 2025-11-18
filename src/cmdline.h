#ifndef CUSTOM_CMDLINE
#define CUSTOM_CMDLINE

int cmdline_quick_move();
int cmdline_search();
int search_substr(char *str, int offset, char *match, int search_len);
int search_entries(int win_n, char *match, int search_len, int found, int step);

#endif
