/**
 * @file       incon_clock.h
 * @copyright  Copyright (C) 2019 QuyLe Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    0.0.1
 * @date       2023
 * @author     Quy Le
 * @author     Quy Le
 *
 * @brief      Middleware for Icon clock
 *
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __ICON_CLOCK_H
#define __ICON_CLOCK_H

/* Includes ----------------------------------------------------------- */

/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* 'icon alarm' */
extern const unsigned char icon_alarm[];
/* 'icon tick off', 16x16px */
extern const unsigned char icon_tick_off[];
/* 'icon tick on', 16x16px */
extern const unsigned char icon_tick_on[];
/* 'icon gach', 12x2px */
extern const unsigned char icon_select[];
/* 'alarm_solid-512', 128x64px */
extern const unsigned char screen_alarm[];
/* Array of all bitmaps for convenience. */
extern unsigned char *gif_alarm_reminder[];
/* 'Screen Save', 128x64px */
extern const unsigned char icon_screen_save[];
/* Icon for snake game*/
/* 'icon snake', 55x55px */
extern unsigned char icon_snake[];
/* icon game win */
extern unsigned char icon_snake_game_win[];
/* 'icon Game over game' */
extern unsigned char icon_snake_game_over[];
/* 'snake game text', 76x15px */
extern unsigned char icon_snake_game_text[];
/* 'choose_pointer', 7x7px */
extern unsigned char icon_snake_choose_pointer[];
/* 'icon select big', 23x2px */
extern const unsigned char icon_select_big[];
/* Icon for Main menu */
/* 26x26 pixels */
extern const unsigned char icon_menu_pc_control[];
/* 26x26 pixels */
extern const unsigned char icon_menu_setting[];
/* 26x26 pixels */
extern const unsigned char icon_menu_snake[];
/* 26x26 pixels */
extern const unsigned char icon_menu_theme[];
/* 26x26 pixels */
extern const unsigned char icon_menu_time[];
/* 28x28 pixels */
extern const unsigned char icon_menu_bound[];
/* Icon for Boot Screen */

extern const unsigned char icon_button_theme[];

#endif
