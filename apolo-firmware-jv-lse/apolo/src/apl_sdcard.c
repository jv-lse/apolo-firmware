#include <stdio.h>

#include "esp_vfs.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#include "apl_camera.h"
#include "apl_sdcard.h"

#define SDCARD_PIN_MISO (40)
#define SDCARD_PIN_MOSI (38)
#define SDCARD_PIN_CLK  (39)
#define SDCARD_PIN_CS   (47)

static const char *SD_CARD_TAG = "SD_CARD";
esp_err_t card_mout()
{
	esp_err_t ret;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SDCARD_PIN_MOSI,
        .miso_io_num = SDCARD_PIN_MISO,
        .sclk_io_num = SDCARD_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(SD_CARD_TAG, "Falha ao inicializar o barramento SPI");
        return ESP_FAIL;
    }
    ESP_LOGI(SD_CARD_TAG, "Inicializando o SDCard");
    ESP_LOGI(SD_CARD_TAG, "Usando o periferico SPI");
    
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs   = SDCARD_PIN_CS;
    slot_config.host_id = SPI2_HOST;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
		.allocation_unit_size = 16 * 1024,
    };

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

// void apl_save_picture(char *file_name)
// {
//     picture_infos_t pic_info;
//     if(file_name == NULL)
//     {
// 	    ESP_LOGE(SD_CARD_TAG, "Nome de arquivo invalido");
//         return;
//     }

//     FILE *file = fopen(file_name, "w");
//     if (file == NULL)
//     {
// 	    ESP_LOGE(SD_CARD_TAG, "Falha ao abrir o arquivo");
//         return;
//     }
//     else
//     {
//         apl_get_picture_info(&pic_info);
//         fwrite(pic_info.buf, 1, pic_info.len, file);
//         fclose(file);
// 	    ESP_LOGI(SD_CARD_TAG, "Imagem salva!");
//     }
// }
