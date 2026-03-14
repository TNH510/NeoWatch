/**
 * @file       bsp_button.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     NEO
 * @brief      BSP Button - Board-specific button initialization
 * @note       Adapted from https://github.com/mauriciobarroso/button
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"
#include "drv_button.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize the board button
 *
 * Creates and configures the button instance using the board-specific
 * GPIO pin and active level settings.
 *
 * @return BS_OK    Button initialized successfully
 * @return BS_ERROR Button initialization failed
 */
base_status_t bsp_button_init(void);

/**
 * @brief  Get the button handle
 *
 * @return drv_button_handle_t  Button handle, or NULL if not initialized
 */
drv_button_handle_t bsp_button_get_handle(void);

/**
 * @brief  Register a callback for a button event
 *
 * @param[in] event     Event type to register callback for
 * @param[in] callback  Callback function
 * @param[in] usr_data  User data pointer passed to callback
 *
 * @return BS_OK    Callback registered successfully
 * @return BS_ERROR Registration failed
 */
base_status_t bsp_button_register_callback(drv_button_event_t    event,
                                           drv_button_callback_t callback,
                                           void                 *usr_data);

/**
 * @brief  Unregister a callback for a button event
 *
 * @param[in] event  Event type to unregister
 *
 * @return BS_OK    Callback unregistered successfully
 * @return BS_ERROR Unregistration failed
 */
base_status_t bsp_button_unregister_callback(drv_button_event_t event);

/* -------------------------------------------------------------------- */
#endif /* __BSP_BUTTON_H */

/* End of file -------------------------------------------------------- */
