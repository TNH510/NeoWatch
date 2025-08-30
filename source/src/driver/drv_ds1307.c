/**
 * @file       driver_ds1307.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    2.0.1
 * @date       2023-08-21
 * @author     Hieu Tran
 * @author     Quy Le
 *
 * @brief      DS1307 Driver
 */

/* Includes ----------------------------------------------------------- */
#include "drv_ds1307.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */
/* Struct for register from ds1307 data */
typedef struct
{
    uint8_t year_reg;
    uint8_t month_reg;
    uint8_t day_of_month_reg;
    uint8_t day_of_week_reg;
    uint8_t hour_reg;
    uint8_t minute_reg;
    uint8_t second_reg;
} rtc_reg_t;

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief Convert bcd to string
 *
 * @param byte          Contain data register
 * @param str_buffer    Contain result
 * @return char*        Pointer to result
 */
static char *bcd_to_str(uint8_t byte, char *str_buffer);

/**
 * @brief Convert str to byte
 *
 * @param buffer        Contain data want to convert
 * @return uint8_t      Result
 */
static uint8_t str_to_bcd(char *buffer);

/**
 * @brief Convert str to bcd
 *
 * @param bcd           BCD number
 * @return uint8_t      Byte number
 */
static uint8_t bcd_to_byte(uint8_t bcd);

/**
 * @brief Convert byte to bcd
 *
 * @param byte          Byte number
 * @return uint8_t      BCD number
 */
static uint8_t byte_to_bcd(uint8_t byte);

/* Function definitions ----------------------------------------------- */
static char *bcd_to_str(uint8_t byte, char *str_buffer)
{
    str_buffer[0] = ((byte >> 4) & 0x0F) + '0';
    str_buffer[1] = (byte & 0x0F) + '0';
    str_buffer[2] = '\0';

    return str_buffer;
}

static uint8_t str_to_bcd(char *buffer)
{
    uint8_t byte_data;

    byte_data = ((buffer[0] - '0') << 4) | (buffer[1] - '0');

    return byte_data;
}

static uint8_t bcd_to_byte(uint8_t bcd)
{
    uint8_t byte;

    byte = ((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F);

    return byte;
}

static uint8_t byte_to_bcd(uint8_t byte)
{
    uint8_t bcd;

    bcd = ((byte / 10) << 4 & 0x0F) | (byte % 10 & 0x0F);

    return bcd;
}

uint8_t calculate_day_of_week(uint8_t day, uint8_t month, uint8_t year)
{
    if (month < 3)
    {
        month += 12;
        year--;
    }

    uint16_t century         = year / 100;
    uint16_t year_of_century = year % 100;

    /* Zeller's Congruence */
    uint16_t day_of_week = (day + ((13 * (month + 1)) / 5) + year_of_century + (year_of_century / 4)
                            + (century / 4) - (2 * century))
                           % 7;

    if (day_of_week < 0)
    {
        day_of_week += 7;
    }

    return day_of_week + 1;
}

bool ds1307_set_day_of_month(ds1307_t *ds1307, char *buffer)
{
    uint8_t data = str_to_bcd(buffer) & DS1307_MASK_DAY_OF_MONTH;

    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_DAY_OF_MONTH, &data, 1);

    return true;
}

bool ds1307_set_day_of_week(ds1307_t *ds1307, char *buffer)
{
    uint8_t data = str_to_bcd(buffer) & DS1307_MASK_DAY_OF_WEEK;

    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_DAY_OF_WEEK, &data, 1);

    return true;
}

bool ds1307_set_month(ds1307_t *ds1307, char *buffer)
{
    uint8_t data = str_to_bcd(buffer) & DS1307_MASK_MONTH;

    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_MONTH, &data, 1);

    return true;
}

bool ds1307_set_year(ds1307_t *ds1307, char *buffer)
{
    uint8_t data = str_to_bcd(buffer) & DS1307_MASK_YEAR;

    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_YEAR, &data, 1);

    return true;
}

bool ds1307_set_sec(ds1307_t *ds1307, char *buffer)
{
    /* Check if sec is invalid */
    if (buffer[0] > '5' || (buffer[0] <= '5' && buffer[1] > '9'))
        return false;

    uint8_t data = str_to_bcd(buffer) & DS1307_MASK_SECOND;

    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_SECOND, &data, 1);

    return true;
}

bool ds1307_set_min(ds1307_t *ds1307, char *buffer)
{
    /* Check if min is invalid */
    if (buffer[0] > '5' || (buffer[0] <= '5' && buffer[1] > '9'))
        return false;

    uint8_t data = str_to_bcd(buffer) & DS1307_MASK_MINUTE;

    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_MINUTE, &data, 1);

    return true;
}

bool ds1307_set_hour_24h(ds1307_t *ds1307, char *buffer)
{
    /* Check if hour is invalid */
    if (buffer[0] > '2' || (buffer[0] == '2' && buffer[1] > '3'))
        return false;

    uint8_t data = str_to_bcd(buffer) & DS1307_MASK_HOUR;

    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_HOUR, &data, 1);

    return true;
}

bool ds1307_set_hour_12h(ds1307_t *ds1307, char *buffer)
{
    /* Check if hour is invalid */
    if (buffer[0] > '1' || (buffer[0] <= '1' && buffer[1] > '1')
        || ((buffer[2] != 'a') && (buffer[2] != 'p')))
        return false;

    uint8_t data;

    if (buffer[2] == 'a')
        data =
          ((DS1307_FLAG_0X02_12_AM) | (((buffer[0] - '0') << 4) | ((buffer[1] - '0') & DS1307_MASK_HOUR)));

    if (buffer[2] == 'p')
        data =
          ((DS1307_FLAG_0X02_12_PM) | (((buffer[0] - '0') << 4) | ((buffer[1] - '0') & DS1307_MASK_HOUR)));

    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_HOUR, &data, 1);

    return true;
}

