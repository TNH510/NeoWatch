
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/task.h"
#include "nimble-nordic-uart.h"
#include "base_type.h"
#include "bsp_i2c.h"
#include "bsp_lcd.h"
#include "system_display.h"
#include "bsp_rtc.h"

void app_main(void)
{
    while (1)
    {
        // Delay forever
        vTaskDelay(portMAX_DELAY);
    }
}