/**
 * @file       bsp_led.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      BSP LED implementation — single WS2818 on ESP32-S3 Mini (GPIO 48)
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_led.h"

#include "drv_led.h"
#include "esp_log.h"

/* Private defines ---------------------------------------------------- */
static const char *TAG = "BSP_LED";

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static drv_led_handle_t s_led_handle = NULL;

/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */

base_status_t bsp_led_init(void)
{
    drv_led_config_t config = {
        .gpio_num = BSP_LED_GPIO_NUM,
        .num_leds = BSP_LED_COUNT,
    };

    base_status_t ret = drv_led_init(&config, &s_led_handle);
    if (ret != BS_OK)
    {
        ESP_LOGE(TAG, "drv_led_init failed");
        return BS_ERROR;
    }

    /* Ensure LED starts in off state */
    drv_led_clear(s_led_handle);

    ESP_LOGI(TAG, "Init OK (gpio=%d, count=%d)", BSP_LED_GPIO_NUM, BSP_LED_COUNT);
    return BS_OK;
}

base_status_t bsp_led_set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    CHECK(s_led_handle != NULL, BS_ERROR);

    CHECK_STATUS(drv_led_set_pixel(s_led_handle, 0, r, g, b));
    CHECK_STATUS(drv_led_refresh(s_led_handle));

    return BS_OK;
}

base_status_t bsp_led_clear(void)
{
    CHECK(s_led_handle != NULL, BS_ERROR);

    /* drv_led_clear() zeroes pixels and refreshes internally */
    return drv_led_clear(s_led_handle);
}

/* End of file -------------------------------------------------------- */
