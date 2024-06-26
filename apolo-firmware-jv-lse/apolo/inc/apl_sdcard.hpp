#ifndef APOLO_SDCARD_H
#define APOLO_SDCARD_H

#include "esp_err.h"
#include "apolo_essentials.h"

esp_err_t apl_card_mout();
void apl_save_file(char *file_name, uint8_t *file_buf, size_t file_size);
uint64_t get_next_filename(char *next_filename, size_t max_len);

#endif
