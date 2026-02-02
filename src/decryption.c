#include <sodium.h>
#include "decryption.h"
#include "globals.h"
#include "helpers.h"

int get_profile_configs(char *keys_data) {
	char **array = NULL;
	int array_size = 0;
	file_log("LOG", "Contents: %s", keys_data, NULL);
	array_size = create_str_array(&array, keys_data, ',');
	clear_sensitive_1d_entries(array, array_size);
	free(array);
	return 0;
}
