/**
 * @file       drv_led.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      WS2812/WS2818 LED strip driver implementation
 * @note       Uses espressif/led_strip component (RMT backend)
 */

/* Includes ----------------------------------------------------------- */
#include "drv_led.h"

#include "led_strip.h"
#include "esp_log.h"
#include <stdlib.h>

/* Private defines ---------------------------------------------------- */
#define DRV_LED_TAG "DRV_LED"

/* RMT clock: 10 MHz gives 100 ns resolution, sufficient for WS2812 timing */
#define DRV_LED_RMT_RESOLUTION_HZ (10 * 1000 * 1000)

/* Private enumerate/structure ---------------------------------------- */

/**
 * @brief  Internal driver context — wraps the led_strip handle
 */
typedef struct
{
    led_strip_handle_t strip;
    uint32_t           num_leds;
} drv_led_ctx_t;

/* Private macros ----------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static const char *TAG = DRV_LED_TAG;

/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */

base_status_t drv_led_init(const drv_led_config_t *config, drv_led_handle_t *handle)
{
    CHECK(config != NULL, BS_ERROR);
    CHECK(handle != NULL, BS_ERROR);
    CHECK(config->num_leds > 0, BS_ERROR);

    drv_led_ctx_t *ctx = (drv_led_ctx_t *) malloc(sizeof(drv_led_ctx_t));
    CHECK(ctx != NULL, BS_ERROR);

    led_strip_config_t strip_config = {
        .strip_gpio_num         = config->gpio_num,
        .max_leds               = config->num_leds,
        .led_model              = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src       = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = DRV_LED_RMT_RESOLUTION_HZ,
    };

    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &ctx->strip);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "led_strip_new_rmt_device failed: %s", esp_err_to_name(err));
        free(ctx);
        return BS_ERROR;
    }

    ctx->num_leds = config->num_leds;
    *handle       = (drv_led_handle_t) ctx;

    ESP_LOGI(TAG, "Init OK: gpio=%d, leds=%lu", config->gpio_num,
             (unsigned long) config->num_leds);
    return BS_OK;
}

base_status_t drv_led_set_pixel(drv_led_handle_t handle, uint32_t index,
                                uint8_t r, uint8_t g, uint8_t b)
{
    CHECK(handle != NULL, BS_ERROR);

    drv_led_ctx_t *ctx = (drv_led_ctx_t *) handle;
    CHECK(index < ctx->num_leds, BS_ERROR);

    esp_err_t err = led_strip_set_pixel(ctx->strip, index, r, g, b);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "set_pixel[%lu] failed: %s", (unsigned long) index,
                 esp_err_to_name(err));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t drv_led_set_all(drv_led_handle_t handle, uint8_t r, uint8_t g, uint8_t b)
{
    CHECK(handle != NULL, BS_ERROR);

    drv_led_ctx_t *ctx = (drv_led_ctx_t *) handle;

    for (uint32_t i = 0; i < ctx->num_leds; i++)
    {
        esp_err_t err = led_strip_set_pixel(ctx->strip, i, r, g, b);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "set_all[%lu] failed: %s", (unsigned long) i,
                     esp_err_to_name(err));
            return BS_ERROR;
        }
    }

    return BS_OK;
}

base_status_t drv_led_clear(drv_led_handle_t handle)
{
    CHECK(handle != NULL, BS_ERROR);

    drv_led_ctx_t *ctx = (drv_led_ctx_t *) handle;

    /* led_strip_clear() zeroes all pixels and refreshes the hardware */
    esp_err_t err = led_strip_clear(ctx->strip);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "clear failed: %s", esp_err_to_name(err));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t drv_led_refresh(drv_led_handle_t handle)
{
    CHECK(handle != NULL, BS_ERROR);

    drv_led_ctx_t *ctx = (drv_led_ctx_t *) handle;

    esp_err_t err = led_strip_refresh(ctx->strip);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "refresh failed: %s", esp_err_to_name(err));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t drv_led_deinit(drv_led_handle_t handle)
{
    CHECK(handle != NULL, BS_ERROR);

    drv_led_ctx_t *ctx = (drv_led_ctx_t *) handle;

    esp_err_t err = led_strip_del(ctx->strip);
    free(ctx);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "del failed: %s", esp_err_to_name(err));
        return BS_ERROR;
    }

    return BS_OK;
}

/* End of file -------------------------------------------------------- */
