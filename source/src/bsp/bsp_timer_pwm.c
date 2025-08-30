/**
 * @file       bsp_timer_pwm.c
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

/* Includes ----------------------------------------------------------- */
#include "bsp_timer_pwm.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
/*                Timer 3 implement                */
bool bsp_tim3_start_tim_it()
{
    if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
    {
        return false;
    }
    return true;
}

bool bsp_tim3_pwm_start()
{
    if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1) != HAL_OK || HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2) != HAL_OK
        || HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3) != HAL_OK)
    {
        return false;
    }
    return true;
}

void bsp_tim3_pwm_set_compare(uint32_t tim_channel, uint32_t value_ch_1)
{
    __HAL_TIM_SetCompare(&htim3, tim_channel, value_ch_1);
}

/*                Timer 4 implement                */
bool bsp_tim4_start_tim_it()
{
    if (HAL_TIM_Base_Start_IT(&htim4) != HAL_OK)
    {
        return false;
    }
    return true;
}

bool bsp_tim4_pwm_start()
{
    if (HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1) != HAL_OK)
    {
        return false;
    }
    return true;
}

void bsp_tim4_pwm_set_compare(uint32_t tim_channel, uint32_t value_ch_1)
{
    __HAL_TIM_SetCompare(&htim4, tim_channel, value_ch_1);
}

/*                Timer 9 implement                */
bool bsp_tim9_start_tim_it()
{
    if (HAL_TIM_Base_Start_IT(&htim9) != HAL_OK)
    {
        return false;
    }
    return true;
}

/*                Timer 11 implement                */
bool bsp_tim11_start_tim_it()
{
    if (HAL_TIM_Base_Start_IT(&htim11) != HAL_OK)
    {
        return false;
    }
    return true;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == htim9.Instance)
    {
        bsp_tim9_handle();
    }
    else if (htim->Instance == htim3.Instance)
    {
        bsp_tim3_handle();
    }
    else if (htim->Instance == htim4.Instance)
    {
        bsp_tim4_handle();
    }
    else if (htim->Instance == htim11.Instance)
    {
        bsp_tim11_handle();
    }
}

// weak function
__weak void bsp_tim3_handle()
{
    // do nothing
}

__weak void bsp_tim4_handle()
{
    // do nothing
}

__weak void bsp_tim9_handle()
{
    // do nothing
}

__weak void bsp_tim11_handle()
{
    // do nothing
}

void bsp_tim_set_prescale(TIM_HandleTypeDef *timer, uint32_t prescale)
{
    __HAL_TIM_SET_PRESCALER(timer, prescale);
}

void bsp_tim_set_autoreload(TIM_HandleTypeDef *timer, uint32_t autoreload)
{
    __HAL_TIM_SET_AUTORELOAD(timer, autoreload);
}

void bsp_delay(uint32_t ms)
{
    HAL_Delay(ms);
}

/* End of file -------------------------------------------------------- */
