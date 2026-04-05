/**
 * @file       system_ui.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      System UI implementation
 * @note       Based on SDD_System_UI.md
 *
 *  Architecture:
 *    System Manager  --(cmd)-->  Q_UI  --(dequeue)-->  system_ui_task
 *    system_ui_task  --(evt)-->  system_manager_publish()
 *
 *  LED state machine (priority, highest wins):
 *    UI_LED_STATE_ERROR     — red flash 4 Hz (125 ms half-period)
 *    UI_LED_STATE_CONNECTED — green solid
 *    UI_LED_STATE_NORMAL    — off
 *
 *  Flash timing is driven by xQueueReceive() timeout, avoiding timers.
 */

/* Includes ----------------------------------------------------------- */
#include "system_ui.h"

#include "bsp_led.h"
#include "system_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_random.h"
#include "freertos/task.h"

/* Private defines ---------------------------------------------------- */
#define UI_TAG                    "SYS_UI"
#define UI_TASK_STACK_SIZE        (4096)
#define UI_TASK_PRIORITY          (8)
#define UI_QUEUE_DEPTH            (64)
#define UI_QUEUE_SEND_TIMEOUT_MS  (10)

/* 4 Hz error flash: half-period = 125 ms (on 125 ms, off 125 ms) */
#define UI_LED_FLASH_HALF_PERIOD_MS (125)

/* Private enumerate/structure ---------------------------------------- */

/**
 * @brief  Internal Q_UI message (Manager -> UI)
 */
typedef struct
{
    system_ui_cmd_t cmd;
    uint32_t        timestamp_ms;
    uint8_t         arg0;
    uint8_t         arg1;
} system_ui_cmd_msg_t;

/* Private macros ----------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static const char    *TAG         = UI_TAG;
static QueueHandle_t  s_ui_queue  = NULL;
static ui_led_state_t s_led_state = UI_LED_STATE_NORMAL;
static sm_mode_t      s_ui_mode   = SM_MODE_BOOT;
static bool_t         s_flash_on  = BS_FALSE;

/* Private function prototypes ---------------------------------------- */
static void          system_ui_task(void *pvParameters);
static void          ui_handle_cmd(const system_ui_cmd_msg_t *msg);
static void          ui_set_led_state(ui_led_state_t new_state);
static base_status_t ui_apply_led(void);
static void          ui_publish_evt(system_ui_evt_t evt, base_status_t status);

/* Function definitions ----------------------------------------------- */

void system_ui_init(void)
{
    /* Initialize board LED hardware */
    if (bsp_led_init() != BS_OK)
    {
        ESP_LOGE(TAG, "BSP LED init failed — LED feedback unavailable");
    }

    s_ui_queue = xQueueCreate(UI_QUEUE_DEPTH, sizeof(system_ui_cmd_msg_t));
    if (s_ui_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create Q_UI");
        return;
    }

    if (xTaskCreate(system_ui_task, "system_ui",
                    UI_TASK_STACK_SIZE, NULL, UI_TASK_PRIORITY, NULL) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to create UI task");
        return;
    }

    ESP_LOGI(TAG, "System UI initialized");
}

base_status_t system_ui_send_cmd(system_ui_cmd_t cmd, uint8_t arg0, uint8_t arg1)
{
    CHECK(s_ui_queue != NULL, BS_ERROR);
    CHECK(cmd < SYS_UI_CMD_MAX, BS_ERROR);

    system_ui_cmd_msg_t msg = {
        .cmd          = cmd,
        .timestamp_ms = (uint32_t) (xTaskGetTickCount() * portTICK_PERIOD_MS),
        .arg0         = arg0,
        .arg1         = arg1,
    };

    if (xQueueSend(s_ui_queue, &msg, pdMS_TO_TICKS(UI_QUEUE_SEND_TIMEOUT_MS)) != pdTRUE)
    {
        ESP_LOGW(TAG, "Q_UI full — cmd=%d dropped", cmd);
        return BS_ERROR;
    }

    return BS_OK;
}

/* Private function definitions --------------------------------------- */

/**
 * @brief  Main UI task
 *
 * When in error state, the queue receive timeout is set to the flash half-period
 * so the task wakes up every 125 ms to toggle the red LED.
 * All other states block indefinitely until a command arrives.
 */
