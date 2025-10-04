
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/task.h"
#include "nimble-nordic-uart.h"
#include "base_type.h"
#include "bsp_i2c.h"
#include "bsp_lcd.h"
#include "system_display.h"
#include "bsp_rtc.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Function prototypes
void echoTask(void *parameter);
void display_clock_task(void *pvParameters);

void echoTask(void *parameter)
{
    static char mbuf[CONFIG_NORDIC_UART_MAX_LINE_LENGTH + 1];

    for (;;)
    {
        size_t item_size;
        if (nordic_uart_rx_buf_handle)
        {
            const char *item =
              (char *) xRingbufferReceive(nordic_uart_rx_buf_handle, &item_size, portMAX_DELAY);

            if (item)
            {
                // Tạo bản sao tạm thời của dữ liệu nhận được để in dạng string
                memcpy(mbuf, item, item_size);
                mbuf[item_size] = '\0'; // Thêm ký tự null để kết thúc chuỗi
                
                // In dữ liệu dạng chuỗi string
                printf("Received %d bytes: \"%s\"\n", item_size, mbuf);
                
                // Xử lý dữ liệu nhận được từ BLE
                if (strstr(mbuf, "buttonIndex : ") != NULL) {
                    // Tìm vị trí của giá trị buttonIndex
                    char *pos = strstr(mbuf, "buttonIndex : ") + strlen("buttonIndex : ");
                    
                    // Lấy giá trị buttonIndex
                    int buttonIndex = atoi(pos);
                    
                    printf("Detected buttonIndex: %d\n", buttonIndex);
                    bsp_lcd_demo_video(buttonIndex);
                }
                
                // // Nếu muốn vẫn xem dữ liệu dạng hex, có thể giữ lại dòng này
                // printf("Hex: ");
                // int i;
                // for (i = 0; i < item_size; ++i)
                // {
                //     printf("%02X ", (unsigned char) item[i]);
                // }
                // printf("\n");

                vRingbufferReturnItem(nordic_uart_rx_buf_handle, (void *) item);
            }
        }
        else
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    system_display_init();
    
    // Create a task for the display clock to run in background
    xTaskCreate(display_clock_task, "display_clock", 2048, NULL, 5, NULL);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // Nhường CPU mỗi 100ms
    }
    
    
    // Initialize Bluetooth if needed
    // nimble_nordic_uart_init();
    
    // Create echo task for BLE
    // xTaskCreate(echoTask, "echoTask", 4096, NULL, 5, NULL);
    
    // Main task can exit now - FreeRTOS scheduler will take over
}

// Separate task for display clock
void display_clock_task(void *pvParameters)
{
    while (1)
    {
        system_display_clock();
        // Task delay already in system_display_clock()
    }
}