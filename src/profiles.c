#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include "profiles.h"
#include "encryption.h"
#include "helpers.h"
#include "globals.h"
#include "profiles_import.h"
#include "side_win.h"

struct _profile *profile;
char **current_profile_paths;

void clear_profile()
{
	strcpy(profile->device_name, "NONE");
	for (int i = 0; i < VPN_KEY_SIZE; i++)
		profile->private_key[i] = '\0';
	for (int i = 0; i < profile->n_configs; i++) {
		memset(profile->configs[i], 0, sizeof(struct _profile_config));
		free(profile->configs[i]);
		profile->configs[i] = NULL;
	}
	profile->n_configs = 0;
}

char **create_array_from_str(char *str, char sep, int size, int max_word_len)
{
	char **array = malloc(sizeof(char *) * size);
	if (array == NULL) {
		return NULL;
	}

	for (int index = 0, i = 0; index < size && i < max_word_len;) {
		array[index] = malloc(sizeof(char) * max_word_len);
		int x = 0;
		while (
			x < max_word_len - 1 &&
			i < max_word_len &&
			str[i] != sep &&
			str[i] != '\0')
		{
			array[index][x] = str[i];
			x++;
			i++;
		}
		array[index][x] = '\0';
		index++;
		if (str[i] == sep)
			i++;
	}

	return array;
}


