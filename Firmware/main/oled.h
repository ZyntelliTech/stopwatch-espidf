#pragma once
#include "u8g2.h"
#include <stdbool.h>

void oled_init(void);
void oled_draw_stopwatch(const char *sw_str, const char *lap_str, const char *split_str, const char *lap_count_str);
void oled_draw_swimming_watch(const char *sw_str, const char *name_str, const char *type_str, const char *lane_count_str, const char *watch_number_str, bool is_streaming);
void oled_draw_headtimer_watch(const char *sw_str, bool is_streaming);
void oled_draw_confirm_watch(void);

typedef enum {
    BATTERY_EMPTY = 0,
    BATTERY_17,
    BATTERY_33,
    BATTERY_50,
    BATTERY_67,
    BATTERY_83,
    BATTERY_FULL
} battery_level_t;

typedef enum {
    WIFI_NOT_CONNECTED = 0,
    WIFI_1_BAR,
    WIFI_2_BARS,
    WIFI_3_BARS,
    WiFI_CONNECTED
} wifi_signal_t;

