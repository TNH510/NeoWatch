/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUTTON_3_Pin GPIO_PIN_2
#define BUTTON_3_GPIO_Port GPIOC
#define BUTTON_3_EXTI_IRQn EXTI2_IRQn
#define BUTTON_6_Pin GPIO_PIN_3
#define BUTTON_6_GPIO_Port GPIOC
#define BUTTON_6_EXTI_IRQn EXTI3_IRQn
#define R_Pin GPIO_PIN_6
#define R_GPIO_Port GPIOC
#define G_Pin GPIO_PIN_7
#define G_GPIO_Port GPIOC
#define B_Pin GPIO_PIN_8
#define B_GPIO_Port GPIOC
#define BUTTON_1_Pin GPIO_PIN_15
#define BUTTON_1_GPIO_Port GPIOA
#define BUTTON_1_EXTI_IRQn EXTI15_10_IRQn
#define BUTTON_4_Pin GPIO_PIN_10
#define BUTTON_4_GPIO_Port GPIOC
#define BUTTON_4_EXTI_IRQn EXTI15_10_IRQn
#define BUTTON_5_Pin GPIO_PIN_12
#define BUTTON_5_GPIO_Port GPIOC
#define BUTTON_5_EXTI_IRQn EXTI15_10_IRQn
#define BUZZER_Pin GPIO_PIN_6
#define BUZZER_GPIO_Port GPIOB
#define BUTTON_2_Pin GPIO_PIN_7
#define BUTTON_2_GPIO_Port GPIOB
#define BUTTON_2_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
