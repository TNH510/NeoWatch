/**
 * @file       bsp_flash.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      Board Support Package for NVS Flash
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize NVS flash storage
 *
 * @return BS_OK     Initialization succeeded
 * @return BS_ERROR  Initialization failed
 */
base_status_t bsp_flash_init(void);

/**
 * @brief  Set a uint8_t value in NVS
 *
 * @param[in] namespace  NVS namespace
 * @param[in] key        Key name
 * @param[in] value      Value to store
 *
 * @return BS_OK     Value stored successfully
 * @return BS_ERROR  Failed to store value
 */
base_status_t bsp_flash_set_u8(const char *namespace, const char *key, uint8_t value);

/**
 * @brief  Get a uint8_t value from NVS
 *
 * @param[in]  namespace  NVS namespace
 * @param[in]  key        Key name
 * @param[out] value      Pointer to store retrieved value
 *
 * @return BS_OK     Value retrieved successfully
 * @return BS_ERROR  Failed to retrieve value
 */
base_status_t bsp_flash_get_u8(const char *namespace, const char *key, uint8_t *value);

/**
 * @brief  Set an int8_t value in NVS
 *
 * @param[in] namespace  NVS namespace
 * @param[in] key        Key name
 * @param[in] value      Value to store
 *
 * @return BS_OK     Value stored successfully
 * @return BS_ERROR  Failed to store value
 */
base_status_t bsp_flash_set_i8(const char *namespace, const char *key, int8_t value);

/**
 * @brief  Get an int8_t value from NVS
 *
 * @param[in]  namespace  NVS namespace
 * @param[in]  key        Key name
 * @param[out] value      Pointer to store retrieved value
 *
 * @return BS_OK     Value retrieved successfully
 * @return BS_ERROR  Failed to retrieve value
 */
base_status_t bsp_flash_get_i8(const char *namespace, const char *key, int8_t *value);

/**
 * @brief  Set a uint16_t value in NVS
 *
 * @param[in] namespace  NVS namespace
 * @param[in] key        Key name
 * @param[in] value      Value to store
 *
 * @return BS_OK     Value stored successfully
 * @return BS_ERROR  Failed to store value
 */
base_status_t bsp_flash_set_u16(const char *namespace, const char *key, uint16_t value);

/**
 * @brief  Get a uint16_t value from NVS
 *
 * @param[in]  namespace  NVS namespace
 * @param[in]  key        Key name
 * @param[out] value      Pointer to store retrieved value
 *
 * @return BS_OK     Value retrieved successfully
 * @return BS_ERROR  Failed to retrieve value
 */
base_status_t bsp_flash_get_u16(const char *namespace, const char *key, uint16_t *value);

/**
 * @brief  Set an int16_t value in NVS
 *
 * @param[in] namespace  NVS namespace
 * @param[in] key        Key name
 * @param[in] value      Value to store
 *
 * @return BS_OK     Value stored successfully
 * @return BS_ERROR  Failed to store value
 */
base_status_t bsp_flash_set_i16(const char *namespace, const char *key, int16_t value);

/**
 * @brief  Get an int16_t value from NVS
 *
 * @param[in]  namespace  NVS namespace
 * @param[in]  key        Key name
 * @param[out] value      Pointer to store retrieved value
 *
 * @return BS_OK     Value retrieved successfully
 * @return BS_ERROR  Failed to retrieve value
 */
base_status_t bsp_flash_get_i16(const char *namespace, const char *key, int16_t *value);

/**
 * @brief  Set a uint32_t value in NVS
 *
 * @param[in] namespace  NVS namespace
 * @param[in] key        Key name
 * @param[in] value      Value to store
 *
 * @return BS_OK     Value stored successfully
 * @return BS_ERROR  Failed to store value
 */
base_status_t bsp_flash_set_u32(const char *namespace, const char *key, uint32_t value);

/**
 * @brief  Get a uint32_t value from NVS
 *
 * @param[in]  namespace  NVS namespace
 * @param[in]  key        Key name
 * @param[out] value      Pointer to store retrieved value
 *
 * @return BS_OK     Value retrieved successfully
 * @return BS_ERROR  Failed to retrieve value
 */
base_status_t bsp_flash_get_u32(const char *namespace, const char *key, uint32_t *value);

/**
 * @brief  Set an int32_t value in NVS
 *
 * @param[in] namespace  NVS namespace
 * @param[in] key        Key name
 * @param[in] value      Value to store
 *
 * @return BS_OK     Value stored successfully
 * @return BS_ERROR  Failed to store value
 */
base_status_t bsp_flash_set_i32(const char *namespace, const char *key, int32_t value);

/**
 * @brief  Get an int32_t value from NVS
 *
 * @param[in]  namespace  NVS namespace
 * @param[in]  key        Key name
 * @param[out] value      Pointer to store retrieved value
 *
 * @return BS_OK     Value retrieved successfully
 * @return BS_ERROR  Failed to retrieve value
 */
base_status_t bsp_flash_get_i32(const char *namespace, const char *key, int32_t *value);

/**
 * @brief  Set a string value in NVS
 *
 * @param[in] namespace  NVS namespace
 * @param[in] key        Key name
 * @param[in] value      Null-terminated string to store
 *
 * @return BS_OK     Value stored successfully
 * @return BS_ERROR  Failed to store value
 */
base_status_t bsp_flash_set_str(const char *namespace, const char *key, const char *value);

/**
 * @brief  Get a string value from NVS
 *
 * @param[in]  namespace  NVS namespace
 * @param[in]  key        Key name
 * @param[out] value      Buffer to store retrieved string
 * @param[in]  max_len    Maximum length of the buffer
 *
 * @return BS_OK     Value retrieved successfully
 * @return BS_ERROR  Failed to retrieve value
 */
base_status_t bsp_flash_get_str(const char *namespace, const char *key, char *value, size_t max_len);

/**
 * @brief  Erase a specific key from NVS
 *
 * @param[in] namespace  NVS namespace
 * @param[in] key        Key name to erase
 *
 * @return BS_OK     Key erased successfully
 * @return BS_ERROR  Failed to erase key
 */
base_status_t bsp_flash_erase_key(const char *namespace, const char *key);

/**
 * @brief  Erase all keys in an NVS namespace
 *
 * @param[in] namespace  NVS namespace to erase
 *
 * @return BS_OK     Namespace erased successfully
 * @return BS_ERROR  Failed to erase namespace
 */
base_status_t bsp_flash_erase_all(const char *namespace);

/* -------------------------------------------------------------------- */
#endif /* __BSP_FLASH_H */

/* End of file -------------------------------------------------------- */
