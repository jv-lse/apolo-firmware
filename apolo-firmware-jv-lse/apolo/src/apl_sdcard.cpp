#include <stdio.h>

#include "esp_vfs.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#include "apl_camera.hpp"
#include "apl_sdcard.hpp"

#include <time.h>
#include "esp_log.h"
#include "dirent.h"

#define SDCARD_PIN_MISO (40)  //MTDO -> SD/DATA
#define SDCARD_PIN_MOSI (38)  // SD/CMD
#define SDCARD_PIN_CLK  (39)  //MTCK -> SD/CLK
#define SDCARD_PIN_CS   (47)  // SD/CS

static const char *SD_CARD_TAG = "SD_CARD";

void apl_save_file(char *file_name, uint8_t *file_buf, size_t file_size)
{
    if(file_name == NULL)
    {
	    ESP_LOGE(SD_CARD_TAG, "Nome de arquivo invalido");
        return;
    }

    FILE *file = fopen(file_name, "w");
    if (file == NULL)
    {
	    ESP_LOGE(SD_CARD_TAG, "Falha ao abrir o arquivo");
        return;
    }
    else
    {
        fwrite(file_buf, 1, file_size, file);
        fclose(file);
	    ESP_LOGI(SD_CARD_TAG, "Imagem salva!");
    }
}

esp_err_t apl_card_mout()
{
	esp_err_t ret;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {};
    
    bus_cfg.mosi_io_num = SDCARD_PIN_MOSI;
    bus_cfg.miso_io_num = SDCARD_PIN_MISO;
    bus_cfg.sclk_io_num = SDCARD_PIN_CLK;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = 4000;

    ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(SD_CARD_TAG, "Falha ao inicializar o barramento SPI");
        return ESP_FAIL;
    }
    ESP_LOGI(SD_CARD_TAG, "Inicializando o SDCard");
    ESP_LOGI(SD_CARD_TAG, "Usando o periferico SPI");
    
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs   = (gpio_num_t)SDCARD_PIN_CS;
    slot_config.host_id = SPI2_HOST;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {};

    mount_config.format_if_mount_failed = true;
    mount_config.max_files = 5;
    mount_config.allocation_unit_size = 16 * 1024;

    sdmmc_card_t* card;
    ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(SD_CARD_TAG, "Falha ao montar o filesystem"
                "Se voce quiser que o cartao seja formatado, defina format_if_mount_failed = true.");
        } else {
            ESP_LOGE(SD_CARD_TAG, "Falha ao inicializar o SDCard (%d). "
                "Certifique-se de que as linhas do cartao SD tenham resistores de pull-up em seus devidos lugares.", ret);
        }
        return ESP_FAIL;
    }
	ESP_LOGI(SD_CARD_TAG, "Filesystem montado");

    return ret;
}


uint64_t get_next_filename(char *next_filename, size_t max_len) {
    DIR *dir;
    struct dirent *ent;
    uint64_t max_number = 0;

    // Abrir o diretório
    if ((dir = opendir("/sdcard")) != NULL) {
        // Loop através de todos os arquivos no diretório
        while ((ent = readdir(dir)) != NULL) {
            // Verificar se o arquivo é um arquivo .jpg
            if (strstr(ent->d_name, ".jpg") != NULL) {
                // Extrair o número do nome do arquivo
                uint64_t number;
                sscanf(ent->d_name, "apl_img_%lld.jpg", &number);
                number++;
                // printf("number -> %lld\n",number);
                // Se o número for maior que o máximo encontrado até agora, atualize o máximo
                if (number > max_number) {
                    max_number = number;
                    // printf("max_number -> %lld\n",max_number);
                }
            }
        }
        closedir(dir);
        
    }

    // Construir o próximo nome de arquivo
    // snprintf(next_filename, max_len, "/sdcard/apl_img_%lld.jpg", max_number + 1);
    return max_number;
}

