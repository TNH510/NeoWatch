/**
 * @file       bsp_gpio.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     NEO
 * @brief      BSP GPIO - Board-specific GPIO initialization
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_gpio.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */

base_status_t bsp_gpio_init(void)
{
    drv_gpio_config_t button_gpio_cfg = {
        .gpio_num = BSP_GPIO_BUTTON_PIN,
        .dir      = DRV_GPIO_DIR_INPUT,
        .pull     = DRV_GPIO_PULL_UP,
    };

    CHECK_STATUS(drv_gpio_init(&button_gpio_cfg));

    return BS_OK;
}

/* End of file -------------------------------------------------------- */
