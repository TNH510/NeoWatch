/**
 * @file       bsp_rtc.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-09
 * @author     Hieu Tran Ngoc
 * @brief      RTC Module BSP
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "bsp_rtc.h"

#include "base_type.h"
#include "esp_log.h"
#include "rtc.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static uint32_t device_epoch_time = 0;  // Store the epoch time set by the user
static uint32_t last_rtc_counter  = 0;  // Store the last RTC time when epoch_time was set

/* Private function prototypes ---------------------------------------- */
static time_t m_get_build_time();

/* Function definitions ----------------------------------------------- */
void bsp_rtc_init(void)
{
    // Set the initial epoch time to the build time
    device_epoch_time = m_get_build_time();
    last_rtc_counter  = (uint32_t) (esp_rtc_get_time_us() / 1000000);
}

time_t bsp_rtc_get_time(void)
{
    // Calculate the current epoch time
    uint32_t current_rtc_counter = (uint32_t) (esp_rtc_get_time_us() / 1000000);
    uint32_t elapsed_time        = current_rtc_counter - last_rtc_counter;

    device_epoch_time += elapsed_time;
    last_rtc_counter = current_rtc_counter;

    return (device_epoch_time);
}

void bsp_rtc_set_time(time_t epoch_time)
{
    device_epoch_time = epoch_time;
    last_rtc_counter  = (time_t) (esp_rtc_get_time_us() / 1000000);
}

bsp_rtc_time_t bsp_rtc_epoch_to_time(time_t epoch_time)
{
    time_t         m_time = (time_t) epoch_time;
    struct tm     *h_time;
    bsp_rtc_time_t res;

    h_time = localtime(&m_time);

    res.year  = (uint16_t) (h_time->tm_year + 1900);
    res.month = (uint8_t) (h_time->tm_mon + 1);
    res.day   = (uint8_t) (h_time->tm_mday);
    res.hour  = (uint8_t) (h_time->tm_hour);
    res.min   = (uint8_t) (h_time->tm_min);
    res.sec   = (uint8_t) (h_time->tm_sec);

    return res;
}

time_t bsp_rtc_time_to_epoch(bsp_rtc_time_t time)
{
    struct tm h_time = { 0 };

    h_time.tm_year = time.year - 1900;
    h_time.tm_mon  = time.month - 1;
    h_time.tm_mday = time.day;
    h_time.tm_hour = time.hour;
    h_time.tm_min  = time.min;
    h_time.tm_sec  = time.sec;

    return (time_t) mktime(&h_time);
}

void bsp_rtc_string_timestyle(char *out, time_t time, char splitter)
{
    struct tm *h_time;

    h_time = localtime(&time);
    strftime(out, 14, "%y%m%d:%H%M%S", h_time);
    out[6] = splitter;
}

static time_t m_get_build_time()
{
    // Use static strings to avoid runtime allocation
    static const char date[] = __DATE__;  // e.g., "Mar 31 2025"
    static const char time[] = __TIME__;  // e.g., "12:34:56"

    struct tm tm = {
        .tm_isdst = -1  // Daylight saving unspecified
    };

    // Precomputed month lookup table (3-char string to int)
    static const struct
    {
        const char *name;
        int         num;
    } months[] = { { "Jan", 0 }, { "Feb", 1 }, { "Mar", 2 }, { "Apr", 3 }, { "May", 4 },  { "Jun", 5 },
                   { "Jul", 6 }, { "Aug", 7 }, { "Sep", 8 }, { "Oct", 9 }, { "Nov", 10 }, { "Dec", 11 } };

    // Parse date: "Mon DD YYYY"
    char mon[4];
    sscanf(date, "%3s %2d %4d", mon, &tm.tm_mday, &tm.tm_year);
    sscanf(time, "%2d:%2d:%2d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    // Fast month lookup (avoid loop)
    for (int i = 0; i < 12; i++)
    {
        if (mon[0] == months[i].name[0] && mon[1] == months[i].name[1] && mon[2] == months[i].name[2])
        {
            tm.tm_mon = months[i].num;
            break;
        }
    }

    tm.tm_year -= 1900;  // Adjust year from 1900 base

    // Return epoch time
    return mktime(&tm);
}

/* End of file -------------------------------------------------------- */
