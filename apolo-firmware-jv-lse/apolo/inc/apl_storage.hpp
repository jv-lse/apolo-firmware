#ifndef APOLO_STORAGE_H
#define APOLO_STORAGE_H

#include <string>
#include "apolo_essentials.h"

void apl_read_configs(mias_configs_t *out_configs);
void apl_write_configs(const mias_configs_t new_configs);
void apl_init_spiffs(void);

void apl_write_device_id(const char *device_id);
std::string apl_read_device_id(void);

#define CONFIGS_PARTITION_LABEL ("apl_configs")
#define APL_SPIFFS_ROOT_PATH ("/spiffs")
#define APL_SPIFFS_CONFIGS_PATH ("/spiffs/configs")
#define APL_SPIFFS_DEVICE_ID_PATH ("/spiffs/device_id")

#define DEVICE_ID_STR_BUFFER_SIZE (40)

// UART DEFINITIONS
#define UART_NUM UART_NUM_0
#define BUF_SIZE (1024)

#endif
