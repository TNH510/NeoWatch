/**
 * @file       drv_buzzer.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    0.0.1
 * @date       2023
 * @author     Quynh Le
 * @author     Quynh Le
 *
 * @brief      Driver for Buzzer
 *
 * @note       None
 * @example    None
 */

/* Includes ----------------------------------------------------------- */
#include "drv_buzzer.h"

#include "table_music.h"
/* Private defines ---------------------------------------------------- */
#define BUZZER_AUTORELOAD (1000)
#define BUZZER_PRESCALE   (100)
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/*------------------- For implement buzzer basic sound ---------------- */
static bool     buzzer_blink_start    = false;
static bool     buzzer_music_start    = false;
static uint8_t  buzzer_current_mussic = NO_MUSSIC;
static uint16_t buzzer_count          = 0;
static uint32_t buzzer_volume         = 3;
static uint32_t buzzer_count_period   = 0;
/*------------------- For implement buzzer music sound ---------------- */
static buzzer_music_t buzzer_choose_music   = 0;
static uint16_t       buzzer_count_note     = 0;
static uint32_t       buzzer_count_duration = 0;
static int            divider = 0, noteDuration = 0;
/* Private function prototypes ---------------------------------------- */
/**
 * @brief  Make tone from frequency
 *
 * @param[in]    frequency   Frequency represent for each note
 *
 * @attention
 *
 * @return
 *  - None
 */
static void drv_buzzer_make_tone(int frequency);

/**
 * @brief  Stop making tone from frequency
 *
 * @param[in]    frequency   Frequency represent for each note
 *
 * @attention
 *
 * @return
 *  - None
 */
static void drv_buzzer_notone();

/* Function definitions ----------------------------------------------- */
void drv_buzzer_init()
{
    bsp_tim_set_autoreload(&htim4, BUZZER_AUTORELOAD - 1);
    bsp_tim_set_prescale(&htim4, BUZZER_PRESCALE - 1);
    bsp_tim4_start_tim_it();
    bsp_tim4_pwm_start();
}

void drv_buzzer_on()
{
    bsp_tim4_pwm_set_compare(BUZZER_CHANNEL, (buzzer_volume * BUZZER_AUTORELOAD) / 10);
}

void drv_buzzer_volume(uint16_t volume)
{
    volume = (volume >= 10) ? 10 : volume;
    // convert volume range(0,10) into couter period (range(0,BUZZER_AUTORELOAD)) in counter register
    buzzer_volume = volume;
}

void drv_buzzer_blink(uint16_t frequency)
{
    drv_buzzer_off();

    // set frequency = 1000 HZ
    bsp_tim_set_autoreload(&htim4, BUZZER_AUTORELOAD - 1);

    // check input
    frequency = (frequency >= 10) ? 10 : frequency;

    buzzer_blink_start = true;
    // convert frequency range(0,10) into couter period (range(0,BUZZER_AUTORELOAD)) in counter register
    buzzer_count_period = BUZZER_AUTORELOAD / frequency;
}

void drv_buzzer_music(uint8_t choose_music)
{
    //   	drv_buzzer_off();
    //	buzzer_count_duration = HAL_GetTick();
    //    buzzer_choose_music   = (choose_music >= Super_Mario) ? Super_Mario : choose_music;

    if (buzzer_music_start == false)
    {
        buzzer_count_duration = HAL_GetTick();
        buzzer_count_note     = 0;
        buzzer_choose_music   = choose_music;
        buzzer_music_start    = true;
    }
}

void drv_buzzer_off()
{
    buzzer_blink_start  = false;
    buzzer_music_start  = false;
    buzzer_choose_music = NO_MUSSIC;
    buzzer_count_note   = 0;
    bsp_tim4_pwm_set_compare(BUZZER_CHANNEL, 0);
}

static void drv_buzzer_make_tone(int frequency)
{
    // convert frequency of each note to counter period
    if (frequency == 0)
    {
        // rest note
        bsp_tim4_pwm_set_compare(BUZZER_CHANNEL, 0);
    }
    else
    {
        uint32_t autoreload;
        autoreload = 1000000 / frequency;
        bsp_tim_set_autoreload(&htim4, autoreload);
        // keep making sound at volume which setted at drv_buzzer_volume function
        bsp_tim4_pwm_set_compare(BUZZER_CHANNEL, (buzzer_volume * autoreload) / 10);
    }
}

static void drv_buzzer_notone()
{
    bsp_tim4_pwm_set_compare(BUZZER_CHANNEL, 0);
}

