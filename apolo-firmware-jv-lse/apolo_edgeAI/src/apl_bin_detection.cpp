#include <memory>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "apl_bin_detection.hpp"
#include "apl_camera.hpp"

#include "bin_model_data.h"
// #define BOUNDING_BOX_ENABLE
#define kTensorArenaSize (2 * 1024 * 1024)

uint16_t color[] = {
  0x0000,
  0x03E0,
  0x001F,
  0x7FE0,
  0xFFFF,
};

void apl_eval_bin_img(el_img_t *img, std::vector<box_results_t> &result_list)
{
    std::unique_ptr<edgelab::EngineTFLite> engine = std::make_unique<edgelab::EngineTFLite>();
    void *tensor_arena = heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    engine->init(tensor_arena, kTensorArenaSize);
    engine->load_model(g_bin_model_data, g_bin_model_data_len);
    std::unique_ptr<edgelab::AlgorithmYOLO> algorithm = std::make_unique<edgelab::AlgorithmYOLO>(engine.get());

    ApoloCamera::Get_camera_device().start_stream();
    ApoloCamera::Get_camera_device().get_frame(img);

    algorithm->run(img);
    uint32_t preprocess_time  = algorithm->get_preprocess_time();
    uint32_t run_time         = algorithm->get_run_time();
    uint32_t postprocess_time = algorithm->get_postprocess_time();
    uint8_t  i                = 0u;
    for (const auto& box : algorithm->get_results()) {
        box_results_t result = {
            .x = box.x,
            .y = box.y,
            .w = box.w,
            .h = box.h,
            .target = box.target,
            .score = box.score,
            };
                                
        edgelab::el_printf("box -> cx_cy_w_h: [%d, %d, %d, %d] t: [%d] s: [%d]\n",
                    result.x,
                    result.y,
                    result.w,
                    result.h,
                    result.target,
                    result.score);

        result_list.push_back(result);

        #if defined(BOUNDING_BOX_ENABLE)
            int16_t y = result.y - result.h / 2;
            int16_t x = result.x - result.w / 2;

            edgelab::el_draw_rect(img, x, y, box.w, box.h, color[++i % 5], 4);
        #endif
    }
    edgelab::el_printf("preprocess: %d, run: %d, postprocess: %d\n", preprocess_time, run_time, postprocess_time);
    ApoloCamera::Get_camera_device().stop_stream();
}
