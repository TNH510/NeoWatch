neo-/**
 * @file       bsp_touch_button.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-09
 * @author     Hieu Tran Ngoc
 * @brief      Board support package for touch button
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "bsp_touch_button.h"
#include "iot_button.h"
// Removed touch sensor dependencies to fix build errors
// #include "touch_button.h"
// #include "touch_sensor_lowlevel.h"

static const char *TAG = "BSP_TOUCH";

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */

/**
 * @brief Initialize touch button (stub implementation)
 * 
 * @return ESP_OK on success
 */
esp_err_t bsp_touch_button_init(void)
{
    ESP_LOGI(TAG, "Touch button disabled (not supported on this device)");
    return ESP_OK;
}

/* End of file -------------------------------------------------------- */
