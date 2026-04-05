/**
 * @file       system_button.c
 * @copyright  Copyright (C) 2026 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2026-03
 * @author     Hieu Tran Ngoc
 * @brief      System Button
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "system_button.h"
#include "system_manager.h"
#include "bsp_button.h"
#include "esp_timer.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
static void system_button_task(void *pvParameters);
static void m_publish_button_event(system_button_event_t btn_event);

// Callback function for button event
static void m_button_click_callback(drv_button_event_t event, void *usr_data);
static void m_button_double_callback(drv_button_event_t event, void *usr_data);
static void m_button_hold_callback(drv_button_event_t event, void *usr_data);
// static void m_button_long_press_callback(drv_button_event_t event, void *usr_data);
static void m_button_press_up_callback(drv_button_event_t event, void *usr_data);

/* Function definitions ----------------------------------------------- */
base_status_t system_button_init(void)
{
    // Create system button main thread
    if (xTaskCreate(system_button_task, "system_button", 4096, NULL, 10, NULL) != pdPASS)
    {
        return BS_ERROR;
    }

    // Register button callback for single click event
    if (bsp_button_register_callback(DRV_BUTTON_EVENT_SINGLE_CLICK, m_button_click_callback, NULL) != BS_OK)
    {
        return BS_ERROR;
    }

    // Register button callback for double click event
    if (bsp_button_register_callback(DRV_BUTTON_EVENT_DOUBLE_CLICK, m_button_double_callback, NULL) != BS_OK)
    {
        return BS_ERROR;
    }

    // Register button callback for hold event
    if (bsp_button_register_callback(DRV_BUTTON_EVENT_HOLD, m_button_hold_callback, NULL) != BS_OK)
    {
        return BS_ERROR;
    }

    // // Register button callback for double click event
    // if (bsp_button_register_callback(DRV_BUTTON_EVENT_LONG_PRESS, m_button_long_press_callback, NULL) != BS_OK)
    // {
    //     return BS_ERROR;
    // }

    // Register button callback for double click event
    if (bsp_button_register_callback(DRV_BUTTON_EVENT_PRESS_UP, m_button_press_up_callback, NULL) != BS_OK)
    {
        return BS_ERROR;
    }

    return BS_OK;
}

static void system_button_task(void *pvParameters)
{
    (void) pvParameters;
    /* Button callbacks handle all work; task kept for future use */
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void m_publish_button_event(system_button_event_t btn_event)
{
    sm_event_msg_t msg = {
        .source       = SM_SRC_BUTTON,
        .event_id     = (uint32_t) btn_event,
        .timestamp_ms = (uint32_t)(esp_timer_get_time() / 1000),
    };
    system_manager_publish(&msg);
}

static void m_button_click_callback(drv_button_event_t event, void *usr_data)
{
    (void) event;
    (void) usr_data;
    ESP_LOGI("SYSTEM_BUTTON", "Button Single Clicked!");
    m_publish_button_event(SYS_BUTTON_EVENT_CLICK);
}

static void m_button_double_callback(drv_button_event_t event, void *usr_data)
{
    (void) event;
    (void) usr_data;
    ESP_LOGI("SYSTEM_BUTTON", "Button Double Clicked!");
    m_publish_button_event(SYS_BUTTON_EVENT_DOUBLE_CLICK);
}

static void m_button_hold_callback(drv_button_event_t event, void *usr_data)
{
    (void) event;
    (void) usr_data;
    ESP_LOGI("SYSTEM_BUTTON", "Button Held!");
    m_publish_button_event(SYS_BUTTON_EVENT_HOLD_1S);
}

// static void m_button_long_press_callback(drv_button_event_t event, void *usr_data)
// {
//     (void) event;
//     (void) usr_data;
//     ESP_LOGI("SYSTEM_BUTTON", "Button Long Pressed!");
// }

static void m_button_press_up_callback(drv_button_event_t event, void *usr_data)
{
    (void) event;
    (void) usr_data;
    ESP_LOGI("SYSTEM_BUTTON", "Button Pressed Up!");
}

/* End of file -------------------------------------------------------- */
