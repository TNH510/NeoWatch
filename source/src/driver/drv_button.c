/**
 * @file       drv_button.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     NEO
 * @brief      Button Driver
 * @note       Adapted from https://github.com/mauriciobarroso/button
 *             using espressif/button component
 */

/* Includes ----------------------------------------------------------- */
#include "drv_button.h"

#include "iot_button.h"
#include "button_gpio.h"

/* Private defines ---------------------------------------------------- */
#define DRV_BUTTON_TAG "drv_button"

/* Private enumerate/structure ---------------------------------------- */

/**
 * @brief Internal structure to pass context to espressif button callbacks
 */
typedef struct
{
    drv_button_callback_t callback;
    void                 *usr_data;
    drv_button_event_t    event;
} drv_button_cb_ctx_t;

/**
 * @brief Internal button instance that wraps espressif handle and tracks contexts
 */
typedef struct
{
    button_handle_t      esp_handle;
    drv_button_cb_ctx_t *cb_ctx[DRV_BUTTON_EVENT_MAX];
} drv_button_instance_t;

/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */

/**
 * @brief  Map drv_button_event_t to espressif button_event_t
 */
static button_event_t drv_button_map_event(drv_button_event_t event);

/**
 * @brief  Internal espressif button callback dispatcher
 */
static void drv_button_esp_callback(void *button_handle, void *usr_data);

/* Function definitions ----------------------------------------------- */

base_status_t drv_button_init(const drv_button_config_t *config, drv_button_handle_t *handle)
{
    if ((config == NULL) || (handle == NULL))
    {
        return BS_ERROR;
    }

    drv_button_instance_t *instance = (drv_button_instance_t *)calloc(1, sizeof(drv_button_instance_t));
    if (instance == NULL)
    {
        return BS_ERROR;
    }

    button_config_t btn_cfg = {
        .long_press_time  = 0,
        .short_press_time = 0,
    };

    button_gpio_config_t gpio_cfg = {
        .gpio_num         = config->gpio_num,
        .active_level     = config->active_level,
        .enable_power_save = false,
        .disable_pull     = false,
    };

    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &instance->esp_handle);

    if (ret != ESP_OK)
    {
        ESP_LOGE(DRV_BUTTON_TAG, "Failed to create button: %s", esp_err_to_name(ret));
        free(instance);
        return BS_ERROR;
    }

    *handle = (drv_button_handle_t)instance;
    return BS_OK;
}

base_status_t drv_button_register_callback(drv_button_handle_t  handle,
                                           drv_button_event_t   event,
                                           drv_button_callback_t callback,
                                           void                *usr_data)
{
    if ((handle == NULL) || (callback == NULL) || (event >= DRV_BUTTON_EVENT_MAX))
    {
        return BS_ERROR;
    }

    drv_button_instance_t *instance = (drv_button_instance_t *)handle;

    /* Free any previously registered context for this event */
    if (instance->cb_ctx[event] != NULL)
    {
        free(instance->cb_ctx[event]);
        instance->cb_ctx[event] = NULL;
    }

    /* Allocate context for the callback */
    drv_button_cb_ctx_t *ctx = (drv_button_cb_ctx_t *)malloc(sizeof(drv_button_cb_ctx_t));
    if (ctx == NULL)
    {
        return BS_ERROR;
    }

    ctx->callback = callback;
    ctx->usr_data = usr_data;
    ctx->event    = event;

    button_event_t esp_event = drv_button_map_event(event);

    esp_err_t ret = iot_button_register_cb(instance->esp_handle, esp_event, NULL,
                                           drv_button_esp_callback, (void *)ctx);
    if (ret != ESP_OK)
    {
        free(ctx);
        return BS_ERROR;
    }

    instance->cb_ctx[event] = ctx;
    return BS_OK;
}

base_status_t drv_button_unregister_callback(drv_button_handle_t handle,
                                             drv_button_event_t  event)
{
    if ((handle == NULL) || (event >= DRV_BUTTON_EVENT_MAX))
    {
        return BS_ERROR;
    }

    drv_button_instance_t *instance = (drv_button_instance_t *)handle;
    button_event_t esp_event = drv_button_map_event(event);

    esp_err_t ret = iot_button_unregister_cb(instance->esp_handle, esp_event, NULL);

    /* Free the callback context */
    if (instance->cb_ctx[event] != NULL)
    {
        free(instance->cb_ctx[event]);
        instance->cb_ctx[event] = NULL;
    }

    return (ret == ESP_OK) ? BS_OK : BS_ERROR;
}

base_status_t drv_button_deinit(drv_button_handle_t handle)
{
    if (handle == NULL)
    {
        return BS_ERROR;
    }

    drv_button_instance_t *instance = (drv_button_instance_t *)handle;

    esp_err_t ret = iot_button_delete(instance->esp_handle);

    /* Free all callback contexts */
    for (int i = 0; i < DRV_BUTTON_EVENT_MAX; i++)
    {
        if (instance->cb_ctx[i] != NULL)
        {
            free(instance->cb_ctx[i]);
            instance->cb_ctx[i] = NULL;
        }
    }

    free(instance);

    return (ret == ESP_OK) ? BS_OK : BS_ERROR;
}

drv_button_event_t drv_button_get_event(drv_button_handle_t handle)
{
    if (handle == NULL)
    {
        return DRV_BUTTON_EVENT_MAX;
    }

    drv_button_instance_t *instance = (drv_button_instance_t *)handle;
    button_event_t esp_event = iot_button_get_event(instance->esp_handle);

    switch (esp_event)
    {
    case BUTTON_SINGLE_CLICK:
        return DRV_BUTTON_EVENT_SINGLE_CLICK;
    case BUTTON_DOUBLE_CLICK:
        return DRV_BUTTON_EVENT_DOUBLE_CLICK;
    case BUTTON_PRESS_DOWN:
        return DRV_BUTTON_EVENT_PRESS_DOWN;
    case BUTTON_LONG_PRESS_HOLD:
        return DRV_BUTTON_EVENT_HOLD;
    case BUTTON_LONG_PRESS_START:
        return DRV_BUTTON_EVENT_LONG_PRESS;
    case BUTTON_PRESS_UP:
        return DRV_BUTTON_EVENT_PRESS_UP;
    default:
        return DRV_BUTTON_EVENT_MAX;
    }
}

/* Private function definitions --------------------------------------- */

static button_event_t drv_button_map_event(drv_button_event_t event)
{
    switch (event)
    {
    case DRV_BUTTON_EVENT_SINGLE_CLICK:
        return BUTTON_SINGLE_CLICK;
    case DRV_BUTTON_EVENT_DOUBLE_CLICK:
        return BUTTON_DOUBLE_CLICK;
    case DRV_BUTTON_EVENT_PRESS_DOWN:
        return BUTTON_PRESS_DOWN;
    case DRV_BUTTON_EVENT_HOLD:
        return BUTTON_LONG_PRESS_HOLD;
    case DRV_BUTTON_EVENT_LONG_PRESS:
        return BUTTON_LONG_PRESS_START;
    case DRV_BUTTON_EVENT_PRESS_UP:
        return BUTTON_PRESS_UP;
    default:
        return BUTTON_NONE_PRESS;
    }
}

static void drv_button_esp_callback(void *button_handle, void *usr_data)
{
    drv_button_cb_ctx_t *ctx = (drv_button_cb_ctx_t *)usr_data;

    if ((ctx != NULL) && (ctx->callback != NULL))
    {
        ctx->callback(ctx->event, ctx->usr_data);
    }
}

/* End of file -------------------------------------------------------- */
