/**
 * @file       base_type.h
 * @copyright  Copyright (C) 2025 Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.2
 * @date       2025-08-16
 * @author     Hieu Tran
 *
 * @brief      Base type definitions
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BASE_TYPE_H
#define __BASE_TYPE_H

/* Includes ----------------------------------------------------------- */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Base status
 */
typedef enum
{
    BS_OK      = 0x00,
    BS_ERROR   = 0x01,
    BS_BUSY    = 0x02,
    BS_TIMEOUT = 0x03
} base_status_t;

/**
 * @brief Boolean type
 */
typedef enum
{
    BS_FALSE = 0x00,
    BS_TRUE  = 0x01
} bool_t;

/**
 * @brief Byte structure
 */
typedef struct
{
    uint8_t High: 4;
    uint8_t Low : 4;
} byte_t;

/* Public macros ------------------------------------------------------ */
#if !defined(NOT)
#define NOT(x) ((x) ? BS_FALSE : BS_TRUE)
#else
#error "NOT operator has been defined"
#endif

#if !defined(MAX)
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#if !defined(STRINGIFY)
#define STRINGIFY(x) #x
#endif

#define TOSTRING(x)          STRINGIFY(x)

// Structure padding
#define _addstr1(a, b, c)    a##b##c
#define _addstr2(a, b, c)    _addstr1(a, b, c)

#define PRE_ASSERT(exp, msg) typedef char _addstr2(assert_, __LINE__, _##msg)[(exp) ? 1 : -1]

#define assert_param(expr)   ((expr) ? (void) 0 : assert_failed((char *) __func__, __LINE__))

#define CHECK_STATUS(expr)          \
    do                              \
    {                               \
        base_status_t ret = (expr); \
        if (BS_OK != ret)           \
        {                           \
            return (ret);           \
        }                           \
    } while (0)

#define CHECK_ESP_STATUS(expr)  \
    do                          \
    {                           \
        esp_err_t ret = (expr); \
        if (ESP_OK != ret)      \
        {                       \
            return (BS_ERROR);  \
        }                       \
    } while (0)

#define CHECK(expr, ret_val) \
    do                       \
    {                        \
        if (!(expr))         \
        {                    \
            return ret_val;  \
        }                    \
    } while (0)

/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
void assert_failed(char *name, uint32_t line);

#endif  // __BASE_TYPE_H
/* End of file -------------------------------------------------------- */
