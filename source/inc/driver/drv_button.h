/**
 * @file       drv_button.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     NEO
 * @brief      Button Driver
 * @note       Adapted from https://github.com/mauriciobarroso/button
 *             using espressif/button component
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DRV_BUTTON_H
#define __DRV_BUTTON_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */

/**
 * @brief Button event types
 *
 * Adapted from mauriciobarroso/button event model:
 *   SINGLE, DOUBLE, PRESSED, HOLD, LONG
 */
typedef enum
{
    DRV_BUTTON_EVENT_SINGLE_CLICK = 0,  /*!< Single click detected        */
    DRV_BUTTON_EVENT_DOUBLE_CLICK,      /*!< Double click detected         */
    DRV_BUTTON_EVENT_PRESS_DOWN,        /*!< Button pressed down           */
    DRV_BUTTON_EVENT_HOLD,              /*!< Button held after press       */
    DRV_BUTTON_EVENT_LONG_PRESS,        /*!< Long press detected           */
    DRV_BUTTON_EVENT_PRESS_UP,          /*!< Button released               */
    DRV_BUTTON_EVENT_MAX
} drv_button_event_t;

/**
 * @brief Button callback function type
 *
 * @param[in] event   Event that triggered the callback
 * @param[in] usr_data  User data pointer passed during registration
 */
typedef void (*drv_button_callback_t)(drv_button_event_t event, void *usr_data);

/**
 * @brief Button configuration
 */
typedef struct
{
    int     gpio_num;      /*!< GPIO pin number                       */
    uint8_t active_level;  /*!< GPIO level when button is pressed (0 or 1) */
} drv_button_config_t;

/**
 * @brief Opaque button handle
 */
typedef void *drv_button_handle_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Create and initialize a button instance
 *
 * @param[in]  config  Pointer to button configuration
 * @param[out] handle  Pointer to store the created button handle
 *
 * @return BS_OK    Button created successfully
 * @return BS_ERROR Button creation failed
 */
base_status_t drv_button_init(const drv_button_config_t *config, drv_button_handle_t *handle);

/**
 * @brief  Register a callback for a button event
 *
 * @param[in] handle    Button handle from drv_button_init()
 * @param[in] event     Event type to register callback for
 * @param[in] callback  Callback function
 * @param[in] usr_data  User data pointer passed to callback
 *
 * @return BS_OK    Callback registered successfully
 * @return BS_ERROR Registration failed
 */
base_status_t drv_button_register_callback(drv_button_handle_t  handle,
                                           drv_button_event_t   event,
                                           drv_button_callback_t callback,
                                           void                *usr_data);

/**
 * @brief  Unregister a callback for a button event
 *
 * @param[in] handle  Button handle from drv_button_init()
 * @param[in] event   Event type to unregister
 *
 * @return BS_OK    Callback unregistered successfully
 * @return BS_ERROR Unregistration failed
 */
base_status_t drv_button_unregister_callback(drv_button_handle_t handle,
                                             drv_button_event_t  event);

/**
 * @brief  Delete a button instance and free resources
 *
 * @param[in] handle  Button handle from drv_button_init()
 *
 * @return BS_OK    Button deleted successfully
 * @return BS_ERROR Deletion failed
 */
base_status_t drv_button_deinit(drv_button_handle_t handle);

/**
 * @brief  Get the current event of a button
 *
 * @param[in] handle  Button handle from drv_button_init()
 *
 * @return drv_button_event_t  Current button event
 */
drv_button_event_t drv_button_get_event(drv_button_handle_t handle);

/* -------------------------------------------------------------------- */
#endif /* __DRV_BUTTON_H */

/* End of file -------------------------------------------------------- */
