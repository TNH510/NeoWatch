/**
 * @file       bsp_lcd.c
 * @copyright  Copyright (C) 202NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-09
 * @author     Thach Nguyen Ba Vu
 * @brief      Board Support Package
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "bsp_lcd.h"

#include "driver/i2c_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "video_frame.h"
#include "bsp_rtc.h"

#include <esp_heap_caps.h>
#include <sys/lock.h>
#include <unistd.h>

#include "ui.h"

static const char *TAG = "BSP_LCD";

/* Private defines ---------------------------------------------------- */
#define I2C_BUS_PORT           0
#define PIN_NUM_SDA            22
#define PIN_NUM_SCL            21
#define PIN_NUM_RST            -1
#define I2C_HW_ADDR            0x3C  // 0x3C or 0x3D for SSD1306, 0x3C for SH1107
#define LCD_PIXEL_CLOCK_HZ     (400 * 1000)
#define I2C_HW_ADDR            0x3C  // 0x3C or 0x3D for SSD1306, 0x3C for SH1107
// Bit number used to represent command and parameter
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8
// The pixel number in horizontal and vertical
#define LCD_H_RES              128
#define LCD_V_RES              64

#define LVGL_TICK_PERIOD_MS    5
#define LVGL_TASK_STACK_SIZE   (4 * 1024)
#define LVGL_TASK_PRIORITY     2
#define LVGL_PALETTE_SIZE      8
#define LVGL_TASK_MAX_DELAY_MS 500
#define LVGL_TASK_MIN_DELAY_MS 1000 / CONFIG_FREERTOS_HZ
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static i2c_master_bus_handle_t   i2c_bus      = NULL;
static esp_lcd_panel_io_handle_t io_handle    = NULL;
static esp_lcd_panel_handle_t    panel_handle = NULL;
// To use LV_COLOR_FORMAT_I1, we need an extra buffer to hold the converted data
static uint8_t oled_buffer[LCD_H_RES * LCD_V_RES / 8];
static uint8_t current_display[LCD_H_RES * LCD_V_RES / 8];
 
// LVGL library is not thread-safe, this example will call LVGL APIs from different tasks, so use a mutex to
// protect it
static _lock_t lvgl_api_lock;
// Biến kiểm soát animation task
static TaskHandle_t  animation_task_handle = NULL;
static volatile bool animation_running     = false;
// Biến theo dõi video đang phát
static uint8_t current_video = 0xFF; // 0xFF nghĩa là không có video nào đang phát

// create a lvgl display
lv_display_t *display;
// Static label object to reuse for clock display
static lv_obj_t *s_clock_label = NULL;

base_status_t bsp_lcd_clock_set_mode(bsp_lcd_clock_t mode)
{
    if (mode >= BSP_LCD_CLOCK_TYPE_MAX)
    {
        return BS_ERROR;
    }

    return BS_OK;
}
/* Private function prototypes ---------------------------------------- */
/**
 * @brief init I2C for LCD
 *
 */
static base_status_t m_i2c_init(void);
/**
 * @brief
 *
 * @return base_status_t
 */
static base_status_t m_panel_init(void);
/**
 * @brief
 *
 * @return base_status_t
 */
static base_status_t m_driver_ssd1306_init(void);
/**
 * @brief
 *
 * @return base_status_t
 */
static base_status_t m_graphic_init(void);
/**
 * @brief
 *
 * @param disp
 * @param area
 * @param px_map
 */
static void m_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

/**
 * @brief
 *
 * @param io_panel
 * @param edata
 * @param user_ctx
 * @return true
 * @return false
 */
static bool m_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t      io_panel,
                                      esp_lcd_panel_io_event_data_t *edata,
                                      void                          *user_ctx);
/**
 * @brief
 *
 * @param arg
 */
static void m_increase_lvgl_tick(void *arg);
/**
 * @brief
 *
 * @param arg
 */
static void m_lvgl_port_task(void *arg);
/**
 * @brief Animation task function
 *
 * @param arg Task arguments
 */
static void m_animation_task_1(void *arg);
static void m_animation_task_2(void *arg);
static void m_animation_task_3(void *arg);
/**
 * @brief
 *
 * @param buf
 * @param x
 * @param y
 */
static inline void _set_oled_pixel(uint8_t *buf, int x, int y);
/**
 * @brief
 *
 * @param buf
 */
static void draw_smiley_to_buffer(uint8_t *buf);
/**
 * @brief
 *
 * @param src
 * @param dst
 * @param w
 * @param h
 */
static void convert_horiz_msb_to_ssd1306(const uint8_t *src, uint8_t *dst, int w, int h);