static void system_ui_task(void *pvParameters)
{
    system_ui_cmd_msg_t msg;
    TickType_t          wait_ticks;

    for (;;)
    {
        wait_ticks = (s_led_state == UI_LED_STATE_ERROR)
                         ? pdMS_TO_TICKS(UI_LED_FLASH_HALF_PERIOD_MS)
                         : portMAX_DELAY;

        if (xQueueReceive(s_ui_queue, &msg, wait_ticks) == pdTRUE)
        {
            ui_handle_cmd(&msg);
        }
        else
        {
            /* Timeout — drive the 4 Hz error flash toggle */
            if (s_led_state == UI_LED_STATE_ERROR)
            {
                s_flash_on = NOT(s_flash_on);
                if (s_flash_on == BS_TRUE)
                {
                    bsp_led_set_rgb(255, 0, 0);
                }
                else
                {
                    bsp_led_clear();
                }
            }
        }
    }
}

/**
 * @brief  Dispatch a single UI command
 *
 * Priority rules enforced here (SDD section 5):
 *  - SYS_UI_CMD_ERROR_STATE  — always overrides, highest priority
 *  - SYS_UI_CMD_APP_CONNECTED — applies only when not in error
 *  - SYS_UI_CMD_APP_DISCONNECTED — drops only the CONNECTED state
 *  - SYS_UI_CMD_LED_CLEAR    — explicit clear, resets even error state
 */
static void ui_handle_cmd(const system_ui_cmd_msg_t *msg)
{
    ESP_LOGD(TAG, "Cmd=%d arg0=%d arg1=%d", msg->cmd, msg->arg0, msg->arg1);

    switch (msg->cmd)
    {
    /* ---- Mode transitions ------------------------------------------ */
    case SYS_UI_CMD_MODE_BOOT:
        s_ui_mode = SM_MODE_BOOT;
        ESP_LOGI(TAG, "Mode -> BOOT");
        ui_publish_evt(SYS_UI_EVT_MODE_CHANGED, BS_OK);
        break;

    case SYS_UI_CMD_MODE_STANDBY:
        s_ui_mode = SM_MODE_STANDBY;
        ESP_LOGI(TAG, "Mode -> STANDBY");
        ui_publish_evt(SYS_UI_EVT_MODE_CHANGED, BS_OK);
        break;

    case SYS_UI_CMD_MODE_MENU:
        s_ui_mode = SM_MODE_MENU;
        ESP_LOGI(TAG, "Mode -> MENU");
        ui_publish_evt(SYS_UI_EVT_MODE_CHANGED, BS_OK);
        break;

    case SYS_UI_CMD_MODE_SETTING:
        s_ui_mode = SM_MODE_SETTING;
        ESP_LOGI(TAG, "Mode -> SETTING");
        ui_publish_evt(SYS_UI_EVT_MODE_CHANGED, BS_OK);
        break;

    /* ---- Notification (placeholder — no display coupling here) ----- */
    case SYS_UI_CMD_SHOW_NOTIFICATION:
        ESP_LOGI(TAG, "Show notification (payload_id=%d)", msg->arg0);
        ui_publish_evt(SYS_UI_EVT_NOTIFICATION_ACK, BS_OK);
        break;

    case SYS_UI_CMD_HIDE_NOTIFICATION:
        ESP_LOGI(TAG, "Hide notification");
        ui_publish_evt(SYS_UI_EVT_NOTIFICATION_ACK, BS_OK);
        break;

    /* ---- LED state commands ---------------------------------------- */
    case SYS_UI_CMD_ERROR_STATE:
        /* Highest priority — always applies */
        ESP_LOGE(TAG, "Error state -> red flash");
        ui_set_led_state(UI_LED_STATE_ERROR);
        break;

    case SYS_UI_CMD_APP_CONNECTED:
        /* Applies only when error is not active */
        if (s_led_state < UI_LED_STATE_ERROR)
        {
            ESP_LOGI(TAG, "App connected -> green solid");
            ui_set_led_state(UI_LED_STATE_CONNECTED);
        }
        else
        {
            ESP_LOGW(TAG, "APP_CONNECTED ignored — error active");
        }
        break;

    case SYS_UI_CMD_APP_DISCONNECTED:
        /* Only drops the connected indication; never clears error */
        if (s_led_state == UI_LED_STATE_CONNECTED)
        {
            ESP_LOGI(TAG, "App disconnected -> LED off");
            ui_set_led_state(UI_LED_STATE_NORMAL);
        }
        break;

    case SYS_UI_CMD_LED_SET_STATE:
        /* arg0 encodes the requested ui_led_state_t */
        if (msg->arg0 < UI_LED_STATE_MAX)
        {
            ui_led_state_t req = (ui_led_state_t) msg->arg0;
            /* Apply only if it is an upgrade; error can only be cleared
               by explicit LED_CLEAR */
            if (req >= s_led_state)
            {
                ui_set_led_state(req);
            }
            else
            {
                ESP_LOGW(TAG, "LED_SET_STATE=%d blocked by current state=%d",
                         req, s_led_state);
            }
        }
        else
        {
            ESP_LOGW(TAG, "LED_SET_STATE: invalid arg0=%d", msg->arg0);
            ui_publish_evt(SYS_UI_EVT_ERROR, BS_ERROR);
        }
        break;

    case SYS_UI_CMD_LED_CLEAR:
        /* Explicit clear — resets all LED indication including error */
        ESP_LOGI(TAG, "LED clear -> off");
        ui_set_led_state(UI_LED_STATE_NORMAL);
        break;

    /* ---- Unknown command ------------------------------------------- */
    default:
        ESP_LOGW(TAG, "Unknown cmd=%d — ignored", msg->cmd);
        ui_publish_evt(SYS_UI_EVT_ERROR, BS_ERROR);
        break;
    }
}

