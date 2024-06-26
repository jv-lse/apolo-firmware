#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "cJSON.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "apl_wifi.hpp"
#include "apl_camera.hpp"
#include "apl_storage.hpp"
#include "apl_bin_detection.hpp"
#include "apl_task_led.hpp"

#pragma pack(push)
#pragma pack(1)

typedef enum
{
    EVAL_BIN_TRIGGER = 0,
    ASYNC_TRIGGER = 1,
}request_trigger_e;

typedef enum
{
    HTTP_REQUEST_FAIL = 0,
    HTTP_SERVICE_ACQUIRED = 1,
}request_event_e;

typedef struct
{
    char mias_id[40];
    request_trigger_e rqst_trigger;
    time_t time_stamp;
}service_request_msg_t;

typedef struct
{
    char mias_id[40];
    uint32_t wakeup_time;
    char ssid[MIAS_CONFIG_SSID_MAX_LENGTH] ;
}send_configs_request_msg_t;

typedef struct
{
    char mias_id[40];
    mias_services_e service_requested;
    char device_config[100];
    request_event_e http_event;
}service_response_t;

#pragma pack(pop)

// HTTP RELATED ==============================================================================================
#define SERVER_RESPONSE_TIMEOUT (5000)

#define URL_SERVICE_BIN_ROUTE_SIZE (50)
#define URL_STOCKING_BIN_ROUTE_SIZE (100)
#define URL_CONFIG_BIN_ROUTE_SIZE (100)

static const char *SERVICE_BIN_ROUTE = "bin/service";
static const char *STOCKING_BIN_ROUTE = "bin/stocking";
static const char *CONFIG_BIN_ROUTE = "bin/config/";

// SNTP RELATED ==============================================================================================
#define SNTP_SERVER_ADDR ("pool.ntp.org")
#define SNTP_WAIT_SYNC_TIME (1000)
#define SNTP_TZ_FORMAT ("UTC+4") // TZ Manaus

// WIFI RELATED ==============================================================================================
#define WIFI_SUCCESS (1 << 0)
#define WIFI_FAILURE (1 << 1)
#define TCP_SUCCESS (1 << 0)
#define TCP_FAILURE (1 << 1)
#define MAX_RECONNECT_TRIES (1)

service_response_t response_json_strc;
static SemaphoreHandle_t http_response_timeout_bin_sem = NULL;
static esp_event_handler_instance_t wifi_handler_event_instance;
static esp_event_handler_instance_t got_ip_event_instance;
static EventGroupHandle_t wifi_event_group_handler;
esp_netif_t *apl_netif = NULL;
static uint8_t wifi_connect_tries = 0;

static const char *APL_WIFI_CONTROLLER_TAG = "APL_WIFI_CONTROLLER";
static const char *APL_SNPT_TAG = "APL_SNTP";
static const char *APL_HTTP_CLIENT_TAG = "APL_HTTP_CLIENT";

// UTILS PROTOTYPES =======================================================================================================
static void parse_response(char *response_msg);
static void parse_new_configs(cJSON *new_configs);
static void parse_response(char *response_msg);
static void send_post_request(char *url_server, char *post_data);
static void send_image_post_request(char *url_server, bin_analysis_trigger_e wakeup_trigger);

// EVENT HANDLERS =======================================================================================================
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_START))
    {
        ESP_LOGI(APL_WIFI_CONTROLLER_TAG, "conectando ao AP...");
        esp_wifi_connect();
    }
    else if ((event_base == WIFI_EVENT) && (event_id == WIFI_EVENT_STA_DISCONNECTED))
    {
        if (wifi_connect_tries < MAX_RECONNECT_TRIES)
        {
            ESP_LOGI(APL_WIFI_CONTROLLER_TAG, "tentativa %d de reconexao wifi", wifi_connect_tries);
            wifi_connect_tries++;
            esp_wifi_connect();
        }
        else
        {
            xEventGroupSetBits(wifi_event_group_handler, WIFI_FAILURE);
            wifi_connect_tries = 0;
        }
    }
}

static void ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        apl_delete_blinkled_wifi();
        led_on();
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(APL_WIFI_CONTROLLER_TAG, "IP addr recebido");
        ESP_LOGD(APL_WIFI_CONTROLLER_TAG, "IP addr: " IPSTR, IP2STR(&event->ip_info.ip));
        wifi_connect_tries = 0;
        xEventGroupSetBits(wifi_event_group_handler, WIFI_SUCCESS);
    }
}