/* Function definitions ----------------------------------------------- */
static inline void _set_oled_pixel(uint8_t *buf, int x, int y)
{
    if (x < 0 || x >= LCD_H_RES || y < 0 || y >= LCD_V_RES)
        return;
    size_t  idx  = (size_t) x + ((size_t) (y >> 3) * (size_t) LCD_H_RES);
    uint8_t mask = (uint8_t) (1u << (y & 7));
    buf[idx] |= mask;
}
static void draw_smiley_to_buffer(uint8_t *buf)
{
    memset(buf, 0x00, LCD_H_RES * LCD_V_RES / 8);

    // mắt
    _set_oled_pixel(buf, 40, 20);
    _set_oled_pixel(buf, 44, 20);
    _set_oled_pixel(buf, 40, 24);
    _set_oled_pixel(buf, 44, 24);

    // miệng (đường cong đơn giản)
    for (int x = 34; x <= 50; x++) _set_oled_pixel(buf, x, 40);
    for (int x = 36; x <= 48; x++) _set_oled_pixel(buf, x, 41);
    for (int x = 38; x <= 46; x++) _set_oled_pixel(buf, x, 42);
}

static void convert_horiz_msb_to_ssd1306(const uint8_t *src, uint8_t *dst, int w, int h)
{
    int bytes_per_row = w >> 3;
    memset(dst, 0x00, (size_t) w * (size_t) h / 8);

    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            int     src_idx  = y * bytes_per_row + (x >> 3);
            uint8_t src_byte = src[src_idx];
            int     src_bit  = 7 - (x & 7);  // MSB-first in row
            bool    pixel_on = ((src_byte >> src_bit) & 0x1) != 0;
            if (pixel_on)
            {
                size_t dst_idx = (size_t) x + ((size_t) (y >> 3) * (size_t) w);
                dst[dst_idx] |= (uint8_t) (1u << (y & 7));
            }
        }
    }
}

static void m_display_update(void)
{
    // Chuyển đổi dữ liệu frame sang định dạng SSD1306
    convert_horiz_msb_to_ssd1306(current_display, oled_buffer, LCD_H_RES, LCD_V_RES);
    // Vẽ frame lên màn hình
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, oled_buffer);
}

base_status_t bsp_lcd_init(void)
{
    ESP_LOGI(TAG, "Initialize I2C bus");
    CHECK_STATUS(m_i2c_init());
    CHECK_STATUS(m_panel_init());
    CHECK_STATUS(m_driver_ssd1306_init());
    m_graphic_init();

    // ui_init();

    // memset(oled_buffer, 0x00, sizeof(oled_buffer));
    // esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, oled_buffer);
    // esp_lcd_panel_invert_color(panel_handle, false);

    return BS_OK;
}

static void m_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

void bsp_lcd_clock_display(uint16_t year, uint8_t month, uint16_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    // Lock the mutex due to the LVGL APIs are not thread-safe
    _lock_acquire(&lvgl_api_lock);
    
    lv_obj_t *scr = lv_display_get_screen_active(display);
    if (scr == NULL) {
        _lock_release(&lvgl_api_lock);
        return;
    }

    // // Clear previous label if screen changed or first time initialization
    // if (s_clock_label != NULL && lv_obj_get_parent(s_clock_label) != scr)
    // {
    //     lv_obj_del(s_clock_label);
    //     s_clock_label = NULL;
    // }

    // // Create label only if it doesn't exist yet
    // if (s_clock_label == NULL)
    // {
    //     s_clock_label = lv_label_create(scr);
    //     // lv_label_set_long_mode(s_clock_label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */

    //     /* Size of the screen (if you use rotation 90 or 270, please use lv_display_get_vertical_resolution)
    //      */
    //     lv_obj_set_width(s_clock_label, lv_display_get_horizontal_resolution(display));
    //     lv_obj_align(s_clock_label, LV_ALIGN_TOP_MID, 0, 0);
    // }

    // // Update time text on the existing label
    // char time_str[14];
    // bsp_rtc_string_timestyle(time_str, bsp_rtc_get_time(), ':');
    // lv_label_set_text(s_clock_label, time_str);

    ui_init();
    _lock_release(&lvgl_api_lock);

    // m_display_update();
}

