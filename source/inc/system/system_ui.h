/**
 * @file       system_ui.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      System UI — navigation state and RGB LED status indication
 * @note       Based on SDD_System_UI.md
 *             Consumes Q_UI commands from System Manager.
 *             Publishes result events back via system_manager_publish().
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_UI_H
#define __SYSTEM_UI_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"
#include "system_manager_events.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */

/**
 * @brief  LED indication states, ordered by priority (higher value = higher priority)
 *
 * Priority rules (from SDD section 5):
 *  - ERROR     overrides all lower states
 *  - CONNECTED overrides NORMAL
 *  - NORMAL    is the default (LED off)
 *  - Error indication persists until an explicit SYS_UI_CMD_LED_CLEAR
 */
typedef enum
{
    UI_LED_STATE_NORMAL    = 0, /*!< LED off — normal operation          */
    UI_LED_STATE_CONNECTED = 1, /*!< Green solid — app connected         */
    UI_LED_STATE_ERROR     = 2, /*!< Red flash 4 Hz — system error       */
    UI_LED_STATE_MAX
} ui_led_state_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize the System UI subsystem
 *
 * Initializes the BSP LED, creates Q_UI, and starts the UI task.
 * Must be called once from system_manager_init() before posting any commands.
 */
void system_ui_init(void);

/**
 * @brief  Send a command to System UI via Q_UI
 *
 * Called by System Manager to route UI commands. Non-blocking.
 * Safe to call from any task context (not ISR).
 *
 * @param[in] cmd   Command identifier
 * @param[in] arg0  Command argument 0 (command-specific, 0 if unused)
 * @param[in] arg1  Command argument 1 (command-specific, 0 if unused)
 *
 * @return BS_OK    Command enqueued successfully
 * @return BS_ERROR Queue full or subsystem not initialized
 */
base_status_t system_ui_send_cmd(system_ui_cmd_t cmd, uint8_t arg0, uint8_t arg1);

/**
 * @brief  Test function — blinks LED in blue every 500 ms
 *
 * Blocking function (runs forever). Call directly from app_main()
 * for standalone LED hardware verification. Does not use Q_UI or tasks.
 */
void system_ui_test(void);

/* -------------------------------------------------------------------- */
#endif /* __SYSTEM_UI_H */

/* End of file -------------------------------------------------------- */
