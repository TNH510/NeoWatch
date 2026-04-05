/**
 * @file       system_manager.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      System Manager
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_MANAGER_H
#define __SYSTEM_MANAGER_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"
#include "system_manager_events.h"
#include "freertos/queue.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize all subsystems and start the System Manager task
 */
void system_manager_init(void);

/**
 * @brief  Publish an event to the System Manager ingress queue
 *
 * Any subsystem calls this to send an event to the manager for routing.
 * Safe to call from task context. Do not call from ISR.
 *
 * @param[in] event  Pointer to event message
 *
 * @return BS_OK     Event published successfully
 * @return BS_ERROR  Queue full or invalid parameter
 */
base_status_t system_manager_publish(const sm_event_msg_t *event);

/**
 * @brief  Get the current system mode
 *
 * @return sm_mode_t  Current mode
 */
sm_mode_t system_manager_get_mode(void);

/* -------------------------------------------------------------------- */
#endif /* __SYS_MANAGER_H */

/* End of file -------------------------------------------------------- */