void bsp_lcd_demo_video(uint8_t video_num)
{
    if (video_num >= 3)
    {
        return;
    }
    
    // Kiểm tra và dừng task đang chạy nếu có
    if (animation_running && animation_task_handle != NULL)
    {
        // Đánh dấu để task dừng vòng lặp animation
        animation_running = false;
        
        // Đợi một khoảng thời gian ngắn để task kết thúc vòng lặp hiện tại
        vTaskDelay(pdMS_TO_TICKS(30));
        
        // Xóa task trước đó nếu nó chưa tự xóa
        if (animation_task_handle != NULL)
        {
            vTaskDelete(animation_task_handle);
            animation_task_handle = NULL;
            ESP_LOGI(TAG, "Previous animation task stopped");
        }
    }
    
    // Tạo task mới cho video được chọn
    if (video_num == 0)
    {
        // Tạo task chạy animation thay vì dùng vòng lặp for
        animation_running = true;
        xTaskCreate(m_animation_task_1, "anim_task", 2048, NULL, 3, &animation_task_handle);
    }
    else if (video_num == 1)
    {
        // Tạo task chạy animation thay vì dùng vòng lặp for
        animation_running = true;
        xTaskCreate(m_animation_task_2, "anim_task", 2048, NULL, 3, &animation_task_handle);
    }
    else if (video_num == 2)
    {
        // Tạo task chạy animation thay vì dùng vòng lặp for
        animation_running = true;
        xTaskCreate(m_animation_task_3, "anim_task", 2048, NULL, 3, &animation_task_handle);
    }
}

static base_status_t m_i2c_init(void)
{
    i2c_master_bus_config_t bus_config = {
        .clk_source                   = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt            = 7,
        .i2c_port                     = I2C_BUS_PORT,
        .sda_io_num                   = PIN_NUM_SDA,
        .scl_io_num                   = PIN_NUM_SCL,
        .flags.enable_internal_pullup = true,
    };

    if (i2c_new_master_bus(&bus_config, &i2c_bus) == ESP_OK)
    {
        ESP_LOGI(TAG, "Initialized I2C bus success");
        return BS_OK;
    }
    return BS_ERROR;
}

static base_status_t m_panel_init(void)
{
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr            = I2C_HW_ADDR,
        .scl_speed_hz        = LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,
        .lcd_cmd_bits        = LCD_CMD_BITS,
        .lcd_param_bits      = LCD_CMD_BITS,
        .dc_bit_offset       = 6,
    };
    if (esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle) == ESP_OK)
    {
        ESP_LOGI(TAG, "Installed Panel IO success");
        return BS_OK;
    }
    return BS_ERROR;
}

static base_status_t m_driver_ssd1306_init(void)
{
    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = PIN_NUM_RST,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    CHECK_ESP_STATUS(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    CHECK_ESP_STATUS(esp_lcd_panel_reset(panel_handle));
    CHECK_ESP_STATUS(esp_lcd_panel_init(panel_handle));
    CHECK_ESP_STATUS(esp_lcd_panel_disp_on_off(panel_handle, true));
    CHECK_ESP_STATUS(esp_lcd_panel_mirror(panel_handle, true, true));
    ESP_LOGI(TAG, "Installed SSD1306 panel driver success");
    return BS_OK;
}

static void m_lvgl_port_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t time_till_next_ms = 0;
    while (1)
    {
        _lock_acquire(&lvgl_api_lock);
        time_till_next_ms = lv_timer_handler();
        _lock_release(&lvgl_api_lock);
        // in case of triggering a task watch dog time out
        time_till_next_ms = MAX(time_till_next_ms, LVGL_TASK_MIN_DELAY_MS);
        // in case of lvgl display not ready yet
        time_till_next_ms = MIN(time_till_next_ms, LVGL_TASK_MAX_DELAY_MS);
        usleep(1000 * time_till_next_ms);
    }
}

static base_status_t m_graphic_init(void)
{
    ESP_LOGI(TAG, "Initialize LVGL");
    lv_init();
    display = lv_display_create(LCD_H_RES, LCD_V_RES);

    // associate the i2c panel handle to the display
    lv_display_set_user_data(display, panel_handle);
    // create draw buffer
    void *buf = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers");
    // LVGL reserves 2 x 4 bytes in the buffer, as these are assumed to be used as a palette.
    size_t draw_buffer_sz = LCD_H_RES * LCD_V_RES / 8 + LVGL_PALETTE_SIZE;
    buf                   = heap_caps_calloc(1, draw_buffer_sz, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf);

    // LVGL9 suooprt new monochromatic format.
    lv_display_set_color_format(display, LV_COLOR_FORMAT_I1);
    // initialize LVGL draw buffers
    lv_display_set_buffers(display, buf, NULL, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_FULL);
    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(display, m_lvgl_flush_cb);

    ESP_LOGI(TAG, "Register io panel event callback for LVGL flush ready notification");
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = m_notify_lvgl_flush_ready,
    };
    /* Register done callback */
    esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, display);

    ESP_LOGI(TAG, "Use esp_timer as LVGL tick timer");
    const esp_timer_create_args_t lvgl_tick_timer_args = { .callback = &m_increase_lvgl_tick,
                                                           .name     = "lvgl_tick" };
    esp_timer_handle_t            lvgl_tick_timer      = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreate(m_lvgl_port_task, "LVGL", LVGL_TASK_STACK_SIZE, NULL, LVGL_TASK_PRIORITY, NULL);

    return BS_OK;
}

