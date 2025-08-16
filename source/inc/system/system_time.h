/**
 * @file       system_time.c
 * @copyright  Copyright (C) 2019 QuyLe Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    0.0.1
 * @date       2023
 * @author     QuyLe
 * @author     QuyLe
 *
 * @brief      System manager time
 *
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_TIME_H
#define __SYSTEM_TIME_H

/* Includes ----------------------------------------------------------- */
#include "bsp_flash.h"
#include "bsp_timer_pwm.h"
#include "drv_cjmcu75.h"
#include "drv_ds1307.h"
#include "main.h"
#include "stdbool.h"

/* Public defines ----------------------------------------------------- */
#define SYSTEM_TIME_MAX_REMINDER 6
/* Public enumerate/structure ----------------------------------------- */

typedef enum
{
    SYSTEM_TIME_ERROR,
    SYSTEM_TIME_OK,
} system_time_error_t;

typedef struct
{
    uint8_t  number_reminder;
    uint32_t rtc_reminder[SYSTEM_TIME_MAX_REMINDER];
} system_time_reminder_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

/**
 * @brief Init system time
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_init(void);

/**
 * @brief Set reminder time
 *
 * @param[in] min             minute set reminder
 * @param[in] sec             second set reminder
 * @param[in] hour            hour set reminder
 * @param[in] day_of_week     day_of_week set reminder
 * @param[in] day_of_month    day_of_month set reminder
 * @param[in] mon             month set reminder
 * @param[in] year            year set reminder
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_set_reminder(uint8_t sec,
                                             uint8_t min,
                                             uint8_t hour,
                                             uint8_t day_of_week,
                                             uint8_t day_of_month,
                                             uint8_t mon,
                                             uint8_t year);

/**
 * @brief Set reminder str time
 *
 * @param[in] time string data time set reminder
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_set_reminder_str(char *time);

/**
 * @brief Get reminder time
 *
 * @param[out] data_reminder struct data reminder
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_get_reminder(system_time_reminder_t *data_reminder);

/**
 * @brief Get value time
 *
 * @param[out] min             minute set reminder
 * @param[out] sec             second set reminder
 * @param[out] hour            hour set reminder
 * @param[out] day_of_week     day_of_week set reminder
 * @param[out] day_of_month    day_of_month set reminder
 * @param[out] mon             month set reminder
 * @param[out] year            year set reminder
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_get_value(uint8_t *sec,
                                          uint8_t *min,
                                          uint8_t *hour,
                                          uint8_t *day_of_week,
                                          uint8_t *day_of_month,
                                          uint8_t *mon,
                                          uint8_t *year);

/**
 * @brief Get epoch time
 *
 * @param[out] epoch_time data time tyle epoch time
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_get_epoch_time(uint32_t *epoch_time);

/**
 * @brief Get date time
 *
 * @param[out] date_time data time tyle date/time
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_get_date_time(rtc_t *date_time);

/**
 * @brief Set value time clock
 *
 * @param[in] sec             second set time clock
 * @param[in] min             minute set time clock
 * @param[in] hour            hour set time clock
 * @param[in] day_of_week     day_of_week set time clock
 * @param[in] day_of_month    day_of_month set time clock
 * @param[in] mon             month set time clock
 * @param[in] year            year set time clock
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_set_value(uint8_t sec,
                                          uint8_t min,
                                          uint8_t hour,
                                          uint8_t day_of_week,
                                          uint8_t day_of_month,
                                          uint8_t mon,
                                          uint8_t year);

/**
 * @brief Clear reminder
 *
 * @param[in] id id reminder clear
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_clear_reminder(uint8_t id);

/**
 * @brief Clear all reminder
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_clear_all_reminder(void);

/**
 * @brief Save reminder
 *
 * @param[in] data_save_reminder data reminder save
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_save_reminder(system_time_reminder_t data_save_reminder);

/**
 * @brief Set function callback alarm
 *
 * @param[in] cb function callback
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_alarm_set_callback(void *cb);

/**
 * @brief Get number reminder
 *
 * @param[out] reminder_number number reninder current
 *
 * @return
 *  - SYSTEM_TIME_ERROR : system time error
 *  - SYSTEM_TIME_OK : system time ok
 */
system_time_error_t system_time_number_reminder(uint8_t *reminder_number);

/**
 * @brief Change time rtc
 *
 * @param[in] choose select second, minute, hour, day, date, month, year
 * @param[in] rtc    struct data rtc need to change
 * @param[in] direct increase or decrease
 *
 * @return
 *  None
 */
void system_time_change(uint8_t choose, rtc_t *rtc, int8_t direct);

/**
 * @brief Check realtime ready
 *
 * @return
 *  - true : ok
 *  - false : error
 */
bool system_time_is_realtime_ready(void);

/**
 * @brief Check temp sensor ready
 *
 * @return
 *  - true : ok
 *  - false : error
 */
bool system_time_is_temp_sensor_ready(void);

#endif /* __SYSTEM_TIME_H */
