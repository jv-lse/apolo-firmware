#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbedtls/base64.h"

#include "apl_camera.hpp"
#include "apl_sdcard.hpp"

#include "esp_log.h"
#include "dirent.h"

#define CAM_DEFAULT_WIDTH (480)
#define CAM_DEFAULT_HEIGHT (320)

#if defined(APOLO_MIAS_PINOUT)
    #define APL_CAM_PIN_D0 (17)
    #define APL_CAM_PIN_D1 (9)
    #define APL_CAM_PIN_D2 (10)
    #define APL_CAM_PIN_D3 (11)
    #define APL_CAM_PIN_D4 (12)
    #define APL_CAM_PIN_D5 (13)
    #define APL_CAM_PIN_D6 (48)
    #define APL_CAM_PIN_D7 (45)

    #define APL_CAM_PIN_XCLK (21)
    #define APL_CAM_PIN_PCLK (14)
    #define APL_CAM_PIN_VSYNC (6)
    #define APL_CAM_PIN_HREF (7)

    #define APL_CAM_PIN_SIOD (4)
    #define APL_CAM_PIN_SIOC (5)
    #define APL_CAM_PIN_PWDN (-1)
    #define APL_CAM_PIN_RESET (18)

#elif defined(LILYGO_PINOUT)
    #define APL_CAM_PIN_D0 (11)
    #define APL_CAM_PIN_D1 (9)
    #define APL_CAM_PIN_D2 (8)
    #define APL_CAM_PIN_D3 (10)
    #define APL_CAM_PIN_D4 (12)
    #define APL_CAM_PIN_D5 (17)
    #define APL_CAM_PIN_D6 (16)
    #define APL_CAM_PIN_D7 (15)

    #define APL_CAM_PIN_XCLK (14)
    #define APL_CAM_PIN_PCLK (13)
    #define APL_CAM_PIN_VSYNC (6)
    #define APL_CAM_PIN_HREF (7)

    #define APL_CAM_PIN_SIOD (4)
    #define APL_CAM_PIN_SIOC (5)
    #define APL_CAM_PIN_PWDN (-1)
    #define APL_CAM_PIN_RESET (-1)
#endif

#define APL_CAM_XCLK_FREQ_HZ (15000000)

#define APL_CAM_LEDC_CHANNEL (LEDC_CHANNEL_0)
#define APL_CAM_LEDC_TIMER (LEDC_TIMER_0)


#define FILENAME_MAX_LEN 50

static const char *APL_CAM_TAG = "APOLO_CAM";
static const char *TAG = "LIST_FILES";

DIR *dir;
struct dirent *ent;
const char *path = "/sdcard";
int64_t highest_index = -1;
char filename[20];
static uint64_t load_value;

// APOLO CAMERA RELATED =====================================================================
ApoloCamera& ApoloCamera::Get_camera_device()
{
    static ApoloCamera cam_instance;
    return cam_instance;
} 

el_err_code_t ApoloCamera::init(size_t width, size_t height) 
{
    

    camera_config_t config = {};
    config.ledc_channel = APL_CAM_LEDC_CHANNEL;
    config.ledc_timer   = APL_CAM_LEDC_TIMER;
    config.pin_d0       = APL_CAM_PIN_D0;
    config.pin_d1       = APL_CAM_PIN_D1;
    config.pin_d2       = APL_CAM_PIN_D2;
    config.pin_d3       = APL_CAM_PIN_D3;
    config.pin_d4       = APL_CAM_PIN_D4;
    config.pin_d5       = APL_CAM_PIN_D5;
    config.pin_d6       = APL_CAM_PIN_D6;
    config.pin_d7       = APL_CAM_PIN_D7;
    config.pin_xclk     = APL_CAM_PIN_XCLK;
    config.pin_pclk     = APL_CAM_PIN_PCLK;
    config.pin_vsync    = APL_CAM_PIN_VSYNC;
    config.pin_href     = APL_CAM_PIN_HREF;
    config.pin_sccb_sda = APL_CAM_PIN_SIOD;
    config.pin_sccb_scl = APL_CAM_PIN_SIOC;
    config.pin_pwdn     = APL_CAM_PIN_PWDN;
    config.pin_reset    = APL_CAM_PIN_RESET; 
    config.xclk_freq_hz = APL_CAM_XCLK_FREQ_HZ;
    config.frame_size   = FRAMESIZE_HD;
    config.pixel_format = PIXFORMAT_RGB565;
    config.jpeg_quality = APL_CAM_JPEG_QUALITY;
    config.fb_count     = 1;
    config.fb_location  = CAMERA_FB_IN_PSRAM;
    config.grab_mode    = CAMERA_GRAB_LATEST;

    #if defined(APOLO_MIAS_PINOUT)
        esp_rom_gpio_pad_select_gpio(CAMERA_SUPPLY_PIN);
        gpio_set_direction(CAMERA_SUPPLY_PIN, GPIO_MODE_OUTPUT);
        gpio_set_level(CAMERA_SUPPLY_PIN, APL_STATE_HIGH);
    #endif

    vTaskDelay(10);

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        ESP_LOGE(APL_CAM_TAG, "[Camera] falha na inicializacao: erro: 0x%x", err);
        return EL_EIO;
    }
    ESP_LOGI(APL_CAM_TAG, "[Camera] Camera inicializada!",);

    this->apply_sensor_default_settings();

    this->take_picture();
    this->clear_picture();
    this->take_picture();

    this->_is_present = true;

    #ifdef SDCARD_SAVE
    apl_card_mout();
    
    // Abre o diretório
    dir = opendir(path);
    if (dir == NULL) {
        ESP_LOGE(TAG, "Falha ao abrir o diretório %s", path);
        // return;
    }

    // Itera sobre os arquivos no diretório
    while ((ent = readdir(dir)) != NULL) 
    {
        printf("%s\n", ent->d_name);
        // inicio da nova implementacao
        char *filename = ent->d_name;
    }

    closedir(dir);
    // printf("valor encontrado: %lld\n",highest_index);
    #endif

    return EL_OK;
}

