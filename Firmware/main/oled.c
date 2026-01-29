#include "oled.h"
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include <u8g2.h>
#include "u8g2_esp32_hal.h"

// CLK - GPIO12
#define PIN_CLK 12

// MOSI - GPIO 11
#define PIN_MOSI 11

// RESET - GPIO 8
#define PIN_RESET 8

// DC - GPIO 9
#define PIN_DC 9

// CS - GPIO 10
#define PIN_CS 10

static const char *TAG = "OLED";

static u8g2_t u8g2;

/* Stopwatch Assets*/
static const uint8_t image_battery_full_bits[] = {0x00,0x00,0x00,0xf0,0xff,0x7f,0x08,0x00,0x80,0x68,0xdb,0xb6,0x6e,0xdb,0xb6,0x61,0xdb,0xb6,0x61,0xdb,0xb6,0x61,0xdb,0xb6,0x61,0xdb,0xb6,0x61,0xdb,0xb6,0x6e,0xdb,0xb6,0x68,0xdb,0xb6,0x08,0x00,0x80,0xf0,0xff,0x7f,0x00,0x00,0x00,0x00,0x00,0x00};
static const uint8_t image_battery_17_bits[] = {0x00,0x00,0x00,0xf0,0xff,0x7f,0x08,0x00,0x80,0x08,0x00,0xb0,0x0e,0x00,0xb0,0x01,0x00,0xb0,0x01,0x00,0xb0,0x01,0x00,0xb0,0x01,0x00,0xb0,0x01,0x00,0xb0,0x0e,0x00,0xb0,0x08,0x00,0xb0,0x08,0x00,0x80,0xf0,0xff,0x7f,0x00,0x00,0x00,0x00,0x00,0x00};
static const uint8_t image_battery_33_bits[] = {0x00,0x00,0x00,0xf0,0xff,0x7f,0x08,0x00,0x80,0x08,0x00,0xb6,0x0e,0x00,0xb6,0x01,0x00,0xb6,0x01,0x00,0xb6,0x01,0x00,0xb6,0x01,0x00,0xb6,0x01,0x00,0xb6,0x0e,0x00,0xb6,0x08,0x00,0xb6,0x08,0x00,0x80,0xf0,0xff,0x7f,0x00,0x00,0x00,0x00,0x00,0x00};
static const uint8_t image_battery_50_bits[] = {0x00,0x00,0x00,0xf0,0xff,0x7f,0x08,0x00,0x80,0x08,0xc0,0xb6,0x0e,0xc0,0xb6,0x01,0xc0,0xb6,0x01,0xc0,0xb6,0x01,0xc0,0xb6,0x01,0xc0,0xb6,0x01,0xc0,0xb6,0x0e,0xc0,0xb6,0x08,0xc0,0xb6,0x08,0x00,0x80,0xf0,0xff,0x7f,0x00,0x00,0x00,0x00,0x00,0x00};
static const uint8_t image_battery_67_bits[] = {0x00,0x00,0x00,0xf0,0xff,0x7f,0x08,0x00,0x80,0x08,0xd8,0xb6,0x0e,0xd8,0xb6,0x01,0xd8,0xb6,0x01,0xd8,0xb6,0x01,0xd8,0xb6,0x01,0xd8,0xb6,0x01,0xd8,0xb6,0x0e,0xd8,0xb6,0x08,0xd8,0xb6,0x08,0x00,0x80,0xf0,0xff,0x7f,0x00,0x00,0x00,0x00,0x00,0x00};
static const uint8_t image_battery_83_bits[] = {0x00,0x00,0x00,0xf0,0xff,0x7f,0x08,0x00,0x80,0x08,0xdb,0xb6,0x0e,0xdb,0xb6,0x01,0xdb,0xb6,0x01,0xdb,0xb6,0x01,0xdb,0xb6,0x01,0xdb,0xb6,0x01,0xdb,0xb6,0x0e,0xdb,0xb6,0x08,0xdb,0xb6,0x08,0x00,0x80,0xf0,0xff,0x7f,0x00,0x00,0x00,0x00,0x00,0x00};

