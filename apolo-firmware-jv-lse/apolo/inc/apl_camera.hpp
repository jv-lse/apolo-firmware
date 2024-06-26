#ifndef APOLO_CAMERA_H
#define APOLO_CAMERA_H

#include "esp_camera.h"
#include "apolo_essentials.h"

#include "core/edgelab.h"
#include "porting/el_camera.h"

#define APL_CAM_JPEG_QUALITY (12)
#define CAMERA_SUPPLY_PIN (GPIO_NUM_8)

// BOARD PINOUT 
#define APOLO_MIAS_PINOUT
// #define LILYGO_PINOUT

//SDCARD SAVE FLAG
// #define SDCARD_SAVE

class ApoloCamSensor
{
public:
    ApoloCamSensor();

    void set_brightness(int8_t brightness);
    void set_contrast(int8_t contrast);
    void set_saturation(int8_t saturation);
    void set_special_effect(uint8_t effect);
    void set_white_balance(bool state);
    void set_white_balance_gain(bool state);
    void set_white_balance_mode(uint8_t mode);
    void set_exposure_control(bool state);
    void set_exposure_time(uint16_t time);
    void set_horizontal_flip(bool state);
    void set_vertical_flip(bool state);

    void cam_sensor_default_confgs();

private:
    sensor_t *m_cam_sensor;
};

class ApoloCamera : public edgelab::Camera {
public:
    static ApoloCamera& Get_camera_device(); 

    el_err_code_t init(size_t width, size_t height) override;
    el_err_code_t init();
    el_err_code_t deinit() override;

    void clear_picture();
    el_err_code_t start_stream() override;
    el_err_code_t stop_stream() override;
    el_err_code_t get_frame(el_img_t* img) override;

    void take_picture();
    void save_picture();
    void apply_sensor_default_settings();

    uint8_t *JPEG_to_base64();
    void free_webserver_img(uint8_t *base64_buf);

    size_t get_image_leght() const;
    uint8_t* get_image_buf() const;

    ApoloCamera& operator=(const ApoloCamera&) = delete;
    ApoloCamera(const ApoloCamera&) = delete;
    ~ApoloCamera() = default;

   protected:
    framesize_t fit_resolution(size_t width, size_t height);

   private:
    ApoloCamera();
    ApoloCamSensor cam_sensor;

    camera_config_t config;
    camera_fb_t*    fb;
    sensor_t*       sensor;
};

#endif
