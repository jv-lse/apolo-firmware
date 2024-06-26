#ifndef APOLO_WIFI_H
#define APOLO_WIFI_H

#include "esp_http_server.h"                    
#include "esp_sntp.h"

#include "apolo_essentials.h"

#include "driver/uart.h"

#define TIMESTAMP_STRING_SIZE (23)

typedef enum
{
    GO_TO_SLEEP = 0,
    CONFIGURE_MIAS_DEVICE = 1,
    EVALUATE_BIN_STATUS = 2,
    SEND_MIAS_DEVICE_CONFIGURATION_DATA = 3,
}mias_services_e;

typedef enum 
{
    SYNCHRONOUS_BIN_ANALYSIS_REQUEST = 0, 
    ASYNCHRONOUS_BIN_ANALYSIS_REQUEST = 1, 
}bin_analysis_trigger_e;

void apl_wifi_nvs_init(void);

void ssid_decode(char *ssid);
char* password_decode(char *input);
void url_decode(char* str);

void apl_sntp_init(void);
void apl_get_time(time_t *epoch_buffer, char * time_string_buffer);
void apl_sntp_stop(void);

apl_status_t apl_wifi_start(void);
void apl_wifi_deinit(void);

void apl_wifi_init_access_point(void);
void apl_get_access_point_ip(void);
httpd_handle_t apl_start_webserver(void);

void apl_request_for_service(time_t curr_time_stamp);
void apl_send_image(time_t curr_time_stamp, bin_analysis_trigger_e wakeup_trigger);
void apl_send_mias_device_configs(void);

void apl_response_timeout_semaphore_init(void);
mias_services_e apl_get_service_response(void);

void uart_init();
void apl_cadastra_id(void *pvParameters);
void task_cadastro_id();

#endif
