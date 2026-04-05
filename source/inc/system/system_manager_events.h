/**
 * @file       system_manager_events.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      Shared event/message definitions for System Manager routing
 * @note       All subsystems include this header to publish/consume events
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_MANAGER_EVENTS_H
#define __SYSTEM_MANAGER_EVENTS_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
#define SM_QUEUE_DEPTH_DEFAULT (32)
#define SM_QUEUE_DEPTH_UI      (64)

/* ====================================================================
 * Event source identifiers
 * ==================================================================== */
typedef enum
{
    SM_SRC_BUTTON = 0,
    SM_SRC_DISPLAY,
    SM_SRC_UI,
    SM_SRC_NETWORK,
    SM_SRC_SETTINGS,
    SM_SRC_MANAGER,
    SM_SRC_MAX
} sm_event_source_t;

/* ====================================================================
 * Button events (Button -> Manager)
 * ==================================================================== */
typedef enum
{
    SYS_BUTTON_EVENT_CLICK = 0,
    SYS_BUTTON_EVENT_DOUBLE_CLICK,
    SYS_BUTTON_EVENT_HOLD_1S,
    SYS_BUTTON_EVENT_MAX
} system_button_event_t;

/* ====================================================================
 * Display events (Manager -> Display)
 * ==================================================================== */
typedef enum
{
    SYS_DISPLAY_EVENT_MODE_BOOT = 0,
    SYS_DISPLAY_EVENT_MODE_STANDBY,
    SYS_DISPLAY_EVENT_MODE_MENU,
    SYS_DISPLAY_EVENT_MODE_SETTING,
    SYS_DISPLAY_EVENT_REFRESH,
    SYS_DISPLAY_EVENT_BRIGHTNESS_SET,
    SYS_DISPLAY_EVENT_SLEEP,
    SYS_DISPLAY_EVENT_WAKE,
    SYS_DISPLAY_EVENT_MAX
} system_display_event_t;

/* ====================================================================
 * UI commands (Manager -> UI) and events (UI -> Manager)
 * ==================================================================== */
typedef enum
{
    SYS_UI_CMD_MODE_BOOT = 0,
    SYS_UI_CMD_MODE_STANDBY,
    SYS_UI_CMD_MODE_MENU,
    SYS_UI_CMD_MODE_SETTING,
    SYS_UI_CMD_LED_SET_STATE,
    SYS_UI_CMD_LED_CLEAR,
    SYS_UI_CMD_APP_CONNECTED,
    SYS_UI_CMD_APP_DISCONNECTED,
    SYS_UI_CMD_ERROR_STATE,
    SYS_UI_CMD_MAX
} system_ui_cmd_t;

typedef enum
{
    SYS_UI_EVT_MODE_CHANGED = 0,
    SYS_UI_EVT_LED_STATE_APPLIED,
    SYS_UI_EVT_ERROR,
    SYS_UI_EVT_MAX
} system_ui_evt_t;

/* ====================================================================
 * Settings commands (Manager -> Settings) and events (Settings -> Manager)
 * ==================================================================== */
typedef enum
{
    SYS_SET_CMD_SET_BRIGHTNESS = 0,
    SYS_SET_CMD_SET_CLOCK_TYPE,
    SYS_SET_CMD_SET_TIMEZONE,
    SYS_SET_CMD_GET_ALL,
    SYS_SET_CMD_FACTORY_RESET,
    SYS_SET_CMD_MAX
} system_settings_cmd_t;

typedef enum
{
    SYS_SET_EVT_UPDATED = 0,
    SYS_SET_EVT_READBACK,
    SYS_SET_EVT_FACTORY_RESET_DONE,
    SYS_SET_EVT_VALIDATION_FAILED,
    SYS_SET_EVT_PERSIST_FAILED,
    SYS_SET_EVT_MAX
} system_settings_evt_t;

/* ====================================================================
 * System Manager modes
 * ==================================================================== */
typedef enum
{
    SM_MODE_BOOT = 0,
    SM_MODE_STANDBY,
    SM_MODE_MENU,
    SM_MODE_SETTING,
    SM_MODE_MAX
} sm_mode_t;

/* ====================================================================
 * Unified event message posted to System Manager ingress queue.
 * All subsystems use this same structure to publish events.
 * ==================================================================== */
typedef struct
{
    sm_event_source_t source;
    uint16_t          event_id;   /* Enum value from subsystem event type */
    uint32_t          timestamp_ms;
    union
    {
        /* Button payload */
        struct
        {
            uint8_t button_id;
        } button;

        /* Display payload */
        struct
        {
            uint8_t brightness;
            uint8_t mode;
        } display;

        /* UI payload */
        struct
        {
            uint8_t arg0;
            uint8_t arg1;
        } ui;

        /* Settings payload */
        struct
        {
            uint8_t  brightness;
            uint8_t  clock_type;
            int8_t   time_zone;
        } settings;

    } data;
} sm_event_msg_t;

/* -------------------------------------------------------------------- */
#endif /* __SYSTEM_MANAGER_EVENTS_H */

/* End of file -------------------------------------------------------- */
