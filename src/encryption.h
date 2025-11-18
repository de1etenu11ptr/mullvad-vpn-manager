#ifndef CUSTOM_KEY_MANAGER
#define CUSTOM_KEY_MANAGER

#include <sodium.h>
#include <stdbool.h>

int encrypt_profile();
void create_filename(char **filename, int is_keys_file, int device_name_len, int filename_max_size);
unsigned int get_keys_file_data(char **data, char *enc_data_key, unsigned long long enc_data_key_len, char **nonce);
unsigned long long encrypt_data(char **enc_data, char *data_key, char *nonce);
unsigned long long get_data(char **data);
unsigned long long serialize_profile(char **data);
unsigned long long encrypt_data_key(char **enc_data_key, char *data_key, char *nonce);
unsigned long long encrypt(char **enc_data, char *data, unsigned long long data_len, char *key, char *nonce, bool is_key);
void cleanup_encryption_data(char **data, long long len);
void uc_cleanup_encryption_data(char **data, long long len);
int get_master_key(char **master_key, int len, int iter);

#endif
