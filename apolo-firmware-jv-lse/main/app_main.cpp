#include "apolo_main.hpp"

extern "C" void app_main(void)
{
    apolo_main_init();

    while(1)
    {
        vTaskDelay(10 * 1000);
    }
}
