#include <string.h>
#include "apl_storage.hpp"
#include "esp_spiffs.h"

#define CONFIGS_PARTITION_LABEL ("apl_configs")
#define APL_SPIFFS_ROOT_PATH ("/spiffs")
#define APL_SPIFFS_CONFIGS_PATH ("/spiffs/configs")
#define APL_SPIFFS_DEVICE_ID_PATH ("/spiffs/device_id")

#define DEVICE_ID_STR_BUFFER_SIZE (40)

static const std::string BLANK_CONFIG_ID_VALUE = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
static const char *STORAGE_TAG = "APL_STORAGE";

void apl_write_configs(const mias_configs_t new_configs)
{
    ESP_LOGI(STORAGE_TAG, "abrindo arquivo para escrita");

    FILE *config_file = fopen(APL_SPIFFS_CONFIGS_PATH, "wb");
    if(config_file == NULL)
    {
        ESP_LOGE(STORAGE_TAG, "falha em abrir arquivo");
        return;
    }

    fwrite(&new_configs, sizeof(mias_configs_t), 1, config_file);
    ESP_LOGI(STORAGE_TAG, "Configuracoes salvas");
    fclose(config_file);
}

void apl_read_configs(mias_configs_t *out_configs)
{
    ESP_LOGI(STORAGE_TAG, "abrindo arquivo para leitura");

    FILE *config_file = fopen(APL_SPIFFS_CONFIGS_PATH, "rb");
    if(config_file == NULL)
    {
        ESP_LOGE(STORAGE_TAG, "falha em abrir arquivo");
        return;
    }

    fread(out_configs, sizeof(mias_configs_t), 1, config_file);
    ESP_LOGI(STORAGE_TAG, "Configuracoes lidas");
    fclose(config_file);
}

std::string apl_read_device_id(void)
{
    std::string out_device_id; 
    char str_buffer[DEVICE_ID_STR_BUFFER_SIZE];
    ESP_LOGI(STORAGE_TAG, "abrindo arquivo de device ID");

    FILE *device_id_file = fopen(APL_SPIFFS_DEVICE_ID_PATH, "rb");
    if(device_id_file == NULL)
    {
        ESP_LOGE(STORAGE_TAG, "falha em abrir arquivo de device ID");
        ESP_LOGW(STORAGE_TAG, "utilizando valor nulo de device ID");
        ESP_LOGW(STORAGE_TAG, "novo valor: %s", BLANK_CONFIG_ID_VALUE.c_str());
        return BLANK_CONFIG_ID_VALUE;
    }

    fread(str_buffer, DEVICE_ID_STR_BUFFER_SIZE, sizeof(char), device_id_file);
    fclose(device_id_file);

    out_device_id = std::string(str_buffer);

    ESP_LOGI(STORAGE_TAG, "identificador lido: (%s)", out_device_id.c_str());
    ESP_LOGI(STORAGE_TAG, "identificador de dispositivo adquirido");

    return out_device_id;
}

void apl_write_device_id(const char *device_id)
{
    ESP_LOGI(STORAGE_TAG, "abrindo arquivo de device ID");

    FILE *device_id_file = fopen(APL_SPIFFS_DEVICE_ID_PATH, "wb");
    if(device_id_file == NULL)
    {
        ESP_LOGE(STORAGE_TAG, "falha em abrir arquivo device ID");
        return;
    }

    fwrite(device_id, strlen(device_id), strlen(device_id), device_id_file);
    ESP_LOGI(STORAGE_TAG, "identificador (%s) atribuido a este dispositvo", device_id);
    fclose(device_id_file);
}

void apl_init_spiffs(void)
{
    esp_err_t ret;

    esp_vfs_spiffs_conf_t conf = {
        .base_path = APL_SPIFFS_ROOT_PATH,
        .partition_label = CONFIGS_PARTITION_LABEL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(STORAGE_TAG, "Falha ao montar ou formatar config_file system");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(STORAGE_TAG, "falha em encontrar particao SPIFFS");
        } else {
            ESP_LOGE(STORAGE_TAG, "Falha ao inicializar SPIFFS (%s)", esp_err_to_name(ret));
        }
    }
    else
    {
        ESP_LOGI(STORAGE_TAG, "SPIFFS inicializado!");
    }
}
