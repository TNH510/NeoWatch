#include "base_type.h"
#include "bsp_i2c.h"

#include <stdio.h>

void app_main(void)
{
    bool test_var = false;
    test_var = bsp_i2c_init();
    test_var == true ? printf("I2C Init Success\n") : printf("I2C Init Fail\n");
}