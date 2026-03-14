/**
 * @file       drv_gpio.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     NEO
 * @brief      GPIO Driver
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "drv_gpio.h"

#include "driver/gpio.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */

base_status_t drv_gpio_init(const drv_gpio_config_t *config)
{
    if (config == NULL)
    {
        return BS_ERROR;
    }

    gpio_config_t gpio_cfg = {
        .pin_bit_mask = (1ULL << config->gpio_num),
        .mode         = (config->dir == DRV_GPIO_DIR_INPUT) ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT,
        .pull_up_en   = (config->pull == DRV_GPIO_PULL_UP)   ? GPIO_PULLUP_ENABLE   : GPIO_PULLUP_DISABLE,
        .pull_down_en = (config->pull == DRV_GPIO_PULL_DOWN) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };

    return (gpio_config(&gpio_cfg) == ESP_OK) ? BS_OK : BS_ERROR;
}

base_status_t drv_gpio_set_level(int gpio_num, drv_gpio_level_t level)
{
    return (gpio_set_level(gpio_num, (uint32_t)level) == ESP_OK) ? BS_OK : BS_ERROR;
}

drv_gpio_level_t drv_gpio_get_level(int gpio_num)
{
    return (gpio_get_level(gpio_num) == 0) ? DRV_GPIO_LEVEL_LOW : DRV_GPIO_LEVEL_HIGH;
}

/* End of file -------------------------------------------------------- */