static const uint8_t image_chrono_minute_colon_bits[] = {0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03};
static const uint8_t image_chrono_second_point_bits[] = {0x03,0x03};
static const uint8_t image_lap_string_bits[] = {0x03,0x30,0xfc,0x00,0x03,0x78,0xfc,0x01,0x03,0x78,0x0c,0x03,0x03,0xcc,0x0c,0x03,0x03,0xcc,0xfc,0x01,0x03,0x86,0xfd,0x00,0x03,0xfe,0x0d,0x00,0x03,0xfe,0x0d,0x00,0x03,0x86,0x0d,0x00,0xff,0x86,0x0d,0x00,0xff,0x86,0x0d,0x00};
static const uint8_t image_split_hour_colon_bits[] = {0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x03,0x03};
static const uint8_t image_split_string_bits[] = {0x3c,0x7e,0x0c,0xf8,0xf7,0x0f,0x7e,0xfe,0x0c,0xf8,0xf7,0x0f,0xc3,0x86,0x0d,0xc0,0x80,0x01,0x03,0x86,0x0d,0xc0,0x80,0x01,0x3e,0xfe,0x0c,0xc0,0x80,0x01,0x7c,0x7e,0x0c,0xc0,0x80,0x01,0xe0,0x06,0x0c,0xc0,0x80,0x01,0xc3,0x06,0x0c,0xc0,0x80,0x01,0xc3,0x06,0x0c,0xc0,0x80,0x01,0x7e,0x06,0xfc,0xfb,0x87,0x01,0x3c,0x06,0xfc,0xfb,0x87,0x01};

static const uint8_t image_wifi_connected_bits[] = {0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x70,0x00,0x77,0x00,0x77,0x00,0x77,0x00,0x77,0x70,0x77,0x70,0x77,0x70,0x77,0x70,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x00,0x00};
static const uint8_t image_wifi_not_connected_bits[] = {0x41,0x70,0x22,0x50,0x14,0x50,0x08,0x50,0x14,0x57,0x22,0x55,0x41,0x55,0x00,0x55,0x70,0x55,0x50,0x55,0x50,0x55,0x50,0x55,0x57,0x55,0x55,0x55,0x77,0x77,0x00,0x00};
static const uint8_t image_wifi_1_bar_bits[] = {0x00,0x70,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x57,0x00,0x55,0x00,0x55,0x00,0x55,0x70,0x55,0x50,0x55,0x50,0x55,0x50,0x55,0x57,0x55,0x57,0x55,0x77,0x77,0x00,0x00};
static const uint8_t image_wifi_2_bars_bits[] = {0x00,0x70,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x57,0x00,0x55,0x00,0x55,0x00,0x55,0x70,0x55,0x70,0x55,0x70,0x55,0x70,0x55,0x77,0x55,0x77,0x55,0x77,0x77,0x00,0x00};
static const uint8_t image_wifi_3_bars_bits[] = {0x00,0x70,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x57,0x00,0x57,0x00,0x57,0x00,0x57,0x70,0x57,0x70,0x57,0x70,0x57,0x70,0x57,0x77,0x57,0x77,0x57,0x77,0x77,0x00,0x00};