esp_err_t http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGE(APL_HTTP_CLIENT_TAG, "Falha na requisicao ao servidor");
            server_fail_connect();
            response_json_strc.http_event = HTTP_REQUEST_FAIL;
            return ESP_FAIL;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(APL_HTTP_CLIENT_TAG, "Cliente conectado ao servidor");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(APL_HTTP_CLIENT_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(APL_HTTP_CLIENT_TAG, "HTTP_EVENT_ON_HEADER");
            printf("HTTP_EVENT_ON_HEADER %.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(APL_HTTP_CLIENT_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client))
            {
                printf("HTTP_EVENT_ON_DATA %.*s\n", evt->data_len, (char*)evt->data);
                response_json_strc.http_event = HTTP_SERVICE_ACQUIRED;
            }
            parse_response((char *)evt->data);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(APL_HTTP_CLIENT_TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(APL_HTTP_CLIENT_TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(APL_HTTP_CLIENT_TAG, "HTTP_EVENT_REDIRECT");
            break;
    }

    return ESP_OK;
}

// UTILS ============================================================================================================
static void parse_new_configs(cJSON *new_configs)
{
    mias_configs_t new_device_configs;
    apl_read_configs(&new_device_configs);
    std::string new_id;

    if(cJSON_IsObject(new_configs))
    {
        cJSON *wakeup_time_item = cJSON_GetObjectItem(new_configs, "wakeup_time");
        if(wakeup_time_item && cJSON_IsNumber(wakeup_time_item))
        {
            new_device_configs.wakeup_interval = (uint32_t)wakeup_time_item->valueint;
        }

        cJSON *device_id_item = cJSON_GetObjectItem(new_configs, "device_id");
        if(device_id_item && cJSON_IsString(device_id_item))
        {
            new_id = std::string(device_id_item->valuestring);
        }
        else
        {
            new_id = apl_read_device_id();
        }

        cJSON *ssid_item = cJSON_GetObjectItem(new_configs, "ssid");
        if(ssid_item && cJSON_IsString(ssid_item))
        {
            strncpy(new_device_configs.ssid, ssid_item->valuestring, sizeof(new_device_configs.ssid));
        }
        ESP_LOGI(APL_HTTP_CLIENT_TAG, "\r\nNovas configuracoes: \r\n\twakeup: (%d)\r\n\tssid:(%s)\r\n\tnew id: (%s)", (int)new_device_configs.wakeup_interval, new_device_configs.ssid, new_id.c_str());
    }

    apl_write_configs(new_device_configs);
    apl_write_device_id(new_id.c_str());
}

static void parse_response(char *response_msg)
{
    cJSON *response_json = cJSON_Parse(response_msg);
    if (response_json == NULL)
    {
        ESP_LOGE(APL_HTTP_CLIENT_TAG, "Erro ao analisar JSON");
        return;
    }

    cJSON *mias_id_item = cJSON_GetObjectItem(response_json, "mias_id");
    cJSON *service_item = cJSON_GetObjectItem(response_json, "service");
    cJSON *device_config_item = cJSON_GetObjectItem(response_json, "config");

    if (mias_id_item && mias_id_item->valuestring) 
    {
        strncpy(response_json_strc.mias_id, mias_id_item->valuestring, sizeof(response_json_strc.mias_id));
    }
    if (service_item && cJSON_IsNumber(service_item)) 
    {
        response_json_strc.service_requested = (mias_services_e)service_item->valueint;
    }
    else
    {
        response_json_strc.service_requested = GO_TO_SLEEP;
    }
    if (device_config_item && (response_json_strc.service_requested == CONFIGURE_MIAS_DEVICE)) 
    {
        ESP_LOGI(APL_HTTP_CLIENT_TAG, "Servico de reconfiguracao do dispositivo selecionado!");
        parse_new_configs(device_config_item);
    }

    cJSON_Delete(response_json);
    xSemaphoreGive(http_response_timeout_bin_sem);
}

void url_decode(char* str)
{
    int len = strlen(str);
    int i, j = 0;

    for (i = 0; i < len; i++) {
        if (str[i] == '%' && i + 2 < len) {
            char hex[3];
            hex[0] = str[i + 1];
            hex[1] = str[i + 2];
            hex[2] = '\0';

            int decodedChar;
            sscanf(hex, "%x", &decodedChar);

            str[j++] = (char)decodedChar;

            i += 2;
        } else {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
}

void ssid_decode(char *ssid)
{
    for (int i = 0; i < strlen(ssid); i++)
    {
        if(ssid[i] == '+')
        {
            ssid[i] = ' ';
        }
    }
}

char* password_decode(char *input)
{
    char *output = (char *)malloc(strlen(input) + 1);
    if (output == NULL) {
        return NULL;
    }
    char *in = (char*) input;
    char *out = output;

    while (*in) {
        if (*in == '%') {
            int value;
            if (sscanf(in + 1, "%2x", &value) == 1) {
                *out++ = value;
                in += 3;
            } else {
                *out++ = *in++;
            }
        } else if (*in == '+') {
            *out++ = ' ';
            in++;
        } else {
            *out++ = *in++;
        }
    }

    *out = '\0';
    return output;

}

static void send_post_request(char *url_server, char *post_data)
{
    esp_http_client_config_t config = {};
    config.url = url_server;
    config.event_handler = &http_event_handle;

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(APL_HTTP_CLIENT_TAG, "HTTP POST Status = %d, content_length = %lld", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(APL_HTTP_CLIENT_TAG, "Falha na execucao da requisicao POST");
    }
    esp_http_client_cleanup(client);
}

static void send_image_post_request(char *url_server, bin_analysis_trigger_e wakeup_trigger)
{
    std::string mias_id = apl_read_device_id();
    el_img_t img;
    esp_http_client_config_t config = {};
    std::vector<box_results_t> edgeAI_results;

    cJSON *json_root = cJSON_CreateObject();
    cJSON_AddStringToObject(json_root, "mias_id", mias_id.c_str());
    cJSON_AddNumberToObject(json_root, "analysis_trigger", wakeup_trigger);

    config.event_handler = http_event_handle;
    config.url = url_server;

    apl_eval_bin_img(&img, edgeAI_results);

    if(edgeAI_results.empty() == false)
    {
        cJSON *result_list = cJSON_CreateArray();

        for(auto &result : edgeAI_results)
        {
            cJSON *result_json = cJSON_CreateObject();
            cJSON_AddItemToObject(result_json, "x", cJSON_CreateNumber(result.x));
            cJSON_AddItemToObject(result_json, "y", cJSON_CreateNumber(result.y));
            cJSON_AddItemToObject(result_json, "w", cJSON_CreateNumber(result.w));
            cJSON_AddItemToObject(result_json, "h", cJSON_CreateNumber(result.h));
            cJSON_AddItemToObject(result_json, "target", cJSON_CreateNumber(result.target));
            cJSON_AddItemToObject(result_json, "score", cJSON_CreateNumber(result.score));

            cJSON_AddItemToArray(result_list, result_json);
        }
        cJSON_AddItemToObject(json_root, "prediction_list", result_list);
    }

    char *json_str = cJSON_Print(json_root);

    uint8_t * jpg_buf = (uint8_t *) heap_caps_malloc(200000, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    size_t jpg_size = 0;

    if(jpg_buf == NULL)
    {
        ESP_LOGE(APL_HTTP_CLIENT_TAG, "Malloc - falha ao alocar buffer para JPG.");
        cJSON_Delete(json_root);
        response_json_strc.service_requested = GO_TO_SLEEP;
        return;
    }

    fmt2jpg(img.data, img.size, img.width, img.height, PIXFORMAT_RGB565, APL_CAM_JPEG_QUALITY, &jpg_buf, &jpg_size);

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "multipart/form-data; boundary=boundary");
    esp_http_client_set_method(client, HTTP_METHOD_POST);

    const char *image_data = (const char *)jpg_buf;
    size_t image_len = jpg_size;

    std::string image_header =  "--boundary\r\n"
                                "Content-Disposition: form-data; name=\"image\"; filename=\"imagem.jpeg\"\r\n"
                                "Content-Type: image/jpeg\r\n\r\n";
    
    std::string json_pkg =  "\r\n"
                            "--boundary\r\n"
                            "Content-Disposition: form-data; name=\"json\"\r\n"
                            "Content-Type: application/json\r\n\r\n" + std::string(json_str) + "\r\n"
                            "--boundary--\r\n";

    size_t post_data_len = image_header.length() + image_len + json_pkg.length();
    char post_data[post_data_len] = {'\0'};

    memcpy((post_data), image_header.c_str(), image_header.length());
    memcpy((post_data + image_header.length()), image_data, image_len);
    memcpy((post_data + image_header.length() + image_len), json_pkg.c_str(), json_pkg.length());

    esp_http_client_set_post_field(client, (const char*)post_data, (image_header.length() + image_len + json_pkg.length()));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(APL_HTTP_CLIENT_TAG, "[IMAGE SENT] HTTP POST Status = %d, content_length = %lld", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(APL_HTTP_CLIENT_TAG, "Falha na execucao da requisicao POST da imagem!");
    }

    free(jpg_buf);
    cJSON_Delete(json_root);
    esp_http_client_cleanup(client);
}

// FEATURES ============================================================================================================
void apl_get_time(time_t *epoch_buffer, char * time_string_buffer)
{
    struct tm time_info;

    time(epoch_buffer);
    localtime_r(epoch_buffer, &time_info);
    strftime(time_string_buffer, TIMESTAMP_STRING_SIZE, "%d/%m/%Y-%X-%p", &time_info);
}

mias_services_e apl_get_service_response(void)
{
   switch (response_json_strc.http_event)
   {
   case HTTP_REQUEST_FAIL:
        ESP_LOGW(APL_HTTP_CLIENT_TAG, "Falha na requisicao!");
        return GO_TO_SLEEP;
    break;
   case HTTP_SERVICE_ACQUIRED:
        if((xSemaphoreTake(http_response_timeout_bin_sem, pdMS_TO_TICKS(SERVER_RESPONSE_TIMEOUT)) == true))
        {
            xSemaphoreGive(http_response_timeout_bin_sem);
            return response_json_strc.service_requested; 
        }
        else
        {
            ESP_LOGW(APL_HTTP_CLIENT_TAG, "Estouro de timeout do HTTP response!");
            return GO_TO_SLEEP;
        }
    break;
   default:
        ESP_LOGW(APL_HTTP_CLIENT_TAG, "Evento HTTP nao mapeado!");
        return GO_TO_SLEEP;
    break;
   }
}

void apl_send_mias_device_configs(void)
{
    std::string str_buf = apl_read_device_id();
    char url_server[URL_CONFIG_BIN_ROUTE_SIZE] = {'\0'};

    mias_configs_t mias_configs;
    send_configs_request_msg_t tx_msg;

    strcpy(tx_msg.mias_id, str_buf.c_str());

    apl_read_configs(&mias_configs);

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "mias_id", tx_msg.mias_id);
    cJSON_AddNumberToObject(root, "wakeup_time", mias_configs.wakeup_interval);
    cJSON_AddStringToObject(root, "ssid", mias_configs.ssid);

    char *post_data = cJSON_Print(root);
    cJSON_Delete(root);

    snprintf(url_server, 100, "http://%s/%s", mias_configs.server_addr, CONFIG_BIN_ROUTE);
    strcat(url_server, tx_msg.mias_id);

    send_post_request(url_server, post_data);
}

void apl_request_for_service(time_t curr_time_stamp)
{
    std::string str_buf = apl_read_device_id();
    char url_server[URL_SERVICE_BIN_ROUTE_SIZE] = {'\0'};
    mias_configs_t mias_configs = {};
    service_request_msg_t tx_msg = {};

    tx_msg.rqst_trigger = EVAL_BIN_TRIGGER;
    tx_msg.time_stamp = curr_time_stamp;

    strcpy(tx_msg.mias_id, str_buf.c_str());

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "mias_id", tx_msg.mias_id);
    cJSON_AddNumberToObject(root, "request_trigger", tx_msg.rqst_trigger);
    cJSON_AddNumberToObject(root, "time_stamp", tx_msg.time_stamp);

    char *post_data = cJSON_Print(root);
    cJSON_Delete(root);
    apl_read_configs(&mias_configs);

    sprintf(url_server, "http://");
    strcat(url_server, mias_configs.server_addr);
    strcat(url_server, "/");
    strcat(url_server, SERVICE_BIN_ROUTE);

    send_post_request(url_server, post_data);
}

void apl_send_image(time_t curr_time_stamp, bin_analysis_trigger_e wakeup_trigger)
{
    char url_server[URL_STOCKING_BIN_ROUTE_SIZE] = {'\0'};
    mias_configs_t mias_configs;

    apl_read_configs(&mias_configs);

    sprintf(url_server, "http://");
    strcat(url_server, mias_configs.server_addr);
    strcat(url_server, "/");
    strcat(url_server, STOCKING_BIN_ROUTE);

    printf("rota: %s\n", url_server);

    send_image_post_request(url_server, wakeup_trigger);
}

// INITS ============================================================================================================
void apl_wifi_nvs_init(void)
{
    esp_err_t err = nvs_flash_init();
    if ((err == ESP_ERR_NVS_NO_FREE_PAGES) || (err == ESP_ERR_NVS_NEW_VERSION_FOUND))
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}

apl_status_t apl_wifi_init(void)
{
    apl_status_t apl_ret = APL_FAIL;
    mias_configs_t wifi_credentials;

    apl_read_configs(&wifi_credentials);

    ESP_LOGI(APL_WIFI_CONTROLLER_TAG, "camada fisica STA inicializada!");
    apl_wifi_nvs_init();
    wifi_event_group_handler = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    apl_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &wifi_handler_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL, &got_ip_event_instance));

    wifi_config_t wifi_config={};

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_HUNT_AND_PECK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    strcpy((char *)(wifi_config.sta.ssid), wifi_credentials.ssid);
    strcpy((char *)(wifi_config.sta.password), wifi_credentials.password);

    // printf("\n\n\nlogin: %s, senha: %s\n\n\n", wifi_credentials.ssid, wifi_credentials.password);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(APL_WIFI_CONTROLLER_TAG, "camada fisica STA inicializada!");

    EventBits_t wifi_event_bits = xEventGroupWaitBits(wifi_event_group_handler, (WIFI_SUCCESS | WIFI_FAILURE), pdFALSE, pdFALSE, portMAX_DELAY);

    if (wifi_event_bits & WIFI_SUCCESS)
    {
        ESP_LOGI(APL_WIFI_CONTROLLER_TAG, "Conectado ao AP na rede: %s", wifi_credentials.ssid);
        apl_ret = APL_SUCCESS;
    }
    else if (wifi_event_bits & WIFI_FAILURE)
    {
        ESP_LOGW(APL_WIFI_CONTROLLER_TAG, "Falha ao conectar ao AP");
        apl_ret = APL_FAIL;
    }
    else
    {
        ESP_LOGE(APL_WIFI_CONTROLLER_TAG, "evento wifi nao mapeado");
        apl_ret = APL_FAIL;
    }

    vEventGroupDelete(wifi_event_group_handler);

    return apl_ret;
}