rtc_t *ds1307_get_time(ds1307_t *ds1307)
{
    rtc_reg_t rtc_reg;

    ds1307->i2c_read_at(ds1307->device_address, DS1307_REGISTER_SECOND, (uint8_t *) &(rtc_reg.second_reg),
                        sizeof(rtc_reg.second_reg));

    ds1307->i2c_read_at(ds1307->device_address, DS1307_REGISTER_MINUTE, (uint8_t *) &(rtc_reg.minute_reg),
                        sizeof(rtc_reg.minute_reg));

    ds1307->i2c_read_at(ds1307->device_address, DS1307_REGISTER_HOUR, (uint8_t *) &(rtc_reg.hour_reg),
                        sizeof(rtc_reg.hour_reg));

    ds1307->i2c_read_at(ds1307->device_address, DS1307_REGISTER_DAY_OF_WEEK,
                        (uint8_t *) &(rtc_reg.day_of_week_reg), sizeof(rtc_reg.day_of_week_reg));

    ds1307->i2c_read_at(ds1307->device_address, DS1307_REGISTER_DAY_OF_MONTH,
                        (uint8_t *) &(rtc_reg.day_of_month_reg), sizeof(rtc_reg.day_of_month_reg));

    ds1307->i2c_read_at(ds1307->device_address, DS1307_REGISTER_MONTH, (uint8_t *) &(rtc_reg.month_reg),
                        sizeof(rtc_reg.month_reg));

    ds1307->i2c_read_at(ds1307->device_address, DS1307_REGISTER_YEAR, (uint8_t *) &(rtc_reg.year_reg),
                        sizeof(rtc_reg.year_reg));

    /* Only keep bits have correct information */
    ds1307->rtc.second       = bcd_to_byte(rtc_reg.second_reg & DS1307_MASK_SECOND);
    ds1307->rtc.minute       = bcd_to_byte(rtc_reg.minute_reg & DS1307_MASK_MINUTE);
    ds1307->rtc.hour         = bcd_to_byte(rtc_reg.hour_reg & DS1307_MASK_HOUR);
    ds1307->rtc.day_of_week  = bcd_to_byte(rtc_reg.day_of_week_reg & DS1307_MASK_DAY_OF_WEEK);
    ds1307->rtc.day_of_month = bcd_to_byte(rtc_reg.day_of_month_reg & DS1307_MASK_DAY_OF_MONTH);
    ds1307->rtc.month        = bcd_to_byte(rtc_reg.month_reg & DS1307_MASK_MONTH);
    ds1307->rtc.year         = bcd_to_byte(rtc_reg.year_reg & DS1307_MASK_YEAR);

    // Return ds1307->rtc
    return &ds1307->rtc;
}

bool ds1307_set_time(ds1307_t *ds1307, char *buffer)
{
    /* YYYY-MM-DD HH:MM:SS */
    /* Set year, month, day*/
    ds1307_set_year(ds1307, &buffer[2]);
    ds1307_set_month(ds1307, &buffer[5]);
    ds1307_set_day_of_month(ds1307, &buffer[8]);

    ds1307_get_time(ds1307);

    /* Calculate day of week */
    uint16_t year = (*(buffer) - '0') * 1000 + (*(buffer + 1) - '0') * 100 + (*(buffer + 2) - '0') * 10
                    + (*(buffer + 3) - '0');
    uint8_t day_of_week = calculate_day_of_week(ds1307->rtc.day_of_month, ds1307->rtc.month, year);
    uint8_t data        = byte_to_bcd(day_of_week) & DS1307_MASK_DAY_OF_WEEK;
    ds1307->i2c_write_at(ds1307->device_address, DS1307_REGISTER_DAY_OF_WEEK, &data, 1);

    ds1307_set_hour_24h(ds1307, &buffer[11]);
    ds1307_set_min(ds1307, &buffer[14]);
    ds1307_set_sec(ds1307, &buffer[17]);
    return true;
}

void ds1307_init(ds1307_t *ds1307)
{
    ds1307->device_address      = DS1307_ADDRESS_8BIT;
    ds1307->i2c_is_device_ready = bsp_i2c1_is_device_ready;
    ds1307->i2c_read_at         = bsp_i2c1_read_mem;
    ds1307->i2c_write_at        = bsp_i2c1_write_mem;
}

/* Default mode 24h */
void ds1307_convert_to_epoch_time(rtc_t rtc, uint32_t *epoch_time)
{
    struct tm time_date;
    memset((void *) &time_date, 0, sizeof(time_date));

    time_date.tm_min  = rtc.minute;
    time_date.tm_sec  = rtc.second;
    time_date.tm_hour = rtc.hour;
    time_date.tm_mday = rtc.day_of_month;
    time_date.tm_mon  = rtc.month - 1;
    time_date.tm_year = (2000 + rtc.year) - 1900;
    *epoch_time       = mktime(&time_date);
}

void ds1307_convert_to_date_time(rtc_t *rtc, uint32_t epoch_time)
{
    struct tm *_time;
    time_t     epoch_time_c = epoch_time;
    _time                   = localtime(&epoch_time_c);
    _time->tm_year += 1900;

    rtc->year         = _time->tm_year % 100;
    rtc->month        = _time->tm_mon + 1;
    rtc->day_of_month = _time->tm_mday;
    rtc->hour         = _time->tm_hour;
    rtc->minute       = _time->tm_min;
    rtc->second       = _time->tm_sec;
    rtc->day_of_week  = _time->tm_wday;
}

/* End of file -------------------------------------------------------- */
