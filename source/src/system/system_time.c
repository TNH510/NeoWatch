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

/* Includes ----------------------------------------------------------- */
#include "system_time.h"

#include "system_data.h"

/* Private defines ---------------------------------------------------- */
typedef system_time_error_t (*system_time_alarm_callback_t)(uint8_t);

system_time_alarm_callback_t system_time_alarm_callback = NULL;
/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
extern UART_HandleTypeDef huart2;
static cjmcu75_t          cjmcu75;
ds1307_t                  ds1307;
static rtc_t              rtc;
static uint8_t            ds1307_initialize_success = false;
static uint8_t            flag_rtc_reminder         = 0;
static system_time_reminder_t data_remider;

/* Private function prototypes ---------------------------------------- */
static uint8_t                str_to_byte(char *buffer);
static uint8_t check_leap_year(uint16_t year);
static uint8_t day_of_month(uint8_t month, uint8_t year);

/* Function definitions ----------------------------------------------- */
static uint8_t str_to_byte(char *buffer)
{
    uint8_t byte_data;

    byte_data = ((buffer[0] - '0') * 10) + (buffer[1] - '0');

    return byte_data;
}

system_time_error_t system_time_update_reminder(void)
{
    system_data_set_reminder(data_remider);
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_init(void)
{
    /* Init ds1307 ok */
    ds1307_init(&ds1307);
    cjmcu75_init(&cjmcu75);
    if (!ds1307.i2c_is_device_ready(ds1307.device_address))
    {
        return SYSTEM_TIME_ERROR;
    }
    ds1307_initialize_success = true;

    /* Update data reminder */
    system_time_get_reminder(&data_remider);
    if (data_remider.number_reminder == 0xFF)
    {
        data_remider.number_reminder = 0;
        system_time_update_reminder();
    }

    bsp_tim11_start_tim_it();
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_alarm_set_callback(void *cb)
{
    system_time_alarm_callback = cb;
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_save_reminder(system_time_reminder_t data_save_reminder)
{
    data_remider = data_save_reminder;
    system_time_update_reminder();
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_set_reminder(uint8_t sec,
                                             uint8_t min,
                                             uint8_t hour,
                                             uint8_t day_of_week,
                                             uint8_t day_of_month,
                                             uint8_t mon,
                                             uint8_t year)
{
    uint8_t i = 0;
    for (i = 0; i < SYSTEM_TIME_MAX_REMINDER; i++)
    {
        if (!(data_remider.number_reminder & (1 << i)))
            break;
    }
    if (i == SYSTEM_TIME_MAX_REMINDER)
        return SYSTEM_TIME_ERROR;
    rtc_t time;
    time.second       = sec;
    time.minute       = min;
    time.hour         = hour;
    time.day_of_week  = day_of_week;
    time.day_of_month = day_of_month;
    time.month        = mon;
    time.year         = year;
    data_remider.number_reminder |= (1 << i);
    ds1307_convert_to_epoch_time(time, &data_remider.rtc_reminder[i]);

    system_time_update_reminder();
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_set_reminder_str(char *time)
{
    uint16_t year        = (*(time) - '0') * 1000 + (*(time + 1) - '0') * 100 + (*(time + 2) - '0') * 10 + (*(time + 3) - '0');
    uint8_t  day_of_week = calculate_day_of_week(str_to_byte(time + 8), str_to_byte(time + 5), year);

    system_time_set_reminder(str_to_byte(time + 17), str_to_byte(time + 14), str_to_byte(time + 11), day_of_week,
                             str_to_byte(time + 8), str_to_byte(time + 5), str_to_byte(time + 2));

    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_get_reminder(system_time_reminder_t *reminder_read)
{
    system_data_get_reminder(reminder_read);
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_number_reminder(uint8_t *reminder_number)
{
    uint8_t value = 0;
    for (uint8_t i = 0; i < SYSTEM_TIME_MAX_REMINDER; i++)
    {
        if (data_remider.number_reminder & (1 << i))
        {
            value++;
        }
    }
    *reminder_number = value;
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_clear_reminder(uint8_t id)
{
    if (data_remider.number_reminder & (1 << id))
    {
        data_remider.number_reminder &= ~(1 << id);
        system_time_update_reminder();
    }
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_clear_all_reminder(void)
{
    if (data_remider.number_reminder)
    {
        data_remider.number_reminder = 0;
        system_time_update_reminder();
    }
    return SYSTEM_TIME_OK;
}

system_time_error_t system_time_get_value(uint8_t *sec,
                                          uint8_t *min,
                                          uint8_t *hour,
                                          uint8_t *day_of_week,
                                          uint8_t *day_of_month,
                                          uint8_t *mon,
                                          uint8_t *year)
{
    if (ds1307_initialize_success)
    {
        *sec          = rtc.second;
        *min          = rtc.minute;
        *hour         = rtc.hour;
        *day_of_week  = rtc.day_of_week;
        *day_of_month = rtc.day_of_month;
        *mon          = rtc.month;
        *year         = rtc.year;
        return SYSTEM_TIME_OK;
    }
    return SYSTEM_TIME_ERROR;
}

system_time_error_t system_time_get_epoch_time(uint32_t *epoch_time)
{
    if (ds1307_initialize_success)
    {
        ds1307_convert_to_epoch_time(rtc, epoch_time);
        return SYSTEM_TIME_OK;
    }
    return SYSTEM_TIME_ERROR;
}

system_time_error_t system_time_get_date_time(rtc_t *date_time)
{
    if (ds1307_initialize_success)
    {
        *date_time = rtc;
        return SYSTEM_TIME_OK;
    }
    return SYSTEM_TIME_ERROR;
}

system_time_error_t system_time_set_value(uint8_t sec,
                                          uint8_t min,
                                          uint8_t hour,
                                          uint8_t day_of_week,
                                          uint8_t day_of_month,
                                          uint8_t mon,
                                          uint8_t year)
{
    if (ds1307_initialize_success)
    {
        /* YYYY-MM-DD HH:MM:SS */
        /* Set year, month, day*/
        char string_set_time[50];
        sprintf(string_set_time, "%04d-%02d-%02d %02d-%02d-%02d", 2000 + year, mon, day_of_month, hour, min, sec);
        ds1307_set_time(&ds1307, string_set_time);
        return SYSTEM_TIME_OK;
    }
    return SYSTEM_TIME_ERROR;
}

void bsp_tim11_handle()
{
    rtc                       = *ds1307_get_time(&ds1307);
    uint32_t value_epoch_time = 0;
    ds1307_convert_to_epoch_time(rtc, &value_epoch_time);

    if ((data_remider.number_reminder != 0) && ds1307_initialize_success)
    {
        uint8_t i = 0;
        for (i = 0; i < SYSTEM_TIME_MAX_REMINDER; i++)
        {
            if (data_remider.number_reminder & (1 << i))
            {
                if (data_remider.rtc_reminder[i] == value_epoch_time)
                {
                    flag_rtc_reminder |= (1 << i);
                    system_time_clear_reminder(i);
                    if (system_time_alarm_callback != NULL)
                        system_time_alarm_callback(i);
                }
            }
        }
    }
}

static uint8_t check_leap_year(uint16_t year)
{
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

static uint8_t day_of_month(uint8_t month, uint8_t year)
{
    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12: return 31;
    case 4:
    case 6:
    case 9:
    case 11: return 30;
    case 2:
        if (check_leap_year(2000 + year))
            return 29;
        else
            return 28;
    default: break;
    }
    return 0;
}

void system_time_change(uint8_t choose, rtc_t *rtc, int8_t direct)
{
    switch (choose + 1)
    {
    case 1:
    {
        if ((rtc->hour != 0 || direct != -1) && (rtc->hour != 23 || direct != 1))
            rtc->hour += direct;
        break;
    }
    case 2:
    {
        if ((rtc->minute != 0 || direct != -1) && (rtc->minute != 59 || direct != 1))
            rtc->minute += direct;
        break;
    }
    case 3:
    {
        if ((rtc->second != 0 || direct != -1) && (rtc->second != 59 || direct != 1))
            rtc->second += direct;
        break;
    }
    case 4:
    {
        if ((rtc->day_of_month != 0 || direct != -1)
            && (rtc->day_of_month != day_of_month(rtc->month, 2000 + rtc->year) || direct != 1))
            rtc->day_of_month += direct;
        break;
    }
    case 5:
    {
        if ((rtc->month != 1 || direct != -1) && (rtc->month != 12 || direct != 1))
            rtc->month += direct;
        break;
    }
    case 6:
    {
        if ((rtc->year != 0 || direct != -1) && (rtc->year != 99 || direct != 1))
            rtc->year += direct;
        break;
    }
    default: break;
    }
}

bool system_time_is_realtime_ready(void)
{
    return ds1307.i2c_is_device_ready(ds1307.device_address);
}

bool system_time_is_temp_sensor_ready(void)
{
    return cjmcu75.i2c_is_device_ready(cjmcu75.device_address);
}

/* End of file -------------------------------------------------------- */
