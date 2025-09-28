/**
 * @file       system_display.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-09-28
 * @author     Thach Nguyen Ba Vu
 * @brief      system display
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "system_display.h"
#include "bsp_lcd.h"
#include "esp_timer.h"
#include "bsp_rtc.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
void system_display_init(void)
{
    bsp_lcd_init();
    bsp_rtc_init();
    // bsp_lcd_clock_set_mode();
}

void system_display_clock(void)
{
    static int64_t tick_start = 0;
    int64_t now = esp_timer_get_time();

    if ((now - tick_start) > 1000000) // 1 second interval
    {
        tick_start = now;

        // Call bsp lcd display clock function
        bsp_lcd_clock_display(1, 1, 1, 1, 1, 1);
    }
    
    // Add a small delay to prevent watchdog timeout
    vTaskDelay(1); // Yield to other tasks
}

/* End of file -------------------------------------------------------- */