void bsp_tim4_handle()
{
    if (buzzer_blink_start == true)
    {
        // set frequency for buzzer based on drv_buzzer_blink (uint16_t frequency) function
        if (buzzer_count == 1)
        {
            // buzzer on high
            bsp_tim4_pwm_set_compare(BUZZER_CHANNEL, (buzzer_volume * BUZZER_AUTORELOAD) / 10);
        }
        // time buzzer on high before moving to next cycle
        else if (buzzer_count == 50)
        {
            // buzzer on low
            bsp_tim4_pwm_set_compare(BUZZER_CHANNEL, 0);
        }
        else if (buzzer_count == buzzer_count_period)
        {
            buzzer_count = 0;
        }

        buzzer_count++;
    }

    else if (buzzer_choose_music == HAPPY_BIRTHDAY)
    {
        // calculates the duration of each note
        divider = melody1[buzzer_count_note + 1];
        if (divider > 0)
        {
            // regular note, just proceed
            noteDuration = (wholenote1) / divider;
        }
        else if (divider < 0)
        {
            // dotted notes are represented with negative durations!!
            noteDuration = (wholenote1) / abs(divider);
            noteDuration *= 1.5;  // increases the duration in half for dotted notes
        }
        // play the note
        drv_buzzer_make_tone(melody1[buzzer_count_note]);

        // Wait for the specief duration before playing the next note.
        if (HAL_GetTick() - buzzer_count_duration >= noteDuration)
        {
            // stop the waveform generation before the next note.
            drv_buzzer_notone();
            buzzer_count_note += 2;
            buzzer_count_duration = HAL_GetTick();
        }

        // turn off music after whole note is finished
        if (buzzer_count_note >= notes1 * 2)
        {
            buzzer_count_note = 0;
            drv_buzzer_off();
        }
    }

    else if (buzzer_choose_music == NOKIA_TUNE)
    {
        // calculates the duration of each note
        divider = melody2[buzzer_count_note + 1];
        if (divider > 0)
        {
            // regular note, just proceed
            noteDuration = (wholenote2) / divider;
        }
        else if (divider < 0)
        {
            // dotted notes are represented with negative durations!!
            noteDuration = (wholenote2) / abs(divider);
            noteDuration *= 1.5;  // increases the duration in half for dotted notes
        }
        // play the note
        drv_buzzer_make_tone(melody2[buzzer_count_note]);

        // Wait for the specief duration before playing the next note.
        if (HAL_GetTick() - buzzer_count_duration >= noteDuration)
        {
            // stop the waveform generation before the next note.
            drv_buzzer_notone();
            buzzer_count_note += 2;
            buzzer_count_duration = HAL_GetTick();
        }

        // turn off music after whole note is finished
        if (buzzer_count_note >= notes2 * 2)
        {
            buzzer_count_note = 0;
            drv_buzzer_off();
        }
    }

    else if (buzzer_choose_music == Merry_CHRISTMAS)
    {
        // calculates the duration of each note
        divider = melody3[buzzer_count_note + 1];
        if (divider > 0)
        {
            // regular note, just proceed
            noteDuration = (wholenote3) / divider;
        }
        else if (divider < 0)
        {
            // dotted notes are represented with negative durations!!
            noteDuration = (wholenote3) / abs(divider);
            noteDuration *= 1.5;  // increases the duration in half for dotted notes
        }
        // play the note
        drv_buzzer_make_tone(melody3[buzzer_count_note]);

        // Wait for the specief duration before playing the next note.
        if (HAL_GetTick() - buzzer_count_duration >= noteDuration)
        {
            // stop the waveform generation before the next note.
            drv_buzzer_notone();
            buzzer_count_note += 2;
            buzzer_count_duration = HAL_GetTick();
        }

        // turn off music after whole note is finished
        if (buzzer_count_note >= notes3 * 2)
        {
            buzzer_count_note = 0;
            drv_buzzer_off();
        }
    }

    else if (buzzer_choose_music == Super_Mario)
    {
        // calculates the duration of each note
        divider = melody4[buzzer_count_note + 1];
        if (divider > 0)
        {
            // regular note, just proceed
            noteDuration = (wholenote4) / divider;
        }
        else if (divider < 0)
        {
            // dotted notes are represented with negative durations!!
            noteDuration = (wholenote4) / abs(divider);
            noteDuration *= 1.5;  // increases the duration in half for dotted notes
        }
        // play the note
        drv_buzzer_make_tone(melody4[buzzer_count_note]);

        // Wait for the specief duration before playing the next note.
        if (HAL_GetTick() - buzzer_count_duration >= noteDuration)
        {
            // stop the waveform generation before the next note.
            drv_buzzer_notone();
            buzzer_count_note += 2;
            buzzer_count_duration = HAL_GetTick();
        }

        // turn off music after whole note is finished
        if (buzzer_count_note >= notes4 * 2)
        {
            buzzer_count_note = 0;
            drv_buzzer_off();
        }
    }
}

/* End of file -------------------------------------------------------- */
