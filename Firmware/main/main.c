#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "rtc_timer.h"
#include "oled.h"
#include "stopwatch.h"
#include "driver/gpio.h"

#define DEBOUNCE_MS  30
typedef struct {
    char name[32];
    char type[32];
    char lane_count[2];
    char watch_number[2];
} player_t;

typedef enum {
    STOPWATCH_PAGE = 0,
    SWIMMING_PAGE
} page_state_t;

static const char *TAG = "main";
static char time_buffer[16] = {0};
uint8_t page_index = STOPWATCH_PAGE;

#define BTN_START_STOP   GPIO_NUM_5
#define BTN_RECALL       GPIO_NUM_6
#define BTN_LAP_RESET    GPIO_NUM_7

static void buttons_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask =
            (1ULL << BTN_START_STOP) |
            (1ULL << BTN_RECALL) |
            (1ULL << BTN_LAP_RESET),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

/* 1-second periodic task */
static void one_second_task(void *arg)
{
    TickType_t last_wake_time = xTaskGetTickCount();

    while (1)
    {
        /* Runs exactly every 1 second */
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(1000));

        rtc_get_time(time_buffer, sizeof(time_buffer));
    }
}

void app_main(void)
{
    uint32_t last_state = 0xFFFFFFFF;
    uint32_t stable_state = last_state;
    bool stopwatch_running = false;

    ESP_LOGI(TAG, "Initializing OLED display");
    oled_init();
    buttons_init();

    rtc_set_time(2026, 1, 6, 15, 0, 0);

    ESP_LOGI(TAG, "All done!");
    
    char buf[16];   // ✅ RAM buffer
    char lap_buf[16];
    char split_buf[16];
    char lap_count_str[4];
    uint8_t lap_count = 0;
    
    memset(buf, '0', sizeof(buf));
    memset(lap_buf, '0', sizeof(lap_buf));
    memset(split_buf, '0', sizeof(split_buf));
    memset(lap_count_str, '0', sizeof(lap_count_str));

    while (1)
    {  
        uint32_t state =
            (gpio_get_level(BTN_START_STOP) << 0) |
            (gpio_get_level(BTN_RECALL)     << 1) |
            (gpio_get_level(BTN_LAP_RESET)  << 2);
        
        if (state != last_state) {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
            last_state = state;
            continue;
        }
        if (state != stable_state) {
            stable_state = state;

            //Active Low buttons
            if (!(state & (1 << 0))) {  // START/STOP
                if(get_stopwatch_state()) {
                    stopwatch_stop();
                } else {
                    stopwatch_start();
                }
            }
            if (!(state & (1 << 1))) {  // LAP / PAGE SWITCH
                if (get_stopwatch_state()) {
                    if (stopwatch_lap()) {
                        lap_entry_t lap;
                        uint8_t idx = stopwatch_get_lap_count() - 1;
                        stopwatch_get_lap(idx, &lap);
                        stopwatch_get_lap_str(idx, lap_buf, sizeof(lap_buf));
                        stopwatch_get_split_str(idx, split_buf, sizeof(split_buf));
                        snprintf(lap_count_str, sizeof(lap_count_str), "%02u", idx + 1);
                    }

                } else {
                    page_index = (page_index == STOPWATCH_PAGE) ? SWIMMING_PAGE : STOPWATCH_PAGE;
                }
            }
            if (!(state & (1 << 2))) {   // RESET
                stopwatch_reset();
                memset(buf, '0', sizeof(buf));
                memset(lap_buf, '0', sizeof(lap_buf));
                memset(split_buf, '0', sizeof(split_buf));
                memset(lap_count_str, '0', sizeof(lap_count_str));
            }
        }

        stopwatch_get_str_10ms(buf, sizeof(buf));

        switch (page_index)
        {
            case STOPWATCH_PAGE:
            oled_draw_stopwatch(buf, lap_buf, split_buf, lap_count_str);
            break;
        case SWIMMING_PAGE:
            player_t player = {
                .name = "JOHN DOE",
                .type = "M 50 FREE",
                .lane_count = "6",
                .watch_number = "3",
            };
            oled_draw_swimming_watch(buf, player.name, player.type, player.lane_count, player.watch_number, true);
            break;
        default:
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