el_err_code_t ApoloCamera::init()
{
    return init(CAM_DEFAULT_WIDTH, CAM_DEFAULT_HEIGHT); 
}

el_err_code_t ApoloCamera::deinit()
{
    this->_is_present = !(esp_camera_deinit() == ESP_OK);
    ESP_LOGW(APL_CAM_TAG, "[Camera] Camera deinicializada!",);
    return this->_is_present ? EL_AGAIN : EL_OK;
}

void ApoloCamera::clear_picture()
{
    if(this->fb != NULL)
    {
        esp_camera_fb_return(this->fb);
    }
}

el_err_code_t ApoloCamera::start_stream()
{
    clear_picture();
    fb = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(APL_CAM_TAG, "[Camera] capture failed");
        return EL_EIO;
    }
    this->_is_streaming = true;
    return EL_OK;
}

el_err_code_t ApoloCamera::stop_stream()
{
    if (this->_is_streaming) [[likely]] {
        esp_camera_fb_return(fb);
        this->_is_streaming = false;
    return EL_OK;
}
return EL_ELOG;
}

el_err_code_t ApoloCamera::get_frame(el_img_t* img)
{
    if (!this->_is_streaming) {
        return EL_EIO;
    }
    if (!fb) {
        ESP_LOGE(APL_CAM_TAG, "[Camera] capture failed");
        this->_is_streaming = false;
        return EL_EIO;
    }
    img->width  = fb->width;
    img->height = fb->height;
    img->data   = fb->buf;
    img->size   = fb->len;
    img->format = EL_PIXEL_FORMAT_RGB565;

    #ifdef SDCARD_SAVE
    this->save_picture();
    #endif

    return EL_OK;
}

void ApoloCamera::take_picture()
{
    
    clear_picture();
    this->fb = esp_camera_fb_get();
    ESP_LOGI(APL_CAM_TAG, "captura de imagem realizada | tamanho: %zu bytes", this->get_image_leght());
}

void ApoloCamera::apply_sensor_default_settings()
{
    this->cam_sensor.cam_sensor_default_confgs();
}

void ApoloCamera::save_picture()
{
    uint8_t *outbuffer = NULL;
    size_t outsize = 0;
    char timestamp_value[20];
    char next_filename[FILENAME_MAX_LEN];
    fmt2jpg(this->fb->buf, this->fb->width * this->fb->height * 2, this->fb->width, this->fb->height, this->fb->format, 50, &outbuffer, &outsize);
    static uint64_t picture_counter = 0;
    char photo_name[50];

    uint64_t index_photo = get_next_filename(next_filename,FILENAME_MAX_LEN);
    // get_next_filename(next_filename, FILENAME_MAX_LEN);
    sprintf(photo_name, "/sdcard/apl_img_%lld.jpg",index_photo);
    // get_timestamp(timestamp_value, sizeof(timestamp_value));
    // sprintf(photo_name, "/sdcard/%s.jpg", timestamp_value);
    // sprintf(photo_name, "/sdcard/apl_img_%lld.jpg", picture_counter++);
    // sprintf(photo_name, "/sdcard/teste.txt");
    ESP_LOGI(APL_CAM_TAG, "nome do arquivo da foto: %s", photo_name);
    apl_save_file(photo_name, outbuffer, outsize);
}



