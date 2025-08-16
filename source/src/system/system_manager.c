/**
 * @file       system_manager.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-08-31
 * @author     Hieu Tran
 *
 * @brief      System to manage manager all devices
 */

/* Includes ----------------------------------------------------------- */
#include "system_manager.h"

/* Private defines ---------------------------------------------------- */
#define SYSTEM_SELECTING (0xff)
/* Private enumerate/structure ---------------------------------------- */
typedef enum
{
    BOOT_STATE = 1,
    WAIT_STATE,
    MENU_STATE,
    GAME_STATE,
    TIME_STATE,
    THEME_STATE,
    DIAGNOSTIC_STATE,
    PC_CONTROL_STATE,
    INVALID_STATE
} system_state_t;

typedef enum
{
    INVALID_APP,
    TIME,
    GAME,
    THEME,
    DIAGNOSTIC,
    PC_CONTROL,

} system_id_app_t;

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
system_state_t  system_state  = BOOT_STATE;
system_id_app_t system_id_app = TIME;
uint8_t         object_select = SYSTEM_SELECTING;

/* Private function prototypes ---------------------------------------- */
static system_state_t system_manager_check_state(button_name_t *button_state);
/* Function definitions ----------------------------------------------- */
static system_state_t system_manager_check_state(button_name_t *button_state)
{
    drv_button_check_event(button_state);

    /* FSM */
    switch (system_state)
    {
    case BOOT_STATE:
        if (*button_state == HOLD_SELECT_BUTTON)
            system_state = WAIT_STATE;
        break;

    case WAIT_STATE:
        if (*button_state == CLICK_SELECT_BUTTON)
            system_state = MENU_STATE;
        break;

    case MENU_STATE:
        // Check id_app here
        if (*button_state == CLICK_RIGHT_BUTTON)
        {
            system_id_app++;
            if (system_id_app >= PC_CONTROL)
                system_id_app = PC_CONTROL;
        }

        else if (*button_state == CLICK_LEFT_BUTTON)
        {
            system_id_app--;
            if (system_id_app <= TIME)
                system_id_app = TIME;
        }

        if (*button_state == CLICK_SELECT_BUTTON && system_id_app == GAME)
            system_state = GAME_STATE;

        else if (*button_state == CLICK_SELECT_BUTTON && system_id_app == TIME)
            system_state = TIME_STATE;

        else if (*button_state == CLICK_SELECT_BUTTON && system_id_app == DIAGNOSTIC)
            system_state = DIAGNOSTIC_STATE;

        else if (*button_state == CLICK_SELECT_BUTTON && system_id_app == THEME)
            system_state = THEME_STATE;

        else if (*button_state == CLICK_SELECT_BUTTON && system_id_app == PC_CONTROL)
            system_state = PC_CONTROL_STATE;

        else if (*button_state == CLICK_CANCEL_BUTTON)
            system_state = WAIT_STATE;

        break;
    case GAME_STATE:
        /* Game state handle */
        if (*button_state == HOLD_CANCEL_BUTTON)
            system_state = WAIT_STATE;
        if (*button_state == CLICK_CANCEL_BUTTON)
            system_state = MENU_STATE;
        break;
    case TIME_STATE:
        /* Time state handle */
        if (*button_state == CLICK_CANCEL_BUTTON)
        {
            system_state  = MENU_STATE;
            object_select = SYSTEM_SELECTING;
        }
        break;
    case THEME_STATE:
        /* Theme state handle */
        if (*button_state == CLICK_CANCEL_BUTTON || *button_state == HOLD_CANCEL_BUTTON)
        {
            system_state  = MENU_STATE;
            object_select = SYSTEM_SELECTING;
        }
        break;
    case DIAGNOSTIC_STATE:
        /* Diagnostic state handle */
        if (*button_state == CLICK_CANCEL_BUTTON || *button_state == HOLD_CANCEL_BUTTON)
        {
            system_state  = MENU_STATE;
            object_select = SYSTEM_SELECTING;
        }
        break;
    case PC_CONTROL_STATE:
        /* PC Control state handle */
        if (*button_state == CLICK_CANCEL_BUTTON || *button_state == HOLD_CANCEL_BUTTON)
        {
            system_state  = MENU_STATE;
            object_select = SYSTEM_SELECTING;
        }
        break;
    default: break;
    }

    return system_state;
}

system_manager_error_t system_manager_init(void)
{
    system_data_init();
    system_game_init();
    system_display_init();
    system_time_init();
    system_pc_control_init();
    drv_button_init();
    drv_buzzer_init();
    drv_led_init();

    drv_uart_receive();
    return SYSTEM_MANAGER_OK;
}

system_manager_error_t system_manager_run(button_name_t *button_state)
{
    system_step_counter();
    system_step_heart_rate();

    return status;
}
/* End of file -------------------------------------------------------- */
