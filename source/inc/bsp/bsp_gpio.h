/**
 * @file       bsp_gpio.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     NEO
 * @brief      BSP GPIO - Board-specific GPIO pin definitions
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"
#include "drv_gpio.h"

/* Public defines ----------------------------------------------------- */
#define BSP_GPIO_BUTTON_PIN     9   /*!< GPIO pin for the main button */
#define BSP_GPIO_BUTTON_ACTIVE  0   /*!< Active level (0 = active low) */

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize board-specific GPIO pins
 *
 * @return BS_OK    Initialization succeeded
 * @return BS_ERROR Initialization failed
 */
base_status_t bsp_gpio_init(void);

/* -------------------------------------------------------------------- */
#endif /* __BSP_GPIO_H */

/* End of file -------------------------------------------------------- */
