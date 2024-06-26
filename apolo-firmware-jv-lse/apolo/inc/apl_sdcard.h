#ifndef APOLO_SDCARD_H
#define APOLO_SDCARD_H

#include "apolo_essentials.h"
#include "esp_err.h"

esp_err_t card_mout();
void apl_save_picture(char *file_name);

#endif