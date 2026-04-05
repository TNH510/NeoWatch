/**
 * @file       bsp_led.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      Board Support Package — onboard RGB LED (WS2818, DIN = GPIO 48)
 * @note       ESP32-S3 Mini module, single WS2818 LED
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_LED_H
#define __BSP_LED_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
#define BSP_LED_GPIO_NUM (48) /*!< DIN pin of the onboard WS2818 LED */
#define BSP_LED_COUNT    (1)  /*!< Number of LEDs on the board       */

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize the onboard RGB LED
 *
 * Configures the LED strip driver with board-specific settings.
 * Must be called before any bsp_led_set_rgb() or bsp_led_clear() calls.
 *
 * @return BS_OK    Success
 * @return BS_ERROR Initialization failed
 */
base_status_t bsp_led_init(void);

/**
 * @brief  Set the LED to an RGB color and immediately refresh
 *
 * @param[in] r  Red   component (0–255)
 * @param[in] g  Green component (0–255)
 * @param[in] b  Blue  component (0–255)
 *
 * @return BS_OK    Success
 * @return BS_ERROR Driver not initialized or hardware error
 */
base_status_t bsp_led_set_rgb(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief  Turn off the LED
 *
 * @return BS_OK    Success
 * @return BS_ERROR Driver not initialized or hardware error
 */
base_status_t bsp_led_clear(void);

/* -------------------------------------------------------------------- */
#endif /* __BSP_LED_H */

/* End of file -------------------------------------------------------- */
