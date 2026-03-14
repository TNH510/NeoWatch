/**
 * @file       drv_gpio.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     NEO
 * @brief      GPIO Driver
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DRV_GPIO_H
#define __DRV_GPIO_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */

/**
 * @brief GPIO direction
 */
typedef enum
{
    DRV_GPIO_DIR_INPUT = 0,
    DRV_GPIO_DIR_OUTPUT
} drv_gpio_dir_t;

/**
 * @brief GPIO pull mode
 */
typedef enum
{
    DRV_GPIO_PULL_NONE = 0,
    DRV_GPIO_PULL_UP,
    DRV_GPIO_PULL_DOWN
} drv_gpio_pull_t;

/**
 * @brief GPIO level
 */
typedef enum
{
    DRV_GPIO_LEVEL_LOW = 0,
    DRV_GPIO_LEVEL_HIGH
} drv_gpio_level_t;

/**
 * @brief GPIO configuration structure
 */
typedef struct
{
    int      gpio_num;  /*!< GPIO pin number */
    drv_gpio_dir_t  dir;       /*!< GPIO direction  */
    drv_gpio_pull_t pull;      /*!< GPIO pull mode  */
} drv_gpio_config_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize a GPIO pin
 *
 * @param[in] config  Pointer to GPIO configuration
 *
 * @return BS_OK    Initialization succeeded
 * @return BS_ERROR Initialization failed
 */
base_status_t drv_gpio_init(const drv_gpio_config_t *config);

/**
 * @brief  Set GPIO output level
 *
 * @param[in] gpio_num  GPIO pin number
 * @param[in] level     Output level (LOW or HIGH)
 *
 * @return BS_OK    Set succeeded
 * @return BS_ERROR Set failed
 */
base_status_t drv_gpio_set_level(int gpio_num, drv_gpio_level_t level);

/**
 * @brief  Get GPIO input level
 *
 * @param[in]  gpio_num  GPIO pin number
 *
 * @return DRV_GPIO_LEVEL_LOW   GPIO level is low
 * @return DRV_GPIO_LEVEL_HIGH  GPIO level is high
 */
drv_gpio_level_t drv_gpio_get_level(int gpio_num);

/* -------------------------------------------------------------------- */
#endif /* __DRV_GPIO_H */

/* End of file -------------------------------------------------------- */
