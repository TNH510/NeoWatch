
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/task.h"
#include "nimble-nordic-uart.h"
#include "base_type.h"
#include "bsp_i2c.h"
#include "bsp_lcd.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    bsp_lcd_init();
    nordic_uart_start("Nordic UART", NULL);
    xTaskCreate(echoTask, "echoTask", 5000, NULL, 1, NULL);
}