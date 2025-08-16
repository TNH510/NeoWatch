/**
 * @file       drv_led_rgb.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    0.0.1
 * @date       2023
 * @author     Quynh Le
 * @author     Quynh Le
 *
 * @brief      Driver for LED RGB
 *
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef INC_DRV_LED_RGB_H_
#define INC_DRV_LED_RGB_H_

/* Includes ----------------------------------------------------------- */
#include "bsp_timer_pwm.h"
#include "string.h"

/* Public defines ----------------------------------------------------- */
#define RED_CHANNEL   TIM_CHANNEL_1
#define GREEN_CHANNEL TIM_CHANNEL_2
#define BLUE_CHANNEL  TIM_CHANNEL_3
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize driver for led rgb
 *
 * @param[in]     None
 *
 * @attention  Should call this function fisrt before using other function
 *
 * @return
 *  - None
 */
void drv_led_init();

/**
 * @brief  Turn off LED RGB
 *
 * @param[in]
 *
 * @attention
 *
 * @return
 *  - None
 */
void drv_led_off();

/**
 * @brief  Set color for LED RGB
 *
 * @param[in]     buffer_color  Buffer store three value of R G B channel
 *
 * @attention  value of each R G B channel range from 0 to 255
 *
 * @return
 *  - None
 */
void drv_led_set_color(uint16_t *buffer_color);

/**
 * @brief  Blink LED RGB
 *
 * @param[in]     buffer_color  Buffer store three value of R G B channel
 *
 * @attention  Value of each R G B channel range from 0 to 255
 *             Frequency : DELAY_BLINK
 *
 * @return
 *  - None
 */
void drv_led_blink_color(uint16_t *buffer_color);

/**
 * @brief  Fade in Fade out mode for LED RGB
 *
 * @param[in]     None
 *
 * @attention  Fada in fade out mode base on color table in header file
 *             Frequency : DELAY_BLINK
 *
 * @return
 *  - None
 */
void drv_led_fade_in_fade_out();

#endif /* INC_DRV_LED_RGB_H_ */
