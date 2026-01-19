#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include <stddef.h>
#include "esp_err.h"

/* Initialize NVS flash (call once at boot) */
esp_err_t nvs_storage_init(void);

/* Write a null-terminated string to flash */
esp_err_t nvs_write_string(const char *key, const char *value);

/* Read a string from flash into buffer */
esp_err_t nvs_read_string(const char *key, char *out, size_t max_len);

/* Erase a stored key */
esp_err_t nvs_storage_erase_key(const char *key);

/* Erase entire namespace */
esp_err_t nvs_storage_erase_all(void);

#endif /* NVS_STORAGE_H */
