#include <sodium.h>
#include <string.h>
#include "encryption.h"
#include "globals.h"
#include "helpers.h"
#include "profiles_import.h"

FILE *PROFILE_FILE;
FILE *PROFILE_FILE_KEYS;

int encrypt_profile()
{
	int ret = SUCCESS;

	int device_name_len = VPN_MAX_NAME_SIZE +
		strlen(_MVM_PROFILE_CONFIG_PREFIX) +
		strlen(_MVM_PROFILE_CONFIG_SUFFIX);
	int full_path_len = strlen(HOME_DIR) + strlen(_MVM_PROFILE_CONFIGS_DIR) +
		device_name_len + 2;

	char *profile_config_filename = malloc(full_path_len + 1);
	memset(profile_config_filename, '\0', full_path_len + 1);
	char *profile_keys_filename = malloc(full_path_len + 1);
	memset(profile_keys_filename, '\0', full_path_len + 1);

	create_filename(&profile_config_filename, 0, device_name_len, full_path_len);

	create_filename(&profile_keys_filename, 1, device_name_len, full_path_len);

	PROFILE_FILE = fopen(profile_config_filename, "wb");
	PROFILE_FILE_KEYS = fopen(profile_keys_filename, "wb");

	if (PROFILE_FILE == NULL || PROFILE_FILE_KEYS == NULL) {
		file_log("ERROR", "Failed to create profile file (and/or key file) for: \"%s\"",
			profile->device_name,
			NULL);
		return FILE_OPEN_ERROR;
	}

	file_log("ENCRYPTION", "Encrypting profile: \"%s\"",
		profile->device_name,
		NULL);

	char *data_key = malloc(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
	char *nonces[2];
	for (int i = 0; i < 2; i++)
		nonces[i] = malloc(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
	char *enc_data = NULL;
	unsigned long long enc_data_len;
	char *enc_data_key = NULL;
	unsigned long long enc_data_key_len;
	char *profile_keys_data = NULL;

	randombytes_buf(data_key,
		crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
	randombytes_buf(nonces[0],
		crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
	randombytes_buf(nonces[1],
		crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);

	enc_data_len = encrypt_data(&enc_data, data_key, nonces[0]);
	if (enc_data_len == (unsigned long long) -1) {
		ret = PROFILE_ENCRYPTION_FAILED;
		file_log("ERROR", "Failed to encrypt data for: \"%s\"",
			profile->device_name,
			NULL);
		goto end;
	}
	enc_data_key_len = encrypt_data_key(&enc_data_key, data_key, nonces[1]);
	if (enc_data_key_len == (unsigned long long) -1) {
		ret = PROFILE_ENCRYPTION_FAILED;
		file_log("ERROR", "Failed to encrypt data key for: \"%s\"",
			profile->device_name,
			NULL);
		goto end;
	}

	unsigned int profile_keys_data_len = get_keys_file_data(&profile_keys_data,
		enc_data_key, enc_data_key_len,
		nonces);

	fwrite(enc_data, 1, enc_data_len, PROFILE_FILE);
	fwrite(profile_keys_data, 1, profile_keys_data_len, PROFILE_FILE_KEYS);
	file_log("ENCRYPTION", "Encrypted profile: \"%s\"",
		profile->device_name,
		NULL);

	end:;
	fclose(PROFILE_FILE);
	fclose(PROFILE_FILE_KEYS);
	PROFILE_FILE = NULL;
	PROFILE_FILE_KEYS = NULL;

	free(profile_config_filename);
	free(profile_keys_filename);

	uc_cleanup_encryption_data(&enc_data, enc_data_len);
	uc_cleanup_encryption_data(&enc_data_key, enc_data_key_len);
	uc_cleanup_encryption_data(&enc_data_key, enc_data_key_len);
	uc_cleanup_encryption_data(&profile_keys_data, profile_keys_data_len);
	cleanup_encryption_data(&data_key,
		crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
	cleanup_encryption_data(&nonces[0],
		crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
	cleanup_encryption_data(&nonces[1],
		crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
	return ret;
}

void create_filename(char **filename, int is_keys_file, int device_name_len, int filename_max_size)
{
	int len = filename_max_size - device_name_len - 1;
	strcpy(*filename, HOME_DIR);
	strcat(*filename, "/");
	strcat(*filename, _MVM_PROFILE_CONFIGS_DIR);
	strcat(*filename, "/");
	len = device_name_len -
		VPN_MAX_NAME_SIZE -
		strlen(_MVM_PROFILE_CONFIG_SUFFIX);
	if (is_keys_file)
		strncat(*filename,
			_MVM_PROFILE_CONFIG_KEYS_PREFIX,
			len);
	else
		strncat(*filename,
			_MVM_PROFILE_CONFIG_PREFIX,
			len);
	strncat(*filename, profile->device_name, VPN_MAX_NAME_SIZE);
	strcat(*filename, _MVM_PROFILE_CONFIG_SUFFIX);
	(*filename)[filename_max_size] = '\0';
}

unsigned int get_keys_file_data(char **data, char *enc_data_key, unsigned long long enc_data_key_len, char **nonce)
{
	unsigned long long b64_nonce_len = sodium_base64_encoded_len(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES, sodium_base64_VARIANT_ORIGINAL) - 1;
	unsigned long long b64_enc_data_key_len = sodium_base64_encoded_len(enc_data_key_len, sodium_base64_VARIANT_ORIGINAL) - 1;
	char *b64_enc_data_key = malloc(b64_enc_data_key_len + 1);
	char *b64_nonce1 = malloc(b64_nonce_len + 1);
	char *b64_nonce2 = malloc(b64_nonce_len + 1);

	sodium_bin2base64(b64_enc_data_key, b64_enc_data_key_len + 1,
		enc_data_key, enc_data_key_len,
		sodium_base64_VARIANT_ORIGINAL);
	sodium_bin2base64(b64_nonce1, b64_nonce_len + 1,
		nonce[0], crypto_aead_xchacha20poly1305_ietf_NPUBBYTES,
		sodium_base64_VARIANT_ORIGINAL);
	sodium_bin2base64(b64_nonce2, b64_nonce_len + 1,
		nonce[1], crypto_aead_xchacha20poly1305_ietf_NPUBBYTES,
		sodium_base64_VARIANT_ORIGINAL);

	int len = strlen(KEYS_AAD) +
		b64_nonce_len * 2 +
		b64_enc_data_key_len +
		strlen(CONFIGS_AAD) + 4;
	*data = malloc(len + 1);
	snprintf(*data, len + 1,
		"%s,%s,%s,%s,%s",
		KEYS_AAD, b64_nonce2, b64_enc_data_key,
		CONFIGS_AAD, b64_nonce1);
	free(b64_enc_data_key);
	free(b64_nonce1);
	free(b64_nonce2);
	return len;
}

unsigned long long encrypt_data(char **enc_data, char *data_key, char *nonce)
{
	char *data = NULL;
	unsigned long long data_len;
	if ((data_len = get_data(&data)) == (unsigned long long)  -1)
		return (unsigned long long) -1;
	unsigned long long enc_data_len = encrypt(enc_data,
		data, data_len,
		data_key,
		nonce,
		false);
	cleanup_encryption_data(&data, data_len);
	return enc_data_len;
}

unsigned long long get_data(char **data)
{
	unsigned long long data_len = serialize_profile(data);
	if (data_len == (unsigned long long) -1)
		return (unsigned long long) -1;
	return data_len;
}

unsigned long long serialize_profile(char **data)
{
	unsigned long long config_len = sizeof(struct _profile_config) +
		PROFILE_CONFIG_ENTRIES;
	char *configs = malloc((config_len) * profile->n_configs + sizeof(char));
	unsigned long long tracker = 0;
	for (int i = 0; i < profile->n_configs; i++) {
		int written = snprintf(configs + tracker, config_len - 1,
			"%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
			profile->configs[i]->public_key,
			profile->configs[i]->server_code,
			profile->configs[i]->ipv4_address,
			profile->configs[i]->ipv6_address,
			profile->configs[i]->dns,
			profile->configs[i]->ipv4_allowed_ips,
			profile->configs[i]->ipv6_allowed_ips,
			profile->configs[i]->ipv4_endpoint,
			profile->configs[i]->port);
		if (written < 0) {
			free(configs);
			configs = NULL;
			file_log("ERROR", "Failed to serialize profile.", NULL);
			return (unsigned long long) -1;
		}
		tracker += written;
	}
	configs[tracker] = '\0';
	*data = configs;
	configs = NULL;
	return tracker;
}

unsigned long long encrypt_data_key(char **enc_data_key, char *data_key, char *nonce)
{
	char *master_key = malloc(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
	int master_key_len = get_master_key(&master_key,
		crypto_aead_xchacha20poly1305_ietf_KEYBYTES,
		0);
	if (master_key_len == -1) {
		return (unsigned long long) -1;
	}
	unsigned long long enc_data_key_len = encrypt(enc_data_key,
		data_key, crypto_aead_xchacha20poly1305_ietf_KEYBYTES,
		master_key,
		nonce,
		true);
	cleanup_encryption_data(&master_key,
		crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
	return enc_data_key_len;
}

unsigned long long encrypt(char **enc_data,
	char *data, unsigned long long data_len,
	char *key, char *nonce,
	bool is_key)
{
	*(enc_data) = malloc(data_len + crypto_aead_xchacha20poly1305_ietf_ABYTES);
	unsigned long long enc_data_len = 0;

	crypto_aead_xchacha20poly1305_ietf_encrypt((*enc_data), &enc_data_len,
		data, data_len,
		is_key ? KEYS_AAD : CONFIGS_AAD,
		is_key ? KEYS_AAD_LEN : CONFIGS_AAD_LEN,
		NULL,
		nonce,
		key);

	return enc_data_len;
}

void cleanup_encryption_data(char **data, long long len)
{
	if (*data == NULL)
		return;
	memset(*data, '\0', len);
	free(*data);
	*data = NULL;
}

void uc_cleanup_encryption_data(char **data, long long len)
{
	if (*data == NULL)
		return;
	memset(*data, '\0', len);
	free(*data);
	*data = NULL;
}

int get_master_key(char **master_key, int len, int iter)
{
	int key_len = -1;
	WINDOW *win = win_manager->main_win->inner;
	werase(win);
	mvwprintw(win, getcury(win), 1,
		"Please enter the master key for the selected profile: \"%s\"",
		profile->device_name);
	wattron(win, A_BLINK | SAFE_COLOR_PAIR(PAIR_BLUE));
	mvwprintw(win, getcury(win) + 1, 1,
		"(The length of the master key must be %s and less than %d)",
		"greater than or equals to 3",
		len);
	wattroff(win, A_BLINK | SAFE_COLOR_PAIR(PAIR_BLUE));
	wmove(win, getcury(win) + 1, 1);

	echo();
	wgetnstr(win, *master_key, len);
	noecho();

	if (((*master_key)[len - 1] != '\0' &&
		(*master_key)[len - 1] != '\n' &&
		(*master_key)[len - 1] != '\r' &&
		(*master_key)[len - 1] != EOF) ||
		(key_len = strlen(*master_key)) <= 3) {
		if (key_len == -1)
			cmd_win_print(true, SAFE_COLOR_PAIR(PAIR_ERROR),
				"The length of the provided master key must be less than %d. %s",
				len,
				iter <= RETRIES ? "Try again" : "",
				NULL);
		else
			cmd_win_print(true, SAFE_COLOR_PAIR(PAIR_ERROR),
				"The length of the provided master key must be greater than or equals to 3. %s (%d %s)",
				iter <= RETRIES ? "Try again" : "",
				RETRIES - iter,
				RETRIES - iter == 1 ? "more chance left" : "chances left",
				NULL);
		return iter < RETRIES ? get_master_key(master_key, len, iter + 1) : -1;
	}

	return key_len;
}
