/**
 * @file       drv_led.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      WS2812/WS2818 LED Strip Driver (espressif/led_strip, RMT backend)
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DRV_LED_H
#define __DRV_LED_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */

/**
 * @brief  LED strip driver configuration
 */
typedef struct
{
    int      gpio_num; /*!< GPIO pin connected to DIN of the LED strip */
    uint32_t num_leds; /*!< Number of LEDs in the strip                */
} drv_led_config_t;

/**
 * @brief  Opaque LED strip handle
 */
typedef void *drv_led_handle_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize the LED strip driver and allocate resources
 *
 * @param[in]  config  Pointer to driver configuration
 * @param[out] handle  Pointer to store the created handle
 *
 * @return BS_OK    Success
 * @return BS_ERROR Initialization failed
 */
base_status_t drv_led_init(const drv_led_config_t *config, drv_led_handle_t *handle);

/**
 * @brief  Set a single pixel to the given RGB color
 *
 * Call drv_led_refresh() after all pixels are set.
 *
 * @param[in] handle  LED strip handle
 * @param[in] index   Pixel index (0-based, must be < num_leds)
 * @param[in] r       Red   (0–255)
 * @param[in] g       Green (0–255)
 * @param[in] b       Blue  (0–255)
 *
 * @return BS_OK    Success
 * @return BS_ERROR Failed
 */
base_status_t drv_led_set_pixel(drv_led_handle_t handle, uint32_t index,
                                uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief  Set all pixels to the same RGB color
 *
 * Call drv_led_refresh() after calling this function.
 *
 * @param[in] handle  LED strip handle
 * @param[in] r       Red   (0–255)
 * @param[in] g       Green (0–255)
 * @param[in] b       Blue  (0–255)
 *
 * @return BS_OK    Success
 * @return BS_ERROR Failed
 */
base_status_t drv_led_set_all(drv_led_handle_t handle, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief  Turn off all pixels and immediately refresh the hardware
 *
 * @param[in] handle  LED strip handle
 *
 * @return BS_OK    Success
 * @return BS_ERROR Failed
 */
base_status_t drv_led_clear(drv_led_handle_t handle);

/**
 * @brief  Push pending pixel data to the LED strip hardware
 *
 * Must be called after drv_led_set_pixel() or drv_led_set_all().
 * Not needed after drv_led_clear() which refreshes internally.
 *
 * @param[in] handle  LED strip handle
 *
 * @return BS_OK    Success
 * @return BS_ERROR Failed
 */
base_status_t drv_led_refresh(drv_led_handle_t handle);

/**
 * @brief  Delete the LED strip instance and free all resources
 *
 * @param[in] handle  LED strip handle
 *
 * @return BS_OK    Success
 * @return BS_ERROR Failed
 */
base_status_t drv_led_deinit(drv_led_handle_t handle);

/* -------------------------------------------------------------------- */
#endif /* __DRV_LED_H */

/* End of file -------------------------------------------------------- */
