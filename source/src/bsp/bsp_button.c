/**
 * @file       bsp_button.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     NEO
 * @brief      BSP Button - Board-specific button initialization
 * @note       Adapted from https://github.com/mauriciobarroso/button
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_button.h"
#include "bsp_gpio.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static drv_button_handle_t s_button_handle = NULL;

/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */

base_status_t bsp_button_init(void)
{
    if (s_button_handle != NULL)
    {
        return BS_OK;
    }

    drv_button_config_t btn_cfg = {
        .gpio_num     = BSP_GPIO_BUTTON_PIN,
        .active_level = BSP_GPIO_BUTTON_ACTIVE,
    };

    CHECK_STATUS(drv_button_init(&btn_cfg, &s_button_handle));

    return BS_OK;
}

drv_button_handle_t bsp_button_get_handle(void)
{
    return s_button_handle;
}

base_status_t bsp_button_register_callback(drv_button_event_t    event,
                                           drv_button_callback_t callback,
                                           void                 *usr_data)
{
    if (s_button_handle == NULL)
    {
        return BS_ERROR;
    }

    return drv_button_register_callback(s_button_handle, event, callback, usr_data);
}

base_status_t bsp_button_unregister_callback(drv_button_event_t event)
{
    if (s_button_handle == NULL)
    {
        return BS_ERROR;
    }

    return drv_button_unregister_callback(s_button_handle, event);
}

/* End of file -------------------------------------------------------- */
