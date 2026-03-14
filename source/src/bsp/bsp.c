/**
 * @file       bsp.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      BSP General
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "bsp.h"
#include "bsp_i2c.h"
#include "bsp_rtc.h"
#include "bsp_lcd.h"
#include "bsp_gpio.h"
#include "bsp_button.h"
#include "base_type.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
void bsp_init(void)
{
    // Initialize all BSP components
    // CHECK_STATUS(bsp_i2c_init());
    bsp_i2c_init();
    bsp_lcd_init();
    bsp_gpio_init();
    bsp_button_init();
    // CHECK_STATUS(bsp_rtc_init());
    // CHECK_STATUS(bsp_lcd_init());
}

/* End of file -------------------------------------------------------- */
