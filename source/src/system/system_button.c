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
#include "bsp_button.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
static void system_button_task(void *pvParameters);

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
    // Log hello world every second
    while (1)
    {
        ESP_LOGI("SYSTEM_BUTTON", "Hello World from System Button!");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void m_button_click_callback(drv_button_event_t event, void *usr_data)
{
    (void) event;
    (void) usr_data;
    ESP_LOGI("SYSTEM_BUTTON", "Button Single Clicked!");
}

static void m_button_double_callback(drv_button_event_t event, void *usr_data)
{
    (void) event;
    (void) usr_data;
    ESP_LOGI("SYSTEM_BUTTON", "Button Double Clicked!");
}

static void m_button_hold_callback(drv_button_event_t event, void *usr_data)
{
    (void) event;
    (void) usr_data;
    ESP_LOGI("SYSTEM_BUTTON", "Button Held!");
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
