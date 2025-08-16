/**
 * @file       bsp_timer_pwm.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    0.0.1
 * @date       2023
 * @author     Quynh Le
 * @author     Quynh Le
 *
 * @brief      Board Support Package for Timer and PWM Peripheral
 *
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef INC_BSP_TIMER_PWM_H_
#define INC_BSP_TIMER_PWM_H_

/* Includes ----------------------------------------------------------- */
#include "stdbool.h"
#include "stm32f4xx_hal.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim11;
/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Set compare register for Timer3
 *
 * @param[in]    tim_channel channel of timer3
 * @param[in]    value_ch    value to set into compare register
 *
 * @attention    CHANNEL IN TIMER 3
 *
 * @return
 *  - None
 */
void bsp_tim3_pwm_set_compare(uint32_t tim_channel, uint32_t value_ch);

/**
 * @brief  Start PWM channel for  Timer 3
 *
 * @param[in]    None
 *
 * @attention   TIM_CHANNEL_1 , TIM_CHANNEL_2, TIM_CHANNEL_3
 *
 * @return
 *  - ok    : True
 *  - error : False
 */
bool bsp_tim3_pwm_start();

/**
 * @brief  Start timer 3 interupt mode
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - ok    : true
 *  - error : false
 */
bool bsp_tim3_start_tim_it();

/**
 * @brief  Timer 3 handle interupt
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - None
 */
void bsp_tim3_handle();

/**
 * @brief  Start PWM channel for  Timer 4
 *
 * @param[in]    None
 *
 * @attention   TIM_CHANNEL_1
 *
 * @return
 *  - ok    : True
 *  - error : False
 */
bool bsp_tim4_pwm_start();

/**
 * @brief  Start timer 4 interupt mode
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - ok    : true
 *  - error : false
 */
bool bsp_tim4_start_tim_it();

/**
 * @brief  Set compare register for Timer 4
 *
 * @param[in]    tim_channel channel of timer3
 * @param[in]    value_ch    value to set into compare register
 *
 * @attention    CHANNEL IN TIMER 4
 *
 * @return
 *  - None
 */
void bsp_tim4_pwm_set_compare(uint32_t tim_channel, uint32_t value_ch);

/**
 * @brief  Timer 4 handle interupt
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - None
 */
void bsp_tim4_handle();

/**
 * @brief  Timer 9 handle interupt
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - None
 */
void bsp_tim9_handle();

/**
 * @brief  Start timer 9 interupt mode
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - ok    : true
 *  - error : false
 */
bool bsp_tim9_start_tim_it();

/**
 * @brief  Timer 11 handle interupt
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - None
 */
void bsp_tim11_handle();

/**
 * @brief  Start timer 11 interupt mode
 *
 * @param[in]    None
 *
 * @attention
 *
 * @return
 *  - ok    : true
 *  - error : false
 */
bool bsp_tim11_start_tim_it();

/**
 * @brief  Set prescale for timer
 *
 * @param[in]    timer     timer channel
 * @param[in]    prescale  value to set to prescale register
 *
 * @attention
 *
 * @return
 *  - None
 */
void bsp_tim_set_prescale(TIM_HandleTypeDef *timer, uint32_t prescale);

/**
 * @brief  Set autoreload for timer
 *
 * @param[in]    timer       timer channel
 * @param[in]    autoreload  value to set to autoreload register
 *
 * @attention
 *
 * @return
 *  - None
 */
void bsp_tim_set_autoreload(TIM_HandleTypeDef *timer, uint32_t autoreload);

/**
 * @brief  Set delay time (miliseconds)
 *
 * @param[in]    ms    Time to delay
 *
 * @attention
 *
 * @return
 *  - None
 */
void bsp_delay(uint32_t ms);

#endif /* INC_BSP_TIMER_PWM_H_ */
