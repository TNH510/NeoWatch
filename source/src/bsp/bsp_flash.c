/**
 * @file       bsp_flash.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      Board Support Package for NVS Flash
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_flash.h"

#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

/* Private defines ---------------------------------------------------- */
static const char *TAG = "BSP_FLASH";

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
base_status_t bsp_flash_init(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGW(TAG, "NVS partition truncated, erasing...");
        CHECK_ESP_STATUS(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    CHECK_ESP_STATUS(ret);

    ESP_LOGI(TAG, "NVS flash initialized successfully");
    return BS_OK;
}

base_status_t bsp_flash_set_u8(const char *namespace, const char *key, uint8_t value)
{
    CHECK(namespace != NULL && key != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_set_u8(handle, key, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set u8 key=%s: %s", key, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

base_status_t bsp_flash_get_u8(const char *namespace, const char *key, uint8_t *value)
{
    CHECK(namespace != NULL && key != NULL && value != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READONLY, &handle));

    esp_err_t ret = nvs_get_u8(handle, key, value);
    nvs_close(handle);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Key not found u8 key=%s: %s", key, esp_err_to_name(ret));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t bsp_flash_set_i8(const char *namespace, const char *key, int8_t value)
{
    CHECK(namespace != NULL && key != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_set_i8(handle, key, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set i8 key=%s: %s", key, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

base_status_t bsp_flash_get_i8(const char *namespace, const char *key, int8_t *value)
{
    CHECK(namespace != NULL && key != NULL && value != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READONLY, &handle));

    esp_err_t ret = nvs_get_i8(handle, key, value);
    nvs_close(handle);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Key not found i8 key=%s: %s", key, esp_err_to_name(ret));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t bsp_flash_set_u16(const char *namespace, const char *key, uint16_t value)
{
    CHECK(namespace != NULL && key != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_set_u16(handle, key, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set u16 key=%s: %s", key, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

base_status_t bsp_flash_get_u16(const char *namespace, const char *key, uint16_t *value)
{
    CHECK(namespace != NULL && key != NULL && value != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READONLY, &handle));

    esp_err_t ret = nvs_get_u16(handle, key, value);
    nvs_close(handle);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Key not found u16 key=%s: %s", key, esp_err_to_name(ret));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t bsp_flash_set_i16(const char *namespace, const char *key, int16_t value)
{
    CHECK(namespace != NULL && key != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_set_i16(handle, key, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set i16 key=%s: %s", key, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

base_status_t bsp_flash_get_i16(const char *namespace, const char *key, int16_t *value)
{
    CHECK(namespace != NULL && key != NULL && value != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READONLY, &handle));

    esp_err_t ret = nvs_get_i16(handle, key, value);
    nvs_close(handle);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Key not found i16 key=%s: %s", key, esp_err_to_name(ret));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t bsp_flash_set_u32(const char *namespace, const char *key, uint32_t value)
{
    CHECK(namespace != NULL && key != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_set_u32(handle, key, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set u32 key=%s: %s", key, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

base_status_t bsp_flash_get_u32(const char *namespace, const char *key, uint32_t *value)
{
    CHECK(namespace != NULL && key != NULL && value != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READONLY, &handle));

    esp_err_t ret = nvs_get_u32(handle, key, value);
    nvs_close(handle);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Key not found u32 key=%s: %s", key, esp_err_to_name(ret));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t bsp_flash_set_i32(const char *namespace, const char *key, int32_t value)
{
    CHECK(namespace != NULL && key != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_set_i32(handle, key, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set i32 key=%s: %s", key, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

base_status_t bsp_flash_get_i32(const char *namespace, const char *key, int32_t *value)
{
    CHECK(namespace != NULL && key != NULL && value != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READONLY, &handle));

    esp_err_t ret = nvs_get_i32(handle, key, value);
    nvs_close(handle);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Key not found i32 key=%s: %s", key, esp_err_to_name(ret));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t bsp_flash_set_str(const char *namespace, const char *key, const char *value)
{
    CHECK(namespace != NULL && key != NULL && value != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_set_str(handle, key, value);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set str key=%s: %s", key, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

base_status_t bsp_flash_get_str(const char *namespace, const char *key, char *value, size_t max_len)
{
    CHECK(namespace != NULL && key != NULL && value != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READONLY, &handle));

    size_t    len = max_len;
    esp_err_t ret = nvs_get_str(handle, key, value, &len);
    nvs_close(handle);

    if (ret != ESP_OK)
    {
        ESP_LOGW(TAG, "Key not found str key=%s: %s", key, esp_err_to_name(ret));
        return BS_ERROR;
    }

    return BS_OK;
}

base_status_t bsp_flash_erase_key(const char *namespace, const char *key)
{
    CHECK(namespace != NULL && key != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_erase_key(handle, key);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to erase key=%s: %s", key, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

base_status_t bsp_flash_erase_all(const char *namespace)
{
    CHECK(namespace != NULL, BS_ERROR);

    nvs_handle_t handle;
    CHECK_ESP_STATUS(nvs_open(namespace, NVS_READWRITE, &handle));

    esp_err_t ret = nvs_erase_all(handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to erase namespace=%s: %s", namespace, esp_err_to_name(ret));
        nvs_close(handle);
        return BS_ERROR;
    }

    ret = nvs_commit(handle);
    nvs_close(handle);

    CHECK_ESP_STATUS(ret);
    return BS_OK;
}

/* End of file -------------------------------------------------------- */
