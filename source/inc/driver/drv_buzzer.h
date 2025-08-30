/**
 * @file       drv_buzzer.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    0.0.1
 * @date       2023
 * @author     Quynh Le
 * @author     Quynh Le
 *
 * @brief      Driver for Buzzer
 *
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef INC_DRV_BUZZER_H_
#define INC_DRV_BUZZER_H_

/* Includes ----------------------------------------------------------- */
#include "bsp_timer_pwm.h"
#include "stdlib.h"

/* Public defines ----------------------------------------------------- */
#define BUZZER_CHANNEL TIM_CHANNEL_1
/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
    NO_MUSSIC,
    HAPPY_BIRTHDAY,
    NOKIA_TUNE,
    Merry_CHRISTMAS,
    Super_Mario
} buzzer_music_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize for driver
 *
 * @param[in]    None
 *
 * @attention Should call this function first before using other function
 *
 * @return
 *  - None
 */
void drv_buzzer_init();

/**
 * @brief  Turn on buzzer
 *
 * @param[in]    None
 *
 * @attention   Buzzer keep sounding continuously
 *
 * @return
 *  - None
 */
void drv_buzzer_on();

/**
 * @brief  Adjust volume
 *
 * @param[in]    volume
 *
 * @attention Volume unit range from 0 to 10
 *
 * @return
 *  - None
 */
void drv_buzzer_volume(uint16_t volume);

/**
 * @brief  Blink buzzer
 *
 * @param[in]    frequency    Frequency of blink
 *
 * @attention  Frequency unit range from 0 to 10 (Hz)
 *
 * @return
 *  - None
 */
void drv_buzzer_blink(uint16_t frequency);

/**
 * @brief  Choose music for buzzer
 *
 * @param[in]    choose_music      which tune to be played
 *
 * @attention   type buzzer_music_t is declared in this file
 *
 * @return
 *  - None
 */
void drv_buzzer_music(uint8_t choose_music);

/**
 * @brief  Turn off buzzer
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - None
 */
void drv_buzzer_off();

#endif /* INC_DRV_BUZZER_H_ */