uint8_t* ApoloCamera::JPEG_to_base64()
{
    uint8_t *outbuffer = NULL;
    size_t outsize = 0;

    if(this->fb == NULL)
    {
        ESP_LOGE(APL_CAM_TAG, "falha ao obter uma imagem");
        return NULL;
    }

    #ifdef SDCARD_SAVE
    this->save_picture();
    #endif

    if (PIXFORMAT_JPEG != this->fb->format) {
        fmt2jpg(this->fb->buf, this->fb->width * this->fb->height * 2, this->fb->width, this->fb->height, this->fb->format, 50, &outbuffer, &outsize);
    } else {
        outbuffer = this->fb->buf;
        outsize = this->fb->len;
    }

    uint8_t *base64_buf = (uint8_t *)calloc(1, outsize * 4);
    if (NULL != base64_buf) {
        size_t out_len = 0;
        mbedtls_base64_encode(base64_buf, outsize * 4, &out_len, outbuffer, outsize);

        if (PIXFORMAT_JPEG != this->fb->format) {
            free(outbuffer);
        }
    } else {
        ESP_LOGE(APL_CAM_TAG, "falha no malloc para o buffer base64");
    }

    return base64_buf;
}

void ApoloCamera::free_webserver_img(uint8_t *base64_buf)
{
    free((void *)base64_buf);
}

size_t ApoloCamera::get_image_leght() const
{
    return this->fb->len;
}

uint8_t* ApoloCamera::get_image_buf() const
{
    return this->fb->buf;
}

framesize_t ApoloCamera::fit_resolution(size_t width, size_t height)
{
    framesize_t res = FRAMESIZE_INVALID;
    return res;
}

ApoloCamera::ApoloCamera(): cam_sensor{ApoloCamSensor()} {}

// APOLO SENSOR RELATED =====================================================================
ApoloCamSensor::ApoloCamSensor(): m_cam_sensor{NULL} {}

void ApoloCamSensor::set_brightness(int8_t brightness)
{
    if(brightness < -2) brightness = -2;
    if(brightness > 2) brightness = 2;

    m_cam_sensor->set_brightness(m_cam_sensor, brightness);
}

void ApoloCamSensor::set_contrast(int8_t contrast)
{
    if(contrast < -2) contrast = -2;
    if(contrast > 2) contrast = 2;

    m_cam_sensor->set_contrast(m_cam_sensor, contrast);
}

void ApoloCamSensor::set_saturation(int8_t saturation)
{
    if(saturation < -2) saturation = -2;
    if(saturation > 2) saturation = 2;

    m_cam_sensor->set_saturation(m_cam_sensor, saturation);
}

void ApoloCamSensor::set_special_effect(uint8_t effect)
{
    if(effect < 6) effect = 0;

    m_cam_sensor->set_special_effect(m_cam_sensor, effect);
}

void ApoloCamSensor::set_white_balance(bool state)
{
    m_cam_sensor->set_whitebal(m_cam_sensor, state);
}

void ApoloCamSensor::set_white_balance_gain(bool state)
{
    m_cam_sensor->set_awb_gain(m_cam_sensor, state);
}

void ApoloCamSensor::set_white_balance_mode(uint8_t mode)
{
    if(mode > 4) mode = 0;

    m_cam_sensor->set_wb_mode(m_cam_sensor, mode);
}

void ApoloCamSensor::set_exposure_control(bool state)
{
    m_cam_sensor->set_exposure_ctrl(m_cam_sensor, state);
}

void ApoloCamSensor::set_exposure_time(uint16_t time)
{
    if(time > 1200) time = 1200;

    m_cam_sensor->set_aec_value(m_cam_sensor, time);
}

void ApoloCamSensor::set_horizontal_flip(bool state)
{
    m_cam_sensor->set_hmirror(m_cam_sensor, state);
}

void ApoloCamSensor::set_vertical_flip(bool state)
{
    m_cam_sensor->set_vflip(m_cam_sensor, state);
}

void ApoloCamSensor::cam_sensor_default_confgs()
{
    m_cam_sensor = esp_camera_sensor_get();

    this->set_vertical_flip(0);
    this->set_exposure_control(0);
    this->set_exposure_time(1000);

    if (m_cam_sensor->id.PID == OV3660_PID) {
        this->set_brightness(1);
        this->set_saturation(-2);
    }

    ESP_LOGI(APL_CAM_TAG, "Configuracoes padroes do sensor de camera aplicadas!");
}