/**
 * @brief  Apply a new LED state unconditionally and publish result
 *
 * The caller is responsible for all priority checks before calling this.
 */
static void ui_set_led_state(ui_led_state_t new_state)
{
    s_led_state = new_state;
    s_flash_on  = BS_FALSE;

    if (ui_apply_led() == BS_OK)
    {
        ui_publish_evt(SYS_UI_EVT_LED_STATE_APPLIED, BS_OK);
    }
    else
    {
        ESP_LOGE(TAG, "LED apply failed for state=%d — fallback off", new_state);
        bsp_led_clear();
        ui_publish_evt(SYS_UI_EVT_ERROR, BS_ERROR);
    }
}

/**
 * @brief  Write current LED state to hardware
 *
 * For UI_LED_STATE_ERROR, sets the first flash frame (red on).
 * Subsequent toggles are driven by the task timeout loop.
 *
 * @return BS_OK    Hardware updated successfully
 * @return BS_ERROR BSP LED call failed
 */
static base_status_t ui_apply_led(void)
{
    switch (s_led_state)
    {
    case UI_LED_STATE_NORMAL:
        return bsp_led_clear();

    case UI_LED_STATE_CONNECTED:
        return bsp_led_set_rgb(0, 255, 0);  /* Solid green */

    case UI_LED_STATE_ERROR:
        s_flash_on = BS_TRUE;
        return bsp_led_set_rgb(255, 0, 0);  /* Flash starts with red on */

    default:
        return BS_ERROR;
    }
}

/**
 * @brief  Publish a UI event to the System Manager ingress queue
 *
 * @param[in] evt     Event type
 * @param[in] status  Result status (packed into data.ui.arg0)
 */
static void ui_publish_evt(system_ui_evt_t evt, base_status_t status)
{
    sm_event_msg_t event = {
        .source       = SM_SRC_UI,
        .event_id     = (uint16_t) evt,
        .timestamp_ms = (uint32_t) (xTaskGetTickCount() * portTICK_PERIOD_MS),
        .data.ui      = {
            .arg0 = (uint8_t) status,
            .arg1 = (uint8_t) s_ui_mode,
        },
    };

    if (system_manager_publish(&event) != BS_OK)
    {
        ESP_LOGW(TAG, "Failed to publish evt=%d", evt);
    }
}

/* ====================================================================
 * Test / debug
 * ==================================================================== */

void system_ui_test(void)
{
    ESP_LOGI(TAG, "=== LED random color test START (500 ms) ===");

    if (bsp_led_init() != BS_OK)
    {
        ESP_LOGE(TAG, "bsp_led_init failed — aborting test");
        return;
    }

    for (;;)
    {
        uint8_t r = (uint8_t) (esp_random() & 0xFF);
        uint8_t g = (uint8_t) (esp_random() & 0xFF);
        uint8_t b = (uint8_t) (esp_random() & 0xFF);

        bsp_led_set_rgb(r, g, b);
        ESP_LOGI(TAG, "LED: R=%d G=%d B=%d", r, g, b);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* End of file -------------------------------------------------------- */
