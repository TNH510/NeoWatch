/**
 * @file       bsp_rtc.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-09
 * @author     Hieu Tran Ngoc
 * @brief      BSP RTC Header File
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_RTC_H
#define __BSP_RTC_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
typedef struct
{
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  min;
  uint8_t  sec;
} bsp_rtc_time_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief Initialize the RTC
 * 
 */
void bsp_rtc_init(void);

/**
 * @brief Get the current time
 *
 * @return time_t  Current epoch time
 */
time_t bsp_rtc_get_time(void);

/**
 * @brief Set the current time
 *
 * @param[in] epoch_time  Epoch time to set
 */
void bsp_rtc_set_time(time_t epoch_time);

/**
 * @brief Convert epoch time to BSP RTC time structure
 *
 * @param[in] epoch_time  Epoch time to convert
 * @return bsp_rtc_time_t  Corresponding BSP RTC time structure
 */
bsp_rtc_time_t bsp_rtc_epoch_to_time(time_t epoch_time);

/**
 * @brief Convert BSP RTC time structure to epoch time
 *
 * @param[in] time  BSP RTC time structure
 * @return time_t  Corresponding epoch time
 */
time_t bsp_rtc_time_to_epoch(bsp_rtc_time_t time);

/**
 * @brief Create string from <time>
 *
 * @param[out] out  char *out = "YYMMDD:HHMMSS\0" <splitter> = ':', buffer size >= 14
 * @param[in] time  Time use for make string
 */
void bsp_rtc_string_timestyle(char *out, time_t time, char splitter);

/* -------------------------------------------------------------------- */
#endif /* __BSP_RTC_H */

/* End of file -------------------------------------------------------- */
