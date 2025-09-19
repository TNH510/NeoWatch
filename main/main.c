#include "base_type.h"
#include "bsp_i2c.h"
#include "bsp_lcd.h"

#include <stdio.h>

void app_main(void)
{
    bsp_lcd_init();
    // bsp_lcd_clock_display();
}