/* Swimming Watch Assets*/
static const uint8_t image_music_radio_streaming_bits[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x40,0x00,0x02,0x80,0x00,0x12,0x90,0x00,0x09,0x21,0x01,0xa5,0x4b,0x01,0x95,0x52,0x01,0xa5,0x4b,0x01,0x09,0x21,0x01,0x12,0x90,0x00,0x02,0x80,0x00,0x04,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void draw_battery(u8g2_t *u8g2, uint8_t level) {
    u8g2_SetBitmapMode(u8g2, 1);
    // battery_full
    switch (level)
    {
    case BATTERY_FULL:
        /* code */
        u8g2_DrawXBM(u8g2, 0, 0, 24, 16, image_battery_full_bits);
        break;
    case BATTERY_83:
        u8g2_DrawXBM(u8g2, 0, 0, 24, 16, image_battery_83_bits);
        break;
    case BATTERY_67:
        u8g2_DrawXBM(u8g2, 0, 0, 24, 16, image_battery_67_bits);
        break;
    case BATTERY_50:
        u8g2_DrawXBM(u8g2, 0, 0, 24, 16, image_battery_50_bits);
        break;  
    case BATTERY_33:    
        u8g2_DrawXBM(u8g2, 0, 0, 24, 16, image_battery_33_bits);
        break;
    case BATTERY_17:
        u8g2_DrawXBM(u8g2, 0, 0, 24, 16, image_battery_17_bits);
        break;
    default:
        break;
    }
    
}

void draw_wfi_status(u8g2_t *u8g2, uint8_t signal) {
    u8g2_SetBitmapMode(u8g2, 1);
    // network_not_connected
    switch (signal)
    {
        case WIFI_NOT_CONNECTED:
            u8g2_DrawXBM(u8g2, 112, 0, 16, 16, image_wifi_not_connected_bits);
            break;
        case WIFI_1_BAR:
            u8g2_DrawXBM(u8g2, 112, 0, 16, 16, image_wifi_1_bar_bits);
            break;  
        case WIFI_2_BARS:  
            u8g2_DrawXBM(u8g2, 112, 0, 16, 16, image_wifi_2_bars_bits);
            break;
        case WIFI_3_BARS:   
            u8g2_DrawXBM(u8g2, 112, 0, 16, 16, image_wifi_3_bars_bits);
            break;
        case WiFI_CONNECTED:  
            u8g2_DrawXBM(u8g2, 112, 0, 16, 16, image_wifi_connected_bits);
            break;
    }
}

void draw_frame(u8g2_t *u8g2) {
    u8g2_SetBitmapMode(u8g2, 1);
    // line1
    u8g2_DrawLine(u8g2, 1, 35, 126, 35);
    // line2
    u8g2_DrawLine(u8g2, 1, 35, 1, 63);
    // line3
    u8g2_DrawLine(u8g2, 50, 35, 50, 63);
    // line4
    u8g2_DrawLine(u8g2, 126, 35, 126, 63);
    // line5
    u8g2_DrawLine(u8g2, 1, 50, 125, 50);
    // line6
    u8g2_DrawLine(u8g2, 2, 63, 125, 63);

    // chrono_minute_colon
    u8g2_DrawXBM(u8g2, 58, 19, 2, 11, image_chrono_minute_colon_bits);
    // chrono_hour_colon
    u8g2_DrawXBM(u8g2, 24, 19, 2, 11, image_chrono_minute_colon_bits);
    // chrono_second_point
    u8g2_DrawXBM(u8g2, 92, 31, 2, 2, image_chrono_second_point_bits);
    // split_hour_colon
    u8g2_DrawXBM(u8g2, 62, 39, 2, 9, image_split_hour_colon_bits);
    // lap_hour_colon
    u8g2_DrawXBM(u8g2, 62, 53, 2, 9, image_split_hour_colon_bits);
    // split_minute_colon
    u8g2_DrawXBM(u8g2, 83, 39, 2, 9, image_split_hour_colon_bits);
    // lap_minute_colon
    u8g2_DrawXBM(u8g2, 83, 53, 2, 9, image_split_hour_colon_bits);
    // split_second_point
    u8g2_DrawXBM(u8g2, 104, 47, 2, 2, image_chrono_second_point_bits);
    // lap_second_point
    u8g2_DrawXBM(u8g2, 104, 61, 2, 2, image_chrono_second_point_bits);
    // split_string
    u8g2_DrawXBM(u8g2, 3, 38, 44, 11, image_split_string_bits);
    // lap_string
    u8g2_DrawXBM(u8g2, 3, 52, 26, 11, image_lap_string_bits);
}

void draw_streaming_icon(u8g2_t *u8g2, bool is_streaming) {
    u8g2_SetBitmapMode(u8g2, 1);
    // radio_streaming
    if (is_streaming){
        u8g2_DrawXBM(u8g2, 56, 0, 17, 16, image_music_radio_streaming_bits);
    }
}

void draw_stopwatch(u8g2_t *u8g2, const char *sw_str, const char *lap_str, const char *split_str, const char *lap_count_str) {
    const char chrono_hour[2] = {sw_str[1], '\0'};
    const char chrono_minute[3] = {sw_str[3], sw_str[4], '\0'};
    const char chrono_second[3] = {sw_str[6], sw_str[7], '\0'};
    const char chrono_millisecond[3] = {sw_str[9], sw_str[10], '\0'};
    
    u8g2_SetFontMode(u8g2, 1);

    u8g2_SetFont(u8g2, u8g2_font_profont29_tr);
    // chrono_hour
    u8g2_DrawStr(u8g2, 9, 34, chrono_hour);
    // chrono_minute
    u8g2_DrawStr(u8g2, 27, 34, chrono_minute);
    // chrono_second
    u8g2_DrawStr(u8g2, 61, 34, chrono_second);
    // chrono_millisecond
    u8g2_SetFont(u8g2, u8g2_font_profont22_tr);
    u8g2_DrawStr(u8g2, 95, 34, chrono_millisecond);
    
    const char lap_hour[2] = {lap_str[1], '\0'};
    const char lap_minute[3] = {lap_str[3], lap_str[4], '\0'};
    const char lap_second[3] = {lap_str[6], lap_str[7], '\0'};  
    const char lap_millisecond[3] = {lap_str[9], lap_str[10], '\0'};

    u8g2_SetFont(u8g2, u8g2_font_profont17_tr);
    // lap_hour
    u8g2_DrawStr(u8g2, 53, 63, lap_hour);
    // lap_minute
    u8g2_DrawStr(u8g2, 65, 63, lap_minute);
    // lap_second
    u8g2_DrawStr(u8g2, 86, 63, lap_second);
    // lap_millisecond
    u8g2_DrawStr(u8g2, 107, 63, lap_millisecond);

    const char split_hour[2] = {split_str[1], '\0'};
    const char split_minute[3] = {split_str[3], split_str[4], '\0'};
    const char split_second[3] = {split_str[6], split_str[7], '\0'};  
    const char split_millisecond[3] = {split_str[9], split_str[10], '\0'};

     u8g2_SetFont(u8g2, u8g2_font_profont17_tr);
    // split_hour
    u8g2_DrawStr(u8g2, 53, 49, split_hour);
    // split_minute
    u8g2_DrawStr(u8g2, 65, 49, split_minute);
    // split_second
    u8g2_DrawStr(u8g2, 86, 49, split_second);
    // split_millisecond
    u8g2_DrawStr(u8g2, 107, 49, split_millisecond);


    // lap_counter
    const char lap_counter[3] = {lap_count_str[0], lap_count_str[1], '\0'};
    u8g2_SetFont(u8g2, u8g2_font_profont17_tr);
    u8g2_DrawStr(u8g2, 30, 63, lap_counter);
}

void draw_swimming_watch(u8g2_t *u8g2, const char *sw_str, const char *name_str, const char *type_str, const char *lane_count_str, const char *watch_number_str) {
    const char chrono_hour[2] = {sw_str[1], '\0'};
    const char chrono_minute[3] = {sw_str[3], sw_str[4], '\0'};
    const char chrono_second[3] = {sw_str[6], sw_str[7], '\0'};
    const char chrono_millisecond[3] = {sw_str[9], sw_str[10], '\0'};

    u8g2_SetFontMode(u8g2, 1);

    u8g2_SetFont(u8g2, u8g2_font_profont29_tr);
    // chrono_hour
    u8g2_DrawStr(u8g2, 9, 34, chrono_hour);
    // chrono_minute
    u8g2_DrawStr(u8g2, 27, 34, chrono_minute);
    // chrono_second
    u8g2_DrawStr(u8g2, 61, 34, chrono_second);
    // chrono_millisecond
    u8g2_SetFont(u8g2, u8g2_font_profont22_tr);
    u8g2_DrawStr(u8g2, 95, 34, chrono_millisecond);

    u8g2_SetFont(u8g2, u8g2_font_profont17_tr);
     // player_name
    u8g2_DrawStr(u8g2, 28, 48, name_str);
    // play_type
    u8g2_DrawStr(u8g2, 24, 63, type_str);
    // lane_count
    u8g2_SetFont(u8g2, u8g2_font_profont15_tr);
    u8g2_DrawStr(u8g2, 33, 11, lane_count_str);
    // watch_number
    u8g2_DrawStr(u8g2, 119, 12, watch_number_str);

    // frame
    u8g2_SetFont(u8g2, u8g2_font_profont15_tr);
    u8g2_DrawStr(u8g2, 5, 11, "LANE");
    u8g2_DrawStr(u8g2, 84, 12, "WATCH");

    u8g2_DrawLine(u8g2, 1, 35, 126, 35);
    u8g2_DrawLine(u8g2, 1, 50, 125, 50);
    u8g2_DrawLine(u8g2, 1, 35, 1, 63);
    u8g2_DrawLine(u8g2, 126, 35, 126, 63);
    u8g2_DrawLine(u8g2, 2, 63, 125, 63);
    u8g2_DrawXBM(u8g2, 58, 19, 2, 11, image_chrono_minute_colon_bits);
    u8g2_DrawXBM(u8g2, 24, 19, 2, 11, image_chrono_minute_colon_bits);
    u8g2_DrawXBM(u8g2, 92, 31, 2, 2, image_chrono_second_point_bits);
}

void draw_headtimer_page(u8g2_t *u8g2, const char *sw_str)
{
    // To be implemented
    const char chrono_hour[2] = {sw_str[1], '\0'};
    const char chrono_minute[3] = {sw_str[3], sw_str[4], '\0'};
    const char chrono_second[3] = {sw_str[6], sw_str[7], '\0'};
    const char chrono_millisecond[3] = {sw_str[9], sw_str[10], '\0'};

    u8g2_SetFontMode(u8g2, 1);

    u8g2_SetFont(u8g2, u8g2_font_profont29_tr);
    // chrono_hour
    u8g2_DrawStr(u8g2, 9, 44, chrono_hour);
    // chrono_minute
    u8g2_DrawStr(u8g2, 27, 44, chrono_minute);
    // chrono_second
    u8g2_DrawStr(u8g2, 61, 44, chrono_second);
    // chrono_millisecond
    u8g2_SetFont(u8g2, u8g2_font_profont22_tr);
    u8g2_DrawStr(u8g2, 95, 44, chrono_millisecond);

    u8g2_SetFont(u8g2, u8g2_font_profont15_tr);
    u8g2_DrawStr(u8g2, 5, 11, "HEAD");

    u8g2_DrawXBM(u8g2, 58, 29, 2, 11, image_chrono_minute_colon_bits);
    // chrono_hour_colon
    u8g2_DrawXBM(u8g2, 24, 29, 2, 11, image_chrono_minute_colon_bits);
    // chrono_second_point
    u8g2_DrawXBM(u8g2, 92, 41, 2, 2, image_chrono_second_point_bits);
    u8g2_DrawLine(u8g2, 1, 22, 126, 22);
    u8g2_DrawLine(u8g2, 1, 47, 126, 47);
    u8g2_DrawLine(u8g2, 1, 22, 1, 47);
    u8g2_DrawLine(u8g2, 126, 22, 126, 47);

}
void draw_confirm_page(u8g2_t *u8g2)
{
    u8g2_SetBitmapMode(u8g2, 1);
    u8g2_SetFontMode(u8g2, 1);

    // YES
    u8g2_SetFont(u8g2, u8g2_font_profont15_tr);
    u8g2_DrawStr(u8g2, 5, 11, "YES");

    // NO
    u8g2_DrawStr(u8g2, 109, 12, "NO");

    // player_name
    u8g2_SetFont(u8g2, u8g2_font_profont17_tr);
    u8g2_DrawStr(u8g2, 27, 48, "JOHN DOE");

    // Message
    u8g2_SetFont(u8g2, u8g2_font_profont15_tr);
    u8g2_DrawStr(u8g2, 8, 28, "Confirm Swimmer:");
}

void oled_init(void)
{
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.bus.spi.clk = PIN_CLK;
    u8g2_esp32_hal.bus.spi.mosi = PIN_MOSI;
    u8g2_esp32_hal.bus.spi.cs = PIN_CS;
    u8g2_esp32_hal.dc = PIN_DC;
    u8g2_esp32_hal.reset = PIN_RESET;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_Setup_ssd1309_128x64_noname0_f(
        &u8g2,
        U8G2_R0,
        u8g2_esp32_spi_byte_cb,
        u8g2_esp32_gpio_and_delay_cb);

    ESP_LOGI(TAG, "u8g2_InitDisplay");
    u8g2_InitDisplay(&u8g2);  // send init sequence to the display, display is in
                            // sleep mode after this,

    ESP_LOGI(TAG, "u8g2_SetPowerSave");
    u8g2_SetPowerSave(&u8g2, 0);  // wake up display
    u8g2_SendF(&u8g2, "cac", 0x0db, 5 << 4, 0xaf); // set contrast
}

void oled_draw_stopwatch(const char *sw_str, const char *lap_str, const char *split_str, const char *lap_count_str)
{
    u8g2_ClearBuffer(&u8g2);
    draw_battery(&u8g2, BATTERY_FULL);
    draw_wfi_status(&u8g2, WiFI_CONNECTED);
    draw_frame(&u8g2);
    draw_stopwatch(&u8g2, sw_str, lap_str, split_str, lap_count_str);
    u8g2_SendBuffer(&u8g2);
}

void oled_draw_swimming_watch(const char *sw_str, const char *name_str, const char *type_str, const char *lane_count_str, const char *watch_number_str, bool is_streaming)
{
    u8g2_ClearBuffer(&u8g2);
    draw_streaming_icon(&u8g2, is_streaming);
    draw_swimming_watch(&u8g2, sw_str, name_str, type_str, lane_count_str, watch_number_str);
    u8g2_SendBuffer(&u8g2);
}

void oled_draw_headtimer_watch(const char *sw_str, bool is_streaming)
{
    u8g2_ClearBuffer(&u8g2);
    draw_streaming_icon(&u8g2, is_streaming);
    draw_headtimer_page(&u8g2, sw_str);
    u8g2_SendBuffer(&u8g2);
}
void oled_draw_confirm_watch(void)
{
    u8g2_ClearBuffer(&u8g2);
    draw_confirm_page(&u8g2);
    u8g2_SendBuffer(&u8g2);
}

