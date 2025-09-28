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
    // bsp_lcd_clock_set_mode();
}

void system_display_clock(void)
{
    static uint32_t tick_start = 0;

    if ((esp_timer_get_time() - tick_start) > 1000000)
    {
        tick_start = esp_timer_get_time();

        // Call bsp lcd display clock function
        bsp_lcd_clock_display();
    }
}

/* End of file -------------------------------------------------------- */
