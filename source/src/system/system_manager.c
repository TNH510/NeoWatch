/**
 * @file       system_manager.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    2.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      System Manager — central event router
 * @note       Based on SDD_System_Manager.md queue architecture
 */

/* Includes ----------------------------------------------------------- */
#include "system_manager.h"

#include "system_display.h"
#include "system_settings.h"
#include "system_button.h"

/* Private defines ---------------------------------------------------- */
#define SM_TAG              "SYS_MGR"
#define SM_TASK_STACK_SIZE  (4096)
#define SM_TASK_PRIORITY    (10)
#define SM_INGRESS_DEPTH    (32)
#define SM_QUEUE_WAIT_MS    (portMAX_DELAY)

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static QueueHandle_t s_ingress_queue = NULL;
static sm_mode_t     s_mode          = SM_MODE_BOOT;

/* Private function prototypes ---------------------------------------- */
static void system_manager_task(void *pvParameters);
static void sm_handle_button_event(const sm_event_msg_t *evt);
static void sm_handle_display_event(const sm_event_msg_t *evt);
static void sm_handle_ui_event(const sm_event_msg_t *evt);
static void sm_handle_settings_event(const sm_event_msg_t *evt);
static void sm_set_mode(sm_mode_t new_mode);

/* Function definitions ----------------------------------------------- */
void system_manager_init(void)
{
    /* Create ingress queue */
    s_ingress_queue = xQueueCreate(SM_INGRESS_DEPTH, sizeof(sm_event_msg_t));
    if (s_ingress_queue == NULL)
    {
        ESP_LOGE(SM_TAG, "Failed to create ingress queue");
        return;
    }

    /* Init subsystems */
    system_settings_init();
    system_button_init();

    /* Create manager task */
    if (xTaskCreate(system_manager_task, "system_manager",
                    SM_TASK_STACK_SIZE, NULL, SM_TASK_PRIORITY, NULL) != pdPASS)
    {
        ESP_LOGE(SM_TAG, "Failed to create manager task");
        return;
    }

    ESP_LOGI(SM_TAG, "System Manager initialized");
}

base_status_t system_manager_publish(const sm_event_msg_t *event)
{
    if ((event == NULL) || (s_ingress_queue == NULL))
    {
        return BS_ERROR;
    }

    if (xQueueSend(s_ingress_queue, event, pdMS_TO_TICKS(10)) != pdTRUE)
    {
        ESP_LOGW(SM_TAG, "Ingress queue full, event dropped (src=%d, id=%d)",
                 event->source, event->event_id);
        return BS_ERROR;
    }

    return BS_OK;
}

sm_mode_t system_manager_get_mode(void)
{
    return s_mode;
}

/* Private function definitions --------------------------------------- */

/**
 * @brief  Main event loop — dequeue and route events
 */
static void system_manager_task(void *pvParameters)
{
    sm_event_msg_t evt;

    /* Enter boot mode */
    sm_set_mode(SM_MODE_BOOT);

    /* Transition to standby after boot */
    sm_set_mode(SM_MODE_STANDBY);

    for (;;)
    {
        if (xQueueReceive(s_ingress_queue, &evt, SM_QUEUE_WAIT_MS) == pdTRUE)
        {
            ESP_LOGD(SM_TAG, "Event: src=%d id=%d ts=%lu",
                     evt.source, evt.event_id, (unsigned long) evt.timestamp_ms);

            switch (evt.source)
            {
            case SM_SRC_BUTTON:
                sm_handle_button_event(&evt);
                break;

            case SM_SRC_DISPLAY:
                sm_handle_display_event(&evt);
                break;

            case SM_SRC_UI:
                sm_handle_ui_event(&evt);
                break;

            case SM_SRC_SETTINGS:
                sm_handle_settings_event(&evt);
                break;

            default:
                ESP_LOGW(SM_TAG, "Unknown event source: %d", evt.source);
                break;
            }
        }   
    }
}

/**
 * @brief  Handle button events and route to appropriate subsystems
 */
static void sm_handle_button_event(const sm_event_msg_t *evt)
{
    switch ((system_button_event_t) evt->event_id)
    {
    case SYS_BUTTON_EVENT_CLICK:
        ESP_LOGI(SM_TAG, "Button click -> route to UI/Display");
        /* Route: navigate forward in current mode */
        if (s_mode == SM_MODE_STANDBY)
        {
            sm_set_mode(SM_MODE_MENU);
        }
        break;

    case SYS_BUTTON_EVENT_DOUBLE_CLICK:
        ESP_LOGI(SM_TAG, "Button double click -> shortcut action");
        /* Route: toggle between menu and standby */
        if (s_mode == SM_MODE_MENU)
        {
            sm_set_mode(SM_MODE_STANDBY);
        }
        else if (s_mode == SM_MODE_STANDBY)
        {
            sm_set_mode(SM_MODE_MENU);
        }
        break;

    case SYS_BUTTON_EVENT_HOLD_1S:
        ESP_LOGI(SM_TAG, "Button hold 1s -> mode/state transition");
        /* Route: enter settings from menu, or return to standby */
        if (s_mode == SM_MODE_MENU)
        {
            sm_set_mode(SM_MODE_SETTING);
        }
        else if (s_mode == SM_MODE_SETTING)
        {
            sm_set_mode(SM_MODE_STANDBY);
        }
        break;

    default:
        break;
    }
}

/**
 * @brief  Handle display response events
 */
static void sm_handle_display_event(const sm_event_msg_t *evt)
{
    ESP_LOGD(SM_TAG, "Display event id=%d", evt->event_id);
    /* Display completion events can be handled here in the future */
}

/**
 * @brief  Handle UI response events
 */
static void sm_handle_ui_event(const sm_event_msg_t *evt)
{
    ESP_LOGD(SM_TAG, "UI event id=%d", evt->event_id);
    /* UI state change events can be handled here in the future */
}

/**
 * @brief  Handle settings response events
 */
static void sm_handle_settings_event(const sm_event_msg_t *evt)
{
    switch ((system_settings_evt_t) evt->event_id)
    {
    case SYS_SET_EVT_UPDATED:
        ESP_LOGI(SM_TAG, "Settings updated -> refresh display");
        /* Could route display refresh here */
        break;

    case SYS_SET_EVT_FACTORY_RESET_DONE:
        ESP_LOGI(SM_TAG, "Factory reset done -> reboot to boot mode");
        sm_set_mode(SM_MODE_BOOT);
        break;

    case SYS_SET_EVT_VALIDATION_FAILED:
        ESP_LOGW(SM_TAG, "Settings validation failed");
        break;

    case SYS_SET_EVT_PERSIST_FAILED:
        ESP_LOGE(SM_TAG, "Settings persist failed");
        break;

    default:
        break;
    }
}

/**
 * @brief  Transition to a new system mode and log
 */
static void sm_set_mode(sm_mode_t new_mode)
{
    if (new_mode >= SM_MODE_MAX)
    {
        return;
    }

    static const char *mode_names[] = {"BOOT", "STANDBY", "MENU", "SETTING"};

    ESP_LOGI(SM_TAG, "Mode: %s -> %s", mode_names[s_mode], mode_names[new_mode]);
    s_mode = new_mode;

    /* Future: post mode event to Q_DIS and Q_UI here */
}

/* End of file -------------------------------------------------------- */