static void m_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);

    // This is necessary because LVGL reserves 2 x 4 bytes in the buffer, as these are assumed to be used as a
    // palette. Skip the palette here More information about the monochrome, please refer to
    // https://docs.lvgl.io/9.2/porting/display.html#monochrome-displays
    px_map += LVGL_PALETTE_SIZE;

    uint16_t hor_res = lv_display_get_physical_horizontal_resolution(disp);
    int      x1      = area->x1;
    int      x2      = area->x2;
    int      y1      = area->y1;
    int      y2      = area->y2;

    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            /* The order of bits is MSB first
                        MSB           LSB
               bits      7 6 5 4 3 2 1 0
               pixels    0 1 2 3 4 5 6 7
                        Left         Right
            */
            bool chroma_color = (px_map[(hor_res >> 3) * y + (x >> 3)] & 1 << (7 - x % 8));

            /* Write to the buffer as required for the display.
             * It writes only 1-bit for monochrome displays mapped vertically.*/
            uint8_t *buf = oled_buffer + hor_res * (y >> 3) + (x);
            if (chroma_color)
            {
                (*buf) &= ~(1 << (y % 8));
            }
            else
            {
                (*buf) |= (1 << (y % 8));
            }
        }
    }
    // pass the draw buffer to the driver
    esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, oled_buffer);
}

static bool m_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t      io_panel,
                                      esp_lcd_panel_io_event_data_t *edata,
                                      void                          *user_ctx)
{
    lv_display_t *disp = (lv_display_t *) user_ctx;
    lv_display_flush_ready(disp);
    return false;
}

/**
 * @brief Task để chạy animation
 *
 * @param arg Task arguments
 */
static void m_animation_task_1(void *arg)
{
    ESP_LOGI(TAG, "Animation task started");

    // Chạy animation từ frame 0 đến frame 141
    for (int i = 0; i < 60 && animation_running; i++)
    {
        // Chuyển đổi dữ liệu frame sang định dạng SSD1306
        convert_horiz_msb_to_ssd1306(throw_computer_video[i], oled_buffer, LCD_H_RES, LCD_V_RES);

        // Vẽ frame lên màn hình
        esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, oled_buffer);

        // Delay để tạo hiệu ứng animation (20ms mỗi frame - 50 FPS)
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    ESP_LOGI(TAG, "Animation completed");
    animation_running     = false;
    animation_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Task để chạy animation
 *
 * @param arg Task arguments
 */
static void m_animation_task_2(void *arg)
{
    ESP_LOGI(TAG, "Animation task started");

    // Chạy animation từ frame 0 đến frame 60
    for (int i = 0; i < 60 && animation_running; i++)
    {
        // Chuyển đổi dữ liệu frame sang định dạng SSD1306
        convert_horiz_msb_to_ssd1306(em_chua_18_video[i], oled_buffer, LCD_H_RES, LCD_V_RES);

        // Vẽ frame lên màn hình
        esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, oled_buffer);

        // Delay để tạo hiệu ứng animation (20ms mỗi frame - 50 FPS)
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    ESP_LOGI(TAG, "Animation completed");
    animation_running     = false;
    animation_task_handle = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief Task để chạy animation
 *
 * @param arg Task arguments
 */
static void m_animation_task_3(void *arg)
{
    ESP_LOGI(TAG, "Animation task started");

    // Chạy animation từ frame 0 đến frame 60
    for (int i = 0; i < 60 && animation_running; i++)
    {
        // Chuyển đổi dữ liệu frame sang định dạng SSD1306
        convert_horiz_msb_to_ssd1306(meme_cuoi_video[i], oled_buffer, LCD_H_RES, LCD_V_RES);

        // Vẽ frame lên màn hình
        esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, oled_buffer);

        // Delay để tạo hiệu ứng animation (20ms mỗi frame - 50 FPS)
        vTaskDelay(pdMS_TO_TICKS(40));
    }

    ESP_LOGI(TAG, "Animation completed");
    animation_running     = false;
    animation_task_handle = NULL;
    vTaskDelete(NULL);
}

/* End of file -------------------------------------------------------- */