apl_status_t apl_wifi_start(void)
{
    esp_log_level_set(APL_WIFI_CONTROLLER_TAG, ESP_LOG_VERBOSE);
    esp_log_level_set(APL_SNPT_TAG, ESP_LOG_VERBOSE);
    apl_blinkled_connect_fail();
    if (apl_wifi_init() == APL_SUCCESS)
    {
        ESP_LOGI(APL_WIFI_CONTROLLER_TAG, "WiFI inicializado");
        return APL_SUCCESS;
    }
    else
    {
        ESP_LOGE(APL_WIFI_CONTROLLER_TAG, "Falha ao inicializar WiFi");
        return APL_FAIL;
    }
}

void apl_sntp_init(void)
{
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, SNTP_SERVER_ADDR);
    setenv("TZ", SNTP_TZ_FORMAT, 1);
    tzset();
    esp_sntp_init();

    int retry = 0;
    while ((sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) && (retry < 10)) {
        ESP_LOGD(APL_SNPT_TAG, "Tentativa (%d) de sincronizacao com SNTP", ++retry);
        vTaskDelay(pdMS_TO_TICKS(SNTP_WAIT_SYNC_TIME));
    }
    ESP_LOGI(APL_SNPT_TAG, "Sitema sincronizado com servidor SNTP");
}

void apl_response_timeout_semaphore_init(void)
{
    http_response_timeout_bin_sem = xSemaphoreCreateBinary();
}

// DEINITS ============================================================================================================
void apl_wifi_deinit(void)
{
    led_off();
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
        ESP_LOGD(APL_WIFI_CONTROLLER_TAG, "handlers de evento desalocados");

    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_stop());
    esp_netif_destroy_default_wifi(apl_netif);
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_LOGI(APL_WIFI_CONTROLLER_TAG, "WiFi desligado");
}

void apl_sntp_stop(void)
{
    esp_sntp_stop();
    ESP_LOGD(APL_SNPT_TAG, "Servico de SNTP encerrado");
}
